#include <iostream>

#include "static_loader.h"
namespace qwen_loader {
    void load_qwen_weights(const std::string& filepath, qwen_loader::QwenWeights& weights) {
        std::cout << "loading qwen weights from: " << filepath << std::endl;

        // step1: Memory map the safetensor file
        int fd = open(filepath.c_str(), O_RDONLY);
        if (fd == -1) {
            throw std::runtime_error("Failed to open file: " + filepath);
        }

        struct stat file_stat;

        if (fstat(fd, &file_stat) == -1) {
            close(fd);
            throw std::runtime_error("Failed to get file stats.");
        }
        size_t file_size = file_stat.st_size;

        uint64_t json_header_len;
        if (pread(fd, &json_header_len, 8, 0) != 8) {
            close(fd);
            throw std::runtime_error("Failed to read safetensors header length.");
        }
        const size_t data_start_offset = 8 + json_header_len;

        char* mapped_file = (char*)mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (mapped_file == MAP_FAILED) {
            close(fd);
            throw std::runtime_error("Failed to mmap file.");
        }
        close(fd);

        char* data_ptr = mapped_file + data_start_offset;

        // step2: allocate one giant block on the GPU
        CUDA_CHECK(cudaMalloc(&weights._gpu_mem_block, qwen_loader::TOTAL_WEIGHT_BYTES));
        char* all_weights_gpu = (char*)weights._gpu_mem_block;

        // step3: copy each tensor from mmap'd memory to the GPU
        cudaStream_t stream;
        CUDA_CHECK(cudaStreamCreate(&stream));

        for (int i = 0; i < qwen_loader::NUM_TENSORS; i++) {
            const auto& meta = qwen_loader::TENSOR_METADATA[i];
            void* gpu_target_ptr = all_weights_gpu + meta.offset;
            char* host_source_ptr = data_ptr + meta.offset;
            CUDA_CHECK(cudaMemcpyAsync(gpu_target_ptr, host_source_ptr, meta.size_bytes, cudaMemcpyHostToDevice, stream));
        }
        CUDA_CHECK(cudaStreamSynchronize(stream));
        CUDA_CHECK(cudaStreamDestroy(stream));

        munmap(mapped_file, file_size);

        // step 4: assign the pointers in our C++ struct
        weights.output_head_weight      =   (qwen_loader::bf16*)(all_weights_gpu + qwen_loader::TENSOR_METADATA[0].offset);
        weights.token_embedding_table   =   (qwen_loader::bf16*)(all_weights_gpu + qwen_loader::TENSOR_METADATA[1].offset);
        weights.final_norm_weight       =   (qwen_loader::bf16*)(all_weights_gpu + qwen_loader::TENSOR_METADATA[310].offset);

        // safetensors file does not store layers sequentially
        const int layer_map[] = {0, 1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 2, 20, 21, 22, 23, 24, 25, 26, 27, 3, 4, 5, 6, 7, 8, 9};
        const int tensors_per_layer =  11;

        // layer weights start at index 2 in the TENSOR_METADATA array
        int current_tensor_idx_offset = 2;

        for (int i = 0; i < N_LAYERS; i++) {
            int layer_idx = layer_map[i];
            int start_idx = current_tensor_idx_offset + i * tensors_per_layer;

            qwen_loader::TransformerBlockWeights& layer = weights.layers[layer_idx];
            layer.input_layernorm_weight          = (qwen_loader::bf16*)(all_weights_gpu + qwen_loader::TENSOR_METADATA[start_idx + 0].offset);
            layer.ffn.down_proj_weight            = (qwen_loader::bf16*)(all_weights_gpu + qwen_loader::TENSOR_METADATA[start_idx + 1].offset);
            layer.ffn.gate_proj_weight            = (qwen_loader::bf16*)(all_weights_gpu + qwen_loader::TENSOR_METADATA[start_idx + 2].offset);
            layer.ffn.up_proj_weight              = (qwen_loader::bf16*)(all_weights_gpu + qwen_loader::TENSOR_METADATA[start_idx + 3].offset);
            layer.post_attention_layernorm_weight = (qwen_loader::bf16*)(all_weights_gpu + qwen_loader::TENSOR_METADATA[start_idx + 4].offset);
            layer.attention.k_norm_weight         = (qwen_loader::bf16*)(all_weights_gpu + qwen_loader::TENSOR_METADATA[start_idx + 5].offset);
            layer.attention.k_proj_weight         = (qwen_loader::bf16*)(all_weights_gpu + qwen_loader::TENSOR_METADATA[start_idx + 6].offset);
            layer.attention.o_proj_weight         = (qwen_loader::bf16*)(all_weights_gpu + qwen_loader::TENSOR_METADATA[start_idx + 7].offset);
            layer.attention.q_norm_weight         = (qwen_loader::bf16*)(all_weights_gpu + qwen_loader::TENSOR_METADATA[start_idx + 8].offset);
            layer.attention.q_proj_weight         = (qwen_loader::bf16*)(all_weights_gpu + qwen_loader::TENSOR_METADATA[start_idx + 9].offset);
            layer.attention.v_proj_weight         = (qwen_loader::bf16*)(all_weights_gpu + qwen_loader::TENSOR_METADATA[start_idx + 10].offset);
        }
    }
}

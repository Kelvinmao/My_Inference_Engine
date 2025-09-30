#include <fstream>
#include <memory>
#include <catch2/catch_test_macros.hpp>
#include "../tokenizer.h"

TEST_CASE("load_single_template loads file test", "[tokenizer]") {
    // load_single_template(char* buffer, size_t buffer_size, const char* dir_path, const char* filename);
    const char* test_dir = ".";
    const char* test_file = "test_template.txt";

    {
        std::ofstream ofs(test_file);
        REQUIRE(ofs.is_open());
        ofs << "This is the char template";
    }

    char buffer[256];
    load_single_template(buffer, sizeof(buffer), test_dir, test_file);

    REQUIRE(std::string(buffer) == "This is the char template");

    std::remove(test_file);
}

TEST_CASE("build_tokenizer constrct a tokenizer test", "[tokenizer]") {
    // build_tokenizer(Tokenizer* t, const char* dir_path, int enable_thinking)
    const char* dir_path = "/workspace/Qwen-600-KM/Qwen3-0.6B";
    auto t =  std::make_unique<Tokenizer>();
    
    SECTION ("Test tokenizer construction") {
        int enable_thinking = 0;
        build_tokenizer(t.get(), dir_path, enable_thinking);
        REQUIRE(t->vocab_size == VOCAB_SIZE);
        REQUIRE(t->vocab != nullptr);
        REQUIRE(t->merge_scores != nullptr);
        REQUIRE(t->max_token_length > 0);
        REQUIRE(t->bos_token_id >= 0);
        REQUIRE(t->eos_token_id >= 0);
        REQUIRE(t->vocab[0] != nullptr);
    }

    SECTION("File not found should exit") {
        auto broken = std::make_unique<Tokenizer>();
        REQUIRE_THROWS_AS(build_tokenizer(broken.get(), "invalid/path", 0), std::runtime_error);
    }
}

TEST_CASE("encode tokenizer encode test", "[tokenizer]") {
    const char* dir_path = "/workspace/Qwen-600-KM/Qwen3-0.6B";
    auto t = std::make_unique<Tokenizer>();

    SECTION ("Test tokenizer encode a string") {
        int enable_thinking = 0;
        build_tokenizer(t.get(), dir_path, enable_thinking);
        char* text = "Test Test the the tokenizer tokenizer tokenizer encode encode encode encode method method method method method";
        int tokens[256];
        int n_tokens = 0;
        memset(tokens, 0, sizeof(tokens));
        encode(t.get(), text, tokens, &n_tokens);
        REQUIRE(n_tokens == 16);
    }

    SECTION ("Test tokenizer encode a empty string") {
        int enable_thinking = 0;
        build_tokenizer(t.get(), dir_path, enable_thinking);
        char* text = "";
        int tokens[256];
        int n_tokens = 0;
        memset(tokens, 0, sizeof(tokens));
        encode(t.get(), text, tokens, &n_tokens);
        REQUIRE(n_tokens == 0);
    }

    SECTION ("Test tokenizer encode a string with special tokens") {
        int enable_thinking = 0;
        build_tokenizer(t.get(), dir_path, enable_thinking);
        char* text = "<|im_start|> Hello world <|im_end|>";
        int tokens[256];
        int n_tokens = 0;
        memset(tokens, 0, sizeof(tokens));
        encode(t.get(), text, tokens, &n_tokens);
        // for (int i = 0; i < n_tokens; i++) {
        //     printf("token[%d]: %s\n", i, t->vocab[tokens[i]]);
        // }
        REQUIRE(!strcmp(t->vocab[tokens[0]], "<|im_start|>"));
        REQUIRE(!strcmp(t->vocab[tokens[1]], " Hello"));
        REQUIRE(!strcmp(t->vocab[tokens[2]], " world"));
        REQUIRE(!strcmp(t->vocab[tokens[3]], " "));
        REQUIRE(!strcmp(t->vocab[tokens[4]], "<|im_end|>"));
    }
}
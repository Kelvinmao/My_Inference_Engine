#include "tokenizer.h"

void load_single_template(char* buffer, size_t buffer_size, const char* dir_path, const char* filename) {
    char full_path[1024];
    construct_path(full_path, sizeof(full_path), dir_path, filename);

    memset(buffer, 0, buffer_size);
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Couldn't load template file %s\n", full_path);
        exit(EXIT_FAILURE);
    }
    // Read up to buffer_size - 1 to ensure null termination
    fread(buffer, 1, buffer_size - 1, file);
    fclose(file);
}

void build_tokenizer(Tokenizer* t, const char* dir_path, int enable_thinking) {
    char tokenizer_path[1024];
    construct_path(tokenizer_path, sizeof(tokenizer_path), dir_path, "tokenizer.bin");

    t->vocab_size = VOCAB_SIZE;
    t->vocab = (char**)malloc(t->vocab_size * sizeof(char*));
    t->merge_scores = (float*)malloc(t->vocab_size * sizeof(float));
}

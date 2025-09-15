#include "tokenizer.h"

void load_single_template(char* buffer, size_t buffer_size, const char* dir_path, const char* filename) {
    char full_path[1024];
    construct_path(full_path, sizeof(full_path), dir_path, filename);

    memset(buffer, 0, buffer_size);
    FILE* file = fopen(full_path, "rb");
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

    FILE* file = fopen(tokenizer_path, "rb");
    if (!file) {
        fprintf(stderr, "Couldn't load tokenizer at: %s\n", tokenizer_path);
        exit(EXIT_FAILURE);
    }
    fread(&t->max_token_length, sizeof(int), 1, file);
    fread(&t->bos_token_id, sizeof(int), 1, file);
    fread(&t->eos_token_id, sizeof(int), 1, file);

    int len;
    // Read the merge score from tokenizer for each token_i
    for (int i = 0; i < t->vocab_size; i++) {
        // token_i has NO merge_scores
        if (fread(t->merge_scores + i, sizeof(float), 1, file) != 1) {
            // A special token, allocate one char ('\0')
            t->vocab[i] = (char*)malloc(1);
            t->vocab[i][0] = 0;
        }
        // token_i has merge_scores
        else {
            // Read the len of the token
            fread(&len, sizeof(int), 1, file);
            // Allocate len+1 spaces
            t->vocab[i] = (char*)malloc((len+1));
            // Read 'len' chars
            fread(t->vocab[i], 1, len, file);
            // Add the trailing '\0'
            t->vocab[i][len] = 0;
        }
    }
    fclose(file);

    if (enable_thinking) {
        // load thinking version of the templates
        load_single_template(t->prompt_template, sizeof(t->prompt_template), dir_path, "template_user_thinking.txt");
        load_single_template(t->system_prompt_template, sizeof(t->system_prompt_template), dir_path, "template_system_thinking.txt");
    } else {
        // load non-thinking version of the template
        load_single_template(t->prompt_template, sizeof(t->prompt_template), dir_path, "template_user.txt");
        load_single_template(t->system_prompt_template, sizeof(t->system_prompt_template), dir_path, "template_system.txt");
    }
}

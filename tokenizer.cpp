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
        char buf[2048];
        fprintf(stderr, "Couldn't load tokenizer at: %s\n", tokenizer_path);
        // exit(EXIT_FAILURE);
        throw std::runtime_error(buf);
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

void free_tokenizer(Tokenizer* t) {
    for (int i = 0; i < t->vocab_size; i++) { free(t->vocab[i]); }
    free(t->vocab);
    free(t->merge_scores);
}

char* decode(Tokenizer* t, int token) {
    return t->vocab[token];
}

int str_lookup(char* str, char** vocab, int vocab_size) {
    // find a match for str in vocab, return its index or -1 if not found
    for (int i = 0; i < vocab_size; i++) {
        if (!strcmp(str, vocab[i]))
            return i;
    }
    // unknown token, return -1
    return -1;
}

void encode(Tokenizer* t, char* text, int* tokens, int* n_tokens) {
    // encode the string text (input) into an upper-bound preallocated tokens[] array

    // create a temporary buffer that will store merge candidates of two consecutive tokens
    // *2 for concat, +1 for null terminator, +2 for UTF-8 (in case max_token_length is 1)

    char* str_buffer = (char*)malloc((t->max_token_length * 2 + 1 + 2) * sizeof(char));
    char special_token[64 + 1];

    // start at 0 tokens
    *n_tokens = 0;

    // process the raw (UTF-8) byte sequence of the input string
    for (char* c = text; *c != 0; c++) {
        int id, found_special_token = 0;
        
        // set the buffer to the current byte
        str_buffer[0] = *c;
        str_buffer[1] = 0;

        // special token begin with < and end with >. If we find a substring beginning with <
        // and end with > and there is a token in the vocab for it, use that instead of parsing into
        // shorter token
        if (*c == '<') {
            int end_of_token_pos = -1;
            found_special_token = 0;

            for (int k = 0; *c != 0 && k < 64; k++) {
                if (c[k] == '>') {
                    end_of_token_pos = k;
                    break;
                }
            }

            // found '>'
            if (end_of_token_pos != -1) {
                strncpy(special_token, c, end_of_token_pos + 1);
                special_token[end_of_token_pos + 1] = 0;

                id = str_lookup(special_token, t->vocab, t->vocab_size);
                // printf("Trying special token: %s, id=%d\n", special_token, id);
                if (id != -1) {
                    c += end_of_token_pos;
                    found_special_token = 1;
                }
            }
        }

        // not a special token, just look up the single character
        if (!found_special_token)
            id = str_lookup(str_buffer, t->vocab, t->vocab_size);
        
        // found the token
        if (id != -1) {
            tokens[(*n_tokens)++] = id;
        } else {
            printf("Warning: unknown character code point %d in input, skipping.\n", *str_buffer);
            (*n_tokens)++;
        }
    }

    // merge the best consecutive pair each iteration
    while (1) {
        float best_score = -1e10;
        int best_id = -1;
        int best_idx = -1;

        for (int i = 0; i < (*n_tokens) - 1; i++) {
            sprintf(str_buffer, "%s%s", t->vocab[tokens[i]], t->vocab[tokens[i+1]]);
            int id = str_lookup(str_buffer, t->vocab, t->vocab_size);

            if (id != -1 && t->merge_scores[id] > best_score) {
                // found the merge pair in the vocab, record its score and position
                best_score = t->merge_scores[id];
                best_id = id;
                best_idx = i;
            }
        }

        if (best_idx == -1)
            break; // No further merge could be done

        // merge the consecutive pair (best_idx, best_idx + 1) into new token best_id
        tokens[best_idx] = best_id;

        for (int i = best_idx + 1; i < (*n_tokens) - 1; i++) {
            tokens[i] = tokens[i+1];
        }
        (*n_tokens)--;
        // printf("Find token: %s\n", t->vocab[best_id]);
    }

    free(str_buffer);
}

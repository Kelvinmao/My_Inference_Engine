// tokenizer.h

#pragma once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "config.h"

typedef struct {
    char** vocab;
    float* merge_scores;
    int vocab_size;
    unsigned int max_token_length;
    unsigned int bos_token_id;
    unsigned int eos_token_id;
    char prompt_template[1024];
    char system_prompt_template[1024];
} Tokenizer;

void 
load_single_template(char* buffer, size_t buffer_size, const char* dir_path, const char* filename);

void 
build_tokenizer(Tokenizer* t, const char* dir_path, int enable_thinking);

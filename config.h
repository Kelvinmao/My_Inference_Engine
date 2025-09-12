// config.h

#pragma once

//========================================
// ANSI color codes
//========================================

#define COLOR_RESET "\x1b[0m"
#define COLOR_BOLD_RED "\x1b[1;31m"
#define COLOR_GREEN    "\x1b[32m"
#define COLOR_YELLOW   "\x1b[33m"
#define COLOR_ORANGE   "\x1b[33m"
#define COLOR_CYAN     "\033[36m"

//========================================
// Configs
//========================================
#define MAX_LINE_WIDTH 80

constexpr int SEQ_LEN = 8192;
constexpr int PROMPT_BUFFER_SIZE = 32768;
constexpr int VOCAB_SIZE = 151936;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>

#include "config.h"

// ========================================
// utils
// ========================================

void
print_banner();

long
time_in_ms();

void construct_path(
    char* out_path,
    size_t out_size,
    const char* dir,
    const char* filename
);

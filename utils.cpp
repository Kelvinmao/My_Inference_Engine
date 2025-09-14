#include "utils.h"

void
print_banner() {
    printf("\n" COLOR_ORANGE R"(
   ____ _       _________   _______ ____  ____  __ __ __  ___
  / __ \ |     / / ____/ | / / ___// __ \/ __ \/ //_//  |/  /
 / / / / | /| / / __/ /  |/ / __ \/ / / / / / / ,<  / /|_/ / 
/ /_/ /| |/ |/ / /___/ /|  / /_/ / /_/ / /_/ / /| |/ /  / /  
\___\_\|__/|__/_____/_/ |_/\____/\____/\____/_/ |_/_/  /_/   
                                                             
                                               
    )" COLOR_RESET);
}

long
time_in_ms() {
    // return time in milliseconds, for benchmarking the model speed
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    return time.tv_sec * 1000 + time.tv_nsec / 1000000;
}

void construct_path(
    char* out_path,
    size_t out_size,
    const char* dir,
    const char* filename
) {
    size_t len = strlen(dir);
    if (len > 0 && dir[len-1] == '/') {
        // there is already a slash at the end
        snprintf(out_path, out_size, "%s%s", dir, filename);
    } else {
        // no slash at the end, add one
        snprintf(out_path, out_size, "%s/%s", dir, filename);
    }
}

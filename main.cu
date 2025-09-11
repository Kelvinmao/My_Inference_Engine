// main.cu

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
print_banner() {
    printf("\n" COLOR_ORANGE R"(
   ____ _       _________   _______ ____  ____  __ __ __  ___
  / __ \ |     / / ____/ | / / ___// __ \/ __ \/ //_//  |/  /
 / / / / | /| / / __/ /  |/ / __ \/ / / / / / / ,<  / /|_/ / 
/ /_/ /| |/ |/ / /___/ /|  / /_/ / /_/ / /_/ / /| |/ /  / /  
\___\_\|__/|__/_____/_/ |_/\____/\____/\____/_/ |_/_/  /_/   
                                                             
                                               
    )" COLOR_RESET);
}

struct Args {
    std::string model_dir;
    float temperature = 0.6f;
    float top_p = 0.95f;
    int top_k = 20;
    std::string prompt;
    std::string system_prompt;
    unsigned long long rng_seed = 0;
    int enable_thinking = 0;
};

// main
void error_usage() {
    std::cerr << "\nusage: ./myengine <model_dir> [options]\n";
    std::cerr << "example: ./myengine <model_dir> -r 1\n";
    std::cerr << "arguments:\n";
    std::cerr << "  -r <int>    reasoning mode, 0 = no thinking, 1 = thinking\n";
    std::cerr << "  -s <int>    random seed\n";
    std::cerr << "  -k <int>    top-k sampling\n";
    std::cerr << "  -t <float>  temperature\n";
    std::cerr << "  -p <float>  top-p sampling\n";
    std::cerr << "  -i <string> input_prompt\n";
    std::cerr << "  -y <string> system_prompt\n";
    exit(EXIT_FAILURE);
}

Args parse_args(int argc, char* argv[]) {
    Args args;
    if (argc < 2) {
        error_usage();
    }
    args.model_dir = argv[1];

    for (int i = 2; i < argc; i++) {
        if (i + 1 >= argc) // no value after the last flag
            error_usage();
        std::string flag = argv[i];
        std::string value = argv[i+1];

        if (flag == "-t")   args.temperature = std::atof(value.c_str());
        else if (flag == "-p")  args.top_p = std::atof(value.c_str());
        else if (flag == "-k")  args.top_k = std::atoi(value.c_str());
        else if (flag == "-s")  args.rng_seed = std::atoll(value.c_str());
        else if (flag == "-i")  args.prompt = value;
        else if (flag == "-y")  args.system_prompt = value;
        else if (flag == "-r")  args.enable_thinking = std::atoi(value.c_str());
        else error_usage();
    }
    return args;
}

int main(int argc, char* argv[]) {
    print_banner();
    Args args = parse_args(argc, argv);

    std::cout << "\n[Config]\n";
    std::cout << "  model_dir   = " << args.model_dir << std::endl;
    std::cout << "  temperature   = " << args.temperature << std::endl;
    std::cout << "  top_p   = " << args.top_p << std::endl;
    std::cout << "  top_k   = " << args.top_k << std::endl;
    std::cout << "  rng_seed   = " << args.rng_seed << std::endl;
    std::cout << "  prompt   = " << args.prompt << std::endl;
    std::cout << "  system_prompt   = " << args.system_prompt << std::endl;
    std::cout << "  enable_thinking   = " << args.enable_thinking << std::endl;

    return 0;
}

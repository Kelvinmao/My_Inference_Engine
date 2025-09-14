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
    const char* dir_path = "/path/to/model";
    auto t =  std::make_unique<Tokenizer>();
    
    SECTION ("Test tokenizer construction") {
        int enable_thinking = 0;
        build_tokenizer(t.get(), dir_path, enable_thinking);
        REQUIRE(t->vocab_size == VOCAB_SIZE);
    }
}
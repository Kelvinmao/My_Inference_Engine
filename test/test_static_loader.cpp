#include <memory>
#include <catch2/catch_test_macros.hpp>

#include "../static_loader.h"

TEST_CASE("load_qwen_weights test", "[static_loader]") {
    SECTION("Test safetensor file not exist") {
        const char* model_path = "/path/to/file";
        auto weight = std::make_unique<qwen_loader::QwenWeights>();
        REQUIRE_THROWS_AS(qwen_loader::load_qwen_weights(model_path, *weight), std::runtime_error);
    }

    
}
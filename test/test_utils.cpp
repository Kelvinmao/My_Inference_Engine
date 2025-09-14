#include <catch2/catch_test_macros.hpp>
#include "../utils.h"

TEST_CASE("Construct model path", "[model_path]") {
    // void construct_path(
    //     char* out_path,
    //     size_t out_size,
    //     const char* dir,
    //     const char* filename
    // );
    char out_path[1024];
    int out_size = 1024;


    SECTION( "Concatenate filename WITHOUT slash at the end of dir" ) {
        const char dir[1024] = "/path/to/model";
        const char filename[1024] = "model.tensor";
        construct_path(out_path, out_size, dir, filename);
        char expected[1024] = "/path/to/model/model.tensor";
        bool result = (strcmp(out_path, expected) == 0);
        REQUIRE(result == true);
    }

    SECTION( "Concatenate filename WITH slash at the end of dir " ) {
        const char dir[1024] = "/path/to/model/";
        const char filename[1024] = "model.tensor";
        construct_path(out_path, out_size, dir, filename);
        char expected[1024] = "/path/to/model/model.tensor";
        printf("%s", out_path);
        bool result = (strcmp(out_path, expected) == 0);
        REQUIRE(result == true);
    }
}

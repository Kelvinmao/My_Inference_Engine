cmake_minimum_required(VERSION 3.20)
project(QWEN3_600MKM LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===========================
include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.2
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CATCH_CONFIG_CPP17_OR_GREATER ON)
FetchContent_MakeAvailable(Catch2)
# ===========================

message(STATUS "====================")
message(STATUS "Project: ${PROJECT_NAME}")

# ============================
# Main program
# ============================
add_executable(
    qwen600
    main.cu
    utils.cpp
    tokenizer.cpp
)

target_compile_options(qwen600 PRIVATE
    $<$<COMPILE_LANGUAGE:CUDA>:--expt-relaxed-constexpr>
)

set_target_properties(qwen600 PROPERTIES COMPILE_FLAGS "-O3")

# ============================
# Test program
# ============================
add_executable(
    tests
    test/test_tokenizer.cpp
    test/test_utils.cpp
)

target_include_directories(
    tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/test
)

set_target_properties(tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

set_target_properties(Catch2 PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)
set_target_properties(Catch2WithMain PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
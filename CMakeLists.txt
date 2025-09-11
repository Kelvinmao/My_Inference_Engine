cmake_minimum_required(VERSION 3.20)
project(QWEN3_600MKM LANGUAGES CXX CUDA)

message(STATUS "====================")
message(STATUS "Project: ${PROJECT_NAME}")

add_executable(
    qwen600
    main.cu
)

target_compile_options(qwen600 PRIVATE
    $<$<COMPILE_LANGUAGE:CUDA>:--expt-relaxed-constexpr>
)

set_target_properties(qwen600 PROPERTIES COMPILE_FLAGS "-O3")

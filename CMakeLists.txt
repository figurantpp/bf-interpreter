cmake_minimum_required(VERSION 3.17)
project(bf C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "-Og")

add_library(
        external
        external/streams.c
        external/streams.h
)

add_executable(bf main.c bf/bf.c bf/bf.h big_input.h bf/bf_tests.c bf/bf_tests.h bf/bf_benchmark.c bf/bf_benchmark.h)

target_link_libraries(bf external)
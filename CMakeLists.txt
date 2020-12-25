cmake_minimum_required(VERSION 3.17)
project(bf C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "-O3")

add_library(
        external
        external/streams.c
        external/streams.h
)

add_executable(bf main.c bf/bf.c bf/bf.h big_input.h testing/bf_tests.c testing/bf_tests.h testing/bf_benchmark.c testing/bf_benchmark.h
        testing/testing.c testing/testing.h)

target_link_libraries(bf external)

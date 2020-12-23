cmake_minimum_required(VERSION 3.17)
project(bf)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_C_FLAGS "-O3")

add_library(
        external
        external/streams.c
        external/streams.h
)

add_library(cmap
        cmap/cmap.cpp
        cmap/cmap.h
)

add_executable(bf main.c bf/bf.c bf/bf.h big_input.h bf/bf_tests.c bf/bf_tests.h bf/bf_benchmark.c bf/bf_benchmark.h
        cmap/cmap.h cmap/cmap.cpp)

target_link_libraries(bf external)

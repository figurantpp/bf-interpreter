//
// Created by figurantpp on 22/12/2020.
//

#include "bf_benchmark.h"

struct BFBenchmarkUnit bf_global_benchmark_units[] = {
        {
                .name = "Mandelbrot test",
                .source_file_name = "mandel.bf",
                .expected_output_file_name = "mandel.output"
        },
        {}
};

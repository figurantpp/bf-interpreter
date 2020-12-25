//
// Created by figurantpp on 22/12/2020.
//

#ifndef BF_BF_BENCHMARK_H
#define BF_BF_BENCHMARK_H

struct BFBenchmarkUnit
{
    const char *name;
    const char *source_file_name;
    const char *expected_output_file_name;
    const char *input_file_name;
};

extern struct BFBenchmarkUnit bf_global_benchmark_units[];

#endif //BF_BF_BENCHMARK_H

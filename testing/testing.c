//
// Created by figurantpp on 25/12/2020.
//

#include "testing.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#include "bf_tests.h"
#include "bf_benchmark.h"
#include "../bf/bf.h"
#include "../external/streams.h"


#define RED_COLOR "\033[0;31m"
#define RESET_COLOR "\033[0;0m"
#define GREEN_COLOR "\033[0;32m"

#define BENCHMARK_DIRECTORY "/home/figurantpp/Desktop/programming/c/bf-i/test_input"

void bf_test()
{
    char *output = NULL;
    size_t output_size = 0;

    FILE *output_file = open_memstream(&output, &output_size);


    if (output_file == NULL)
    {
        perror("Failed to open test memstream");
        exit(1);
    }

    struct BFTest* test = global_bf_tests;

    unsigned long test_index = 0;

    FILE *input_file = NULL;

    while (test->source_code || test->expected_output)
    {
        if (test->input != NULL)
        {
            input_file = fmemopen((char*) test->input, strlen(test->input), "r");

            if (input_file == NULL)
            {
                fprintf(stderr, "Failed to set input file for test #%lu", test_index + 1);
                abort();
            }
        }

        rewind(output_file);

        memset(output, 0, output_size);

        bf_execute(test->source_code, 0, input_file ?: stdin, output_file);

        if (strcmp(test->expected_output, output) != 0)
        {
            printf(RED_COLOR);

            printf("Test #%lu (%s) failed.\n", test_index, test->test_name);
            printf("Expected Output: %.40s\n", test->expected_output);
            printf("Output: %.40s\n", output);

            printf(RESET_COLOR);

            assert(strcmp(test->expected_output, output) == 0);
        }

        if (input_file != NULL)
        {
            fclose(input_file);
        }

        input_file = NULL;
        printf("Test #%lu (%s) passed\n", test_index + 1, test->test_name);
        fflush(stdout);
        fflush(stderr);

        test++;
        test_index++;


    }

    fclose(output_file);

    free(output);

    printf(GREEN_COLOR "All %lu Tests Passed" RESET_COLOR, test_index);
}

__attribute__((returns_nonnull))
FILE *get_file(const char *path)
{
    FILE *file = fopen(path, "r");

    if (!file)
    {
        fprintf(stderr, "Failed to open file '%s' : %s\n", path, strerror(errno));
        abort();
    }

    return file;
}

void bf_benchmark()
{
    struct BFBenchmarkUnit* unit = bf_global_benchmark_units;

    unsigned long unit_number = 1;

    FILE *input_file;
    FILE *source_file;

    if (chdir(BENCHMARK_DIRECTORY) != 0)
    {
        perror("Failed to load benchmarking directory (" BENCHMARK_DIRECTORY ")");
        abort();
    }

    while (unit->source_file_name)
    {
        printf("Benchmarking #%lu\n", unit_number);

        source_file = get_file(unit->source_file_name);

        char *source_code = read_whole_file(source_file, NULL);

        if (!source_code)
        {
            fprintf(stderr, "Failed to load input file %s: %s", unit->input_file_name, strerror(errno));
            abort();
        }

        fclose(source_file);

        input_file = unit->input_file_name ? get_file(unit->input_file_name) : stdin;

        clock_t clocks_before = clock();

        bf_execute(source_code, 0, input_file, stdout);

        clock_t clocks_after = clock();

        printf("Benchmark #%lu result: \n", unit_number);
        printf("Clocks: %lu\n", clocks_after - clocks_before);
        printf("Approximate Seconds: %lu\n\n", (clocks_after - clocks_before) / CLOCKS_PER_SEC);

        unit_number++;
        unit++;
    }
}


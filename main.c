
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "bf/bf.h"
#include "bf/bf_tests.h"

#define RED_COLOR "\033[0;31m"
#define RESET_COLOR "\033[0;0m"
#define GREEN_COLOR "\033[0;32m"

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

        bf_execute(test->source_code, input_file ?: stdin, output_file);

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

        test++;
        test_index++;


    }

    fclose(output_file);

    free(output);

    printf(GREEN_COLOR "All %lu Tests Passed" RESET_COLOR, test_index);
}

int main()
{
    bf_test();
    return 0;
}

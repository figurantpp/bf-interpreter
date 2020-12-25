
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "bf/bf.h"
#include "external/streams.h"

#define BF_EXIT_BAD_USAGE 2
#define BF_EXIT_IO 3

int main(int argc, char * const * argv)
{

#define show_usage() ({fprintf(stderr, "Usage: %s [-i input_file] [-o output_file] source_code\n", argv[0]);})

    if (argc == 0)
    {
        puts("Missing argv");
        exit(BF_EXIT_BAD_USAGE);
    }

    int option;

    const char *input_file_name = NULL;
    const char *output_file_name = NULL;
    const char *source_code_file_name = NULL;

    while ((option = getopt(argc, argv, "i:o:")) != -1)
    {
        switch (option)
        {
            case 'i':
                input_file_name = optarg;
                break;
            case 'o':
                output_file_name = optarg;
                break;
            default:
                show_usage();
                exit(BF_EXIT_BAD_USAGE);
        }
    }

    if (argc == 1)
    {
        fputs("Missing bf file to execute\n", stderr);
        show_usage();
        exit(BF_EXIT_BAD_USAGE);
    }

    if (optind >= argc)
    {
        fprintf(stderr, "Missing argument after command line options\n");
        show_usage();
        exit(BF_EXIT_BAD_USAGE);
    }

    source_code_file_name = argv[optind];


    FILE *source_code_file = fopen(source_code_file_name, "r");

    if (source_code_file == NULL)
    {
        perror("Failed to open source code file");
        exit(BF_EXIT_IO);
    }

    size_t source_code_length = 0;

    char *source_code = read_whole_file(source_code_file, &source_code_length);

    if (source_code == NULL)
    {
        perror("Failed to read source code");
        exit(BF_EXIT_IO);
    }

    fclose(source_code_file);

    FILE *input_file = stdin;
    FILE *output_file = stdout;

    if (input_file_name != NULL)
    {
        input_file = fopen(input_file_name, "r");

        if (input_file == NULL)
        {
            int error = errno;

            fprintf(stderr,
                    "%s: Failed to open provided input file %s: %s\n",
                    argv[0], input_file_name, strerror(error));

            exit(BF_EXIT_IO);
        }
    }

    if (output_file_name != NULL)
    {
        output_file = fopen(output_file_name, "w");

        if (output_file == NULL)
        {
            int error = errno;

            fprintf(stderr,
                    "%s: Failed to open provided output file %s: %s\n",
                    argv[0], output_file_name, strerror(error));

            exit(BF_EXIT_IO);
        }
    }


    bf_execute(source_code, source_code_length, input_file, output_file);

    free(source_code);

    if (input_file_name != NULL)
    {
        fclose(input_file);
    }

   if (output_file_name != NULL)
    {
        fclose(output_file);
    }


   return 0;
}


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "bf/bf.h"
#include "external/streams.h"

#define BF_EXIT_BAD_USAGE 2
#define BF_EXIT_IO 3

#define VERSION_NUMBER "0.0.1"

const char *program_name;

void write_usage(FILE *file)
{
    fprintf(file, "Usage: %s [-h] [-v] [-i input_file] [-o output_file] source_code\n", program_name);
}

#define show_usage() ({write_usage(stderr);})

void show_version()
{
    printf("%s (BFI) " VERSION_NUMBER "\n" "figurant++\n", program_name);
}

void show_help()
{
    write_usage(stdout);

    puts("\n"
         "  OPTIONS\n"
         "      -h\n"
         "          Display this help message\n"
         "      -v\n"
         "          Display program version\n"
         "      -i input_file\n"
         "          File used for read (',' bf instruction) calls.\n"
         "          The program's stdin will be used if this option is not provided.\n"
         "\n"
         "      -o output_file\n"
         "          File used to write ('.' bf instruction) calls.\n"
         "          Defaults to the program's stdout if not provided.\n"

         );
}

int main(int argc, char * const * argv)
{
    if (argc == 0)
    {
        fputs("No arv[0] provided.\n", stderr);
        show_usage();
        exit(BF_EXIT_BAD_USAGE);
    }

    program_name = argv[0];

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
            case 'v':
                show_version();
                exit(EXIT_SUCCESS);
            case 'h':
                show_help();
                exit(EXIT_SUCCESS);
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

    source_code_file_name = argv[optind];

    if (source_code_file_name == NULL)
    {
        fprintf(stderr, "Missing source code file.\n");
        show_usage();
        exit(BF_EXIT_BAD_USAGE);
    }



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

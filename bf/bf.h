//
// Created by figurantpp on 22/12/2020.
//

#ifndef BF_BF_H
#define BF_BF_H

#include <stdio.h>


struct BFState
{
    const char *source_code_start;
    const char *source_cursor;
    char *data_cursor;
    char *data_buffer_start;
    FILE *output_file;
    FILE *input_file;
    size_t data_buffer_limit;
    size_t source_code_length;
};

void bf_execute(const char *source_code, FILE *input, FILE *output);
void bf_execute_string(const char *source_code);
void bf_execute_state(struct BFState *bf_state);


#define BF_DEFAULT_BUFFER_LIMIT 4196lu

#endif //BF_BF_H

//
// Created by figurantpp on 22/12/2020.
//

#ifndef BF_BF_H
#define BF_BF_H

#include <stdio.h>




void bf_execute(const char *source_code, size_t source_code_length, FILE *input_file, FILE *output_file);


#define BF_DEFAULT_BUFFER_LIMIT 4196lu

#endif //BF_BF_H

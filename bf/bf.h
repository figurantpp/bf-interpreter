//
// Created by figurantpp on 22/12/2020.
//

#ifndef BF_BF_H
#define BF_BF_H

#include <stdio.h>




void bf_execute(const char *source_code, FILE *input, FILE *output);
void bf_execute_string(const char *source_code);


#define BF_DEFAULT_BUFFER_LIMIT 4196lu

#endif //BF_BF_H

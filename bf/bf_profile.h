//
// Created by figurantpp on 22/12/2020.
//

#ifndef BF_BF_PROFILE_H
#define BF_BF_PROFILE_H

struct BFProfileUnit
{
    const char *name;
    const char *source_file_name;
    const char *expected_output_file_name;
    const char *input_file_name;
};

extern struct BFProfileUnit bf_global_profile_unit[];

#endif //BF_BF_PROFILE_H

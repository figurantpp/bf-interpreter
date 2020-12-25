//
// Created by figurantpp on 22/12/2020.
//

#ifndef BF_BF_TESTS_H
#define BF_BF_TESTS_H

struct BFTest
{
    const char *test_name;
    const char *source_code;
    const char *expected_output;
    const char *input;
};

extern struct BFTest global_bf_tests[];

#endif //BF_BF_TESTS_H

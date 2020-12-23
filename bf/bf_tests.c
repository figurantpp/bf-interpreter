//
// Created by figurantpp on 22/12/2020.
//

#include "bf_tests.h"
#include "../big_input.h"

struct BFTest global_bf_tests[] = {
        {
                .test_name = "BF Hello World",
                .source_code = HELLO_WORLD_BF_STRING,
                .expected_output = HELLO_WORLD_BF_STRING_OUTPUT
        },
        {
                .test_name = "BF Email String",
                .source_code = EMAIL_BF_STRING,
                .expected_output = EMAIL_BF_STRING_OUTPUT
        },
        {
                .test_name = "BF Code that reads input after the end",
                .source_code = ",.,.,.",
                .expected_output = "abc",
                .input = "abc"
        },
        {
                .test_name = "BF Code that outputs itself",
                .source_code = SELF_OUTPUT_BF_STRING,
                .expected_output = SELF_OUTPUT_BF_STRING
        },
        {
                .test_name = "BF Code that compiles BF to C",
                .source_code = COMPILER_BF_STRING,
                .expected_output = COMPILER_BF_STRING_OUTPUT,
                .input = COMPILER_BF_STRING_INPUT
        },

        {}
};

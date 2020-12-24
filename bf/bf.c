//
// Created by figurantpp on 22/12/2020.
//

#include "bf.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define BOUND_CHECK 0

struct BFState
{
    /**
     * Start of source code
     */
    const char *source_code_start;
    const char *source_cursor;
    char *data_cursor;
    char *data_buffer_start;
    FILE *output_file;
    FILE *input_file;
    size_t data_buffer_limit;
    size_t source_code_length;


    const char **jump_table;

};


static void bf_execute_state(struct BFState *bf_state);


static inline void bf_advance_data_cursor(struct BFState *bf_state)
{
#if BOUND_CHECK
    if (bf_state->data_cursor == bf_state->data_buffer_start + bf_state->data_buffer_limit)
    {
        fprintf(stderr, "Exceeded buffer end (position %zu)\n", bf_state->data_buffer_limit);
        abort();
    }
#endif

    bf_state->data_cursor++;
}

static inline void bf_retreat_data_cursor(struct BFState *bf_state)
{
#if BOUND_CHECK
    if (bf_state->data_cursor == bf_state->data_buffer_start)
    {
        fprintf(stderr, "Tried to move before start of data buffer.\n");
        abort();
    }
#endif

    bf_state->data_cursor--;
}

static inline void bf_increment_value(struct BFState *bf_state)
{
    (*bf_state->data_cursor)++;
}

static inline void bf_decrement_value(struct BFState *bf_state)
{
    (*bf_state->data_cursor)--;
}

static inline void bf_write_output(struct BFState *bf_state)
{
    fputc(*bf_state->data_cursor, bf_state->output_file);
}

static inline void bf_read_input(struct BFState *bf_state)
{
    int input = fgetc(bf_state->input_file);

    if (input != EOF)
    {
        *bf_state->data_cursor = (char) input;
    }
}

// 	if the byte at the data pointer is zero,
// 	then instead of moving the instruction pointer forward to the next command,
// 	jump it forward to the command after the matching ] command.

static inline void bf_left_brace(struct BFState *bf_state)
{
    if (*bf_state->data_cursor != 0)
    { return; }

    const char *result = bf_state->jump_table[bf_state->source_cursor - bf_state->source_code_start];

#if BOUND_CHECK
    if (result == NULL)
    {
        fprintf(stderr, "No result found for left brace at position %lu",
                bf_state->source_cursor - bf_state->source_code_start);
        abort();
    }
#endif

    bf_state->source_cursor = result;
}

// 	if the byte at the data pointer is nonzero,
// 	then instead of moving the instruction pointer forward to the next command,
// 	jump it back to the command after the matching [ command.
static inline void bf_right_brace(struct BFState *bf_state)
{
    if (*bf_state->data_cursor == 0)
    { return; }

    const char *result = bf_state->jump_table[bf_state->source_cursor - bf_state->source_code_start];

#if BOUND_CHECK
    if (result == NULL)
    {
        fprintf(stderr, "No result found for left brace at position %lu",
                bf_state->source_cursor - bf_state->source_code_start);
        abort();
    }
#endif

    bf_state->source_cursor = result;

}

__attribute__((always_inline))
static inline void bf_advance_instruction(struct BFState *bf_state)
{
    bf_state->source_cursor++;
}

void bf_loop(struct BFState *bf_state)
{
    while (bf_state->source_cursor != bf_state->source_code_start + bf_state->source_code_length)
    {
        switch (*bf_state->source_cursor)
        {
            case '>': bf_advance_data_cursor(bf_state);
                break;
            case '<': bf_retreat_data_cursor(bf_state);
                break;
            case '+': bf_increment_value(bf_state);
                break;
            case '-': bf_decrement_value(bf_state);
                break;
            case '[': bf_left_brace(bf_state);
                break;
            case ']': bf_right_brace(bf_state);
                break;
            case '.': bf_write_output(bf_state);
                break;
            case ',': bf_read_input(bf_state);
                break;
        }

        bf_advance_instruction(bf_state);
    }
}


static void bf_seek_matching_right_brace(struct BFState *bf_state)
{
    typeof(bf_state->source_cursor)
            pointer = bf_state->source_cursor,
            end = bf_state->source_cursor + bf_state->source_code_length;

    int open_count = 1;

    // We have to start looking after the current cursor
    pointer++;

    while (open_count != 0 && pointer != end)
    {
        switch (*pointer)
        {
            case ']': open_count--; break;
            case '[': open_count++; break;
        }

        pointer++;
    }

    // Pointer will point to the char after the ']' because of the while loop
    // So we need to adjust it to the preceding ']'
    pointer--;

    if (open_count != 0)
    {
        fprintf(stderr, "Unclosed bracket in char %lu",
                bf_state->source_cursor - bf_state->source_code_start);

        abort();
    }

    if (*pointer != ']')
    {
        printf("source code start: %p\n", bf_state->source_code_start);
        printf("pointer: %p\n", pointer);
        printf("end: %p\n", end);
        printf("source code length: %zu\n", bf_state->source_code_length);
        printf("position: %lu\n", pointer - bf_state->source_code_start);
        printf("*pointer: %d\n", *pointer);

        fflush(stdout);
    }

    assert(*pointer == ']');

    bf_state->source_cursor = pointer;
}


void bf_get_metadata(struct BFState *bf_state)
{
    // First part: looking at the source to find number of open and close braces


    unsigned long left_brace_count = 0;
    unsigned long right_brace_count = 0;

    const char *position = bf_state->source_code_start;

    const char *end = bf_state->source_code_start + bf_state->source_code_length;

    while (position != end)
    {
        if (*position == '[')
        {
            left_brace_count++;
        }
        else if (*position == ']')
        {
            right_brace_count++;
        }

        position++;
    }

    if (left_brace_count != right_brace_count)
    {
        fprintf(stderr, "Invalid source code. (%zu) left braces while (%zu) right braces found.\n",
                left_brace_count, right_brace_count);
        abort();
    }

    bf_state->jump_table = calloc(bf_state->source_code_length, sizeof(const char*));

    if (bf_state->jump_table == NULL)
    {
        perror("Failed to allocate jump table to execute source code");
        abort();
    }

    position = bf_state->source_code_start;

    // Second part: mapping positions

    size_t index_position = 0;

    while (position != end)
    {
        if (*position == '[')
        {
            bf_state->source_cursor = position;
            bf_seek_matching_right_brace(bf_state);

            const char *left_position = position;
            const char *right_position = bf_state->source_cursor;

            // Both left and right index have to be mapped
            bf_state->jump_table[left_position - bf_state->source_code_start] = right_position;
            bf_state->jump_table[right_position - bf_state->source_code_start] = left_position;

            index_position++;
        }

        position++;
    }


    // Third part: sort indexes


    bf_state->source_cursor = bf_state->source_code_start;
}

void bf_cleanup_metadata(struct BFState *bf_state)
{
    free(bf_state->jump_table);
}

#define null_check(parameter) \
    ({                        \
        if (parameter == NULL)\
        {                     \
            fprintf(stderr, "%s: NULL parameter %s given. Aborting.\n", __FUNCTION__, #parameter); \
            abort();\
        }                 \
    })

void bf_execute(const char *source_code, FILE *input_file, FILE *output_file)
{
    null_check(source_code);
    null_check(input_file);

    struct BFState bf_state[1] = {{}};

    bf_state->source_code_start = source_code;
    bf_state->source_code_length = strlen(source_code);
    bf_state->source_cursor = source_code;
    bf_state->input_file = input_file;
    bf_state->output_file = output_file;

    bf_execute_state(bf_state);
}


/**
 * Executes using the given bf_state.
 * @param bf_state
 * @note If the \p data_buffer property of the given state is NULL and the data_buffer_length property is 0,
 * a buffer of size \p BF_DEFAULT_BUFFER_LIMIT is automatically allocated and fred.
 * @note If the state contains a data buffer and a source code but the cursor pointers are set to NULL,
 * they will point of the start of their respective strings.
 */
void bf_execute_state(struct BFState *bf_state)
{
    null_check(bf_state);
    null_check(bf_state->source_code_start);

    unsigned char is_self_allocated = 0;

    if (bf_state->data_buffer_start == NULL && bf_state->data_buffer_limit == 0 && bf_state->data_cursor == NULL)
    {

        is_self_allocated = 1;

        char *data_buffer = calloc(1, BF_DEFAULT_BUFFER_LIMIT);

        if (data_buffer == NULL)
        {
            fprintf(stderr, "Failed to allocate data buffer (size %zu)\n", BF_DEFAULT_BUFFER_LIMIT);
            abort();
        }

        bf_state->data_buffer_start = data_buffer;

        bf_state->data_cursor = data_buffer;

        bf_state->data_buffer_limit = BF_DEFAULT_BUFFER_LIMIT;
    }

    if (bf_state->data_cursor == NULL)
    {
        bf_state->data_cursor = bf_state->data_buffer_start;
    }

    if (bf_state->source_cursor == NULL)
    {
        bf_state->source_cursor = bf_state->source_code_start;
    }

    bf_get_metadata(bf_state);

    bf_loop(bf_state);

    bf_cleanup_metadata(bf_state);

    fflush(bf_state->output_file);

    if (is_self_allocated)
    {
        free(bf_state->data_buffer_start);
        bf_state->data_buffer_start = NULL;
    }
}
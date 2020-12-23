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
    const char *source_code_start;
    const char *source_cursor;
    char *data_cursor;
    char *data_buffer_start;
    FILE *output_file;
    FILE *input_file;
    size_t data_buffer_limit;
    size_t source_code_length;

    struct BFMetadata
    {
        struct BFMetadataBracketPair
        {
            const char *left_brace;
            const char *right_brace;
        } *left_bracket_index;

        struct BFMetadataBracketPair *right_bracket_index;

        size_t bracket_count;
    } metadata;


};

static inline int bf_compare_left_index(const void *first, const void* second)
{
    const struct BFMetadataBracketPair *first_pair = first;
    const struct BFMetadataBracketPair *second_pair = second;

    return (int)(first_pair->left_brace - second_pair->left_brace);
}

static inline int bf_compare_right_index(const void *first, const void* second)
{
    const struct BFMetadataBracketPair *first_pair = first;
    const struct BFMetadataBracketPair *second_pair = second;

    return (int)(first_pair->right_brace - second_pair->right_brace);
}

static void bf_execute_state(struct BFState *bf_state);


void bf_advance_data_cursor(struct BFState *bf_state)
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

void bf_retreat_data_cursor(struct BFState *bf_state)
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

void bf_read_input(struct BFState *bf_state)
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

void bf_left_brace(struct BFState *bf_state)
{
    if (*bf_state->data_cursor != 0)
    { return; }

    struct BFMetadataBracketPair key = {.left_brace = bf_state->source_cursor};
    struct BFMetadataBracketPair* result = bsearch(&key,
            bf_state->metadata.left_bracket_index, bf_state->metadata.bracket_count, sizeof(key),
            bf_compare_left_index);

    if (result == NULL)
    {
        fprintf(stderr, "No result found for left brace at position %lu",
                bf_state->source_cursor - bf_state->source_code_start);
        abort();
    }

    bf_state->source_cursor = result->right_brace;
}

// 	if the byte at the data pointer is nonzero,
// 	then instead of moving the instruction pointer forward to the next command,
// 	jump it back to the command after the matching [ command.
void bf_right_brace(struct BFState *bf_state)
{
    if (*bf_state->data_cursor == 0)
    { return; }

    struct BFMetadataBracketPair key = {.right_brace = bf_state->source_cursor};
    struct BFMetadataBracketPair* result = bsearch(&key, bf_state->metadata.right_bracket_index, bf_state->metadata.bracket_count,
                                                   sizeof(key), bf_compare_right_index);

    if (result == NULL)
    {
        fprintf(stderr, "No result found for right brace at position %lu",
                bf_state->source_cursor - bf_state->source_code_start);
        abort();
    }

    bf_state->source_cursor = result->left_brace;
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

    bf_state->metadata.bracket_count = left_brace_count;

    bf_state->metadata.left_bracket_index = malloc(left_brace_count * sizeof(struct BFMetadataBracketPair));
    bf_state->metadata.right_bracket_index = malloc(left_brace_count * sizeof(struct BFMetadataBracketPair));

    if (!bf_state->metadata.left_bracket_index || !bf_state->metadata.right_bracket_index)
    {
        fputs("Failed to allocate index to execute source code", stderr);
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

            // Both left and right index have to be mapped
            bf_state->metadata.left_bracket_index[index_position].left_brace = position;
            bf_state->metadata.left_bracket_index[index_position].right_brace = bf_state->source_cursor;

            bf_state->metadata.right_bracket_index[index_position].left_brace = position;
            bf_state->metadata.right_bracket_index[index_position].right_brace = bf_state->source_cursor;

            index_position++;
        }

        position++;
    }


    // Third part: sort indexes

    qsort(bf_state->metadata.left_bracket_index, left_brace_count, sizeof(struct BFMetadataBracketPair), bf_compare_left_index);
    qsort(bf_state->metadata.right_bracket_index, right_brace_count, sizeof(struct BFMetadataBracketPair), bf_compare_right_index);

    bf_state->source_cursor = bf_state->source_code_start;
}

void bf_execute(const char *source_code, FILE* input_file, FILE *output_file)
{
    struct BFState bf_state[1] = {{}};

    bf_state->source_code_start = source_code;
    bf_state->source_code_length = strlen(source_code);
    bf_state->source_cursor = source_code;
    bf_state->input_file = input_file;
    bf_state->output_file = output_file;

    bf_execute_state(bf_state);
}

void bf_execute_string(const char *source_code)
{
    bf_execute(source_code, stdin, stdout);
}

static void bf_execute_state(struct BFState *bf_state)
{
    if (bf_state == NULL)
    {
        fprintf(stderr, "bf_execute_state: NULL bf_state given.\n");
        abort();
    }

    if (bf_state->source_code_start == NULL)
    {
        fprintf(stderr, "bf_execute_state: NULL source code given.\n");
        abort();
    }

    unsigned char is_self_allocated = 0;

    if (bf_state->data_buffer_start == NULL && bf_state->data_buffer_limit == 0 && bf_state->data_cursor == NULL) {

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

    if (bf_state->input_file == NULL)
    {
        bf_state->input_file = stdin;
    }

    if (bf_state->output_file == NULL)
    {
        bf_state->output_file = stdout;
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

    fflush(bf_state->output_file);

    if (is_self_allocated)
    {
        free(bf_state->data_buffer_start);
    }
}
//
// Created by figurantpp on 22/12/2020.
//

#include "streams.h"
//
// Created by figurantpp on 21/10/2020.
//

/**
 *
 *  This is free and unencumbered software released into the public domain.
 *  Anyone is free to copy, modify, publish, use, compile, sell, or distribute this software, either in source code
 *  form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
 *
 *  In jurisdictions that recognize copyright laws, the author or authors of this software dedicate any and all
 *  copyright interest in the software to the public domain. We make this dedication for the benefit of the public at
 *  large and to the detriment of our heirs and successors. We intend this dedication to be an overt act of
 *  relinquishment in perpetuity of all present and future rights to this software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include "streams.h"

char *read_whole_stream(int file, size_t *size_ptr)
{
    size_t total_size = 0;
    size_t position = 0;

    const size_t buffer_size = 256;

    char *result = NULL;

    // temporary buffer for realloc
    char *allocation;

    int bytes_read = 0;

    // required to preserve errno
    int status;

    do
    {
        if (bytes_read + position == total_size)
        {
            allocation = realloc(result, total_size + buffer_size);

            if (!allocation)
            { return NULL; }

            result = allocation;


            total_size += buffer_size;
        }

        position += bytes_read;
    } while ((bytes_read = read(file, result + position, total_size - position)) > 0);

    if (bytes_read == -1)
    {
        status = errno;
        free(result);
        errno = status;
        return NULL;
    }

    allocation = realloc(result, position + 1);

    if (!allocation)
    { return NULL; }

    result = allocation;

    result[position] = '\0';

    if (size_ptr)
    {
        *size_ptr = position;
    }

    return result;
}

char *read_whole_file(FILE *file, size_t *size_ptr)
{
    long old_pos = ftell(file);

    if (old_pos == -1)
    {
        return NULL;
    }

    fseek(file, 0, SEEK_END);

    size_t size = ftell(file);
    fseek(file, old_pos, SEEK_SET);

    if (size_ptr)
    {
        *size_ptr = size;
    }

    char *result = malloc(size + 1);

    if (!result)
    { return NULL; }

    fread(result, size, 1, file);

    result[size] = '\0';

    return result;
}
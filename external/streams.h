//
// Created by figurantpp on 22/12/2020.
//

#ifndef BF_STREAMS_H
#define BF_STREAMS_H

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

#include <stdio.h>

/**
 * `read`s the whole of content of the file descriptor `file` until end-of-file is reached.
 * If `size_ptr` is not not null, the number of bytes read is saved in `*size_ptr`.
 *
 * @param file The file descriptor of the content to be read.
 * @param size_ptr If not null, points to memory in which to save the number of bytes read.
 * @return NULL if the allocation failed.
 * @note If NULL is returned, the value of <code>errno</code> is updated accordingly to the ones defined by the
 * <code> read </code> function.
 */
char* read_whole_stream(int file, size_t* size_ptr);

/**
 * Reads the whole content of `file` until EOF is found.
 * @param file The file to read from.
 * @param size_ptr If not NULL, points to the location in which to save the number of bytes read.
 * @return A pointer containing the bytes read, or NULL if the operation failed.
 * @note <code>file</code> must point to a seekable stream (e.g not a pipe, FIFO, or socket).
 * @note The values of <code>errno</code> are the intersection of the ones set by <code> ftell(3) </code> and
 * <code> malloc(3) </code>
 */
char* read_whole_file(FILE* file, size_t* size_ptr);

#endif //BF_STREAMS_H

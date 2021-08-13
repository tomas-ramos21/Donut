#ifndef MEM_UTILS_H_
#define MEM_UTILS_H_

#include "stdio.h"

/**
 * @file mem_utils.c
 * Functions used to act or manipulate memory.
 */

/**
 * Constant comparison of memory buffers.
 *
 * Compares the content of 2 memory buffers in constant time to prevent timing
 * attacks for cryptographic or safety critical data.
 *
 * @param s1 Pointer to buffer 1
 * @param s2 Pointer to buffer 2
 * @param n Amount of bytes to compare
 * @returns In case of equality 1 is returned, otherwise 0
 */
int const_memcmp(void* s1, void* s2, size_t n);

/**
 * Collection of unit tests for const_memcmp.
 * Runs tests to check if the function is comparing 2 memory buffers correctly.
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
int test_const_memcmp(void);

#endif // MEM_UTILS_H_

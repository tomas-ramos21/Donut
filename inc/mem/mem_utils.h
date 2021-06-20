#ifndef MEM_UTILS_H_
#define MEM_UTILS_H_

#include "stdio.h"

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

/* Unit tests */
int test_const_memcmp(void);

#endif // MEM_UTILS_H_

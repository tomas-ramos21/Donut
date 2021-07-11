#ifndef __ALLOC_H_
#define __ALLOC_H_

#include "stdio.h"

/**
 * @file alloc.h
 *
 * Simple wrappers around the standard memory allocation functions.
 *
 * Using the standard allocation functions is boring, since it doesn't
 * provide any error control over the allocation process and a lot of code ends
 * up being repeated to do so. Therefore, these wrappers are meant to provide a
 * layer of control over the memory allocation functions to ensure they behave
 * as intended.
 */

/**
 * Wrapper of the standard "malloc" function.
 *
 * Wrapper around the standard "malloc" function which provides the following:
 *  1. Ensures memory is aligned to the cache line size
 *  2. If the allocation fails, the program is immediatly failed
 *
 * @param sz Byte size of the memory to be allocated
 * @returns Pointer to the allocated buffer
 */
void* xmalloc(size_t sz);

/**
 * Wrapper of the standard "calloc" function.
 *
 * Wrapper around the standard "calloc" function which provides the following:
 *  1. Ensures memory is aligned to the cache line size
 *  2. If the allocation fails, the program is immediatly failed
 *
 * @param n Number of memory blocks to be allocated
 * @param sz Byte size of each memory block to be allocated
 * @returns Pointer to the allocated buffer
 */
void* xcalloc(size_t n, size_t sz);

/**
 * Wrapper of the standard "realloc" function.
 *
 * Wrapper around the standard "realloc" function which provides the following:
 *  1. Ensures memory is aligned to the cache line size
 *  2. If the allocation fails, the program is immediatly failed
 *
 * @param buff Buffer whose memory content needs to be expanded
 * @param sz Byte size of the buffer to be allocated
 * @returns Pointer to the allocated buffer
 */
void* xrealloc(void* buff, size_t sz);

/**
 * Collection of unit tests for xmalloc.
 * Runs tests to check if the function is capable of allocating aligned memory.
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
int test_xmalloc(void);

/**
 * Collection of unit tests for xcalloc.
 * Runs tests to check if the function is capable of allocating aligned memory.
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
int test_xcalloc(void);

/**
 * Collection of unit tests for xrealloc.
 * Runs tests to check if the function is capable of allocating aligned memory.
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
int test_xrealloc(void);

#endif // __ALLOC_H_

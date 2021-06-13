#ifndef __ALLOC_H_
#define __ALLOC_H_

#include "stdio.h"

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

/* Unit tests */
int test_xmalloc(int reps);
int test_xcalloc(int reps);
int test_xrealloc(int reps);

#endif // __ALLOC_H_

#ifndef __SLOB_H_
#define __SLOB_H_

#include "stdlib.h"

/**
 * @file slob.h
 *
 * In order to facilitate manual memory management this slob allocator was
 * created. It allows the "owner" of the allocator to request and realease
 * memory slobs as needed, with the guarantuee of no memory leaks. Moreover
 * it will align memory to the cache-line's size to prevent false sharing.
 */

/**
 * Initialize a slob allocator.
 * @returns Pointer to the slob allocator.
 */
struct slobs* init_slobs(void);


/**
 * Allocate a slob with a given slob allocator.
 *
 * This function will allocate a new slob of aligned memory and add it to the
 * slob allocator provided. It will grow the amount of slobs supported if all
 * the slots have already been filled.
 *
 * @param ptr Pointer to a slob allocator structure
 * @param slob_sz Byte of the slob to be allocated
 * @returns Pointer to the allocated slob
 */
void* alloc_slob(struct slobs* ptr, size_t sz);

/**
 * Free a slob from the given slob allocator structure.
 *
 * Receives a pointer and determines if it corresponds to a slob within the
 * given slob structure. If that's the case the following happens:
 *   1. The memory slob is freed
 *   2. The slob list is updated to support 1 more slob
 *   3. The slob list is re-order so that no gaps exist
 *
 * @param ptr Pointer to a slob allocator structure
 * @param slob Pointer to the slob to deallocate
 */
void free_slob(struct slobs* ptr, void* slob);


/**
 * Free all memory manageed by the allocator and the allocator itself.
 *
 * The function will walk over the list of slobs maintained by the allocator and
 * free all of them. At the end the allocator itself is freed.
 *
 * @param ptr Pointer to the allocator structure to free
 */
void clear_slobs(struct slobs* ptr);

/**
 * Unit test for "init_slobs" function.
 *
 * Tests that after the slob allocator initialization all fields are set to 0.
 *
 * @returns In case of success the return value is 1 otherwise its 0.
 */
int test_init_slob(void);

/**
 * Unit test for "alloc_slob" function.
 *
 * Performs several tests to ensure that:
 *   1. Slob allocation under the machine's page size is performed successfully
 *   2. Slob allocation above the machine's page size is performed successfully
 *   3. The slobs allocated are aligned to the machine's cache line size
 *   4. The bookkeeping variables are properly updated as allocations occur
 *
 * @returns In case of success the return value is 1 otherwise its 0.
 */
int test_alloc_slob(void);

/**
 * Unit test for "free_slob" function.
 *
 * Performs several tests to ensure that:
 *   1. The allocator is capable of freeing memory slobs
 *   2. The bookkeeping variables are properly updated as deallocations occur
 *
 * @returns In case of success the return value is 1 otherwise its 0.
 */
int test_free_slob(void);

#endif // __SLOB_H_

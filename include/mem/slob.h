#ifndef __SLOB_H_
#define __SLOB_H_

#include "stdlib.h"

/**
 * @file slob.h
 *
 * In order to facilitate manual memory management this SLOB allocator was
 * created. It allows the "owner" of the allocator to request and realease
 * memory slabs as needed, with the guarantuee of no memory leaks. Moreover
 * it will align memory to the cache-line's size to prevent false sharing.
 */

/**
 * Initialize a SLOB allocator.
 * @returns Pointer to the slob allocator.
 */
struct slobs* init_slobs(void);


/**
 * Allocate a slab with a given SLOB allocator.
 *
 * This function will allocate a new slab of aligned memory and add it to the
 * SLOB allocator provided. It will grow the amount of slabs supported if all
 * the slots have already been filled.
 *
 * @param ptr Pointer to a slob allocator structure
 * @param slob_sz Byte of the slab to be allocated
 * @returns Pointer to the allocated slab
 */
void* alloc_slob(struct slobs* ptr, size_t sz);

/**
 * Free a slab from the given SLOB allocator structure.
 *
 * Receives a pointer and determines if it corresponds to a slab within the
 * given SLOB structure. If that's the case the following happens:
 *   1. The memory slab is freed
 *   2. The SLOB list is updated to support 1 more slab
 *   3. The SLOB list is re-ordered so that no gaps exist
 *
 * @param ptr Pointer to a SLOB allocator
 * @param slob Pointer to the slab to deallocate
 */
void free_slob(struct slobs* ptr, void* slob);


/**
 * Free all memory managed by the allocator and the allocator itself.
 *
 * The function will walk over the list of slabs maintained by the allocator and
 * free all of them. At the end the allocator itself is freed.
 *
 * @param ptr Pointer to the allocator structure to free
 */
void clear_slobs(struct slobs* ptr);

/**
 * Unit test for "init_slobs" function.
 *
 * Tests that after the SLOB allocator initialization all fields are set to 0.
 *
 * @returns In case of success the return value is 1 otherwise its 0.
 */
int test_init_slob(void);

/**
 * Unit test for "alloc_slab" function.
 *
 * Performs several tests to ensure that:
 *   1. Slab allocation under the machine's page size is performed successfully
 *   2. Slab allocation above the machine's page size is performed successfully
 *   3. The slabs allocated are aligned to the machine's cache line size
 *   4. The bookkeeping variables are properly updated as allocations occur
 *
 * @returns In case of success the return value is 1 otherwise its 0.
 */
int test_alloc_slob(void);

/**
 * Unit test for "free_slob" function.
 *
 * Performs several tests to ensure that:
 *   1. The allocator is capable of freeing memory slabs
 *   2. The bookkeeping variables are properly updated as deallocations occur
 *
 * @returns In case of success the return value is 1 otherwise its 0.
 */
int test_free_slob(void);

#endif // __SLOB_H_

#include "mem/slab.h"
#include "mem/alloc.h"
#include "inttypes.h"

/**
 * @file slab.c
 *
 * Implementation of slab allocator for memory management.
 *
 * In order to facilitate manual memory management this slab allocator was
 * created. It allows the "owner" of the allocator to request and realease
 * memory slabs as needed, with the guarantuee of no memory leaks.
 */

/**
 * Structure that contains the slab allocator bookkeeping variables.
 */
struct slabs {
        uint32_t slab_t; /**< Total amount of slabs                 */
        uint32_t slab_l; /**< Amount of slabs left in the allocator */
        void** slabs;    /**< Complete list of slabs                */
        void** curr;     /**< Current free slab                     */
};

/**
 * Initialize a slab allocator.
 * @returns Pointer to the slab allocator.
 */
struct slabs*
init_slabs(void)
{
        return calloc(1, sizeof(struct slabs));
}

/**
 * Unit test for "init_slabs" function.
 * @returns In case of success the return value is 1 otherwise its 0.
 */
int
test_init_slab(void)
{
        struct slabs* p = init_slabs();
        return (!p->slab_t && !p->slab_l && !p->slabs && !p->curr) ? 1: 0;
}

/**
 * Allocate a slab with a given slab allocator.
 *
 * This function will allocate a new slab of aligned memory and add it to the
 * slab allocator provided. It will grow the amount of slabs supported if all
 * the slots have already been filled.
 *
 * @param ptr Pointer to a slab allocator structure
 * @param slab_sz Byte of the slab to be allocated
 * @returns Pointer to the allocated slab
 */
void*
alloc_slab(struct slabs* restrict ptr, size_t slab_sz)
{
        size_t sz  = (slab_sz < PAGE_SIZE) ? PAGE_SIZE : slab_sz;
        void*  ret = xcalloc(1, sz);

        if (!ptr->slab_l) {
                ptr->slabs = xrealloc(ptr->slabs, (ptr->slab_t + 10) * sizeof(void*));
                ptr->slab_t += 10;
                ptr->slab_l += 10;

                void** slabs = ptr->slabs;
                while(*slabs)
                        slabs++;
                ptr->curr = slabs;
        }

        /* Adjust slab bookkeeping state */
        ptr->slab_l--;
        *ptr->curr = ret;
        ptr->curr++;
        return ret;
}

/**
 * Unit test for "alloc_slab" function.
 *
 * The following tests are performed:
 *   1. Slab allocation under the machine's page size
 *   2. Slab allocation above the machine's page size
 *
 * @returns In case of success the return value is 1 otherwise its 0.
 */
int
test_alloc_slab(void)
{
        struct slabs* ptr;
        void* pg;
        int sum = 0;

        /* Test Allocation below page size */
        ptr = init_slabs();
        pg = alloc_slab(ptr, PAGE_SIZE >> 1);
        if (ptr->slab_l == 9 &&
            ptr->slab_t == 10 &&
            ptr->slabs &&
            pg &&
            *ptr->slabs == pg &&
            ptr->curr &&
            ptr->curr == (ptr->slabs + 1))
                sum += 1;

        /* Test second allocation, but above page size */
        pg = alloc_slab(ptr, PAGE_SIZE << 1);
        if (ptr->slab_l == 8 &&
            ptr->slab_t == 10 &&
            (ptr->slabs + 1) &&
            pg &&
            *(ptr->slabs + 1) == pg &&
            ptr->curr &&
            ptr->curr == (ptr->slabs + 2))
                sum += 1;

        return (sum == 2) ? 1 : 0;
}

/**
 * Free a slab from the given slab allocator structure.
 *
 * Receives a pointer and determines if it corresponds to a slab within the
 * given slab structure. If that's the case the following happens:
 *   1. The memory slab is freed
 *   2. The slab list is updated to support 1 more slab
 *   3. The slab list is re-order to that no gaps exist
 *
 * @param ptr Pointer to a slab allocator structure
 * @param slab Pointer to the slab to deallocate
 */
void
free_slab(struct slabs* restrict ptr, void* slab)
{
        void** slabs = ptr->slabs;
        while (*slabs && *slabs != slab)
                slabs++;

        if (!*slabs)
                return;

        free(*slabs);
        ptr->slab_l += 1;

        void** nxt = ++slabs;
        slabs--;
        while ((*slabs++ = *nxt++));
}

/**
 * Unit test for "free_slab" function.
 *
 * Runs tests to check if the function is capable of freeing a memory slab
 * and then update the respective variables within the allocator structure.
 *
 * @returns In case of success the return value is 1 otherwise its 0.
 */
int
test_free_slab(void)
{
        struct slabs* ptr = init_slabs();
        void* pg = alloc_slab(ptr, PAGE_SIZE);
        void* pg2 = alloc_slab(ptr, PAGE_SIZE);

        free_slab(ptr, pg);
        if (*ptr->slabs == pg2 &&
            *(ptr->slabs + 1) == 0x0 &&
            ptr->slab_t == 10 &&
            ptr->slab_l == 9)
                return 1;
        else
                return 0;
}

/**
 * Free all memory manageed by the allocator and the allocator itself.
 *
 * The function will walk over the list of slabs maintained by the allocator and
 * free all of them. At the end the allocator itself is freed.
 *
 * @param ptr Pointer to the allocator structure to free
 */
void
clear_slabs(struct slabs* restrict ptr)
{
        void** slabs = ptr->slabs;
        while (*slabs) {
                free(*slabs);
                slabs++;
        }
        free(ptr);
}

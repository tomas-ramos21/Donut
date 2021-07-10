#include "mem/slab.h"
#include "mem/alloc.h"
#include "inttypes.h"
#include "string.h"

/**
 * @file slab.c
 * Implementation of slab allocator module.
 */

#define SLAB_GROWTH 10

/**
 * Structure that contains the slab allocator bookkeeping variables.
 */
struct slabs {
        uint32_t slab_t;  /**< Total amount of slabs                 */
        uint32_t slab_l;  /**< Amount of slabs left in the allocator */
        uint64_t idx;     /**< Index of the next slab                */
        void** slabs;     /**< List of aligned slabs                 */
        void** origs;     /**< List of original unaligned slabs      */
};

struct slabs*
init_slabs(void)
{
        return xcalloc(1, sizeof(struct slabs));
}

/**
 * Unit test for "init_slabs" function.
 * @returns In case of success the return value is 1 otherwise its 0.
 */
int
test_init_slab(void)
{
        struct slabs* p = init_slabs();
        int ret = (!p->slab_t && !p->slab_l && !p->slabs &&
                   !p->origs && !p->idx) ? 1: 0;
        clear_slabs(p);
        return ret;
}

/**
 * Align a memory address to the cache line's size.
 *
 * @param ptr Pointer to the memory address to be aligned
 * @returns Pointer to an aligned memory address
 */
inline static void*
align_addr(void* ptr)
{
        const int align = CACHE_LINE - 1;
        uintptr_t bit_mask = ~(uintptr_t)align;
        return (void*)(((uintptr_t)ptr + align) & bit_mask);

}

void*
alloc_slab(struct slabs* restrict ptr, size_t slab_sz)
{
        size_t sz  = (slab_sz < PAGE_SIZE) ? PAGE_SIZE : slab_sz;
        void*  mem = xcalloc(1, sz + CACHE_LINE);
        void*  ret = align_addr(mem);

        if (!ptr->slab_l) {
                ptr->slab_t += SLAB_GROWTH;
                ptr->slab_l += SLAB_GROWTH;
                ptr->slabs = xrealloc(ptr->slabs, ptr->slab_t * __SIZEOF_POINTER__);
                ptr->origs = xrealloc(ptr->origs, ptr->slab_t * __SIZEOF_POINTER__);

                /* Zero new indices */
                memset(&ptr->slabs[ptr->idx], 0x0, SLAB_GROWTH * __SIZEOF_POINTER__);
                memset(&ptr->origs[ptr->idx], 0x0, SLAB_GROWTH * __SIZEOF_POINTER__);
        }

        /* Adjust slab bookkeeping state */
        ptr->origs[ptr->idx] = mem;
        ptr->slabs[ptr->idx] = ret;
        ptr->idx++;
        ptr->slab_l--;
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
            !((uintptr_t)pg % CACHE_LINE) &&
            ptr->slabs[0] == pg &&
            ptr->origs &&
            ptr->origs[0])
                sum += 1;

        /* Test second allocation, but above page size */
        pg = alloc_slab(ptr, PAGE_SIZE << 1);
        if (ptr->slab_l == 8 &&
            ptr->slab_t == 10 &&
            &ptr->slabs[1] &&
            pg &&
            !((uintptr_t)pg % CACHE_LINE) &&
            ptr->slabs[1] == pg &&
            &ptr->origs[1] &&
            ptr->origs[1])
                sum += 1;

        for (int i = 0; i < 512; i++)
                     alloc_slab(ptr, PAGE_SIZE);

        clear_slabs(ptr);

        return (sum == 2) ? 1 : 0;
}

void
free_slab(struct slabs* restrict ptr, void* slab)
{
        void** slabs = ptr->slabs;
        void** orig = ptr->origs;

        /* Lookup pointer to be freed */
        while (*slabs && *slabs != slab) {
                slabs++;
                orig++;
        }

        if (!*orig)
                return;

        free(*orig);
        ptr->slab_l++;
        ptr->idx--;

        /* Shrink list of pointers [Aligned] */
        void** nxt = ++slabs;
        slabs--;
        while ((*slabs++ = *nxt++));

        /* Shrink list of pointers [Unaligned] */
        nxt = orig;
        orig--;
        while((*orig++ = *nxt++));
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
        int ret = 0;
        void* pg = alloc_slab(ptr, PAGE_SIZE);
        void* pg2 = alloc_slab(ptr, PAGE_SIZE);
        void* orig_cp = ptr->origs[1];

        free_slab(ptr, pg);
        if (ptr->slabs[0] == pg2 &&
            ptr->slabs[1] == 0x0 &&
            ptr->slab_t == 10 &&
            ptr->slab_l == 9 &&
            ptr->origs[0] == orig_cp)
                ret = 1;
        else
                ret = 0;

        clear_slabs(ptr);
        return ret;
}

void
clear_slabs(struct slabs* restrict ptr)
{
        void** slabs = ptr->origs;
        for (uint64_t i = 0; i != ptr->idx; i++)
                free(slabs[i]);
        free(ptr);
}

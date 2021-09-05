#include "core/wrappers.h"
#include "mem/slob.h"
#include "stdio.h"
#include "inttypes.h"
#include "string.h"

/**
 * @file slob.c
 * Implementation of slob allocator module.
 */

/**
 * @def SLOB_GROWTH
 * Number of new slobs slots added at each expansion.
 */
#define SLOB_GROWTH 10

/**
 * Structure that contains the slob allocator bookkeeping variables.
 */
struct slobs {
        uint32_t slob_t;  /**< Total amount of slobs                 */
        uint32_t slob_l;  /**< Amount of slobs left in the allocator */
        uint64_t idx;     /**< Index of the next slob                */
        void** slobs;     /**< List of aligned slobs                 */
        void** origs;     /**< List of original unaligned slobs      */
};

struct slobs*
init_slobs(void)
{
        return xcalloc(1, sizeof(struct slobs));
}

int
test_init_slob(void)
{
        struct slobs* p = init_slobs();
        int ret = (!p->slob_t && !p->slob_l && !p->slobs &&
                   !p->origs && !p->idx) ? 1: 0;
        clear_slobs(p);
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
alloc_slob(struct slobs* restrict ptr, size_t slob_sz)
{
        size_t sz  = (slob_sz < PAGE_SIZE) ? PAGE_SIZE : slob_sz;
        void*  mem = xcalloc(1, sz + CACHE_LINE);
        void*  ret = align_addr(mem);

        if (!ptr->slob_l) {
                ptr->slob_t += SLOB_GROWTH;
                ptr->slob_l += SLOB_GROWTH;
                ptr->slobs = xrealloc(ptr->slobs, ptr->slob_t * __SIZEOF_POINTER__);
                ptr->origs = xrealloc(ptr->origs, ptr->slob_t * __SIZEOF_POINTER__);

                /* Zero new indices */
                memset(&ptr->slobs[ptr->idx], 0x0, SLOB_GROWTH * __SIZEOF_POINTER__);
                memset(&ptr->origs[ptr->idx], 0x0, SLOB_GROWTH * __SIZEOF_POINTER__);
        }

        /* Adjust slob bookkeeping state */
        ptr->origs[ptr->idx] = mem;
        ptr->slobs[ptr->idx] = ret;
        ptr->idx++;
        ptr->slob_l--;
        return ret;
}

int
test_alloc_slob(void)
{
        struct slobs* ptr;
        void* pg;
        int sum = 0;

        /* Test Allocation below page size */
        ptr = init_slobs();
        pg = alloc_slob(ptr, PAGE_SIZE >> 1);
        if (ptr->slob_l == 9 &&
            ptr->slob_t == 10 &&
            ptr->slobs &&
            pg &&
            !((uintptr_t)pg % CACHE_LINE) &&
            ptr->slobs[0] == pg &&
            ptr->origs &&
            ptr->origs[0])
                sum += 1;

        /* Test second allocation, but above page size */
        pg = alloc_slob(ptr, PAGE_SIZE << 1);
        if (ptr->slob_l == 8 &&
            ptr->slob_t == 10 &&
            &ptr->slobs[1] &&
            pg &&
            !((uintptr_t)pg % CACHE_LINE) &&
            ptr->slobs[1] == pg &&
            &ptr->origs[1] &&
            ptr->origs[1])
                sum += 1;

        for (int i = 0; i < 512; i++)
                     alloc_slob(ptr, PAGE_SIZE);

        clear_slobs(ptr);

        return (sum == 2) ? 1 : 0;
}

void
free_slob(struct slobs* restrict ptr, void* slob)
{
        void** slobs = ptr->slobs;
        void** orig = ptr->origs;

        /* Lookup pointer to be freed */
        while (*slobs && *slobs != slob) {
                slobs++;
                orig++;
        }

        if (!*orig)
                return;

        free(*orig);
        ptr->slob_l++;
        ptr->idx--;

        /* Shrink list of pointers [Aligned] */
        void** nxt = ++slobs;
        slobs--;
        while ((*slobs++ = *nxt++));

        /* Shrink list of pointers [Unaligned] */
        nxt = ++orig;
        orig--;
        while((*orig++ = *nxt++));
}

int
test_free_slob(void)
{
        struct slobs* ptr = init_slobs();
        int ret = 0;
        void* pg = alloc_slob(ptr, PAGE_SIZE);
        void* pg2 = alloc_slob(ptr, PAGE_SIZE);
        void* orig_cp = ptr->origs[1];

        free_slob(ptr, pg);
        if (ptr->slobs[0] == pg2 &&
            ptr->slobs[1] == 0x0 &&
            ptr->slob_t == 10 &&
            ptr->slob_l == 9 &&
            ptr->origs[0] == orig_cp)
                ret = 1;
        else
                ret = 0;

        clear_slobs(ptr);
        return ret;
}

void
clear_slobs(struct slobs* restrict ptr)
{
        void** slobs = ptr->origs;
        for (uint64_t i = 0; i != ptr->idx; i++)
                free(slobs[i]);
	free(ptr->origs);
	free(ptr->slobs);
        free(ptr);
}

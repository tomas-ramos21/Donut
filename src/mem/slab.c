#include "core/wrappers.h"
#include "mem/slab.h"
#include "stdio.h"
#include "inttypes.h"
#include "string.h"

/**
 * @file slab.c
 * Implementation of slab allocator module.
 */

/**
 * @def SLAB_GROWTH
 * Number of new slabs slots added at each expansion.
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
        printf("Slabs Left: %d\n", ptr->slab_l);
        printf("Slabs Total: %d\n", ptr->slab_t);
        printf("Slabs not null: %p\n", (void*)ptr->slabs);
        printf("Page not null: %p\n", (void*)pg);
        printf("Is Aligned: %d\n", !((uintptr_t)pg % CACHE_LINE));
        printf("First Index matches Page: %d\n", ptr->slabs[0] == pg);
        printf("Origs not null: %p\n", (void*)ptr->origs);
        printf("First Origs not null: %p\n", ptr->origs[0]);

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
        printf("Slabs Left: %d\n", ptr->slab_l);
        printf("Slabs Total: %d\n", ptr->slab_t);
        printf("Slabs not null: %p\n", (void*)&ptr->slabs[1]);
        printf("Page not null: %p\n", (void*)pg);
        printf("Is Aligned: %d\n", !((uintptr_t)pg % CACHE_LINE));
        printf("First Index matches Page: %d\n", ptr->slabs[1] == pg);
        printf("Origs not null: %p\n", (void*)&ptr->origs[1]);
        printf("First Origs not null: %p\n", ptr->origs[1]);

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
        nxt = ++orig;
        orig--;
        while((*orig++ = *nxt++));
}

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
	free(ptr->origs);
	free(ptr->slabs);
        free(ptr);
}

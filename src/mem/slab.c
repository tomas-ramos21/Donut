#include "mem/slab.h"
#include "mem/alloc.h"
#include "inttypes.h"

/**
 * @file slab.c
 *
 * Implementation of slab allocator module.
 *
 * In order to facilitate manual memory management this slab allocator was
 * created. It allows the "owner" of the allocator to request and realease
 * memory slabs as needed, with the guarantuee of no memory leaks. Moreover
 * it will align memory to the cache-line's size to prevent false sharing.
 */

/**
 * Structure that contains the slab allocator bookkeeping variables.
 */
struct slabs {
        uint32_t slab_t;  /**< Total amount of slabs                 */
        uint32_t slab_l;  /**< Amount of slabs left in the allocator */
        void** slabs;     /**< Complete list of slabs                */
        void** origs;     /**< Current free slab                     */
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
        return (!p->slab_t && !p->slab_l && !p->slabs && !p->origs) ? 1: 0;
}

inline static void*
align_ptr(void* ptr)
{
        const int align = CACHE_LINE - 1;
        uintptr_t bit_mask = ~(uintptr_t)align;
        return (void*)(((uintptr_t)ptr + align) & bit_mask);

}

void*
alloc_slab(struct slabs* restrict ptr, size_t slab_sz)
{
        uint32_t idx;
        size_t sz  = (slab_sz < PAGE_SIZE) ? PAGE_SIZE : slab_sz;
        void*  mem = xcalloc(1, sz + CACHE_LINE);
        void*  ret = align_ptr(mem);

        if (!ptr->slab_l) {
                ptr->slab_t += 10;
                ptr->slab_l += 10;
                ptr->slabs = xrealloc(ptr->slabs, (ptr->slab_t) * sizeof(void*));
                ptr->origs= xrealloc(ptr->origs, (ptr->slab_t) * sizeof(void*));
        }

        /* Adjust slab bookkeeping state */
        idx = ptr->slab_t - ptr->slab_l;
        ptr->origs[idx] = mem;
        ptr->slabs[idx] = ret;
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
            *ptr->slabs == pg &&
            ptr->curr &&
            ptr->curr == (ptr->slabs + 1) &&
            ptr->orig_ptr &&
            ptr->orig_ptr[0] &&
            ptr->orig_curr == (ptr->orig_ptr + 1))
                sum += 1;

        /* Test second allocation, but above page size */
        pg = alloc_slab(ptr, PAGE_SIZE << 1);
        if (ptr->slab_l == 8 &&
            ptr->slab_t == 10 &&
            (ptr->slabs + 1) &&
            pg &&
            !((uintptr_t)pg % CACHE_LINE) &&
            *(ptr->slabs + 1) == pg &&
            ptr->curr &&
            ptr->curr == (ptr->slabs + 2) &&
            ptr->orig_ptr &&
            ptr->orig_ptr[1] &&
            ptr->orig_curr == (ptr->orig_ptr + 2))
                sum += 1;

        for (int i = 0; i < 512; i++)
                     alloc_slab(ptr, PAGE_SIZE);

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
        ptr->slab_l += 1;

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
        void* pg = alloc_slab(ptr, PAGE_SIZE);
        void* pg2 = alloc_slab(ptr, PAGE_SIZE);
        void* curr = ptr->curr - 1;
        void* orig_cp = *(ptr->orig_ptr + 1);
        void* orig_curr_cp = ptr->orig_curr - 1;

        free_slab(ptr, pg);
        if (*ptr->slabs == pg2 &&
            *(ptr->slabs + 1) == 0x0 &&
            ptr->slab_t == 10 &&
            ptr->slab_l == 9 &&
            ptr->curr == curr &&
            ptr->orig_ptr[0] == orig_cp &&
            ptr->orig_curr == orig_curr_cp)
                return 1;
        else
                return 0;
}

void
clear_slabs(struct slabs* restrict ptr)
{
        void** slabs = ptr->orig_ptr;
        while (*slabs) {
                free(*slabs);
                slabs++;
        }
        free(ptr);
}

#include "mem/slab.h"
#include "mem/alloc.h"
#include "inttypes.h"

struct slabs {
        uint32_t nd_t; /* Total nodes   */
        uint32_t nd_l; /* Nodes left    */
        void** slabs;  /* List of slabs */
        void** curr;   /* Current slab  */
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
        return (!p->nd_t && !p->nd_l && !p->slabs && !p->curr) ? 1: 0;
}

void*
alloc_slab(struct slabs* restrict ptr, size_t slab_sz)
{
        size_t sz  = (slab_sz < PAGE_SIZE) ? PAGE_SIZE : slab_sz;
        void*  ret = xcalloc(1, sz);

        if (!ptr->nd_l) {
                ptr->slabs = xrealloc(ptr->slabs, (ptr->nd_t + 10) * sizeof(void*));
                ptr->nd_t += 10;
                ptr->nd_l += 10;

                void** slabs = ptr->slabs;
                while(*slabs)
                        slabs++;
                ptr->curr = slabs;
        }

        /* Adjust slab bookkeeping state */
        ptr->nd_l--;
        *ptr->curr = ret;
        ptr->curr++;
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
        if (ptr->nd_l == 9 &&
            ptr->nd_t == 10 &&
            ptr->slabs &&
            pg &&
            *ptr->slabs == pg &&
            ptr->curr &&
            ptr->curr == (ptr->slabs + 1))
                sum += 1;

        /* Test second allocation, but above page size */
        pg = alloc_slab(ptr, PAGE_SIZE << 1);
        if (ptr->nd_l == 8 &&
            ptr->nd_t == 10 &&
            (ptr->slabs + 1) &&
            pg &&
            *(ptr->slabs + 1) == pg &&
            ptr->curr &&
            ptr->curr == (ptr->slabs + 2))
                sum += 1;

        return (sum == 2) ? 1 : 0;
}

void
free_slab(struct slabs* restrict ptr, void* slab)
{
        void** slabs = ptr->slabs;
        while (*slabs && *slabs != slab)
                slabs++;

        if (!*slabs)
                return;

        free(*slabs);
        ptr->nd_l += 1;

        void** nxt = ++slabs;
        slabs--;
        while ((*slabs++ = *nxt++));
}

int
test_free_slab(void)
{
        struct slabs* ptr = init_slabs();
        void* pg = alloc_slab(ptr, PAGE_SIZE);
        void* pg2 = alloc_slab(ptr, PAGE_SIZE);

        free_slab(ptr, pg);
        if (*ptr->slabs == pg2 &&
            *(ptr->slabs + 1) == 0x0 &&
            ptr->nd_t == 10 &&
            ptr->nd_l == 9)
                return 1;
        else
                return 0;
}

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

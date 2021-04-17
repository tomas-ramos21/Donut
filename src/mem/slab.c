#include "mem/slab.h"
#include "mem/alloc.h"
#include "inttypes.h"

struct slab {
        uint64_t mem;  /* Total memory  */
        uint32_t nd_t; /* Total nodes   */
        uint32_t nd_l; /* Nodes left    */
        void** slabs;  /* List of slabs */
        void** curr;   /* Current slab  */
};


struct slab*
init_slab(void)
{
        return xcalloc(1, sizeof(struct slab));
}

int
test_init_slab(void)
{
        struct slab* p = init_slab();
        return (!p->mem && !p->nd_t && !p->nd_l && !p->slabs && !p->curr) ? 1: 0;
}

void*
alloc_slab(struct slab* ptr, size_t slab_sz)
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
        ptr->mem += sz;
        *ptr->curr = ret;
        ptr->curr++;
        return ret;
}

int
test_alloc_slab(void)
{
        struct slab* ptr;
        void* pg;
        int sum = 0;

        /* Test Allocation below page size */
        ptr = init_slab();
        pg = alloc_slab(ptr, PAGE_SIZE / 2);
        if (ptr->mem == PAGE_SIZE &&
            ptr->nd_l == 9 &&
            ptr->nd_t == 10 &&
            ptr->slabs &&
            pg &&
            *ptr->slabs == pg &&
            ptr->curr &&
            ptr->curr == (ptr->slabs + sizeof(void)))
                sum += 1;

        /* Test second allocation, but above page size */
        pg = alloc_slab(ptr, PAGE_SIZE * 2);
        if (ptr->mem == (PAGE_SIZE * 3) &&
            ptr->nd_l == 8 &&
            ptr->nd_t == 10 &&
            (ptr->slabs + sizeof(void)) &&
            pg &&
            *(ptr->slabs + sizeof(void)) == pg &&
            ptr->curr &&
            ptr->curr == (ptr->slabs + sizeof(void) * 2))
                sum += 1;

        return (sum == 2) ? 1 : 0;
}

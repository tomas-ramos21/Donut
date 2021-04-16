#include "mem/slab.h"
#include "mem/alloc.h"
#include "inttypes.h"

struct slab {
        uint32_t mem_t;   /* Total memory    */
        uint32_t nr;      /* Node Count      */
        void** slabs;     /* List of slabs   */
};


struct slab*
new_slab(void)
{
        return xcalloc(1, sizeof(struct slab));
}

void*
slab_alloc(struct slab* ptr, size_t slab_sz)
{
        size_t sz = (slab_sz < PAGE_SIZE) ? PAGE_SIZE : slab_sz;
        void* ret = xcalloc(1, slab_sz);

        /* Adjust slab bookkeeping state */
        ptr->nr++;
        ptr->mem_t += sz;
        *ptr->slabs++ = ret;

        return ret;
}

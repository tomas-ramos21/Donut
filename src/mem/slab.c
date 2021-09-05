#include "mem/slab.h"
#include "core/wrappers.h"

union donut_objects {
        struct dataframe dataframe;
        struct data_file file;
        struct node node;
};

struct slab {
        uint32_t slab_t; /**< Total amount of slabs                 */
        uint32_t slab_l; /**< Amount of slabs left in the allocator */
        uint64_t idx;    /**< Index of the next slab                */
        void** origs;    /**< List of aligned slabs                 */
        void** slabs;    /**< List of original unaligned slabs      */
};

struct slab*
init_slabs(void)
{
        struct slab* ret = xcalloc(1, sizeof(struct slab));
        return ret;
}

int
test_init_slabs(void)
{
        int ret = 0;
        struct slab* slab = init_slabs();

        ret |= (slab->slab_t == 0) ? 1 : 0;
        ret &= (slab->slab_l == 0) ? 1 : 0;
        ret &= (slab->idx == 0) ? 1 : 0;
        ret &= (slab->origs == 0) ? 1 : 0;
        ret &= (slab->slabs == 0) ? 1 : 0;

        return ret;
}

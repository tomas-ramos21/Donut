#include "core/data-list.h"
#include "const/const.h"
#include "stdio.h"
#include "string.h"

#define GROWTH_FACTOR PAGE_SIZE / __SIZEOF_POINTER__
#define ELEM_PER_PG PAGE_SIZE / DATA_FILE_NAME_SIZE

struct data_list {
        uint32_t idx;        /**< Next index to be populated */
        uint32_t pg_cnt;     /**< Current number of first level pages */
        uint32_t elem_l;     /**< Elements lefts */
        uint32_t elem_t;     /**< Maximum array capacity */
        void** pgs;          /**< Allocated Pages */
        struct slabs* slabs; /**< Slab Allocator */
};

struct data_list*
init_data_list(struct slabs* slabs)
{
        struct data_list* ret = calloc(1, sizeof(struct data_list));
        ret->slabs = slabs;
        return ret;
}

int
test_data_list_init(void)
{
        int ret = 0;
        struct slabs* slabs = init_slabs();
        struct data_list* buf = init_data_list(slabs);
        ret |= (buf->idx == 0) ? 1 : 0;
        ret &= (buf->pg_cnt == 0) ? 1 : 0;
        ret &= (buf->elem_l == 0) ? 1 : 0;
        ret &= (buf->elem_t == 0) ? 1 : 0;
        ret &= (buf->pgs == 0x0) ? 1 : 0;
        ret &= (buf->slabs != 0x0) ? 1 : 0;
        return ret;
}

inline static void
grow_allocation(struct data_list* restrict data)
{
        /* Grow number of pages suppported */
        uint32_t new_pgs = data->pg_cnt + GROWTH_FACTOR;
        void** tmp = alloc_slab(data->slabs, new_pgs * __SIZEOF_POINTER__);
        memcpy(tmp, data->pgs, data->pg_cnt * __SIZEOF_POINTER__);

        /* Allocate new pages */
        while (data->pg_cnt < new_pgs)
                tmp[data->pg_cnt++] = alloc_slab(data->slabs, PAGE_SIZE);

        /* Update bookkeeping state */
        free_slab(data->slabs, data->pgs);
        data->pgs = (void**)tmp;
        data->elem_l += (GROWTH_FACTOR * ELEM_PER_PG);
        data->elem_t += (GROWTH_FACTOR * ELEM_PER_PG);
}

void
add_file_to_list(struct data_list* restrict data, char* f_name)
{
        if (!data->elem_l)
                grow_allocation(data);

        /* Get relative page & element index */
        uint64_t p_idx = (data->idx / ELEM_PER_PG) + (data->idx % ELEM_PER_PG) ? 1 : 0;
        uint64_t e_idx = (data->idx  / p_idx) + (data->idx % p_idx) ? 1 : 0;
        char* elem = ((char*)(data->pgs[p_idx])) + (e_idx * DATA_FILE_NAME_SIZE);

        data->idx++;
        data->elem_l--;
        strncpy(elem, f_name, 31);
}

int
test_add_file_to_data_list(void)
{
        int ret = 0;
        char* test = "123456789";
        struct slabs* slabs = init_slabs();
        struct data_list* buf = init_data_list(slabs);

        /* Add first item to the list */
        add_file_to_list(buf, test);
        ret |= (buf->elem_t == (GROWTH_FACTOR * ELEM_PER_PG)) ? 1 : 0;
        ret &= (buf->elem_l == ((GROWTH_FACTOR * ELEM_PER_PG)) - 1) ? 1 : 0;
        ret &= (buf->idx == 1) ? 1 : 0;
        ret &= (buf->pg_cnt == 512) ? 1 : 0;

        /* Check first item content is correct and starts at the right index */
        char* data = buf->pgs[0];
        ret &= !strncmp(data, test, 31);

        /* Add second item to the list */
        add_file_to_list(buf, test);
        ret &= (buf->elem_t == (GROWTH_FACTOR * ELEM_PER_PG)) ? 1 : 0;
        ret &= (buf->elem_l == ((GROWTH_FACTOR * ELEM_PER_PG)) - 2) ? 1 : 0;
        ret &= (buf->idx == 2) ? 1 : 0;
        ret &= (buf->pg_cnt == 512) ? 1 : 0;

        /* Check second item content is correct and starts at the right index */
        data = ((char*)buf->pgs[0]) + 1 * 32;
        ret &= !strncmp(data, test, 31);

        return ret;
}

#include "core/data-list.h"
#include "const/const.h"
#include "stdio.h"
#include "string.h"

#define GROWTH_FACTOR PAGE_SIZE / __SIZEOF_POINTER__
#define ELEM_PER_PG PAGE_SIZE / DATA_FILE_NAME_SIZE

struct data_list {
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
        ret |= buf->elem_t;
        ret |= buf->elem_l;
        ret |= *((uint8_t*)*(buf->pgs));
        ret |= !((uintptr_t)buf->slabs);
        return !ret;
}

inline static void
grow_allocation(struct data_list* restrict data)
{
        /* Grow number of pages suppported */
        uint32_t cur_pgs = data->elem_t / ELEM_PER_PG;
        uint32_t new_pgs = (cur_pgs + GROWTH_FACTOR);
        void** tmp = alloc_slab(data->slabs, new_pgs * __SIZEOF_POINTER__);
        memcpy(tmp, data->pgs, cur_pgs * __SIZEOF_POINTER__);

        /* Allocate new pages */
        while (cur_pgs < new_pgs)
                tmp[cur_pgs++] = alloc_slab(data->slabs, PAGE_SIZE);

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

        /* Get page & element index relative to the page */
        uint64_t idx = data->elem_t - data->elem_l;
        printf("Index: %llu\n", idx);
        uint64_t p_idx = (idx / ELEM_PER_PG) + (idx % ELEM_PER_PG) ? 1 : 0;
        printf("Page Index: %llu\n", p_idx);
        uint64_t e_idx = (idx  / p_idx) + (idx % p_idx) ? 1 : 0;
        printf("Element Index: %llu\n", e_idx);
        char* elem = ((char*)(data->pgs[p_idx])) + (e_idx * DATA_FILE_NAME_SIZE);

        strncpy(elem, f_name, 31);
        data->elem_l--;
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
        ret |= (buf->elem_l == ((GROWTH_FACTOR * ELEM_PER_PG)) - 1) ? 1 : 0;

        /* Check if the first item content is correct */
        char* data = buf->pgs[0];
        printf("Pointer: %p\n", data);
        printf("First Element: %s\n", data);

        /* Add second item to the list */
        add_file_to_list(buf, test);
        ret |= (buf->elem_t == (GROWTH_FACTOR * ELEM_PER_PG)) ? 1 : 0;
        ret |= (buf->elem_l == ((GROWTH_FACTOR * ELEM_PER_PG)) - 2) ? 1 : 0;

        /* Check if the second item content is correct */
        data = ((char*)buf->pgs[0]) + 1 * 32;
        printf("Pointer: %p\n", data);
        printf("First Element: %s\n", data);

        return ret;
}

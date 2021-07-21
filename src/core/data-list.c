#include "core/data-list.h"
#include "const/const.h"
#include "stdio.h"
#include "string.h"

#define GROWTH_FACTOR PAGE_SIZE / __SIZEOF_POINTER__
#define ELEMENTS_PER_PAGE DATA_FILE_NAME_SIZE / PAGE_SIZE

struct data_list {
        uint32_t elem_l;      /**< Elements lefts */
        uint32_t elem_t;      /**< Maximum array capacity */
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
        ret |= buf->e_idx;
        ret |= buf->e_tot;
        ret |= *((uint8_t*)*(buf->pgs));
        ret |= !((uintptr_t)buf->slabs);
        return !ret;
}

inline static void
grow_pages_allocation(struct data_list* restrict data)
{
        uint32_t pgs_t = PAGE_SIZE / (data->elem_t * DATA_FILE_NAME_SIZE);
        void** tmp = alloc_slab(data->slabs, (pgs_t + GROWTH_FACTOR) * __SIZEOF_POINTER__);
        memcpy(tmp, data->pgs, pgs_t * __SIZEOF_POINTER__);

        /* Adjust Bookkeeping Pointers */
        data->p_cur = &tmp[data->p_tot];
        free_slab(data->slabs, data->pgs);
        data->pgs = (void**)tmp;

        /* Update bookkeeping state  */
        data->p_tot += GROWTH_FACTOR;
        data->p_cnt += GROWTH_FACTOR;

        /* Allocate new pages */
        tmp = data->p_cur;
}

void
add_file_to_list(struct data_list* restrict data, char* f_name)
{
        if (!data->elem_l)
                grow_pages_allocation(data);

        memcpy(data->e_cur, f_name, 31);
        data->e_cur += 32;
        data->e_cnt--;
}

int
test_add_file_to_data_list(void)
{
        int ret = 0;
        char* test = "123456789";
        struct slabs* slabs = init_slabs();
        struct data_list* buf = init_data_list(slabs);
        add_file_to_list(buf, test);
        printf("Page Count: %d\n", buf->p_tot);
        printf("Pages Left: %d\n", buf->p_cnt);
        printf("Elements Left: %d\n", buf->e_cnt);
        printf("Next Free element: %p\n", (void*)buf->e_cur);
        printf("Next Free Page: %p\n", (void*)buf->p_cur);
        printf("First Page: %p\n", (void*)buf->pgs);
        return ret;
}

#include "core/data-list.h"
#include "stdio.h"
#include "string.h"

#define GROWTH_FACTOR PAGE_SIZE / __SIZEOF_POINTER__
#define HASH_SIZE 32

struct data_list {
        uint32_t e_cnt;      /**< Elements left in the current allocation */
        uint32_t p_cnt;      /**< Pages in the current allocation */
        uint32_t p_tot;      /**< Total pages allocated */
        struct slabs* slabs; /**< Slab Allocator */
        char* e_cur;         /**< Next Free Element */
        void** p_cur;        /**< Next Free Page */
        void** pgs;          /**< Allocated Pages */
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
        ret |= buf->e_cnt;
        ret |= buf->p_cnt;
        ret |= buf->p_tot;
        ret |= *((uint8_t*)*(buf->pgs));
        ret |= !((uintptr_t)buf->slabs);
        ret |= ((uintptr_t)buf->e_cur);
        ret |= ((uintptr_t)buf->p_cur);
        return !ret;
}

inline static void
grow_pages_allocation(struct data_list* restrict data)
{
        uint64_t* tmp = alloc_slab(data->slabs, (data->p_tot + GROWTH_FACTOR) * __SIZEOF_POINTER__);
        memcpy(tmp, data->pgs, data->p_tot * __SIZEOF_POINTER__);

        /* Allocate new pages */
        data->p_cur = tmp + data->p_tot;
        free_slab(data->slabs, data->pgs);
        data->pgs = (void**)tmp;
        while (tmp) {
                *tmp = (uintptr_t) alloc_slab(data->slabs, PAGE_SIZE);
                tmp++;
        }

        /* Update bookkeeping state  */
        data->p_tot += GROWTH_FACTOR;
        data->p_cnt += GROWTH_FACTOR;
        data->pgs = (void**)tmp;
}

void
add_file_to_list(struct data_list* restrict data, char* f_name)
{
        if (!data->e_cnt) {

                if (!data->p_cnt)
                        grow_pages_allocation(data);

                data->e_cur = *(data->p_cur);
                data->e_cnt = PAGE_SIZE / HASH_SIZE;
                data->p_cur++;
                data->p_cnt--;
        }

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

#include "core/data-list.h"
#include "const/const.h"
#include "misc/decorations.h"
#include "sys/stat.h"
#include "core/wrappers.h"
#include "dirent.h"
#include "stdio.h"
#include "string.h"

#define GROWTH_FACTOR ((PAGE_SIZE) / (__SIZEOF_POINTER__))
#define ELEM_PER_PG ((PAGE_SIZE) / (DATA_FILE_NAME_SIZE))

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

char*
get_data_list_index(struct data_list* restrict data, uint32_t idx)
{
        char* ret;
        uint32_t index = idx + 1;
        uint32_t p_idx = (index / ELEM_PER_PG) + (index % ELEM_PER_PG) ? 1 : 0;
        uint32_t e_idx = idx - ((p_idx - 1) * ELEM_PER_PG);

        if (p_idx >= data->pg_cnt || e_idx >= ELEM_PER_PG) {
                printf(DONUT_ERROR "Data-List index out of bounds.\n");
                exit(1);
        }

        ret = ((char*)(data->pgs[--p_idx])) + (e_idx * DATA_FILE_NAME_SIZE);
        return ret;
}

int
test_get_data_list_index()
{
        int ret = 1;
        void* get;
        char* test = "123456789";
        struct slabs* slabs = init_slabs();
        struct data_list* buf = init_data_list(slabs);

        /* Add 1 element to trigger array growth */
        add_file_to_list(buf, test);

        /* Check if all indices of the first page are in the expected locations */
        for (uint32_t i = 0; i < ELEM_PER_PG; i++) {
                get = get_data_list_index(buf, i);
                ret &= (get == ((char*)buf->pgs[0] + (i * DATA_FILE_NAME_SIZE))) ? 1 : 0;
        }

        return ret;
}

void
add_file_to_list(struct data_list* restrict data, char* f_name)
{
        if (!data->elem_l) {

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

        /* Get relative page & element index */
        char* elem = get_data_list_index(data, data->idx);

        data->idx++;
        data->elem_l--;
        strncpy(elem, f_name, DATA_FILE_NAME_SIZE);
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
        ret &= (buf->pg_cnt == GROWTH_FACTOR) ? 1 : 0;

        /* Check first item content is correct and starts at the right index */
        char* data = buf->pgs[0];
        ret &= !strncmp(data, test, 31);

        /* Add second item to the list */
        add_file_to_list(buf, test);
        ret &= (buf->elem_t == (GROWTH_FACTOR * ELEM_PER_PG)) ? 1 : 0;
        ret &= (buf->elem_l == ((GROWTH_FACTOR * ELEM_PER_PG)) - 2) ? 1 : 0;
        ret &= (buf->idx == 2) ? 1 : 0;
        ret &= (buf->pg_cnt == GROWTH_FACTOR) ? 1 : 0;

        /* Check second item content is correct and starts at the right index */
        data = ((char*)buf->pgs[0]) + 1 * 32;
        ret &= !strncmp(data, test, 31);

        return ret;
}

int
is_in_data_list(struct data_list* restrict list, char* str)
{
        int ret = 0;
        char* base_addr;
        uint32_t i, j;

        for (i = 0; i < list->pg_cnt ; i++) {
                base_addr = list->pgs[i];

                if (ret || *base_addr == 0x0)
                        return ret;

                j = 0;
                while (j < ELEM_PER_PG && !ret)
                        ret |= !strncmp(base_addr + (j++ * DATA_FILE_NAME_SIZE),
                                        str, DATA_FILE_NAME_SIZE);
        }

        return ret;
}

void
get_repo_data_list(struct data_list* list, char* path)
{
        struct dirent* entry;
        DIR* dir = xopendir(path);

        while ((entry = readdir(dir))) {
                if (entry->d_type != DT_REG)
                        continue;

                add_file_to_list(list, entry->d_name);
        }

        xclosedir(dir);
}

#include "io/fio.h"
#include "dirent.h"
#include "mem/slab.h"
#include "const/const.h"
#include "io/fio-utils.h"

/* TODO: Finish implementing "get_repo_data_list" */
/* TODO: Implement data files data structure */

void**
get_repo_data_list(struct slabs* slabs, void** pgs)
{
        DIR* dir = xopendir(DATA_FOLDER_RELATIVE);
        struct dirent* entry;
        void** ret = 0x0;

        while ((entry = readdir(dir))) {
                if (entry->d_type == DT_REG)
                        *ret = entry->d_name;
        }

        return ret;
}

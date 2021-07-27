#include "io/fio.h"
#include "dirent.h"
#include "mem/slab.h"
#include "string.h"
#include "sys/stat.h"
#include "stdio.h"
#include "const/const.h"
#include "io/fio-utils.h"

/* TODO: Finish implementing "get_repo_data_list" */
/* TODO: Implement data files data structure */

void
get_repo_data_list(struct slabs* slabs, void** pgs, struct data_list* list)
{
        struct stat st;
        struct dirent* entry;

        DIR* dir = xopendir(DATA_FOLDER_RELATIVE);

        while ((entry = readdir(dir))) {
                if (!strncmp(entry->d_name, ".", 2) ||
                    !strncmp(entry->d_name, "..", 2))
                        continue;

                add_file_to_list(list, entry->d_name);
        }
}

#include "io/fio.h"
#include "dirent.h"
#include "mem/slab.h"
#include "string.h"
#include "sys/stat.h"
#include "stdio.h"
#include "const/const.h"
#include "misc/decorations.h"
#include "io/fio-utils.h"
#include "unistd.h"
#include "stdio.h"

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
}

char*
xgetcwd(char* buf, size_t size)
{
        char* ret = getcwd(buf, size);

        if (!ret) {
                printf(DONUT_ERROR "Failed obtaining CWD.\n");
                exit(1);
        }

        return ret;

}

int
test_xgetcwd(void)
{
        int ret = 0;
        char* buf = malloc(PAGE_SIZE);

        buf = getcwd(buf, PAGE_SIZE);
        if (buf) {
                ret = 1;
                goto cleanup_return;
        }

cleanup_return:
        free(buf);
        return ret;
}

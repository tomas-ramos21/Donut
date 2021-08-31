#include "stdio.h"
#include "string.h"
#include "core/wrappers.h"
#include "cli/cmd.h"
#include "sys/stat.h"
#include "mem/slab.h"
#include "const/err.h"
#include "const/const.h"
#include "misc/decorations.h"
#include "tools/validation.h"


int
ls_data(const int argc, char** argv, int arg_idx, char* opts,
        uint64_t oflags)
{
        if (validate_donut_repo()) {
                printf(DONUT_ERROR "Donut isn't initialized or no path/file was\
 given. Try running \"donut init\" or check your arguments.\n");
                return DEF_ERR;
        }

        struct stat f;
        struct dirent* entry;
        struct slabs* slabs = init_slabs();
        char* cwd = alloc_slab(slabs, PAGE_SIZE);
        cwd = xgetcwd(cwd, PAGE_SIZE);
        strncat(cwd, DATA_FOLDER, 14);
        char* df_name = opts + (NAME_ARG_IDX * MAX_ARG_SZ);

        if (*df_name && strncmp(DEFAULT_DF, df_name, 4)) {
                strncat(cwd, df_name, strnlen(df_name, MAX_ARG_SZ));
                strncat(cwd, "/", 2);
        } else {
                df_name = "main";
        }

        DIR* dir = xopendir(cwd);
        size_t n_len, len = strnlen(cwd, PAGE_SIZE);
        while ((entry = readdir(dir))) {
                if (entry->d_type != DT_REG)
                        continue;

                n_len = PAGE_SIZE - strlen(cwd);
                strncat(cwd, entry->d_name, n_len);
                stat(cwd, &f);
                printf("%s\t%19li\t%32s\n", df_name, f.st_size, entry->d_name);
                memset(cwd + len, 0x0, n_len);
        }

        xclosedir(dir);
        clear_slabs(slabs);
        return 0;
}

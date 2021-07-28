#include "cli/cmd.h"
#include "stdio.h"
#include "const/const.h"
#include "misc/decorations.h"
#include "mem/slab.h"
#include "crypto/sha2.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "unistd.h"
#include "dirent.h"
#include "const/err.h"
#include "io/fio.h"
#include "io/fio-utils.h"
#include "core/data-list.h"
#include "string.h"

#define CTOR_MODE S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH

/*
 * TODO: Add more conditions to "validate_init" in the future.
 * TODO: Implement "chkin_dir"
 * TODO: Implement "transfer_large_file" option
 * TODO: Implement "missing_file_transfer"
 */

/**
  * Determines if there is donut is setup correctly in the current path.
  * @returns If donut is initialized 1 is returned otherwise 0
  */
static int
validate_init(void)
{
        int ret;
        struct stat info = {0};

        ret = stat(DONUT_FOLDER_RELATIVE, &info);
        ret &= stat(DATA_FOLDER_RELATIVE, &info);

        return ret;
}

static int
chkin_dir(const char* src)
{
        printf(DONUT "Is a directory.\n");
        return 0;
}


static int
chkin_file(const char* src, struct stat* f, char* df_name)
{
        int src_fd;
        off_t bytes;

        /* Get memory */
        struct slabs* slabs = init_slabs();
        char* cwd = alloc_slab(slabs, PAGE_SIZE);
        void* hash = alloc_slab(slabs, PAGE_SIZE);
        void* buf = alloc_slab(slabs, PAGE_SIZE);
        uint8_t* str = ((uint8_t*)hash + SHA_STRUCT_SZ);

        // TODO: Implement "xgetcwd"
        /* Build CWD & open file */
        cwd = getcwd(cwd, PAGE_SIZE);
        strncat(cwd, DATA_FOLDER, 13);
        if (*df_name) {
                strncat(cwd, df_name, strnlen(df_name, MAX_ARG_SZ));
                strncat(cwd, "/", 1);
                xmkdir(cwd, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        }
        src_fd = xopen(src, O_RDONLY);

        /* Compute SHA-2 */
        sha2_init(hash);
        do {
                bytes = xread(src_fd, buf, PAGE_SIZE);
                sha2_update(buf, str, hash, bytes);
                memset(buf, 0x0, bytes);
        } while (bytes);
        sha2_to_strn(str, (char*)(str + SHA_BLK_SZ), DATA_FILE_NAME_SIZE);
        str += SHA_BLK_SZ;

        /* Get existing files & add file if it's not present */
        struct data_list* list = init_data_list(slabs);
        get_repo_data_list(slabs, 0x0, list, cwd);

        if (!is_in_data_list(list, (char*)str)) {
                strncat(cwd, (char*)str, DATA_FILE_NAME_SIZE);
                xrename(src, cwd);
                xchmod(cwd, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        }

        /* Release resources */
        xclose(src_fd);
        clear_slabs(slabs);
        return 0;
}

int
chkin(const int argc, char** argv, int arg_idx, char* opts, uint64_t oflags)
{
        char* src;
        char* df_name = opts + (NAME_ARG_IDX * MAX_ARG_SZ);
        register int ret;
        register mode_t f_tp;
        struct stat f = {0};

        if (validate_init() || !(argc - 2)) {
                printf(DONUT_ERROR "Donut isn't initialized or no path/file was\
 given. Try running \"donut init\" or check your arguments.\n");
                return DEF_ERR;
        }

        src = argv[arg_idx];
        if (stat(argv[arg_idx], &f)) {
                printf(DONUT_ERROR "Path provided is invalid.\n");
                return DEF_ERR;
        }

        f_tp = f.st_mode;
        if (f_tp & S_IFDIR)
                ret = chkin_dir(src);
        else if (f_tp & S_IFREG)
                ret = chkin_file(src, &f, df_name);
        else {
                printf(DONUT_ERROR "Path given is not a directory or regular file.\n");
                ret = DEF_ERR;
        }

        return ret;
}

#include "cmd/cmd.h"
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
#include "string.h"

#define LARGE_FILE 52428800


/*
 * TODO: Add more conditions to "validate_init" in the future.
 * TODO: Implement "chkin_dir"
 * TODO: Implement "transfer_large_file"
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
        ret = ret | stat(DATA_FOLDER_RELATIVE, &info);

        return ret;
}

static int
chkin_dir(const char* src)
{
        printf(DONUT "Is a directory.\n");
        return 0;
}

static int
transfer_large_file(struct slabs* slabs, int src_fd, int dst_fd)
{
        printf(DONUT "Called Large transfer.");
        return 0;
}

static int
chkin_file(const char* src, struct stat* f)
{
        int dir_fd, fd, is_xl;
        off_t f_sz = f->st_size;
        is_xl = (f_sz > LARGE_FILE) ? 1 : 0;

        /* Get memory */
        struct slabs* slabs = init_slabs();
        char* cwd = alloc_slab(slabs, PAGE_SIZE);

        /* Open CWD */
        cwd = getcwd(cwd, PAGE_SIZE);
        cwd = strncat(cwd, DATA_FOLDER, 12);
        dir_fd = xopen(cwd, O_RDONLY);

        /* Open file */
        f_sz = f->st_size;
        fd = xopen(src, O_RDONLY);
        if (is_xl) {
                transfer_large_file(slabs, fd, dir_fd);
        } else {
                printf(DONUT "Basic file transfer.\n");
                void* pg = alloc_slab(slabs, f_sz);
                uint8_t* hash = alloc_slab(slabs, PAGE_SIZE);
                void* hx = alloc_slab(slabs, PAGE_SIZE);
                char* str = alloc_slab(slabs, PAGE_SIZE);
                f_sz = xread(fd, pg, f_sz);
                sha2_hash(pg, hash, hx, f_sz);
                sha2_to_str(hash, str);
                printf("Hash: %s\n", str);
        }

        /* Release resources */
        xclose(dir_fd);
        xclose(fd);
        clear_slabs(slabs);
        return 0;
}

int
chkin(const int argc, const struct parsed_args* args)
{
        char* src;
        register int ret;
        register mode_t f_tp;
        struct stat f = {0};

        if (validate_init() || !argc) {
                printf(DONUT_ERROR "Donut isn't initialized or no path/file was\
 given. Try running \"donut init\" or check your arguments.\n");
                return DEF_ERR;
        }

        if (stat(args->args, &f)) {
                printf(DONUT_ERROR "Path provided is invalid.\n");
                return DEF_ERR;
        }

        src = args->args;
        f_tp = f.st_mode;
        if (f_tp & S_IFDIR)
                ret = chkin_dir(src);
        else if (f_tp & S_IFREG)
                ret = chkin_file(src, &f);
        else {
                printf(DONUT_ERROR "Path given is not a directory or regular file.\n");
                ret = DEF_ERR;
        }

        return ret;
}

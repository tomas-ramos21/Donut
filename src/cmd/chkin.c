#include "cmd/cmd.h"
#include "stdio.h"
#include "const/const.h"
#include "misc/decorations.h"
#include "mem/slab.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "unistd.h"
#include "dirent.h"
#include "const/err.h"
#include "io/fio.h"

#define LARGE_FILE 52428800

/*
 * TODO: Add more conditions in the future if needed.
 * Check if the PWD has a valid initialized Donut directory.
 */
static int
validate_init(void)
{
        int ret;
        struct stat info = {0};

        ret = stat(DONUT_FOLDER, &info);
        ret = ret | stat(DATA_FOLDER, &info);

        return ret;
}

/*
 * TODO: Implement
 */
static int
chkin_dir(const char* src)
{
        printf(DONUT "Is a directory.\n");
        return 0;
}

/*
 * TODO: Implement
 */
static int
transfer_large_file(struct slabs* slabs, int src_fd, int dst_fd)
{
        printf(DONUT "Called Large transfer.");
        return 0;
}

/*
 * TODO: Implement missing file transfers
 */
static int
chkin_file(const char* src, struct stat* f)
{
        int dir_fd, fd, is_xl;
        off_t f_sz = f->st_size;
        is_xl = (f_sz < LARGE_FILE) ? 1 : 0;

        /* Get memmory */
        struct slabs* slabs = init_slabs();
        char* cwd = alloc_slab(slabs, PAGE_SIZE);

        /* Open CWD */
        cwd = getcwd(cwd, PAGE_SIZE);
        dir_fd = xopen(cwd, O_RDONLY);

        /* Open file */
        f_sz = f->st_size;
        fd = xopen(src, O_RDONLY);
        if (is_xl) {
                printf(DONUT "Basic file transfer.\n");
        } else {
                transfer_large_file(slabs, fd, dir_fd);
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
                printf(DONUT "Donut isn't initialized or no path/file was given.\
 Try running \"donut init\" or check your arguments.\n");
                return DEF_ERR;
        }

        if (stat(args->args, &f)) {
                printf(DONUT "Path provided is invalid.\n");
                return DEF_ERR;
        }

        src = args->args;
        f_tp = f.st_mode;
        if (f_tp & S_IFDIR)
                ret = chkin_dir(src);
        else if (f_tp & S_IFREG)
                ret = chkin_file(src, &f);
        else {
                printf(DONUT "Path given is not a directory or regular file.\n");
                ret = DEF_ERR;
        }

        return ret;
}

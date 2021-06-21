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
chkin_file(const char* src, struct stat* f)
{
        off_t f_sz = f->st_size, bytes;
        int src_fd, i;
        int t_pgs = (f_sz / PAGE_SIZE) + !((f_sz % PAGE_SIZE) == 0);
        void* pgs[t_pgs];

        /* Get memory */
        struct slabs* slabs = init_slabs();
        char* cwd = alloc_slab(slabs, PAGE_SIZE);
        void* hash = alloc_slab(slabs, PAGE_SIZE);
        uint8_t* str = ((uint8_t*)hash + SHA_STRUCT_SZ);

        /* Get CWD & open file */
        cwd = getcwd(cwd, PAGE_SIZE);
        cwd = strncat(cwd, DATA_FOLDER, 12);
        src_fd = xopen(src, O_RDONLY);

        /* Read data & compute SHA-2 Simultaneously */
        i = 0;
        sha2_init(hash);
        while (t_pgs--) {
                pgs[i] = alloc_slab(slabs, PAGE_SIZE);
                bytes = xread(src_fd, pgs[i], PAGE_SIZE);
                sha2_update(pgs[i++], str, hash, bytes);
        }
        sha2_to_str(str, (char*)(str + SHA_BLK_SZ));
        str += SHA_BLK_SZ;
        printf("Hash: %s\n", str);

        /* Get list of existing hashes */


        /* Get Sha-2 */
        /* f_sz = f->st_size; */
        /* uint8_t* hash = alloc_slab(slabs, PAGE_SIZE); */
        /* void* hx = hash + 65; */
        /* void* str = hash + 65 + 365; */
        /* sha2_hash(pg, hash, hx, f_sz); */
        /* sha2_to_str(hash, str); */
        /* memset(str--, 0x0, 64); */
        /* *hash = '/'; */

        /* strncpy((char*)(hash + 1), (char*)(hash + 65 + 365), 32); */
        /* printf("Hash: %s\n", hash); */
        /* printf("CWD: %s\n", cwd); */
        /* char* name = strncat(cwd, hash, 32 + 1); */
        /* printf("File Path: %s\n", name); */
        /* dst_fd = xopen(name, O_RDWR | O_CREAT, 0755); */
        /* f_sz = xwrite(dst_fd, pg, f_sz); */

        /* Release resources */
        /* xclose(dir_fd); */
        /* xclose(src_fd); */
        /* xclose(dst_fd); */
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

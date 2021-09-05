#include "cli/cmd.h"
#include "stdio.h"
#include "const/const.h"
#include "misc/decorations.h"
#include "mem/slob.h"
#include "crypto/sha2.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "unistd.h"
#include "dirent.h"
#include "const/err.h"
#include "core/wrappers.h"
#include "core/data-list.h"
#include "tools/validation.h"
#include "string.h"

#define CTOR_MODE S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH

/*
 * TODO: Add more conditions to "validate_init" in the future.
 * TODO: Implement a multithread option in case of large amount of files
 */

inline static void
compute_file_sha2(int fd, void* hash, void* buf, uint8_t* str)
{
        off_t bytes;

        sha2_init(hash);
        do {
                bytes = xread(fd, buf, PAGE_SIZE);
                sha2_update(buf, str, hash, bytes);
                memset(buf, 0x0, bytes);
        } while (bytes);
}

static int
chkin_dir(const char* src, struct data_list* list, struct slobs* slobs,
          char* cwd, void* hash, void* buf, uint8_t* str)
{
        DIR* dir;
        int src_fd;
        char* f_name;
        size_t name_len;
        struct dirent* entry;

        /* Get Variables for Path Treatments */
        size_t src_len = strlen(src);
        size_t cwd_len = strnlen(cwd, PAGE_SIZE);
        char* src_cp = alloc_slob(slobs, PAGE_SIZE);

        /* Directory Path Treatment */
        strncpy(src_cp, src, src_len);
        if (src[src_len - 1] != '/')
                src_cp[src_len++] = '/';

        dir = xopendir(src_cp);
        while ((entry = readdir(dir))) {

                if (entry->d_type != DT_REG)
                        continue;

                /* Get File Path */
                f_name = entry->d_name;
                name_len = PAGE_SIZE - strlen(src_cp);
                strncat(src_cp, f_name, name_len);
                src_fd = xopen(src_cp, O_RDONLY);

                /* Read File & Compute Hash */
                compute_file_sha2(src_fd, hash, buf, str);
                sha2_to_strn(str, (char*)(str + SHA_BLK_SZ), DATA_FILE_NAME_SIZE - 1);
                str += SHA_BLK_SZ;

                /* Move File if it's not present */
                if (!is_in_data_list(list, (char*)str)) {
                        strncat(cwd, (char*)str, DATA_FILE_NAME_SIZE);
                        xrename(src_cp, cwd);
                        xchmod(cwd, S_IRUSR | S_IRGRP | S_IROTH);
                        add_file_to_list(list, (char*)str);
                        memset(cwd + cwd_len, 0x0, DATA_FILE_NAME_SIZE - 1);
                }

                /* Reset Variables */
                xclose(src_fd);
                memset(str, 0x0, DATA_FILE_NAME_SIZE - 1);
                memset(src_cp + src_len, 0x0, name_len);
                str -= SHA_BLK_SZ;
        }

        xclosedir(dir);
        return 0;
}


static int
chkin_file(const char* src, struct data_list* list, char* cwd, void* hash,
           void* buf, uint8_t* str)
{
        int src_fd = xopen(src, O_RDONLY);

        compute_file_sha2(src_fd, hash, buf, str);
        sha2_to_strn(str, (char*)(str + SHA_BLK_SZ), DATA_FILE_NAME_SIZE - 1);
        str += SHA_BLK_SZ;

        if (!is_in_data_list(list, (char*)str)) {
                strncat(cwd, (char*)str, DATA_FILE_NAME_SIZE);
                xrename(src, cwd);
                xchmod(cwd, S_IRUSR | S_IRGRP | S_IROTH);
        }

        xclose(src_fd);
        return 0;
}

int
chkin(const int argc, char** argv, int arg_idx, char* opts, uint64_t oflags)
{
        char* src;
        char* df_name = opts + (NAME_ARG_IDX * MAX_ARG_SZ);
        register int ret;
        register mode_t f_tp;
        struct stat f, dir;

        if (validate_donut_repo() || !(argc - 2)) {
                printf(DONUT_ERROR "Donut isn't initialized or no path/file was\
 given. Try running \"donut init\" or check your arguments.\n");
                return DEF_ERR;
        }

        src = argv[arg_idx];
        if (stat(argv[arg_idx], &f)) {
                printf(DONUT_ERROR "Path provided is invalid.\n");
                return DEF_ERR;
        }

        /* Get memory */
        struct slobs* slobs = init_slobs();
        char* cwd = alloc_slob(slobs, PAGE_SIZE);
        void* hash = alloc_slob(slobs, PAGE_SIZE);
        void* buf = alloc_slob(slobs, PAGE_SIZE);
        uint8_t* str = ((uint8_t*)hash + SHA_STRUCT_SZ);

        /* Build CWD & open file */
        cwd = xgetcwd(cwd, PAGE_SIZE);
        strncat(cwd, DATA_FOLDER, 14);
        if (*df_name && strncmp(DEFAULT_DF, df_name, 4)) {
                strncat(cwd, df_name, strnlen(df_name, MAX_ARG_SZ));

                if (stat(cwd, &dir))
                        xmkdir(cwd, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

                strncat(cwd, "/", 2);
        }

        /* Get data in the current Dataframe or General Repository */
        struct data_list* list = init_data_list(slobs);
        get_repo_data_list(list, cwd);

        f_tp = f.st_mode;
        if (f_tp & S_IFDIR)
                ret = chkin_dir(src, list, slobs, cwd, hash, buf, str);
        else if (f_tp & S_IFREG)
                ret = chkin_file(src, list, cwd, hash, buf, str);
        else {
                printf(DONUT_ERROR "Path given is not a directory or regular file.\n");
                ret = DEF_ERR;
        }

        clear_slobs(slobs);
        return ret;
}

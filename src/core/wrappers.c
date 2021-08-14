#include "core/wrappers.h"
#include "stdio.h"
#include "const/err.h"
#include "const/const.h"
#include "stdlib.h"
#include "misc/decorations.h"
#include "misc/colour.h"
#include "sys/errno.h"
#include "sys/stat.h"
#include "unistd.h"
#include "fcntl.h"
#include <sys/types.h>
#include <sys/wait.h>
#include "string.h"
#include "const/nix_cmds.h"
#include "stdarg.h"

/**
 * @file fio.c
 * Implementation of wrappers around the standard file I/O functions.
 */

#define REPETITIONS 10

int
xopen(const char* path, int oflag, ...)
{
        int fd, mode = 0;
        va_list ap;

        va_start(ap, oflag);
        if (oflag & O_CREAT)
                mode = va_arg(ap, int);
        va_end(ap);

        while (1) {

                fd = open(path, oflag, mode);
                if (fd > 0)
                        return fd;
                else if (errno == EINTR)
                        continue;

                printf(DONUT "Failed to open file: %s\n", path);
                exit(DEF_ERR);
        }
}

int
test_xopen(void)
{
        int ret;
        struct stat file;
        char* cwd = malloc(PAGE_SIZE);
        cwd = getcwd(cwd, PAGE_SIZE);
        cwd = strncat(cwd, "/", 1);
        cwd = strncat(cwd, TEST_FILE, 15);
        char* args[] = {TOUCH_PATH, TEST_FILE, NULL};
        pid_t child;

        if (!stat(cwd, &file))
                remove(cwd);

        /* Create a test file */
        if (!(child = fork())) {
                int fail = execv(TOUCH_PATH, args);
                fail = (fail == -1) ? 0 : 1;
                exit(fail);
        }

        waitpid(child, &ret, 0);
        if (ret != 0) {
                ret = 0;
                goto cleanup_return;
        }

        /* Should be successful - All conditions are satisfied */
        ret = xopen(cwd, O_RDWR);

cleanup_return:
        remove(cwd);
        free(cwd);
        return ret;
}

int
xclose(int fd)
{
        while (1) {
                int ret = close(fd);

                if (EBADF == errno)
                        printf(DONUT "Tried to close invalid file descriptor.\n");
                else if (EINTR == errno)
                        continue;

                return ret;
        }
}

int
test_xclose(void)
{
        int ret;
        struct stat file;
        char* cwd = malloc(PAGE_SIZE);
        cwd = getcwd(cwd, PAGE_SIZE);
        cwd = strncat(cwd, "/", 1);
        cwd = strncat(cwd, TEST_FILE, 15);

        if (!stat(cwd, &file))
                remove(cwd);

        ret = open(cwd, O_CREAT, 0640);
        if (ret < 0) {
                ret = 0;
                goto cleanup_return;
        }

        /* Close file and check if the descriptor is still accessible */
        xclose(ret);
        ret = fstat(ret, &file);
        if (!ret)
                goto cleanup_return;
        ret = 1;

cleanup_return:
        remove(cwd);
        free(cwd);
        return ret;
}

size_t
xread(int fd, void* buf, size_t nbyte)
{
        size_t bytes_read, acc = 0, bytes = nbyte;

        while (bytes) {
                bytes_read = read(fd, buf, bytes);

                if(errno == EINTR) {
                        continue;
                } else if (!bytes_read) {
                        return acc;
                } else if (bytes_read < 0) {
                        printf(DONUT "Failed reading from file with error: %lu.\n",
                               bytes_read);
                        exit(DEF_ERR);
                }

                acc += bytes_read;
                bytes = bytes - bytes_read;
        }
        return acc;
}

int
test_xread()
{
        int ret;
        size_t bytes = 20;
        struct stat file;
        char* str = "Do you like donuts?\n";
        char* buf = malloc(bytes);
        char* cwd = malloc(PAGE_SIZE);
        cwd = getcwd(cwd, PAGE_SIZE);
        cwd = strncat(cwd, "/", 1);
        cwd = strncat(cwd, TEST_FILE, 15);

        if (!stat(cwd, &file))
                remove(cwd);

        /* Create a File */
        ret = open(cwd, O_RDWR | O_CREAT, 0640);
        if (ret < 0) {
                ret = 0;
                goto cleanup_return;
        }

        /* Write into the file */
        dprintf(ret, "%s", str);
        lseek(ret, 0, SEEK_SET);
        ret = xread(ret, buf, bytes);

        /* Check if the written string matches */
        ret = (!strncmp(str, buf, bytes)) ? 1 : 0;

cleanup_return:
        remove(cwd);
        free(cwd);
        free(buf);
        return ret;
}

size_t
xpread(int fd, void* restrict buf, size_t nbyte, off_t offset)
{
        size_t bytes_read, acc = 0, bytes = nbyte;

        while (bytes) {
                bytes_read = pread(fd, buf, bytes, offset);

                if(errno == EINTR) {
                        continue;
                } else if (!bytes_read) {
                        return acc;
                } else if (bytes_read < 0) {
                        printf(DONUT "Failed reading from file with error: %lu.\n",
                               bytes_read);
                        exit(DEF_ERR);
                }

                offset = offset + bytes_read;
                acc += bytes_read;
                bytes = bytes - bytes_read;
        }
        return acc;
}

int
test_xpread()
{
        int ret;
        size_t bytes = 20;
        struct stat file;
        char* str = "Do you like donuts?\n";
        char* buf = malloc(bytes);
        char* cwd = malloc(PAGE_SIZE);
        cwd = getcwd(cwd, PAGE_SIZE);
        cwd = strncat(cwd, "/", 1);
        cwd = strncat(cwd, TEST_FILE, 15);

        if (!stat(cwd, &file))
                remove(cwd);

        /* Create a File */
        ret = open(cwd, O_RDWR | O_CREAT, 0640);
        if (ret < 0) {
                ret = 0;
                goto cleanup_return;
        }

        /* Write into the file */
        dprintf(ret, "%s", str);
        lseek(ret, 0, SEEK_SET);
        ret = xpread(ret, buf, bytes, 0);

        /* Check if string written matches */
        ret = (!strncmp(str, buf, bytes)) ? 1 : 0;

cleanup_return:
        remove(cwd);
        free(cwd);
        free(buf);
        return ret;
}

size_t
xwrite(int fd, void* buf, size_t nbyte)
{
        size_t written, bytes = nbyte;

        while (bytes) {
                written = write(fd, buf, bytes);

                if(errno == EINTR) {
                        continue;
                } else if ((long)written < 0) {
                        printf(DONUT "Failed writing to file with error: %lu.\n",
                               written);
                        exit(DEF_ERR);
                }

                bytes = bytes - written;
        }

        return nbyte;
}

int
test_xwrite(void)
{
        int ret;
        size_t bytes = 21;
        struct stat file = {0};
        char* str = "Do you like donuts?\n";
        char* cwd = malloc(PAGE_SIZE);
        cwd = getcwd(cwd, PAGE_SIZE);
        cwd = strncat(cwd, "/", 1);
        cwd = strncat(cwd, TEST_FILE, 15);

        if (!stat(cwd, &file))
                remove(cwd);

        /* Open or create file */
        ret = open(cwd, O_RDWR | O_CREAT, 0640);
        if (ret < 0) {
                ret = 0;
                goto cleanup_return;
        }

        /* Write into file */
        bytes = xwrite(ret, &str, bytes);

        /* Check if file size matches the amount of bytes written */
        fstat(ret, &file);
        if (!fstat(ret, &file))
                ret = (file.st_size == (long long)bytes) ? 1 : 0;
        else
                ret = 0;

cleanup_return:
        remove(cwd);
        free(cwd);
        return ret;
}

size_t
xpwrite(int fd, void* restrict buf, size_t nbyte, off_t offset)
{
        size_t written, bytes = nbyte;

        while (bytes) {
                written = pwrite(fd, buf, bytes, offset);

                if(errno == EINTR) {
                        continue;
                } else if ((long)written < 0) {
                        printf(DONUT "Failed writing to file with error: %lu.\n",
                               written);
                        exit(DEF_ERR);
                }

                offset = offset + written;
                bytes = bytes - written;
        }

        return nbyte;
}

int
test_xpwrite(void)
{
        int ret;
        size_t bytes = 21;
        struct stat file = {0};
        char* str = "Do you like donuts?\n";
        char* cwd = malloc(PAGE_SIZE);
        cwd = getcwd(cwd, PAGE_SIZE);
        cwd = strncat(cwd, "/", 1);
        cwd = strncat(cwd, TEST_FILE, 15);

        if (!stat(cwd, &file))
                remove(cwd);

        /* Open or create file */
        ret = open(cwd, O_RDWR | O_CREAT, 0640);
        if (ret < 0) {
                ret = 0;
                goto cleanup_return;
        }

        /* Write into file */
        bytes = xpwrite(ret, &str, bytes, 0);

        /* Check if file size matches the amount of bytes written */
        fstat(ret, &file);
        if (!fstat(ret, &file))
                ret = (file.st_size == (long long)bytes) ? 1 : 0;
        else
                ret = 0;

cleanup_return:
        remove(cwd);
        free(cwd);
        return ret;
}

DIR*
xopendir(const char* path)
{
        DIR* dir = opendir(path);

        if (!dir) {
                printf(RED DONUT RESET "Failed to open the directory.\n");
                exit(DEF_ERR);
        }

        return dir;
}

int
test_xopendir(void)
{
        const char* path = getenv("HOME");
        DIR* dir = opendir(path);
        int ret = (dir) ? 1 : 0;
        closedir(dir);
        return ret;
}

int
xclosedir(DIR* dir)
{
        int ret = closedir(dir);

        if (ret) {
                printf(RED DONUT RESET "Failed to close directory.\n");
                exit(DEF_ERR);
        }

        return ret;
}

int
test_xclosedir(void)
{
        const char* path = getenv("HOME");
        DIR* dir = opendir(path);
        int ret = (dir) ? 1 : 0;
        ret = xclosedir(dir);
        return !ret;
}

int
xmkdir(const char *pathname, mode_t mode)
{
        int ret = mkdir(pathname, mode);

        if (ret) {
                printf(DONUT_ERROR "Failed to create directory.\n");
                exit(DEF_ERR);
        }

        return ret;
}

int
test_xmkdir(void)
{
        int ret;
        const char* path = getenv("HOME");
        char* dir_name = calloc(1, PAGE_SIZE);

        strncpy(dir_name, path, PAGE_SIZE - 1);
        strncat(dir_name, "/test", 5);
        ret = !xmkdir(dir_name, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

        rmdir(dir_name);
        free(dir_name);
        return ret;
}

int
xrename(const char* old, const char* new)
{
        int ret = rename(old, new);

        if (ret) {
                printf(DONUT_ERROR "Failed to rename file.\n");
                exit(DEF_ERR);
        }

        return ret;
}

int
test_xrename(void)
{
        int ret = 0;
        char* old = calloc(1, PAGE_SIZE);
        char* new = calloc(1, PAGE_SIZE);
        const char* home = getenv("HOME");

        strncpy(old, home, PAGE_SIZE - 1);
        strncat(old, "/test1", 6);

        strncpy(new, home, PAGE_SIZE - 1);
        strncat(new, "/test2", 6);

        ret = open(old, O_RDWR | O_CREAT, 0640);
        if (ret < 0) {
                ret = 0;
                goto cleanup_return;
        }
        close(ret);

        ret = !xrename(old, new);

cleanup_return:
        remove(old);
        remove(new);
        free(old);
        free(new);
        return ret;
}

int
xchmod(const char* path, mode_t mode)
{
        int ret = chmod(path, mode);

        if (ret) {
                printf(DONUT_ERROR "Failed to change the permissions of the\
 file/directory.\n");
                exit(DEF_ERR);
        }

        return ret;
}

int
test_xchmod(void)
{
        int ret = 0;
        char* f = calloc(1, PAGE_SIZE);
        const char* home = getenv("HOME");

        strncpy(f, home, PAGE_SIZE - 1);
        strncat(f, "/test12345", 10);

        ret = open(f, O_RDWR | O_CREAT, 0640);
        if (ret < 0) {
                ret = 0;
                goto cleanup_return;
        }
        close(ret);

        ret = !xchmod(f, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

cleanup_return:
        remove(f);
        free(f);
        return ret;
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

void*
xmalloc(size_t sz)
{
        void* ret = malloc(sz);

        if (!ret) {
                printf(DONUT "Failed to allocate memory.\n");
                exit(ENOMEM);
        }

        return ret;
}

int
test_xmalloc(void)
{
        void* buff;
        int i, sum = 0;
        int res[REPETITIONS];

        for (i = 0; i < REPETITIONS; i++) {
                buff = xmalloc(PAGE_SIZE);

                if (buff)
                        res[i] = 1;

                free(buff);
        }

        for (i = 0; i < REPETITIONS; i++)
                sum += res[i];

        return (REPETITIONS == sum) ? 1 : 0;
}

void*
xcalloc(size_t n, size_t sz)
{
        void* ret = calloc(n, sz);

        if (!ret) {
                printf(DONUT "Failed to allocate memory.\n");
                exit(ENOMEM);
        }

        return ret;
}

int
test_xcalloc(void)
{
        void* buff;
        int i, sum = 0;
        int res[REPETITIONS];

        for (i = 0; i < REPETITIONS; i++) {
                buff = xcalloc(1, PAGE_SIZE);

                if (buff)
                        res[i] = 1;

                free(buff);
        }

        for (i = 0; i < REPETITIONS; i++)
                sum += res[i];

        return (REPETITIONS == sum) ? 1 : 0;
}

void*
xrealloc(void* buff, size_t sz)
{
        if (!buff)
                return xmalloc(sz);

        void* ret = realloc(buff, sz);
        if (!ret) {
                printf(DONUT "Failed to allocate memory.\n");
                exit(ENOMEM);
        }

        return ret;
}

int
test_xrealloc(void)
{
        void* buff;
        int i, sum;
        int res[REPETITIONS];

        for (i = 0; i < REPETITIONS; i++) {
                sum = 0;
                char* dt = malloc(PAGE_SIZE);
                memset(dt, 0x1, PAGE_SIZE);
                buff = xrealloc(dt, PAGE_SIZE + 1);

                char* buff_cp = buff;
                while (*buff_cp)
                        sum += *buff_cp++;

                if (buff && sum == PAGE_SIZE)
                        res[i] = 1;

                free(buff);
        }

        sum = 0;
        for (i = 0; i < REPETITIONS; i++)
                sum += res[i];

        return (REPETITIONS == sum) ? 1 : 0;
}

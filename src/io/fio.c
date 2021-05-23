#include "io/fio.h"
#include "stdio.h"
#include "const/err.h"
#include "stdlib.h"
#include "misc/decorations.h"
#include "sys/errno.h"
#include "sys/stat.h"
#include "unistd.h"
#include "string.h"

#define TOUCH_PATH "/usr/bin/touch"
#define TEST_FILE "donut_test.txt"


int
xopen(const char* path, int oflag)
{
        int fd;

        if (oflag < O_RDONLY || oflag > O_RDWR) {
                printf(DONUT "Invalid flags passed for file opening.\n");
                exit(DEF_ERR);
        }

        while (1) {

                fd = open(path, oflag);
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

        ret = open(cwd, O_CREAT);
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
        size_t bytes_read, bytes = nbyte;

        while (bytes) {
                bytes_read = read(fd, buf, bytes);

                if(errno == EINTR) {
                        continue;
                } else if (!bytes_read) {
                        return nbyte;
                } else if (bytes_read < 0) {
                        printf(DONUT "Failed reading from file with error: %lu.\n",
                               bytes_read);
                        exit(DEF_ERR);
                }

                bytes = bytes - bytes_read;
        }
        return nbyte;
}

int
test_xread()
{
        int ret;
        size_t bytes = 21;
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
        ret = open(cwd, O_RDWR | O_CREAT);
        if (ret < 0) {
                ret = 0;
                goto cleanup_return;
        }

        /* Write into the file */
        dprintf(ret, "%s", str);
        lseek(ret, 0, SEEK_SET);
        ret = xread(ret, buf, bytes);

        /* Check if string written matches */
        ret = (!strncmp(str, buf, bytes)) ? 1 : 0;

cleanup_return:
        remove(cwd);
        free(cwd);
        return ret;
}

size_t
xpread(int fd, void* restrict buf, size_t nbyte, off_t offset)
{
        size_t bytes_read, bytes = nbyte;

        while (bytes) {
                bytes_read = pread(fd, buf, bytes, offset);

                if(errno == EINTR) {
                        continue;
                } else if (!bytes_read) {
                        return nbyte;
                } else if (bytes_read < 0) {
                        printf(DONUT "Failed reading from file with error: %lu.\n",
                               bytes_read);
                        exit(DEF_ERR);
                }

                offset = offset + bytes_read;
                bytes = bytes - bytes_read;
        }
        return nbyte;
}

int
test_xpread()
{
        int ret;
        size_t bytes = 21;
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
        ret = open(cwd, O_RDWR | O_CREAT);
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
                        printf(DONUT "Failed writing to file with error: %ld.\n",
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
        struct stat file;
        char* str = "Do you like donuts?\n";
        char* cwd = malloc(PAGE_SIZE);
        cwd = getcwd(cwd, PAGE_SIZE);
        cwd = strncat(cwd, "/", 1);
        cwd = strncat(cwd, TEST_FILE, 15);

        if (!stat(cwd, &file))
                remove(cwd);

        /* Open or create file */
        ret = open(cwd, O_RDWR | O_CREAT);
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
                        printf(DONUT "Failed writing to file with error: %ld.\n",
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
        struct stat file;
        char* str = "Do you like donuts?\n";
        char* cwd = malloc(PAGE_SIZE);
        cwd = getcwd(cwd, PAGE_SIZE);
        cwd = strncat(cwd, "/", 1);
        cwd = strncat(cwd, TEST_FILE, 15);

        if (!stat(cwd, &file))
                remove(cwd);

        /* Open or create file */
        ret = open(cwd, O_RDWR | O_CREAT);
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

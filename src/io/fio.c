#include "io/fio.h"
#include "stdio.h"
#include "const/err.h"
#include "const/const.h"
#include "stdlib.h"
#include "misc/decorations.h"
#include "misc/colour.h"
#include "sys/errno.h"
#include "sys/stat.h"
#include "unistd.h"
#include "string.h"
#include "const/nix_cmds.h"
#include "stdarg.h"

/**
 * @file fio.c
 *
 * Implementation of wrappers around the standard file I/O functions.
 *
 * Provides a collection of "safer" file I/O functions which provide safeguard
 * towards common issues while using them.
 */

/**
 * Simple wrapper of the standard "open" function.
 *
 * Wrapper around the standard open function which check for the following
 * conditions:
 *   1. Verifies the flags passed are valid for file creation and opening
 *   2. Ignore interrupt signals while opening the file to prevent corruption
 *
 * @param path Path to a file
 * @param oflag Creation and status flags to be used
 * @returns Integer providing the file descriptor.
 */
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

/**
 * Collection of unit tests for xopen.
 *
 * Runs tests to check if the function is capable of opening a file.
 *
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
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

/**
 * Simple wrapper of the standard "close" function.
 *
 * Wrapper around the standard "close" function that ensures the file is closed
 * even if an interrupt signal is received.
 *
 * @param fd File descriptor of the file to close
 * @returns In case of success the return value is 1 otherwise 0
 */
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

/**
 * Collection of unit tests for xclose.
 *
 * Runs tests to check if the function is capable of closing a file.
 *
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
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

/**
 * Simple wrapper of the "read" function.
 *
 * Wrapper around the standard "read" function that ensures bytes are read from
 * the open file, even when an interrupt signal is sent. It also ensures all the
 * bytes are read, since the standard function does guarantee how many bytes are
 * read.
 *
 * @param fd File descriptor
 * @param buf Buffer to populate with the data read
 * @param nbyte Number of bytes to read from the file
 * @returns Number of bytes read
 */
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

/**
 * Collection of unit tests for xread.
 *
 * Runs tests to check if the function is capable of reading from a file.
 *
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
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

        /* Check if the written string matches */
        ret = (!strncmp(str, buf, bytes)) ? 1 : 0;

cleanup_return:
        remove(cwd);
        free(cwd);
        return ret;
}

/**
 * Simple wrapper of the "pread" function.
 *
 * Wrapper around the standard "pread" function that ensures bytes are read from
 * the open file, even when an interrupt signal is sent. It also ensures all the
 * bytes are read, since the standard function doesn't guarantee how many bytes
 * are read.
 *
 * @param fd File descriptor
 * @param buf Buffer to populate with the data read
 * @param nbyte Number of bytes to read from the file
 * @param offset Offset at which the reading process will start
 * @returns Number of bytes read
 */
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

/**
 * Collection of unit tests for xpread.
 *
 * Runs tests to check if the function is capable of reading from a file.
 *
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
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

/**
 * Simple wrapper of the "write" function.
 *
 * Wrapper around the standard "write" function that ensures bytes are written
 * into the open file, even when an interrupt signal is sent. It also ensures
 * all the bytes are written, since the standard function doesn't guarantee how
 * many bytes are written.
 *
 * @param fd File descriptor
 * @param buf Buffer containing the data to be written
 * @param nbyte Number of bytes to read from the file
 * @returns Number of write read
 */
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

/**
 * Collection of unit tests for xwrite.
 *
 * Runs tests to check if the function is capable of writting to a file.
 *
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
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

/**
 * Simple wrapper of the "pwrite" function.
 *
 * Wrapper around the standard "pwrite" function that ensures bytes are written
 * to the open file, even when an interrupt signal is sent. It also ensures all
 * the bytes are written, since the standard function doesn't guarantee how many
 * bytes are written.
 *
 * @param fd File descriptor
 * @param buf Buffer containing the data to be written
 * @param nbyte Number of bytes to write
 * @param offset Offset at which the writing process will start
 * @returns Number of bytes written
 */
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

/**
 * Collection of unit tests for xpwrite.
 *
 * Runs tests to check if the function is capable of writting to a file.
 *
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
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

/**
 * Simple wrapper of the "opendir" function.
 *
 * Wrapper around the standard "opendir" function that ensures the directory is
 * opened properly, otherwise the function will cause the code to exit with an
 * error.
 *
 * @param path String containing the directory of the path to open.
 * @returns Pointer to open directory.
 */
DIR*
xopendir(const char* path)
{
        DIR* dir = opendir(path);

        if (!dir) {
                printf(RED DONUT RESET "Failed to open the directory: %s\n", path);
                exit(DEF_ERR);
        }

        return dir;
}

/**
 * Unit test for opendir which attempts to open the "Home" directory.
 *
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
int
test_xopendir(void)
{
        const char* path = getenv("HOME");
        DIR* dir = opendir(path);
        return (dir) ? 1 : 0;
}

/**
 * Simple wrapper of the "closedir" function.
 *
 * Wrapper around the standard "closedir" function that ensures the directory is
 * closed properly, otherwise the function will cause the code to exit with an
 * error.
 *
 * @param path Pointer to the directory structure.
 * @returns Integer indicating if the close procedure was successful.
 */
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

/**
 * Unit test for closedir which attempts to open the "Home" directory and then
 * close it.
 *
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
int
test_xclosedir(void)
{
        const char* path = getenv("HOME");
        DIR* dir = opendir(path);
        int ret = (dir) ? 1 : 0;
        ret = xclosedir(dir);
        return !ret;
}

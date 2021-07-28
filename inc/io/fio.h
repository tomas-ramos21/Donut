#ifndef __FIO_H_
#define __FIO_H_

#include "sys/types.h"
#include "fcntl.h"
#include "dirent.h"

/**
 * @file fio.h
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
int xopen(const char* path, int oflag, ...);

/**
 * Simple wrapper of the standard "close" function.
 *
 * Wrapper around the standard "close" function that ensures the file is closed
 * even if an interrupt signal is received.
 *
 * @param fd File descriptor of the file to close
 * @returns In case of success the return value is 1 otherwise 0
 */
int xclose(int fd);

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
size_t xread(int fd, void* buf, size_t nbyte);

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
size_t xwrite(int fd, void* buf, size_t nbyte);

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
size_t xpread(int fd, void* restrict buf, size_t nbyte, off_t offset);

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
size_t xpwrite(int fd, void* restrict buf, size_t nbyte, off_t offset);

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
DIR* xopendir(const char* path);

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
int xclosedir(DIR* dir);


int xmkdir(const char *pathname, mode_t mode);

/* Unit Tests */

/**
 * Unit test for xopen.
 * Runs tests to check if the function is capable of opening a file.
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
int test_xopen(void);

/**
 * Unit tests for xclose.
 * Runs tests to check if the function is capable of closing a file.
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
int test_xclose(void);

/**
 * Unit tests for xwrite.
 * Runs tests to check if the function is capable of writting to a file.
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
int test_xwrite(void);

/**
 * Unit tests for xread.
 * Runs tests to check if the function is capable of reading from a file.
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
int test_xread(void);

/**
 * Unit tests for xpread.
 * Runs tests to check if the function is capable of reading from a file.
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
int test_xpread(void);

/**
 * Unit tests for xpwrite.
 * Runs tests to check if the function is capable of writting to a file.
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
int test_xpwrite(void);

/**
 * Unit test for xopendir.
 * Check if the function is capable of opening the "Home" directory.
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
int test_xopendir(void);

/**
 * Unit test for xclosedir.
 * Check if the function is capable of opening then closing the "Home" directory.
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
int test_xclosedir(void);

int test_xmkdir(void);

#endif // __FIO_H_

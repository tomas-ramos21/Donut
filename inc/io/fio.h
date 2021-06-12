#ifndef __FIO_H_
#define __FIO_H_

#include "sys/types.h"
#include "fcntl.h"

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
int xopen(const char* path, int oflag);

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

/* Unit Tests */
int test_xopen(void);
int test_xclose(void);
int test_xwrite(void);
int test_xread(void);
int test_xpread(void);
int test_xpwrite(void);

#endif // __FIO_H_

#ifndef __FIO_H_
#define __FIO_H_

#include "sys/types.h"
#include "fcntl.h"

int xopen(const char* path, int oflag);
int xclose(int fd);
size_t xread(int fd, void* buf, size_t nbyte);
size_t xwrite(int fd, void* buf, size_t nbyte);
size_t xpread(int fd, void* restrict buf, size_t nbyte, off_t offset);
size_t xpwrite(int fd, void* restrict buf, size_t nbyte, off_t offset);

/* Unit Tests */
int test_xopen(void);
int test_xclose(void);
int test_xwrite(void);
int test_xread(void);
int test_xpread(void);
int test_xpwrite(void);

#endif // __FIO_H_

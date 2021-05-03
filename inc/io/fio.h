#ifndef __FIO_H_
#define __FIO_H_

#include "sys/types.h"
#include "fcntl.h"

int xopen(const char* path, int oflag);
int xclose(int fd);
size_t xread(int fd, void* buf, size_t nbyte);
size_t xwrite(int fd, void* buf, size_t nbyte);

/* Unit Tests */
int test_xopen(void);
int test_xclose(void);

#endif // __FIO_H_

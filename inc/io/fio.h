#ifndef __FIO_H_
#define __FIO_H_

#include "sys/types.h"

int xopen(const char* path, int oflag);
int xclose(int fd);
size_t xread(int fd, void* buf, size_t nbyte);
size_t xwrite(int fd, void* buf, size_t nbyte);


#endif // __FIO_H_

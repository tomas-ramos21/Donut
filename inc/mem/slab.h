#ifndef __SLAB_H_
#define __SLAB_H_

#include "stdio.h"

struct slab* init_slab(void);
void* alloc_slab(struct slab* ptr, size_t sz);
int test_init_slab(void);
int test_alloc_slab(void);

#endif // __SLAB_H_

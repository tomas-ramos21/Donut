#ifndef __SLAB_H_
#define __SLAB_H_

#include "stdio.h"

struct slab* new_slab(void);
void* slab_alloc(struct slab* ptr, size_t sz);

#endif // __SLAB_H_

#ifndef __SLAB_H_
#define __SLAB_H_

#include "stdlib.h"

struct slabs* init_slabs(void);
void* alloc_slab(struct slabs* ptr, size_t sz);
void free_slab(struct slabs* ptr, void* slab);
void clear_slabs(struct slabs* ptr);

/* Unit tests */
int test_init_slab(void);
int test_alloc_slab(void);
int test_free_slab(void);

#endif // __SLAB_H_

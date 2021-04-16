#ifndef __ALLOC_H_
#define __ALLOC_H_

#include "stdio.h"

void* xmalloc(size_t sz);
void* xcalloc(size_t n, size_t sz);
void* xrealloc(void* buff, size_t sz);
int test_xmalloc(int reps);
int test_xcalloc(int reps);
int test_xrealloc(int reps);

#endif // __ALLOC_H_

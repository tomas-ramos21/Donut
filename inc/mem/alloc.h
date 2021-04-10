#ifndef __ALLOC_H_
#define __ALLOC_H_

#ifdef __SIZE_OF_POINTER__
#define ALIGN __SIZE_OF_POINTER__ * 8
#else
#define ALIGN 64
#endif

#include "stdio.h"

void* xmalloc(size_t sz);
void* xcalloc(size_t n, size_t sz);

#endif // __ALLOC_H_

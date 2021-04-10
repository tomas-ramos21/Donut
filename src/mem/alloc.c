#include "mem/alloc.h"
#include "stdlib.h"
#include "inttypes.h"

void*
xmalloc(size_t sz)
{
        void* ret = malloc(sz + ALIGN);
        const int align = ALIGN - 1;

        if(!ret){
                printf("Failed to allocate memory\n");
                exit(1);
        }

        uintptr_t bit_mask = ~(uintptr_t) align;
        ret = (void*) (((uintptr_t) ret + align) & bit_mask);

        return ret;
}


void*
xcalloc(size_t n, size_t sz)
{
        void* ret = calloc(n, sz + ALIGN);
        const int align = ALIGN - 1;

        if(!ret){
                printf("Failed to allocate memory\n");
                exit(1);
        }

        uintptr_t bit_mask = ~(uintptr_t) align;
        ret = (void*) (((uintptr_t) ret + align) & bit_mask);

        return ret;
}

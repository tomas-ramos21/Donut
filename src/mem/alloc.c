#include "mem/alloc.h"
#include "stdlib.h"
#include "string.h"
#include "inttypes.h"
#include "errno.h"

void*
xmalloc(size_t sz)
{
        void* ret = malloc(sz + ALIGN);
        const int align = ALIGN - 1;

        if (!ret) {
                printf("Failed to allocate memory.\n");
                exit(ENOMEM);
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

        if (!ret) {
                printf("Failed to allocate memory.\n");
                exit(ENOMEM);
        }

        uintptr_t bit_mask = ~(uintptr_t) align;
        ret = (void*) (((uintptr_t) ret + align) & bit_mask);

        return ret;
}

void*
xrealloc(void* buff, size_t sz)
{
        if (!buff) {
                printf("Trying to reallocate null pointer.\n");
                exit(EFAULT);
        }

        void* ret = xmalloc(sz);
        char* ret_cp = ret;
        char* buff_cp = buff;

        while (*buff_cp) {
                *ret_cp = *buff_cp;
                ret_cp++;
                buff_cp++;
        }

        return ret;
}

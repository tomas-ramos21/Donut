#include "mem/alloc.h"
#include "misc/decorations.h"
#include "stdlib.h"
#include "string.h"
#include "inttypes.h"
#include "errno.h"

void*
xmalloc(size_t sz)
{
        void* ret = malloc(sz + CACHE_LINE);
        const int align = CACHE_LINE - 1;

        if (!ret) {
                printf(DONUT "Failed to allocate memory.\n");
                exit(ENOMEM);
        }

        uintptr_t bit_mask = ~(uintptr_t) align;
        ret = (void*) (((uintptr_t) ret + align) & bit_mask);

        return ret;
}

int
test_xmalloc(int reps)
{
        void* buff;
        int i, sum = 0;
        int res[reps];

        for (i = 0; i < reps; i++) {
                buff = xmalloc(PAGE_SIZE);

                if (((uintptr_t) buff % CACHE_LINE) == 0)
                        res[i] = 1;

                free(buff);
        }

        for (i = 0; i < reps; i++)
                sum += res[i];

        return (reps == sum) ? 1 : 0;
}

void*
xcalloc(size_t n, size_t sz)
{
        void* ret = calloc(n, sz + CACHE_LINE);
        const int align = CACHE_LINE - 1;

        if (!ret) {
                printf(DONUT "Failed to allocate memory.\n");
                exit(ENOMEM);
        }

        uintptr_t bit_mask = ~(uintptr_t) align;
        ret = (void*) (((uintptr_t) ret + align) & bit_mask);

        return ret;
}

int
test_xcalloc(int reps)
{
        void* buff;
        int i, sum = 0;
        int res[reps];

        for (i = 0; i < reps; i++) {
                buff = xcalloc(1, PAGE_SIZE);

                if (((uintptr_t) buff % CACHE_LINE) == 0)
                        res[i] = 1;

                free(buff);
        }

        for (i = 0; i < reps; i++)
                sum += res[i];

        return (reps == sum) ? 1 : 0;
}

void*
xrealloc(void* buff, size_t sz)
{
        if (!buff)
                return xmalloc(sz);

        void* ret = xmalloc(sz);
        char* ret_cp = ret;
        char* buff_cp = buff;

        while ((*ret_cp++ = *buff_cp++));
        free(buff);

        return ret;
}

int
test_xrealloc(int reps)
{
        void* buff;
        int i, sum;
        int res[reps];

        for (i = 0; i < reps; i++) {
                sum = 0;
                char* dt = (char*) malloc(PAGE_SIZE);
                memset(dt, 0x1, PAGE_SIZE);
                buff = xrealloc(dt, PAGE_SIZE + 1);

                char* buff_cp = buff;
                while (*buff_cp)
                        sum += *buff_cp++;

                if (((uintptr_t) buff % CACHE_LINE) == 0 && sum == PAGE_SIZE)
                        res[i] = 1;

                free(buff);
        }

        sum = 0;
        for (i = 0; i < reps; i++)
                sum += res[i];

        return (reps == sum) ? 1 : 0;
}

#include "mem/alloc.h"
#include "misc/decorations.h"
#include "stdlib.h"
#include "string.h"
#include "errno.h"

/**
 * @file alloc.c
 * Implementation of safety wrappers over the C's memory allocation functions.
 */

/**
 * @def REPETITIONS
 * Number times allocation are repeated in the unit tests.
 */
#define REPETITIONS 10

void*
xmalloc(size_t sz)
{
        void* ret = malloc(sz);

        if (!ret) {
                printf(DONUT "Failed to allocate memory.\n");
                exit(ENOMEM);
        }

        return ret;
}

int
test_xmalloc(void)
{
        void* buff;
        int i, sum = 0;
        int res[REPETITIONS];

        for (i = 0; i < REPETITIONS; i++) {
                buff = xmalloc(PAGE_SIZE);

                if (buff)
                        res[i] = 1;

                free(buff);
        }

        for (i = 0; i < REPETITIONS; i++)
                sum += res[i];

        return (REPETITIONS == sum) ? 1 : 0;
}

void*
xcalloc(size_t n, size_t sz)
{
        void* ret = calloc(n, sz);

        if (!ret) {
                printf(DONUT "Failed to allocate memory.\n");
                exit(ENOMEM);
        }

        return ret;
}

int
test_xcalloc(void)
{
        void* buff;
        int i, sum = 0;
        int res[REPETITIONS];

        for (i = 0; i < REPETITIONS; i++) {
                buff = xcalloc(1, PAGE_SIZE);

                if (buff)
                        res[i] = 1;

                free(buff);
        }

        for (i = 0; i < REPETITIONS; i++)
                sum += res[i];

        return (REPETITIONS == sum) ? 1 : 0;
}

void*
xrealloc(void* buff, size_t sz)
{
        if (!buff)
                return xmalloc(sz);

        void* ret = realloc(buff, sz);
        if (!ret) {
                printf(DONUT "Failed to allocate memory.\n");
                exit(ENOMEM);
        }

        return ret;
}

int
test_xrealloc(void)
{
        void* buff;
        int i, sum;
        int res[REPETITIONS];

        for (i = 0; i < REPETITIONS; i++) {
                sum = 0;
                char* dt = malloc(PAGE_SIZE);
                memset(dt, 0x1, PAGE_SIZE);
                buff = xrealloc(dt, PAGE_SIZE + 1);

                char* buff_cp = buff;
                while (*buff_cp)
                        sum += *buff_cp++;

                if (buff && sum == PAGE_SIZE)
                        res[i] = 1;

                free(buff);
        }

        sum = 0;
        for (i = 0; i < REPETITIONS; i++)
                sum += res[i];

        return (REPETITIONS == sum) ? 1 : 0;
}

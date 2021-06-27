#include "mem/alloc.h"
#include "misc/decorations.h"
#include "stdlib.h"
#include "string.h"
#include "inttypes.h"
#include "errno.h"

/**
 * @file alloc.c
 *
 * Simple wrappers around the standard memory allocation functions.
 *
 * Using the standard allocation functions is boring, leads to fragmentation
 * and doesn't allow to align the memory to the machine's cache line.
 * Therefore, this file implements wrappers around these functions to provide
 * some customised behaviour.
 */

/**
 * Wrapper of the standard "malloc" function.
 *
 * Wrapper around the standard "malloc" function which provides the following:
 *  1. Ensures memory is aligned to the cache line size
 *  2. If the allocation fails, the program is immediatly terminated
 *
 * @param sz Byte size of the memory to be allocated
 * @returns Pointer to the allocated buffer
 */
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

/**
 * Collection of unit tests for xmalloc.
 * Runs tests to check if the function is capable of allocating aligned memory.
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
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

/**
 * Wrapper of the standard "calloc" function.
 *
 * Wrapper around the standard "calloc" function which provides the following:
 *  1. Ensures memory is aligned to the cache line size
 *  2. If the allocation fails, the program is immediatly terminated
 *
 * @param n Number of memory blocks to be allocated
 * @param sz Byte size of each memory block to be allocated
 * @returns Pointer to the allocated buffer
 */
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

/**
 * Collection of unit tests for xcalloc.
 * Runs tests to check if the function is capable of allocating aligned memory.
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
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

/**
 * Wrapper of the standard "realloc" function.
 *
 * Wrapper around the standard "realloc" function which provides the following:
 *  1. Ensures memory is aligned to the cache line size
 *  2. If the allocation fails, the program is immediatly terminated
 *
 * @param buff Buffer whose memory content needs to be expanded
 * @param sz Byte size of the buffer to be allocated
 * @returns Pointer to the allocated buffer
 */
void*
xrealloc(void* buff, size_t sz)
{
        if (!buff)
                return xmalloc(sz);

        void* ret = xmalloc(sz);
        char* ret_cp = ret;
        char* buff_cp = buff;

        while ((*ret_cp++ = *buff_cp++));
        printf("Buffer: %p\n", buff);
        free(buff);

        return ret;
}

/**
 * Collection of unit tests for xrealloc.
 * Runs tests to check if the function is capable of allocating aligned memory.
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
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

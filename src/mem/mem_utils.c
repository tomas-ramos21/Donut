#include "mem/mem_utils.h"
#include "inttypes.h"

/**
 * @file mem_utils.c
 * Implementation of utility functions used to act or manipulate memory.
 */

int
const_memcmp(void* s1, void* s2, size_t n)
{
        unsigned int res = 0;
        uint8_t* buf1 = s1;
        uint8_t* buf2 = s2;

        while (n--)
                res |= *buf1++ ^ *buf2++;

        return !res;
}

/**
 * Collection of unit tests for const_memcmp.
 * Runs tests to check if the function is comparing 2 memory buffers.
 * @returns In case of success the return value is 1 otheriwse its 0.
 */
int
test_const_memcmp(void)
{
        int ret;
        char* str1 = "mytest1";
        char* str2 = "mytest1";
        char* str3 = "mytest3";

        ret = const_memcmp(str1, str2, 8);
        if (!ret)
                return 0;

        return !const_memcmp(str1, str3, 8);
}

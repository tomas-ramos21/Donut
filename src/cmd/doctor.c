#include "cmd/doctor.h"
#include "mem/alloc.h"
#include "stdlib.h"
#include "string.h"
#include "misc/colour.h"
#include "inttypes.h"

#define NUM_ALLOC_TESTS 10

/*
 * Test the memory allocation function modules.
 */
static void
test_mem_alloc(void)
{
        /*  Malloc Test */
        printf("\n[Allocation Module]\n");
        if (test_xmalloc(NUM_ALLOC_TESTS))
                printf(GREEN "- xmalloc: passed" RESET "\n");
        else
                printf(RED "- xmalloc: failed" RESET "\n");

        /* Calloc Test */
        if (test_xcalloc(NUM_ALLOC_TESTS))
                printf(GREEN "- xcalloc: passed" RESET "\n");
        else
                printf(RED "- xcalloc: failed" RESET "\n");

        /* Realloc Test */
        if (test_xrealloc(NUM_ALLOC_TESTS))
                printf(GREEN "- xrealloc: passed" RESET "\n");
        else
                printf(RED "- xrealloc: failed" RESET "\n");
}

int
doctor(int argc, const char** argv, const unsigned int opts)
{
        test_mem_alloc();
        return 0;
}

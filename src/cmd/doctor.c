#include "cmd/cmd.h"
#include "mem/alloc.h"
#include "mem/slab.h"
#include "string.h"
#include "misc/colour.h"

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

static void
test_mem_slab(void)
{
        printf("\n[Slab Module]\n");
        if (test_init_slab())
                printf(GREEN "- init_slab: passed" RESET "\n");
        else
                printf(RED "- init_slab: failed" RESET "\n");

        if (test_alloc_slab())
                printf(GREEN "- alloc_slab: passed" RESET "\n");
        else
                printf(RED "- alloc_slab: failed" RESET "\n");

        if (test_free_slab())
                printf(GREEN "- free_slab: passed" RESET "\n");
        else
                printf(RED "- free_slab: failed" RESET "\n");
}

int
doctor(const int argc, const struct parsed_args* args)
{
        test_mem_alloc();
        test_mem_slab();
        return 0;
}

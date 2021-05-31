#include "cmd/cmd.h"
#include "mem/alloc.h"
#include "mem/slab.h"
#include "io/fio.h"
#include "string.h"
#include "misc/colour.h"
#include "crypto/sha2.h"

#define NUM_ALLOC_TESTS 10

/* Test the memory allocation module */
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

/* Test the slab memory allocator module */
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

/* Test the file I/O module */
static void
test_io_fio(void)
{
        printf("\n[File I/O Module]\n");
        if (test_xopen())
                printf(GREEN "- xopen: passed" RESET "\n");
        else
                printf(RED "- xopen: failed" RESET "\n");

        if (test_xclose())
                printf(GREEN "- xclose: passed" RESET "\n");
        else
                printf(RED "- xclose: failed" RESET "\n");

        if (test_xread())
                printf(GREEN "- xread: passed" RESET "\n");
        else
                printf(RED "- xread: failed" RESET "\n");

        if (test_xwrite())
                printf(GREEN "- xwrite: passed" RESET "\n");
        else
                printf(RED "- xwrite: failed" RESET "\n");

        if (test_xpread())
                printf(GREEN "- xpread: passed" RESET "\n");
        else
                printf(RED "- xpread: failed" RESET "\n");

        if (test_xpwrite())
                printf(GREEN "- xpread: passed" RESET "\n");
        else
                printf(RED "- xpread: failed" RESET "\n");
}

static void
test_crypto(void)
{

        printf("\n[Crypto Module]\n");
        if (test_sha2())
                printf(GREEN "- SHA2: passed" RESET "\n");
        else
                printf(RED "- SHA2: failed" RESET "\n");

}

int
doctor(const int argc, const struct parsed_args* args)
{
        test_mem_alloc();
        test_mem_slab();
        test_io_fio();
        test_crypto();
        return 0;
}

#include "cli/cmd.h"
#include "mem/slob.h"
#include "mem/mem_utils.h"
#include "core/wrappers.h"
#include "string.h"
#include "misc/colour.h"
#include "crypto/sha2.h"
#include "core/data-list.h"
#include "cli/arg-parse.h"

/**
 * @file doctor.c
 *
 * File containing the "doctor" command and all associated utilities.
 *
 * This file contains all functions used by the "doctor" command to test the
 * integrity of the system. The purpose is to provide a module that can be used
 * to detect issues with the installation or setup of the system. It will also
 * contain functions to build reports on what may have occurred with the system.
 */

/**
 * Execute all slob allocator module's unit tests.
 *
 * Executes all unit tests for the slob allocator module and prints onto the
 * screen if they passed or failed.
 */
static void
test_mem_slob(void)
{
        printf("\n[Slob Module]\n");
        if (test_init_slob())
                printf(GREEN "- init_slob: passed" RESET "\n");
        else
                printf(RED "- init_slob: failed" RESET "\n");

        if (test_alloc_slob())
                printf(GREEN "- alloc_slob: passed" RESET "\n");
        else
                printf(RED "- alloc_slob: failed" RESET "\n");

        if (test_free_slob())
                printf(GREEN "- free_slob: passed" RESET "\n");
        else
                printf(RED "- free_slob: failed" RESET "\n");
}

/**
 * Execute all file I/O module's unit tests.
 *
 * Executes all unit tests for the file I/O module and prints onto the
 * screen if they passed or failed.
 */
static void
test_core_wrappers(void)
{
        printf("\n[Wrappers Module]\n");
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
                printf(GREEN "- xpwrite: passed" RESET "\n");
        else
                printf(RED "- xpwrite: failed" RESET "\n");

        if (test_xopendir())
                printf(GREEN "- xopendir: passed" RESET "\n");
        else
                printf(RED "- xopendir: failed" RESET "\n");

        if (test_xclosedir())
                printf(GREEN "- xclosedir: passed" RESET "\n");
        else
                printf(RED "- xclosedir: failed" RESET "\n");

        if (test_xmkdir())
                printf(GREEN "- xmkdir: passed" RESET "\n");
        else
                printf(RED "- xmkdir: failed" RESET "\n");

        if (test_xrename())
                printf(GREEN "- xrename: passed" RESET "\n");
        else
                printf(RED "- xrename: failed" RESET "\n");

        if (test_xchmod())
                printf(GREEN "- xchmod: passed" RESET "\n");
        else
                printf(RED "- xchmod: failed" RESET "\n");

        if (test_xgetcwd())
                printf(GREEN "- xgetcwd: passed" RESET "\n");
        else
                printf(RED "- xgetcwd: failed" RESET "\n");

        if (test_xmalloc())
                printf(GREEN "- xmalloc: passed" RESET "\n");
        else
                printf(RED "- xmalloc: failed" RESET "\n");

        /* Calloc Test */
        if (test_xcalloc())
                printf(GREEN "- xcalloc: passed" RESET "\n");
        else
                printf(RED "- xcalloc: failed" RESET "\n");

        /* Realloc Test */
        if (test_xrealloc())
                printf(GREEN "- xrealloc: passed" RESET "\n");
        else
                printf(RED "- xrealloc: failed" RESET "\n");
}

/**
 * Execute all cryptographic module's unit tests.
 *
 * Executes all unit tests for the cryptographic module and prints onto the
 * screen if they passed or failed.
 */
static void
test_crypto(void)
{

        printf("\n[Crypto Module]\n");
        if (test_sha2())
                printf(GREEN "- SHA2: passed" RESET "\n");
        else
                printf(RED "- SHA2: failed" RESET "\n");

        if (test_sha2_init())
                printf(GREEN "- sha2_init: passed" RESET "\n");
        else
                printf(RED "- sha2_init: failed" RESET "\n");

        if (test_sha2_to_str())
                printf(GREEN "- sha2_to_str: passed" RESET "\n");
        else
                printf(RED "- sha2_to_str: failed" RESET "\n");

        if (test_sha2_to_strn())
                printf(GREEN "- sha2_to_strn: passed" RESET "\n");
        else
                printf(RED "- sha2_to_strn: failed" RESET "\n");
}

/**
 * Execute all memory utility functions module's unit tests.
 *
 * Executes all unit tests for the memory utility module and prints onto the
 * screen if they passed or failed.
 */
static void
test_memory_utilities(void)
{
        printf("\n[Memory Utilities Module]\n");
        if (test_const_memcmp())
                printf(GREEN "- const_memcmp: passed" RESET "\n");
        else
                printf(RED "- const_memcmp: failed" RESET "\n");
}

static void
test_core_module(void)
{
        printf("\n[Core Module]\n");
        if (test_data_list_init())
                printf(GREEN "- init_data_list: passed" RESET "\n");
        else
                printf(RED "- init_data_list: failed" RESET "\n");
        if (test_get_data_list_index())
                printf(GREEN "- get_data_list_index: passed" RESET "\n");
        else
                printf(RED "- get_data_list_index: failed" RESET "\n");
        if (test_add_file_to_data_list())
                printf(GREEN "- add_file_to_list: passed" RESET "\n");
        else
                printf(RED "- add_file_to_list: failed" RESET "\n");
}

static void
test_cli_arg_parsing(void)
{
        printf("\n[Argument Parsing Module]\n");
        if (test_parse_opts())
                printf(GREEN "- parse_opts: passed" RESET "\n");
        else
                printf(RED "- parse_opts: failed" RESET "\n");
}

/**
 * Display all unit tests results to the user.
 *
 * @param argc Number of arguments passed
 * @param args Array of arguments
 */
int
doctor(const int argc, char** argv, int arg_idx, char* opts, uint64_t oflags)
{
        test_mem_slob();
        test_core_wrappers();
        test_crypto();
        test_memory_utilities();
        test_core_module();
        test_cli_arg_parsing();
        return 0;
}

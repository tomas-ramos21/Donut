#include "cli/cmd.h"
#include "stdio.h"

/**
 * @file conf.c
 *
 * Implements all functions and utlities used by the "conf" command.
 */

/**
 * Prints the current configuration detected and being used by donut.
 *
 * @param argc Number of arguments passed
 * @param args Array of arguments
 */
void
conf(const int argc, const struct parsed_args* args)
{
        printf("Page Size: %u\n\
Cache Line Size: %u\n\
L1 Cache Data Size: %u\n\
L1 Cache Instruction Size: %u\n\
Max Amount of Open Files: %u\n\
Number of CPUs: %u\n",
               PAGE_SIZE,
               CACHE_LINE,
               D_CACHE,
               I_CACHE,
               OPEN_MAX,
               N_CPU);
}

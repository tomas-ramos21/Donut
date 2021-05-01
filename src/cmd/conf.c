#include "cmd/cmd.h"
#include "stdio.h"

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

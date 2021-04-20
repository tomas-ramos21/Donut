#include "cmd/conf.h"
#include "stdio.h"

void
conf(int argc, const char** argv, const unsigned int opts)
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

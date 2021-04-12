#include "cmd/conf.h"
#include "unistd.h"
#include "stdio.h"

struct sys_conf
get_sys_conf()
{
        uint32_t pg_sz, fd_max;

#if defined (_SC_PAGESIZE)
        pg_sz  = (uint32_t) sysconf(_SC_PAGESIZE);
#elif defined (PAGESIZE)
        pg_sz  = (uint32_t) sysconf(PAGESIZE);
#else
        pg_sz  = 4096;
#endif


#if defined (_SC_OPEN_MAX)
        fd_max = (uint32_t) sysconf(_SC_OPEN_MAX);
#elif defined (OPEN_MAX)
        fd_max = (uint32_t) sysconf(OPEN_MAX);
#else
        fd_max = 1024;
#endif

        struct sys_conf ret = {pg_sz, CACHE_LINE, D_CACHE, I_CACHE, L2_CACHE_SIZE, L3_CACHE_SIZE, fd_max, N_CPU};
        return ret;
}


void
conf(int argc, const char** argv, const unsigned int opts)
{
        struct sys_conf info = get_sys_conf();
        printf("Page Size: %u\n\
Cache Line Size: %u\n\
L1 Cache Data Size: %u\n\
L1 Cache Instruction Size: %u\n\
L2 Cache Size: %u\n\
L3 Cache Size: %u\n\
Max Amount of Open Files: %u\n\
Number of CPUs: %u\n",
               info.pg_sz,
               info.ch_ln_sz,
               info.ch_l1d_sz,
               info.ch_l1i_sz,
               info.ch_l2_sz,
               info.ch_l3_sz,
               info.fd_max,
               info.n_cpus);
}

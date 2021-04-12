#include "cmd/conf.h"
#include "unistd.h"
#include "stdio.h"

struct sys_conf
get_sys_conf()
{
        uint32_t pg_sz, fd_max, n_cpus, ch_ln_sz;

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

#if defined (_SC_NPROCESSORS_CONF)
        n_cpus = (uint32_t) sysconf(_SC_NPROCESSORS_CONF);
#elif (__APPLE__) | (__MACH__) | (__linux__)
        n_cpus = N_CPU;
#else
        n_cpus = 0;
#endif

#if defined (_SC_LEVEL1_DCACHE_LINESIZE)
        ch_ln_sz = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
#else
        ch_ln_sz = CACHE_LINE;
#endif

        struct sys_conf ret = {pg_sz, ch_ln_sz, 0, 0, 0, 0, fd_max, n_cpus};
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
               info.ch_l1_dt_sz,
               info.ch_l1_in_sz,
               info.ch_l2_sz,
               info.ch_l3_sz,
               info.fd_max,
               info.n_cpus);
}

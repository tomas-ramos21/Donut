#ifndef __CONF_H_
#define __CONF_H_

#include "inttypes.h"

/* Break into several structs in the future if needed */
struct sys_conf {
        uint32_t pg_sz;     /* OS Page Size              */
        uint32_t ch_ln_sz;  /* Cache Line Size           */
        uint32_t ch_l1d_sz; /* L1 data cache size        */
        uint32_t ch_l1i_sz; /* L1 instruction cache size */
        uint32_t ch_l2_sz;  /* L2 cache size             */
        uint32_t ch_l3_sz;  /* L3 cache size             */
        uint32_t fd_max;    /* Max files opened          */
        uint32_t n_cpus;    /* Number of CPUs            */
};

struct sys_conf get_sys_conf();

void conf(int argc, const char** argv, const unsigned int opts);

#endif // __CONF_H_

#ifndef ARG_PARSE_H_
#define ARG_PARSE_H_

#include "inttypes.h"

int parse_opts(int argc, char** argv, void* buf, uint64_t* tmp);

/* Unit Tests */
int test_parse_opts(void);

#endif // ARG_PARSE_H_

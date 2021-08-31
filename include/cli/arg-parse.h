#ifndef ARG_PARSE_H_
#define ARG_PARSE_H_

#include "inttypes.h"

/**
 * @file arg-parse.h
 *
 * Header containing option parsing functions.
 */

/**
 * Updates the option flags and sets named arguments' values into a buffer.
 *
 * The bits corresponding to each argument are set and if those arguments have a
 * value they are copied into the corresponding bit index of the buffer.
 *
 * @param argc Integer containing the number of arguments.
 * @param argv String array containing the arguments.
 * @param buf Buffer into which the arguments' values will be copied.
 * @param opt_flags Unsigned Long whose bits are updated when flags are set.
 * @return The index of the next argument.
 */
int parse_opts(int argc, char** argv, void* buf, uint64_t* opt_flags);

/* Unit Tests */
int test_parse_opts(void);

#endif // ARG_PARSE_H_

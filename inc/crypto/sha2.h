#ifndef SHA2_H_
#define SHA2_H_

#include "stdio.h"

void sha2_hash(void* restrict in, void* out, void* restrict buf, size_t len);
int test_sha2(void);

#endif // SHA2_H_

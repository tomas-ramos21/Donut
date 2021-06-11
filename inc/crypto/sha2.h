#ifndef SHA2_H_
#define SHA2_H_

#include "stdio.h"
#include "stdint.h"

void sha2_hash(uintptr_t* restrict in, uint8_t* out, void* restrict buf, size_t len);
int test_sha2(void);

#endif // SHA2_H_

#ifndef SHA2_H_
#define SHA2_H_

#include "stdio.h"
#include "stdint.h"

/**
 * Computes the SHA-2 hash of a given buffer.
 *
 * Computes the SHA-2 hash of a byte sequence. The hash state is initialized, then
 * the data is changed to big-endian format and hashed. At the end the data is
 * transformed back to little-endian format and return in the respective buffer.
 *
 * @param in Buffer whose content will be hash
 * @param out Buffer where the result of the hashing process will be provided
 * @param buf Buffer where the hash state will be stored during the procedure
 * @param len Byte length of the input buffer
 */
void sha2_hash(uintptr_t* restrict in, uint8_t* out, void* restrict buf, size_t len);

/**
 * SHA-2 module's unit test.
 *
 * The code is tested over several vectors, by providing the input and expected
 * output string representation of the hash. All examples were sourced from:
 * https://www.di-mgt.com.au/sha_testvectors.html
 */
int test_sha2(void);

#endif // SHA2_H_

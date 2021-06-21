#ifndef SHA2_H_
#define SHA2_H_

#include "stdio.h"
#include "stdint.h"

/**
 * @def SHA2_BLK_SZ
 * SHA-2 block size in bytes.
 */
#define SHA_BLK_SZ 64

/**
 * @def SHA2_STRUCT_SZ
 * SHA-2 state structure size in bytes.
 */
#define SHA_STRUCT_SZ 360

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
 * Convert a hash into a string.
 *
 * Converts a hash into a string with it's hex representation.
 *
 * @param hash Pointer to hash buffer
 * @param buf Buffer where the string value of the hash will be placed
 */
void sha2_to_str(uint8_t* hash, char* buf);


/**
 * Initialize the hash state structure.
 *
 * @param buf Buffer in which the structure will be stored
 */
void sha2_init(void* buf);

/**
 * This function processes 64 bytes of data and ypdates the hash state.
 *
 * @param in Input buffer containing the data to be processed
 * @param buf Buffer containg the current hash state
 */
void sha2_update(void* in, void* out, void* buf, size_t bytes);

/**
 * SHA-2 module's small files unit test.
 *
 * The code is tested over several vectors, by providing the input and expected
 * output string representation of the hash. All examples were sourced from:
 * https://www.di-mgt.com.au/sha_testvectors.html
 */
int test_sha2(void);

/**
 * SHA-2 module's tests the initialization of the hash state.
 *
 * The code tests the initialization of the hash state, this function is meant
 * to be used in cases when the file is too large to be processed with the
 * standard "sha2_hash" function. Therefore, requiring the hash state structure
 * to be maintained by the user.
 * @see "sha2_hash"
 */
int test_sha2_init(void);

#endif // SHA2_H_

#include "crypto/sha2.h"
#include "inttypes.h"
#include "stdlib.h"
#include "string.h"

#define ROR(a,b) (((a) >> (b)) | ((a) << (32-(b))))
#define LRS(a,b) ((a) >> (b))

#define CH(x,y,z) (((x) & (y)) ^ ((~(x)) ^ (z)))
#define MA(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define S0(x) (ROR((x),(2)) ^ ROR((x),(13)) ^ ROR((x),(22)))
#define S1(x) (ROR((x),(6)) ^ ROR((x),(11)) ^ ROR((x),(25)))
#define S2(x) (ROR((x),7) ^ ROR((x),(18)) ^ LRS((x),(3)))
#define S3(x) (ROR((x),17) ^ ROR((x),(19)) ^ LRS((x),(10)))

#define BLK_SZ 64

struct hash_state {
        uint64_t len;
        uint32_t dat[8];
};

static const uint32_t k[64] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,
        0x923f82a4,0xab1c5ed5,0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
        0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,0xe49b69c1,0xefbe4786,
        0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,
        0x06ca6351,0x14292967,0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
        0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,0xa2bfe8a1,0xa81a664b,
        0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,
        0x5b9cca4f,0x682e6ff3,0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
        0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

static void
sha2_padding(struct hash_state* hash, void* restrict in, uint64_t bytes)
{
        uint32_t a, b, c, d, e, f, g, h, t1, t2, i, j, m[64];
        uint8_t x[64];

        memset(x, 0x0, 64);
        memcpy(x, in, bytes);
        x[bytes] = 0x80;
        memcpy(m, x, 64);

        if (bytes > 56) {
                for (j = 16; j < 64; j++)
                        m[j] = m[j-16] + m[j-7] + S3(m[j-2]) + S2(m[j-15]);

                a = hash->dat[0];
                b = hash->dat[1];
                c = hash->dat[2];
                d = hash->dat[3];
                e = hash->dat[4];
                f = hash->dat[5];
                g = hash->dat[6];
                h = hash->dat[7];

                for (j = 0; j < 64; j++) {
                        t1 = h + S1(e) + CH(e,f,g) + k[j] + m[j];
                        t2 = S0(a) + MA(a,b,c);
                        h = g;
                        g = f;
                        f = e;
                        e = d + t1;
                        d = c;
                        c = b;
                        b = a;
                        a = t1 + t2;
                }

                hash->dat[0] += a;
                hash->dat[1] += b;
                hash->dat[2] += c;
                hash->dat[3] += d;
                hash->dat[4] += e;
                hash->dat[5] += f;
                hash->dat[6] += g;
                hash->dat[7] += h;
                memset(m, 0x0, 56);
        }

        hash->len += bytes * 8;
        x[56] = hash->len;
        x[57] = hash->len >> 8;
        x[58] = hash->len >> 16;
        x[59] = hash->len >> 24;
        x[60] = hash->len >> 32;
        x[61] = hash->len >> 40;
        x[62] = hash->len >> 48;
        x[63] = hash->len >> 56;

        for (j = 0; j < 64; j++)
                printf("x[%d]: %u\n", j, x[j]);

        for (i = 0, j = 0; i < 16; ++i, j += 4) {
                m[i] = (x[j] << 24) | (x[j + 1] << 16) | (x[j + 2] << 8) | (x[j + 3]);
                printf("m[%d]: %u\n", i, m[i]);
        }
        for (; i < 64; ++i)
                m[i] = m[i-16] + m[i-7] + S3(m[i-2]) + S2(m[i-15]);

        a = hash->dat[0];
        b = hash->dat[1];
        c = hash->dat[2];
        d = hash->dat[3];
        e = hash->dat[4];
        f = hash->dat[5];
        g = hash->dat[6];
        h = hash->dat[7];

        for (j = 0; j < 64; j++) {
                t1 = h + S1(e) + CH(e,f,g) + k[j] + m[j];
                t2 = S0(a) + MA(a,b,c);
                h = g;
                g = f;
                f = e;
                e = d + t1;
                d = c;
                c = b;
                b = a;
                a = t1 + t2;
        }

        hash->dat[0] += a;
        hash->dat[1] += b;
        hash->dat[2] += c;
        hash->dat[3] += d;
        hash->dat[4] += e;
        hash->dat[5] += f;
        hash->dat[6] += g;
        hash->dat[7] += h;

        memcpy(x, hash->dat, 64);
}

void
sha2_hash(void* restrict in, void* out, void* restrict buf, size_t len)
{
        struct hash_state* hash = buf;
        uintptr_t* input = (uintptr_t*)in;
        uint8_t* output = (uint8_t*)out;
        uint64_t rm, i, blks = len / BLK_SZ;
        uint32_t a, b, c, d, e, f, g, h, t1, t2, j, m[64];

        hash->len = blks * 512;
        hash->dat[0] = 0x6a09e667;
        hash->dat[1] = 0xbb67ae85;
        hash->dat[2] = 0x3c6ef372;
        hash->dat[3] = 0xa54ff53a;
        hash->dat[4] = 0x510e527f;
        hash->dat[5] = 0x9b05688c;
        hash->dat[6] = 0x1f83d9ab;
        hash->dat[7] = 0x5be0cd19;

        for (i = 0; i < blks; i++) {
                memcpy(m, input, BLK_SZ);
                input += BLK_SZ;

                for (j = 16; j < 64; j++)
                        m[j] = m[j-16] + m[j-7] + S3(m[j-2]) + S2(m[j-15]);

                a = hash->dat[0];
                b = hash->dat[1];
                c = hash->dat[2];
                d = hash->dat[3];
                e = hash->dat[4];
                f = hash->dat[5];
                g = hash->dat[6];
                h = hash->dat[7];

                for (j = 0; j < 64; j++) {
                        t1 = h + S1(e) + CH(e,f,g) + k[j] + m[j];
                        t2 = S0(a) + MA(a,b,c);
                        h = g;
                        g = f;
                        f = e;
                        e = d + t1;
                        d = c;
                        c = b;
                        b = a;
                        a = t1 + t2;
                }

                hash->dat[0] += a;
                hash->dat[1] += b;
                hash->dat[2] += c;
                hash->dat[3] += d;
                hash->dat[4] += e;
                hash->dat[5] += f;
                hash->dat[6] += g;
                hash->dat[7] += h;
        }

        rm = len - (BLK_SZ * blks);
        printf("Remainder: %llu\n", rm);
        sha2_padding(hash, input, rm);
        printf("Length: %llu\n", hash->len);
        printf("Result: %lu\n", len / BLK_SZ);

        for (i = 0; i < 4; i++) {
                output[i]      = (hash->dat[0] >> (24 - i * 8)) & 0x000000ff;
                output[i + 4]  = (hash->dat[1] >> (24 - i * 8)) & 0x000000ff;
                output[i + 8]  = (hash->dat[2] >> (24 - i * 8)) & 0x000000ff;
                output[i + 12] = (hash->dat[3] >> (24 - i * 8)) & 0x000000ff;
                output[i + 16] = (hash->dat[4] >> (24 - i * 8)) & 0x000000ff;
                output[i + 20] = (hash->dat[5] >> (24 - i * 8)) & 0x000000ff;
                output[i + 24] = (hash->dat[6] >> (24 - i * 8)) & 0x000000ff;
                output[i + 28] = (hash->dat[7] >> (24 - i * 8)) & 0x000000ff;
        }
}

int
test_sha2(void)
{
        char* in = "abc";
        char* out = calloc(1,65);
        void* buf = calloc(1,512);

        sha2_hash(in, out, buf, 3);
        char* cp = out;
        printf("Hash Result: ");
        for (int i = 0; i < 32; i++)
                printf("%hhx", *out++);
        printf("\n");

        return 0;
}

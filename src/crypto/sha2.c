#include "crypto/sha2.h"
#include "stdlib.h"
#include "string.h"

#define ROR(a,b) (((a) >> (b)) | ((a) << (32-(b))))
#define LRS(a,b) ((a) >> (b))

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MA(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define S0(x) (ROR((x),(2)) ^ ROR((x),(13)) ^ ROR((x),(22)))
#define S1(x) (ROR((x),(6)) ^ ROR((x),(11)) ^ ROR((x),(25)))
#define S2(x) (ROR((x),(7)) ^ ROR((x),(18)) ^ LRS((x),(3)))
#define S3(x) (ROR((x),(17)) ^ ROR((x),(19)) ^ LRS((x),(10)))

#define BLK_SZ 64

struct hash_state {
        uint64_t len;
        uint32_t hx[8];
        uint32_t acc[64];
        uint8_t  data[64];
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

inline static void
xform(struct hash_state* restrict hash)
{
        uint32_t a, b, c, d, e, f, g, h, t1, t2, i, j;

        for (i = 0, j = 0; i < 16; ++i, j += 4)
                hash->acc[i] = (hash->data[j]   << 24) |
                               (hash->data[j+1] << 16) |
                               (hash->data[j+2] << 8)  |
                               (hash->data[j+3]);
        for (; i < 64; i++)
                hash->acc[i] = hash->acc[i-16] + hash->acc[i-7] +
                               S3(hash->acc[i-2]) + S2(hash->acc[i-15]);

        a = hash->hx[0];
        b = hash->hx[1];
        c = hash->hx[2];
        d = hash->hx[3];
        e = hash->hx[4];
        f = hash->hx[5];
        g = hash->hx[6];
        h = hash->hx[7];

        for (j = 0; j < 64; j++) {
                t1 = h + S1(e) + CH(e,f,g) + k[j] + hash->acc[j];
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

        hash->hx[0] += a;
        hash->hx[1] += b;
        hash->hx[2] += c;
        hash->hx[3] += d;
        hash->hx[4] += e;
        hash->hx[5] += f;
        hash->hx[6] += g;
        hash->hx[7] += h;
}

static void
sha2_padding(struct hash_state* hash, uintptr_t* restrict in, uint64_t bytes)
{
        memset(hash->data, 0x0, 64);
        memcpy(hash->data, in, bytes);
        hash->data[bytes] = 0x80;

        if (bytes >= 56) {
                xform(hash);
                memset(hash->data, 0x0, 64);
        }

        hash->len += bytes * 8;
        hash->data[63] = hash->len;
        hash->data[62] = hash->len >> 8;
        hash->data[61] = hash->len >> 16;
        hash->data[60] = hash->len >> 24;
        hash->data[59] = hash->len >> 32;
        hash->data[58] = hash->len >> 40;
        hash->data[57] = hash->len >> 48;
        hash->data[56] = hash->len >> 56;
        xform(hash);
}

void
sha2_hash(uintptr_t* restrict in, uint8_t* restrict out, void* restrict buf, size_t len)
{
        struct hash_state* hash = buf;
        uint64_t rm, i, blks = len / BLK_SZ;

        hash->len = blks * 512;
        hash->hx[0] = 0x6a09e667;
        hash->hx[1] = 0xbb67ae85;
        hash->hx[2] = 0x3c6ef372;
        hash->hx[3] = 0xa54ff53a;
        hash->hx[4] = 0x510e527f;
        hash->hx[5] = 0x9b05688c;
        hash->hx[6] = 0x1f83d9ab;
        hash->hx[7] = 0x5be0cd19;

        for (i = 0; i < blks; i++) {
                memcpy(hash->data, in, BLK_SZ);
                in += (BLK_SZ / 8);
                xform(hash);
        }

        rm = len - (BLK_SZ * blks);
        sha2_padding(hash, in, rm);

        for (i = 0; i < 4; i++) {
                out[i]      = (hash->hx[0] >> (24 - i * 8)) & 0x000000ff;
                out[i + 4]  = (hash->hx[1] >> (24 - i * 8)) & 0x000000ff;
                out[i + 8]  = (hash->hx[2] >> (24 - i * 8)) & 0x000000ff;
                out[i + 12] = (hash->hx[3] >> (24 - i * 8)) & 0x000000ff;
                out[i + 16] = (hash->hx[4] >> (24 - i * 8)) & 0x000000ff;
                out[i + 20] = (hash->hx[5] >> (24 - i * 8)) & 0x000000ff;
                out[i + 24] = (hash->hx[6] >> (24 - i * 8)) & 0x000000ff;
                out[i + 28] = (hash->hx[7] >> (24 - i * 8)) & 0x000000ff;
        }
}

int
test_sha2(void)
{
        /* Test Strings */
        char* tests[4] = { "abc",
        "",
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
        "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmn\
opjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"};

        /* Result Strings */
        char* results[4] = {"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad",
        "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
        "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1",
        "cf5b16a778af8380036ce59e7b0492370b249b11e8f07a51afac45037afee9d1"};

        char* out = calloc(1,32);
        char* str = calloc(1,64);
        void* buf = calloc(1,sizeof(struct hash_state));
        char* out_cp = out;
        int off = 0, res = 0, ret;

        /* Run Tests */
        for (int i = 0; i < 4; i++) {
                sha2_hash((uintptr_t*)tests[i], (uint8_t*)out, buf, strlen(tests[i]));
                while (off < 64) {
                        ret = snprintf(str + off, 3, "%02hhx", *out++);
                        if (ret != -1)
                                off += ret;
                }
                out = out_cp;
                off = 0;
                res = res | strncmp(str, results[i], 64);
        }

        /* Free resources */
        free(out);
        free(str);
        free(buf);

        return !res;
}

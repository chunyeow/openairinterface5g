/* $Id: crypto.h 1.1 06/02/23 17:26:41+02:00 anttit@tcs.hut.fi $ */

#ifndef __CRYPTO_H__
#define __CRYPTO_H__ 1

#define SHA_DIGESTSIZE 20
#define SHA_BLOCKSIZE 64

typedef struct {
    uint32_t h0,h1,h2,h3,h4;
    uint32_t nblocks;
    uint8_t buf[SHA_BLOCKSIZE];
    int count;
} SHA1_CTX;

typedef struct {
        SHA1_CTX ictx;
        uint8_t pad[SHA_BLOCKSIZE];
} HMAC_SHA1_CTX;

void SHA1_init(SHA1_CTX *ctx);

void SHA1_update(SHA1_CTX *ctx, const uint8_t *buf, size_t len);

void SHA1_final(SHA1_CTX *ctx, uint8_t *digest);

void HMAC_SHA1_init(HMAC_SHA1_CTX *ctx, const uint8_t *key, size_t keylen);

void HMAC_SHA1_update(HMAC_SHA1_CTX *ctx, const uint8_t *buf, size_t len);

void HMAC_SHA1_final(HMAC_SHA1_CTX *ctx, uint8_t *digest);

int random_bytes(uint8_t *buffer, int num);

#endif

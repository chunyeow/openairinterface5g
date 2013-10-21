#include <stdlib.h>
#include <stdint.h>

#include <nettle/hmac.h>

#include "security_types.h"
#include "secu_defs.h"

inline
void kdf(const uint8_t *s, const uint32_t s_length, const uint8_t *key,
         const uint32_t key_length, uint8_t **out, uint32_t out_length)
{
    struct hmac_sha256_ctx ctx;

    uint8_t *buffer;

    buffer = malloc(sizeof(uint8_t) * out_length);

    hmac_sha256_set_key(&ctx, key_length, key);
    hmac_sha256_update(&ctx, s_length, s);
    hmac_sha256_digest(&ctx, out_length, buffer);

    *out = buffer;
}

int derive_keNB(const uint8_t kasme[32], const uint32_t nas_count, uint8_t **keNB)
{
    uint8_t string[7];

    /* FC */
    string[0] = FC_KENB;
    /* P0 = Uplink NAS count */
    string[1] = (nas_count & 0xff000000) >> 24;
    string[2] = (nas_count & 0x00ff0000) >> 16;
    string[3] = (nas_count & 0x0000ff00) >> 8;
    string[4] = (nas_count & 0x000000ff);

    /* Length of NAS count */
    string[5] = 0x00;
    string[6] = 0x04;

    kdf(string, 7, kasme, 32, keNB, 32);

    return 0;
}

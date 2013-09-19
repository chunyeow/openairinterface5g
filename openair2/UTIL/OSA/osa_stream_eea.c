#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <nettle/nettle-meta.h>
#include <nettle/aes.h>
#include <nettle/ctr.h>

#include "UTIL/LOG/log.h"

#include "assertions.h"
#include "osa_defs.h"
#include "osa_internal.h"

int stream_encrypt_eea2(stream_cipher_t *stream_cipher, uint8_t **out)
{
    uint8_t m[16];
    uint32_t local_count;

    void *ctx;
    uint8_t *data = NULL;

    uint32_t zero_bit = 0;
    uint32_t byte_length;

    DevAssert(stream_cipher != NULL);
    DevAssert(out != NULL);

    LOG_D(OSA, "Entering stream_encrypt_eea2, bits length %u, bearer %u, "
          "count %u, direction %s\n", stream_cipher->blength,
          stream_cipher->bearer, stream_cipher->count, stream_cipher->direction == SECU_DIRECTION_DOWNLINK ?
          "Downlink" : "Uplink");

    zero_bit = stream_cipher->blength & 0x7;

    byte_length = stream_cipher->blength >> 3;
    if (zero_bit > 0)
        byte_length += 1;

    ctx = malloc(nettle_aes128.context_size);

    if (*out == NULL) {
        /* User provided output buffer */
        data = malloc(byte_length);
        *out = data;
    } else {
        data = *out;
    }

    local_count = hton_int32(stream_cipher->count);

    memset(m, 0, sizeof(m));
    memcpy(&m[0], &local_count, 4);
    m[4] = ((stream_cipher->bearer & 0x1F) << 3) |
              ((stream_cipher->direction & 0x01) << 2);
    /* Other bits are 0 */

#if defined(SECU_DEBUG)
    {
        int i;
        char payload[6 * byte_length + 1];
        int  index = 0;

        for (i = 0; i < byte_length; i++)
            index += sprintf(&payload[index], "0x%02x ", stream_cipher->message[i]);
        LOG_D(OSA, "Original message: %s\n", payload);
    }
#endif

    nettle_aes128.set_encrypt_key(ctx, stream_cipher->key_length,
                                  stream_cipher->key);

    nettle_ctr_crypt(ctx, nettle_aes128.encrypt,
                     nettle_aes128.block_size, m,
                     byte_length, data, stream_cipher->message);

    if (zero_bit > 0)
        data[byte_length - 1] = data[byte_length - 1] & (uint8_t)(0xFF << (8 - zero_bit));

    free(ctx);

#if defined(SECU_DEBUG)
    {
        int i;
        char payload[6 * byte_length + 1];
        int  index = 0;

        for (i = 0; i < byte_length; i++)
            index += sprintf(&payload[index], "0x%02x ", data[i]);
        LOG_D(OSA, "Encrypted message: %s\n", payload);
    }
#endif

    return 0;
}

int stream_encrypt(uint8_t algorithm, stream_cipher_t *stream_cipher, uint8_t **out)
{
    if (algorithm == EEA0_ALG_ID) {
//         return stream_encrypt_eea0(stream_cipher, out);
    } else if (algorithm == EEA1_128_ALG_ID) {
        LOG_E(OSA, "SNOW-3G algorithms are currently not implemented\n");
        return -1;
    } else if (algorithm == EEA2_128_ALG_ID) {
        return stream_encrypt_eea2(stream_cipher, out);
    }
    LOG_E(OSA, "Provided algorithm is currently not supported = %u\n", algorithm);
    return -1;
}

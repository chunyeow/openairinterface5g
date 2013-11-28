#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "assertions.h"

#include <openssl/aes.h>
#include <openssl/cmac.h>
#include <openssl/evp.h>

#include "UTIL/LOG/log.h"

#include "osa_defs.h"
#include "osa_internal.h"

/*!
 * @brief Create integrity cmac t for a given message.
 * @param[in] stream_cipher Structure containing various variables to setup encoding
 * @param[out] out For EIA2 the output string is 32 bits long
 */
int stream_compute_integrity_eia2(stream_cipher_t *stream_cipher, uint8_t out[4])
{
    uint8_t  *m;
    uint32_t local_count;
    size_t size = 4;

    uint8_t  data[16];

    CMAC_CTX *cmac_ctx;

    uint32_t zero_bit = 0;
    uint32_t m_length;

    DevAssert(stream_cipher != NULL);
    DevAssert(stream_cipher->key != NULL);

    memset(data, 0, 16);

    zero_bit = stream_cipher->blength & 0x7;

    m_length = stream_cipher->blength >> 3;
    if (zero_bit > 0)
        m_length += 1;

    local_count = hton_int32(stream_cipher->count);

    m = calloc(m_length + 8, sizeof(uint8_t));

    memcpy(&m[0], &local_count, 4);
    m[4] = ((stream_cipher->bearer & 0x1F) << 3) | ((stream_cipher->direction & 0x01) << 2);

    memcpy(&m[8], stream_cipher->message, m_length);

    #if defined(SECU_DEBUG)
    {
        int i;
        char payload[6 * sizeof(m) + 1];
        int  index = 0;
        LOG_T(OSA, "Blength: %u, Zero bits: %u\n",
                       stream_cipher->blength, zero_bit);
        for (i = 0; i < sizeof(m); i++)
            index += sprintf(&payload[index], "0x%02x ", m[i]);
        LOG_T(OSA, "Payload: %s\n", payload);
    }
    #endif

    cmac_ctx = CMAC_CTX_new();

    CMAC_Init(cmac_ctx, stream_cipher->key, stream_cipher->key_length, EVP_aes_128_cbc(), NULL);
    CMAC_Update(cmac_ctx, m, m_length + 8);

    CMAC_Final(cmac_ctx, data, &size);

    CMAC_CTX_free(cmac_ctx);

    memcpy(out, data, 4);
    free(m);

    return 0;
}

int stream_compute_integrity(uint8_t algorithm, stream_cipher_t *stream_cipher, uint8_t out[4])
{
    if (algorithm == EIA1_128_ALG_ID) {
        LOG_E(OSA, "SNOW-3G algorithms are currently not implemented for integrity\n");
        return -1;
    } else if (algorithm == EIA2_128_ALG_ID) {
        return stream_compute_integrity_eia2(stream_cipher, out);
    }
    LOG_E(OSA, "Provided integrity algorithm is currently not supported = %u\n", algorithm);
    return -1;
}

int stream_check_integrity(uint8_t algorithm, stream_cipher_t *stream_cipher, uint8_t *expected)
{
    uint8_t result[4];

    if (algorithm != EIA0_ALG_ID) {
        if (stream_compute_integrity(algorithm, stream_cipher, result) != 0) {
            return -1;
        }

        if (memcmp(result, expected, 4) != 0) {
            LOG_E(OSA, "Mismatch found in integrity for algorithm %u,\n"
                "\tgot %02x.%02x.%02x.%02x, expecting %02x.%02x.%02x.%02x\n",
                algorithm, result[0], result[1], result[2], result[3], expected[0],
                expected[1], expected[2], expected[3]);
            return -1;
        }
    }

    /* Integrity verification succeeded */
    return 0;
}

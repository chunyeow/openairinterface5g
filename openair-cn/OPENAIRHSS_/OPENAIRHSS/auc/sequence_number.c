#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "auc.h"

uint8_t *sqn_ms_derive(uint8_t *key, uint8_t *auts, uint8_t *rand_p)
{
    /* AUTS = Conc(SQN MS ) || MAC-S
     * Conc(SQN MS ) = SQN MS ^ f5* (RAND)
     * MAC-S = f1* (SQN MS || RAND || AMF)
     */
    uint8_t  ak[6];
    uint8_t *conc_sqn_ms;
    uint8_t *mac_s;
    uint8_t  mac_s_computed[MAC_S_LENGTH];
    uint8_t *sqn_ms;
    uint8_t  amf[2] = { 0, 0 };
    int i;

    conc_sqn_ms = auts;
    mac_s = &auts[6];

    sqn_ms = malloc(SQN_LENGTH_OCTEST);

    SetOPc(opc);

    /* Derive AK from key and rand */
    f5star(key, rand_p, ak);

    for (i = 0; i < 6; i++) {
        sqn_ms[i] = ak[i] ^ conc_sqn_ms[i];
    }

    print_buffer("KEY    : ", key, 16);
    print_buffer("RAND   : ", rand_p, 16);
    print_buffer("AUTS   : ", auts, 14);
    print_buffer("AK     : ", ak, 6);
    print_buffer("SQN_MS : ", sqn_ms, 6);
    print_buffer("MAC_S  : ", mac_s, 8);

    f1star(key, rand_p, sqn_ms, amf, mac_s_computed);

    print_buffer("MAC_S +: ", mac_s_computed, 8);

    if (memcmp(mac_s_computed, mac_s, 8) != 0) {
        fprintf(stderr, "Failed to verify computed SQN_MS\n");
        free(sqn_ms);
        return NULL;
    }

    return sqn_ms;
}

/*******************************************************************************
    OpenAirInterface
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is
   included in this distribution in the file called "COPYING". If not,
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr

  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.

 *******************************************************************************/
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

    print_buffer("sqn_ms_derive() KEY    : ", key, 16);
    print_buffer("sqn_ms_derive() RAND   : ", rand_p, 16);
    print_buffer("sqn_ms_derive() AUTS   : ", auts, 14);
    print_buffer("sqn_ms_derive() AK     : ", ak, 6);
    print_buffer("sqn_ms_derive() SQN_MS : ", sqn_ms, 6);
    print_buffer("sqn_ms_derive() MAC_S  : ", mac_s, 8);

    f1star(key, rand_p, sqn_ms, amf, mac_s_computed);

    print_buffer("MAC_S +: ", mac_s_computed, 8);

    if (memcmp(mac_s_computed, mac_s, 8) != 0) {
        fprintf(stderr, "Failed to verify computed SQN_MS\n");
        free(sqn_ms);
        return NULL;
    }

    return sqn_ms;
}

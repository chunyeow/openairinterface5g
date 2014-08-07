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


#include "TLVEncoder.h"
#include "TLVDecoder.h"
#include "UeSecurityCapability.h"

int decode_ue_security_capability(UeSecurityCapability *uesecuritycapability, uint8_t iei, uint8_t *buffer, uint32_t len)
{
    int decoded = 0;
    uint8_t ielen = 0;
    if (iei > 0)
    {
        CHECK_IEI_DECODER(iei, *buffer);
        decoded++;
    }
    memset(uesecuritycapability, 0, sizeof(UeSecurityCapability));
    ielen = *(buffer + decoded);
    decoded++;
    CHECK_LENGTH_DECODER(len - decoded, ielen);
    uesecuritycapability->eea = *(buffer + decoded);
    decoded++;
    uesecuritycapability->eia = *(buffer + decoded);
    decoded++;
    if (len >= decoded + 3) {
        uesecuritycapability->umts_present = 1;
        uesecuritycapability->uea = *(buffer + decoded);
        decoded++;
        uesecuritycapability->uia = *(buffer + decoded) & 0x7f;
        decoded++;
        if (len == decoded + 4) {
            uesecuritycapability->gprs_present = 1;
            uesecuritycapability->gea = *(buffer + decoded) & 0x7f;
            decoded++;
        }
    }
#if defined (NAS_DEBUG)
    dump_ue_security_capability_xml(uesecuritycapability, iei);
#endif
    return decoded;
}
int encode_ue_security_capability(UeSecurityCapability *uesecuritycapability, uint8_t iei, uint8_t *buffer, uint32_t len)
{
    uint8_t *lenPtr;
    uint32_t encoded = 0;
    /* Checking IEI and pointer */
    CHECK_PDU_POINTER_AND_LENGTH_ENCODER(buffer, UE_SECURITY_CAPABILITY_MINIMUM_LENGTH, len);
#if defined (NAS_DEBUG)
    dump_ue_security_capability_xml(uesecuritycapability, iei);
#endif
    if (iei > 0)
    {
        *buffer = iei;
        encoded++;
    }
    lenPtr  = (buffer + encoded);
    encoded ++;
    *(buffer + encoded) = uesecuritycapability->eea;
    encoded++;
    *(buffer + encoded) =  uesecuritycapability->eia;
    encoded++;
    if (uesecuritycapability->umts_present) {
        *(buffer + encoded) = uesecuritycapability->uea;
        encoded++;
        *(buffer + encoded) = 0x00 |
        (uesecuritycapability->uia & 0x7f);
        encoded++;
#warning "force gea to 0x60 if umts security capability present"
        //if (uesecuritycapability->gprs_present) {
            *(buffer + encoded) = 0x00 | 0x60;
            //(uesecuritycapability->gea & 0x7f);
            encoded++;
        //}
    }
    *lenPtr = encoded - 1 - ((iei > 0) ? 1 : 0);
    return encoded;
}

void dump_ue_security_capability_xml(UeSecurityCapability *uesecuritycapability, uint8_t iei)
{
    printf("<Ue Security Capability>\n");
    if (iei > 0)
        /* Don't display IEI if = 0 */
        printf("    <IEI>0x%X</IEI>\n", iei);
    printf("    <EEA>%u</EEA>\n", uesecuritycapability->eea);
    printf("    <EIA>%u</EIA>\n", uesecuritycapability->eia);
    if (uesecuritycapability->umts_present == 1) {
        printf("    <UEA>%u</UEA>\n", uesecuritycapability->uea);
        printf("    <UIA>%u</UIA>\n", uesecuritycapability->uia);
    }
    if (uesecuritycapability->gprs_present == 1) {
        printf("    <GEA>%u</GEA>\n", uesecuritycapability->gea);
    }
    printf("</Ue Security Capability>\n");
}


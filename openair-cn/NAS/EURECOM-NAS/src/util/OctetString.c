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
#include <string.h>

#include "TLVEncoder.h"
#include "TLVDecoder.h"
#include "OctetString.h"

OctetString* dup_octet_string(OctetString *octetstring)
{
    OctetString *os_p = NULL;
    if (octetstring) {
        os_p = calloc(1,sizeof(OctetString));
        os_p->length = octetstring->length;
        os_p->value = malloc(octetstring->length+1);
        memcpy(os_p->value, octetstring->value, octetstring->length);
        os_p->value[octetstring->length] = '\0';
    }

    return os_p;
}


void free_octet_string(OctetString *octetstring)
{
    if (octetstring) {
        if (octetstring->value) free(octetstring->value);
        octetstring->value  = NULL;
        octetstring->length = 0;
        free(octetstring);
    }
}


int encode_octet_string(OctetString *octetstring, uint8_t *buffer, uint32_t buflen)
{
    CHECK_PDU_POINTER_AND_LENGTH_ENCODER(buffer, octetstring->length, buflen);
    memcpy((void*)buffer, (void*)octetstring->value, octetstring->length);
    return octetstring->length;
}

int decode_octet_string(OctetString *octetstring, uint16_t pdulen, uint8_t *buffer, uint32_t buflen)
{
    if (buflen < pdulen)
        return -1;
    octetstring->length = pdulen;
    octetstring->value = malloc(sizeof(uint8_t) * (pdulen+1));
    memcpy((void*)octetstring->value, (void*)buffer, pdulen);
    octetstring->value[pdulen] = '\0';
    return octetstring->length;
}

void dump_octet_string_xml(OctetString *octetstring)
{
    int i;
    printf("    <Length>%u</Length>\n    <values>", octetstring->length);
    for (i = 0; i < octetstring->length; i++)
        printf("0x%x ", octetstring->value[i]);
    printf("</values>\n");
}

void dump_octet_string(OctetString *octetstring)
{
    int i;
    printf("    <Length=%u><values>", octetstring->length);
    for (i = 0; i < octetstring->length; i++)
        printf("0x%x ", octetstring->value[i]);
    printf("</values>\n");
}

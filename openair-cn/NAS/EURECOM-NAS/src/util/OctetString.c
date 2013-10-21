#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TLVEncoder.h"
#include "TLVDecoder.h"
#include "OctetString.h"

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


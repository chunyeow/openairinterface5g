#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#include "TLVEncoder.h"
#include "TLVDecoder.h"
#include "ProtocolConfigurationOptions.h"

int decode_protocol_configuration_options(ProtocolConfigurationOptions *protocolconfigurationoptions, uint8_t iei, uint8_t *buffer, uint32_t len)
{
    int decoded = 0;
    uint8_t ielen = 0;
    int decode_result;
    if (iei > 0)
    {
        CHECK_IEI_DECODER(iei, *buffer);
        decoded++;
    }
    ielen = *(buffer + decoded);
    decoded++;
    CHECK_LENGTH_DECODER(len - decoded, ielen);
    if (((*buffer >> 7) & 0x1) != 1)
    {
        errorCodeDecoder = TLV_DECODE_VALUE_DOESNT_MATCH;
        return TLV_DECODE_VALUE_DOESNT_MATCH;
    }
    protocolconfigurationoptions->configurationprotol = (*(buffer + decoded) >> 1) & 0x7;
    //IES_DECODE_U16(protocolconfigurationoptions->protocolid, *(buffer + decoded));
    IES_DECODE_U16(buffer, decoded, protocolconfigurationoptions->protocolid);
    protocolconfigurationoptions->lengthofprotocolid = *(buffer + decoded);
    decoded++;
    if ((decode_result = decode_octet_string(&protocolconfigurationoptions->protocolidcontents, ielen, buffer + decoded, len - decoded)) < 0)
        return decode_result;
    else
        decoded += decode_result;
#if defined (NAS_DEBUG)
    dump_protocol_configuration_options_xml(protocolconfigurationoptions, iei);
#endif
    return decoded;
}
int encode_protocol_configuration_options(ProtocolConfigurationOptions *protocolconfigurationoptions, uint8_t iei, uint8_t *buffer, uint32_t len)
{
    uint8_t *lenPtr;
    uint32_t encoded = 0;
    int encode_result;
    /* Checking IEI and pointer */
    CHECK_PDU_POINTER_AND_LENGTH_ENCODER(buffer, PROTOCOL_CONFIGURATION_OPTIONS_MINIMUM_LENGTH, len);
#if defined (NAS_DEBUG)
    dump_protocol_configuration_options_xml(protocolconfigurationoptions, iei);
#endif
    if (iei > 0)
    {
        *buffer = iei;
        encoded++;
    }
    lenPtr  = (buffer + encoded);
    encoded ++;
    *(buffer + encoded) = 0x00 | (1 << 7) |
    (protocolconfigurationoptions->configurationprotol & 0x7);
    encoded++;
    IES_ENCODE_U16(buffer, encoded, protocolconfigurationoptions->protocolid);
    *(buffer + encoded) = protocolconfigurationoptions->lengthofprotocolid;
    encoded++;
    if ((encode_result = encode_octet_string(&protocolconfigurationoptions->protocolidcontents, buffer + encoded, len - encoded)) < 0)
        return encode_result;
    else
        encoded += encode_result;
    *lenPtr = encoded - 1 - ((iei > 0) ? 1 : 0);
    return encoded;
}

void dump_protocol_configuration_options_xml(ProtocolConfigurationOptions *protocolconfigurationoptions, uint8_t iei)
{
    printf("<Protocol Configuration Options>\n");
    if (iei > 0)
        /* Don't display IEI if = 0 */
        printf("    <IEI>0x%X</IEI>\n", iei);
    printf("    <Configuration protol>%u</Configuration protol>\n", protocolconfigurationoptions->configurationprotol);
    printf("    <Protocol ID>%u</Protocol ID>\n", protocolconfigurationoptions->protocolid);
    printf("    <Length of protocol ID>%u</Length of protocol ID>\n", protocolconfigurationoptions->lengthofprotocolid);
    dump_octet_string_xml(&protocolconfigurationoptions->protocolidcontents);
    printf("</Protocol Configuration Options>\n");
}


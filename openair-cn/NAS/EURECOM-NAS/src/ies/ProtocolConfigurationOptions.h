#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "OctetString.h"

#ifndef PROTOCOL_CONFIGURATION_OPTIONS_H_
#define PROTOCOL_CONFIGURATION_OPTIONS_H_

#define PROTOCOL_CONFIGURATION_OPTIONS_MINIMUM_LENGTH 3
#define PROTOCOL_CONFIGURATION_OPTIONS_MAXIMUM_LENGTH 253

typedef struct ProtocolConfigurationOptions_tag {
    uint8_t  configurationprotol:3;
    uint16_t protocolid;
    uint8_t  lengthofprotocolid;
    OctetString protocolidcontents;
} ProtocolConfigurationOptions;

int encode_protocol_configuration_options(ProtocolConfigurationOptions *protocolconfigurationoptions, uint8_t iei, uint8_t *buffer, uint32_t len);

int decode_protocol_configuration_options(ProtocolConfigurationOptions *protocolconfigurationoptions, uint8_t iei, uint8_t *buffer, uint32_t len);

void dump_protocol_configuration_options_xml(ProtocolConfigurationOptions *protocolconfigurationoptions, uint8_t iei);

#endif /* PROTOCOL CONFIGURATION OPTIONS_H_ */


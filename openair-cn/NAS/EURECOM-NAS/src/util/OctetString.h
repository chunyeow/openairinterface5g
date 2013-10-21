#include <stdint.h>

#ifndef OCTET_STRING_H_
#define OCTET_STRING_H_

typedef struct OctetString_tag {
        uint32_t  length;
        uint8_t  *value;
} OctetString;

int encode_octet_string(OctetString *octetstring, uint8_t *buffer, uint32_t len);

int decode_octet_string(OctetString *octetstring, uint16_t pdulen, uint8_t *buffer, uint32_t buflen);

void dump_octet_string_xml(OctetString *octetstring);

#endif /* OCTET_STRING_H_ */


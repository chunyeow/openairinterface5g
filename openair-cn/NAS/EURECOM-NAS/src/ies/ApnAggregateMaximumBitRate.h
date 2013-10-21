#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "OctetString.h"

#ifndef APN_AGGREGATE_MAXIMUM_BIT_RATE_H_
#define APN_AGGREGATE_MAXIMUM_BIT_RATE_H_

#define APN_AGGREGATE_MAXIMUM_BIT_RATE_MINIMUM_LENGTH 4
#define APN_AGGREGATE_MAXIMUM_BIT_RATE_MAXIMUM_LENGTH 8

typedef struct ApnAggregateMaximumBitRate_tag {
    uint8_t  apnambrfordownlink;
    uint8_t  apnambrforuplink;
} ApnAggregateMaximumBitRate;

int encode_apn_aggregate_maximum_bit_rate(ApnAggregateMaximumBitRate *apnaggregatemaximumbitrate, uint8_t iei, uint8_t *buffer, uint32_t len);

int decode_apn_aggregate_maximum_bit_rate(ApnAggregateMaximumBitRate *apnaggregatemaximumbitrate, uint8_t iei, uint8_t *buffer, uint32_t len);

void dump_apn_aggregate_maximum_bit_rate_xml(ApnAggregateMaximumBitRate *apnaggregatemaximumbitrate, uint8_t iei);

#endif /* APN AGGREGATE MAXIMUM BIT RATE_H_ */


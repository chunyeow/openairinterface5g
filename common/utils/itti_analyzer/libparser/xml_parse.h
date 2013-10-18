#include "../rc.h"
#include "types.h"

#ifndef XML_PARSE_H_
#define XML_PARSE_H_

int xml_parse_file(const char *filename);

int xml_parse_buffer(const char *xml_buffer, const int size);

int dissect_signal(const uint32_t message_number);

#endif  /* XML_PARSE_H_ */

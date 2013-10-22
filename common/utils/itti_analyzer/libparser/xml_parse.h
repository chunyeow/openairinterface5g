#include "rc.h"
#include "types.h"

#ifndef XML_PARSE_H_
#define XML_PARSE_H_

extern types_t *root;

int xml_parse_file(const char *filename);

int xml_parse_buffer(const char *xml_buffer, const int size);

int dissect_signal(buffer_t *buffer);

#endif  /* XML_PARSE_H_ */

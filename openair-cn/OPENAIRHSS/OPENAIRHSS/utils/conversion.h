#ifndef CONVERSION_H_
#define CONVERSION_H_

void hexa_to_ascii(uint8_t *from, char *to, size_t length);
int ascii_to_hex(uint8_t *dst, const char *h);
int bcd_to_hex(uint8_t *dst, const char *h, int h_length);

#endif /* CONVERSION_H_ */

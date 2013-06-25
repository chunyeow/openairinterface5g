#include <stdint.h>


uint32_t crc32_table[256];

void init_crc32() {

  uint32_t i,j;
  uint32_t crc;

  for (i=0;i<256;i++) {
    crc=i;
    for (j=0;j<8;j++) {
      crc = (crc>>1); 
      if ((crc&1) > 0)
        crc ^= 0xedb88320;
    }
    crc32_table[i]=crc;
  }
}

void crc32(uint8_t *data,uint32_t *crc,int len) {

  int i;

  for (i=0;i<len;i++)
    *crc = (*crc>>8)^crc32_table[(*crc^data[i])&0xff];

}

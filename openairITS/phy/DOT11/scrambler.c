#include <stdint.h>

uint8_t scrambler[127*8] = {0x70,
			    0x4f,
			    0x93,
			    0x40,
			    0x64,
			    0x74,
			    0x6d,
			    0x30,
			    0x2b,
			    0xe7,
			    0x2d,
			    0x54,
			    0x5f,
			    0x8a,
			    0x1d,
			    0xff};

void init_scrambler() {

  int i;

  for (i=16;i<(127*8);i++) {
    scrambler[i] = (scrambler[i-16]>>1) + ((scrambler[i-15]&1)<<7);
  }
}

#include <stdint.h>

int interleaver_bpsk[48];
int interleaver_qpsk[96];
int interleaver_16qam[192];
int interleaver_64qam[288];

extern int Ncbps[8];

void init_interleavers() {

  int i,j,k;

  // BPSK
  for (k=0;k<48;k++) {
    i=(3*(k&15)) + (k>>4);
    interleaver_bpsk[k] = i;
  }
  // QPSK
  for (k=0;k<96;k++) {
    i=(6*(k&15)) + (k>>4);
    interleaver_qpsk[k] = i;
  }
  // 16QAM
  for (k=0;k<192;k++) {
    i=(12*(k&15)) + (k>>4);
    j=((i>>1)<<1) + ((i + 192 - (i/12))&1);
    interleaver_16qam[k] = j;
  }
  // 64QAM
  for (k=0;k<288;k++) {
    i=(18*(k&15)) + (k>>4);
    j=((i/3)*3) + ((i + 288 - (i/12))&1);
    interleaver_64qam[k] = j;
  }
}

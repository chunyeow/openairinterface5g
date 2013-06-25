//#define BIGPHYS_NUMPAGES 8192
#define BIGPHYS_NUMPAGES 32768

// one page is 4096 bytes

void *bigphys_malloc(int);

void reserve_mem(unsigned long buffer,unsigned long size);

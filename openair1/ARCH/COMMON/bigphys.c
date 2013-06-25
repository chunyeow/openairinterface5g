#include "defs.h"
#include "linux/kernel.h"
#include "linux/module.h"

#ifdef BIGPHYSAREA
#ifdef ARCH_64 
char *bigphys_ptr,*bigphys_current;
#else //ARCH_64
unsigned int bigphys_ptr,bigphys_current;
#endif //ARCH_64

extern int exit_openair;

// return pointer to memory in big physical area aligned to 16 bytes

void *bigphys_malloc(n) {

  //printk("[BIGPHYSAREA] Calling bigphys_malloc\n");

  int n2 = n + ((16-(n%16))%16);

  bigphys_current += n2;

  if (bigphys_current > bigphys_ptr + (BIGPHYS_NUMPAGES*4096)) {
    printk("[BIGPHYS][FATAL] Memory overload!!!!! Exiting.\n");
    exit_openair = 1;
  }

#ifdef ARCH_64 
  //printk("[BIGPHYSAREA] Allocated Memory %d\n",bigphys_current-bigphys_ptr);
  return ((void *)(bigphys_current - (char *)n2));
#else //ARCH_64
  //printk("[BIGPHYSAREA] Allocated Memory %d\n",bigphys_current-bigphys_ptr);
  return ((void *)(bigphys_current - n2));
#endif //ARCH_64
}

EXPORT_SYMBOL(bigphys_malloc);
#endif




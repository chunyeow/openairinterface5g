#include <asm/io.h>
#include <asm/bitops.h>
#include <asm/uaccess.h>
#include <asm/segment.h>
#include <asm/page.h>
#include <asm/delay.h>

#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/mm.h>
#include <linux/mman.h>

#include <linux/slab.h>
//#include <linux/config.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/fs.h>

#include <linux/errno.h>

#ifdef KERNEL2_6
//#include <linux/config.h>
#include <linux/slab.h>
#endif

// Function to set reserved bit on pages to be mapped to user-space by mmap
void reserve_mem(unsigned long buffer,unsigned long size) {

  unsigned long virt_addr;
  //  printk("[openair][DEVICE] Reserving %p, size %d bytes\n",buffer,size);

  for (virt_addr=(unsigned long)buffer; 
       virt_addr<(unsigned long)buffer+size;
       virt_addr+=PAGE_SIZE)
    {
      /* reserve all pages to make them remapable */
      SetPageReserved(virt_to_page(virt_addr));
    }    
}


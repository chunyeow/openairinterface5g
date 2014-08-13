/*******************************************************************************
    OpenAirInterface 
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is 
   included in this distribution in the file called "COPYING". If not, 
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/
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


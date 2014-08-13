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
#include "ARCH/COMMON/defs.h"
#include "extern.h"
#include "defs.h"

#define PCI_FIFO_NO 60
#define PCI_FIFO_MAX_STRING_SIZE 500
#define PCI_FIFO_PRINTF_SIZE 1024

void pci_printk_fifo_init() {
  printk ("[OPENAIR1] PCI_PRINTK_FIFO INIT\n");
  rtf_create (PCI_FIFO_NO, PCI_FIFO_PRINTF_SIZE);
}

void
pci_printk_fifo_clean_up (void)
{
//-----------------------------------------------------------------------------
  rtf_destroy (PCI_FIFO_NO);
}


void exmimo_firmware_init() {
  size_t size=0;

  // increase exmimo_pci_interface_bot to multiple of 128 bytes 
  size = sizeof(exmimo_pci_interface_bot);
  size = size >> 7;
  size++;
  size = size << 7;

  exmimo_pci_bot = (exmimo_pci_interface_bot *)bigphys_malloc(size);
  printk("Intializing EXMIMO firmware support (exmimo_pci_bot at %p)\n",exmimo_pci_bot);
  exmimo_pci_bot->firmware_block_ptr = virt_to_phys((unsigned int*)bigphys_malloc(262144));
  printk("firmware_code_block_ptr : %x\n",exmimo_pci_bot->firmware_block_ptr);
  exmimo_pci_bot->printk_buffer_ptr = virt_to_phys((unsigned int*)bigphys_malloc(4096));
  printk("printk_buffer_ptr : %x\n",exmimo_pci_bot->printk_buffer_ptr);

  exmimo_pci_interface = (exmimo_pci_interface_t *)bigphys_malloc(sizeof(exmimo_pci_interface_t)+64*4/*overhead to allow DMA transfers of 64 DW*/); 
  exmimo_pci_bot->pci_interface_ptr = virt_to_phys((unsigned int*)exmimo_pci_interface);
  printk("pci_interface_ptr : %x\n",exmimo_pci_bot->pci_interface_ptr);  

  pci_printk_fifo_init();

  iowrite32((u32)(virt_to_phys(exmimo_pci_bot)),(bar[0]+0x1c));
  iowrite32(0,(bar[0]+0x20));

  openair_dma(0,EXMIMO_PCIE_INIT);

}

void pci_fifo_printk() {

  char *buffer = (char *)phys_to_virt(exmimo_pci_bot->printk_buffer_ptr);
  unsigned int len = ((unsigned int *)buffer)[0];
  unsigned int off=0,i;
  unsigned char *dword;
  unsigned char tmp;

  printk("In pci_fifo_printk : buffer %p, len %d\n",buffer,len);
  if ((len&3)>0)
    off=1;

  for (i=0;i<(off+(len>>2));i++) {
    dword = &((unsigned char *)buffer)[(1+i)<<2];
    tmp = dword[3];
    dword[3] = dword[0];
    dword[0] = tmp;
    tmp = dword[2];
    dword[2] = dword[1];
    dword[1] = tmp;
  }

  rtf_put(PCI_FIFO_NO,
	  &buffer[4],len);

}

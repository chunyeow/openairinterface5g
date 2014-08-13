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
#ifndef EXMIMO_FW_H
#define EXMIMO_FW_H

typedef struct {
  unsigned int firmware_block_ptr;
  unsigned int printk_buffer_ptr;
  unsigned int pci_interface_ptr;
} exmimo_pci_interface_bot;

#define EXMIMO_NOP                  0x9999

#define EXMIMO_PCIE_INIT  0x0000
#define EXMIMO_FW_INIT    0x0001
#define EXMIMO_CLEAR_BSS  0x0002
#define EXMIMO_START_EXEC 0x0003
#define EXMIMO_REBOOT     0x0004
#define EXMIMO_CONFIG     0x0005
#define EXMIMO_GET_FRAME  0x0006
#define EXMIMO_START_RT_ACQUISITION 0x0007
#define EXMIMO_STOP       0x0008


#define SLOT_INTERRUPT 0x1111
#define PCI_PRINTK 0x2222
#define GET_FRAME_DONE 0x3333
void pci_fifo_printk(void);

#endif

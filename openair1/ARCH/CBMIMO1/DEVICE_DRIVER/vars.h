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
#ifndef __CBMIMO1_VARS_H__
#define __CBMIMO1_VARS_H__
#ifndef USER_MODE
#define __NO_VERSION__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>

#ifdef KERNEL2_6
//#include <linux/config.h>
#include <linux/slab.h>
#endif

#include "defs.h"

unsigned int openair_irq;

u32 openair_irq_enabled=0;

struct pci_dev *pdev[4];

void __iomem *bar[4];
resource_size_t mmio_start,mmio_length;
unsigned int mmio_flags;

unsigned int vid,did;

char card,master_id;

int major;


//dma_addr_t dummy_dma_ptr;

unsigned int pci_buffer[4][2*NB_ANTENNAS_RX];
unsigned int mbox;

//unsigned short NODE_ID[1];
//EXPORT_SYMBOL(NODE_ID);

#endif

#include "cbmimo1_pci.h"

PCI_interface_t *pci_interface[4];
char number_of_cards;

exmimo_pci_interface_bot *exmimo_pci_bot;
exmimo_pci_interface_t *exmimo_pci_interface;

#ifdef RTAI_ENABLED
s32 *inst_cnt_ptr = NULL;
SEM *oai_semaphore = NULL;
RT_TASK *lxrt_task;
#endif
#endif

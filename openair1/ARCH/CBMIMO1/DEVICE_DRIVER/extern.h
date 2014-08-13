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
#ifndef __CBMIMO1_EXTERN_H__
#define __CBMIMO1_EXTERN_H__
#ifndef USER_MODE
#define __NO_VERSION__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>

#ifdef KERNEL2_6
//#include <linux/config.h>
#include <linux/slab.h>
#endif

//#include "pci_commands.h"
#include "defs.h"


extern struct pci_dev *pdev[4];
extern void __iomem *bar[4];


extern char card,master_id;

extern int major;

extern unsigned short eedata[];

extern unsigned int openair_irq;

extern u32 openair_irq_enabled;

//extern dma_addr_t dummy_dma_ptr;

extern unsigned int pci_buffer[4][2*NB_ANTENNAS_RX];
extern unsigned int RX_DMA_BUFFER[4][NB_ANTENNAS_RX];
extern unsigned int TX_DMA_BUFFER[4][NB_ANTENNAS_TX];
extern unsigned int mbox;

extern unsigned int vid,did;

//extern unsigned short NODE_ID[1];
#endif

#include "cbmimo1_pci.h"

extern PCI_interface_t *pci_interface[4];
extern char number_of_cards;

extern exmimo_pci_interface_bot *exmimo_pci_bot;
extern exmimo_pci_interface_t *exmimo_pci_interface;

#ifdef RTAI_ENABLED
extern s32 *inst_cnt_ptr;
extern SEM *oai_semaphore;
extern RT_TASK *lxrt_task;
#endif
#endif

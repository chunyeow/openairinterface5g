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

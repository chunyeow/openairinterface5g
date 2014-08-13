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
#ifndef __CBMIMO1_DEFS_H__
#define __CBMIMO1_DEFS_H__
#ifndef USER_MODE
#define __NO_VERSION__

//#include "rt_compat.h"

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

#ifdef KERNEL2_4
#include <linux/malloc.h>
#include <linux/wrapper.h>
#endif

#ifdef RTAI_ENABLED
#include <asm/rtai.h>
#include <rtai.h>
#include <rtai_posix.h>
#include <rtai_fifos.h>
#include <rtai_math.h>
#include <rtai_sem.h>
#endif //RTAI_ENABLED

#ifdef BIGPHYSAREA
#include <linux/bigphysarea.h>
#endif 

#include "PHY/impl_defs_lte.h"
#include "cbmimo1_device.h"

#include "from_grlib_softconfig.h"
#include "from_grlib_softregs.h"
#include "linux/moduleparam.h"


/*------------------------------------------------*/
/*   Prototypes                                   */
/*------------------------------------------------*/
int openair_device_open    (struct inode *inode,struct file *filp);
int openair_device_release (struct inode *inode,struct file *filp);
int openair_device_mmap    (struct file *filp, struct vm_area_struct *vma);
int openair_device_ioctl   (struct inode *inode,struct file *filp, unsigned int cmd, unsigned long arg);

void openair_generate_ofdm(void);
void openair_generate_fs4(unsigned char);

void openair_get_frame(unsigned char card_id);

int openair_dma(unsigned char card_id, unsigned int cmd);

int setup_regs(unsigned char card_id, LTE_DL_FRAME_PARMS *frame_parms);

void exmimo_firmware_init(void);

void dump_config(void);

int add_chbch_stats(void);
void remove_chbch_stats(void);
void remove_openair_stats(void);
int add_openair1_stats(void);
int fifo_printf(const char *fmt,...);
void fifo_printf_clean_up(void);
void fifo_printf_init(void); 

void pci_printk_fifo_init(void);
void pci_printk_fifo_clean_up (void);
#endif

void openair_set_rx_rf_mode(unsigned char card_id,unsigned int arg);
void openair_set_tx_gain_openair(unsigned char card_id,unsigned char txgain00,unsigned char txgain10,unsigned char txgain01, unsigned char txgain11);
void openair_set_rx_gain_openair(unsigned char card_id,unsigned char rxgain00,unsigned char rxgain10,unsigned char rxgain01,unsigned char rxgain11);
void openair_set_lo_freq_openair(unsigned char card_id,char freq0,char freq1);
void openair_set_rx_gain_cal_openair(unsigned char card_id,unsigned int gain_dB);
int openair_set_freq_offset(unsigned char card_id,int freq_offset);
void openair_set_tcxo_dac(unsigned char card_id,unsigned int);
#endif

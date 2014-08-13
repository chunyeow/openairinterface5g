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
#ifndef USER_MODE
#define __NO_VERSION__

//#include "rt_compat.h"

#ifdef RTAI_ENABLED
#include <rtai.h>
#include <rtai_posix.h>
#include <rtai_fifos.h>
#endif


#include <asm/io.h>
#include <asm/bitops.h>
#include <asm/uaccess.h>
#include <asm/segment.h>
#include <asm/page.h>
#include <asm/delay.h>

#include <linux/init.h>
#include <linux/module.h>
//#include <linux/malloc.h>
#endif


#include "openair_device.h"
#include "openair_device_extern.h"





int test_FPGA_regs(void ) {

#ifndef NOCARD_TEST
  int readback;

  printk("[openair][TEST_FPGA_REGS] : writing %x to ADC0_HEAD\n",0x12345678);
  openair_writel(0x12345678,(bar[0] + REG_BAR + ADC0_HEAD));
  readback = openair_readl((bar[0] + REG_BAR + ADC0_HEAD));

  printk("[openair][TEST_FPGA_REGS] : readback %x\n",readback);
#endif NOCARD_TEST

  return(0);
}














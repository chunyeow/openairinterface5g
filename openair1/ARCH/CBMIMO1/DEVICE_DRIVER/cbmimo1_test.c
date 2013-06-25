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














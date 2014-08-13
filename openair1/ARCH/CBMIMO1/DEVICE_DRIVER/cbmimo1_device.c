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

#endif //USER_MODE

#include "cbmimo1_device.h"
#include "defs.h"
#include "vars.h"

#include "ARCH/COMMON/defs.h"

#include "PHY/types.h"
#include "PHY/defs.h"
#include "PHY/vars.h"

#include "SCHED/defs.h"
#include "SCHED/vars.h"

#include "MAC_INTERFACE/defs.h"
#include "MAC_INTERFACE/vars.h"
#include "LAYER2/MAC/vars.h"
#ifdef OPENAIR2
#include "RRC/LITE/vars.h"
#endif
#include "UTIL/LOG/log.h"

#include "from_grlib_softconfig.h"
#include "from_grlib_softregs.h"

#include "linux/moduleparam.h"

#include "SIMULATION/ETH_TRANSPORT/vars.h"


/*------------------------------------------------*/
/*   Prototypes                                   */
/*------------------------------------------------*/
static int   openair_init_module( void );
static void  openair_cleanup_module(void);

/*------------------------------------------------*/
/*   Prototypes                                   */
/*------------------------------------------------*/
int openair_device_open    (struct inode *inode,struct file *filp);
int openair_device_release (struct inode *inode,struct file *filp);
int openair_device_mmap    (struct file *filp, struct vm_area_struct *vma);
int openair_device_ioctl   (struct inode *inode,struct file *filp, unsigned int cmd, unsigned long arg)
;

/* The variable 'updatefirmware' defined below is used by the driver at insmod time
 * to decide if whether or not it must jump directly to user firmware settled in Scratch Pad
 * Rams of the Carbus-MIMO-1 SoC (updatefirmware == 1) OR if it must wait for update of the
 * firmware (updatefirmware == 0, the default). The update of the firmware is handled through
 * a specific ioctl code.
 * The value of updatefirmware can be changed at insmod time this very simple way:
 *   $ insmod openair_rf_softmodem.ko updatefirmware=1
 * (For more information on how to transmit parameter to modules at insmod time,
 * refer to [LinuxDeviceDrivers, 3rd edition, by Corbet/Rubini/Kroah-Hartman] pp 35-36). */
static int updatefirmware = 0;
module_param(updatefirmware, bool, S_IRUGO); /* permission mask S_IRUGO is for sysfs possible entry
                                                and means "readable to all" (from include/linux/stat.h) */

extern void dummy_macphy_scheduler(unsigned char last_slot);
extern void dummy_macphy_setparams(void *params);
extern void dummy_macphy_init(void );

static void openair_cleanup(void);

#ifdef BIGPHYSAREA
extern char *bigphys_current,*bigphys_ptr;
#endif

extern int intr_in;
/*------------------------------------------------*/

static struct file_operations openair_fops = {
ioctl:openair_device_ioctl,
open: openair_device_open,
release:openair_device_release,
mmap: openair_device_mmap
};

extern int pci_enable_pcie_error_reporting(struct pci_dev *dev);
extern int pci_cleanup_aer_uncorrect_error_status(struct pci_dev *dev);


int oai_trap_handler (int vec, int signo, struct pt_regs *regs, void *dummy) {

  RT_TASK *rt_task;
  
  rt_task = rt_smp_current[rtai_cpuid()];

  printk("[openair][TRAP_HANDLER] vec %d, signo %d, task %p, ip %04x (%04x), frame %d, slot %d\n", 
         vec, signo, 
		 rt_task, 
         (unsigned int)regs->ip, 
         (unsigned int)regs->ip - (unsigned int) &bigphys_malloc, 
         openair_daq_vars.hw_frame,
         openair_daq_vars.slot_count);

  if (PHY_vars_eNB_g!=NULL)
    dump_frame_parms(&PHY_vars_eNB_g[0]->lte_frame_parms);
  else if (PHY_vars_UE_g!=NULL)
    dump_frame_parms(&PHY_vars_UE_g[0]->lte_frame_parms);

  openair_sched_exit("[openair][TRAP_HANDLER] Exiting!");

  rt_task_suspend(rt_task);

  return 1;

}




static int __init openair_init_module( void ) {
  //-----------------------------------------------------------------------------
  int res = 0;
  // unsigned long i;
  
  
  char *adr;
  int32_t temp_size;
  unsigned int readback;  


#ifndef NOCARD_TEST     
  //------------------------------------------------
  // Look for GRPCI
  //------------------------------------------------
  unsigned int i=0;  
  printk("[openair][INIT_MODULE][INFO]: Looking for GRLIB (%x,%x)\n",
	 FROM_GRLIB_CFG_PCIVID,
	 FROM_GRLIB_CFG_PCIDID);

  pdev[0] = pci_get_device(FROM_GRLIB_CFG_PCIVID, FROM_GRLIB_CFG_PCIDID, NULL);

  if(pdev[0]) {
    printk("[openair][INIT_MODULE][INFO]:  openair card (CBMIMO1) %d found, bus %x, primary %x, secondate %x\n",i,
	     pdev[i]->bus->number,pdev[i]->bus->primary,pdev[i]->bus->secondary);
    i=1;
    vid = FROM_GRLIB_CFG_PCIVID;
    did = FROM_GRLIB_CFG_PCIDID;
  }
  else {
    printk("[openair][INIT_MODULE][INFO]:  no CBMIMO1 card found, checking for Express MIMO:\n");
    
    pdev[0] = pci_get_device(XILINX_VENDOR, XILINX_ID, NULL);
    if(pdev[0]) {
      printk("[openair][INIT_MODULE][INFO]:  openair card (ExpressMIMO) %d found, bus %x, primary %x, secondary %x\n",i,
	     pdev[i]->bus->number,pdev[i]->bus->primary,pdev[i]->bus->secondary);
      i=1;
      vid = XILINX_VENDOR;
      did = XILINX_ID;
      
    }
    else {
      printk("[openair][INIT_MODULE][INFO]:  no card found, stopping.\n");
      return -ENODEV;
    }
  }

  // Now look for more cards on the same bus
  while (i<3) {
    pdev[i] = pci_get_device(vid,did, pdev[i-1]);
    if(pdev[i]) {
      printk("[openair][INIT_MODULE][INFO]:  openair card %d found, bus %x, primary %x, secondary %x\n",i,
	     pdev[i]->bus->number,pdev[i]->bus->primary,pdev[i]->bus->secondary);
      i++;
    }
    else {
      break;
    }
  }

  // at least one device found, enable it
  number_of_cards = i;

  for (i=0;i<number_of_cards;i++) {
    if(pci_enable_device(pdev[i])) {
      printk("[openair][INIT_MODULE][INFO]: Could not enable device %d\n",i);

      return -ENODEV;
    }
    else {
      //      pci_read_config_byte(pdev[i], PCI_INTERRUPT_PIN, &pdev[i]->pin);
      //      if (pdev[i]->pin)
      //	pci_read_config_byte(pdev[i], PCI_INTERRUPT_LINE, &pdev[i]->irq);

      printk("[openair][INIT_MODULE][INFO]: Device %d (%p)enabled, irq %d\n",i,pdev[i],pdev[i]->irq);
    }
      
      
    // Make the FPGA to a PCI master
    pci_set_master(pdev[i]);
      
      


  }

  if (vid != XILINX_VENDOR) {
    for (i=0;i<number_of_cards;i++) {
      openair_readl(pdev[i], 
		    FROM_GRLIB_CFG_GRPCI_EUR_CTRL_OFFSET, 
		    &res);
      if ((res & FROM_GRLIB_BOOT_GOK) != 0)
	printk("[openair][INIT_MODULE][INFO]: LEON3 on card %d is ok!\n",i);
      else {
	printk("[openair][INIT_MODULE][INFO]: Readback from LEON CMD %x\n",res);
	return -ENODEV;
      }
    }
  /* The boot strap of Leon is waiting for us, polling the HOK bit and 
   * waiting for us to assert it high.
   * If we also set the flag IRQ_FROM_PCI_IS_JUMP_USER_ENTRY in the PCI Irq field,
   * then it will automatically:
   *   1) set the stack pointer to the top of Data Scratch Pad Ram
   *   2) jump to Ins. Scratch Pad Ram.
   * So if the user performing the insmod of openair does not want to use
   * the default firmware, it must inform the driver by setting the boolean
   * variable 'updatefirmware' to 1/TRUE (by default, this variable is statically
   * equal to 0/FALSE.
   * In the latter case (that is, updatefirmware == 1) we only set the HOK bit,
   * without asking for an auto. jump to user firmware. This way, the user can
   * later call the driver with an ioctl to ask for firmware download & jump to it.
   * In the former case (that is, updatefirmware == 0), which is the default,
   * we ask for auto. jump to user firmware.
   * (for more information on how to transmit parameter to modules at insmod time,
   * refer to [LinuxDeviceDrivers, 3rd edition, by Corbet/Rubini/Kroah-Hartman] pp 35-36). */
    for (i=0;i<number_of_cards;i++) {
      if (!updatefirmware) {
	printk("[openair][INIT_MODULE][INFO]: Card %d Setting HOK bit with auto jump to user firmware.\n",i);
	openair_writel(pdev[i], FROM_GRLIB_CFG_GRPCI_EUR_CTRL_OFFSET, FROM_GRLIB_BOOT_HOK | FROM_GRLIB_IRQ_FROM_PCI_IS_JUMP_USER_ENTRY);
      } else {
	printk("[openair][INIT_MODULE][INFO]: Setting HOK bit WITHOUT auto jump to user firmware.\n");
	openair_writel(pdev[i], FROM_GRLIB_CFG_GRPCI_EUR_CTRL_OFFSET, FROM_GRLIB_BOOT_HOK);
      }
    }
  }
  else {

    if (pci_enable_pcie_error_reporting(pdev[0]) > 0)
      printk("[openair][INIT_MODULE][INFO]: Enabled PCIe error reporting\n");
    else
      printk("[openair][INIT_MODULE][INFO]: Failed to enable PCIe error reporting\n");

    pci_cleanup_aer_uncorrect_error_status(pdev[0]);

    
    mmio_start = pci_resource_start(pdev[0], 0);

    mmio_length = pci_resource_len(pdev[0], 0);
    mmio_flags = pci_resource_flags(pdev[0], 0);

    if (check_mem_region(mmio_start,256) < 0) {
      printk("[openair][INIT_MODULE][FATAL] : Cannot get memory region 0, aborting\n");
      return(-1);
    }
    else 
	printk("[openair][INIT_MODULE][INFO] : Reserving memory region 0 : %x\n",mmio_start);

    request_mem_region(mmio_start,256,"openair_rf");
    

    bar[0] = pci_iomap(pdev[0],0,mmio_length);
    //    bar_len[i] = mm_len;
    printk("[openair][INIT_MODULE][INFO]: BAR0 card %d = %p\n",i,bar[0]);

    printk("[openair][INIT_MODULE][INFO]: Writing %x to BAR0+0x1c (PCIBASE)\n",0x12345678);

    iowrite32(0x12345678,(bar[0]+0x1c));

    readback = ioread32(bar[0]+0x1c);
    if (readback != 0x12345678) {
      printk("[openair][INIT_MODULE][INFO]: Readback of FPGA register failed (%x)\n",readback);
      release_mem_region(mmio_start,256);
      return(-1);
    }
	iowrite32((1<<8) | (1<<9) | (1<<10),bar[0]);
	udelay(1000);
    readback = ioread32(bar[0]);
	printk("CONTROL0 readback %x\n",readback);

  }
#endif //NOCARD_TEST

  //------------------------------------------------
  // Register the device
  //------------------------------------------------
  
  major = openair_MAJOR;

  if((res = register_chrdev(major, "openair", 
			    &openair_fops
			    )) < 0){
    printk("[openair][INIT_MODULE][ERROR]:  can't register char device driver, major : %d, error: %d\n", major, res);
    return -EIO;
  } else {
    printk("[openair][INIT_MODULE][INFO]:  char device driver registered major : %d\n", major);
  }

  

 


#ifdef BIGPHYSAREA
  printk("[openair][module] calling Bigphys_alloc_page for %d ...\n", BIGPHYS_NUMPAGES);
  bigphys_ptr = (char *)bigphysarea_alloc_pages(BIGPHYS_NUMPAGES,0,GFP_KERNEL);
  //bigphys_ptr = (char *)alloc_pages_exact(BIGPHYS_NUMPAGES*4096,GFP_KERNEL);
  if (bigphys_ptr == (char *)NULL) {
    printk("[openair][MODULE][ERROR] Cannot Allocate Memory for shared data\n");
    openair_cleanup();
    return -ENODEV;
  }
  else {
    printk("[openair][MODULE][INFO] Bigphys at %p\n",(void *)bigphys_ptr);

    adr = (char *) bigphys_ptr;
    temp_size = BIGPHYS_NUMPAGES*PAGE_SIZE;
    while (temp_size > 0) {
      SetPageReserved(virt_to_page(adr));
      adr += PAGE_SIZE;
      temp_size -= PAGE_SIZE;
    }
      
    bigphys_current = bigphys_ptr;
    memset(bigphys_ptr,0,BIGPHYS_NUMPAGES*PAGE_SIZE);
  }
  printk("[OPENAIR][INIT_MODULE][INIT] bigphys_ptr =%p ,bigphys_current =%p\n",bigphys_ptr,bigphys_current);
#endif //BIGPHYSAREA

  if (vid == XILINX_VENDOR)  // This is ExpressMIMO
    exmimo_firmware_init();


#ifdef RTAI_ENABLED
  rt_set_oneshot_mode();
  start_rt_timer(0);  //in oneshot mode the argument (period) is ignored
#endif //RTAI_ENABLED

  openair_daq_vars.mac_registered  = 0;
  openair_daq_vars.node_configured = 0;
  openair_daq_vars.node_running    = 0;
  printk("[OPENAIR][INIT_MODULE][INIT] openair_daq_vars set\n");

  printk("[OPENAIR][SCHED][INIT] Trying to get IRQ %d\n",pdev[0]->irq);
  if (rt_request_irq(pdev[0]->irq,
		     slot_irq_handler,
		     NULL,0) == 0) {
    rt_enable_irq(pdev[0]->irq);
    openair_irq_enabled=1;
    printk("[OPENAIR][SCHED][INIT] Got IRQ %d\n",pdev[0]->irq);

  }
  else {
    printk("[OPENAIR][SCHED][INIT] Cannot get IRQ %d for HW\n",pdev[0]->irq);
    return(-1);
    openair_irq_enabled=0;
  }


  mac_xface = malloc16(sizeof(MAC_xface));
  if (mac_xface) {
    /*
    mac_xface->macphy_scheduler = dummy_macphy_scheduler;
    mac_xface->macphy_setparams = dummy_macphy_setparams;
    mac_xface->macphy_init      = dummy_macphy_init;
    */

    printk("[OPENAIR][INIT_MODULE][INIT] mac_xface @ %p\n",mac_xface);
  }
  else {
    printk("[OPENAIR][INIT_MODULE][INIT] mac_xface cannot be allocated\n");
    openair_cleanup();
    return -1;
  }

#ifdef OPENAIR_LTE
  lte_frame_parms_g = malloc16(sizeof(LTE_DL_FRAME_PARMS));

  if (lte_frame_parms_g) {
    printk("[OPENAIR][INIT_MODULE][INIT] lte_frame_parms allocated @ %p\n",lte_frame_parms_g);
  }
  else {
    printk("[OPENAIR][INIT_MODULE][INIT] lte_frame_parms cannot be allocated\n");
    openair_cleanup();
    return -1;
  }
#endif

  printk("[openair][MODULE][INFO] OPENAIR_CONFIG %x, OPENAIR_START_1ARY_CLUSTERHEAD %x,OPENAIR_START_NODE %x\n", openair_GET_CONFIG, openair_START_1ARY_CLUSTERHEAD, _IOR('o',3,long));

  //  for (i=0;i<10;i++)
  //printk("[openair][MODULE][INFO] IOCTL %d : %x\n",i,_IOR('o',i,long));

 		
  fifo_printf_init();

  //#ifdef OPENAIR2
  //logInit();
  //#endif

  printk("[openair][MODULE][INFO] &rtai_global_heap = %p\n",&rtai_global_heap);


  // set default trap handler
  rt_set_trap_handler(oai_trap_handler);

  printk("[openair][MODULE][INFO] &bigphys_malloc = %p\n",&bigphys_malloc);

  printk("[openair][MODULE][INFO] Done init\n");



  msg("[openair][MODULE][INFO] Done init\n");
  return 0;
}

  
static void __exit openair_cleanup_module(void) {
  printk("[openair][CLEANUP MODULE]\n");

  if (vid == XILINX_VENDOR) {
    printk("[openair][CLEANUP_MODULE][INFO] Releasing mem_region %x\n",mmio_start);
    release_mem_region(mmio_start,256);
  }

  openair_cleanup();

  fifo_printf_clean_up();

  if (vid == XILINX_VENDOR)
   pci_printk_fifo_clean_up();

  //#ifdef OPENAIR2
  //logClean();
  //#endif

  
}
static void  openair_cleanup(void) {


  int i;


  unregister_chrdev(major,"openair");

#ifdef RTAI_ENABLED
  printk("[openair][CLEANUP] Cleaning PHY Variables\n");

  openair_sched_cleanup();

#ifdef DLSCH_THREAD
  cleanup_dlsch_threads();
#endif

  udelay(1000);

  phy_cleanup();

  remove_openair_stats();

#ifdef OPENAIR
  mac_top_cleanup();
#endif


#endif //RTAI_ENABLED

  for (i=0;i<number_of_cards;i++) {
    if (bar[i])
      iounmap((void *)bar[i]);
  }

  // unregister interrupt
  printk("[openair][CLEANUP] disabling interrupt\n");
  rt_disable_irq(pdev[0]->irq);
  rt_release_irq(pdev[0]->irq);
  openair_irq_enabled=0;


#ifdef BIGPHYSAREA
  if (bigphys_ptr != (char *)NULL) {
    printk("[openair][MODULE][INFO] Freeing BigPhys buffer\n");
    bigphysarea_free_pages((void *)bigphys_ptr);
    //free_pages_exact((void *)bigphys_ptr,BIGPHYS_NUMPAGES*4096);
  }
#endif //BIGPHYSAREA

#ifdef RTAI_ENABLED
  stop_rt_timer ();             //stop the timer
  printk("[openair][MODULE][INFO] RTAI Timer stopped\n");
#endif //RTAI_ENABLED

  //printk("[openair] intr_in = %d\n",intr_in);

}



MODULE_AUTHOR
  ("Lionel GAUTHIER <lionel.gauthier@eurecom.fr>, Raymond KNOPP <raymond.knopp@eurecom.fr>, Aawatif MENOUNI <aawatif.menouni@eurecom.fr>,Dominique NUSSBAUM <dominique.nussbaum@eurecom.fr>, Michelle WETTERWALD <michelle.wetterwald@eurecom.fr>, Florian KALTENBERGER <florian.kaltenberger@eurecom.fr>");
MODULE_DESCRIPTION ("openair CardBus driver");
MODULE_LICENSE ("GPL");
module_init (openair_init_module);
module_exit (openair_cleanup_module);

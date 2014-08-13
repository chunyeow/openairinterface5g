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

#endif

#include "cbmimo1_device.h"
#include "defs.h"
#include "../../COMMON/defs.h"
#include "extern.h"

#include "PHY/defs.h"
#include "PHY/extern.h"
#include "PHY/INIT/defs.h"
#include "SCHED/defs.h"
#include "SCHED/extern.h"
#include "MAC_INTERFACE/defs.h"
#include "MAC_INTERFACE/extern.h"
#include "RRC/LITE/defs.h"
#include "RRC/LITE/extern.h"

#ifdef OPENAIR2
#include "LAYER2/MAC/extern.h"
extern int transmission_mode_rrc; //fixme
#endif

#include "from_grlib_softconfig.h"
#include "from_grlib_softregs.h"
#include "cbmimo1_pci.h"

extern int rx_sig_fifo;
extern int intr_cnt2;

void set_taus_seed(void);

int dummy_cnt = 0;

#ifdef BIGPHYSAREA
extern int bigphys_ptr;
#endif

//-----------------------------------------------------------------------------
int openair_device_open (struct inode *inode,struct file *filp) {
  //-----------------------------------------------------------------------------
  printk("[openair][MODULE]  openair_open()\n");
#ifdef KERNEL2_4
 MOD_INC_USE_COUNT;
#endif //
  return 0;
}
//-----------------------------------------------------------------------------
int openair_device_release (struct inode *inode,struct file *filp) {
  //-----------------------------------------------------------------------------
  printk("[openair][MODULE]  openair_release(), MODE = %d\n",openair_daq_vars.mode);
#ifdef KERNEL2_4
 MOD_DEC_USE_COUNT;
#endif // KERNEL2_4
  return 0;
}
//-----------------------------------------------------------------------------
int openair_device_mmap(struct file *filp, struct vm_area_struct *vma) {
  //-----------------------------------------------------------------------------
  
  unsigned long phys,pos;
  unsigned long start = (unsigned long)vma->vm_start; 
  unsigned long size = (unsigned long)(vma->vm_end-vma->vm_start); 
  int i;

  
  printk("[openair][MMAP]  called (%x,%x,%x) prot %x\n", 
	 vma->vm_start, 
	 vma->vm_end, 
	 size,
	 vma->vm_page_prot);
  

#ifdef BIGPHYSAREA  
  
  vma->vm_flags |= VM_RESERVED;

  /* if userspace tries to mmap beyond end of our buffer, fail */ 

  if (size>BIGPHYS_NUMPAGES*PAGE_SIZE) {
    printk("[openair][MMAP][ERROR] Trying to map more than %d bytes (%d)\n",
	   (unsigned int)(BIGPHYS_NUMPAGES*PAGE_SIZE),
	   (unsigned int)size);
    return -EINVAL;
  }


  pos = (unsigned long) bigphys_ptr;
  phys = virt_to_phys((void *)pos);
  
  printk("[openair][MMAP]  WILL START MAPPING AT %p (%p) \n", (void*)pos,virt_to_phys(pos));
  
  /* loop through all the physical pages in the buffer */ 
  /* Remember this won't work for vmalloc()d memory ! */

  if (remap_pfn_range(vma, 
		      start, 
		      phys>>PAGE_SHIFT, 
		      vma->vm_end-vma->vm_start, 
		      vma->vm_page_prot)) {
    
    printk("[openair][MMAP] ERROR EAGAIN\n");
    return -EAGAIN;
  }

  /*
  for (i=0;i<16;i++)
    printk("[openair][MMAP] rxsig %d = %x\n",i,((unsigned int*)RX_DMA_BUFFER[0][0])[i]);
  */
/*
  for (i=0;i<16;i++)
    ((unsigned int*)RX_DMA_BUFFER[0][0])[i] = i;

  for (i=0;i<16;i++)
    ((unsigned int*)TX_DMA_BUFFER[0][0])[i] = i;

*/
#endif //BIGPHYSAREA
  return 0; 
}


//-----------------------------------------------------------------------------
int openair_device_ioctl(struct inode *inode,struct file *filp, unsigned int cmd, unsigned long arg) {
  /* arg is not meaningful if no arg is passed in user space */
  //-----------------------------------------------------------------------------
  int ret=-1;
  int i,j,aa;
  int ue,eNb;
  
  void *arg_ptr = (void *)arg;

  unsigned char *scale;
  unsigned char scale_mem;
  int tmp;
  unsigned int ltmp;

#define invert4(x)        {ltmp=x; x=((ltmp & 0xff)<<24) | ((ltmp & 0xff00)<<8) | \
				     ((ltmp & 0xff0000)>>8) | ((ltmp & 0xff000000)>>24); }

  static unsigned int fmw_off;
  static unsigned int update_firmware_command;
  static unsigned int update_firmware_address;
  static unsigned int update_firmware_length;
  static unsigned int* update_firmware_kbuffer;
  static unsigned int* __user update_firmware_ubuffer;
  static unsigned int update_firmware_start_address;
  static unsigned int update_firmware_stack_pointer;
  static unsigned int update_firmware_bss_address;
  static unsigned int update_firmware_bss_size;
  unsigned int sparc_tmp_0;
  unsigned int sparc_tmp_1;
  static unsigned int lendian_length;
  static unsigned int bendian_fmw_off;
  unsigned int ioctl_ack_cnt = 0;

  TX_VARS dummy_tx_vars;
  TX_RX_VARS dummy_tx_rx_vars;
  LTE_DL_FRAME_PARMS *frame_parms = lte_frame_parms_g;
  unsigned short node_id;

  u8 buffer[100];
  u8 size;
  unsigned int *fw_block;

  unsigned int get_frame_cnt=0;

  scale = &scale_mem;
  /*
  printk("[openair][IOCTL] In ioctl(), ioctl = %x (%x,%x)\n",cmd,openair_START_1ARY_CLUSTERHEAD,openair_START_NODE);
  */
  switch(cmd) {
    

  case openair_TEST_FPGA:

      break;


    //----------------------
  case openair_DUMP_CONFIG:
    //----------------------
    printk("[openair][IOCTL]     openair_DUMP_CONFIG\n");
    printk("[openair][IOCTL] sizeof(mod_sym_t)=%d\n",sizeof(mod_sym_t));
    
    set_taus_seed();
    
#ifdef RTAI_ENABLED
    copy_from_user((void*)frame_parms,arg_ptr,sizeof(LTE_DL_FRAME_PARMS));
    dump_frame_parms(frame_parms);
    printk("[openair][IOCTL] Allocating frame_parms\n");

    if (openair_daq_vars.node_configured > 0) {

      if (vid == XILINX_VENDOR) {  // This is ExpressMIMO
	rt_disable_irq(pdev[0]->irq);

	printk("[openair][IOCTL] ExpressMIMO: Triggering reset of OAI firmware\n",openair_daq_vars.node_configured);
	//exmimo_firmware_init();
	//openair_dma(0,EXMIMO_PCIE_INIT);
	ret = setup_regs(0,frame_parms);
	/*
	  pci_dma_sync_single_for_device(pdev[0], 
	  exmimo_pci_interface,
	  1024, 
	  PCI_DMA_TODEVICE);
	*/
	udelay(10000);
	//printk("freq: %d gain: %d\n",exmimo_pci_interface->rf.rf_freq_rx0,exmimo_pci_interface->rf.rx_gain00);
	openair_dma(0,EXMIMO_CONFIG);
      }
      else {
	printk("[openair][IOCTL] CBMIMO1 does not support reconfiguration!\n");
      }
      /*
      udelay(10000);
      for (i=0;i<number_of_cards;i++) { 
	ret = setup_regs(i,frame_parms);
	if (vid != XILINX_VENDOR)
	  pci_interface[i]->freq_offset = 0;
	//openair_dma(i,FROM_GRLIB_IRQ_FROM_PCI_IS_ACQ_DMA_STOP);
	
      }
      */
    }
    else {

      ret = phy_init_top(frame_parms);
      msg("[openair][IOCTL] phy_init_top done: %d\n",ret);
      
      frame_parms->twiddle_fft      = twiddle_fft;
      frame_parms->twiddle_ifft     = twiddle_ifft;
      frame_parms->rev              = rev;

      printk("twiddle_ifft=%p,rev=%p\n",twiddle_ifft,rev);
#ifdef OPENAIR1      
      phy_init_lte_top(frame_parms);
      msg("[openair][IOCTL] phy_init_lte_top done: %d\n",ret);
#endif
      if (ret < 0) {
	printk("[openair][IOCTL] Error in configuring PHY\n");
	break;
      }
      
      else {
	printk("[openair][IOCTL] PHY Configuration successful\n");
	
#ifndef OPENAIR2	  
	openair_daq_vars.mac_registered = mac_init();
	if (openair_daq_vars.mac_registered != 1)
	  printk("[openair][IOCTL] Error in configuring MAC\n");
	else 
	  printk("[openair][IOCTL] MAC Configuration successful\n");
#endif	
      }

#ifndef NOCARD_TEST
      // Initialize FPGA PCI registers
      
      openair_daq_vars.dual_tx = frame_parms->dual_tx;
      openair_daq_vars.tdd     = frame_parms->frame_type;
      openair_daq_vars.tx_rx_switch_point = TX_RX_SWITCH_SYMBOL;  //unused for FDD
      
#ifdef OPENAIR_LTE
      openair_daq_vars.freq = frame_parms->freq_idx;
      printk("[openair][IOCTL] Configuring for frequency %d\n",openair_daq_vars.freq);
#else
      openair_daq_vars.freq = ((int)(PHY_config->PHY_framing.fc_khz - 1902600)/5000)&3;
      printk("[openair][IOCTL] Configuring for frequency %d kHz (%d)\n",(unsigned int)PHY_config->PHY_framing.fc_khz,openair_daq_vars.freq);
#endif
      openair_daq_vars.freq_info = 1 + (openair_daq_vars.freq<<1) + (openair_daq_vars.freq<<4);
      openair_daq_vars.rx_gain_val = 0;
      openair_daq_vars.tcxo_dac = 256;       // PUT the card in calibrated frequency mode by putting a value > 255 in tcxo register
      openair_daq_vars.node_id = NODE;
      openair_daq_vars.mode    = openair_NOT_SYNCHED;
      openair_daq_vars.node_running = 0;
      openair_daq_vars.rx_gain_mode = DAQ_AGC_ON;
      openair_daq_vars.rx_rf_mode = 0; //RF mode 0 = mixer low gain, lna off
      
      openair_daq_vars.auto_freq_correction = 1;
      openair_daq_vars.manual_timing_advance = 0;
      openair_daq_vars.timing_advance = 19;
      if (frame_parms->mode1_flag)
	openair_daq_vars.dlsch_transmission_mode = 1;
      else
	openair_daq_vars.dlsch_transmission_mode = 2;
      openair_daq_vars.target_ue_dl_mcs = 4;
      openair_daq_vars.target_ue_ul_mcs = 4;
      openair_daq_vars.dlsch_rate_adaptation = 0;
      openair_daq_vars.ue_ul_nb_rb = 4;
      openair_daq_vars.ulsch_allocation_mode = 0;

      openair_daq_vars.is_eNB = 0;
      openair_daq_vars.hw_frame = 0;

      //mac_xface->slots_per_frame = SLOTS_PER_FRAME;
      //mac_xface->is_primary_cluster_head = 0;
      //mac_xface->is_secondary_cluster_head = 0;
      //mac_xface->cluster_head_index = 0;


      printk("[openair][IOCTL] Setting up registers\n");
      for (i=0;i<number_of_cards;i++) { 
	ret = setup_regs(i,frame_parms);
	if (vid != XILINX_VENDOR)
	  pci_interface[i]->freq_offset = 0;
	//	openair_dma(i,FROM_GRLIB_IRQ_FROM_PCI_IS_ACQ_DMA_STOP);
	
      }

      if (vid == XILINX_VENDOR) {
	openair_dma(0,EXMIMO_CONFIG);
	udelay(10000);
      }
      // usleep(10);
      ret = openair_sched_init();
      if (ret != 0)
	printk("[openair][IOCTL] Error in starting scheduler\n");
      else
	printk("[openair][IOCTL] Scheduler started\n");

      openair_daq_vars.node_configured = 1;

      // add Layer 1 stats in /proc/openair	
      add_openair1_stats();
      // rt_preempt_always(1);

#endif //NOCARD_TEST
    }
#endif // RTAI_ENABLED
    break;

#ifdef OPENAIR1
    //----------------------
  case openair_START_1ARY_CLUSTERHEAD:
    //----------------------

#ifdef RTAI_ENABLED
    printk("[openair][IOCTL]     openair_1ARY_START_CLUSTERHEAD\n");
    printk("[openair][IOCTL]     Freq corr = %d, Freq0 = %d, Freq1 = %d, NODE_ID = %d\n",*((unsigned int *)arg_ptr)&1,
	   (*((unsigned int *)arg_ptr)>>1)&7,
	   (*((unsigned int *)arg_ptr)>>4)&7,
	   (*((unsigned int *)arg_ptr)>>7)&0xFF);


    if ( (openair_daq_vars.node_configured > 0) && 
	 (openair_daq_vars.node_running == 0)) {

      if (openair_daq_vars.node_configured==1) {

	// allocate memory for PHY
	PHY_vars_eNB_g = (PHY_VARS_eNB**) malloc16(sizeof(PHY_VARS_eNB*));
	if (PHY_vars_eNB_g == NULL) {
	  printk("[openair][IOCTL] Cannot allocate PHY_vars_eNb\n");
	  break;
	}
	PHY_vars_eNB_g[0] = (PHY_VARS_eNB*) malloc16(sizeof(PHY_VARS_eNB));
	if (PHY_vars_eNB_g[0] == NULL) {
	  printk("[openair][IOCTL] Cannot allocate PHY_vars_eNb\n");
	  break;
	}
	bzero(PHY_vars_eNB_g[0],sizeof(PHY_VARS_eNB));

	//copy frame parms
	memcpy((void*) &PHY_vars_eNB_g[0]->lte_frame_parms, (void*) frame_parms, sizeof(LTE_DL_FRAME_PARMS));

	dump_frame_parms(&PHY_vars_eNB_g[0]->lte_frame_parms);
 
	if (  phy_init_lte_eNB(PHY_vars_eNB_g[0],0,0,0)) {
	  printk("[openair][IOCTL] phy_init_lte_eNB error\n");
	  break;
	}
	else
	  printk("[openair][IOCTL] phy_init_lte_eNB successful\n");

	PHY_vars_eNB_g[0]->Mod_id = 0;

	// allocate DLSCH structures
	PHY_vars_eNB_g[0]->dlsch_eNB_SI  = new_eNB_dlsch(1,1,0);
	if (!PHY_vars_eNB_g[0]->dlsch_eNB_SI) {
	  msg("Can't get eNb dlsch SI structures\n");
	  break;
	}
	else {
	  msg("dlsch_eNB_SI => %p\n",PHY_vars_eNB_g[0]->dlsch_eNB_SI);
	  PHY_vars_eNB_g[0]->dlsch_eNB_SI->rnti  = SI_RNTI;
	}
	PHY_vars_eNB_g[0]->dlsch_eNB_ra  = new_eNB_dlsch(1,1,0);
	if (!PHY_vars_eNB_g[0]->dlsch_eNB_ra) {
	  msg("Can't get eNb dlsch RA structures\n");
	  break;
	}
	else {
	  msg("dlsch_eNB_ra => %p\n",PHY_vars_eNB_g[0]->dlsch_eNB_ra);
	  PHY_vars_eNB_g[0]->dlsch_eNB_ra->rnti  = 0;//RA_RNTI;
	}

	for (i=0; i<NUMBER_OF_UE_MAX;i++){ 
	  for (j=0;j<2;j++) {
	    PHY_vars_eNB_g[0]->dlsch_eNB[i][j] = new_eNB_dlsch(1,8,0);
	    if (!PHY_vars_eNB_g[0]->dlsch_eNB[i][j]) {
	      msg("Can't get eNb dlsch structures\n");
	      break;
	    }
	    else {
	      msg("dlsch_eNB[%d][%d] => %p\n",i,j,PHY_vars_eNB_g[0]->dlsch_eNB[i][j]);
	      PHY_vars_eNB_g[0]->dlsch_eNB[i][j]->rnti=0;
	    }
	  }
	  // this will be overwritten with the real transmission mode by the RRC once the UE is connected
	  PHY_vars_eNB_g[0]->transmission_mode[i] = openair_daq_vars.dlsch_transmission_mode;

	}

	for (i=0; i<NUMBER_OF_UE_MAX+1;i++){ //+1 because 0 is reserved for RA
	  PHY_vars_eNB_g[0]->ulsch_eNB[i] = new_eNB_ulsch(3,0);
	  if (!PHY_vars_eNB_g[0]->ulsch_eNB[i]) {
	    msg("Can't get eNb ulsch structures\n");
	    break;
	  }
	  else {
	    msg("ulsch_eNB[%d] => %p\n",i,PHY_vars_eNB_g[0]->ulsch_eNB[i]);
	  }
	}

	//init_transport_channels(openair_daq_vars.dlsch_transmission_mode);

	openair_daq_vars.node_configured = 5;
	msg("[openair][IOCTL] phy_init_lte_eNB done: %d\n",openair_daq_vars.node_configured);
    

	for (aa=0;aa<NB_ANTENNAS_TX; aa++)
	  bzero((void*) TX_DMA_BUFFER[0][aa],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(mod_sym_t));
	udelay(10000);
		
	// Initialize MAC layer

#ifdef OPENAIR2
	//NODE_ID[0] = ((*((unsigned int *)arg_ptr))>>7)&0xFF;
	NB_eNB_INST=1;
	NB_UE_INST=0;
	openair_daq_vars.mac_registered = 
	  l2_init(&PHY_vars_eNB_g[0]->lte_frame_parms);
	if (openair_daq_vars.mac_registered != 1) {
	  printk("[openair][IOCTL] Error in configuring MAC\n");
	  break;
	}
	else 
	  printk("[openair][IOCTL] MAC Configuration successful\n");
	
	//mac_xface->mrbch_phy_sync_failure(0,0);

#endif

	/*
	// configure SRS parameters statically
	for (ue=0;ue<NUMBER_OF_UE_MAX;ue++) {
	  PHY_vars_eNB_g[0]->eNB_UE_stats[ue].SRS_parameters.Csrs = 2;
	  PHY_vars_eNB_g[0]->eNB_UE_stats[ue].SRS_parameters.Bsrs = 0;
	  PHY_vars_eNB_g[0]->eNB_UE_stats[ue].SRS_parameters.kTC = 0;
	  PHY_vars_eNB_g[0]->eNB_UE_stats[ue].SRS_parameters.n_RRC = 0;
	  if (ue>=3) {
	    msg("This SRS config will only work for 3 users! \n");
	    break;
	  }
	  PHY_vars_eNB_g[0]->eNB_UE_stats[ue].SRS_parameters.Ssrs = ue+1;
	}
	*/
      } // eNB Configuration check

      for (i=0;i<NUMBER_OF_UE_MAX;i++) {
	clean_eNb_dlsch(PHY_vars_eNB_g[0]->dlsch_eNB[i][0],0);
	clean_eNb_dlsch(PHY_vars_eNB_g[0]->dlsch_eNB[i][1],0);
	clean_eNb_ulsch(PHY_vars_eNB_g[0]->ulsch_eNB[i],0);
	memset(&(PHY_vars_eNB_g[0]->eNB_UE_stats[i]),0,sizeof(LTE_eNB_UE_stats));
      }
      clean_eNb_dlsch(PHY_vars_eNB_g[0]->dlsch_eNB_SI,0);
      clean_eNb_dlsch(PHY_vars_eNB_g[0]->dlsch_eNB_ra,0);


      openair_daq_vars.is_eNB = 1;
      //mac_xface->is_primary_cluster_head = 1;
      //mac_xface->is_secondary_cluster_head = 0;
      //mac_xface->cluster_head_index = 0;

      openair_daq_vars.node_id = PRIMARY_CH;
      //openair_daq_vars.dual_tx = 1;

      /*      
#ifdef OPENAIR_LTE
      openair_daq_vars.freq = ((*((unsigned int *)arg_ptr))>>1)&7;
      printk("[openair][IOCTL] Configuring for frequency %d\n",openair_daq_vars.freq);
#else
      openair_daq_vars.freq = ((int)(PHY_config->PHY_framing.fc_khz - 1902600)/5000)&3;
      printk("[openair][IOCTL] Configuring for frequency %d kHz (%d)\n",(unsigned int)PHY_config->PHY_framing.fc_khz,openair_daq_vars.freq);
#endif
      */
      
      openair_daq_vars.freq_info = 1 + (openair_daq_vars.freq<<1) + (openair_daq_vars.freq<<4);
      openair_daq_vars.tx_rx_switch_point = TX_RX_SWITCH_SYMBOL;
      
      for (i=0;i<number_of_cards;i++) 
	ret = setup_regs(i,frame_parms);

      PHY_vars_eNB_g[0]->rx_total_gain_eNB_dB = 138;
      for (i=0;i<number_of_cards;i++)
	openair_set_rx_gain_cal_openair(i,PHY_vars_eNB_g[0]->rx_total_gain_eNB_dB);

      if (ret == 0) {
#ifdef OPENAIR_LTE
	openair_daq_vars.mode = openair_SYNCHED;
	for (ue=0;ue<NUMBER_OF_UE_MAX;ue++)
	  PHY_vars_eNB_g[0]->eNB_UE_stats[ue].mode = PRACH;// NOT_SYNCHED
#else
	openair_daq_vars.mode = openair_SYNCHED_TO_MRSCH;
#endif
	openair_daq_vars.node_running = 1;
	openair_daq_vars.sync_state = 0;
	printk("[openair][IOCTL] Process initialization return code %d\n",ret);

	// Take out later!!!!!!
	for (i=0;i<number_of_cards;i++)
	  openair_dma(i,FROM_GRLIB_IRQ_FROM_PCI_IS_ACQ_START_RT_ACQUISITION);
      }

    }
    else {
      printk("[openair][IOCTL] Radio (%d) or Mac (%d) not configured\n",openair_daq_vars.node_configured,openair_daq_vars.mac_registered);
    }


#endif // RTAI_ENABLED
    break;
#endif //OPENAIR1

    /*
    //----------------------
  case openair_START_1ARY_CLUSTERHEAD_COGNITIVE:
    //----------------------

#ifdef RTAI_ENABLED
    printk("[openair][IOCTL]     openair_1ARY_START_CLUSTERHEAD_COGNITIVE\n");
    printk("[openair][IOCTL]     Freq corr = %d, Freq0 = %d, Freq1 = %d, NODE_ID = %d\n",*((unsigned int *)arg_ptr)&1,
	   (*((unsigned int *)arg_ptr)>>1)&7,
	   (*((unsigned int *)arg_ptr)>>4)&7,
	   (*((unsigned int *)arg_ptr)>>7)&0xFF);


    if ( (openair_daq_vars.node_configured == 1) && 
	 (openair_daq_vars.node_running == 0) && 
	 (openair_daq_vars.mac_registered == 1)) {



      mac_xface->is_cluster_head = 1;
      mac_xface->is_primary_cluster_head = 1;
      mac_xface->is_secondary_cluster_head = 0;
      mac_xface->cluster_head_index = 0;
      NODE_ID[0] = ((*((unsigned int *)arg_ptr))>>7)&0xFF;

      mac_xface->slots_per_frame = SLOTS_PER_FRAME;

      // Initialize MAC layer

      printk("[OPENAIR][IOCTL] MAC Init, is_cluster_head = %d (%p).slots_per_frame = %d (mac_xface %p)\n",mac_xface->is_cluster_head,&mac_xface->is_cluster_head,mac_xface->slots_per_frame,mac_xface);
      mac_xface->macphy_init();

      openair_daq_vars.tx_rx_switch_point = TX_RX_SWITCH_SYMBOL;
      openair_daq_vars.node_id = PRIMARY_CH;
      openair_daq_vars.freq = 0; //this is an initial value for the sensing
      openair_daq_vars.freq_info = 1 + (openair_daq_vars.freq<<1) + (openair_daq_vars.freq<<4);

    }
    else {
      printk("[openair][START_CLUSTERHEAD] Radio (%d) or Mac (%d) not configured\n",openair_daq_vars.node_configured,openair_daq_vars.mac_registered);
    }
  


#endif // RTAI_ENABLED
    break;
    */

#ifdef OPENAIR1
    //----------------------
  case openair_START_NODE:
    //----------------------

#ifdef RTAI_ENABLED
    printk("[openair][IOCTL]     openair_START_NODE\n");
    printk("[openair][IOCTL]     Freq corr = %d, Freq0 = %d, Freq1 = %d, NODE_ID = %d\n",
	   *((unsigned int *)arg_ptr)&1,
	   (*((unsigned int *)arg_ptr)>>1)&7,
	   (*((unsigned int *)arg_ptr)>>4)&7,
	   (*((unsigned int *)arg_ptr)>>7)&0xFF);


    if ( (openair_daq_vars.node_configured > 0) && 
	 (openair_daq_vars.node_running == 0)) {

      if (openair_daq_vars.node_configured == 1) {

	// allocate memory for PHY
	PHY_vars_UE_g = (PHY_VARS_UE**) malloc16(sizeof(PHY_VARS_UE*));
	if (PHY_vars_UE_g == NULL) {
	  printk("[openair][IOCTL] Cannot allocate PHY_vars_UE\n");
	  break;
	}
	PHY_vars_UE_g[0] = (PHY_VARS_UE*) malloc16(sizeof(PHY_VARS_UE));
	if (PHY_vars_UE_g[0] == NULL) {
	  printk("[openair][IOCTL] Cannot allocate PHY_vars_UE\n");
	  break;
	}
	bzero(PHY_vars_UE_g[0],sizeof(PHY_VARS_UE));

	//copy frame parms
	memcpy((void*) &PHY_vars_UE_g[0]->lte_frame_parms, (void*) frame_parms, sizeof(LTE_DL_FRAME_PARMS));

	dump_frame_parms(&PHY_vars_UE_g[0]->lte_frame_parms);

	if (phy_init_lte_ue(PHY_vars_UE_g[0],
			    0)) {
	    msg("[openair][IOCTL] phy_init_lte_ue error\n");
	    break;
	}
	else
	  msg("[openair][IOCTL] phy_init_lte_ue successful\n");

	PHY_vars_UE_g[0]->Mod_id = 0;
	// this is only for visualization in the scope
	PHY_vars_UE_g[0]->lte_ue_common_vars.sync_corr = sync_corr_ue0;

  
	// allocate dlsch structures
	for (i=0; i<NUMBER_OF_eNB_MAX;i++){ 
	  for (j=0;j<2;j++) {
	    PHY_vars_UE_g[0]->dlsch_ue[i][j]  = new_ue_dlsch(1,8,0);
	    if (PHY_vars_UE_g[0]->dlsch_ue[i][j]) {
	      msg("[openair][IOCTL] UE dlsch structure eNb %d layer %d created\n",i,j);
	    }
	    else {
	      msg("[openair][IOCTL] Can't get ue dlsch structures\n");
	      break;
	    }
	  }
	  PHY_vars_UE_g[0]->ulsch_ue[i]  = new_ue_ulsch(3,0);
	  if (PHY_vars_UE_g[0]->ulsch_ue[i]) {
	    msg("[openair][IOCTL] ue ulsch structure %d created\n",i);
	  }
	  else {
	    msg("[openair][IOCTL] Can't get ue ulsch structures\n");
	    break;
	  }
	  
	  PHY_vars_UE_g[0]->dlsch_ue_SI[i]  = new_ue_dlsch(1,1,0);
	  if (PHY_vars_UE_g[0]->dlsch_ue_SI[i]) {
	    msg("[openair][IOCTL] ue dlsch (SI) structure %d created\n",i);
	  }
	  else {
	    msg("[openair][IOCTL] Can't get ue dlsch (SI) structures\n");
	    break;
	  }

	  PHY_vars_UE_g[0]->dlsch_ue_ra[i]  = new_ue_dlsch(1,1,0);
	  if (PHY_vars_UE_g[0]->dlsch_ue_SI[i]) {
	    msg("[openair][IOCTL] ue dlsch (RA) structure %d created\n",i);
	  }
	  else {
	    msg("[openair][IOCTL] Can't get ue dlsch (RA) structures\n");
	    break;
	  }

	  // this will be overwritten with the real transmission mode by the RRC once the UE is connected
	  PHY_vars_UE_g[0]->transmission_mode[i] = openair_daq_vars.dlsch_transmission_mode;

	}

	openair_daq_vars.node_configured = 3;
	msg("[openair][IOCTL] phy_init_lte_ue done: %d\n",openair_daq_vars.node_configured);


#ifdef OPENAIR2	
	//NODE_ID[0] = ((*((unsigned int *)arg_ptr))>>7)&0xFF;
	NB_eNB_INST=0;
	NB_UE_INST=1;
	openair_daq_vars.mac_registered =
	  l2_init(&PHY_vars_UE_g[0]->lte_frame_parms); 
	if (openair_daq_vars.mac_registered != 1) {
	  printk("[openair][IOCTL] Error in configuring MAC\n");
	  break;
	}
	else 
	  printk("[openair][IOCTL] MAC Configuration successful\n");

	Mac_rlc_xface->Is_cluster_head[0] = 0;
#endif

	/*
	// configure SRS parameters (this will only work for one UE)
	PHY_vars_UE_g[0]->SRS_parameters.Csrs = 2;
	PHY_vars_UE_g[0]->SRS_parameters.Bsrs = 0;
	PHY_vars_UE_g[0]->SRS_parameters.kTC = 0;
	PHY_vars_UE_g[0]->SRS_parameters.n_RRC = 0;
	PHY_vars_UE_g[0]->SRS_parameters.Ssrs = 1;
	*/
      }  

	for (i=0;i<NUMBER_OF_eNB_MAX;i++) {
	  PHY_vars_UE_g[0]->lte_ue_pbch_vars[i]->pdu_errors_conseq=0;
	  PHY_vars_UE_g[0]->lte_ue_pbch_vars[i]->pdu_errors=0;
	  
	  PHY_vars_UE_g[0]->lte_ue_pdcch_vars[i]->dci_errors = 0;
	  PHY_vars_UE_g[0]->lte_ue_pdcch_vars[i]->dci_missed = 0;
	  PHY_vars_UE_g[0]->lte_ue_pdcch_vars[i]->dci_false  = 0;    
	  PHY_vars_UE_g[0]->lte_ue_pdcch_vars[i]->dci_received = 0;    

	  node_id = ((*((unsigned int *)arg_ptr))>>7)&0xFF;
	  PHY_vars_UE_g[0]->lte_ue_pdcch_vars[i]->crnti = (node_id>0 ? 0x1236 : 0x1235);
	  msg("[openair][IOCTL] Setting crnti for eNB %d to %x\n",i,PHY_vars_UE_g[0]->lte_ue_pdcch_vars[i]->crnti);
	  PHY_vars_UE_g[0]->UE_mode[i] = NOT_SYNCHED;

	  /*
	  clean_ue_dlsch(PHY_vars_UE_g[0]->lte_ue_dlsch[i][0],0);
	  clean_ue_dlsch(PHY_vars_UE_g[0]->lte_ue_dlsch[i][1],0);
	  clean_ue_dlsch(PHY_vars_UE_g[0]->lte_ue_dlsch_SI[i],0);
	  clean_ue_dlsch(PHY_vars_UE_g[0]->lte_ue_dlsch_ra[i],0);
	  clean_ue_ulsch(PHY_vars_UE_g[0]->lte_ue_ulsch[i],0);
	  */
	} 
	
      openair_daq_vars.is_eNB = 0;
      //mac_xface->is_primary_cluster_head = 0;
      //mac_xface->is_secondary_cluster_head = 0;
      //mac_xface->cluster_head_index = 0;

      openair_daq_vars.node_id = NODE;

#ifdef OPENAIR2
      RRC_CONNECTION_FLAG = 0;
#endif
      
      /*
#ifdef OPENAIR_LTE
      openair_daq_vars.freq = ((*((unsigned int *)arg_ptr))>>1)&7;
      printk("[openair][IOCTL] Configuring for frequency %d\n",openair_daq_vars.freq);
#else
      openair_daq_vars.freq = ((int)(PHY_config->PHY_framing.fc_khz - 1902600)/5000)&3;
      printk("[openair][IOCTL] Configuring for frequency %d kHz (%d)\n",(unsigned int)PHY_config->PHY_framing.fc_khz,openair_daq_vars.freq);
#endif
      */
      
      openair_daq_vars.tx_rx_switch_point = TX_RX_SWITCH_SYMBOL;
      openair_daq_vars.freq_info = 1 + (openair_daq_vars.freq<<1) + (openair_daq_vars.freq<<4);
      
      PHY_vars_UE_g[0]->rx_total_gain_dB = MIN_RF_GAIN;
      openair_daq_vars.rx_total_gain_dB = MIN_RF_GAIN;
      openair_daq_vars.rx_gain_mode = DAQ_AGC_ON;
      openair_set_rx_gain_cal_openair(0,PHY_vars_UE_g[0]->rx_total_gain_dB);
      /*
      msg("[openair][IOCTL] RX_DMA_BUFFER[0] = %p = %p RX_DMA_BUFFER[1] = %p = %p\n",
	  RX_DMA_BUFFER[0],
	  PHY_vars_UE_g[0]->lte_ue_common_vars.rxdata[0],
	  RX_DMA_BUFFER[1],
	  PHY_vars_UE_g[0]->lte_ue_common_vars.rxdata[1]);
      */
#ifdef DLSCH_THREAD
      ret = init_dlsch_threads();
      if (ret != 0)
	printk("[openair][IOCTL] Error in starting DLSCH thread\n");
      else
	printk("[openair][IOCTL] DLSCH thread started\n");
#endif
      
      udelay(10000);
      
      ret = setup_regs(0,frame_parms);
      if (ret == 0) {
	openair_daq_vars.node_running = 1;
	printk("[openair][IOCTL] Process initialization return code %d\n",ret);
      }

      /*
	    openair_daq_vars.mode = openair_SYNCHED;
#ifdef OPENAIR2
	    msg("[openair][SCHED][SYNCH] Calling chbch_phy_sync_success\n");
	    //mac_resynch();
	    mac_xface->chbch_phy_sync_success(0,0);
#endif //OPENAIR2
	    PHY_vars_UE_g[0]->UE_mode[0] = PRACH;

	    msg("[openair][SCHED][SYNCH] Starting RT aquisition\n");
	    openair_dma(0,FROM_GRLIB_IRQ_FROM_PCI_IS_ACQ_START_RT_ACQUISITION);
      */

    }
    else {
      printk("[openair][IOCTL] Radio not configured\n");
    }
#endif // RTAI_ENABLED
    break;
#endif

    /*
    //----------------------
  case openair_START_2ARY_CLUSTERHEAD:

#ifdef RTAI_ENABLED
    //----------------------
    printk("[openair][IOCTL]     openair_START_2ARY_CLUSTERHEAD\n");
    printk("[openair][IOCTL]     Freq corr = %d, Freq0 = %d, Freq1 = %d, NODE_ID = %d\n",*((unsigned int *)arg_ptr)&1,
	   (*((unsigned int *)arg_ptr)>>1)&7,
	   (*((unsigned int *)arg_ptr)>>4)&7,
	   (*((unsigned int *)arg_ptr)>>7)&0xFF);

    if ( (openair_daq_vars.node_configured == 1) && (openair_daq_vars.node_running == 0)) {
      mac_xface->is_cluster_head = 1;
      mac_xface->is_primary_cluster_head = 0;
      mac_xface->is_secondary_cluster_head = 1;
      mac_xface->cluster_head_index = 0;
   
      NODE_ID[0] = ((*((unsigned int *)arg_ptr))>>7)&0xFF;
      mac_xface->macphy_init(); ///////H.A

      openair_daq_vars.node_id = SECONDARY_CH;
#ifdef OPENAIR_LTE
      openair_daq_vars.freq = ((*((unsigned int *)arg_ptr))>>1)&7;
      printk("[openair][IOCTL] Configuring for frequency %d\n",openair_daq_vars.freq);
#else
      openair_daq_vars.freq = ((int)(PHY_config->PHY_framing.fc_khz - 1902600)/5000)&3;
      printk("[openair][IOCTL] Configuring for frequency %d kHz (%d)\n",(unsigned int)PHY_config->PHY_framing.fc_khz,openair_daq_vars.freq);
#endif

      openair_daq_vars.tx_rx_switch_point = TX_RX_SWITCH_SYMBOL;
      openair_daq_vars.freq_info = 1 + (openair_daq_vars.freq<<1) + (openair_daq_vars.freq<<4);

      ret = setup_regs(0);
      if (ret == 0) {
	openair_daq_vars.node_running = 1;
	printk("[openair][START_2ARYCLUSTERHEAD] Process initialization return code %d\n",ret);
      }
    }

    else {
      printk("[openair][START_2ARY_CLUSTERHEAD] Radio not configured\n");
    }

#endif // RTAI_ENABLED
    break;
    //----------------------
    */


    //----------------------
  case openair_STOP:
    //----------------------
    printk("[openair][IOCTL]     openair_STOP, NODE_CONFIGURED %d\n",openair_daq_vars.node_configured);

    
#ifdef RTAI_ENABLED
    if (openair_daq_vars.node_configured > 0) {
      openair_daq_vars.node_running = 0;

      for (aa=0;aa<NB_ANTENNAS_TX; aa++)
	bzero((void*) TX_DMA_BUFFER[0][aa],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(mod_sym_t));
      udelay(1000);

      openair_daq_vars.node_id = NODE;

      /*
#ifdef OPENAIR_LTE
      openair_daq_vars.freq = ((*((unsigned int *)arg_ptr))>>1)&7;
      printk("[openair][IOCTL] Configuring for frequency %d\n",openair_daq_vars.freq);
#else
      openair_daq_vars.freq = ((int)(PHY_config->PHY_framing.fc_khz - 1902600)/5000)&3;
      printk("[openair][IOCTL] Configuring for frequency %d kHz (%d)\n",(unsigned int)PHY_config->PHY_framing.fc_khz,openair_daq_vars.freq);
#endif
      */

      openair_daq_vars.tx_rx_switch_point = TX_RX_SWITCH_SYMBOL; 
      openair_daq_vars.freq_info = 1 + (openair_daq_vars.freq<<1) + (openair_daq_vars.freq<<4);

      for (i=0;i<number_of_cards;i++) {
	setup_regs(i,frame_parms);
	if (vid != XILINX_VENDOR) {
	  openair_dma(i,FROM_GRLIB_IRQ_FROM_PCI_IS_ACQ_DMA_STOP);
	}
	else {
	  openair_dma(i,EXMIMO_STOP);
	}
      }

      openair_daq_vars.tx_test=0;
      openair_daq_vars.mode = openair_NOT_SYNCHED;
      openair_daq_vars.sync_state = 0;
      //mac_xface->frame = 0;
      openair_daq_vars.is_eNB = 0;

      /*
      for (j=0;j<NB_ANTENNAS;j++) 
	for (i=0;i<FRAME_LENGTH_BYTES;i+=OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES*4)
	  copy_to_user(&((unsigned char *)arg)[i+(j*FRAME_LENGTH_BYTES)],&((unsigned char *)RX_DMA_BUFFER[j])[i],OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES*4);
      */

      udelay(1000);

      /*
      if (vid == XILINX_VENDOR) {
	printk("ADC0 (%p) :",(unsigned int *)RX_DMA_BUFFER[0][0]);
	for (i=0;i<128;i++) {
	  printk("%x.",((unsigned int *)RX_DMA_BUFFER[0][0])[i]);
	}
      }
      printk("\n");*/
    }
    else {
      printk("[openair][STOP][ERROR] Cannot stop, radio is not configured ...\n");
      return -1;
    }
#endif // RTAI_ENABLED

#ifndef OPENAIR_LTE
    for (i=0;i<4;i++) {
      PHY_vars->PHY_measurements.chbch_detection_count[i]= 0;
    }
    PHY_vars->PHY_measurements.mrbch_detection_count= 0;
    PHY_vars->PHY_measurements.chbch_search_count= 0;
    PHY_vars->PHY_measurements.mrbch_search_count= 0;
#endif //OPENAIR_LTE    
    break;
  
  case openair_GET_BUFFER:

    printk("[openair][IOCTL]     openair_GET_BUFFER (%p)\n",(void *)RX_DMA_BUFFER[0]);

    /*
#ifndef OPENAIR_LTE
    openair_daq_vars.freq = ((int)(PHY_config->PHY_framing.fc_khz - 1902600)/5000)&3;
    printk("[openair][IOCTL] Configuring for frequency %d kHz (%d)\n",(unsigned int)PHY_config->PHY_framing.fc_khz,openair_daq_vars.freq);
#else
    openair_daq_vars.freq = ((*((unsigned int *)arg_ptr))>>1)&7;
    //    printk("[openair][IOCTL] Configuring for frequency %d\n",openair_daq_vars.freq);
#endif
    */

    //openair_daq_vars.tx_rx_switch_point = NUMBER_OF_SYMBOLS_PER_FRAME; //this puts the node into RX mode only for TDD, its ignored in FDD mode
    openair_daq_vars.freq_info = 1 + (openair_daq_vars.freq<<1) + (openair_daq_vars.freq<<4);


    
    if (vid != XILINX_VENDOR) {

#ifdef RTAI_ENABLED
      if (openair_daq_vars.node_configured > 0) {
	
	openair_daq_vars.node_id = NODE;      
	
	for (i=0;i<number_of_cards;i++) {
	  ret = setup_regs(i,frame_parms);
	  openair_get_frame(i);
	}

	//openair_daq_vars.one_shot_get_frame=1;
	
      }
      else {
	printk("[openair][GET_BUFFER][ERROR]  Radio not configured\n");
	return -1;
      }
      
#else
      
      
#endif // RTAI_ENABLED
    }
    else {

      rt_enable_irq(pdev[0]->irq);
      openair_daq_vars.get_frame_done = 0;
      setup_regs(0,frame_parms);
      get_frame_cnt=0;
      rt_enable_irq(pdev[0]->irq);
      printk("calling openair_dma(0,EXMIMO_GET_FRAME);\n");
      openair_dma(0,EXMIMO_GET_FRAME);
      
      while ((get_frame_cnt<100) &&
             (openair_daq_vars.get_frame_done == 0)) {
        udelay(1000);
        get_frame_cnt++;
      }
      if (get_frame_cnt==100)
        printk("TIMEOUT: GET_FRAME_DONE didn't arrive within 100ms.\n");
      rt_disable_irq(pdev[0]->irq);

      rt_disable_irq(pdev[0]->irq);
      pci_dma_sync_single_for_cpu(pdev[0], 
				  exmimo_pci_interface->rf.adc_head[0],
				  76800*4, 
				  PCI_DMA_FROMDEVICE);
      /*
      printk("RX_DMA_BUFFER[0][0] 0x%x\n",RX_DMA_BUFFER[0][0]);
      for (i=0;i<76800;i+=1024)
	printk("rx_buffer %d => %x\n",i,((unsigned int*)RX_DMA_BUFFER[0][0])[i]);
      */
      
    }
    break;
    
    //----------------------

  case openair_GET_CONFIG:

#ifdef RTAI_ENABLED
    printk("[openair][IOCTL]     openair_GET_CONFIG ...(%p)\n",(void *)arg);
    copy_to_user((char *)arg,lte_frame_parms_g,sizeof(LTE_DL_FRAME_PARMS));
#endif // RTAI_ENABLED

    break;

  case openair_GET_BIGPHYSTOP:

#ifdef RTAI_ENABLED
    printk("[openair][IOCTL]     openair_GET_BIGPHYSTOP ...(%p)\n",(void *)arg);
    printk("[openair][IOCTL]     bigphys_ptr = %x\n",bigphys_ptr);
    copy_to_user((char *)arg,&bigphys_ptr,sizeof(char *));
#endif // RTAI_ENABLED
    break;

  case openair_GET_VARS:

#ifdef PC_TARGET
#ifdef RTAI_ENABLED
    printk("[openair][IOCTL]     openair_GET_VARS ...(%p)\n",(void *)arg);
    if (openair_daq_vars.node_configured == 3){    
      printk("[openair][IOCTL]  ... for UE (%d bytes) \n",sizeof(PHY_VARS_UE));
      copy_to_user((char *)arg,PHY_vars_UE_g[0],sizeof(PHY_VARS_UE));
    }
    else if (openair_daq_vars.node_configured == 5) {
      printk("[openair][IOCTL]  ... for eNB (%d bytes)\n",sizeof(PHY_VARS_eNB));
      copy_to_user((char *)arg,PHY_vars_eNB_g[0],sizeof(PHY_VARS_eNB));
    }
    else {
      printk("[openair][IOCTL] neither UE or eNb configured, sending TX_RX_VARS\n");
      dummy_tx_rx_vars.TX_DMA_BUFFER[0] = (char*) TX_DMA_BUFFER[0][0];
      dummy_tx_rx_vars.TX_DMA_BUFFER[1] = (char*) TX_DMA_BUFFER[0][1];
      dummy_tx_rx_vars.RX_DMA_BUFFER[0] = (int*) RX_DMA_BUFFER[0][0];
      dummy_tx_rx_vars.RX_DMA_BUFFER[1] = (int*) RX_DMA_BUFFER[0][1];
      copy_to_user((char *)arg,&dummy_tx_rx_vars,sizeof(TX_RX_VARS));
    }
#endif // RTAI_ENABLED
#endif // PC_TARGET
    break;

  case openair_START_LXRT:

    // get condition and semaphore variables by name

    //rt_sem_init(&oai_semaphore, 1);
    //rt_register(nam2num("MUTEX"),&oai_semaphore,IS_SEM, 0);
    oai_semaphore = rt_get_adr(nam2num("MUTEX"));
    if (oai_semaphore==0)
      printk("Error init mutex\n");

    lxrt_task = rt_get_adr(nam2num("TASK0"));
    if (lxrt_task==0)
      printk("Error init lxrt_task\n");

    inst_cnt_ptr = malloc16(sizeof(s32));
    *inst_cnt_ptr = -1;

    intr_cnt2=0;

    printk("[openair][IOCTL] openair_START_LXRT, oai_semaphore=%p, lxrt_task=%p, inst_cnt_ptr = %p\n",oai_semaphore,lxrt_task,inst_cnt_ptr);

    // init instance count and copy its pointer to userspace
    copy_to_user((char *)arg,&inst_cnt_ptr,sizeof(s32*));

    /*
    // enable the DMA transfers
    for (i=0;i<number_of_cards;i++)
      openair_dma(i,FROM_GRLIB_IRQ_FROM_PCI_IS_ACQ_START_RT_ACQUISITION);
    */

    break;

  case openair_SET_TX_GAIN:

    printk("[openair][IOCTL]     openair_SET_TX_GAIN ...(%p)\n",(void *)arg);
    for (i=0;i<number_of_cards;i++)
      openair_set_tx_gain_openair(i,((unsigned char *)arg)[0],((unsigned char *)arg)[1],((unsigned char *)arg)[2],((unsigned char *)arg)[3]
);

    break;

  case openair_SET_RX_GAIN:

    printk("[openair][IOCTL]     openair_SET_RX_GAIN ...(%p)\n",(void *)arg);

    for (i=0;i<number_of_cards;i++)
      openair_set_rx_gain_openair(i,((unsigned char *)arg)[0],((unsigned char *)arg)[1],((unsigned char *)arg)[2],((unsigned char *)arg)[3]);
    openair_daq_vars.rx_gain_mode = DAQ_AGC_OFF; // ((unsigned int *)arg)[0] & 0x1; 
    break;

  case openair_SET_CALIBRATED_RX_GAIN:

    printk("[openair][IOCTL]     openair_SET_CALIBRATED_RX_GAIN ...(%p)\n",(void *)arg);

    for (i=0;i<number_of_cards;i++)
      openair_set_rx_gain_cal_openair(i,((unsigned int *)arg)[0]);

    //PHY_vars->rx_total_gain_dB = ((unsigned int *)arg)[0];
    //PHY_vars->rx_total_gain_eNB_dB = ((unsigned int *)arg)[0];
    openair_daq_vars.rx_gain_mode = DAQ_AGC_OFF; // ((unsigned int *)arg)[0] & 0x1; 
    break;

  case openair_START_FS4_TEST:
    
    printk("[openair][IOCTL]     openair_START_FS4_TEST ...(%p)\n",(void *)arg);
    openair_daq_vars.node_id = PRIMARY_CH;

    /*
#ifndef OPENAIR_LTE
    openair_daq_vars.freq = ((int)(PHY_config->PHY_framing.fc_khz - 1902600)/5000)&3;
    printk("[openair][IOCTL] Configuring for frequency %d kHz (%d)\n",(unsigned int)PHY_config->PHY_framing.fc_khz,openair_daq_vars.freq);
#else
    openair_daq_vars.freq = ((*((unsigned int *)arg_ptr))>>1)&7;
    printk("[openair][IOCTL] Configuring for frequency %d\n",openair_daq_vars.freq);
#endif
    */

    openair_daq_vars.freq_info = 1 + (openair_daq_vars.freq<<1) + (openair_daq_vars.freq<<4);

    openair_daq_vars.tx_rx_switch_point = NUMBER_OF_SYMBOLS_PER_FRAME-2;
    
    openair_daq_vars.tx_test=1;

    /*
#ifdef BIT8_TX
    for (i=0;i<FRAME_LENGTH_COMPLEX_SAMPLES<<1;i+=8) {
      ((char*) (TX_DMA_BUFFER[0][0]))[i] = 127;
      ((char*) (TX_DMA_BUFFER[0][0]))[i+1] = 0;
      ((char*) (TX_DMA_BUFFER[0][0]))[i+2] = 0;
      ((char*) (TX_DMA_BUFFER[0][0]))[i+3] = 127;
      ((char*) (TX_DMA_BUFFER[0][0]))[i+4] = -127;
      ((char*) (TX_DMA_BUFFER[0][0]))[i+5] = 0;
      ((char*) (TX_DMA_BUFFER[0][0]))[i+6] = 0;
      ((char*) (TX_DMA_BUFFER[0][0]))[i+7] = -127;
    }
#endif
    */

    for (i=0;i<number_of_cards;i++) {
      ret = setup_regs(i,frame_parms);
      openair_dma(i,FROM_GRLIB_IRQ_FROM_PCI_IS_ACQ_DMA_STOP);
      udelay(1000);
      /*
#ifdef BIT8_TX
      openair_dma(i,FROM_GRLIB_IRQ_FROM_PCI_IS_ACQ_START_RT_ACQUISITION);
#else
      */
      openair_dma(i,FROM_GRLIB_IRQ_FROM_PCI_IS_ACQ_GEN_FS4);
      /* #endif */
    }

    break;

  case openair_START_REAL_FS4_WITH_DC_TEST:

    printk("[openair][IOCTL]     openair_START_REAL_FS4_WITH_DC_TEST ...(%p)\n",(void *)arg);


    break;

  case openair_START_OFDM_TEST:
    printk("[openair][IOCTL]     openair_START_OFDM_TEST ...(%p)\n",(void *)arg);

    openair_daq_vars.node_id = NODE;

    /*
#ifndef OPENAIR_LTE
    openair_daq_vars.freq = ((int)(PHY_config->PHY_framing.fc_khz - 1902600)/5000)&3;
    printk("[openair][IOCTL] Configuring for frequency %d kHz (%d)\n",(unsigned int)PHY_config->PHY_framing.fc_khz,openair_daq_vars.freq);
#else
    openair_daq_vars.freq = ((*((unsigned int *)arg_ptr))>>1)&7;
    printk("[openair][IOCTL] Configuring for frequency %d\n",openair_daq_vars.freq);
#endif
    */

    openair_daq_vars.freq_info = 1 + (openair_daq_vars.freq<<1) + (openair_daq_vars.freq<<4);

    openair_daq_vars.tx_rx_switch_point = NUMBER_OF_SYMBOLS_PER_FRAME-2;
    
    openair_daq_vars.tx_test=1;
    printk("[openair][IOCTL] OFDM: first rb %d, nb_rb %d\n",
	   ((*((unsigned *)arg_ptr))>>7)&0x1f,
	   ((*((unsigned *)arg_ptr))>>12)&0x1f);

    for (i=0;i<number_of_cards;i++) {
      ret = setup_regs(i,frame_parms);
      pci_interface[i]->first_rb = ((*((unsigned *)arg_ptr))>>7)&0x1f;
      pci_interface[i]->nb_rb = ((*((unsigned *)arg_ptr))>>12)&0x1f;
    //    start_rt_timer(0);  //in oneshot mode the argument (period) is ignored
      openair_dma(i,FROM_GRLIB_IRQ_FROM_PCI_IS_ACQ_DMA_STOP);
      udelay(1000);
    //    openair_dma(0,FROM_GRLIB_IRQ_FROM_PCI_IS_ACQ_START_RT_ACQUISITION);
      openair_dma(i,FROM_GRLIB_IRQ_FROM_PCI_IS_ACQ_GEN_OFDM);
      udelay(1000);
    }
    break;

  case openair_START_QAM16_TEST:

    printk("[openair][IOCTL]     openair_START_QAM16_TEST ...(%p)\n",(void *)arg);

    break;

  case openair_START_QPSK_TEST:

    printk("[openair][IOCTL]     openair_START_QPSK_TEST ...(%p)\n",(void *)arg);

    break;

  case openair_START_IQ_IMPULSES_TEST:

    printk("[openair][IOCTL]     openair_START_IQ_IMPULSES_TEST ...(%p)\n",(void *)arg);

    break;

  case openair_RX_RF_MODE:
    printk("[openair][IOCTL]     openair_RX_RF_MODE ...(%p), setting to %d\n",(void *)arg,((unsigned int *)arg)[0]);

    for (i=0;i<number_of_cards;i++)
      openair_set_rx_rf_mode(i,((unsigned int *)arg)[0]);
    break;

  case openair_SET_TCXO_DAC:
    printk("[openair][IOCTL]     openair_set_tcxo_dac ...(%p)\n",(void *)arg);

    for (i=0;i<number_of_cards;i++)
      openair_set_tcxo_dac(i,((unsigned int *)arg)[0]);
    break;


  case openair_START_TX_SIG:


    openair_daq_vars.node_id = PRIMARY_CH;

    /*
#ifndef OPENAIR_LTE
    openair_daq_vars.freq = ((int)(PHY_config->PHY_framing.fc_khz - 1902600)/5000)&3;
    printk("[openair][IOCTL] Configuring for frequency %d kHz (%d)\n",(unsigned int)PHY_config->PHY_framing.fc_khz,openair_daq_vars.freq);
#else
    printk("[openair][IOCTL] Configuring for frequency %d\n",openair_daq_vars.freq);
#endif
    */
    
    openair_daq_vars.freq_info = 1 + (openair_daq_vars.freq<<1) + (openair_daq_vars.freq<<4);
    openair_daq_vars.tx_rx_switch_point = NUMBER_OF_SYMBOLS_PER_FRAME-2;

    openair_daq_vars.tx_test=1;
    ret = setup_regs(0,frame_parms);

    /*
      openair_dma(0,FROM_GRLIB_IRQ_FROM_PCI_IS_ACQ_DMA_STOP);
      
      bzero((void*)TX_DMA_BUFFER[0][0],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(mod_sym_t));
      bzero((void*)TX_DMA_BUFFER[0][1],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(mod_sym_t));
      copy_from_user((unsigned char*)&dummy_tx_vars,
		     (unsigned char*)arg,
		     sizeof(TX_VARS));
      
      copy_from_user((unsigned char*)TX_DMA_BUFFER[0][0],
		     (unsigned char*)dummy_tx_vars.TX_DMA_BUFFER[0],
		     FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(mod_sym_t));
      copy_from_user((unsigned char*)TX_DMA_BUFFER[0][1],
		     (unsigned char*)dummy_tx_vars.TX_DMA_BUFFER[1],
		     FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(mod_sym_t));
      
      printk("TX_DMA_BUFFER[0] = %p, arg = %p, FRAMELENGTH_BYTES = %x\n",(void *)TX_DMA_BUFFER[0][0],(void *)arg,FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(mod_sym_t));
      
      for (i=0;i<128;i++) {
	printk("TX_DMA_BUFFER[0][%d] = %x\n",i,((unsigned short *)TX_DMA_BUFFER[0][0])[i]);
	printk("TX_DMA_BUFFER[1][%d] = %x\n",i,((unsigned short *)TX_DMA_BUFFER[0][1])[i]);
      }
      
      

      openair_dma(0,FROM_GRLIB_IRQ_FROM_PCI_IS_ACQ_START_RT_ACQUISITION);
    }
    else {

    }
    */

    for (i=0;i<128;i++) {
      printk("TX_DMA_BUFFER[0][%d] = %x\n",i,((unsigned short *)TX_DMA_BUFFER[0][0])[i]);
      printk("TX_DMA_BUFFER[1][%d] = %x\n",i,((unsigned short *)TX_DMA_BUFFER[0][1])[i]);
    }

    if (vid != XILINX_VENDOR) {
      openair_dma(0,FROM_GRLIB_IRQ_FROM_PCI_IS_ACQ_START_RT_ACQUISITION);
    }
    else {
      //      openair_dma(0,EXMIMO_CONFIG);
      //      udelay(1000);
      openair_dma(0,EXMIMO_START_RT_ACQUISITION);
    }
    break;

  case openair_START_TX_SIG_NO_OFFSET:
    openair_dma(0,FROM_GRLIB_IRQ_FROM_PCI_IS_ACQ_DMA_STOP);
    openair_daq_vars.tx_test=1;    
    copy_from_user((unsigned char*)TX_DMA_BUFFER[0][0],
		   (unsigned char*)arg,
		   FRAME_LENGTH_BYTES);
    printk("TX_DMA_BUFFER[0] = %p, arg = %p, FRAMELENGTH_BYTES = %x\n",(void *)TX_DMA_BUFFER[0],(void *)arg,FRAME_LENGTH_BYTES);

    //    for (i=0;i<16;i++)
    //      printk("TX_DMA_BUFFER[0][%d] = %x\n",i,((unsigned int *)TX_DMA_BUFFER[0])[i]);

    openair_daq_vars.node_id = PRIMARY_CH;

    /*
#ifndef OPENAIR_LTE
    openair_daq_vars.freq = ((int)(PHY_config->PHY_framing.fc_khz - 1902600)/5000)&3;
    printk("[openair][IOCTL] Configuring for frequency %d kHz (%d)\n",(unsigned int)PHY_config->PHY_framing.fc_khz,openair_daq_vars.freq);
#else
    printk("[openair][IOCTL] Configuring for frequency %d\n",openair_daq_vars.freq);
#endif
    */
    
    openair_daq_vars.freq_info = 0 + (openair_daq_vars.freq<<1) + (openair_daq_vars.freq<<4);
    openair_daq_vars.tx_rx_switch_point = NUMBER_OF_SYMBOLS_PER_FRAME-2;
    ret = setup_regs(0,frame_parms);

    openair_dma(0,FROM_GRLIB_IRQ_FROM_PCI_IS_ACQ_START_RT_ACQUISITION);
		
    break;


  case openair_UPDATE_FIRMWARE:

    printk("[openair][IOCTL]     openair_UPDATE_FIRMWARE\n");
    /***************************************************
     *   Updating the firmware of Cardbus-MIMO-1 or ExpressMIMO SoC   *
     ***************************************************/
    /* 1st argument of this ioctl indicates the action to perform among these:
       - Transfer a block of data at a specified address (given as the 2nd argument)
       and for a specified length (given as the 3rd argument, in number of 32-bit words).
	 The USER-SPACE address where to find the block of data is given as the 4th
	 argument.
         - Ask the Leon processor to clear the .bss section. In this case, the base
	 address of section .bss is given as the 2nd argument, and its size is
	 given as the 3rd one.
         - Ask the Leon processor to jump at a specified address (given as the 2nd
	 argument, most oftenly expected to be the top address of Ins, Scratch Pad
	 Ram), after having set the stack pointer (given as the 3rd argument).
	 For the openair_UPDATE_FIRMWARE ioctl, we perform a partial infinite loop
	 while acknowledging the PCI irq from Leon software: the max number of loop
	 is yielded by preprocessor constant MAX_IOCTL_ACK_CNT. This avoids handing
	 the kernel with an infinite polling loop. An exception is the case of clearing
	 the bss: it takes time to Leon3 to perform this operation, so we poll te
	 acknowledge with no limit */

#define MAX_IOCTL_ACK_CNT    500
    update_firmware_command = *((unsigned int*)arg);
    
    
    switch (update_firmware_command) {
      
    case UPDATE_FIRMWARE_TRANSFER_BLOCK:
      update_firmware_address   = ((unsigned int*)arg)[1];
      update_firmware_length    = ((unsigned int*)arg)[2];
      
      if (vid != XILINX_VENDOR) {  // This is CBMIMO1     

	invert4(update_firmware_address); /* because Sparc is big endian */
	invert4(update_firmware_length); /* because Sparc is big endian */

	update_firmware_ubuffer   = (unsigned int*)((unsigned int*)arg)[3];
	/* Alloc some space from kernel to copy the user data block into */
	lendian_length = update_firmware_length;
	invert4(lendian_length); /* because Sparc is big endian */
	update_firmware_kbuffer = (unsigned int*)kmalloc(lendian_length * 4 /* 4 because kmalloc expects bytes */,
							 GFP_KERNEL);
	if (!update_firmware_kbuffer) {
	  printk("[openair][IOCTL]  Could NOT allocate %u bytes from kernel memory (kmalloc failed).\n", lendian_length * 4);
	  return -1; 
	  break;
	}
	/* Copy the data block from user space */
	tmp = copy_from_user(
			     update_firmware_kbuffer, /* to   */
			     update_firmware_ubuffer, /* from */
			     lendian_length * 4       /* in bytes */
			     );
	if (tmp) {
	  printk("[openair][IOCTL] Could NOT copy all data from user-space to kernel-space (%d bytes remained uncopied).\n", tmp);
	  if (update_firmware_kbuffer)
	    kfree(update_firmware_kbuffer);
	  return -1;
	  break;
	}
	
	for (fmw_off = 0 ; fmw_off < (lendian_length * 4) ; fmw_off += 4) {
	  bendian_fmw_off = fmw_off; invert4(bendian_fmw_off);
	  sparc_tmp_0 = update_firmware_address + bendian_fmw_off;
	  invert4(sparc_tmp_0); /* because Sparc is big endian */
	  sparc_tmp_1 = update_firmware_kbuffer[fmw_off/4];
	  invert4(sparc_tmp_1); /* because Sparc is big endian */
	  openair_writel(pdev[0], FROM_GRLIB_CFG_GRPCI_EUR_CTRL0_OFFSET, sparc_tmp_0);
	  openair_writel(pdev[0], FROM_GRLIB_CFG_GRPCI_EUR_CTRL1_OFFSET, sparc_tmp_1);
	  wmb();
	  openair_writel(pdev[0], FROM_GRLIB_CFG_GRPCI_EUR_CTRL_OFFSET,
			 FROM_GRLIB_BOOT_HOK | FROM_GRLIB_IRQ_FROM_PCI_IS_SINGLE_WRITE | FROM_GRLIB_IRQ_FROM_PCI);
	  wmb();
	  /* Poll the IRQ bit */
	  ioctl_ack_cnt = 0;
	  do {
	    openair_readl(pdev[0], FROM_GRLIB_CFG_GRPCI_EUR_CTRL_OFFSET, &tmp);
	    rmb();
	  } while ((tmp & FROM_GRLIB_IRQ_FROM_PCI) && (ioctl_ack_cnt++ < MAX_IOCTL_ACK_CNT));
	  if (tmp & FROM_GRLIB_IRQ_FROM_PCI) {
	    printk("[openair][IOCTL] ERROR: Leon did not acknowledge 'SINGLE_WRITE' irq (after a %u polling loop).\n", MAX_IOCTL_ACK_CNT);
	    kfree(update_firmware_kbuffer);
	    return -1;
	    break;
	  }
	}

	kfree(update_firmware_kbuffer);
	sparc_tmp_0 = update_firmware_address; sparc_tmp_1 = update_firmware_length;
	invert4(sparc_tmp_0); invert4(sparc_tmp_1);
	printk("[openair][IOCTL] ok %u words copied at address 0x%08x (Leon ack after %u polling loops)\n",
	       sparc_tmp_1, sparc_tmp_0, ioctl_ack_cnt);
      }
      else {  // This is ExpressMIMO
	update_firmware_ubuffer   = (unsigned int*)((unsigned int*)arg)[3];
	update_firmware_kbuffer = (unsigned int*)kmalloc(update_firmware_length * 4 /* 4 because kmalloc expects bytes */,
							 GFP_KERNEL);
	if (!update_firmware_kbuffer) {
	  printk("[openair][IOCTL]  Could NOT allocate %u bytes from kernel memory (kmalloc failed).\n", lendian_length * 4);
	  return -1; 
	  break;
	}
	fw_block = (unsigned int *)phys_to_virt(exmimo_pci_bot->firmware_block_ptr);
	/* Copy the data block from user space */
	fw_block[0] = update_firmware_address;
	fw_block[1] = update_firmware_length;
	//	printk("copy_from_user %p => %p (pci) => %p (ahb) length %d\n",update_firmware_ubuffer,&fw_block[16],update_firmware_address,update_firmware_length);
	tmp = copy_from_user(update_firmware_kbuffer,
			     update_firmware_ubuffer, /* from */
			     update_firmware_length * 4       /* in bytes */
			     );
//	pci_map_single(pdev[0],(void*)fw_block, update_firmware_length*4,PCI_DMA_BIDIRECTIONAL);
	for (i=0;i<update_firmware_length;i++) {
	  fw_block[32+i] = ((unsigned int *)update_firmware_kbuffer)[i];
	  // Endian flipping is done in user-space so undo it
	  invert4(fw_block[32+i]);
	}

	kfree(update_firmware_kbuffer);
	
	if (tmp) {
	  printk("[openair][IOCTL] Could NOT copy all data from user-space to kernel-space (%d bytes remained uncopied).\n", tmp);
	  return -1;
	  break;
	}
	
	openair_dma(0,EXMIMO_FW_INIT);
	
	printk("[openair][IOCTL] ok %u words copied at address 0x%08x (fw_block %p)\n",
	       ((unsigned int*)arg)[2],((unsigned int*)arg)[1],fw_block);
	
	
      }
      break;

    case UPDATE_FIRMWARE_CLEAR_BSS:

      update_firmware_bss_address   = ((unsigned int*)arg)[1];
      update_firmware_bss_size      = ((unsigned int*)arg)[2];
      sparc_tmp_0 = update_firmware_bss_address;
      sparc_tmp_1 = update_firmware_bss_size;

      if (vid != XILINX_VENDOR) {  // This is CBMIMO1     
	
	
	//printk("[openair][IOCTL]  BSS address passed to Leon3 = 0x%08x\n", sparc_tmp_0);
	//printk("[openair][IOCTL]  BSS  size   passed to Leon3 = 0x%08x\n", sparc_tmp_1);
	openair_writel(pdev[0], FROM_GRLIB_CFG_GRPCI_EUR_CTRL0_OFFSET, sparc_tmp_0);
	openair_writel(pdev[0], FROM_GRLIB_CFG_GRPCI_EUR_CTRL1_OFFSET, sparc_tmp_1);
	wmb();
	openair_writel(pdev[0], FROM_GRLIB_CFG_GRPCI_EUR_CTRL_OFFSET,
		       FROM_GRLIB_BOOT_HOK | FROM_GRLIB_IRQ_FROM_PCI_IS_CLEAR_BSS | FROM_GRLIB_IRQ_FROM_PCI);
	wmb();
	/* Poll the IRQ bit */
	do {
	  openair_readl(pdev[0], FROM_GRLIB_CFG_GRPCI_EUR_CTRL_OFFSET, &tmp);
	  rmb();
	} while (tmp & FROM_GRLIB_IRQ_FROM_PCI);
	printk("[openair][IOCTL] ok asked Leon to clear .bss (addr 0x%08x, size %d bytes)\n", sparc_tmp_0, sparc_tmp_1);
      }
      else {
	printk("[openair][IOCTL] ok asked Leon to clear .bss (addr 0x%08x, size %d bytes)\n", sparc_tmp_0, sparc_tmp_1);
	fw_block = (unsigned int *)phys_to_virt(exmimo_pci_bot->firmware_block_ptr);
	/* Copy the data block from user space */
	fw_block[0] = update_firmware_bss_address;
	fw_block[1] = update_firmware_bss_size;

	openair_dma(0,EXMIMO_CLEAR_BSS);
	
	
      }
      
      break;
        
    case UPDATE_FIRMWARE_START_EXECUTION:

      update_firmware_start_address = ((unsigned int*)arg)[1];
      update_firmware_stack_pointer = ((unsigned int*)arg)[2];
      sparc_tmp_0 = update_firmware_start_address;
      sparc_tmp_1 = update_firmware_stack_pointer;

      if (vid != XILINX_VENDOR) {  // This is CBMIMO1     

	//printk("[openair][IOCTL]  Entry point   passed to Leon3 = 0x%08x\n", sparc_tmp_0);
	//printk("[openair][IOCTL]  Stack pointer passed to Leon3 = 0x%08x\n", sparc_tmp_1);
	openair_writel(pdev[0], FROM_GRLIB_CFG_GRPCI_EUR_CTRL0_OFFSET, sparc_tmp_0);
	openair_writel(pdev[0], FROM_GRLIB_CFG_GRPCI_EUR_CTRL1_OFFSET, sparc_tmp_1);
	wmb();
	openair_writel(pdev[0], FROM_GRLIB_CFG_GRPCI_EUR_CTRL_OFFSET,
		       FROM_GRLIB_BOOT_HOK | FROM_GRLIB_IRQ_FROM_PCI_IS_JUMP_USER_ENTRY | FROM_GRLIB_IRQ_FROM_PCI);
	wmb();
	/* Poll the IRQ bit */
	ioctl_ack_cnt = 0;
	do {
	  openair_readl(pdev[0], FROM_GRLIB_CFG_GRPCI_EUR_CTRL_OFFSET, &tmp);
	  rmb();
	} while ((tmp & FROM_GRLIB_IRQ_FROM_PCI) && (ioctl_ack_cnt++ < MAX_IOCTL_ACK_CNT));
	if (tmp & FROM_GRLIB_IRQ_FROM_PCI) {
	  printk("[openair][IOCTL] ERROR: Leon did not acknowledge 'START_EXECUTION' irq (after a %u polling loop).\n", MAX_IOCTL_ACK_CNT);
	  return -1;
	  break;
	}
	printk("[openair][IOCTL] ok asked Leon to run firmware (ep = 0x%08x, sp = 0x%08x, Leon ack after %u polling loops)\n",
	       sparc_tmp_0, sparc_tmp_1, ioctl_ack_cnt);
      }
      else {
	printk("[openair][IOCTL] ok asked Leon to set stack and start execution (addr 0x%08x, size %d bytes)\n", sparc_tmp_0, sparc_tmp_1);
	fw_block = (unsigned int *)phys_to_virt(exmimo_pci_bot->firmware_block_ptr);
	/* Copy the data block from user space */
	fw_block[0] = update_firmware_start_address;
	fw_block[1] = update_firmware_stack_pointer;

	openair_dma(0,EXMIMO_START_EXEC);

	udelay(1000);

	exmimo_firmware_init();

      }
    break;
          
    case UPDATE_FIRMWARE_FORCE_REBOOT:

      if (vid != XILINX_VENDOR) {  // This is CBMIMO1     
	openair_writel(pdev[0], FROM_GRLIB_CFG_GRPCI_EUR_CTRL_OFFSET,
		       /*FROM_GRLIB_BOOT_HOK |*/ FROM_GRLIB_IRQ_FROM_PCI_IS_FORCE_REBOOT | FROM_GRLIB_IRQ_FROM_PCI);
	wmb();
	/* We don't wait for any acknowledge from Leon, because it can't acknowledge upon reboot */
	printk("[openair][IOCTL] ok asked Leon to reboot.\n");
      }
      else {
	printk("[openair][IOCTL] ok asked Leon to reboot.\n");
	openair_dma(0,EXMIMO_REBOOT);
      }
      break;
	
    case UPDATE_FIRMWARE_TEST_GOK:
      if (vid != XILINX_VENDOR) {  // This is CBMIMO1     
	/* No loop, just a single test (the polling loop should better be placed in user-space code). */
	openair_readl(pdev[0], FROM_GRLIB_CFG_GRPCI_EUR_CTRL_OFFSET, &tmp);
	rmb();
	if (tmp & FROM_GRLIB_BOOT_GOK)
	  return 0;
	else
	  return -1;
      }
      else {
	printk("[openair][IOCTL] TEST_GOK command doesn't work with ExpressMIMO, check User-space call!!!!\n");
      }
      break;
      
    default:
      return -1;
      break;
      
    }
    break;
  
  case openair_SET_TIMING_ADVANCE:

    for (i=0;i<number_of_cards;i++) 
      pci_interface[i]->timing_advance = ((unsigned int *)arg)[0];

    /*
    openair_daq_vars.manual_timing_advance = 1;
    openair_daq_vars.timing_advance = ((unsigned int *)arg)[0]; 

    msg("[openair][IOCTL] openair_daq_vars.timing_advance = %d\n",openair_daq_vars.timing_advance);

    for (i=0;i<number_of_cards;i++)
      ret = setup_regs(i,frame_parms);

    if (ret != 0)
      msg("[openair][IOCTL] Failed to set timing advance\n");
    */

    break;

  case openair_SET_FREQ_OFFSET:

    openair_daq_vars.freq_offset = ((int *)arg)[0];
    openair_daq_vars.auto_freq_correction = 0;
    if (openair_set_freq_offset(0,((int *)arg)[0]) == 0)
      msg("[openair][IOCTL] Set frequency offset to %d\n",((int *)arg)[0]);
    else 
      msg("[openair][IOCTL] Problem setting frequency offset\n");

  case openair_SET_UE_DL_MCS:

    if ( ((((unsigned int *)arg)[0]) >= 0) && 
	 ((((unsigned int *)arg)[0]) <32) )
      openair_daq_vars.target_ue_dl_mcs = (unsigned char)(((unsigned int *)arg)[0]);
    break;

  case openair_SET_UE_UL_MCS:

    if ( ((((unsigned int *)arg)[0]) >= 0) && 
	 ((((unsigned int *)arg)[0]) <32) )
      openair_daq_vars.target_ue_ul_mcs = (unsigned char)(((unsigned int *)arg)[0]);
    break;

  case openair_SET_UE_UL_NB_RB:

    if ( ((((unsigned int *)arg)[0]) >= 0) && 
	 ((((unsigned int *)arg)[0]) <10) )
      openair_daq_vars.ue_ul_nb_rb = (unsigned char)(((unsigned int *)arg)[0]);
    break;

  case openair_SET_DLSCH_RATE_ADAPTATION:

    if ( ((((unsigned int *)arg)[0]) >= 0) && 
	 ((((unsigned int *)arg)[0]) <2) )
      openair_daq_vars.dlsch_rate_adaptation = (unsigned char)(((unsigned int *)arg)[0]);
    break;

  case openair_SET_DLSCH_TRANSMISSION_MODE:

    if ( ((((unsigned int *)arg)[0]) > 0) && 
	 ((((unsigned int *)arg)[0]) < 7) ) {
      openair_daq_vars.dlsch_transmission_mode = (unsigned char)(((unsigned int *)arg)[0]);
#ifdef OPENAIR2
      transmission_mode_rrc = (int)(((unsigned int *)arg)[0]);
#endif
    }
    if  ((PHY_vars_eNB_g != NULL) && (PHY_vars_eNB_g[0] != NULL))
      // if eNb is already configured, frame parms are local to it
      PHY_vars_eNB_g[0]->lte_frame_parms.mode1_flag = (openair_daq_vars.dlsch_transmission_mode==1);
    else
      // global frame parms have not been copied yet to eNB vars
      frame_parms->mode1_flag = (openair_daq_vars.dlsch_transmission_mode==1);
    break;

  case openair_SET_ULSCH_ALLOCATION_MODE:

    if ( ((((unsigned int *)arg)[0]) >= 0) && 
	 ((((unsigned int *)arg)[0]) <2) )
      openair_daq_vars.ulsch_allocation_mode = (unsigned char)(((unsigned int *)arg)[0]);
    break;

  case openair_SET_RRC_CONN_SETUP:
#ifdef OPENAIR2
    RRC_CONNECTION_FLAG = 1;
    printk("[IOCTL] Setting RRC_CONNECTION_FLAG\n");
#endif
  break;

  case openair_SET_COOPERATION_FLAG:
    if (PHY_vars_eNB_g && PHY_vars_eNB_g[0]) {
      PHY_vars_eNB_g[0]->cooperation_flag = ((unsigned int *)arg)[0];
      printk("[IOCTL] Setting cooperation flag to %d\n",PHY_vars_eNB_g[0]->cooperation_flag);
    }
    else
      printk("[IOCTL] Cooperation flag not set, PHY_vars_eNB_g not allocated!!!\n");
    break;

  case openair_SET_RX_OFFSET:

    for (i=0;i<number_of_cards;i++) 
      pci_interface[i]->frame_offset = ((unsigned int *)arg)[0];

    printk("[IOCTL] Setting frame offset to %d\n", pci_interface[0]->frame_offset);

    break;

  case openair_GET_PCI_INTERFACE:
    if (vid != XILINX_VENDOR) {
      copy_to_user((void *)arg,&pci_interface[0],sizeof(PCI_interface_t*));
      printk("[IOCTL] copying pci_interface[0]=%p to %p\n", pci_interface[0],arg);
    }
    else {
      copy_to_user((void *)arg,&exmimo_pci_interface,sizeof(exmimo_pci_interface_t*));
      printk("[IOCTL] copying exmimo_pci_interface=%p to %p\n", exmimo_pci_interface,arg);
    }
    break;
    

  default:
    //----------------------
    return -EPERM;
    break;
  }
  return 0;
}



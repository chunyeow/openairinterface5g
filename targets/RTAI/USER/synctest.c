/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2011 Eurecom

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information
  Openair Admin: openair_admin@eurecom.fr
  Openair Tech : openair_tech@eurecom.fr
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/

/*! \file synctest.c
* \brief main program to control HW and scheduling
* \author R. Knopp, F. Kaltenberger
* \date 2012
* \version 0.1
* \company Eurecom
* \email: knopp@eurecom.fr,florian.kaltenberger@eurecom.fr
* \note
* \warning
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <signal.h>
#include <execinfo.h>
#include <getopt.h>

#include <rtai_lxrt.h>
#include <rtai_sem.h>
#include <rtai_msg.h>

#include "PHY/types.h"
#include "PHY/defs.h"

#include "ARCH/COMMON/defs.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_device.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_pci.h"
#include "SIMULATION/LTE_PHY/openair_hw.h"

#include "PHY/vars.h"
#include "MAC_INTERFACE/vars.h"
#include "SCHED/vars.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/vars.h"
#include "LAYER2/MAC/vars.h"

#include "../../SIMU/USER/init_lte.h"

#ifdef EMOS
#include "SCHED/phy_procedures_emos.h"
#endif

#ifdef OPENAIR2
#include "LAYER2/MAC/defs.h"
#include "LAYER2/MAC/vars.h"
#ifndef CELLULAR
#include "RRC/LITE/vars.h"
#endif
#include "PHY_INTERFACE/vars.h"
#endif

#include "UTIL/LOG/log_extern.h"
#include "UTIL/OTG/otg.h"
#include "UTIL/OTG/otg_vars.h"
#include "UTIL/MATH/oml.h"


#ifdef XFORMS
#include <forms.h>
#include "lte_scope.h"
//#include "USERSPACE_TOOLS/SCOPE/lte_scope.h"
#include "stats.h"
FD_lte_scope *form_dl=NULL;
FD_stats_form *form_stats=NULL;
#endif //XFORMS

#define FRAME_PERIOD 100000000ULL
#define DAQ_PERIOD 66666ULL

#undef MALLOC //there are two conflicting definitions, so we better make sure we don't use it at all

static SEM *mutex;
//static CND *cond;

static int thread0;
static int thread1;
static int sync_thread;

pthread_t  thread2;

static int instance_cnt=-1; //0 means worker is busy, -1 means its free
int instance_cnt_ptr_kern,*instance_cnt_ptr_user;
int pci_interface_ptr_kern;

extern unsigned int bigphys_top;
extern unsigned int mem_base;

int openair_fd = 0;

int oai_exit = 0;

//PCI_interface_t *pci_interface[3];

unsigned int *DAQ_MBOX;

//int time_offset[4] = {-138,-138,-138,-138};
int time_offset[4] = {-145,-145,-145,-145};
//int time_offset[4] = {0,0,0,0};

int fs4_test=0;
char UE_flag=0;

struct timing_info_t {
  unsigned int frame, hw_slot, last_slot, next_slot;
  RTIME time0, time1, time2;
  unsigned int mbox0, mbox1, mbox2, mbox_target;
} timing_info[20];

extern s16* sync_corr_ue0;
extern s16 prach_ifft[4][1024*2];


runmode_t mode;
int rx_input_level_dBm;
int otg_enabled = 0;

int init_dlsch_threads(void);
void cleanup_dlsch_threads(void);

LTE_DL_FRAME_PARMS *frame_parms;
#ifdef EXMIMO
  u32 carrier_freq[4]= {1907600000,1907600000,1907600000,1907600000};
#endif

void signal_handler(int sig)
{
  void *array[10];
  size_t size;

  oai_exit=1;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, 2);
  exit(-1);
}

void exit_fun(const char* s)
{
  void *array[10];
  size_t size;
  int fd;

  printf("Exiting: %s\n",s);

  oai_exit=1;
  rt_sleep(nano2count(FRAME_PERIOD));

  // cleanup
  stop_rt_timer();

  fd = 0;
  ioctl(openair_fd,openair_STOP,&fd);
  munmap((void*)mem_base, BIGPHYS_NUMPAGES*4096);

  exit (-1);
}

#ifdef XFORMS
void ia_receiver_on_off( FL_OBJECT *button, long arg) {

  if (fl_get_button(button)) {
    if (UE_flag==1) {
      fl_set_object_label(button, "IA Receiver ON");
      openair_daq_vars.use_ia_receiver = 1;
      fl_set_object_color(button, FL_GREEN, FL_GREEN);
      //    LOG_I(PHY,"Pressed the button: IA receiver ON\n");
    }
    else {
      fl_set_object_label(button, "DL traffic ON");
      fl_set_object_color(button, FL_GREEN, FL_GREEN);
      otg_enabled = 1;
    }
  }
  else {
    if (UE_flag==1) {
      fl_set_object_label(button, "IA Receiver OFF");
      openair_daq_vars.use_ia_receiver = 0;
      fl_set_object_color(button, FL_RED, FL_RED);
      //    LOG_I(PHY,"Pressed the button: IA receiver OFF\n");
    }
    else {
      fl_set_object_label(button, "DL traffic OFF");
      fl_set_object_color(button, FL_RED, FL_RED);
      otg_enabled = 0;
    }
  }
}

void do_forms2(FD_lte_scope *form,
               LTE_DL_FRAME_PARMS *frame_parms,
               int pdcch_symbols,
	       int UE_flag,
               s16 **channel,
               s16 **channel_f,
               s16 **rx_sig,
               s16 **rx_sig_f,
               s16 *pdcch_comp,
               s16 *dlsch_comp,
               s16 *dlsch_comp_i,
               s16 *dlsch_llr,
               s16 *pbch_comp,
               s8 *pbch_llr,
	       u32 *avg_tput,
	       u32 *avg_tput_time,
	       u16 tput_window,
               s16 coded_bits_per_codeword,
	       s16 *sync_corr,
	       s16 sync_corr_len)
{

  int i,j,k,s;
  int aa, xx=128, yy=128;//b  
  float x_label[300], y_label[300];//b

  float Re,Im;
  float mag_sig[NB_ANTENNAS_RX*4*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_OFDM_SYMBOLS_PER_SLOT],
  sig_time[NB_ANTENNAS_RX*4*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_OFDM_SYMBOLS_PER_SLOT],
  sig2[FRAME_LENGTH_COMPLEX_SAMPLES],
  time2[FRAME_LENGTH_COMPLEX_SAMPLES],
    I[25*12*11*4], Q[25*12*11*4],tput[tput_window],tput_time[tput_window],
  *llr,*llr_time;
  int ind;
  float avg, cum_avg,tput_max=0;
  int nb_tx_ant = (UE_flag==1 ? frame_parms->nb_antennas_tx_eNB : 1);
  int nb_ce_symb = (UE_flag==1 ? 1 : frame_parms->symbols_per_tti); 
		 
  llr = malloc(coded_bits_per_codeword*sizeof(float));
  llr_time = malloc(coded_bits_per_codeword*sizeof(float));

  // Channel frequency response
  if ((channel_f != NULL) && (channel_f[0] != NULL))
    {
      cum_avg = 0;
      ind = 0;
      for (j=0; j<nb_tx_ant; j++)
        {
          for (i=0; i<frame_parms->nb_antennas_rx; i++)
            {
              for (k=0; k<(12*frame_parms->N_RB_DL*nb_ce_symb); k++)
                {
                  sig_time[ind] = (float)ind;
                  Re = (float)(channel_f[(j<<1)+i][(2*k)]);
                  Im = (float)(channel_f[(j<<1)+i][(2*k)+1]);
                  //mag_sig[ind] = (short) rand();
                  mag_sig[ind] = (short)10*log10(1.0+((double)Re*Re + (double)Im*Im));
                  cum_avg += (short)sqrt((double)Re*Re + (double)Im*Im) ;
                  ind++;
                }
              //      ind+=NUMBER_OF_OFDM_CARRIERS/4; // spacing for visualization
            }
        }

      avg = cum_avg/NUMBER_OF_USEFUL_CARRIERS;

      //fl_set_xyplot_ybounds(form->channel_f,30,70);
      fl_set_xyplot_data(form->channel_f,sig_time,mag_sig,ind,"","","");
    }

  // time domain channel
  if ((channel != NULL) && (channel[0] !=NULL))
    {
      for (i=0; i<512; i++)
        {
          time2[i] = (float) i;
          sig2[i] = (float) (channel[0][4*i]*channel[0][4*i]+channel[0][4*i+1]*channel[0][4*i+1]);
        }
      fl_set_xyplot_data(form->channel_t_im,time2,sig2,128,"","","");
      //fl_set_xyplot_ybounds(form->channel_t_im,0,1e6);
    }

  /*
  // sync_corr
  if (sync_corr != NULL)
    {
      for (i=0; i<sync_corr_len; i++)
        {
          time2[i] = (float) i;
          sig2[i] = (float) sync_corr[i];
        }
      fl_set_xyplot_data(form->channel_t_im,time2,sig2,sync_corr_len,"","","");
      //fl_set_xyplot_ybounds(form->channel_t_im,0,1e6);
    }
  */

  // rx sig 0
  if (rx_sig != NULL) { 
    if  (rx_sig[0] != NULL)
      {
	//for (i=30720; i<38400; i++)
	//for (i=0; i<NUMBER_OF_OFDM_CARRIERS*frame_parms->symbols_per_tti/2; i++)  {
	for (i=0; i<FRAME_LENGTH_COMPLEX_SAMPLES; i++)
	  {
	    sig2[i] = 10*log10(1.0+(double) ((rx_sig[0][2*i])*(rx_sig[0][2*i])+(rx_sig[0][2*i+1])*(rx_sig[0][2*i+1])));
	    //sig2[i] = (float) ((rx_sig[0][2*i]));
	    time2[i] = (float) i;
	  }
	fl_set_xyplot_ybounds(form->channel_t_re,30,70);
	//fl_set_xyplot_data(form->channel_t_re,&time2[640*12*6],&sig2[640*12*6],640*12,"","","");
	fl_set_xyplot_data(form->channel_t_re,time2,sig2,FRAME_LENGTH_COMPLEX_SAMPLES,"","","");
      }
    /*
    // rx sig 1
    if (rx_sig[1] !=NULL) {
      //for (i=30720; i<38400; i++)
	for (i=0; i<FRAME_LENGTH_COMPLEX_SAMPLES; i++)
	  {
	    sig2[i] = 10*log10(1.0+(double) ((rx_sig[1][2*i])*(rx_sig[1][2*i])+(rx_sig[1][2*i+1])*(rx_sig[1][2*i+1])));
	    //sig2[i] = (float) ((rx_sig[1][2*i]));
	    time2[i] = (float) i;
	  }
	fl_set_xyplot_ybounds(form->channel_t_im,30,70);
	//fl_set_xyplot_data(form->channel_t_im,&time2[640*12*6],&sig2[640*12*6],640*12,"","","");
	fl_set_xyplot_data(form->channel_t_im,time2,sig2,FRAME_LENGTH_COMPLEX_SAMPLES,"","","");
    }
    */
  }

  // PBCH LLR
  if (pbch_llr!=NULL)
    {
      j=0;
      for (i=0; i<1920; i++)
        {
          llr[j] = (float) pbch_llr[i];
          llr_time[j] = (float) j;
          //if (i==63)
          //  i=127;
          //else if (i==191)
          //  i=319;
          j++;
        }

      fl_set_xyplot_data(form->decoder_input,llr_time,llr,1920,"","","");
      //fl_set_xyplot_ybounds(form->decoder_input,-100,100);
    }

  // PBCH I/Q
  if (pbch_comp!=NULL)
    {
      j=0;
      for (i=0; i<12*12; i++)
        {
          I[j] = pbch_comp[2*i];
          Q[j] = pbch_comp[2*i+1];
          j++;
          //if (i==47)
          //  i=96;
          //else if (i==191)
          //  i=239;
        }

      fl_set_xyplot_data(form->scatter_plot,I,Q,12*12,"","","");
      fl_set_xyplot_xbounds(form->scatter_plot,-100,100);
      fl_set_xyplot_ybounds(form->scatter_plot,-100,100);
    }


  // PDCCH I/Q
  if (pdcch_comp!=NULL)
    {
      j=0;
      for (i=0; i<12*25*1; i++)
        {
          I[j] = pdcch_comp[2*i];
          Q[j] = pdcch_comp[2*i+1];
          j++;
        }

      fl_set_xyplot_data(form->scatter_plot1,I,Q,12*25*1,"","","");
      fl_set_xyplot_xbounds(form->scatter_plot1,-100,100);
      fl_set_xyplot_ybounds(form->scatter_plot1,-100,100);
    }

  // DLSCH LLR
  if (dlsch_llr != NULL)
    {
      for (i=0; i<coded_bits_per_codeword; i++)
        {
          llr[i] = (float) dlsch_llr[i];
          llr_time[i] = (float) i;
        }

      fl_set_xyplot_data(form->demod_out,llr_time,llr,coded_bits_per_codeword,"","","");
      //    fl_set_xyplot_ybounds(form->demod_out,-1000,1000);
    }
  else
    printf("lls==NULL\n");

  // DLSCH I/Q
  if (dlsch_comp!=NULL)
    {
      j=0;
      for (s=pdcch_symbols; s<frame_parms->symbols_per_tti; s++)
        {
          for (i=0; i<12*25; i++)
            {
              I[j] = dlsch_comp[(2*25*12*s)+2*i];
              Q[j] = dlsch_comp[(2*25*12*s)+2*i+1];
              j++;
            }
          //if (s==2)
          //  s=3;
          //else if (s==5)
          //  s=6;
          //else if (s==8)
          //  s=9;
        }

      fl_set_xyplot_data(form->scatter_plot2,I,Q,j,"","","");
      //fl_set_xyplot_xbounds(form->scatter_plot2,-100,100);
      //fl_set_xyplot_ybounds(form->scatter_plot2,-100,100);
    }

  // Throughput
  if (avg_tput!=NULL) {
      for (i=0; i<tput_window; i++) {
	tput[i] = (float) avg_tput[i];
	tput_time[i] = (float) avg_tput_time[i];
	if (tput[i] > tput_max) {
	  tput_max = tput[i];
	}
      } 
      fl_set_xyplot_data(form->tput,tput_time,tput,tput_window,"","","");
      fl_set_xyplot_ybounds(form->tput,0,tput_max);
  }

  fl_check_forms();

  free(llr);
  free(llr_time);

}

void *scope_thread(void *arg)
{
  s16 prach_corr[1024], i;
  char stats_buffer[16384];
  //FILE *UE_stats, *eNB_stats;
  int len=0;
  u16 tput_window = 100;
  unsigned int avg_tput_eNB[tput_window];
  unsigned int avg_tput_UE[tput_window];
  unsigned int tput_time_UE[tput_window];
  unsigned int tput_time_eNB[tput_window];
  memset((void*) avg_tput_UE,0,sizeof(unsigned int)*tput_window);
  memset((void*) avg_tput_eNB,0,sizeof(unsigned int)*tput_window);
  memset((void*) tput_time_UE,0,sizeof(unsigned int)*tput_window);
  memset((void*) tput_time_eNB,0,sizeof(unsigned int)*tput_window);

  /*
  if (UE_flag==1) 
    UE_stats  = fopen("UE_stats.txt", "w");
  else 
    eNB_stats = fopen("eNB_stats.txt", "w");
  */

  while (!oai_exit)
    {
      if (UE_flag==1) {
	len = dump_ue_stats (PHY_vars_UE_g[0], stats_buffer, 0, mode,rx_input_level_dBm);
	fl_set_object_label(form_stats->stats_text, stats_buffer);
	//rewind (UE_stats);
	//fwrite (stats_buffer, 1, len, UE_stats);
	/*
	if (PHY_vars_UE_g[0]->frame<tput_window) {
	  avg_tput_UE[PHY_vars_UE_g[0]->frame] = PHY_vars_UE_g[0]->bitrate[0]/1000;
	  tput_time_UE[PHY_vars_UE_g[0]->frame] = PHY_vars_UE_g[0]->frame;
	} 
	else {
	*/
	  memcpy((void*)avg_tput_UE,(void*)&avg_tput_UE[1],(tput_window-1)*sizeof(unsigned int));
	  memcpy((void*)tput_time_UE,(void*)&tput_time_UE[1],(tput_window-1)*sizeof(unsigned int));
	  avg_tput_UE[tput_window-1] = PHY_vars_UE_g[0]->bitrate[0]/1000;
	  tput_time_UE[tput_window-1] = PHY_vars_UE_g[0]->frame;

        do_forms2(form_dl,
                  &(PHY_vars_UE_g[0]->lte_frame_parms),
                  PHY_vars_UE_g[0]->lte_ue_pdcch_vars[0]->num_pdcch_symbols,
		  UE_flag,
                  (s16**)PHY_vars_UE_g[0]->lte_ue_common_vars.dl_ch_estimates_time[0],
                  (s16**)PHY_vars_UE_g[0]->lte_ue_common_vars.dl_ch_estimates[0],
                  (s16**)PHY_vars_UE_g[0]->lte_ue_common_vars.rxdata,
                  (s16**)PHY_vars_UE_g[0]->lte_ue_common_vars.rxdataF,
                  (s16*)PHY_vars_UE_g[0]->lte_ue_pdcch_vars[0]->rxdataF_comp[0],
                  (s16*)PHY_vars_UE_g[0]->lte_ue_pdsch_vars[0]->rxdataF_comp[0],
                  (s16*)PHY_vars_UE_g[0]->lte_ue_pdsch_vars[1]->rxdataF_comp[0],
                  (s16*)PHY_vars_UE_g[0]->lte_ue_pdsch_vars[0]->llr[0],
                  (s16*)PHY_vars_UE_g[0]->lte_ue_pbch_vars[0]->rxdataF_comp[0],
                  (s8*)PHY_vars_UE_g[0]->lte_ue_pbch_vars[0]->llr,
		  avg_tput_UE,
		  tput_time_UE,
		  tput_window,
		  15000,
		  /*get_G(&PHY_vars_UE_g[0]->lte_frame_parms,
			PHY_vars_UE_g[0]->dlsch_ue[0][0]->nb_rb,
			PHY_vars_UE_g[0]->dlsch_ue[0][0]->rb_alloc,
			get_Qm(PHY_vars_UE_g[0]->dlsch_ue[0][0]->harq_processes[0]->mcs),  
			PHY_vars_UE_g[0]->lte_ue_pdcch_vars[0]->num_pdcch_symbols,7),*/
		  sync_corr_ue0,
		  PHY_vars_UE_g[0]->lte_frame_parms.samples_per_tti*10);
      }
      else {
	len = dump_eNB_stats (PHY_vars_eNB_g[0], stats_buffer, 0);
	fl_set_object_label(form_stats->stats_text, stats_buffer);
	//rewind (eNB_stats);
	//fwrite (stats_buffer, 1, len, eNB_stats);

	/*
	if (PHY_vars_eNB_g[0]->frame<tput_window) {
	  avg_tput_eNB[PHY_vars_UE_g[0]->frame] = PHY_vars_UE_g[0]->bitrate[0]/1000;
	  tput_time_UE[PHY_vars_UE_g[0]->frame] = PHY_vars_UE_g[0]->frame;
	} 
	else {
	*/
	  memcpy((void*)avg_tput_eNB,(void*)&avg_tput_eNB[1],(tput_window-1)*sizeof(unsigned int));
	  memcpy((void*)tput_time_eNB,(void*)&tput_time_eNB[1],(tput_window-1)*sizeof(unsigned int));
	  avg_tput_eNB[tput_window-1] = PHY_vars_eNB_g[0]->total_dlsch_bitrate/1000;
	  tput_time_eNB[tput_window-1] = PHY_vars_eNB_g[0]->frame;
	
	for (i=0;i<1024;i++) 
	  prach_corr[i] = ((s32)prach_ifft[0][i<<2]*prach_ifft[0][i<<2]+
			   (s32)prach_ifft[0][1+(i<<2)]*prach_ifft[0][1+(i<<2)]) >> 15;
        do_forms2(form_dl,
                  &(PHY_vars_eNB_g[0]->lte_frame_parms),
                  0,
		  UE_flag,
                  (s16**)PHY_vars_eNB_g[0]->lte_eNB_pusch_vars[0]->drs_ch_estimates_time[0],
                  (s16**)PHY_vars_eNB_g[0]->lte_eNB_pusch_vars[0]->drs_ch_estimates[0],
                  (s16**)PHY_vars_eNB_g[0]->lte_eNB_common_vars.rxdata[0],
                  (s16**)PHY_vars_eNB_g[0]->lte_eNB_common_vars.rxdataF[0],
		  NULL,
                  (s16*)PHY_vars_eNB_g[0]->lte_eNB_pusch_vars[0]->rxdataF_comp[0][0],
                  NULL,
                  (s16*)PHY_vars_eNB_g[0]->lte_eNB_pusch_vars[0]->llr,
                  NULL,
                  NULL,
		  avg_tput_eNB,
		  tput_time_eNB,
		  tput_window,
		  PHY_vars_eNB_g[0]->ulsch_eNB[0]->harq_processes[0]->nb_rb*12*get_Qm(PHY_vars_eNB_g[0]->ulsch_eNB[0]->harq_processes[0]->mcs)*PHY_vars_eNB_g[0]->ulsch_eNB[0]->Nsymb_pusch,
                  prach_corr,
                  1024);


      }
      //printf("doing forms\n");
      usleep(100000); // 100 ms
    }

  //fclose (UE_stats);
  //fclose (eNB_stats);

  return (void*)(1);
}
#endif

int dummy_tx_buffer[3840*4] __attribute__((aligned(16)));

static void *sync_hw(void *arg)
{
  RT_TASK *task;
  task = rt_task_init_schmod(nam2num("TASK2"), 0, 0, 0, SCHED_FIFO, 0xF);
  mlockall(MCL_CURRENT | MCL_FUTURE);

  rt_printk("fun0: task %p\n",task);

#ifdef HARD_RT
  rt_make_hard_real_time();
#endif

  while (!oai_exit)
    {

      rt_printk("exmimo_pci_interface->mbox = %d\n",((unsigned int *)DAQ_MBOX)[0]);

      rt_sleep(nano2count(FRAME_PERIOD*10));
    }

}


/* This is the main eNB thread. It gets woken up by the kernel driver using the RTAI message mechanism (rt_send and rt_receive). */
static void *eNB_thread(void *arg)
{
  RT_TASK *task;
  int slot=0,hw_slot,last_slot, next_slot,frame=0;
  unsigned int msg1;
  unsigned int aa,slot_offset, slot_offset_F;
  int diff;
  int delay_cnt;
  RTIME time_in;
  int mbox_target=0,mbox_current=0;
  int i;
  int tx_offset;

  task = rt_task_init_schmod(nam2num("TASK0"), 0, 0, 0, SCHED_FIFO, 0xF);
  mlockall(MCL_CURRENT | MCL_FUTURE);

  rt_printk("Started eNB thread (id %p)\n",task);

#ifdef HARD_RT
  rt_make_hard_real_time();
#endif

  while (!oai_exit)
    {
      //      rt_printk("eNB: slot %d\n",slot);

#ifdef CBMIMO1
      rt_sem_wait(mutex);
      /*
      if ((slot%2000)<10)
        rt_printk("fun0: Hello World %d, instance_cnt %d!\n",slot,*instance_cnt_ptr_user);
      */
      while (*instance_cnt_ptr_user<0)
        {
          rt_sem_signal(mutex);
          rt_receive(0,&msg1);
          rt_sem_wait(mutex);
          /*
                if ((slot%2000)<10)
                  rt_printk("fun0: instance_cnt %d, msg1 %d!\n",*instance_cnt_ptr_user,msg1);
          */
        }
      rt_sem_signal(mutex);
      slot = msg1 % LTE_SLOTS_PER_FRAME;

#else
      hw_slot = (((((unsigned int *)DAQ_MBOX)[0]+1)%150)<<1)/15;
      //this is the mbox counter where we should be 
      mbox_target = ((((slot+1)%20)*15+1)>>1)%150;
      //this is the mbox counter where we are
      mbox_current = ((unsigned int *)DAQ_MBOX)[0];
      //this is the time we need to sleep in order to synchronize with the hw (in multiples of DAQ_PERIOD)
      if ((mbox_current>=135) && (mbox_target<15)) //handle the frame wrap-arround
	diff = 150-mbox_current+mbox_target;
      else if ((mbox_current<15) && (mbox_target>=135))
	diff = -150+mbox_target-mbox_current;
      else
        diff = mbox_target - mbox_current;
 
      if (diff < (-5)) {
	rt_printk("eNB Frame %d: missed slot, proceeding with next one (slot %d, hw_slot %d, diff %d)\n",frame, slot, hw_slot, diff);
	slot++;
	if (slot==20)
          slot=0;
	continue;
      }
      if (diff>8) 
	rt_printk("eNB Frame %d: skipped slot, waiting for hw to catch up (slot %d, hw_slot %d, mbox_current %d, mbox_target %d, diff %d)\n",frame, slot, hw_slot, mbox_current, mbox_target, diff);

      delay_cnt = 0;
      while ((diff>0) && (!oai_exit))
        {
	  time_in = rt_get_time_ns();
	  //rt_printk("eNB Frame %d delaycnt %d : hw_slot %d (%d), slot %d, (slot+1)*15=%d, diff %d, time %llu\n",frame,delay_cnt,hw_slot,((unsigned int *)DAQ_MBOX)[0],slot,(((slot+1)*15)>>1),diff,time_in);
	  //rt_printk("Frame %d: slot %d, sleeping for %llu\n", frame, slot, diff*DAQ_PERIOD);
          rt_sleep(nano2count(diff*DAQ_PERIOD));
          hw_slot = (((((unsigned int *)DAQ_MBOX)[0]+1)%150)<<1)/15;
          //rt_printk("eNB Frame %d : hw_slot %d, time %llu\n",frame,hw_slot,rt_get_time_ns());
          delay_cnt++;
          if (delay_cnt == 10)
            {
              oai_exit = 1;
              rt_printk("eNB Frame %d: HW stopped ... \n",frame);
            }
	  mbox_current = ((unsigned int *)DAQ_MBOX)[0];
	  if ((mbox_current>=135) && (mbox_target<15)) //handle the frame wrap-arround
	    diff = 150-mbox_current+mbox_target;
	  else
	    diff = mbox_target - mbox_current;
        }

#endif
      last_slot = (slot)%LTE_SLOTS_PER_FRAME;
      if (last_slot <0)
        last_slot+=20;
      next_slot = (slot+3)%LTE_SLOTS_PER_FRAME;

      //PHY_vars_eNB_g[0]->frame = frame;
      if (frame>5)
        {
	  /*
          if (frame%100==0)
            rt_printk("frame %d (%d), slot %d, hw_slot %d, next_slot %d (before): DAQ_MBOX %d\n",frame, PHY_vars_eNB_g[0]->frame, slot, hw_slot,next_slot,DAQ_MBOX[0]);
	  */
          if (fs4_test==0)
            {
              phy_procedures_eNB_lte (last_slot, next_slot, PHY_vars_eNB_g[0], 0);
#ifndef IFFT_FPGA
              slot_offset_F = (next_slot)*
                              (PHY_vars_eNB_g[0]->lte_frame_parms.ofdm_symbol_size)*
                              ((PHY_vars_eNB_g[0]->lte_frame_parms.Ncp==1) ? 6 : 7);
              slot_offset = (next_slot)*
                            (PHY_vars_eNB_g[0]->lte_frame_parms.samples_per_tti>>1);
              if ((subframe_select(&PHY_vars_eNB_g[0]->lte_frame_parms,next_slot>>1)==SF_DL)||
                  ((subframe_select(&PHY_vars_eNB_g[0]->lte_frame_parms,next_slot>>1)==SF_S)&&((next_slot&1)==0)))
                {
                  //	  rt_printk("Frame %d: Generating slot %d\n",frame,next_slot);

                  for (aa=0; aa<PHY_vars_eNB_g[0]->lte_frame_parms.nb_antennas_tx; aa++)
                    {
                      if (PHY_vars_eNB_g[0]->lte_frame_parms.Ncp == 1)
                        {
                          PHY_ofdm_mod(&PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdataF[0][aa][slot_offset_F],
#ifdef BIT8_TX
                                       &PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa][slot_offset>>1],
#else
                                       dummy_tx_buffer,//&PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa][slot_offset],
#endif
                                       PHY_vars_eNB_g[0]->lte_frame_parms.log2_symbol_size,
                                       6,
                                       PHY_vars_eNB_g[0]->lte_frame_parms.nb_prefix_samples,
                                       PHY_vars_eNB_g[0]->lte_frame_parms.twiddle_ifft,
                                       PHY_vars_eNB_g[0]->lte_frame_parms.rev,
                                       CYCLIC_PREFIX);
                        }
                      else
                        {
                          normal_prefix_mod(&PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdataF[0][aa][slot_offset_F],
#ifdef BIT8_TX
                                            &PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa][slot_offset>>1],
#else
                                            dummy_tx_buffer,//&PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa][slot_offset],
#endif
                                            7,
                                            &(PHY_vars_eNB_g[0]->lte_frame_parms));
                        }
#ifdef EXMIMO
		      for (i=0; i<PHY_vars_eNB_g[0]->lte_frame_parms.samples_per_tti/2; i++) 
			{
			  tx_offset = (int)slot_offset+time_offset[aa]+i;
			  if (tx_offset<0)
			    tx_offset += LTE_NUMBER_OF_SUBFRAMES_PER_FRAME*PHY_vars_eNB_g[0]->lte_frame_parms.samples_per_tti;
			  if (tx_offset>=(LTE_NUMBER_OF_SUBFRAMES_PER_FRAME*PHY_vars_eNB_g[0]->lte_frame_parms.samples_per_tti))
			    tx_offset -= LTE_NUMBER_OF_SUBFRAMES_PER_FRAME*PHY_vars_eNB_g[0]->lte_frame_parms.samples_per_tti;
			  ((short*)&PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa][tx_offset])[0]=
			    ((short*)dummy_tx_buffer)[2*i]<<4;
			  ((short*)&PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa][tx_offset])[1]=
			    ((short*)dummy_tx_buffer)[2*i+1]<<4;
			}
#endif //EXMIMO
                    }
                }
            }

#endif //IFFT_FPGA
	  /*
          if (frame%100==0)
            rt_printk("hw_slot %d (after): DAQ_MBOX %d\n",hw_slot,DAQ_MBOX[0]);
	  */
        }

      /*
      if ((slot%2000)<10)
      rt_printk("fun0: doing very hard work\n");
      */
#ifndef CBMIMO1
      slot++;
      if (slot==20)
        slot=0;
#endif
      //slot++;
      if ((slot%20)==0)
        frame++;
#ifdef CBMIMO1
      rt_sem_wait(mutex);
      (*instance_cnt_ptr_user)--;
      //rt_printk("fun0: instance_cnt %d!\n",*instance_cnt_ptr_user);
      rt_sem_signal(mutex);
#endif

    }

  rt_printk("fun0: finished, ran %d times.\n",slot);

#ifdef HARD_RT
  rt_make_soft_real_time();
#endif

  // clean task
  rt_task_delete(task);
  rt_printk("Task deleted. returning\n");
  return 0;
}

/* This is the main UE thread. Initially it is doing a periodic get_frame. One synchronized it gets woken up by the kernel driver using the RTAI message mechanism (rt_send and rt_receive). */
static void *UE_thread(void *arg)
{
  RT_TASK *task;
  RTIME in, out, diff;
  int slot=0,frame=0,hw_slot,last_slot, next_slot;
  unsigned int msg1;
  unsigned int aa,slot_offset, slot_offset_F;
  static int is_synchronized = 0;
  static int received_slots = 0;
  static int slot0 = 0;
  int delay_cnt;
  RTIME time_in;
  int hw_slot_offset=0,rx_offset_mbox=0,mbox_target=0,mbox_current=0;
  int diff2;
  static int first_run=1;
  int carrier_freq_offset = 0; //-7500;
  int i;

  task = rt_task_init_schmod(nam2num("TASK0"), 0, 0, 0, SCHED_FIFO, 0xF);
  mlockall(MCL_CURRENT | MCL_FUTURE);

  rt_printk("Started UE thread (id %p)\n",task);

#ifdef HARD_RT
  rt_make_hard_real_time();
#endif
  
  if (mode == rx_calib_ue) {
    carrier_freq_offset = -7500;
    for (i=0; i<4; i++) {
      frame_parms->carrier_freq[i] = carrier_freq[i]+carrier_freq_offset;
      frame_parms->carrier_freqtx[i] = carrier_freq[i]+carrier_freq_offset;
    }
    ioctl(openair_fd,openair_DUMP_CONFIG,frame_parms);	      
    //    dump_frame_parms(frame_parms);
  }

  while (!oai_exit)
    {
#ifdef CBMIMO1
      rt_sem_wait(mutex);
      /*
      if ((slot%2000)<10)
        rt_printk("fun0: Hello World %d, instance_cnt %d!\n",slot,*instance_cnt_ptr_user);
      */
      while (*instance_cnt_ptr_user<0)
        {
          rt_sem_signal(mutex);
          rt_receive(0,&msg1);
          rt_sem_wait(mutex);
          /*
                if ((slot%2000)<10)
                  rt_printk("fun0: instance_cnt %d, msg1 %d!\n",*instance_cnt_ptr_user,msg1);
          */
        }

      rt_sem_signal(mutex);

      slot = (msg1 - slot0) % LTE_SLOTS_PER_FRAME;
#else
      hw_slot = (((((unsigned int *)DAQ_MBOX)[0]+1)%150)<<1)/15; //the slot the hw is about to store
      
      if (is_synchronized) {
      //this is the mbox counter that indicates the start of the frame
      rx_offset_mbox = (PHY_vars_UE_g[0]->rx_offset * 150) / (10*PHY_vars_UE_g[0]->lte_frame_parms.samples_per_tti); 
      //this is the mbox counter where we should be 
      mbox_target = (((((slot+1)%20)*15+1)>>1) + rx_offset_mbox + 1)%150;
      //this is the mbox counter where we are
      mbox_current = ((unsigned int *)DAQ_MBOX)[0];
      //this is the time we need to sleep in order to synchronize with the hw (in multiples of DAQ_PERIOD)
      if ((mbox_current>=120) && (mbox_target<30)) //handle the frame wrap-arround
	diff2 = 150-mbox_current+mbox_target;
      else if ((mbox_current<30) && (mbox_target>=120))
	diff2 = -150+mbox_target-mbox_current;
      else
        diff2 = mbox_target - mbox_current;

      if (diff2 <(-5)) {
	rt_printk("UE Frame %d: missed slot, proceeding with next one (slot %d, hw_slot %d, diff %d)\n",frame, slot, hw_slot, diff2);
	slot++;
	if (slot==20)
          slot=0;
	continue;
      }
      if (diff2>8) 
	rt_printk("UE Frame %d: skipped slot, waiting for hw to catch up (slot %d, hw_slot %d, mbox_current %d, mbox_target %d, diff %d)\n",frame, slot, hw_slot, mbox_current, mbox_target, diff2);

      /*
      if (frame%100==0)
	rt_printk("frame %d (%d), slot %d, hw_slot %d, rx_offset_mbox %d, mbox_target %d, mbox_current %d, diff %d\n",frame, PHY_vars_UE_g[0]->frame, slot,hw_slot,rx_offset_mbox,mbox_target,mbox_current,diff2);
      */
      timing_info[slot].time0 = rt_get_time_ns();
      timing_info[slot].mbox0 = ((unsigned int *)DAQ_MBOX)[0];

      delay_cnt = 0;
      while ((diff2>0) && (!oai_exit) && (is_synchronized) )
        {
          rt_sleep(nano2count(diff2*DAQ_PERIOD)); 
          hw_slot = (((((unsigned int *)DAQ_MBOX)[0]+1)%150)<<1)/15;
          delay_cnt++;
          if (delay_cnt == 30)
            {
              oai_exit = 1;
              rt_printk("UE frame %d: HW stopped ... \n",frame);
            }
	  mbox_current = ((unsigned int *)DAQ_MBOX)[0];
	  if ((mbox_current>=135) && (mbox_target<15)) //handle the frame wrap-arround
	    diff2 = 150-mbox_current+mbox_target;
	  else
	    diff2 = mbox_target - mbox_current;
        }

      timing_info[slot].time1 = rt_get_time_ns();
      timing_info[slot].mbox1 = ((unsigned int *)DAQ_MBOX)[0];
      timing_info[slot].mbox_target = mbox_target;
      }
#endif
      last_slot = (slot)%LTE_SLOTS_PER_FRAME;
      if (last_slot <0)
        last_slot+=LTE_SLOTS_PER_FRAME;
      next_slot = (slot+3)%LTE_SLOTS_PER_FRAME;

      timing_info[slot].frame = PHY_vars_UE_g[0]->frame;
      timing_info[slot].hw_slot = hw_slot;
      timing_info[slot].last_slot = last_slot;
      timing_info[slot].next_slot = next_slot;

      if (is_synchronized)
        {
	  /*
          if (frame%100==0)
            rt_printk("frame %d (%d), slot %d, hw_slot %d, last_slot %d (before): DAQ_MBOX %d\n",frame, PHY_vars_UE_g[0]->frame, slot,hw_slot,last_slot,DAQ_MBOX[0]);
	  */
          in = rt_get_time_ns();
          phy_procedures_UE_lte (last_slot, next_slot, PHY_vars_UE_g[0], 0, 0,mode);
          out = rt_get_time_ns();
          diff = out-in;
	  /*
          if (frame % 100 == 0)
            rt_printk("hw_slot %d (after): DAQ_MBOX %d\n",hw_slot,DAQ_MBOX[0]);
	  
            rt_printk("Frame %d: last_slot %d, phy_procedures_lte_ue time_in %llu, time_out %llu, diff %llu\n",
                      frame, last_slot,in,out,diff);
	  */

	  timing_info[slot].time2 = rt_get_time_ns();
	  timing_info[slot].mbox2 = ((unsigned int *)DAQ_MBOX)[0];

        }
      else   // we are not yet synchronized
        {
          hw_slot_offset = 0;

#ifdef CBMIMO1
          if (received_slots==0)
            {
              ioctl(openair_fd,openair_GET_BUFFER,NULL);
            }
          if (received_slots==(100*LTE_SLOTS_PER_FRAME)-1)   //we got enough slots so we can do sync (the factor 100 is to wait some time longer)
            {
              rt_printk("fun0: slot %d: doing sync\n",slot);
              received_slots = -1; // will be increased below
              if (initial_sync(PHY_vars_UE_g[0],mode)==0)
                {
                  lte_adjust_synch(&PHY_vars_UE_g[0]->lte_frame_parms,
                                   PHY_vars_UE_g[0],
                                   0,
                                   1,
                                   16384);

                  ioctl(openair_fd,openair_SET_RX_OFFSET,&PHY_vars_UE_g[0]->rx_offset); //synchronize hardware
                  // here we should actually do another dump config with the parameters obtained from the sync.

                  ioctl(openair_fd,openair_START_TX_SIG,NULL); //start the DMA transfers

                  //for better visualization afterwards
                  for (aa=0; aa<PHY_vars_UE_g[0]->lte_frame_parms.nb_antennas_rx; aa++)
                    memset(PHY_vars_UE_g[0]->lte_ue_common_vars.rxdata[aa],0,
                           PHY_vars_UE_g[0]->lte_frame_parms.samples_per_tti*LTE_NUMBER_OF_SUBFRAMES_PER_FRAME*sizeof(int));
                  is_synchronized = 1;
                  slot0 = msg1;
                }
            }
          received_slots++;
#else
          slot = 0;
          ioctl(openair_fd,openair_GET_BUFFER,NULL);
          rt_sleep(nano2count(FRAME_PERIOD));
          //	  rt_printk("fun0: slot %d: doing sync\n",slot);

          if (initial_sync(PHY_vars_UE_g[0],mode)==0) {
              /*
              lte_adjust_synch(&PHY_vars_UE_g[0]->lte_frame_parms,
                   PHY_vars_UE_g[0],
                   0,
                   1,
                   16384);
              */
              //for better visualization afterwards
              /*
              for (aa=0; aa<PHY_vars_UE_g[0]->lte_frame_parms.nb_antennas_rx; aa++)
              memset(PHY_vars_UE_g[0]->lte_ue_common_vars.rxdata[aa],0,
                 PHY_vars_UE_g[0]->lte_frame_parms.samples_per_tti*LTE_NUMBER_OF_SUBFRAMES_PER_FRAME*sizeof(int));
              */
	      if (mode == rx_calib_ue) {
		oai_exit=1;
	      }
	      else {
		is_synchronized = 1;
		ioctl(openair_fd,openair_START_TX_SIG,NULL); //start the DMA transfers
		
		hw_slot_offset = (PHY_vars_UE_g[0]->rx_offset<<1) / PHY_vars_UE_g[0]->lte_frame_parms.samples_per_tti;
		rt_printk("Got synch: hw_slot_offset %d\n",hw_slot_offset);
	      }
	  }
	  else {
	    carrier_freq_offset += 100;
	    if (carrier_freq_offset > 7500) {
	      LOG_I(PHY,"[initial_sync] No cell synchronization found, abondoning\n");
	      oai_exit = 1;
	    }
	    else {
	      LOG_I(PHY,"[initial_sync] trying carrier off %d Hz\n",carrier_freq_offset);
	      for (i=0; i<4; i++) {
		frame_parms->carrier_freq[i] = carrier_freq[i]+carrier_freq_offset;
		frame_parms->carrier_freqtx[i] = carrier_freq[i]+carrier_freq_offset;
	      }
	      ioctl(openair_fd,openair_DUMP_CONFIG,frame_parms);	      
	      //	      dump_frame_parms(frame_parms);
	      
	    }
	  }

#endif
        }

      /*
      if ((slot%2000)<10)
        rt_printk("fun0: doing very hard work\n");
      */
#ifdef CBMIMO1
      rt_sem_wait(mutex);
      (*instance_cnt_ptr_user)--;
      //rt_printk("fun0: instance_cnt %d!\n",*instance_cnt_ptr_user);
      rt_sem_signal(mutex);
#else
      slot++;
      if (slot==20)
        {
          slot=0;
          frame++;
        }
#endif
    }
  rt_printk("fun0: finished, ran %d times.\n",slot);

#ifdef HARD_RT
  rt_make_soft_real_time();
#endif

  // clean task
  rt_task_delete(task);
  rt_printk("Task deleted. returning\n");
  return 0;
}





int main(int argc, char **argv) {

  RT_TASK *task;
  int i,j,aa;


#ifdef EXMIMO
  u32 rf_mode_max[4]     = {55759,55759,55759,55759};
  u32 rf_mode_med[4]     = {39375,39375,39375,39375};
  u32 rf_mode_byp[4]     = {22991,22991,22991,22991};

  u32 rf_local[4]    = {8255000,8255000,8255000,8255000}; // UE zepto
    //{8254617, 8254617, 8254617, 8254617}; //eNB khalifa
    //{8255067,8254810,8257340,8257340}; // eNB PETRONAS

  u32 rf_vcocal[4]   = {910,910,910,910};
  u32 rf_rxdc[4]     = {32896,32896,32896,32896};
  u32 rxgain[4]={30,30,30,30};
  u32 txgain[4]={25,25,25,25};
#endif

  u8  eNB_id=0,UE_id=0;
  u16 Nid_cell = 0;
  u8  cooperation_flag=0, transmission_mode=1, abstraction_flag=0;
  u8 beta_ACK=0,beta_RI=0,beta_CQI=2;

  int c;
  char do_forms=0;
  unsigned int fd;
  unsigned int tcxo = 114;

  int amp;
  u8 prach_fmt;
  int N_ZC;

  char rxg_fname[100];
  char txg_fname[100];
  char rflo_fname[100];
  FILE *rxg_fd=NULL;
  FILE *txg_fd=NULL;
  FILE *rflo_fd=NULL;
  unsigned int rxg_max[4]={133,133,133,133}, rxg_med[4]={127,127,127,127}, rxg_byp[4]={120,120,120,120};
  int tx_max_power;

  char line[1000];
  int l;

#ifdef EMOS
  int error_code;
#endif

  const struct option long_options[] = {
    {"calib-ue-rx", required_argument, NULL, 256},
    {"calib-ue-rx-med", required_argument, NULL, 257},
    {"calib-ue-rx-byp", required_argument, NULL, 258},
    {"debug-ue-prach", no_argument, NULL, 259},
    {"no-L2-connect", no_argument, NULL, 260},
    {NULL, 0, NULL, 0}};

  mode = normal_txrx;


#ifdef XFORMS
  char title[255];
#endif

  while ((c = getopt_long (argc, argv, "C:ST:UdF:",long_options,NULL)) != -1)
    {
      switch (c)
        {
        case 'd':
          do_forms=1;
          break;
        case 'U':
          UE_flag = 1;
          break;
        case 'C':
          carrier_freq[0] = atoi(optarg);
          carrier_freq[1] = atoi(optarg);
          carrier_freq[2] = atoi(optarg);
          carrier_freq[3] = atoi(optarg);
          break;
        case 'S':
          fs4_test=1;
          break;
        case 'T':
          tcxo=atoi(optarg);
          break;
	case 'F':
	  sprintf(rxg_fname,"%srxg.lime",optarg);
	  rxg_fd = fopen(rxg_fname,"r");
	  if (rxg_fd) {
	    printf("Loading RX Gain parameters from %s\n",rxg_fname);
	    l=0;
	    while (fgets(line, sizeof(line), rxg_fd)) {
	      if ((strlen(line)==0) || (*line == '#')) continue; //ignore empty or comment lines
	      else {
		if (l==0) sscanf(line,"%d %d %d %d",&rxg_max[0],&rxg_max[1],&rxg_max[2],&rxg_max[3]);
		if (l==1) sscanf(line,"%d %d %d %d",&rxg_med[0],&rxg_med[1],&rxg_med[2],&rxg_med[3]);
		if (l==2) sscanf(line,"%d %d %d %d",&rxg_byp[0],&rxg_byp[1],&rxg_byp[2],&rxg_byp[3]);
		l++;
	      }
	    }
	  }
	  else 
	    printf("%s not found, running with defaults\n",rxg_fname);

	  sprintf(txg_fname,"%stxg.lime",optarg);
	  txg_fd = fopen(txg_fname,"r");
	  if (txg_fd) {
	    printf("Loading TX Gain parameters from %s\n",txg_fname);
	    l=0;
	    while (fgets(line, sizeof(line), txg_fd)) {
	      if ((strlen(line)==0) || (*line == '#')) {
		continue; //ignore empty or comment lines
	      }
	      else {
		if (l==0) sscanf(line,"%d %d %d %d",&txgain[0],&txgain[1],&txgain[2],&txgain[3]);
		if (l==1) sscanf(line,"%d",&tx_max_power);
		l++;
	      }
	    }
	  }
	  else 
	    printf("%s not found, running with defaults\n",rxg_fname);

	  sprintf(rflo_fname,"%srflo.lime",optarg);
	  rflo_fd = fopen(rflo_fname,"r");
	  if (rflo_fd) {
	    printf("Loading RF LO parameters from %s\n",rflo_fname);
	    fscanf(rflo_fd,"%d %d %d %d",&rf_local[0],&rf_local[1],&rf_local[2],&rf_local[3]);
	  }
	  else 
	    printf("%s not found, running with defaults\n",rflo_fname);


	  break;
	case 256:
	  mode = rx_calib_ue;
	  rx_input_level_dBm = atoi(optarg);
	  printf("Running with UE calibration on (LNA max), input level %d dBm\n",rx_input_level_dBm);
	  break;
	case 257:
	  mode = rx_calib_ue_med;
	  rx_input_level_dBm = atoi(optarg);
	  printf("Running with UE calibration on (LNA med), input level %d dBm\n",rx_input_level_dBm);
	  break;
	case 258:
	  mode = rx_calib_ue_byp;
	  rx_input_level_dBm = atoi(optarg);
	  printf("Running with UE calibration on (LNA byp), input level %d dBm\n",rx_input_level_dBm);
	  break;
	case 259:
	  mode = debug_prach;
	  break;
	case 260:
	  mode = no_L2_connect;
	  break;
        default:
          break;
        }
    }

  if (UE_flag==1)
    printf("configuring for UE\n");
  else
    printf("configuring for eNB\n");

  //randominit (0);
  set_taus_seed (0);

  // initialize the log (see log.h for details)
  logInit();

#ifdef NAS_NETLINK
  netlink_init();
#endif

  // to make a graceful exit when ctrl-c is pressed
  signal(SIGSEGV, signal_handler);

  // init the parameters
  frame_parms = (LTE_DL_FRAME_PARMS*) malloc(sizeof(LTE_DL_FRAME_PARMS));
  frame_parms->N_RB_DL            = 25;
  frame_parms->N_RB_UL            = 25;
  frame_parms->Ncp                = 0;
  frame_parms->Ncp_UL             = 0;
  frame_parms->Nid_cell           = Nid_cell;
  frame_parms->nushift            = 0;
  frame_parms->nb_antennas_tx_eNB = 2; //initial value overwritten by initial sync later
  frame_parms->nb_antennas_tx     = (UE_flag==0) ? 2 : 1;
  frame_parms->nb_antennas_rx     = (UE_flag==0) ? 1 : 1;
  frame_parms->mode1_flag         = (transmission_mode == 1) ? 1 : 0;
  frame_parms->frame_type         = 1;
#ifdef CBMIMO1
  if (fs4_test==1)
    frame_parms->tdd_config         = 255;
  else
#endif
  frame_parms->tdd_config         = 3;
  frame_parms->tdd_config_S       = 0;
  frame_parms->phich_config_common.phich_resource = oneSixth;
  frame_parms->phich_config_common.phich_duration = normal;
  frame_parms->pusch_config_common.ul_ReferenceSignalsPUSCH.cyclicShift = 0;//n_DMRS1 set to 0

  // hardware specific parameters
  // for CBMIMO
  frame_parms->dual_tx            = 0;
  frame_parms->freq_idx           = 1;

  init_frame_parms(frame_parms,1);

  phy_init_top(frame_parms);
  phy_init_lte_top(frame_parms);

  //init prach for openair1 test
  frame_parms->prach_config_common.rootSequenceIndex=1; 
  frame_parms->prach_config_common.prach_ConfigInfo.prach_ConfigIndex=0; 
  frame_parms->prach_config_common.prach_ConfigInfo.zeroCorrelationZoneConfig=1;
  frame_parms->prach_config_common.prach_ConfigInfo.highSpeedFlag=0;
  frame_parms->prach_config_common.prach_ConfigInfo.prach_FreqOffset=0;
  prach_fmt = get_prach_fmt(frame_parms->prach_config_common.prach_ConfigInfo.prach_ConfigIndex,
			    frame_parms->frame_type);
  N_ZC = (prach_fmt <4)?839:139;

  if (UE_flag==1) {
#ifdef OPENAIR2
    g_log->log_component[PHY].level = LOG_INFO;
#else
    g_log->log_component[PHY].level = LOG_INFO;
#endif
    g_log->log_component[PHY].flag  = LOG_HIGH;
    g_log->log_component[MAC].level = LOG_INFO;
    g_log->log_component[MAC].flag  = LOG_HIGH;
    g_log->log_component[RLC].level = LOG_INFO;
    g_log->log_component[RLC].flag  = LOG_HIGH;
    g_log->log_component[PDCP].level = LOG_INFO;
    g_log->log_component[PDCP].flag  = LOG_HIGH;
    g_log->log_component[OTG].level = LOG_INFO;
    g_log->log_component[OTG].flag  = LOG_HIGH;

    frame_parms->node_id = NODE;
    PHY_vars_UE_g = malloc(sizeof(PHY_VARS_UE*));
    PHY_vars_UE_g[0] = init_lte_UE(frame_parms, UE_id,abstraction_flag,transmission_mode);
    
    for (i=0;i<NUMBER_OF_eNB_MAX;i++) {
      PHY_vars_UE_g[0]->pusch_config_dedicated[i].betaOffset_ACK_Index = beta_ACK;
      PHY_vars_UE_g[0]->pusch_config_dedicated[i].betaOffset_RI_Index  = beta_RI;
      PHY_vars_UE_g[0]->pusch_config_dedicated[i].betaOffset_CQI_Index = beta_CQI;
    }
    
    compute_prach_seq(&PHY_vars_UE_g[0]->lte_frame_parms.prach_config_common,
		      PHY_vars_UE_g[0]->lte_frame_parms.frame_type,
		      PHY_vars_UE_g[0]->X_u);

    PHY_vars_UE_g[0]->lte_ue_pdcch_vars[0]->crnti = 0x1234;
#ifndef OPENAIR2
    PHY_vars_UE_g[0]->lte_ue_pdcch_vars[0]->crnti = 0x1235;
    //PHY_vars_UE_g[0]->lte_frame_parms.
#endif
    NB_UE_INST=1;
    NB_INST=1;
    
    openair_daq_vars.manual_timing_advance = 0;
    //openair_daq_vars.timing_advance = TIMING_ADVANCE_HW;
    openair_daq_vars.rx_gain_mode = DAQ_AGC_ON;
    openair_daq_vars.use_ia_receiver = 0;

    // if AGC is off, the following values will be used
    for (i=0;i<4;i++) 
      rxgain[i]=30;

    for (i=0;i<4;i++) {
      PHY_vars_UE_g[0]->rx_gain_max[i] = rxg_max[i];
      PHY_vars_UE_g[0]->rx_gain_med[i] = rxg_med[i];
      PHY_vars_UE_g[0]->rx_gain_byp[i] = rxg_byp[i];
    }
  
    if ((mode == normal_txrx) || (mode == rx_calib_ue) || (mode == no_L2_connect) || (mode == debug_prach)) {
      for (i=0; i<4; i++) {
	PHY_vars_UE_g[0]->rx_gain_mode[i]  = max_gain;
	frame_parms->rfmode[i] = rf_mode_max[i];
      }
      PHY_vars_UE_g[0]->rx_total_gain_dB =  PHY_vars_UE_g[0]->rx_gain_max[0];
      }
    else if ((mode == rx_calib_ue_med)) {
      for (i=0; i<4; i++) {
	PHY_vars_UE_g[0]->rx_gain_mode[i] = med_gain;
	frame_parms->rfmode[i] = rf_mode_med[i];
      }
      PHY_vars_UE_g[0]->rx_total_gain_dB =  PHY_vars_UE_g[0]->rx_gain_med[0];
    }
    else if ((mode == rx_calib_ue_byp)) {
      for (i=0; i<4; i++) {
	PHY_vars_UE_g[0]->rx_gain_mode[i] = byp_gain;
	frame_parms->rfmode[i] = rf_mode_byp[i];
      }
      PHY_vars_UE_g[0]->rx_total_gain_dB =  PHY_vars_UE_g[0]->rx_gain_byp[0];
    }

    PHY_vars_UE_g[0]->tx_power_max_dBm = tx_max_power;

  }
  else {
#ifdef OPENAIR2
    g_log->log_component[PHY].level = LOG_INFO;
#else
    g_log->log_component[PHY].level = LOG_INFO;
#endif
    g_log->log_component[PHY].flag  = LOG_HIGH;
    g_log->log_component[MAC].level = LOG_INFO;
    g_log->log_component[MAC].flag  = LOG_HIGH;
    g_log->log_component[RLC].level = LOG_INFO;
    g_log->log_component[RLC].flag  = LOG_HIGH;
    g_log->log_component[PDCP].level = LOG_INFO;
    g_log->log_component[PDCP].flag  = LOG_HIGH;
    g_log->log_component[OTG].level = LOG_INFO;
    g_log->log_component[OTG].flag  = LOG_HIGH;


    frame_parms->node_id = PRIMARY_CH;
    PHY_vars_eNB_g = malloc(sizeof(PHY_VARS_eNB*));
    PHY_vars_eNB_g[0] = init_lte_eNB(frame_parms,eNB_id,Nid_cell,cooperation_flag,transmission_mode,abstraction_flag);
    
    for (i=0;i<NUMBER_OF_UE_MAX;i++) {
      PHY_vars_eNB_g[0]->pusch_config_dedicated[i].betaOffset_ACK_Index = beta_ACK;
      PHY_vars_eNB_g[0]->pusch_config_dedicated[i].betaOffset_RI_Index  = beta_RI;
      PHY_vars_eNB_g[0]->pusch_config_dedicated[i].betaOffset_CQI_Index = beta_CQI;
    }

    compute_prach_seq(&PHY_vars_eNB_g[0]->lte_frame_parms.prach_config_common,
		      PHY_vars_eNB_g[0]->lte_frame_parms.frame_type,
		      PHY_vars_eNB_g[0]->X_u);
    
    NB_eNB_INST=1;
    NB_INST=1;

    openair_daq_vars.ue_dl_rb_alloc=0x1fff;
    openair_daq_vars.target_ue_dl_mcs=16;
    openair_daq_vars.ue_ul_nb_rb=10;
    openair_daq_vars.target_ue_ul_mcs=4;

    // if AGC is off, the following values will be used
    for (i=0;i<4;i++) 
      rxgain[i]=30;

    // set eNB to max gain
    PHY_vars_eNB_g[0]->rx_total_gain_eNB_dB =  rxg_max[0]; //was measured at rxgain=30;
    for (i=0; i<4; i++) {
      frame_parms->rfmode[i] = rf_mode_max[i];
    }


  }
 
  // for Express MIMO
  for (i=0; i<4; i++) {
    frame_parms->carrier_freq[i] = carrier_freq[i];
    frame_parms->carrier_freqtx[i] = carrier_freq[i];
    frame_parms->rxgain[i]       = rxgain[i];
    frame_parms->txgain[i]       = txgain[i];
    //frame_parms->rf_mode is set above (individually for UE and eNB)
    frame_parms->rflocal[i]      = rf_local[i];
    frame_parms->rfvcolocal[i]   = rf_vcocal[i];
    frame_parms->rxdc[i]         = rf_rxdc[i];
  }

  dump_frame_parms(frame_parms);
  
  mac_xface = malloc(sizeof(MAC_xface));
  
#ifdef OPENAIR2
  int eMBMS_active=0;
  l2_init(frame_parms,eMBMS_active);
  if (UE_flag == 1)
    mac_xface->dl_phy_sync_success (0, 0, 0, 1);
  else
    mac_xface->mrbch_phy_sync_failure (0, 0, 0);
#endif


  mac_xface->macphy_exit = &exit_fun;

#ifdef OPENAIR2
  //if (otg_enabled) {
    init_all_otg(0);
    g_otg->seed = 0;
    init_seeds(g_otg->seed);
    g_otg->num_nodes = 2;
    for (i=0; i<g_otg->num_nodes; i++){
      for (j=0; j<g_otg->num_nodes; j++){ 
	g_otg->application_idx[i][j] = 1;
	//g_otg->packet_gen_type=SUBSTRACT_STRING;
	g_otg->aggregation_level[i][j][0]=1;
	g_otg->application_type[i][j][0] = BCBR; //MCBR, BCBR
      }
    }
    init_predef_traffic();
    //}
#endif

  // start up the hardware
  openair_fd=setup_oai_hw(frame_parms);

  number_of_cards = 1;

  // connect the TX/RX buffers
  if (UE_flag==1)
    {
      setup_ue_buffers(PHY_vars_UE_g[0],frame_parms,0);
#ifndef CBMIMO1
      for (i=0; i<frame_parms->samples_per_tti*10; i++)
        for (aa=0; aa<frame_parms->nb_antennas_tx; aa++)
          PHY_vars_UE_g[0]->lte_ue_common_vars.txdata[aa][i] = 0x00010001;
#endif 
    }
  else
    {
      setup_eNB_buffers(PHY_vars_eNB_g[0],frame_parms);
      if (fs4_test==0)
        {
#ifndef CBMIMO1
          printf("Setting eNB buffer to all-RX\n");
          // Set LSBs for antenna switch (ExpressMIMO)
          for (i=0; i<frame_parms->samples_per_tti*10; i++)
            for (aa=0; aa<frame_parms->nb_antennas_tx; aa++)
              PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa][i] = 0x00010001;

	  // Set the last OFDM symbol of subframe 4 to TX to allow enough time for switch to settle
	  // (that's ok since the last symbol can be configured as SRS)
	  /*
          for (i=frame_parms->samples_per_tti*5-0*(frame_parms->ofdm_symbol_size+frame_parms->nb_prefix_samples); 
	       i<frame_parms->samples_per_tti*5; i++)
            for (aa=0; aa<frame_parms->nb_antennas_tx; aa++)
              PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa][i] = 0x0; 
	  */

#endif
        }
      else
        {
          printf("Setting eNB buffer to fs/4 test signal\n");
          for (j=0; j<PHY_vars_eNB_g[0]->lte_frame_parms.samples_per_tti*10; j+=4)
            for (aa=0; aa<frame_parms->nb_antennas_tx; aa++)
              {
#ifdef CBMIMO1
                amp = 0x80;
                ((char*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j+1] = 0;
                ((char*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j+3] = amp-1;
                ((char*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j+5] = 0;
                ((char*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j+7] = amp;
                ((char*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j] = amp-1;
                ((char*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j+2] = 0;
                ((char*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j+4] = amp;
                ((char*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j+6] = 0;
#else
                amp = 0x8000;
                ((short*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j+1] = 0;
                ((short*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j+3] = amp-1;
                ((short*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j+5] = 0;
                ((short*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j+7] = amp;
                ((short*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j] = amp-1;
                ((short*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j+2] = 0;
                ((short*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j+4] = amp;
                ((short*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j+6] = 0;
#endif
              }
        }
    }

#ifdef EMOS
  error_code = rtf_create(CHANSOUNDER_FIFO_MINOR,CHANSOUNDER_FIFO_SIZE);
  printf("[OPENAIR][SCHED][INIT] Created EMOS FIFO %d, error code %d\n",CHANSOUNDER_FIFO_MINOR,error_code);
#endif

  // make main thread LXRT soft realtime
  task = rt_task_init_schmod(nam2num("MYTASK"), 9, 0, 0, SCHED_FIFO, 0xF);
  mlockall(MCL_CURRENT | MCL_FUTURE);

  // start realtime timer and scheduler
  //rt_set_oneshot_mode();
  rt_set_periodic_mode();
  start_rt_timer(0);

  //now = rt_get_time() + 10*PERIOD;
  //rt_task_make_periodic(task, now, PERIOD);

  printf("Init mutex\n");
  //mutex = rt_get_adr(nam2num("MUTEX"));
  mutex = rt_sem_init(nam2num("MUTEX"), 1);
  if (mutex==0)
    {
      printf("Error init mutex\n");
      exit(-1);
    }
  else
    printf("mutex=%p\n",mutex);

  /*
  cond = rt_cond_init(nam2num("CONDITION"));
  //cond = rt_get_adr(nam2num("CONDITION"));
  if (cond==0)
    printf("Error init cond\n");
  */

  // initialize the instance cnt before starting the thread
  instance_cnt_ptr_user = &instance_cnt;



  // signal the driver to set up for user-space operation
  // this will initialize the semaphore and the task pointers in the kernel
  // further we receive back the pointer to the shared instance counter which is used to signal if the thread is busy or not. This pointer needs to be mapped to user space.
  ioctl(openair_fd,openair_START_LXRT,&instance_cnt_ptr_kern);
  instance_cnt_ptr_user = (int*) (instance_cnt_ptr_kern -bigphys_top+mem_base);
  *instance_cnt_ptr_user = -1;
  printf("instance_cnt_ptr_kern %p, instance_cnt_ptr_user %p, *instance_cnt_ptr_user %d\n", (void*) instance_cnt_ptr_kern, (void*) instance_cnt_ptr_user,*instance_cnt_ptr_user);

  rt_sleep(nano2count(FRAME_PERIOD));

  ioctl(openair_fd,openair_GET_PCI_INTERFACE,&pci_interface_ptr_kern);

#ifdef CBMIMO1
  ioctl(openair_fd,openair_SET_TCXO_DAC,(void *)&tcxo);
#endif

#ifdef CBMIMO1
  pci_interface[0] = (PCI_interface_t*) (pci_interface_ptr_kern-bigphys_top+mem_base);
  printf("pci_interface_ptr_kern = %p, pci_interface = %p, tcxo_dac =%d\n", (void*) pci_interface_ptr_kern, pci_interface[0],pci_interface[0]->tcxo_dac);
#else
  exmimo_pci_interface = (exmimo_pci_interface_t*) (pci_interface_ptr_kern-bigphys_top+mem_base);
  printf("pci_interface_ptr_kern = %p, exmimo_pci_interface = %p\n", (void*) pci_interface_ptr_kern, exmimo_pci_interface);
  DAQ_MBOX = (unsigned int *)(0xc0000000+exmimo_pci_interface->rf.mbox-bigphys_top+mem_base);
#endif
  // this starts the DMA transfers
  if (UE_flag!=1)
    ioctl(openair_fd,openair_START_TX_SIG,NULL);


#ifdef XFORMS
  if (do_forms==1)
    {
      fl_initialize (&argc, argv, NULL, 0, 0);
      form_dl = create_form_lte_scope();
      form_stats = create_form_stats_form();
      if (UE_flag==1)      
	sprintf (title, "LTE DL SCOPE UE");
      else
	sprintf (title, "LTE UL SCOPE eNB");
      fl_show_form (form_dl->lte_scope, FL_PLACE_HOTSPOT, FL_FULLBORDER, title);
      fl_show_form (form_stats->stats_form, FL_PLACE_HOTSPOT, FL_FULLBORDER, "stats");
      if (UE_flag==0) {
	//fl_hide_object(form->ia_receiver_button);
	if (otg_enabled) {
	  fl_set_button(form_dl->ia_receiver_button,1);
	  fl_set_object_label(form_dl->ia_receiver_button,"DL traffic ON");
	}
	else {
	  fl_set_button(form_dl->ia_receiver_button,0);
	  fl_set_object_label(form_dl->ia_receiver_button,"DL traffic OFF");
	}
      }
      else {
	//fl_show_object(form->ia_receiver_button);
	if (openair_daq_vars.use_ia_receiver) {
	  fl_set_button(form_dl->ia_receiver_button,1);
	  fl_set_object_label(form_dl->ia_receiver_button, "IA Receiver ON");
	}
	else {
	  fl_set_button(form_dl->ia_receiver_button,0);
	  fl_set_object_label(form_dl->ia_receiver_button, "IA Receiver OFF");
	}
      }

      thread2 = pthread_create(&thread2, NULL, scope_thread, NULL);
    }
#endif

  rt_sleep(nano2count(10*FRAME_PERIOD));


  // start the main thread
  if (UE_flag == 1) {
    thread1 = rt_thread_create(UE_thread, NULL, 100000000);
#ifdef DLSCH_THREAD
    //init_rx_pdsch_threads();
    rt_sleep(nano2count(FRAME_PERIOD/10));
    init_dlsch_threads();
#endif
  }
  else {
    thread0 = rt_thread_create(eNB_thread, NULL, 100000000);
#ifdef ULSCH_THREAD
    init_ulsch_threads();
#endif
  }

#ifndef CBMIMO1
  //  sync_thread = rt_thread_create(sync_hw,NULL,10000000);
#endif

  printf("threads created\n");

  // wait for end of program
  printf("TYPE <ENTER> TO TERMINATE\n");
  getchar();

#ifdef XFORMS
  if (do_forms==1)
    {
      //pthread_join?
      fl_hide_form(form_stats->stats_form);
      fl_free_form(form_stats->stats_form);
      fl_hide_form(form_dl->lte_scope);
      fl_free_form(form_dl->lte_scope);
    }
#endif

  // stop threads
  oai_exit=1;
  rt_sleep(nano2count(FRAME_PERIOD));

  // cleanup
  if (UE_flag == 1) {
#ifdef DLSCH_THREAD
    cleanup_dlsch_threads();
    //cleanup_rx_pdsch_threads();
#endif
  }
  else {
#ifdef ULSCH_THREAD
    cleanup_ulsch_threads();
#endif
  }
  stop_rt_timer();

  fd = 0;
  ioctl(openair_fd,openair_STOP,&fd);
  munmap((void*)mem_base, BIGPHYS_NUMPAGES*4096);

#ifdef EMOS
  error_code = rtf_destroy(CHANSOUNDER_FIFO_MINOR);
  printf("[OPENAIR][SCHED][CLEANUP] EMOS FIFO closed, error_code %d\n", error_code);
#endif


  return 0;
}

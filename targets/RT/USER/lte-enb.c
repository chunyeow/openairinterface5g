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

/*! \file lte-softmodem.c
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

#include "rt_wrapper.h"
#undef MALLOC //there are two conflicting definitions, so we better make sure we don't use it at all

#include "assertions.h"

#ifdef EMOS
#include <gps.h>
#endif

#include "PHY/types.h"
#include "PHY/defs.h"
#undef MALLOC //there are two conflicting definitions, so we better make sure we don't use it at all
#undef FRAME_LENGTH_COMPLEX_SAMPLES //there are two conflicting definitions, so we better make sure we don't use it at all

#include "openair0_lib.h"
#undef FRAME_LENGTH_COMPLEX_SAMPLES //there are two conflicting definitions, so we better make sure we don't use it at all

#include "PHY/vars.h"
#include "MAC_INTERFACE/vars.h"
#include "SCHED/vars.h"
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
#include "UTIL/LOG/vcd_signal_dumper.h"
#include "enb_config.h"

#if defined(ENABLE_ITTI)
# include "intertask_interface_init.h"
# include "create_tasks.h"
# if defined(ENABLE_USE_MME)
#   include "s1ap_eNB.h"
# endif
#endif

#ifdef XFORMS
#include "PHY/TOOLS/lte_phy_scope.h"
#include "stats.h"
// current status is that every UE has a DL scope for a SINGLE eNB (eNB_id=0)
// at eNB 0, an UL scope for every UE
FD_lte_phy_scope_ue  *form_ue[NUMBER_OF_UE_MAX];
FD_lte_phy_scope_enb *form_enb[NUMBER_OF_UE_MAX];
FD_stats_form *form_stats=NULL;
char title[255];
int UE_id;
unsigned char scope_enb_num_ue = 1;
#endif //XFORMS
unsigned char UE_flag=0;

char *mme_ip;
#define FRAME_PERIOD 100000000ULL
#define DAQ_PERIOD 66667ULL

#undef MALLOC //there are two conflicting definitions, so we better make sure we don't use it at all

#ifdef RTAI
static SEM *mutex;
//static CND *cond;

static int thread0;
static int thread1;
//static int sync_thread;
#else
pthread_t thread0;
pthread_t thread1;
pthread_attr_t attr_dlsch_threads;
struct sched_param sched_param_dlsch;
#endif

pthread_t  thread2;
pthread_t  thread3;
/*
  static int instance_cnt=-1; //0 means worker is busy, -1 means its free
  int instance_cnt_ptr_kern,*instance_cnt_ptr_user;
  int pci_interface_ptr_kern;
*/
//extern unsigned int bigphys_top;
//extern unsigned int mem_base;

int card = 0;
exmimo_config_t *p_exmimo_config;
exmimo_id_t     *p_exmimo_id;
volatile unsigned int *DAQ_MBOX;

#if defined(ENABLE_ITTI)
volatile int start_eNB = 0;
#endif
volatile int oai_exit = 0;
int oai_flag = 0;

//int time_offset[4] = {-138,-138,-138,-138};
//int time_offset[4] = {-145,-145,-145,-145};
int time_offset[4] = {0,0,0,0};

uint8_t eNB_id=0;

uint32_t carrier_freq_fdd[4]= {2680e6,0,0,0};
uint32_t carrier_freq_tdd[4]= {2590e6-4000,0,0,0};
uint32_t carrier_freq[4];
static char *conf_config_file_name = NULL;

struct timing_info_t {
  //unsigned int frame, hw_slot, last_slot, next_slot;
  RTIME time_min, time_max, time_avg, time_last, time_now;
  //unsigned int mbox0, mbox1, mbox2, mbox_target;
  unsigned int n_samples;
} timing_info;

runmode_t mode;
int rx_input_level_dBm;
#ifdef XFORMS
extern int otg_enabled;
#else
int otg_enabled;
#endif
int number_of_cards = 1;

//int mbox_bounds[20] = {8,16,24,30,38,46,54,60,68,76,84,90,98,106,114,120,128,136,144, 0}; ///boundaries of slots in terms ob mbox counter rounded up to even numbers
//int mbox_bounds[10] = {15, 30, 45, 60, 75, 90, 105, 120, 135, 0}; // mbox boundaries of subframes
int mbox_bounds[10] = {14, 28, 44, 58, 74, 88, 104, 118, 134, 148}; // mbox boundaries of subframes
//int mbox_bounds[10] = {16, 30, 46, 60, 76, 90, 106, 120, 136, 0}; ///boundaries of slots in terms ob mbox counter rounded up to even numbers

int init_dlsch_threads(void);
void cleanup_dlsch_threads(void);
int32_t init_rx_pdsch_thread(void);
void cleanup_rx_pdsch_thread(void);
int init_ulsch_threads(void);
void cleanup_ulsch_threads(void);

LTE_DL_FRAME_PARMS *frame_parms;

void setup_eNB_buffers(PHY_VARS_eNB *phy_vars_eNB,
                       LTE_DL_FRAME_PARMS *frame_parms, int carrier);

void signal_handler(int sig)
{
  void *array[10];
  size_t size;

  if (sig==SIGSEGV) {
    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, 2);
    exit(-1);
  } else {
    oai_exit=1;
  }
}

void exit_fun(const char *s)
{
  if (s != NULL) {
    printf("Exiting: %s\n",s);
  }

  oai_exit=1;

#if defined(ENABLE_ITTI)
  itti_terminate_tasks (TASK_UNKNOWN);
#endif

  //rt_sleep_ns(FRAME_PERIOD);

  //exit (-1);
}

#ifdef XFORMS
void *scope_thread(void *arg)
{
  s16 prach_corr[1024], i;
  char stats_buffer[16384];
  //FILE *UE_stats, *eNB_stats;
  int len=0;

  /*
    if (UE_flag==1)
    UE_stats  = fopen("UE_stats.txt", "w");
    else
    eNB_stats = fopen("eNB_stats.txt", "w");
  */

  while (!oai_exit) {
    len = dump_eNB_stats (PHY_vars_eNB_g[0], stats_buffer, 0);
    fl_set_object_label(form_stats->stats_text, stats_buffer);
    //rewind (eNB_stats);
    //fwrite (stats_buffer, 1, len, eNB_stats);
    for(UE_id=0; UE_id<scope_enb_num_ue; UE_id++) {
      phy_scope_eNB(form_enb[UE_id],
                    PHY_vars_eNB_g[eNB_id],
                    UE_id);

    }
    //printf("doing forms\n");
    usleep(100000); // 100 ms
  }

  //fclose (UE_stats);
  //fclose (eNB_stats);

  pthread_exit((void *)arg);
}
#endif

void do_OFDM_mod(mod_sym_t **txdataF, int32_t **txdata, uint16_t next_slot,
                 LTE_DL_FRAME_PARMS *frame_parms)
{
  int aa, slot_offset, slot_offset_F;

  slot_offset_F = (next_slot)*(frame_parms->ofdm_symbol_size)*((
                    frame_parms->Ncp == EXTENDED) ? 6 : 7);
  slot_offset   = (next_slot)*(frame_parms->samples_per_tti>>1);

  for (aa = 0; aa < frame_parms->nb_antennas_tx; aa++) {
    if (frame_parms->Ncp == EXTENDED) {
      PHY_ofdm_mod(&txdataF[aa][slot_offset_F],
                   &txdata[aa][slot_offset],
                   frame_parms->log2_symbol_size,
                   6,
                   frame_parms->nb_prefix_samples,
                   frame_parms->twiddle_fft,
                   frame_parms->rev,
                   CYCLIC_PREFIX);
    } else {
      normal_prefix_mod(&txdataF[aa][slot_offset_F],
                        &txdata[aa][slot_offset],
                        7,
                        frame_parms);
    }
  }
}

int dummy_tx_buffer[3840*4] __attribute__((aligned(16)));

#if defined(ENABLE_ITTI)
void *l2l1_task(void *arg)
{
  MessageDef   *message_p = NULL;
  int           result;

  itti_set_task_real_time(TASK_L2L1);
  itti_mark_task_ready(TASK_L2L1);

  if (UE_flag == 0) {
    /* Wait for the initialize message */
    do {
      if (message_p != NULL) {
        result = itti_free (ITTI_MSG_ORIGIN_ID(message_p), message_p);
        AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
      }
      itti_receive_msg (TASK_L2L1, &message_p);

      switch (ITTI_MSG_ID(message_p)) {
        case INITIALIZE_MESSAGE:
          /* Start eNB thread */
          start_eNB = 1;
          break;

        case TERMINATE_MESSAGE:
          oai_exit=1;
          itti_exit_task ();
          break;

        default:
          LOG_E(EMU, "Received unexpected message %s\n", ITTI_MSG_NAME(message_p));
          break;
      }
    } while (ITTI_MSG_ID(message_p) != INITIALIZE_MESSAGE);
    result = itti_free (ITTI_MSG_ORIGIN_ID(message_p), message_p);
    AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
  }

  do {
    // Wait for a message
    itti_receive_msg (TASK_L2L1, &message_p);

    switch (ITTI_MSG_ID(message_p)) {
      case TERMINATE_MESSAGE:
        oai_exit=1;
        itti_exit_task ();
        break;

      case MESSAGE_TEST:
        LOG_I(EMU, "Received %s\n", ITTI_MSG_NAME(message_p));
        break;

      default:
        LOG_E(EMU, "Received unexpected message %s\n", ITTI_MSG_NAME(message_p));
        break;
    }

    result = itti_free (ITTI_MSG_ORIGIN_ID(message_p), message_p);
    AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
  } while(1);

  return NULL;
}
#endif

/* This is the main eNB thread. It gets woken up by the kernel driver using the RTAI message mechanism (rt_send and rt_receive). */
static void *eNB_thread(void *arg)
{
#ifdef RTAI
  RT_TASK *task;
  SEM *sem;
#endif
  unsigned char last_slot, next_slot;
  int subframe = 0, hw_subframe;
  int frame=0;
  int skip_first=1;
  unsigned int msg1;
  unsigned int aa,slot_offset, slot_offset_F;
  int diff;
  int delay_cnt;
  RTIME time_in, time_diff;
  RTIME period;
  int mbox_target=0,mbox_current=0;
  int i,ret;
  int tx_offset;


#if defined(ENABLE_ITTI)
  /* Wait for eNB application initialization to be complete (eNB registration to MME) */
  {
    char *indicator[] = {".  ", ".. ", "...", " ..", "  .", "   "};
    int i = 0;

    while ((!oai_exit) && (start_eNB == 0)) {
      LOG_D(HW,"Waiting for eNB application to be ready %s\r", indicator[i]);
      i = (i + 1) % (sizeof(indicator) / sizeof(indicator[0]));
      usleep(200000);
    }
    LOG_D(HW,"\n");
  }
#endif

#ifdef RTAI
  task = rt_task_init_schmod(nam2num("TASK0"), 0, 0, 0, SCHED_FIFO, 0xF);
  rt_receive(0, (unsigned long *)((void *)&sem));
  LOG_D(HW,"Started eNB thread (id %p)\n",task);
#endif

#ifdef HARD_RT
  rt_make_hard_real_time();
#endif

  mlockall(MCL_CURRENT | MCL_FUTURE);

  timing_info.time_min = 100000000ULL;
  timing_info.time_max = 0;
  timing_info.time_avg = 0;
  timing_info.n_samples = 0;


  // sync to HW subframe == 0
  mbox_current = ((volatile unsigned int *)DAQ_MBOX)[0];
  rt_sleep_ns((165-mbox_current)*DAQ_PERIOD);

  time_in = rt_get_time_ns();

  while (!oai_exit) {
    //this is the mbox counter where we should be
    mbox_target = mbox_bounds[subframe];
    //this is the mbox counter where we are
    mbox_current = ((volatile unsigned int *)DAQ_MBOX)[0];
    //this is the time we need to sleep in order to synchronize with the hw (in multiples of DAQ_PERIOD)
    if ((mbox_current>=135) && (mbox_target<15)) { //handle the frame wrap-arround
      diff = 150-mbox_current+mbox_target;
    } else if ((mbox_current<15) && (mbox_target>=135)) {
      diff = -150+mbox_target-mbox_current;
    } else {
      diff = mbox_target - mbox_current;
    }

    if (diff < -15) {
      LOG_D(HW,
            "Time %.3f: Frame %d, missed subframe, proceeding with next one (subframe %d, hw_subframe %d, mbox_currend %d, mbox_target %d,diff %d) processing time %.3f\n",
            (float)(rt_get_time_ns()-time_in)/1e6,
            frame, subframe, hw_subframe, mbox_current, mbox_target, diff,
            (float)(timing_info.time_now-timing_info.time_last)/1e6);

      subframe++;
      if (frame>0) {
        oai_exit=1;
      }
      if (subframe==10) {
        subframe=0;
        frame++;
      }
      continue;
    }
    if (diff > 15) {
      LOG_D(HW,
            "Time %.3f: Frame %d, skipped subframe, waiting for hw to catch up (subframe %d, hw_subframe %d, mbox_current %d, mbox_target %d, diff %d), processing time %.3f\n",
            (float)(rt_get_time_ns()-time_in)/1e6,
            frame, subframe, hw_subframe, mbox_current, mbox_target, diff,
            (float)(timing_info.time_now-timing_info.time_last)/1e6);
      //exit(-1);
    }

    delay_cnt = 0;
    vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_DAQ_MBOX,
                                            *((volatile unsigned int *) openair0_exmimo_pci[card].rxcnt_ptr[0]));
    vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_DIFF, diff);

    while ((diff>0) && (!oai_exit)) {
      time_diff = rt_get_time_ns() - time_in;
      ret = rt_sleep_ns(diff*DAQ_PERIOD);
      if (ret) {
        LOG_D(HW,"eNB Frame %d, time %llu: rt_sleep_ns returned %d\n",frame, time_in);
      }

      delay_cnt++;
      if (delay_cnt == 10) {
        oai_exit = 1;
        LOG_D(HW,"eNB Frame %d: HW stopped ... \n",frame);
      }
      mbox_current = ((volatile unsigned int *)DAQ_MBOX)[0];
      if ((mbox_current>=135) && (mbox_target<15)) { //handle the frame wrap-arround
        diff = 150-mbox_current+mbox_target;
      } else if ((mbox_current<15) && (mbox_target>=135)) {
        diff = -150+mbox_target-mbox_current;
      } else {
        diff = mbox_target - mbox_current;
      }
    }

    hw_subframe = ((((volatile unsigned int *)DAQ_MBOX)[0]+135)%150)/15;
    LOG_D(HW,
          "Time: %.3f: Frame %d, subframe %d, hw_subframe %d (mbox %d) processing time %0.3f\n",
          (float)(rt_get_time_ns()-time_in)/1e6,
          frame,subframe,hw_subframe,((volatile unsigned int *)DAQ_MBOX)[0],
          (float)(timing_info.time_now-timing_info.time_last)/1e6);

    last_slot = (subframe<<1)+1;
    if (last_slot <0) {
      last_slot+=20;
    }
    next_slot = ((subframe<<1)+4)%LTE_SLOTS_PER_FRAME;

    if ((skip_first == 0) || (frame>5))
    {
      skip_first = 0;
      timing_info.time_last = rt_get_time_ns();

      //msg("subframe %d, last_slot %d,next_slot %d\n", subframe,last_slot,next_slot);
      vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_DAQ_MBOX,
                                              *((volatile unsigned int *) openair0_exmimo_pci[card].rxcnt_ptr[0]));
      vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_DIFF, diff);

      phy_procedures_eNB_lte (last_slot, next_slot, PHY_vars_eNB_g[0], 0,0,NULL);
      vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_DAQ_MBOX,
                                              *((volatile unsigned int *) openair0_exmimo_pci[card].rxcnt_ptr[0]));
#ifndef IFFT_FPGA

      if ((subframe_select(&PHY_vars_eNB_g[0]->lte_frame_parms,next_slot>>1)==SF_DL)||
          ((subframe_select(&PHY_vars_eNB_g[0]->lte_frame_parms,next_slot>>1)==SF_S)
           &&((next_slot&1)==0))) {
        //LOG_D(HW,"Frame %d: Generating slot %d\n",frame,next_slot);
        /*
        do_OFDM_mod(PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdataF[0],
        PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0],
        next_slot,
        &PHY_vars_eNB_g[0]->lte_frame_parms);

        do_OFDM_mod(PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdataF[0],
        PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0],
        next_slot+1,
        &PHY_vars_eNB_g[0]->lte_frame_parms);
        */

        vcd_signal_dumper_dump_function_by_name(
          VCD_SIGNAL_DUMPER_FUNCTIONS_PHY_ENB_SFGEN,1);
        slot_offset_F = (next_slot)*
                        (PHY_vars_eNB_g[0]->lte_frame_parms.ofdm_symbol_size)*7;
        slot_offset = (next_slot)*
                      (PHY_vars_eNB_g[0]->lte_frame_parms.samples_per_tti>>1);

        normal_prefix_mod(
          &PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdataF[0][0][slot_offset_F],
          dummy_tx_buffer,
          7,
          &(PHY_vars_eNB_g[0]->lte_frame_parms));
        for (i = 0; i < PHY_vars_eNB_g[0]->lte_frame_parms.samples_per_tti/2; i++) {
          tx_offset = slot_offset + time_offset[0] + i;
          ((short *)&PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][0][tx_offset])[0]
            = ((short *)dummy_tx_buffer)[2*i]<<4;
          ((short *)&PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][0][tx_offset])[1]
            = ((short *)dummy_tx_buffer)[2*i+1]<<4;
        }

        slot_offset_F = (next_slot+1)*
                        (PHY_vars_eNB_g[0]->lte_frame_parms.ofdm_symbol_size)*7;
        slot_offset = (next_slot+1)*
                      (PHY_vars_eNB_g[0]->lte_frame_parms.samples_per_tti>>1);

        normal_prefix_mod(
          &PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdataF[0][0][slot_offset_F],
          dummy_tx_buffer,
          7,
          &(PHY_vars_eNB_g[0]->lte_frame_parms));
        for (i = 0; i < PHY_vars_eNB_g[0]->lte_frame_parms.samples_per_tti/2; i++) {
          tx_offset = slot_offset + i;
          ((short *)&PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][0][tx_offset])[0]
            = ((short *)dummy_tx_buffer)[2*i]<<4;
          ((short *)&PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][0][tx_offset])[1]
            = ((short *)dummy_tx_buffer)[2*i+1]<<4;
        }
      }
      vcd_signal_dumper_dump_function_by_name(
        VCD_SIGNAL_DUMPER_FUNCTIONS_PHY_ENB_SFGEN,0);
      timing_info.time_now = rt_get_time_ns();
#endif //IFFT_FPGA
      /*
        if (frame%100==0)
        LOG_D(HW,"hw_slot %d (after): DAQ_MBOX %d\n",hw_slot,DAQ_MBOX[0]);
      */
    }

    /*
    if ((slot%2000)<10)
    LOG_D(HW,"fun0: doing very hard work\n");
    */


    if (oai_flag == 2) {
      //dump_ulsch(PHY_vars_eNB_g[0], subframe, 0);
      //exit(-1);
      oai_exit=1;
      exit_fun("[HW][eNB] HW stopped");
    }

    if (oai_flag == 1) {
      oai_flag = 2;
    }

    subframe++;
    if (subframe==10) {
      subframe=0;
      frame++;
    }
    if(frame == 1024) {
      frame = 0;
      time_in = rt_get_time_ns();
    }
#if defined(ENABLE_ITTI)
    itti_update_lte_time(frame, subframe * 2);
#endif
  }

  LOG_D(HW,"eNB_thread: finished, ran %d times.\n",frame);

#ifdef HARD_RT
  rt_make_soft_real_time();
#endif

  // clean task
#ifdef RTAI
  rt_sem_signal(mutex);
  rt_task_delete(task);
#endif
  LOG_D(HW,"Task deleted. returning\n");
  return 0;
}

int main(int argc, char **argv)
{
  const Enb_properties_array_t *enb_properties;
#ifdef RTAI
  RT_TASK *task;
#endif
  int i,j,aa;
  void *status;

  /*
    uint32_t rf_mode_max[4]     = {55759,55759,55759,55759};
    uint32_t rf_mode_med[4]     = {39375,39375,39375,39375};
    uint32_t rf_mode_byp[4]     = {22991,22991,22991,22991};
  */
  uint32_t my_rf_mode = RXEN + TXEN + TXLPFNORM + TXLPFEN + TXLPF25 + RXLPFNORM +
                   RXLPFEN + RXLPF25 + LNA1ON +LNAMax + RFBBNORM + DMAMODE_RX + DMAMODE_TX;
  uint32_t rf_mode_base = TXLPFNORM + TXLPFEN + TXLPF25 + RXLPFNORM + RXLPFEN +
                     RXLPF25 + LNA1ON + /*LNAMax Antennas*/ LNAByp + RFBBNORM;
  uint32_t rf_mode[4]     = {my_rf_mode,0,0,0};
  uint32_t rf_local[4]    = {8255000,8255000,8255000,8255000}; // UE zepto
  //{8254617, 8254617, 8254617, 8254617}; //eNB khalifa
  //{8255067,8254810,8257340,8257340}; // eNB PETRONAS

  uint32_t rf_vcocal[4]   = {910,910,910,910};
  uint32_t rf_vcocal_850[4] = {2015, 2015, 2015, 2015};
  uint32_t rf_rxdc[4]     = {32896,32896,32896,32896};
  // Gain for antennas connection
  //uint32_t rxgain[4]      = {25,20,20,20};
  //uint32_t txgain[4]      = {30,25,25,25};

  // Gain for Cable connection
  uint32_t rxgain[4]      = {20,20,20,20};
  uint32_t txgain[4]      = {25,25,25,25};


  uint8_t frame_type = FDD;
  uint8_t tdd_config = 3;
  uint8_t tdd_config_S = 0;
  uint8_t extended_prefix_flag = 0;
  uint16_t Nid_cell = 0;
  uint8_t N_RB_DL = 25;
  uint8_t  cooperation_flag = 0;
  uint8_t transmission_mode = 1;
  uint8_t abstraction_flag = 0;
  uint8_t nb_antennas_rx = 1;

  uint8_t beta_ACK=0,beta_RI=0,beta_CQI=2;

  int c;
  char do_forms=0;
  unsigned int fd;

  int amp;
  unsigned int rxg_max[4]= {133,133,133,133}, rxg_med[4]= {127,127,127,127}, rxg_byp[4]= {120,120,120,120};
  int tx_max_power=0;

  int ret, ant;

  int error_code;
  char *itti_dump_file = NULL;

  mode = normal_txrx;

  mme_ip = "146.208.175.6";

  while ((c = getopt(argc, argv, "dC:K:O:T:N:R:Vi;")) != -1) {
    switch (c) {
      case 'd':
        do_forms=1;
        printf("Starting XFORMS ...\n");
        break;
      case 'C':
        carrier_freq[0] = atoi(optarg);
        carrier_freq[1] = atoi(optarg);
        carrier_freq[2] = atoi(optarg);
        carrier_freq[3] = atoi(optarg);
        break;
      case 'T':
        frame_type = TDD; // default FDD
        tdd_config = atoi(optarg);
        break;
      case 'K':
#if defined(ENABLE_ITTI)
        itti_dump_file = strdup(optarg);
#else
        printf("-K option is disabled when ENABLE_ITTI is not defined\n");
#endif
        break;
      case 'O':
        conf_config_file_name = optarg;
        break;
      case 'R':
        N_RB_DL = atoi(optarg);
        if ((N_RB_DL != 6) && (N_RB_DL != 15) && (N_RB_DL != 25) &&
            (N_RB_DL != 50) && (N_RB_DL != 75) && (N_RB_DL != 100)) {
          printf("Illegal N_RB_DL %d (should be one of 6,15,25,50,75,100)\n", N_RB_DL);
          exit(-1);
        }
        break;
      case 'i':
        mme_ip = optarg;
        break;
      case 'V':
        ouput_vcd = 1;
        break;
      default:
        break;
    }
  }

  NB_eNB_INST=1;
  NB_INST=1;

  if ((UE_flag == 0) && (conf_config_file_name != NULL)) {
    int i;

    /* Read eNB configuration file */
    enb_properties = enb_config_init(conf_config_file_name);

    AssertFatal (NB_eNB_INST <= enb_properties->number,
                 "Number of eNB is greater than eNB defined in configuration file %s (%d/%d)!",
                 conf_config_file_name, NB_eNB_INST, enb_properties->number);

    /* Update some simulation parameters */
    frame_type =        enb_properties->properties[0]->frame_type;
    for (i = 0 ; i < (sizeof(carrier_freq) / sizeof (carrier_freq[0])); i++) {
      carrier_freq[i] = enb_properties->properties[0]->downlink_frequency;
    }
  }

  set_taus_seed (0);

  // initialize the log (see log.h for details)
  logInit();

  if (ouput_vcd) {
    vcd_signal_dumper_init("/tmp/openair_dump_eNB.vcd");
  }

#if defined(ENABLE_ITTI)
  log_set_instance_type (LOG_INSTANCE_ENB);

  itti_init(TASK_MAX, THREAD_MAX, MESSAGES_ID_MAX, tasks_info, messages_info,
            messages_definition_xml, itti_dump_file);
#endif

#ifdef NAS_NETLINK
  netlink_init();
#endif

#if !defined(ENABLE_ITTI)
  // to make a graceful exit when ctrl-c is pressed
  signal(SIGSEGV, signal_handler);
  signal(SIGINT, signal_handler);
#endif

#ifndef RTAI
  check_clock();
#endif

  init_lte_vars(&frame_parms, frame_type, tdd_config, tdd_config_S,
                extended_prefix_flag, N_RB_DL,
                Nid_cell, cooperation_flag, transmission_mode, abstraction_flag,
                nb_antennas_rx,0);

  g_log->level = LOG_WARNING;
  g_log->log_component[HW].level = LOG_WARNING;
  g_log->log_component[HW].flag  = LOG_LOW;
  g_log->log_component[PHY].level = LOG_WARNING;
  g_log->log_component[PHY].flag  = LOG_LOW;
  g_log->log_component[MAC].level = LOG_INFO;
  g_log->log_component[MAC].flag  = LOG_LOW;
  g_log->log_component[RLC].level = LOG_WARNING;
  g_log->log_component[RLC].flag  = LOG_LOW;
  g_log->log_component[PDCP].level = LOG_WARNING;
  g_log->log_component[PDCP].flag  = LOG_LOW;
  g_log->log_component[RRC].level = LOG_INFO;
  g_log->log_component[RRC].flag  = LOG_LOW;
  g_log->log_component[OIP].level = LOG_INFO;
  g_log->log_component[OIP].flag = LOG_HIGH;
#if defined(ENABLE_ITTI)
  g_log->log_component[EMU].level = LOG_INFO;
  g_log->log_component[EMU].flag  = LOG_HIGH;
# if defined(ENABLE_USE_MME)
  g_log->log_component[S1AP].level  = LOG_INFO;
  g_log->log_component[S1AP].flag   = LOG_HIGH;
  g_log->log_component[SCTP].level  = LOG_INFO;
  g_log->log_component[SCTP].flag   = LOG_HIGH;
# endif
  g_log->log_component[S1AP].level = LOG_INFO;
  g_log->log_component[S1AP].flag  = LOG_LOW;
#else
#endif
  g_log->log_component[ENB_APP].level = LOG_INFO;
  g_log->log_component[ENB_APP].flag  = LOG_HIGH;

  PHY_vars_eNB_g = malloc(sizeof(PHY_VARS_eNB *));
  PHY_vars_eNB_g[0] = init_lte_eNB(frame_parms,eNB_id,Nid_cell,cooperation_flag,
                                   transmission_mode,abstraction_flag);

#ifndef OPENAIR2
  for (i=0; i<NUMBER_OF_UE_MAX; i++) {
    PHY_vars_eNB_g[0]->pusch_config_dedicated[i].betaOffset_ACK_Index = beta_ACK;
    PHY_vars_eNB_g[0]->pusch_config_dedicated[i].betaOffset_RI_Index  = beta_RI;
    PHY_vars_eNB_g[0]->pusch_config_dedicated[i].betaOffset_CQI_Index = beta_CQI;

    PHY_vars_eNB_g[0]->scheduling_request_config[i].sr_PUCCH_ResourceIndex = i;
    PHY_vars_eNB_g[0]->scheduling_request_config[i].sr_ConfigIndex = 7+(i%3);
    PHY_vars_eNB_g[0]->scheduling_request_config[i].dsr_TransMax = sr_n4;
  }
#endif

  openair_daq_vars.ue_dl_rb_alloc=0x1fff;
  openair_daq_vars.target_ue_dl_mcs=16;
  openair_daq_vars.ue_ul_nb_rb=6;
  openair_daq_vars.target_ue_ul_mcs=9;

  // set eNB to max gain
  PHY_vars_eNB_g[0]->rx_total_gain_eNB_dB =  rxg_max[0] + rxgain[0] -
      30; //was measured at rxgain=30;
  for (i=0; i<4; i++) {
    //        frame_parms->rfmode[i] = rf_mode_max[i];
    rf_mode[i] = (rf_mode[i] & (~LNAGAINMASK)) | LNAMax;
  }

  // Initialize card
  ret = openair0_open();
  if ( ret != 0 ) {
    if (ret == -1) {
      printf("Error opening /dev/openair0\n");
    }
    if (ret == -2) {
      printf("Error mapping bigshm\n");
    }
    if (ret == -3) {
      printf("Error mapping RX or TX buffer\n");
    }
    return(ret);
  }

  printf ("Detected %d number of cards, %d number of antennas.\n",
          openair0_num_detected_cards, openair0_num_antennas[card]);

  p_exmimo_config = openair0_exmimo_pci[card].exmimo_config_ptr;
  p_exmimo_id     = openair0_exmimo_pci[card].exmimo_id_ptr;

  printf("Card %d: ExpressMIMO %d, HW Rev %d, SW Rev 0x%d\n", card,
         p_exmimo_id->board_exmimoversion, p_exmimo_id->board_hwrev,
         p_exmimo_id->board_swrev);

  if (p_exmimo_id->board_swrev>=BOARD_SWREV_CNTL2) {
    p_exmimo_config->framing.eNB_flag   = 0;
  } else {
    p_exmimo_config->framing.eNB_flag   = 0;  //!UE_flag;
  }
  p_exmimo_config->framing.tdd_config = DUPLEXMODE_FDD + TXRXSWITCH_LSB;
  for (ant = 0; ant < 4; ant++) {
    p_exmimo_config->framing.resampling_factor[ant] = 2;
  }

  for (ant=0; ant<max(frame_parms->nb_antennas_tx,frame_parms->nb_antennas_rx);
       ant++) {
    p_exmimo_config->rf.rf_mode[ant] = rf_mode_base;
  }
  for (ant=0; ant<frame_parms->nb_antennas_tx; ant++) {
    p_exmimo_config->rf.rf_mode[ant] += (TXEN + DMAMODE_TX);
  }
  for (ant=0; ant<frame_parms->nb_antennas_rx; ant++) {
    p_exmimo_config->rf.rf_mode[ant] += (RXEN + DMAMODE_RX);
  }
  for (ant=max(frame_parms->nb_antennas_tx,frame_parms->nb_antennas_rx); ant<4;
       ant++) {
    p_exmimo_config->rf.rf_mode[ant] = 0;
    carrier_freq[ant] = 0; //this turns off all other LIMEs
  }

  for (ant = 0; ant < 4; ant++) {
    if (frame_type == FDD) {
      carrier_freq[ant] = carrier_freq_fdd[ant];
    } else {
      carrier_freq[ant] = carrier_freq_tdd[ant];
    }
  }
  for (ant = 0; ant<1; ant++) {
    p_exmimo_config->rf.do_autocal[ant] = 1;
    if (frame_type == FDD) {
      p_exmimo_config->rf.rf_freq_rx[ant] =
        carrier_freq[ant]-120e6;  // LTE FDD band 1 duplex space
    } else {
      p_exmimo_config->rf.rf_freq_rx[ant] = carrier_freq[ant];
    }

    p_exmimo_config->rf.rf_freq_tx[ant] = carrier_freq[ant];
    p_exmimo_config->rf.rx_gain[ant][0] = rxgain[ant];
    p_exmimo_config->rf.tx_gain[ant][0] = txgain[ant];

    p_exmimo_config->rf.rf_local[ant]   = rf_local[ant];
    p_exmimo_config->rf.rf_rxdc[ant]    = rf_rxdc[ant];

    if ((carrier_freq[ant] >= 850000000) && (carrier_freq[ant] <= 865000000)) {
      p_exmimo_config->rf.rf_vcocal[ant]  = rf_vcocal_850[ant];
      p_exmimo_config->rf.rffe_band_mode[ant] = DD_TDD;
    } else {
      p_exmimo_config->rf.rf_vcocal[ant]  = rf_vcocal[ant];
      p_exmimo_config->rf.rffe_band_mode[ant] = B19G_TDD;
    }

    p_exmimo_config->rf.rffe_gain_txlow[ant] = 63;
    p_exmimo_config->rf.rffe_gain_txhigh[ant] = 63;
    p_exmimo_config->rf.rffe_gain_rxfinal[ant] = 63;
    p_exmimo_config->rf.rffe_gain_rxlow[ant] = 63;
  }


  dump_frame_parms(frame_parms);

  mac_xface = malloc(sizeof(MAC_xface));

#ifdef OPENAIR2
  int eMBMS_active=0;
  l2_init(frame_parms,eMBMS_active,
          0,// cba_group_active
          0); //HO active
  mac_xface->mrbch_phy_sync_failure (0, 0, 0);
#endif

  mac_xface->macphy_exit = &exit_fun;

#if defined(ENABLE_ITTI)
  if (create_tasks(UE_flag ? 0 : 1, UE_flag ? 1 : 0) < 0) {
    exit(-1); // need a softer mode
  }
  printf("ITTI tasks created\n");
#endif

  number_of_cards = openair0_num_detected_cards;
  if (p_exmimo_id->board_exmimoversion==1) { //ExpressMIMO1
    openair_daq_vars.timing_advance = 138;
  } else { //ExpressMIMO2
    openair_daq_vars.timing_advance = 15;
  }

  setup_eNB_buffers(PHY_vars_eNB_g[0],frame_parms,0);

  openair0_dump_config(card);

  printf("EXMIMO_CONFIG: rf_mode 0x %x %x %x %x, [0]: TXRXEn %d, TXLPFEn %d, TXLPF %d, RXLPFEn %d, RXLPF %d, RFBB %d, LNA %d, LNAGain %d, RXLPFMode %d, SWITCH %d, rf_rxdc %d, rf_local %d, rf_vcocal %d\n",

         p_exmimo_config->rf.rf_mode[0],
         p_exmimo_config->rf.rf_mode[1],
         p_exmimo_config->rf.rf_mode[2],
         p_exmimo_config->rf.rf_mode[3],
         (p_exmimo_config->rf.rf_mode[0]&3),  // RXen+TXen
         (p_exmimo_config->rf.rf_mode[0]&4)>>2,         //TXLPFen
         (p_exmimo_config->rf.rf_mode[0]&TXLPFMASK)>>3, //TXLPF
         (p_exmimo_config->rf.rf_mode[0]&128)>>7,      //RXLPFen
         (p_exmimo_config->rf.rf_mode[0]&RXLPFMASK)>>8, //TXLPF
         (p_exmimo_config->rf.rf_mode[0]&RFBBMASK)>>16, // RFBB mode
         (p_exmimo_config->rf.rf_mode[0]&LNAMASK)>>12, // RFBB mode
         (p_exmimo_config->rf.rf_mode[0]&LNAGAINMASK)>>14, // RFBB mode
         (p_exmimo_config->rf.rf_mode[0]&RXLPFMODEMASK)>>19, // RXLPF mode
         (p_exmimo_config->framing.tdd_config&TXRXSWITCH_MASK)>>1, // Switch mode
         p_exmimo_config->rf.rf_rxdc[0],
         p_exmimo_config->rf.rf_local[0],
         p_exmimo_config->rf.rf_vcocal[0]);

  for (ant=0; ant<4; ant++) {
    p_exmimo_config->rf.do_autocal[ant] = 0;
  }


  mlockall(MCL_CURRENT | MCL_FUTURE);

#ifdef RTAI
  // make main thread LXRT soft realtime
  task = rt_task_init_schmod(nam2num("MYTASK"), 9, 0, 0, SCHED_FIFO, 0xF);

  // start realtime timer and scheduler
  //rt_set_oneshot_mode();
  rt_set_periodic_mode();
  start_rt_timer(0);

  //now = rt_get_time() + 10*PERIOD;
  //rt_task_make_periodic(task, now, PERIOD);

  printf("Init mutex\n");
  //mutex = rt_get_adr(nam2num("MUTEX"));
  mutex = rt_sem_init(nam2num("MUTEX"), 0);
  if (mutex==0) {
    printf("Error init mutex\n");
    exit(-1);
  } else {
    printf("mutex=%p\n",mutex);
  }
#endif

  DAQ_MBOX = (volatile unsigned int *) openair0_exmimo_pci[card].rxcnt_ptr[0];

  // this starts the DMA transfers
  openair0_start_rt_acquisition(card);


#ifdef XFORMS
  if (do_forms==1) {
    printf("Running XFORMS\n");
    fl_initialize (&argc, argv, NULL, 0, 0);
    form_stats = create_form_stats_form();
    for(UE_id=0; UE_id<scope_enb_num_ue; UE_id++) {
      form_enb[UE_id] = create_lte_phy_scope_enb();
      sprintf (title, "UE%d LTE UL SCOPE eNB",UE_id+1);
      fl_show_form (form_enb[UE_id]->lte_phy_scope_enb, FL_PLACE_HOTSPOT,
                    FL_FULLBORDER, title);
    }
    fl_show_form (form_stats->stats_form, FL_PLACE_HOTSPOT, FL_FULLBORDER, "stats");
    for (UE_id=0; UE_id<scope_enb_num_ue; UE_id++) {
      if (otg_enabled) {
        fl_set_button(form_enb[UE_id]->button_0,1);
        fl_set_object_label(form_enb[UE_id]->button_0,"DL Traffic ON");
      } else {
        fl_set_button(form_enb[UE_id]->button_0,0);
        fl_set_object_label(form_enb[UE_id]->button_0,"DL Traffic OFF");
      }
    }

    ret = pthread_create(&thread2, NULL, scope_thread, NULL);
    printf("Scope thread created, ret=%d\n",ret);
  }
#endif

  rt_sleep_ns(10*FRAME_PERIOD);

#ifndef RTAI
  pthread_attr_init (&attr_dlsch_threads);
  pthread_attr_setstacksize(&attr_dlsch_threads,OPENAIR_THREAD_STACK_SIZE);
  //attr_dlsch_threads.priority = 1;
  sched_param_dlsch.sched_priority = sched_get_priority_max(
                                       SCHED_FIFO); //OPENAIR_THREAD_PRIORITY;
  pthread_attr_setschedparam  (&attr_dlsch_threads, &sched_param_dlsch);
  pthread_attr_setschedpolicy (&attr_dlsch_threads, SCHED_FIFO);
#endif

  // start the main thread
#ifdef RTAI
  thread0 = rt_thread_create(eNB_thread, NULL, 10000000);
  rt_sleep_ns(FRAME_PERIOD*10);
  rt_send(rt_get_adr(nam2num("TASK0")), (unsigned long)mutex);
#else
  error_code = pthread_create(&thread0, &attr_dlsch_threads, eNB_thread, NULL);
  if (error_code!= 0) {
    LOG_D(HW,"[lte-softmodem.c] Could not allocate eNB_thread, error %d\n",
          error_code);
    return(error_code);
  } else {
    LOG_D(HW,"[lte-softmodem.c] Allocate eNB_thread successful\n");
  }
#endif

// #ifdef OPENAIR2
//   init_pdcp_thread(1);
// #endif

#ifdef ULSCH_THREAD
  init_ulsch_threads();
#endif
  printf("eNB threads created\n");


  // wait for end of program
  printf("TYPE <CTRL-C> TO TERMINATE\n");
  //getchar();
  //while (oai_exit==0)
  //  rt_sleep_ns(FRAME_PERIOD);

#if defined(ENABLE_ITTI)
  printf("Entering ITTI signals handler\n");
  itti_wait_tasks_end();
#else
  rt_sem_wait(mutex);
#endif

  // stop threads
#ifdef XFORMS
  printf("waiting for XFORMS thread\n");
  if (do_forms==1) {
    pthread_join(thread2,&status);
    fl_hide_form(form_stats->stats_form);
    fl_free_form(form_stats->stats_form);
    for(UE_id=0; UE_id<scope_enb_num_ue; UE_id++) {
      fl_hide_form(form_enb[UE_id]->lte_phy_scope_enb);
      fl_free_form(form_enb[UE_id]->lte_phy_scope_enb);
    }
  }
#endif

  printf("stopping MODEM threads\n");
  // cleanup
#ifdef RTAI
  rt_thread_join(thread0);
#else
  pthread_join(thread0,&status);
#endif
#ifdef ULSCH_THREAD
  cleanup_ulsch_threads();
#endif

#ifdef RTAI
  rt_sem_delete(mutex);
  stop_rt_timer();
  rt_task_delete(task);
#endif

  printf("stopping card\n");
  openair0_stop(card);
  printf("closing openair0_lib\n");
  openair0_close();



  if (ouput_vcd) {
    vcd_signal_dumper_close();
  }

  logClean();

  return 0;
}

void setup_eNB_buffers(PHY_VARS_eNB *phy_vars_eNB,
                       LTE_DL_FRAME_PARMS *frame_parms, int carrier)
{

  int i,j;
  uint16_t N_TA_offset = 0;

  if (frame_parms->frame_type == TDD) {
    N_TA_offset = 624/4;
  }

  if (phy_vars_eNB) {
    if ((frame_parms->nb_antennas_rx>1) && (carrier>0)) {
      printf("RX antennas > 1 and carrier > 0 not possible\n");
      exit(-1);
    }

    if ((frame_parms->nb_antennas_tx>1) && (carrier>0)) {
      printf("TX antennas > 1 and carrier > 0 not possible\n");
      exit(-1);
    }

    // replace RX signal buffers with mmaped HW versions
    for (i=0; i<frame_parms->nb_antennas_rx; i++) {
      free(phy_vars_eNB->lte_eNB_common_vars.rxdata[0][i]);
      phy_vars_eNB->lte_eNB_common_vars.rxdata[0][i] = ((int32_t *)
          openair0_exmimo_pci[card].adc_head[i+carrier]) -
          N_TA_offset; // N_TA offset for TDD

      /*
        printf("rxdata[%d] @ %p\n",i,phy_vars_eNB->lte_eNB_common_vars.rxdata[0][i]);
        for (j=0;j<16;j++) {
      printf("rxbuffer %d: %x\n",j,phy_vars_eNB->lte_eNB_common_vars.rxdata[0][i][j]);
      phy_vars_eNB->lte_eNB_common_vars.rxdata[0][i][j] = 16-j;
        }
      */
    }
    for (i=0; i<frame_parms->nb_antennas_tx; i++) {
      free(phy_vars_eNB->lte_eNB_common_vars.txdata[0][i]);
      phy_vars_eNB->lte_eNB_common_vars.txdata[0][i] = (int32_t *)
          openair0_exmimo_pci[card].dac_head[i+carrier];

      /*
        printf("txdata[%d] @ %p\n",i,phy_vars_eNB->lte_eNB_common_vars.txdata[0][i]);
        for (j=0;j<16;j++) {
      printf("txbuffer %d: %x\n",j,phy_vars_eNB->lte_eNB_common_vars.txdata[0][i][j]);
      phy_vars_eNB->lte_eNB_common_vars.txdata[0][i][j] = 16-j;
      }
      */
    }
  }
}

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
 * \author R. Knopp, F. Kaltenberger, Navid Nikaein
 * \date 2012
 * \version 0.1
 * \company Eurecom
 * \email: knopp@eurecom.fr,florian.kaltenberger@eurecom.fr, navid.nikaein@eurecom.fr
 * \note
 * \warning
 */
#define _GNU_SOURCE
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

#ifndef EXMIMO
static int hw_subframe;
#endif

#include "assertions.h"

#ifdef EMOS
#include <gps.h>
#endif

#include "PHY/types.h"

#include "PHY/defs.h"
#undef MALLOC //there are two conflicting definitions, so we better make sure we don't use it at all
//#undef FRAME_LENGTH_COMPLEX_SAMPLES //there are two conflicting definitions, so we better make sure we don't use it at all

#ifdef EXMIMO
#include "openair0_lib.h"
#else
#include "../../ARCH/COMMON/common_lib.h"
#endif

//#undef FRAME_LENGTH_COMPLEX_SAMPLES //there are two conflicting definitions, so we better make sure we don't use it at all

#include "PHY/vars.h"
#include "MAC_INTERFACE/vars.h"
//#include "SCHED/defs.h"
#include "SCHED/vars.h"
#include "LAYER2/MAC/vars.h"

#include "../../SIMU/USER/init_lte.h"

#ifdef EMOS
#include "SCHED/phy_procedures_emos.h"
#endif

#ifdef OPENAIR2
#include "otg_tx.h"
#include "LAYER2/MAC/defs.h"
#include "LAYER2/MAC/vars.h"
#include "LAYER2/MAC/proto.h"
#ifndef CELLULAR
#include "RRC/LITE/vars.h"
#endif
#include "PHY_INTERFACE/vars.h"
#endif

#ifdef SMBV
#include "PHY/TOOLS/smbv.h"
unsigned short config_frames[4] = {2,9,11,13};
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
#endif

#define FRAME_PERIOD    100000000ULL
#define DAQ_PERIOD      66667ULL

#define DEBUG_THREADS 1

//#define USRP_DEBUG 1

struct timing_info_t {
  //unsigned int frame, hw_slot, last_slot, next_slot;
  RTIME time_min, time_max, time_avg, time_last, time_now;
  //unsigned int mbox0, mbox1, mbox2, mbox_target;
  unsigned int n_samples;
} timing_info;

extern int16_t* sync_corr_ue0;
extern int16_t prach_ifft[4][1024*2];

int init_dlsch_threads(void);
void cleanup_dlsch_threads(void);
int32_t init_rx_pdsch_thread(void);
void cleanup_rx_pdsch_thread(void);

openair0_config_t openair0_cfg[MAX_CARDS];

int32_t *rxdata;
int32_t *txdata;
int setup_ue_buffers(PHY_VARS_UE **phy_vars_ue, openair0_config_t *openair0_cfg, openair0_rf_map rf_map[MAX_NUM_CCs]);
int setup_eNB_buffers(PHY_VARS_eNB **phy_vars_eNB, openair0_config_t *openair0_cfg, openair0_rf_map rf_map[MAX_NUM_CCs]);

#ifdef XFORMS
// current status is that every UE has a DL scope for a SINGLE eNB (eNB_id=0)
// at eNB 0, an UL scope for every UE 
FD_lte_phy_scope_ue  *form_ue[NUMBER_OF_UE_MAX];
FD_lte_phy_scope_enb *form_enb[NUMBER_OF_UE_MAX];
FD_stats_form                  *form_stats=NULL,*form_stats_l2=NULL;
char title[255];
unsigned char                   scope_enb_num_ue = 1;
#endif //XFORMS

#ifdef RTAI
static SEM                     *mutex;
//static CND *cond;

static long                      main_eNB_thread;
static long                      main_ue_thread;
#ifdef USRP
static SEM *sync_sem; // to sync rx & tx streaming
#endif
//static int sync_thread;
#else
pthread_t                       main_eNB_thread;
pthread_t                       main_ue_thread;
pthread_attr_t                  attr_dlsch_threads;
struct sched_param              sched_param_dlsch;

#ifndef EXMIMO
pthread_cond_t sync_cond;
pthread_mutex_t sync_mutex;

#endif
#endif
RTIME T0;

pthread_attr_t                  attr_UE_init_synch;
pthread_attr_t                  attr_UE_thread_tx;
pthread_attr_t                  attr_UE_thread_rx;
pthread_attr_t                  attr_eNB_proc_tx[MAX_NUM_CCs][10];
pthread_attr_t                  attr_eNB_proc_rx[MAX_NUM_CCs][10];
struct sched_param              sched_param_UE_init_synch;
struct sched_param              sched_param_UE_thread_tx;
struct sched_param              sched_param_UE_thread_rx;
struct sched_param              sched_param_eNB_proc_tx[MAX_NUM_CCs][10];
struct sched_param              sched_param_eNB_proc_rx[MAX_NUM_CCs][10];

#ifdef XFORMS
static pthread_t                forms_thread; //xforms
#endif
#ifdef EMOS
static pthread_t                thread3; //emos
#endif

#ifdef SPECTRA
static pthread_t sensing_thread;
#endif

openair0_device openair0;
openair0_timestamp timestamp;

/*
  static int instance_cnt=-1; //0 means worker is busy, -1 means its free
  int instance_cnt_ptr_kern,*instance_cnt_ptr_user;
  int pci_interface_ptr_kern;
*/
//extern unsigned int bigphys_top;
//extern unsigned int mem_base;

int                             card = 0;


#if defined(ENABLE_ITTI)
static volatile int             start_eNB = 0;
static volatile int             start_UE = 0;
#endif
volatile int                    oai_exit = 0;

//static int                      time_offset[4] = {-138,-138,-138,-138};
//static int                      time_offset[4] = {-145,-145,-145,-145};
static int                      time_offset[4] = {0,0,0,0};


static char                     UE_flag=0;
static uint8_t                  eNB_id=0,UE_id=0;

//uint32_t                        carrier_freq[MAX_NUM_CCs][4] =           {{1907600000,1907600000,1907600000,1907600000}}; /* For UE! */
static uint32_t                 downlink_frequency[MAX_NUM_CCs][4] =     {{1907600000,1907600000,1907600000,1907600000},
									  {1907600000,1907600000,1907600000,1907600000}};
static int32_t                  uplink_frequency_offset[MAX_NUM_CCs][4]= {{0,0,0,0},{0,0,0,0}};

openair0_rf_map rf_map[MAX_NUM_CCs];

static char                    *conf_config_file_name = NULL;
#if defined(ENABLE_ITTI)
static char                    *itti_dump_file = NULL;
#endif

#ifndef USRP
double tx_gain[MAX_NUM_CCs][4] = {{20,20,0,0},{20,20,0,0}};
double rx_gain[MAX_NUM_CCs][4] = {{20,20,0,0},{20,20,0,0}};
// these are for EXMIMO2 target only
/*
static unsigned int             rxg_max[4] =    {133,133,133,133};
static unsigned int             rxg_med[4] =    {127,127,127,127};
static unsigned int             rxg_byp[4] =    {120,120,120,120};
*/
// these are for EXMIMO2 card 39
static unsigned int             rxg_max[4] =    {128,128,128,126};
static unsigned int             rxg_med[4] =    {122,123,123,120};
static unsigned int             rxg_byp[4] =    {116,117,116,116};
static unsigned int             nf_max[4] =    {7,9,16,12};
static unsigned int             nf_med[4] =    {12,13,22,17};
static unsigned int             nf_byp[4] =    {15,20,29,23};
static rx_gain_t                rx_gain_mode[MAX_NUM_CCs][4] = {{max_gain,max_gain,max_gain,max_gain},{max_gain,max_gain,max_gain,max_gain}};
#else
double tx_gain[MAX_NUM_CCs][4] = {{120,0,0,0}};
double rx_gain[MAX_NUM_CCs][4] = {{125,0,0,0}};
#endif

double sample_rate=30.72e6;
double bw = 14e6;

static int                      tx_max_power[MAX_NUM_CCs] =  {0,0};

#ifndef EXMIMO
char ref[128] = "internal";
char channels[128] = "0";

unsigned int samples_per_frame = 307200;
unsigned int samples_per_packets = 2048; // samples got every recv or send
unsigned int tx_forward_nsamps;

int sf_bounds_5[10]    = {8, 15, 23, 30, 38, 45, 53, 60, 68, 75};
int sf_bounds_5_tx[10] = {4, 11, 19, 26, 34, 41, 49, 56, 64, 71};

int sf_bounds_10[10] = {8, 15, 23, 30, 38, 45, 53, 60, 68, 75};
int sf_bounds_10_tx[10] = {4, 11, 19, 26, 34, 41, 49, 56, 64, 71};

int sf_bounds_20[10] = {15, 30, 45, 60, 75, 90, 105, 120, 135, 150};
int sf_bounds_20_tx[10] = {7, 22, 37, 52, 67, 82, 97, 112, 127, 142};

int *sf_bounds;
int *sf_bounds_tx;

int max_cnt;
int tx_delay;

#endif
/*
  uint32_t rf_mode_max[4]     = {55759,55759,55759,55759};
  uint32_t rf_mode_med[4]     = {39375,39375,39375,39375};
  uint32_t rf_mode_byp[4]     = {22991,22991,22991,22991};
*/
//static uint32_t                      rf_mode[4] =        {MY_RF_MODE,0,0,0};
//static uint32_t                      rf_local[4] =       {8255000,8255000,8255000,8255000}; // UE zepto
//{8254617, 8254617, 8254617, 8254617}; //eNB khalifa
//{8255067,8254810,8257340,8257340}; // eNB PETRONAS

//static uint32_t                      rf_vcocal[4] =      {910,910,910,910};
//static uint32_t                      rf_vcocal_850[4] =  {2015, 2015, 2015, 2015};
//static uint32_t                      rf_rxdc[4] =        {32896,32896,32896,32896};
//static uint32_t                      rxgain[4] =         {20,20,20,20};
//static uint32_t                      txgain[4] =         {20,20,20,20};

static runmode_t                mode;
static int                      rx_input_level_dBm;
static int                      online_log_messages=0;
#ifdef XFORMS
extern int                      otg_enabled;
static char                     do_forms=0;
#else
int                             otg_enabled;
#endif
//int                             number_of_cards =   1;
#ifndef USRP
static int                      mbox_bounds[20] =   {8,16,24,30,38,46,54,60,68,76,84,90,98,106,114,120,128,136,144, 0}; ///boundaries of slots in terms ob mbox counter rounded up to even numbers
//static int                      mbox_bounds[20] =   {6,14,22,28,36,44,52,58,66,74,82,88,96,104,112,118,126,134,142, 148}; ///boundaries of slots in terms ob mbox counter rounded up to even numbers
#endif

static LTE_DL_FRAME_PARMS      *frame_parms[MAX_NUM_CCs];

int multi_thread=1;
uint32_t target_dl_mcs = 28; //maximum allowed mcs
uint32_t target_ul_mcs = 10;


int16_t           glog_level=LOG_DEBUG;
int16_t           glog_verbosity=LOG_MED;
int16_t           hw_log_level=LOG_DEBUG;
int16_t           hw_log_verbosity=LOG_MED;
int16_t           phy_log_level=LOG_DEBUG;
int16_t           phy_log_verbosity=LOG_MED;
int16_t           mac_log_level=LOG_DEBUG;
int16_t           mac_log_verbosity=LOG_MED;
int16_t           rlc_log_level=LOG_DEBUG;
int16_t           rlc_log_verbosity=LOG_MED;
int16_t           pdcp_log_level=LOG_DEBUG;
int16_t           pdcp_log_verbosity=LOG_MED;
int16_t           rrc_log_level=LOG_DEBUG;
int16_t           rrc_log_verbosity=LOG_MED;

unsigned int build_rflocal(int txi, int txq, int rxi, int rxq)
{
  return (txi + (txq<<6) + (rxi<<12) + (rxq<<18));
}
unsigned int build_rfdc(int dcoff_i_rxfe, int dcoff_q_rxfe)
{
  return (dcoff_i_rxfe + (dcoff_q_rxfe<<8));
}

#if !defined(ENABLE_ITTI)
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
  }
  else {
    printf("trying to exit gracefully...\n"); 
    oai_exit = 1;
  }
}
#endif

void exit_fun(const char* s)
{
  if (s != NULL) {
    printf("%s %s() Exiting: %s\n",__FILE__, __FUNCTION__, s);
  }

  oai_exit = 1;

#if defined(ENABLE_ITTI)
  itti_terminate_tasks (TASK_UNKNOWN);
#endif

  //rt_sleep_ns(FRAME_PERIOD);

  //exit (-1);
}

static int latency_target_fd = -1;
static int32_t latency_target_value = 0;
/* Latency trick - taken from cyclictest.c 
 * if the file /dev/cpu_dma_latency exists,
 * open it and write a zero into it. This will tell
 * the power management system not to transition to
 * a high cstate (in fact, the system acts like idle=poll)
 * When the fd to /dev/cpu_dma_latency is closed, the behavior
 * goes back to the system default.
 *
 * Documentation/power/pm_qos_interface.txt
 */
static void set_latency_target(void)
{
  struct stat s;
  int ret;
  if (stat("/dev/cpu_dma_latency", &s) == 0) {
    latency_target_fd = open("/dev/cpu_dma_latency", O_RDWR);
    if (latency_target_fd == -1)
      return;
    ret = write(latency_target_fd, &latency_target_value, 4);
    if (ret == 0) {
      printf("# error setting cpu_dma_latency to %d!: %s\n", latency_target_value, strerror(errno));
      close(latency_target_fd);
      return;
    }
    printf("# /dev/cpu_dma_latency set to %dus\n", latency_target_value);
  }
}
 
#ifdef XFORMS

void reset_stats(FL_OBJECT *button, long arg) {
  int i,j,k;
  PHY_VARS_eNB *phy_vars_eNB = PHY_vars_eNB_g[0][0];
  for (i=0;i<NUMBER_OF_UE_MAX;i++) {
    for (k=0;k<8;k++) {//harq_processes
      for (j=0;j<phy_vars_eNB->dlsch_eNB[i][0]->Mdlharq;j++) {
	phy_vars_eNB->eNB_UE_stats[i].dlsch_NAK[k][j]=0;
	phy_vars_eNB->eNB_UE_stats[i].dlsch_ACK[k][j]=0;
	phy_vars_eNB->eNB_UE_stats[i].dlsch_trials[k][j]=0;
      }
      phy_vars_eNB->eNB_UE_stats[i].dlsch_l2_errors[k]=0;
      phy_vars_eNB->eNB_UE_stats[i].ulsch_errors[k]=0;
      phy_vars_eNB->eNB_UE_stats[i].ulsch_consecutive_errors=0;
      for (j=0;j<phy_vars_eNB->ulsch_eNB[i]->Mdlharq;j++) {
	phy_vars_eNB->eNB_UE_stats[i].ulsch_decoding_attempts[k][j]=0;
      phy_vars_eNB->eNB_UE_stats[i].ulsch_decoding_attempts_last[k][j]=0;
      phy_vars_eNB->eNB_UE_stats[i].ulsch_round_errors[k][j]=0;
      phy_vars_eNB->eNB_UE_stats[i].ulsch_round_fer[k][j]=0;
      }
    }
    phy_vars_eNB->eNB_UE_stats[i].dlsch_sliding_cnt=0;
    phy_vars_eNB->eNB_UE_stats[i].dlsch_NAK_round0=0;
    phy_vars_eNB->eNB_UE_stats[i].dlsch_mcs_offset=0;
  }
}

static void *scope_thread(void *arg) {
  char stats_buffer[16384];
# ifdef ENABLE_XFORMS_WRITE_STATS
  FILE *UE_stats, *eNB_stats;
  int len = 0;
# endif
  struct sched_param sched_param;

  sched_param.sched_priority = sched_get_priority_min(SCHED_FIFO)+1; 
  sched_setscheduler(0, SCHED_FIFO,&sched_param);

  printf("Scope thread has priority %d\n",sched_param.sched_priority);
    
# ifdef ENABLE_XFORMS_WRITE_STATS
  if (UE_flag==1) 
    UE_stats  = fopen("UE_stats.txt", "w");
  else 
    eNB_stats = fopen("eNB_stats.txt", "w");
#endif
    
  while (!oai_exit) {
    if (UE_flag==1) {
# ifdef ENABLE_XFORMS_WRITE_STATS
      len =
# endif
	dump_ue_stats (PHY_vars_UE_g[0][0], stats_buffer, 0, mode,rx_input_level_dBm);
      fl_set_object_label(form_stats->stats_text, stats_buffer);

      phy_scope_UE(form_ue[UE_id], 
		   PHY_vars_UE_g[UE_id][0],
		   eNB_id,
		   UE_id,7);
            
    } else {
# ifdef ENABLE_XFORMS_WRITE_STATS
      len =
# endif
      dump_eNB_l2_stats (stats_buffer, 0);
      fl_set_object_label(form_stats_l2->stats_text, stats_buffer);

# ifdef ENABLE_XFORMS_WRITE_STATS
      len =
# endif
	dump_eNB_stats (PHY_vars_eNB_g[0][0], stats_buffer, 0);
      fl_set_object_label(form_stats->stats_text, stats_buffer);

      for(UE_id=0;UE_id<scope_enb_num_ue;UE_id++) {
	phy_scope_eNB(form_enb[UE_id], 
		      PHY_vars_eNB_g[eNB_id][0],
		      UE_id);
      }
              
    }
    //printf("doing forms\n");
    //usleep(100000); // 100 ms
    sleep(1);
  }

  printf("%s",stats_buffer);
    
# ifdef ENABLE_XFORMS_WRITE_STATS
  if (UE_flag==1) {
    if (UE_stats) {
      rewind (UE_stats);
      fwrite (stats_buffer, 1, len, UE_stats);
      fclose (UE_stats);
    }
  }
  else {
    if (eNB_stats) {
      rewind (eNB_stats);
      fwrite (stats_buffer, 1, len, eNB_stats);
      fclose (eNB_stats);
    }
  }
# endif
    
  pthread_exit((void*)arg);
}
#endif

#ifdef EMOS
#define NO_ESTIMATES_DISK 100 //No. of estimates that are aquired before dumped to disk

void *emos_thread (void *arg)
{
  char c;
  char *fifo2file_buffer, *fifo2file_ptr;

  int fifo, counter=0, bytes;

  FILE  *dumpfile_id;
  char  dumpfile_name[1024];
  time_t starttime_tmp;
  struct tm starttime;
  
  int channel_buffer_size;
  
  time_t timer;
  struct tm *now;

  struct gps_data_t *gps_data = NULL;
  struct gps_fix_t dummy_gps_data;

  struct sched_param sched_param;
  
  sched_param.sched_priority = sched_get_priority_max(SCHED_FIFO)-1; 
  sched_setscheduler(0, SCHED_FIFO,&sched_param);
  
  printf("EMOS thread has priority %d\n",sched_param.sched_priority);
 
  timer = time(NULL);
  now = localtime(&timer);

  memset(&dummy_gps_data,1,sizeof(struct gps_fix_t));
  
  gps_data = gps_open("127.0.0.1","2947");
  if (gps_data == NULL) 
    {
      printf("[EMOS] Could not open GPS\n");
      //exit(-1);
    }
#if GPSD_API_MAJOR_VERSION>=4
  else if (gps_stream(gps_data, WATCH_ENABLE,NULL) != 0)
#else
  else if (gps_query(gps_data, "w+x") != 0)
#endif
    {
      //sprintf(tmptxt,"Error sending command to GPS, gps_data = %x", gps_data);
      printf("[EMOS] Error sending command to GPS\n");
      //exit(-1);
    }
  else 
    printf("[EMOS] Opened GPS, gps_data=%p\n");
  
  if (UE_flag==0)
    channel_buffer_size = sizeof(fifo_dump_emos_eNB);
  else
    channel_buffer_size = sizeof(fifo_dump_emos_UE);

  // allocate memory for NO_FRAMES_DISK channes estimations
  fifo2file_buffer = malloc(NO_ESTIMATES_DISK*channel_buffer_size);
  fifo2file_ptr = fifo2file_buffer;

  if (fifo2file_buffer == NULL)
    {
      printf("[EMOS] Cound not allocate memory for fifo2file_buffer\n");
      exit(EXIT_FAILURE);
    }

  if ((fifo = open(CHANSOUNDER_FIFO_DEV, O_RDONLY)) < 0)
    {
      fprintf(stderr, "[EMOS] Error opening the fifo\n");
      exit(EXIT_FAILURE);
    }


  time(&starttime_tmp);
  localtime_r(&starttime_tmp,&starttime);
  snprintf(dumpfile_name,1024,"/tmp/%s_data_%d%02d%02d_%02d%02d%02d.EMOS",
	   (UE_flag==0) ? "eNB" : "UE",
	   1900+starttime.tm_year, starttime.tm_mon+1, starttime.tm_mday, starttime.tm_hour, starttime.tm_min, starttime.tm_sec);

  dumpfile_id = fopen(dumpfile_name,"w");
  if (dumpfile_id == NULL)
    {
      fprintf(stderr, "[EMOS] Error opening dumpfile %s\n",dumpfile_name);
      exit(EXIT_FAILURE);
    }


  printf("[EMOS] starting dump, channel_buffer_size=%d ...\n",channel_buffer_size);
  while (!oai_exit)
    {
      bytes = rtf_read_timed(fifo, fifo2file_ptr, channel_buffer_size,100);
      if (bytes==0)
	continue;

      /*
	if (UE_flag==0)
	printf("eNB: count %d, frame %d, read: %d bytes from the fifo\n",counter, ((fifo_dump_emos_eNB*)fifo2file_ptr)->frame_tx,bytes);
	else
	printf("UE: count %d, frame %d, read: %d bytes from the fifo\n",counter, ((fifo_dump_emos_UE*)fifo2file_ptr)->frame_rx,bytes);
      */

      fifo2file_ptr += channel_buffer_size;
      counter ++;

      if (counter == NO_ESTIMATES_DISK)
        {
          //reset stuff
          fifo2file_ptr = fifo2file_buffer;
          counter = 0;

          //flush buffer to disk
	  if (UE_flag==0)
	    printf("[EMOS] eNB: count %d, frame %d, flushing buffer to disk\n",
		   counter, ((fifo_dump_emos_eNB*)fifo2file_ptr)->frame_tx);
	  else
	    printf("[EMOS] UE: count %d, frame %d, flushing buffer to disk\n",
		   counter, ((fifo_dump_emos_UE*)fifo2file_ptr)->frame_rx);


          if (fwrite(fifo2file_buffer, sizeof(char), NO_ESTIMATES_DISK*channel_buffer_size, dumpfile_id) != NO_ESTIMATES_DISK*channel_buffer_size)
            {
              fprintf(stderr, "[EMOS] Error writing to dumpfile\n");
              exit(EXIT_FAILURE);
            }
	  if (gps_data)
	    {
	      if (gps_poll(gps_data) != 0) {
		printf("[EMOS] problem polling data from gps\n");
	      }
	      else {
		printf("[EMOS] lat %g, lon %g\n",gps_data->fix.latitude,gps_data->fix.longitude);
	      }
	      if (fwrite(&(gps_data->fix), sizeof(char), sizeof(struct gps_fix_t), dumpfile_id) != sizeof(struct gps_fix_t))
		{
		  printf("[EMOS] Error writing to dumpfile, stopping recording\n");
		  exit(EXIT_FAILURE);
		}
	    }
	  else
	    {
	      printf("[EMOS] WARNING: No GPS data available, storing dummy packet\n");
	      if (fwrite(&(dummy_gps_data), sizeof(char), sizeof(struct gps_fix_t), dumpfile_id) != sizeof(struct gps_fix_t))
		{
		  printf("[EMOS] Error writing to dumpfile, stopping recording\n");
		  exit(EXIT_FAILURE);
		}
	    } 
        }
    }
  
  free(fifo2file_buffer);
  fclose(dumpfile_id);
  close(fifo);
  
  pthread_exit((void*) arg);

}
#endif

#ifdef SPECTRA
void *sensing (void *arg)
{
  struct sched_param sched_param;
  
  sched_param.sched_priority = sched_get_priority_max(SCHED_FIFO)-1; 
  sched_setscheduler(0, SCHED_FIFO,&sched_param);
  
  printf("[SPECTRA] sensing thread started with priority %d\n",sched_param.sched_priority);
 
  while (oai_exit==0) {

    
    openair0_cfg[0].rx_freq[2]+= 5e6;
    if (openair0_cfg[0].rx_freq[2] >= 750000000)
      openair0_cfg[0].rx_freq[2] = 727500000;
    

    //LOG_I(HW,"[SPECTRA] changing frequency to %u \n",(uint32_t)openair0_cfg[1].rx_freq[0]);

    openair0_reconfig(&openair0_cfg[0]);

    usleep(200000);
    //sleep(1);
    
  }

  pthread_exit((void*) arg);

}
#endif


#if defined(ENABLE_ITTI)
static void wait_system_ready (char *message, volatile int *start_flag)
{
  /* Wait for eNB application initialization to be complete (eNB registration to MME) */
  {
    static char *indicator[] = {".    ", "..   ", "...  ", ".... ", ".....",
                                " ....", "  ...", "   ..", "    .", "     "};
    int i = 0;

    while ((!oai_exit) && (*start_flag == 0)) {
      LOG_N(EMU, message, indicator[i]);
      i = (i + 1) % (sizeof(indicator) / sizeof(indicator[0]));
      usleep(200000);
    }
    LOG_D(EMU,"\n");
  }
}
#endif

#if defined(ENABLE_ITTI)
void *l2l1_task(void *arg)
{
  MessageDef *message_p = NULL;
  int         result;

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
	LOG_D(EMU, "L2L1 TASK received %s\n", ITTI_MSG_NAME(message_p));
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

    case ACTIVATE_MESSAGE:
      start_UE = 1;
      break;

    case DEACTIVATE_MESSAGE:
      start_UE = 0;
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


void do_OFDM_mod_rt(int subframe,PHY_VARS_eNB *phy_vars_eNB) {

  unsigned int aa,slot_offset, slot_offset_F;
  int dummy_tx_b[7680*4] __attribute__((aligned(16)));
  int i, tx_offset;
  int slot_sizeF = (phy_vars_eNB->lte_frame_parms.ofdm_symbol_size)*
    ((phy_vars_eNB->lte_frame_parms.Ncp==1) ? 6 : 7);

  slot_offset_F = (subframe<<1)*slot_sizeF;
    
  slot_offset = subframe*phy_vars_eNB->lte_frame_parms.samples_per_tti;

  if ((subframe_select(&phy_vars_eNB->lte_frame_parms,subframe)==SF_DL)||
      ((subframe_select(&phy_vars_eNB->lte_frame_parms,subframe)==SF_S))) {
    //	  LOG_D(HW,"Frame %d: Generating slot %d\n",frame,next_slot);

    
    for (aa=0; aa<phy_vars_eNB->lte_frame_parms.nb_antennas_tx; aa++) {
      if (phy_vars_eNB->lte_frame_parms.Ncp == EXTENDED){ 
	PHY_ofdm_mod(&phy_vars_eNB->lte_eNB_common_vars.txdataF[0][aa][slot_offset_F],
		     dummy_tx_b,
		     phy_vars_eNB->lte_frame_parms.log2_symbol_size,
		     6,
		     phy_vars_eNB->lte_frame_parms.nb_prefix_samples,
		     phy_vars_eNB->lte_frame_parms.twiddle_ifft,
		     phy_vars_eNB->lte_frame_parms.rev,
		     CYCLIC_PREFIX);
	PHY_ofdm_mod(&phy_vars_eNB->lte_eNB_common_vars.txdataF[0][aa][slot_offset_F+slot_sizeF],
		     dummy_tx_b+(phy_vars_eNB->lte_frame_parms.samples_per_tti>>1),
		     phy_vars_eNB->lte_frame_parms.log2_symbol_size,
		     6,
		     phy_vars_eNB->lte_frame_parms.nb_prefix_samples,
		     phy_vars_eNB->lte_frame_parms.twiddle_ifft,
		     phy_vars_eNB->lte_frame_parms.rev,
		     CYCLIC_PREFIX);
      }
      else {
	normal_prefix_mod(&phy_vars_eNB->lte_eNB_common_vars.txdataF[0][aa][slot_offset_F],
			  dummy_tx_b,
			  7,
			  &(phy_vars_eNB->lte_frame_parms));
	normal_prefix_mod(&phy_vars_eNB->lte_eNB_common_vars.txdataF[0][aa][slot_offset_F+slot_sizeF],
			  dummy_tx_b+(phy_vars_eNB->lte_frame_parms.samples_per_tti>>1),
			  7,
			  &(phy_vars_eNB->lte_frame_parms));
      }

      for (i=0; i<phy_vars_eNB->lte_frame_parms.samples_per_tti; i++) {
	tx_offset = (int)slot_offset+time_offset[aa]+i;
	if (tx_offset<0)
	  tx_offset += LTE_NUMBER_OF_SUBFRAMES_PER_FRAME*phy_vars_eNB->lte_frame_parms.samples_per_tti;
	if (tx_offset>=(LTE_NUMBER_OF_SUBFRAMES_PER_FRAME*phy_vars_eNB->lte_frame_parms.samples_per_tti))
	  tx_offset -= LTE_NUMBER_OF_SUBFRAMES_PER_FRAME*phy_vars_eNB->lte_frame_parms.samples_per_tti;
	((short*)&phy_vars_eNB->lte_eNB_common_vars.txdata[0][aa][tx_offset])[0]=
	  ((short*)dummy_tx_b)[2*i]<<4;
	((short*)&phy_vars_eNB->lte_eNB_common_vars.txdata[0][aa][tx_offset])[1]=
	  ((short*)dummy_tx_b)[2*i+1]<<4;
      }
    }
  }
}


int eNB_thread_tx_status[10];
static void * eNB_thread_tx(void *param) {

  //unsigned long cpuid;
  eNB_proc_t *proc = (eNB_proc_t*)param;
  //  RTIME time_in,time_out;
#ifdef RTAI
  RT_TASK *task;
  char task_name[8];
#endif

 

#if defined(ENABLE_ITTI)
  /* Wait for eNB application initialization to be complete (eNB registration to MME) */
  wait_system_ready ("Waiting for eNB application to be ready %s\r", &start_eNB);
#endif

#ifdef RTAI
  sprintf(task_name,"TXC%dS%d",proc->CC_id,proc->subframe);
  task = rt_task_init_schmod(nam2num(task_name), 0, 0, 0, SCHED_FIFO, 0xF);

  if (task==NULL) {
    LOG_E(PHY,"[SCHED][eNB] Problem starting eNB_proc_TX thread_index %d (%s)!!!!\n",proc->subframe,task_name);
    return 0;
  }
  else {
    LOG_I(PHY,"[SCHED][eNB] eNB TX thread CC %d SF %d started with id %p\n",
	  proc->CC_id,
	  proc->subframe,
	  task);
  }
#else
  //  LOG_I(PHY,
  printf("[SCHED][eNB] eNB TX thread %d started on CPU %d\n",
	 proc->subframe,sched_getcpu());
#endif

  mlockall(MCL_CURRENT | MCL_FUTURE);

  //rt_set_runnable_on_cpuid(task,1);
  //cpuid = rtai_cpuid();

#ifdef HARD_RT
  rt_make_hard_real_time();
#endif

  while (!oai_exit){
    
    vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_eNB_PROC_TX0+(2*proc->subframe),0);
    
    
    //LOG_I(PHY,"Locking mutex for eNB proc %d (IC %d,mutex %p)\n",proc->subframe,proc->instance_cnt,&proc->mutex);
    //    printf("Locking mutex for eNB proc %d (subframe_tx %d))\n",proc->subframe,proc->subframe_tx);

    if (pthread_mutex_lock(&proc->mutex_tx) != 0) {
      LOG_E(PHY,"[SCHED][eNB] error locking mutex for eNB TX proc %d\n",proc->subframe);
      oai_exit=1;
    }
    else {
      
      while (proc->instance_cnt_tx < 0) {
	//	LOG_I(PHY,"Waiting and unlocking mutex for eNB proc %d (IC %d,lock %d)\n",proc->subframe,proc->instance_cnt,pthread_mutex_trylock(&proc->mutex));
	//printf("Waiting and unlocking mutex for eNB proc %d (subframe_tx %d)\n",proc->subframe,subframe_tx);
	
	pthread_cond_wait(&proc->cond_tx,&proc->mutex_tx);
      }
      //      LOG_I(PHY,"Waking up and unlocking mutex for eNB proc %d\n",proc->subframe);
      if (pthread_mutex_unlock(&proc->mutex_tx) != 0) {	
	LOG_E(PHY,"[SCHED][eNB] error unlocking mutex for eNB TX proc %d\n",proc->subframe);
	oai_exit=1;
      }
    }
    vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_eNB_PROC_TX0+(2*proc->subframe),1);    
    vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_FRAME_NUMBER_TX_ENB, proc->frame_tx);
    
    if (oai_exit) break;

    
    if ((((PHY_vars_eNB_g[0][proc->CC_id]->lte_frame_parms.frame_type == TDD)&&
	  (subframe_select(&PHY_vars_eNB_g[0][proc->CC_id]->lte_frame_parms,proc->subframe_tx)==SF_DL))||
	 (PHY_vars_eNB_g[0][proc->CC_id]->lte_frame_parms.frame_type == FDD))) {
      
      phy_procedures_eNB_TX(proc->subframe,PHY_vars_eNB_g[0][proc->CC_id],0,no_relay,NULL);
      
    }
    if ((subframe_select(&PHY_vars_eNB_g[0][proc->CC_id]->lte_frame_parms,proc->subframe_tx)==SF_S)) {
      phy_procedures_eNB_TX(proc->subframe,PHY_vars_eNB_g[0][proc->CC_id],0,no_relay,NULL);
    }
    
    
    do_OFDM_mod_rt(proc->subframe_tx,PHY_vars_eNB_g[0][proc->CC_id]);  
    
    if (pthread_mutex_lock(&proc->mutex_tx) != 0) {
      printf("[openair][SCHED][eNB] error locking mutex for eNB TX proc %d\n",proc->subframe);
    }
    else {
      proc->instance_cnt_tx--;
      
      if (pthread_mutex_unlock(&proc->mutex_tx) != 0) {	
	printf("[openair][SCHED][eNB] error unlocking mutex for eNB TX proc %d\n",proc->subframe);
      }
    }

    
    proc->frame_tx++;
    if (proc->frame_tx==1024)
      proc->frame_tx=0;

  }    
  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_eNB_PROC_TX0+(2*proc->subframe),0);        
#ifdef HARD_RT
  rt_make_soft_real_time();
#endif
  
#ifdef DEBUG_THREADS
  printf("Exiting eNB thread TX %d\n",proc->subframe);
#endif
  // clean task
#ifdef RTAI
  rt_task_delete(task);
#else
  eNB_thread_tx_status[proc->subframe]=0;
  pthread_exit(&eNB_thread_tx_status[proc->subframe]);
#endif
  
#ifdef DEBUG_THREADS
  printf("Exiting eNB TX thread %d\n",proc->subframe);
#endif
}

int eNB_thread_rx_status[10];
static void * eNB_thread_rx(void *param) {

  //unsigned long cpuid;
  eNB_proc_t *proc = (eNB_proc_t*)param;
  //  RTIME time_in,time_out;
#ifdef RTAI
  RT_TASK *task;
  char task_name[8];
#endif

#if defined(ENABLE_ITTI)
  /* Wait for eNB application initialization to be complete (eNB registration to MME) */
  wait_system_ready ("Waiting for eNB application to be ready %s\r", &start_eNB);
#endif

#ifdef RTAI
  sprintf(task_name,"RXC%1dS%1d",proc->CC_id,proc->subframe);
  task = rt_task_init_schmod(nam2num(task_name), 0, 0, 0, SCHED_FIFO, 0xF);

  if (task==NULL) {
    LOG_E(PHY,"[SCHED][eNB] Problem starting eNB_proc_RX thread_index %d (%s)!!!!\n",proc->subframe,task_name);
    return 0;
  }
  else {
    LOG_I(PHY,"[SCHED][eNB] eNB RX thread CC_id %d SF %d started with id %p\n", /*  on CPU %d*/
	  proc->CC_id,
	  proc->subframe,
	  task); /*,rtai_cpuid()*/
  }
#else
  LOG_I(PHY,"[SCHED][eNB] eNB RX thread %d started on CPU %d\n",
	proc->subframe,sched_getcpu());
#endif

  mlockall(MCL_CURRENT | MCL_FUTURE);

  //rt_set_runnable_on_cpuid(task,1);
  //cpuid = rtai_cpuid();

#ifdef HARD_RT
  rt_make_hard_real_time();
#endif

  while (!oai_exit){

    vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_eNB_PROC_RX0+(2*proc->subframe),0);
    
    //    LOG_I(PHY,"Locking mutex for eNB proc %d (IC %d,mutex %p)\n",proc->subframe,proc->instance_cnt,&proc->mutex);
    if (pthread_mutex_lock(&proc->mutex_rx) != 0) {
      LOG_E(PHY,"[SCHED][eNB] error locking mutex for eNB RX proc %d\n",proc->subframe);
    }
    else {
        
      while (proc->instance_cnt_rx < 0) {
	//	LOG_I(PHY,"Waiting and unlocking mutex for eNB proc %d (IC %d,lock %d)\n",proc->subframe,proc->instance_cnt,pthread_mutex_trylock(&proc->mutex));

	pthread_cond_wait(&proc->cond_rx,&proc->mutex_rx);
      }
      //      LOG_I(PHY,"Waking up and unlocking mutex for eNB proc %d\n",proc->subframe);
      if (pthread_mutex_unlock(&proc->mutex_rx) != 0) {	
	LOG_E(PHY,"[SCHED][eNB] error unlocking mutex for eNB RX proc %d\n",proc->subframe);
      }
    }
    vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_eNB_PROC_RX0+(2*proc->subframe),1);    
    vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_FRAME_NUMBER_RX_ENB, proc->frame_rx);

    if (oai_exit) break;
    
    if ((((PHY_vars_eNB_g[0][proc->CC_id]->lte_frame_parms.frame_type == TDD )&&(subframe_select(&PHY_vars_eNB_g[0][proc->CC_id]->lte_frame_parms,proc->subframe_rx)==SF_UL)) ||
	 (PHY_vars_eNB_g[0][proc->CC_id]->lte_frame_parms.frame_type == FDD))){
      phy_procedures_eNB_RX(proc->subframe,PHY_vars_eNB_g[0][proc->CC_id],0,no_relay);
    }
    if ((subframe_select(&PHY_vars_eNB_g[0][proc->CC_id]->lte_frame_parms,proc->subframe_rx)==SF_S)){
      phy_procedures_eNB_S_RX(proc->subframe,PHY_vars_eNB_g[0][proc->CC_id],0,no_relay);
    }
      
    if (pthread_mutex_lock(&proc->mutex_rx) != 0) {
      printf("[openair][SCHED][eNB] error locking mutex for eNB RX proc %d\n",proc->subframe);
    }
    else {
      proc->instance_cnt_rx--;
      
      if (pthread_mutex_unlock(&proc->mutex_rx) != 0) {	
	printf("[openair][SCHED][eNB] error unlocking mutex for eNB RX proc %d\n",proc->subframe);
      }
    }

    proc->frame_rx++;
    if (proc->frame_rx==1024)
      proc->frame_rx=0;
    
  }
  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_eNB_PROC_RX0+(2*proc->subframe),0);        
#ifdef HARD_RT
  rt_make_soft_real_time();
#endif

#ifdef DEBUG_THREADS
  printf("Exiting eNB thread RX %d\n",proc->subframe);
#endif
  // clean task
#ifdef RTAI
  rt_task_delete(task);
#else
  eNB_thread_rx_status[proc->subframe]=0;
  pthread_exit(&eNB_thread_rx_status[proc->subframe]);
#endif

#ifdef DEBUG_THREADS
  printf("Exiting eNB RX thread %d\n",proc->subframe);
#endif
}




void init_eNB_proc(void) {

  int i;
  int CC_id;

  for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++) {
    for (i=0;i<10;i++) {
      pthread_attr_init (&attr_eNB_proc_tx[CC_id][i]);
      pthread_attr_setstacksize(&attr_eNB_proc_tx[CC_id][i],OPENAIR_THREAD_STACK_SIZE);
      //attr_dlsch_threads.priority = 1;
      sched_param_eNB_proc_tx[CC_id][i].sched_priority = sched_get_priority_max(SCHED_FIFO)-1; //OPENAIR_THREAD_PRIORITY;
      pthread_attr_setschedparam  (&attr_eNB_proc_tx[CC_id][i], &sched_param_eNB_proc_tx[CC_id][i]);
      pthread_attr_setschedpolicy (&attr_eNB_proc_tx[CC_id][i], SCHED_FIFO);
      
      pthread_attr_init (&attr_eNB_proc_rx[CC_id][i]);
      pthread_attr_setstacksize(&attr_eNB_proc_rx[CC_id][i],OPENAIR_THREAD_STACK_SIZE);
      //attr_dlsch_threads.priority = 1;
      sched_param_eNB_proc_rx[CC_id][i].sched_priority = sched_get_priority_max(SCHED_FIFO)-1; //OPENAIR_THREAD_PRIORITY;
      pthread_attr_setschedparam  (&attr_eNB_proc_rx[CC_id][i], &sched_param_eNB_proc_rx[CC_id][i]);
      pthread_attr_setschedpolicy (&attr_eNB_proc_rx[CC_id][i], SCHED_FIFO);
      
      PHY_vars_eNB_g[0][CC_id]->proc[i].instance_cnt_tx=-1;
      PHY_vars_eNB_g[0][CC_id]->proc[i].instance_cnt_rx=-1;
      PHY_vars_eNB_g[0][CC_id]->proc[i].subframe=i;
      PHY_vars_eNB_g[0][CC_id]->proc[i].CC_id = CC_id;
      pthread_mutex_init(&PHY_vars_eNB_g[0][CC_id]->proc[i].mutex_tx,NULL);
      pthread_mutex_init(&PHY_vars_eNB_g[0][CC_id]->proc[i].mutex_rx,NULL);
      pthread_cond_init(&PHY_vars_eNB_g[0][CC_id]->proc[i].cond_tx,NULL);
      pthread_cond_init(&PHY_vars_eNB_g[0][CC_id]->proc[i].cond_rx,NULL);
      pthread_create(&PHY_vars_eNB_g[0][CC_id]->proc[i].pthread_tx,NULL,eNB_thread_tx,(void*)&PHY_vars_eNB_g[0][CC_id]->proc[i]);
      pthread_create(&PHY_vars_eNB_g[0][CC_id]->proc[i].pthread_rx,NULL,eNB_thread_rx,(void*)&PHY_vars_eNB_g[0][CC_id]->proc[i]);
      PHY_vars_eNB_g[0][CC_id]->proc[i].frame_tx = 0;
      PHY_vars_eNB_g[0][CC_id]->proc[i].frame_rx = 0;
#ifdef EXMIMO
      PHY_vars_eNB_g[0][CC_id]->proc[i].subframe_rx = (i+9)%10;
      PHY_vars_eNB_g[0][CC_id]->proc[i].subframe_tx = (i+1)%10;
#else
      PHY_vars_eNB_g[0][CC_id]->proc[i].subframe_rx = i;
      PHY_vars_eNB_g[0][CC_id]->proc[i].subframe_tx = (i+2)%10;
#endif
    }
  
  
#ifdef EXMIMO
    // TX processes subframe + 1, RX subframe -1
    // Note this inialization is because the first process awoken for frame 0 is number 1 and so processes 9 and 0 have to start with frame 1
    
    //PHY_vars_eNB_g[0][CC_id]->proc[0].frame_rx = 1023;
    PHY_vars_eNB_g[0][CC_id]->proc[9].frame_tx = 1;
    PHY_vars_eNB_g[0][CC_id]->proc[0].frame_tx = 1;
#else
    // TX processes subframe +2, RX subframe
    // Note this inialization is because the first process awoken for frame 0 is number 1 and so processes 8,9 and 0 have to start with frame 1
    //    PHY_vars_eNB_g[0][CC_id]->proc[7].frame_tx = 1;
    PHY_vars_eNB_g[0][CC_id]->proc[8].frame_tx = 1;
    PHY_vars_eNB_g[0][CC_id]->proc[9].frame_tx = 1;
    //    PHY_vars_eNB_g[0][CC_id]->proc[0].frame_tx = 1;
#endif
  }
}

void kill_eNB_proc(void) {

  int i;
  int *status_tx,*status_rx;
  int CC_id;

  for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++) 
    for (i=0;i<10;i++) {
      
#ifdef DEBUG_THREADS
      printf("Killing TX CC_id %d thread %d\n",CC_id,i);
#endif
      PHY_vars_eNB_g[0][CC_id]->proc[i].instance_cnt_tx=0; 
      pthread_cond_signal(&PHY_vars_eNB_g[0][CC_id]->proc[i].cond_tx);
#ifdef DEBUG_THREADS
      printf("Joining eNB TX CC_id %d thread %d...\n",CC_id,i);
#endif
      pthread_join(PHY_vars_eNB_g[0][CC_id]->proc[i].pthread_tx,(void**)status_tx);
#ifdef DEBUG_THREADS
      if (status_tx) printf("status %d...\n",*status_tx);
#endif
#ifdef DEBUG_THREADS
      printf("Killing RX CC_id %d thread %d\n",CC_id,i);
#endif
      PHY_vars_eNB_g[0][CC_id]->proc[i].instance_cnt_rx=0; 
      pthread_cond_signal(&PHY_vars_eNB_g[0][CC_id]->proc[i].cond_rx);
#ifdef DEBUG_THREADS
      printf("Joining eNB RX CC_id %d thread %d...\n",CC_id,i);
#endif
      pthread_join(PHY_vars_eNB_g[0][CC_id]->proc[i].pthread_rx,(void**)status_rx);
#ifdef DEBUG_THREADS 
      if (status_rx) printf("status %d...\n",*status_rx);
#endif
      pthread_mutex_destroy(&PHY_vars_eNB_g[0][CC_id]->proc[i].mutex_tx);
      pthread_mutex_destroy(&PHY_vars_eNB_g[0][CC_id]->proc[i].mutex_rx);
      pthread_cond_destroy(&PHY_vars_eNB_g[0][CC_id]->proc[i].cond_tx);
      pthread_cond_destroy(&PHY_vars_eNB_g[0][CC_id]->proc[i].cond_rx);
    }
}




  
/* This is the main eNB thread. */
int eNB_thread_status;



static void *eNB_thread(void *arg)
{
#ifdef RTAI
  RT_TASK *task;
#endif
#ifdef EXMIMO
  unsigned char slot=1;
#else
  unsigned char slot=0;
#endif
  int frame=0;
  int CC_id;

  RTIME time_in, time_diff;

  int sf;
#ifdef EXMIMO
  volatile unsigned int *DAQ_MBOX = openair0_daq_cnt();
  int mbox_target=0,mbox_current=0;
  int hw_slot,delay_cnt;
  int diff;
  int ret;
  int first_run=1;
#else
  unsigned int rx_cnt = 0;
  unsigned int tx_cnt = tx_delay;
  //  int tx_offset;

  hw_subframe = 0;
#endif

#if defined(ENABLE_ITTI)
  /* Wait for eNB application initialization to be complete (eNB registration to MME) */
  wait_system_ready ("Waiting for eNB application to be ready %s\r", &start_eNB);
#endif

#ifdef RTAI
  task = rt_task_init_schmod(nam2num("TASK0"), 0, 0, 0, SCHED_FIFO, 0xF);
#endif

  if (!oai_exit) {
#ifdef RTAI
    printf("[SCHED][eNB] Started eNB thread (id %p)\n",task);
#else
    printf("[SCHED][eNB] Started eNB thread on CPU %d\n",
	   sched_getcpu());
#endif

#ifdef HARD_RT
    rt_make_hard_real_time();
#endif

    printf("eNB_thread: mlockall in ...\n");
    mlockall(MCL_CURRENT | MCL_FUTURE);
    printf("eNB_thread: mlockall out ...\n");

    timing_info.time_min = 100000000ULL;
    timing_info.time_max = 0;
    timing_info.time_avg = 0;
    timing_info.n_samples = 0;

#ifdef USRP
    printf("waiting for USRP sync (eNB_thread)\n");
#ifdef RTAI
    rt_sem_wait(sync_sem);
#else
    pthread_mutex_lock(&sync_mutex);
    pthread_cond_wait(&sync_cond, &sync_mutex);
    pthread_mutex_unlock(&sync_mutex);
#endif
    //    printf("starting eNB thread @ %llu\n",get_usrp_time(&openair0));
#endif

    while (!oai_exit) {

#ifdef EXMIMO
      hw_slot = (((((volatile unsigned int *)DAQ_MBOX)[0]+1)%150)<<1)/15;
      //        LOG_D(HW,"eNB frame %d, time %llu: slot %d, hw_slot %d (mbox %d)\n",frame,rt_get_time_ns(),slot,hw_slot,((unsigned int *)DAQ_MBOX)[0]);
      vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_HW_SUBFRAME, hw_slot>>1);
      vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_HW_FRAME, frame);
      //this is the mbox counter where we should be
      mbox_target = mbox_bounds[slot];
      //this is the mbox counter where we are
      mbox_current = ((volatile unsigned int *)DAQ_MBOX)[0];
      //this is the time we need to sleep in order to synchronize with the hw (in multiples of DAQ_PERIOD)
      if ((mbox_current>=135) && (mbox_target<15)) //handle the frame wrap-arround
	diff = 150-mbox_current+mbox_target;
      else if ((mbox_current<15) && (mbox_target>=135))
	diff = -150+mbox_target-mbox_current;
      else
	diff = mbox_target - mbox_current;
      
      //when we start the aquisition we want to start with slot 0, so we rather wait for the hardware than to advance the slot number (a positive diff will cause the programm to go into the second if clause rather than the first)
      if (first_run==1) {
	first_run=0;
	if (diff<0)
	  diff = diff +150;
	LOG_I(HW,"eNB Frame %d, time %llu: slot %d, hw_slot %d, diff %d\n",frame, rt_get_time_ns(), slot, hw_slot, diff);
      } 

      if (((slot%2==0) && (diff < (-14))) || ((slot%2==1) && (diff < (-7)))) {
	// at the eNB, even slots have double as much time since most of the processing is done here and almost nothing in odd slots
	LOG_D(HW,"eNB Frame %d, time %llu: missed slot, proceeding with next one (slot %d, hw_slot %d, mbox_current %d, mbox_target %d, diff %d)\n",frame, rt_get_time_ns(), slot, hw_slot, mbox_current, mbox_target, diff);
	slot++;
	//if (frame > 0)
	exit_fun("[HW][eNB] missed slot");
      }
      if (diff>8)
	LOG_D(HW,"eNB Frame %d, time %llu: skipped slot, waiting for hw to catch up (slot %d, hw_slot %d, mbox_current %d, mbox_target %d, diff %d)\n",frame, rt_get_time_ns(), slot, hw_slot, mbox_current, mbox_target, diff);
      
      vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_DAQ_MBOX, *DAQ_MBOX);
      vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_DIFF, diff);
      
      delay_cnt = 0;
      while ((diff>0) && (!oai_exit)) {
	time_in = rt_get_time_ns();
	//LOG_D(HW,"eNB Frame %d delaycnt %d : hw_slot %d (%d), slot %d, (slot+1)*15=%d, diff %d, time %llu\n",frame,delay_cnt,hw_slot,((unsigned int *)DAQ_MBOX)[0],slot,(((slot+1)*15)>>1),diff,time_in);
	//LOG_D(HW,"eNB Frame %d, time %llu: sleeping for %llu (slot %d, hw_slot %d, diff %d, mbox %d, delay_cnt %d)\n", frame, time_in, diff*DAQ_PERIOD,slot,hw_slot,diff,((volatile unsigned int *)DAQ_MBOX)[0],delay_cnt);
	vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_RT_SLEEP,1);
	ret = rt_sleep_ns(diff*DAQ_PERIOD);
	vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_RT_SLEEP,0);
	vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_DAQ_MBOX, *DAQ_MBOX);
	if (ret)
	  LOG_D(HW,"eNB Frame %d, time %llu: rt_sleep_ns returned %d\n",frame, time_in);
	hw_slot = (((((volatile unsigned int *)DAQ_MBOX)[0]+1)%150)<<1)/15;
	//LOG_D(HW,"eNB Frame %d : hw_slot %d, time %llu\n",frame,hw_slot,rt_get_time_ns());
	delay_cnt++;
	if (delay_cnt == 10) {
	  LOG_D(HW,"eNB Frame %d: HW stopped ... \n",frame);
	  exit_fun("[HW][eNB] HW stopped");
	}
	mbox_current = ((volatile unsigned int *)DAQ_MBOX)[0];
	if ((mbox_current>=135) && (mbox_target<15)) //handle the frame wrap-arround
	  diff = 150-mbox_current+mbox_target;
	else if ((mbox_current<15) && (mbox_target>=135))
	  diff = -150+mbox_target-mbox_current;
	else
	  diff = mbox_target - mbox_current;
      }

#else  // EXMIMO
      vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_HW_SUBFRAME, hw_subframe);
      vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_HW_FRAME, frame);
      while (rx_cnt < sf_bounds[hw_subframe]) {

	openair0_timestamp time0,time1;
	unsigned int rxs;
#ifndef USRP_DEBUG
	vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_TRX_READ,1);
	/*
	// Grab 1/4 of RX buffer and get timestamp

	rxs = openair0.trx_read_func(&openair0, 
	&timestamp, 
	&rxdata[rx_cnt*samples_per_packets], 
	(samples_per_packets>>2));
	if (rxs != (samples_per_packets>>2))
	oai_exit=1;

	*/

	vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_TXCNT,tx_cnt);
	vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_RXCNT,rx_cnt*samples_per_packets);

	rxs = openair0.trx_read_func(&openair0, 
				     &timestamp, 
				     &rxdata[rx_cnt*samples_per_packets], 
				     samples_per_packets);

	if (rxs != samples_per_packets)
	  oai_exit=1;

	vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_TRX_READ,0);

	// Transmit TX buffer based on timestamp from RX
	vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_TRX_WRITE,1);
	openair0.trx_write_func(&openair0, 
				(timestamp+samples_per_packets*tx_delay-tx_forward_nsamps), 
				&txdata[tx_cnt*samples_per_packets], 
				samples_per_packets, 
				1);
	vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_TRX_WRITE,0);
	/*
	// Grab remaining 3/4 of RX buffer
	vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_TRX_READ,1);
	rxs = openair0.trx_read_func(&openair0, 
	&timestamp, 
	&rxdata[(rx_cnt*samples_per_packets)+(samples_per_packets>>2)], 
	3*((samples_per_packets>>2)));
	vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_TRX_READ,0);
	if (rxs != (3*(samples_per_packets>>2)))
	oai_exit=1;
	*/
#else
	rt_sleep_ns(1000000);
#endif
	if (rx_cnt == sf_bounds_tx[hw_subframe]) {
	  for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++) {
	    if (pthread_mutex_lock(&PHY_vars_eNB_g[0][CC_id]->proc[hw_subframe].mutex_tx) != 0) {
	      LOG_E(PHY,"[eNB] ERROR pthread_mutex_lock for eNB TX thread %d (IC %d)\n",hw_subframe,PHY_vars_eNB_g[0][CC_id]->proc[hw_subframe].instance_cnt_tx);   
	    }
	    else {
	      //	      LOG_I(PHY,"[eNB] Waking up eNB process %d (IC %d,rx_cnt %d)\n",hw_subframe,PHY_vars_eNB_g[0][CC_id]->proc[hw_subframe].instance_cnt_tx,rx_cnt); 
	      PHY_vars_eNB_g[0][CC_id]->proc[hw_subframe].instance_cnt_tx++;
	      pthread_mutex_unlock(&PHY_vars_eNB_g[0][CC_id]->proc[hw_subframe].mutex_tx);
	      if (PHY_vars_eNB_g[0][CC_id]->proc[hw_subframe].instance_cnt_tx == 0) {
		if (pthread_cond_signal(&PHY_vars_eNB_g[0][CC_id]->proc[hw_subframe].cond_tx) != 0) {
		  LOG_E(PHY,"[eNB] ERROR pthread_cond_signal for eNB TX thread %d\n",hw_subframe);
		}
	      }
	      else {
		LOG_W(PHY,"[eNB] Frame %d, eNB TX thread %d busy!! (rx_cnt %d)\n",PHY_vars_eNB_g[0][CC_id]->proc[hw_subframe].frame_tx,hw_subframe,rx_cnt);
		oai_exit=1;
	      }
	    }
	  }
	}
	rx_cnt++;
	tx_cnt++;

	if(tx_cnt == max_cnt)
	  tx_cnt = 0;
      }

      if(rx_cnt == max_cnt)
	rx_cnt = 0; 
      

#endif // USRP
     
      if (oai_exit) break;


      
      timing_info.time_last = timing_info.time_now;
      timing_info.time_now = rt_get_time_ns();
      
      if (timing_info.n_samples>0) {
	time_diff = timing_info.time_now - timing_info.time_last;
	if (time_diff < timing_info.time_min)
	  timing_info.time_min = time_diff;
	if (time_diff > timing_info.time_max)
	  timing_info.time_max = time_diff;
	timing_info.time_avg += time_diff;
      }
      
      timing_info.n_samples++;
      /*
	if ((timing_info.n_samples%2000)==0) {
	LOG_D(HW,"frame %d (%d), slot %d, hw_slot %d: diff=%llu, min=%llu, max=%llu, avg=%llu (n_samples %d)\n",
	frame, PHY_vars_eNB_g[0]->frame, slot, hw_slot,time_diff,
	timing_info.time_min,timing_info.time_max,timing_info.time_avg/timing_info.n_samples,timing_info.n_samples);
	timing_info.n_samples = 0;
	timing_info.time_avg = 0;
	}
      */
      //}
    
      if ((slot&1) == 1) {
#ifdef EXMIMO
	sf = ((slot>>1)+1)%10;
#else
	sf = hw_subframe;
#endif
	//		    LOG_I(PHY,"[eNB] Multithread slot %d (IC %d)\n",slot,PHY_vars_eNB_g[0][CC_id]->proc[sf].instance_cnt);
	
	for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++) {
#ifdef EXMIMO 
	  if (pthread_mutex_lock(&PHY_vars_eNB_g[0][CC_id]->proc[sf].mutex_tx) != 0) {
	    LOG_E(PHY,"[eNB] ERROR pthread_mutex_lock for eNB TX thread %d (IC %d)\n",sf,PHY_vars_eNB_g[0][CC_id]->proc[sf].instance_cnt_tx);   
	  }
	  else {
	    //		      LOG_I(PHY,"[eNB] Waking up eNB process %d (IC %d)\n",sf,PHY_vars_eNB_g[0][CC_id]->proc[sf].instance_cnt); 
	    PHY_vars_eNB_g[0][CC_id]->proc[sf].instance_cnt_tx++;
	    pthread_mutex_unlock(&PHY_vars_eNB_g[0][CC_id]->proc[sf].mutex_tx);
	    if (PHY_vars_eNB_g[0][CC_id]->proc[sf].instance_cnt_tx == 0) {
	      if (pthread_cond_signal(&PHY_vars_eNB_g[0][CC_id]->proc[sf].cond_tx) != 0) {
		LOG_E(PHY,"[eNB] ERROR pthread_cond_signal for eNB TX thread %d\n",sf);
	      }
	    }
	    else {
	      LOG_W(PHY,"[eNB] Frame %d, eNB TX thread %d busy!!\n",PHY_vars_eNB_g[0][CC_id]->proc[sf].frame_tx,sf);
	      oai_exit=1;
	    }
	  }
#endif	    
	  if (pthread_mutex_lock(&PHY_vars_eNB_g[0][CC_id]->proc[sf].mutex_rx) != 0) {
	    LOG_E(PHY,"[eNB] ERROR pthread_mutex_lock for eNB RX thread %d (IC %d)\n",sf,PHY_vars_eNB_g[0][CC_id]->proc[sf].instance_cnt_rx);   
	  }
	  else {
	    //		      LOG_I(PHY,"[eNB] Waking up eNB process %d (IC %d)\n",sf,PHY_vars_eNB_g[0][CC_id]->proc[sf].instance_cnt); 
	    PHY_vars_eNB_g[0][CC_id]->proc[sf].instance_cnt_rx++;
	    pthread_mutex_unlock(&PHY_vars_eNB_g[0][CC_id]->proc[sf].mutex_rx);
	    if (PHY_vars_eNB_g[0][CC_id]->proc[sf].instance_cnt_rx == 0) {
	      if (pthread_cond_signal(&PHY_vars_eNB_g[0][CC_id]->proc[sf].cond_rx) != 0) {
		LOG_E(PHY,"[eNB] ERROR pthread_cond_signal for eNB RX thread %d\n",sf);
	      }
	    }
	    else {
	      LOG_W(PHY,"[eNB] Frame %d, eNB RX thread %d busy!!\n",PHY_vars_eNB_g[0][CC_id]->proc[sf].frame_rx,sf);
	      oai_exit=1;
	    }
	  }
	    
	}
      }
	  
      
#ifndef RTAI
      //pthread_mutex_lock(&tti_mutex);
#endif




#ifndef USRP
      slot++;
      if (slot == 20) {
	frame++;
	slot = 0;
      }
#else
      hw_subframe++;
      slot+=2;
      if(hw_subframe==10) {
	hw_subframe = 0;
	frame++;
	slot = 1;
      }
#endif     


#if defined(ENABLE_ITTI)
      itti_update_lte_time(frame, slot);
#endif
    }
  }
#ifdef DEBUG_THREADS
  printf("eNB_thread: finished, ran %d times.\n",frame);
#endif
  
#ifdef HARD_RT
  rt_make_soft_real_time();
#endif


#ifdef DEBUG_THREADS
  printf("Exiting eNB_thread ...");
#endif
  // clean task
#ifdef RTAI
  rt_task_delete(task);
#else
  eNB_thread_status = 0;
  pthread_exit(&eNB_thread_status);
#endif
#ifdef DEBUG_THREADS
  printf("eNB_thread deleted. returning\n");
#endif
  return 0;
}

#ifdef USRP_DEBUG
int is_synchronized=1;
#else
int is_synchronized=0;
#endif

static void *UE_thread_synch(void *arg) {

  int i,hw_slot_offset,CC_id;
  PHY_VARS_UE *UE = arg;

  printf("UE_thread_sync in with PHY_vars_UE %p\n",arg);
#ifdef USRP
  printf("waiting for USRP sync (UE_thread_synch) \n");
#ifdef RTAI
  rt_sem_wait(sync_sem);
#else

  pthread_mutex_lock(&sync_mutex);
  printf("Locked sync_mutex, waiting (UE_sync_thread)\n");
  pthread_cond_wait(&sync_cond, &sync_mutex);
  pthread_mutex_unlock(&sync_mutex);
  printf("unlocked sync_mutex (UE_sync_thread)\n");
#endif
  printf("starting UE synch thread\n");
#endif
  
  while (!oai_exit) {
    
    if (pthread_mutex_lock(&UE->mutex_synch) != 0) {
      LOG_E(PHY,"[SCHED][eNB] error locking mutex for UE initial synch thread\n");
      oai_exit=1;
    }
    else {
      while (UE->instance_cnt_synch < 0) {
	pthread_cond_wait(&UE->cond_synch,&UE->mutex_synch);
      }
      if (pthread_mutex_unlock(&UE->mutex_synch) != 0) {	
	LOG_E(PHY,"[SCHED][eNB] error unlocking mutex for UE Initial Synch thread\n");
	oai_exit=1;
      }
    
    }  // mutex_lock      
    //    LOG_I(PHY,"[SCHED][UE] Running UE intial synch\n");
    if (initial_sync(PHY_vars_UE_g[0][0],mode)==0) {
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

      T0 = rt_get_time_ns();
      
      is_synchronized = 1;
      PHY_vars_UE_g[0][0]->slot_rx = 0;
      //oai_exit=1;
      //start the DMA transfers
      //LOG_D(HW,"Before openair0_start_rt_acquisition \n");
      //openair0_start_rt_acquisition(0);
      
      hw_slot_offset = (PHY_vars_UE_g[0][0]->rx_offset<<1) / PHY_vars_UE_g[0][0]->lte_frame_parms.samples_per_tti;
      LOG_I(HW,"Got synch: hw_slot_offset %d\n",hw_slot_offset);
      
    }
    else {
      
      if (openair_daq_vars.freq_offset >= 0) {
	openair_daq_vars.freq_offset += 100;
	openair_daq_vars.freq_offset *= -1;
      }
      else {
	openair_daq_vars.freq_offset *= -1;
      }
      if (abs(openair_daq_vars.freq_offset) > 7500) {
	LOG_I(PHY,"[initial_sync] No cell synchronization found, abandoning\n");
	mac_xface->macphy_exit("No cell synchronization found, abandoning");
      }
      else {
	LOG_I(PHY,"[initial_sync] trying carrier off %d Hz, rxgain %d\n",openair_daq_vars.freq_offset,
	      PHY_vars_UE_g[0][0]->rx_total_gain_dB);
	for (card=0;card<MAX_CARDS;card++) {
	  for (i=0; i<openair0_cfg[card].rx_num_channels; i++) {
	    openair0_cfg[card].rx_freq[i] = downlink_frequency[card][i]+openair_daq_vars.freq_offset;
	    openair0_cfg[card].tx_freq[i] = downlink_frequency[card][i]+uplink_frequency_offset[card][i]+openair_daq_vars.freq_offset;
	    openair0_cfg[card].rx_gain[i] = PHY_vars_UE_g[0][0]->rx_total_gain_dB-73;  // 65 calibrated for USRP B210 @ 2.6 GHz
#ifdef USRP
#ifndef USRP_DEBUG
	    openair0_set_frequencies(&openair0,&openair0_cfg[0]);
	    //	    openair0_set_gains(&openair0,&openair0_cfg[0]);
#endif
#endif
	  }
	}

	//	    openair0_dump_config(&openair0_cfg[0],UE_flag);
	
	//	    rt_sleep_ns(FRAME_PERIOD);
      } // freq_offset
    } // initial_sync=0
    
    if (pthread_mutex_lock(&PHY_vars_UE_g[0][0]->mutex_synch) != 0) {
      printf("[openair][SCHED][eNB] error locking mutex for UE synch\n");
    }
    else {
      PHY_vars_UE_g[0][0]->instance_cnt_synch--;
      
      if (pthread_mutex_unlock(&PHY_vars_UE_g[0][0]->mutex_synch) != 0) {	
	printf("[openair][SCHED][eNB] error unlocking mutex for UE synch\n");
      }
    }
    
  }  // while !oai_exit
  return(0);
}

static void *UE_thread_tx(void *arg) {

  PHY_VARS_UE *UE = (PHY_VARS_UE*)arg;

#ifndef OPENAIR2
  UE->UE_mode[eNB_id]=PUSCH;
  UE->prach_resources[eNB_id] = &prach_resources_local;
  prach_resources_local.ra_RNTI = 0xbeef;
  prach_resources_local.ra_PreambleIndex = 0;
#endif
  UE->instance_cnt_tx=-1;

  mlockall(MCL_CURRENT | MCL_FUTURE);

  while (!oai_exit) {

    if (pthread_mutex_lock(&UE->mutex_tx) != 0) {
      LOG_E(PHY,"[SCHED][eNB] error locking mutex for UE TX\n");
      oai_exit=1;
    }
    else {
      
      while (UE->instance_cnt_tx < 0) {
	pthread_cond_wait(&UE->cond_tx,&UE->mutex_tx);
      }
      if (pthread_mutex_unlock(&UE->mutex_tx) != 0) {	
	LOG_E(PHY,"[SCHED][eNB] error unlocking mutex for UE TX\n");
	oai_exit=1;
      }
    }
  

    if ((subframe_select(&UE->lte_frame_parms,UE->slot_tx>>1)==SF_UL)||
	(UE->lte_frame_parms.frame_type == FDD)){
      phy_procedures_UE_TX(UE,eNB_id,0,mode,no_relay);
    }
    if ((subframe_select(&UE->lte_frame_parms,UE->slot_tx>>1)==SF_S) &&
	((UE->slot_tx&1)==1)) {
      phy_procedures_UE_S_TX(UE,eNB_id,0,no_relay);
    }

    if (pthread_mutex_lock(&UE->mutex_tx) != 0) {
      printf("[openair][SCHED][eNB] error locking mutex for UE TX thread\n");
    }
    else {
      UE->instance_cnt_tx--;
      
      if (pthread_mutex_unlock(&UE->mutex_tx) != 0) {	
	printf("[openair][SCHED][eNB] error unlocking mutex for UE\n");
      }
    }

    UE->slot_tx++;
    if (UE->slot_tx==20) {
      UE->slot_tx=0;
      UE->frame_tx++;
    }
  }
  return(0);
}

static void *UE_thread_rx(void *arg) {
  
  PHY_VARS_UE *UE = (PHY_VARS_UE*)arg;
  int i;
  
  UE->instance_cnt_rx=-1;
  
  mlockall(MCL_CURRENT | MCL_FUTURE);
  
#ifndef EXMIMO
  printf("waiting for USRP sync (UE_thread_rx)\n");
#ifdef RTAI
  rt_sem_wait(sync_sem);
#else
  pthread_mutex_lock(&sync_mutex);
  printf("Locked sync_mutex, waiting (UE_thread_rx)\n");
  pthread_cond_wait(&sync_cond, &sync_mutex);
  pthread_mutex_unlock(&sync_mutex);
  printf("unlocked sync_mutex, waiting (UE_thread_rx)\n");
#endif
#endif

  printf("Starting UE RX thread\n");

  while (!oai_exit) { 
    printf("UE_thread_rx: locking UE RX mutex\n");
    if (pthread_mutex_lock(&UE->mutex_rx) != 0) {
      LOG_E(PHY,"[SCHED][eNB] error locking mutex for UE RX\n");
      oai_exit=1;
    }
    else {
      
      printf("UE_thread_rx: unlocking UE RX mutex (IC %d)\n",UE->instance_cnt_rx);
      while (UE->instance_cnt_rx < 0) {
	printf("Waiting for cond_rx (%p)\n",(void*)&UE->cond_rx);
	pthread_cond_wait(&UE->cond_rx,&UE->mutex_rx);
	printf("Got UE RX condition, IC %d @ %llu\n",UE->instance_cnt_rx,rt_get_time_ns()-T0);
      }
      if (pthread_mutex_unlock(&UE->mutex_rx) != 0) {	
	LOG_E(PHY,"[SCHED][eNB] error unlocking mutex for UE RX\n");
	oai_exit=1;
      }
      
      for (i=0;i<2;i++) {
	printf("UE_thread_rx: processing slot %d (slot rx %d) @ %llu\n",i,UE->slot_rx,rt_get_time_ns()-T0);
	if ((subframe_select(&UE->lte_frame_parms,UE->slot_rx>>1)==SF_DL) ||
	    (UE->lte_frame_parms.frame_type == FDD)){
	  phy_procedures_UE_RX(UE,eNB_id,0,mode,no_relay,NULL);
	}
	if ((subframe_select(&UE->lte_frame_parms,UE->slot_rx>>1)==SF_S) &&
	    ((UE->slot_rx&1)==0)) {
	  phy_procedures_UE_RX(UE,eNB_id,0,mode,no_relay,NULL);
	}
	
	UE->slot_rx++;
	if (UE->slot_rx==20) {
	  UE->slot_rx=0;
	  UE->frame_rx++;
	}    
      }
    }
    
    if (pthread_mutex_lock(&UE->mutex_rx) != 0) {
      printf("[openair][SCHED][eNB] error locking mutex for UE RX\n");
    }
    else {
      UE->instance_cnt_rx--;
      
      if (pthread_mutex_unlock(&UE->mutex_rx) != 0) {	
	printf("[openair][SCHED][eNB] error unlocking mutex for UE RX\n");
      }
    }
    printf("UE_thread_rx done\n");
  }
       
  return(0);
}




#ifndef EXMIMO
#define RX_OFF_MAX 10
#define RX_OFF_MIN 5
#define RX_OFF_MID ((RX_OFF_MAX+RX_OFF_MIN)/2)

static void *UE_thread(void *arg) {

  LTE_DL_FRAME_PARMS *frame_parms=&PHY_vars_UE_g[0][0]->lte_frame_parms;

  int slot=1,frame=0,hw_slot,last_slot, next_slot,hw_subframe=0,rx_cnt=0,tx_cnt=0;
  // unsigned int aa;
  int dummy[samples_per_packets];
  int dummy_dump = 0;
  int tx_enabled=0;
  int start_rx_stream=0;
  int rx_off_diff = 0;
  int rx_correction_timer = 0;

  openair0_timestamp time0,time1;
  unsigned int rxs;

  printf("waiting for USRP sync (UE_thread)\n");
#ifdef RTAI
  rt_sem_wait(sync_sem);
#else
  pthread_mutex_lock(&sync_mutex);
  printf("Locked sync_mutex, waiting (UE_thread)\n");
  pthread_cond_wait(&sync_cond, &sync_mutex);
  pthread_mutex_unlock(&sync_mutex);
  printf("unlocked sync_mutex, waiting (UE_thread)\n");
#endif

  printf("starting UE thread\n");

  T0 = rt_get_time_ns();
  while (!oai_exit) {
    vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_HW_SUBFRAME, hw_subframe);
    vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_HW_FRAME, frame);


    while (rx_cnt < sf_bounds[hw_subframe]) {
  
      vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_TRX_READ,1);
  
#ifndef USRP_DEBUG
      rxs = openair0.trx_read_func(&openair0,
				   &timestamp,
				   (dummy_dump==0) ? &rxdata[rx_cnt*samples_per_packets] : dummy,
				   samples_per_packets - ((rx_cnt==0) ? rx_off_diff : 0));
      if (rxs != (samples_per_packets- ((rx_cnt==0) ? rx_off_diff : 0)))
	oai_exit=1;

      rx_off_diff = 0;
      vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_TRX_READ,0);
      
      // Transmit TX buffer based on timestamp from RX
      if (tx_enabled) {
	vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_TRX_WRITE,1);
	openair0.trx_write_func(&openair0,
				(timestamp+samples_per_packets*tx_delay-tx_forward_nsamps),
				&txdata[tx_cnt*samples_per_packets],
				samples_per_packets,
				1);
	vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_TRX_WRITE,0);
      }
#else
      rt_sleep_ns(10000000);
#endif
      rx_cnt++;
      tx_cnt++;
      
      if(tx_cnt == max_cnt)
	tx_cnt = 0;
    }
    
    if(rx_cnt == max_cnt)
      rx_cnt = 0;


    
    if (is_synchronized==1)  {
      LOG_D(HW,"UE_thread: hw_frame %d, hw_subframe %d (time %llu)\n",frame,hw_subframe,rt_get_time_ns()-T0);

      if (start_rx_stream==1) {
	//	printf("UE_thread: locking UE mutex_rx\n");
	if (pthread_mutex_lock(&PHY_vars_UE_g[0][0]->mutex_rx) != 0) {
	  LOG_E(PHY,"[SCHED][UE] error locking mutex for UE RX thread\n");
	  oai_exit=1;
	}
	else {
 	  
	  PHY_vars_UE_g[0][0]->instance_cnt_rx++;
	  //	  printf("UE_thread: Unlocking UE mutex_rx\n");
	  pthread_mutex_unlock(&PHY_vars_UE_g[0][0]->mutex_rx);
	  if (PHY_vars_UE_g[0][0]->instance_cnt_rx == 0) {
	    LOG_D(HW,"Scheduling UE RX for frame %d (hw frame %d), subframe %d (%d), mode %d\n",PHY_vars_UE_g[0][0]->frame_rx,frame,hw_subframe,PHY_vars_UE_g[0][0]->slot_rx>>1,mode);
	    if (pthread_cond_signal(&PHY_vars_UE_g[0][0]->cond_rx) != 0) {
	      LOG_E(PHY,"[SCHED][UE] ERROR pthread_cond_signal for UE RX thread\n");
	      oai_exit=1;
	    }
	    else {
	      //	      printf("UE_thread: cond_signal for RX ok (%p) @ %llu\n",(void*)&PHY_vars_UE_g[0][0]->cond_rx,rt_get_time_ns()-T0);
	    }
	    if (mode == rx_calib_ue) {
	      if (frame == 10) {
		LOG_D(PHY,"[SCHED][UE] Found cell with N_RB_DL %d, PHICH CONFIG (%d,%d), Nid_cell %d, NB_ANTENNAS_TX %d, initial frequency offset %d Hz, frequency offset %d Hz, RSSI (digital) %d dB, measured Gain %d dB, total_rx_gain %d dB, USRP rx gain %f dB\n",
		      PHY_vars_UE_g[0][0]->lte_frame_parms.N_RB_DL,
		      PHY_vars_UE_g[0][0]->lte_frame_parms.phich_config_common.phich_duration,
		      PHY_vars_UE_g[0][0]->lte_frame_parms.phich_config_common.phich_resource,
		      PHY_vars_UE_g[0][0]->lte_frame_parms.Nid_cell,
		      PHY_vars_UE_g[0][0]->lte_frame_parms.nb_antennas_tx_eNB,
		      openair_daq_vars.freq_offset,
		      PHY_vars_UE_g[0][0]->lte_ue_common_vars.freq_offset,
		      PHY_vars_UE_g[0][0]->PHY_measurements.rx_power_avg_dB[0],
		      PHY_vars_UE_g[0][0]->PHY_measurements.rx_power_avg_dB[0] - rx_input_level_dBm,
		      PHY_vars_UE_g[0][0]->rx_total_gain_dB,
		      openair0_cfg[0].rx_gain[0]
		      );
		exit_fun("[HW][UE] UE in RX calibration mode, exiting");
	      }
	    }
	  }
	  else {
	    LOG_E(PHY,"[SCHED][UE] UE RX thread busy!!\n");
	    oai_exit=1;
	  }
	}
      }
    }
    else {  // we are not yet synchronized
      
      if ((hw_subframe == 9)&&(dummy_dump == 0)) {
	// Wake up initial synch thread
	if (pthread_mutex_lock(&PHY_vars_UE_g[0][0]->mutex_synch) != 0) {
	  LOG_E(PHY,"[SCHED][UE] error locking mutex for UE initial synch thread\n");
	  oai_exit=1;
	}
	else {
	  
	  PHY_vars_UE_g[0][0]->instance_cnt_synch++;
	  pthread_mutex_unlock(&PHY_vars_UE_g[0][0]->mutex_synch);
	  dummy_dump = 1;
	  if (PHY_vars_UE_g[0][0]->instance_cnt_synch == 0) {
	    if (pthread_cond_signal(&PHY_vars_UE_g[0][0]->cond_synch) != 0) {
	      LOG_E(PHY,"[SCHED][UE] ERROR pthread_cond_signal for UE sync thread\n");
	      oai_exit=1;
	    }
	  }
	  else {
	    LOG_E(PHY,"[SCHED][UE] UE sync thread busy!!\n");
	    oai_exit=1;
	  }
	}
      }
    }
    /*
      if ((slot%2000)<10)
      LOG_D(HW,"fun0: doing very hard work\n");
    */
    
    hw_subframe++;
    slot+=2;
    if(hw_subframe==10) {
      hw_subframe = 0;
      frame++;
      slot = 1;
      if (PHY_vars_UE_g[0][0]->instance_cnt_synch < 0) {
	if (is_synchronized == 1) {
	  //	  openair0_set_gains(&openair0,&openair0_cfg[0]);
	  rx_off_diff = 0;
	  //	  LOG_D(PHY,"HW RESYNC: hw_frame %d: rx_offset = %d\n",frame,PHY_vars_UE_g[0][0]->rx_offset);
	  if ((PHY_vars_UE_g[0][0]->rx_offset > RX_OFF_MAX)&&(start_rx_stream==0)) {
	    start_rx_stream=1;
	    //LOG_D(PHY,"HW RESYNC: hw_frame %d: Resynchronizing sample stream\n");
	    frame=0;
	    // dump ahead in time to start of frame
#ifndef USRP_DEBUG
	    rxs = openair0.trx_read_func(&openair0,
					 &timestamp,
					 &rxdata[0],
					 PHY_vars_UE_g[0][0]->rx_offset);
#else
	    rt_sleep_ns(10000000);
#endif
	    PHY_vars_UE_g[0][0]->rx_offset=0;
	  }
	  else if ((PHY_vars_UE_g[0][0]->rx_offset < RX_OFF_MIN)&&(start_rx_stream==1)) {
	    //	    rx_off_diff = -PHY_vars_UE_g[0][0]->rx_offset + RX_OFF_MIN;
	  }
	  else if ((PHY_vars_UE_g[0][0]->rx_offset > (FRAME_LENGTH_COMPLEX_SAMPLES-RX_OFF_MAX)) &&(start_rx_stream==1) && (rx_correction_timer == 0)) {
	    rx_off_diff = FRAME_LENGTH_COMPLEX_SAMPLES-PHY_vars_UE_g[0][0]->rx_offset;
	    rx_correction_timer = 5;
	  }
	  if (rx_correction_timer>0)
	    rx_correction_timer--;
	  //	  LOG_D(PHY,"HW RESYNC: hw_frame %d: Correction: rx_off_diff %d (timer %d)\n",frame,rx_off_diff,rx_correction_timer);
	}
	dummy_dump=0;
      }
    }
    
#if defined(ENABLE_ITTI)
    itti_update_lte_time(frame, slot);
#endif
  }
}
#endif



#ifdef EXMIMO
/* This is the main UE thread. Initially it is doing a periodic get_frame. One synchronized it gets woken up by the kernel driver using the RTAI message mechanism (rt_send and rt_receive). */
static void *UE_thread(void *arg) {
#ifdef RTAI
  RT_TASK *task;
#endif
  // RTIME in, out, diff;
  int slot=0,frame=0,hw_slot,last_slot,next_slot;
  // unsigned int aa;
  static int is_synchronized = 0;
  int delay_cnt;
  RTIME time_in;
  int hw_slot_offset=0,rx_offset_mbox=0,mbox_target=0,mbox_current=0;
  int diff2;
  int i, ret;
  int CC_id,card;
  volatile unsigned int *DAQ_MBOX = openair0_daq_cnt();
#ifndef USRP
  //exmimo_config_t *p_exmimo_config = openair0_exmimo_pci[card].exmimo_config_ptr;;
#endif


#if defined(ENABLE_ITTI) && defined(ENABLE_USE_MME)
  /* Wait for NAS UE to start cell selection */
  wait_system_ready ("Waiting for UE to be activated by UserProcess %s\r", &start_UE);
#endif

#ifdef RTAI
  task = rt_task_init_schmod(nam2num("TASK0"), 0, 0, 0, SCHED_FIFO, 0xF);
  LOG_D(HW,"Started UE thread (id %p)\n",task);
#endif

#ifdef HARD_RT
  rt_make_hard_real_time();
#endif

  mlockall(MCL_CURRENT | MCL_FUTURE);

  openair_daq_vars.freq_offset = 0; //-7500;
  /*
    if (mode == rx_calib_ue) {
    openair_daq_vars.freq_offset = -7500;
    for (i=0; i<4; i++) {
    p_exmimo_config->rf.rf_freq_rx[i] = p_exmimo_config->rf.rf_freq_rx[i]+openair_daq_vars.freq_offset;
    p_exmimo_config->rf.rf_freq_tx[i] = p_exmimo_config->rf.rf_freq_rx[i]+openair_daq_vars.freq_offset;
    }
    openair0_dump_config(0);
    }
  */
  while (!oai_exit)  {

    hw_slot = (((((volatile unsigned int *)DAQ_MBOX)[0]+1)%150)<<1)/15; //the slot the hw is about to store

     
    if (is_synchronized) {


      //this is the mbox counter that indicates the start of the frame
      rx_offset_mbox = (PHY_vars_UE_g[0][0]->rx_offset * 150) / (10*PHY_vars_UE_g[0][0]->lte_frame_parms.samples_per_tti);
      //this is the mbox counter where we should be
      mbox_target = (((((slot+1)%20)*15+1)>>1) + rx_offset_mbox + 1)%150;
      // round up to the next multiple of two (mbox counter from express MIMO gives only even numbers)
      mbox_target = ((mbox_target+1)-((mbox_target-1)%2))%150;
      //this is the mbox counter where we are
      mbox_current = ((volatile unsigned int *)DAQ_MBOX)[0];
      //this is the time we need to sleep in order to synchronize with the hw (in multiples of DAQ_PERIOD)
      if ((mbox_current>=120) && (mbox_target<30)) //handle the frame wrap-arround
	diff2 = 150-mbox_current+mbox_target;
      else if ((mbox_current<30) && (mbox_target>=120))
	diff2 = -150+mbox_target-mbox_current;
      else
	diff2 = mbox_target - mbox_current;
	
      if (diff2 <(-7)) {
	LOG_D(HW,"UE Frame %d: missed slot, proceeding with next one (slot %d, hw_slot %d, diff %d)\n",frame, slot, hw_slot, diff2);
	if (frame>0)
	  exit_fun("[HW][UE] missed slot");
	slot++;
	if (slot==20) {
	  slot=0;
	  frame++;
	}
	continue;
      }
      if (diff2>8)
	LOG_D(HW,"UE Frame %d: skipped slot, waiting for hw to catch up (slot %d, hw_slot %d, mbox_current %d, mbox_target %d, diff %d)\n",frame, slot, hw_slot, mbox_current, mbox_target, diff2);
	
      /*
	if (frame%100==0)
	LOG_D(HW,"frame %d (%d), slot %d, hw_slot %d, rx_offset_mbox %d, mbox_target %d, mbox_current %d, diff %d\n",frame, PHY_vars_UE_g[0]->frame, slot,hw_slot,rx_offset_mbox,mbox_target,mbox_current,diff2);
      */
	
      vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_DAQ_MBOX, *DAQ_MBOX);
      vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_DIFF, diff2);
	
      delay_cnt = 0;
      while ((diff2>0) && (!oai_exit) && (is_synchronized) )  {
	time_in = rt_get_time_ns();
	//LOG_D(HW,"eNB Frame %d delaycnt %d : hw_slot %d (%d), slot %d (%d), diff %d, time %llu\n",frame,delay_cnt,hw_slot,((volatile unsigned int *)DAQ_MBOX)[0],slot,mbox_target,diff2,time_in);
	vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_DAQ_MBOX, *DAQ_MBOX);
	vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_RT_SLEEP,1);
	ret = rt_sleep_ns(diff2*DAQ_PERIOD);
	vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_RT_SLEEP,0);
	vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_DAQ_MBOX, *DAQ_MBOX);
	if (ret)
	  LOG_D(HW,"eNB Frame %d, time %llu: rt_sleep_ns returned %d\n",frame, time_in);
	  
	hw_slot = (((((volatile unsigned int *)DAQ_MBOX)[0]+1)%150)<<1)/15;
	//LOG_D(HW,"eNB Frame %d : hw_slot %d, time %llu\n",frame,hw_slot,rt_get_time_ns());
	delay_cnt++;
	if (delay_cnt == 30)  {
	  LOG_D(HW,"UE frame %d: HW stopped ... \n",frame);
	  exit_fun("[HW][UE] HW stopped");
	}
	mbox_current = ((volatile unsigned int *)DAQ_MBOX)[0];
	if ((mbox_current>=135) && (mbox_target<15)) //handle the frame wrap-arround
	  diff2 = 150-mbox_current+mbox_target;
	else if ((mbox_current<15) && (mbox_target>=135))
	  diff2 = -150+mbox_target-mbox_current;
	else
	  diff2 = mbox_target - mbox_current;
	  
	vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_DAQ_MBOX, *DAQ_MBOX);
	vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLES_DIFF, diff2);
      }
	
    }
      
    last_slot = (slot)%LTE_SLOTS_PER_FRAME;
    if (last_slot <0)
      last_slot+=LTE_SLOTS_PER_FRAME;
    next_slot = (slot+3)%LTE_SLOTS_PER_FRAME;

    PHY_vars_UE_g[0][0]->slot_rx = last_slot;
    PHY_vars_UE_g[0][0]->slot_tx = next_slot;
    if (PHY_vars_UE_g[0][0]->slot_rx==20) 
      PHY_vars_UE_g[0][0]->frame_rx++;
    if (PHY_vars_UE_g[0][0]->slot_tx==20) 
      PHY_vars_UE_g[0][0]->frame_tx++;

      
    if (is_synchronized)  {
      phy_procedures_UE_lte (PHY_vars_UE_g[0][0], 0, 0, mode, 0, NULL);
	
    }
    else {  // we are not yet synchronized
      hw_slot_offset = 0;
	
      slot = 0;
      openair0_get_frame(0);
      //          LOG_D(HW,"after get_frame\n");
      //          rt_sleep_ns(FRAME_PERIOD);
      //          LOG_D(HW,"after sleep\n");
	
      if (initial_sync(PHY_vars_UE_g[0][0],mode)==0) {
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
	
	/*if (mode == rx_calib_ue) {
	  exit_fun("[HW][UE] UE in RX calibration mode");
	}
	else {*/
	  is_synchronized = 1;
	  //start the DMA transfers
	  //LOG_D(HW,"Before openair0_start_rt_acquisition \n");
	  for (card=0;card<openair0_num_detected_cards;card++)
	    openair0_start_rt_acquisition(card);
	    
	  hw_slot_offset = (PHY_vars_UE_g[0][0]->rx_offset<<1) / PHY_vars_UE_g[0][0]->lte_frame_parms.samples_per_tti;
	  //LOG_D(HW,"Got synch: hw_slot_offset %d\n",hw_slot_offset);
	  oai_exit=1;
	  /*}*/
      }
      else {
	if (openair_daq_vars.freq_offset >= 0) {
	  openair_daq_vars.freq_offset += 100;
	  openair_daq_vars.freq_offset *= -1;
	}
	else {
	  openair_daq_vars.freq_offset *= -1;
	}
	if (abs(openair_daq_vars.freq_offset) > 7500) {
	  LOG_I(PHY,"[initial_sync] No cell synchronization found, abondoning\n");
	  mac_xface->macphy_exit("No cell synchronization found, abondoning");
	}
	else {
	  //	  LOG_I(PHY,"[initial_sync] trying carrier off %d Hz\n",openair_daq_vars.freq_offset);
#ifndef USRP
	  for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++) {
	    for (i=0; i<openair0_cfg[rf_map[CC_id].card].rx_num_channels; i++) 
	      openair0_cfg[rf_map[CC_id].card].rx_freq[rf_map[CC_id].chain+i] = downlink_frequency[CC_id][i]+openair_daq_vars.freq_offset;
	    for (i=0; i<openair0_cfg[rf_map[CC_id].card].tx_num_channels; i++) 
	      openair0_cfg[rf_map[CC_id].card].tx_freq[rf_map[CC_id].chain+i] = downlink_frequency[CC_id][i]+openair_daq_vars.freq_offset;
	  }
	  openair0_config(&openair0_cfg[0],UE_flag);
#endif
	  rt_sleep_ns(FRAME_PERIOD);
	}
      }
    }
    
    /*
      if ((slot%2000)<10)
      LOG_D(HW,"fun0: doing very hard work\n");
    */
    slot++;
    if (slot==20) {
      slot=0;
      frame++;
    }
#if defined(ENABLE_ITTI)
    itti_update_lte_time(frame, slot);
#endif
  }
  LOG_D(HW,"UE_thread: finished, ran %d times.\n",frame);

#ifdef HARD_RT
  rt_make_soft_real_time();
#endif

  // clean task
#ifdef RTAI
  rt_task_delete(task);
#endif
  LOG_D(HW,"Task deleted. returning\n");
  return 0;
}

#else  // This is for USRP or ETHERNET targets

#endif


void init_UE_threads(void) {
  
  PHY_VARS_UE *UE=PHY_vars_UE_g[0][0];

  pthread_attr_init(&attr_UE_thread_tx);
  pthread_attr_setstacksize(&attr_UE_thread_tx,OPENAIR_THREAD_STACK_SIZE);
  sched_param_UE_thread_tx.sched_priority = sched_get_priority_max(SCHED_FIFO)-1;
  pthread_attr_setschedparam  (&attr_UE_thread_tx, &sched_param_UE_thread_tx);
  pthread_attr_setschedpolicy (&attr_UE_thread_tx, SCHED_FIFO);

  pthread_attr_init(&attr_UE_thread_rx);
  pthread_attr_setstacksize(&attr_UE_thread_rx,OPENAIR_THREAD_STACK_SIZE);
  sched_param_UE_thread_rx.sched_priority = sched_get_priority_max(SCHED_FIFO)-1;
  pthread_attr_setschedparam  (&attr_UE_thread_rx, &sched_param_UE_thread_rx);
  pthread_attr_setschedpolicy (&attr_UE_thread_rx, SCHED_FIFO);

  UE->instance_cnt_tx=-1;
  UE->instance_cnt_rx=-1;
  UE->instance_cnt_synch=-1;
  pthread_mutex_init(&UE->mutex_tx,NULL);
  pthread_mutex_init(&UE->mutex_rx,NULL);
  pthread_mutex_init(&UE->mutex_synch,NULL);
  pthread_cond_init(&UE->cond_tx,NULL);
  pthread_cond_init(&UE->cond_rx,NULL);
  pthread_cond_init(&UE->cond_synch,NULL);
  pthread_create(&UE->thread_tx,NULL,UE_thread_tx,(void*)UE);
  pthread_create(&UE->thread_rx,NULL,UE_thread_rx,(void*)UE);
  pthread_create(&UE->thread_rx,NULL,UE_thread_synch,(void*)UE);
  UE->frame_tx = 0;

#ifndef EXMIMO
  UE->slot_tx = 2;
  UE->slot_rx = 0;
  UE->frame_rx = 0;
#else
  UE->slot_tx = 1;
  UE->slot_rx = 19;
  UE->frame_rx = 0;
#endif

  
  
}


static void get_options (int argc, char **argv) {
  int c;
  //  char                          line[1000];
  //  int                           l;
  int k;//i,j,k;
#ifdef USRP
  int clock_src;
#endif
  int CC_id;

  const Enb_properties_array_t *enb_properties;
  
  enum long_option_e {
    LONG_OPTION_START = 0x100, /* Start after regular single char options */
    
    LONG_OPTION_ULSCH_MAX_CONSECUTIVE_ERRORS,
    LONG_OPTION_CALIB_UE_RX,
    LONG_OPTION_CALIB_UE_RX_MED,
    LONG_OPTION_CALIB_UE_RX_BYP,
    
    LONG_OPTION_DEBUG_UE_PRACH,
    
    LONG_OPTION_NO_L2_CONNECT,
  };
  
  static const struct option long_options[] = {
    {"ulsch-max-errors",required_argument,  NULL, LONG_OPTION_ULSCH_MAX_CONSECUTIVE_ERRORS},
    {"calib-ue-rx",     required_argument,  NULL, LONG_OPTION_CALIB_UE_RX},
    {"calib-ue-rx-med", required_argument,  NULL, LONG_OPTION_CALIB_UE_RX_MED},
    {"calib-ue-rx-byp", required_argument,  NULL, LONG_OPTION_CALIB_UE_RX_BYP},
    {"debug-ue-prach",  no_argument,        NULL, LONG_OPTION_DEBUG_UE_PRACH},
    {"no-L2-connect",   no_argument,        NULL, LONG_OPTION_NO_L2_CONNECT},
    {NULL, 0, NULL, 0}};
  
  while ((c = getopt_long (argc, argv, "C:dK:g:G:qO:m:SUVRMr:s:t:",long_options,NULL)) != -1) {
    switch (c) {
    case LONG_OPTION_ULSCH_MAX_CONSECUTIVE_ERRORS:
      ULSCH_max_consecutive_errors = atoi(optarg);
      printf("Set ULSCH_max_consecutive_errors = %d\n",ULSCH_max_consecutive_errors);
      break;
      
    case LONG_OPTION_CALIB_UE_RX:
      mode = rx_calib_ue;
      rx_input_level_dBm = atoi(optarg);
      printf("Running with UE calibration on (LNA max), input level %d dBm\n",rx_input_level_dBm);
      break;
      
    case LONG_OPTION_CALIB_UE_RX_MED:
      mode = rx_calib_ue_med;
      rx_input_level_dBm = atoi(optarg);
      printf("Running with UE calibration on (LNA med), input level %d dBm\n",rx_input_level_dBm);
      break;
      
    case LONG_OPTION_CALIB_UE_RX_BYP:
      mode = rx_calib_ue_byp;
      rx_input_level_dBm = atoi(optarg);
      printf("Running with UE calibration on (LNA byp), input level %d dBm\n",rx_input_level_dBm);
      break;
      
    case LONG_OPTION_DEBUG_UE_PRACH:
      mode = debug_prach;
      break;
      
    case LONG_OPTION_NO_L2_CONNECT:
      mode = no_L2_connect;
      break;
    case 'M':
      multi_thread=0;
      break;
    case 'C':
      for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++) {
	downlink_frequency[CC_id][0] = atof(optarg); // Use float to avoid issue with frequency over 2^31.
	downlink_frequency[CC_id][1] = downlink_frequency[CC_id][0];
	downlink_frequency[CC_id][2] = downlink_frequency[CC_id][0];
	downlink_frequency[CC_id][3] = downlink_frequency[CC_id][0];
	printf("Downlink for CC_id %d frequency set to %u\n", CC_id, downlink_frequency[CC_id][0]);
      }
      break;
      
    case 'd':
#ifdef XFORMS
      do_forms=1;
      printf("Running with XFORMS!\n");
#endif
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
      
    case 'U':
      UE_flag = 1;
      break;
    
    case 'm':
      target_dl_mcs = atoi (optarg);
      break;
    case 't':
      target_ul_mcs = atoi (optarg);
      break;
      
    case 'V':
      ouput_vcd = 1;
      break;
    case  'q':
      opp_enabled = 1;
      break;
    case  'R' :
      online_log_messages =1;
      break;
    case 'r':
      for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++) {
	switch(atoi(optarg)) {
	case 6:
	  frame_parms[CC_id]->N_RB_DL=6;
	  frame_parms[CC_id]->N_RB_UL=6;
	  break;
	case 25:
	  frame_parms[CC_id]->N_RB_DL=25;
	  frame_parms[CC_id]->N_RB_UL=25;
	  break;
	case 50:
	  frame_parms[CC_id]->N_RB_DL=50;
	  frame_parms[CC_id]->N_RB_UL=50;
	  break;
	case 100:
	  frame_parms[CC_id]->N_RB_DL=100;
	  frame_parms[CC_id]->N_RB_UL=100;
	  break;
	default:
	  printf("Unknown N_RB_DL %d, switching to 25\n",atoi(optarg));
	  break;
	}
      }
      break;
    case 's':
#ifdef USRP

      clock_src = atoi(optarg);
      if (clock_src == 0) {
	//	char ref[128] = "internal";
	//strncpy(uhd_ref, ref, strlen(ref)+1);
      }
      else if (clock_src == 1) {
	//char ref[128] = "external";
	//strncpy(uhd_ref, ref, strlen(ref)+1);
      }
#else
      printf("Note: -s not defined for ExpressMIMO2\n");
#endif
      break;
    case 'g':
      glog_level=atoi(optarg); // value between 1 - 9
      break;
    case 'G':
      glog_verbosity=atoi(optarg);// value from 0, 0x5, 0x15, 0x35, 0x75
      break;
    default:
      break;
    }
  }
  
  if ((UE_flag == 0) && (conf_config_file_name != NULL)) {
    int i;
    
    NB_eNB_INST = 1;
    
    /* Read eNB configuration file */
    enb_properties = enb_config_init(conf_config_file_name);
    
    AssertFatal (NB_eNB_INST <= enb_properties->number,
		 "Number of eNB is greater than eNB defined in configuration file %s (%d/%d)!",
		 conf_config_file_name, NB_eNB_INST, enb_properties->number);

    /* Update some simulation parameters */
    for (i=0; i < enb_properties->number; i++) {
      AssertFatal (MAX_NUM_CCs == enb_properties->properties[i]->nb_cc,
		   "lte-softmodem compiled with MAX_NUM_CCs=%d, but only %d CCs configured for eNB %d!",
		   MAX_NUM_CCs, enb_properties->properties[i]->nb_cc, i);
      
      for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++) {
	frame_parms[CC_id]->frame_type =       enb_properties->properties[i]->frame_type[CC_id];
	frame_parms[CC_id]->tdd_config =       enb_properties->properties[i]->tdd_config[CC_id];
	frame_parms[CC_id]->tdd_config_S =     enb_properties->properties[i]->tdd_config_s[CC_id];
	frame_parms[CC_id]->Ncp =              enb_properties->properties[i]->prefix_type[CC_id];
	
	//for (j=0; j < enb_properties->properties[i]->nb_cc; j++ ){
	frame_parms[CC_id]->Nid_cell          =  enb_properties->properties[i]->Nid_cell[CC_id];
	frame_parms[CC_id]->N_RB_DL          =  enb_properties->properties[i]->N_RB_DL[CC_id];
	//} // j
      }

      glog_level                     = enb_properties->properties[i]->glog_level;
      glog_verbosity                 = enb_properties->properties[i]->glog_verbosity;
      hw_log_level                   = enb_properties->properties[i]->hw_log_level;
      hw_log_verbosity               = enb_properties->properties[i]->hw_log_verbosity ;
      phy_log_level                  = enb_properties->properties[i]->phy_log_level;
      phy_log_verbosity              = enb_properties->properties[i]->phy_log_verbosity;
      mac_log_level                  = enb_properties->properties[i]->mac_log_level;
      mac_log_verbosity              = enb_properties->properties[i]->mac_log_verbosity;
      rlc_log_level                  = enb_properties->properties[i]->rlc_log_level;
      rlc_log_verbosity              = enb_properties->properties[i]->rlc_log_verbosity;
      pdcp_log_level                 = enb_properties->properties[i]->pdcp_log_level;
      pdcp_log_verbosity             = enb_properties->properties[i]->pdcp_log_verbosity;
      rrc_log_level                  = enb_properties->properties[i]->rrc_log_level;
      rrc_log_verbosity              = enb_properties->properties[i]->rrc_log_verbosity;
    
    
      // adjust the log
      for (CC_id=0; CC_id<MAX_NUM_CCs; CC_id++) {
	for (k = 0 ; k < 4; k++) {
	  downlink_frequency[CC_id][k]      =       enb_properties->properties[i]->downlink_frequency[CC_id];
	  uplink_frequency_offset[CC_id][k] =  enb_properties->properties[i]->uplink_frequency_offset[CC_id];
	  rx_gain[CC_id][k]                 =  (double)enb_properties->properties[i]->rx_gain[CC_id];
	  tx_gain[CC_id][k]                 =  (double)enb_properties->properties[i]->tx_gain[CC_id];
	}
	printf("Downlink frequency/ uplink offset of CC_id %d set to %llu/%d\n", CC_id,
	       enb_properties->properties[i]->downlink_frequency[CC_id],
	       enb_properties->properties[i]->uplink_frequency_offset[CC_id]);
      } // CC_id
    }// i
  }
}

int main(int argc, char **argv) {
#ifdef RTAI
  // RT_TASK *task;
#else
  int *eNB_thread_status_p;
  //  int *eNB_thread_status_rx[10],*eNB_thread_status_tx[10];
#endif
  int i,j,aa,card;
#if defined (XFORMS) || defined (EMOS) || (! defined (RTAI)) || defined (SPECTRA)
  void *status;
#endif
  
  int CC_id;
  uint16_t Nid_cell = 0;
  uint8_t  cooperation_flag=0, transmission_mode=1, abstraction_flag=0;
#ifndef OPENAIR2
  uint8_t beta_ACK=0,beta_RI=0,beta_CQI=2;
#endif

#ifdef ENABLE_TCXO
  unsigned int tcxo = 114;
#endif

  //  int amp;
  // uint8_t prach_fmt;
  // int N_ZC;

  //  int ret, ant;
  int ant_offset=0;
#if defined (XFORMS) || defined (SPECTRA)
  int ret;
#endif
#if defined (EMOS) || (! defined (RTAI))
  int error_code;
#endif

  memset(&openair0_cfg[0],0,sizeof(openair0_config_t)*MAX_CARDS);

  set_latency_target();

  mode = normal_txrx;

  for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++) {
    frame_parms[CC_id] = (LTE_DL_FRAME_PARMS*) malloc(sizeof(LTE_DL_FRAME_PARMS));
    /* Set some default values that may be overwritten while reading options */
    frame_parms[CC_id]->frame_type         = FDD; /* TDD */
    frame_parms[CC_id]->tdd_config          = 3;
    frame_parms[CC_id]->tdd_config_S        = 0;
    frame_parms[CC_id]->N_RB_DL             = 25;
    frame_parms[CC_id]->N_RB_UL             = 25;
    frame_parms[CC_id]->Ncp                 = NORMAL;
    frame_parms[CC_id]->Ncp_UL              = NORMAL;
    frame_parms[CC_id]->Nid_cell            = Nid_cell;
    frame_parms[CC_id]->num_MBSFN_config    = 0;
  }

  get_options (argc, argv); //Command-line options

  //randominit (0);
  set_taus_seed (0);

  // initialize the log (see log.h for details)
  logInit();

  set_glog(glog_level, glog_verbosity);
  if (UE_flag==1)
    {
      printf("configuring for UE\n");

      set_comp_log(HW,      LOG_DEBUG,  LOG_HIGH, 1);
#ifdef OPENAIR2
      set_comp_log(PHY,     LOG_DEBUG,   LOG_HIGH, 1);
#else
      set_comp_log(PHY,     LOG_INFO,   LOG_HIGH, 1);
#endif
      set_comp_log(MAC,     LOG_INFO,   LOG_HIGH, 1);
      set_comp_log(RLC,     LOG_INFO,   LOG_HIGH, 1);
      set_comp_log(PDCP,    LOG_INFO,   LOG_HIGH, 1);
      set_comp_log(OTG,     LOG_INFO,   LOG_HIGH, 1);
      set_comp_log(RRC,     LOG_INFO,   LOG_HIGH, 1);
#if defined(ENABLE_ITTI)
      set_comp_log(EMU,     LOG_INFO,   LOG_MED, 1);
# if defined(ENABLE_USE_MME)
      set_comp_log(NAS,     LOG_INFO,   LOG_HIGH, 1);
# endif
#endif
    }
  else
    {
      printf("configuring for eNB\n");

      set_comp_log(HW,      hw_log_level, hw_log_verbosity, 1);
#ifdef OPENAIR2
      set_comp_log(PHY,     phy_log_level,   phy_log_verbosity, 1);
#else
      set_comp_log(PHY,     LOG_INFO,   LOG_HIGH, 1);
#endif
      set_comp_log(MAC,     mac_log_level,  mac_log_verbosity, 1);
      set_comp_log(RLC,     rlc_log_level,   rlc_log_verbosity, 1);
      set_comp_log(PDCP,    pdcp_log_level,  pdcp_log_verbosity, 1);
      set_comp_log(RRC,     rrc_log_level,  rrc_log_verbosity, 1);
#if defined(ENABLE_ITTI)
      set_comp_log(EMU,     LOG_INFO,   LOG_MED, 1);
# if defined(ENABLE_USE_MME)
      set_comp_log(UDP_,    LOG_DEBUG,   LOG_HIGH, 1);
      set_comp_log(GTPU,    LOG_DEBUG,   LOG_HIGH, 1);
      set_comp_log(S1AP,    LOG_DEBUG,   LOG_HIGH, 1);
      set_comp_log(SCTP,    LOG_INFO,   LOG_HIGH, 1);
# endif
#if defined(ENABLE_SECURITY)
      set_comp_log(OSA,    LOG_DEBUG,   LOG_HIGH, 1);
#endif
#endif
      set_comp_log(ENB_APP, LOG_INFO, LOG_HIGH, 1);
      set_comp_log(OTG,     LOG_INFO,   LOG_HIGH, 1);
      if (online_log_messages == 1) { 
	set_component_filelog(RRC);
	set_component_filelog(PDCP);
      }
    }

  if (ouput_vcd) {
    if (UE_flag==1)
      vcd_signal_dumper_init("/tmp/openair_dump_UE.vcd");
    else
      vcd_signal_dumper_init("/tmp/openair_dump_eNB.vcd");
  }

#if defined(ENABLE_ITTI)
  if (UE_flag == 1) {
    log_set_instance_type (LOG_INSTANCE_UE);
  }
  else {
    log_set_instance_type (LOG_INSTANCE_ENB);
  }

  itti_init(TASK_MAX, THREAD_MAX, MESSAGES_ID_MAX, tasks_info, messages_info, messages_definition_xml, itti_dump_file);
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

  // init the parameters
  for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++) {
    frame_parms[CC_id]->nushift            = 0;
    if (UE_flag==0)
      {
	switch (transmission_mode) {
	case 1: 
	  frame_parms[CC_id]->nb_antennas_tx     = 1;
	  frame_parms[CC_id]->nb_antennas_rx     = 1;
	  break;
	case 2:
	case 5:
	case 6:
	  frame_parms[CC_id]->nb_antennas_tx     = 2;
	  frame_parms[CC_id]->nb_antennas_rx     = 2;
	  break;
	default:
	  printf("Unsupported transmission mode %d\n",transmission_mode);
	  exit(-1);
	}
      }
    else
      { //UE_flag==1
	frame_parms[CC_id]->nb_antennas_tx     = 1;
	frame_parms[CC_id]->nb_antennas_rx     = 1;
      }
    frame_parms[CC_id]->nb_antennas_tx_eNB = (transmission_mode == 1) ? 1 : 2; //initial value overwritten by initial sync later
    frame_parms[CC_id]->mode1_flag         = (transmission_mode == 1) ? 1 : 0;
    frame_parms[CC_id]->phich_config_common.phich_resource = oneSixth;
    frame_parms[CC_id]->phich_config_common.phich_duration = normal;
    // UL RS Config
    frame_parms[CC_id]->pusch_config_common.ul_ReferenceSignalsPUSCH.cyclicShift = 0;//n_DMRS1 set to 0
    frame_parms[CC_id]->pusch_config_common.ul_ReferenceSignalsPUSCH.groupHoppingEnabled = 0;
    frame_parms[CC_id]->pusch_config_common.ul_ReferenceSignalsPUSCH.sequenceHoppingEnabled = 0;
    frame_parms[CC_id]->pusch_config_common.ul_ReferenceSignalsPUSCH.groupAssignmentPUSCH = 0;
    init_ul_hopping(frame_parms[CC_id]);
    init_frame_parms(frame_parms[CC_id],1);
    phy_init_top(frame_parms[CC_id]);
  }

  phy_init_lte_top(frame_parms[0]);

  for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++) {
    //init prach for openair1 test
    frame_parms[CC_id]->prach_config_common.rootSequenceIndex=22; 
    frame_parms[CC_id]->prach_config_common.prach_ConfigInfo.zeroCorrelationZoneConfig=1;
    frame_parms[CC_id]->prach_config_common.prach_ConfigInfo.prach_ConfigIndex=0; 
    frame_parms[CC_id]->prach_config_common.prach_ConfigInfo.highSpeedFlag=0;
    frame_parms[CC_id]->prach_config_common.prach_ConfigInfo.prach_FreqOffset=0;
    // prach_fmt = get_prach_fmt(frame_parms->prach_config_common.prach_ConfigInfo.prach_ConfigIndex, frame_parms->frame_type);
    // N_ZC = (prach_fmt <4)?839:139;
  }

  if (UE_flag==1) {
    NB_UE_INST=1;
    NB_INST=1;

    PHY_vars_UE_g = malloc(sizeof(PHY_VARS_UE**));
    PHY_vars_UE_g[0] = malloc(sizeof(PHY_VARS_UE*)*MAX_NUM_CCs);
    for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++) {
      PHY_vars_UE_g[0][CC_id] = init_lte_UE(frame_parms[CC_id], UE_id,abstraction_flag,transmission_mode);
      printf("PHY_vars_UE_g[0][%d] = %p\n",CC_id,PHY_vars_UE_g[0][CC_id]);
#ifndef OPENAIR2
      for (i=0;i<NUMBER_OF_eNB_MAX;i++) {
	PHY_vars_UE_g[0][CC_id]->pusch_config_dedicated[i].betaOffset_ACK_Index = beta_ACK;
	PHY_vars_UE_g[0][CC_id]->pusch_config_dedicated[i].betaOffset_RI_Index  = beta_RI;
	PHY_vars_UE_g[0][CC_id]->pusch_config_dedicated[i].betaOffset_CQI_Index = beta_CQI;
	
	PHY_vars_UE_g[0][CC_id]->scheduling_request_config[i].sr_PUCCH_ResourceIndex = UE_id;
	PHY_vars_UE_g[0][CC_id]->scheduling_request_config[i].sr_ConfigIndex = 7+(UE_id%3);
	PHY_vars_UE_g[0][CC_id]->scheduling_request_config[i].dsr_TransMax = sr_n4;
      }
#endif
      
      compute_prach_seq(&PHY_vars_UE_g[0][CC_id]->lte_frame_parms.prach_config_common,
			PHY_vars_UE_g[0][CC_id]->lte_frame_parms.frame_type,
			PHY_vars_UE_g[0][CC_id]->X_u);
      
      PHY_vars_UE_g[0][CC_id]->lte_ue_pdcch_vars[0]->crnti = 0x1234;
#ifndef OPENAIR2
      PHY_vars_UE_g[0][CC_id]->lte_ue_pdcch_vars[0]->crnti = 0x1235;
#endif
    
#ifdef EXMIMO
      for (i=0;i<4;i++) {
	PHY_vars_UE_g[0][CC_id]->rx_gain_max[i] = rxg_max[i];
	PHY_vars_UE_g[0][CC_id]->rx_gain_med[i] = rxg_med[i];
	PHY_vars_UE_g[0][CC_id]->rx_gain_byp[i] = rxg_byp[i];
      }

      if ((mode == normal_txrx) || (mode == rx_calib_ue) || (mode == no_L2_connect) || (mode == debug_prach)) {
	for (i=0;i<4;i++)
	  rx_gain_mode[CC_id][i] = max_gain;
	PHY_vars_UE_g[0][CC_id]->rx_total_gain_dB =  PHY_vars_UE_g[0][CC_id]->rx_gain_max[0] + (int)rx_gain[CC_id][0] - 30; //-30 because it was calibrated with a 30dB gain
      }
      else if ((mode == rx_calib_ue_med)) {
	for (i=0;i<4;i++)
	  rx_gain_mode[CC_id][i] =  med_gain;
	PHY_vars_UE_g[0][CC_id]->rx_total_gain_dB =  PHY_vars_UE_g[0][CC_id]->rx_gain_med[0]  + (int)rx_gain[CC_id][0] - 30; //-30 because it was calibrated with a 30dB gain;
      }
      else if ((mode == rx_calib_ue_byp)) {
	for (i=0;i<4;i++)
	  rx_gain_mode[CC_id][i] =  byp_gain;
	PHY_vars_UE_g[0][CC_id]->rx_total_gain_dB =  PHY_vars_UE_g[0][CC_id]->rx_gain_byp[0]  + (int)rx_gain[CC_id][0] - 30; //-30 because it was calibrated with a 30dB gain;
      }
#else
      PHY_vars_UE_g[0][CC_id]->rx_total_gain_dB =  (int)rx_gain[CC_id][0]; 
#endif
    
      PHY_vars_UE_g[0][CC_id]->tx_power_max_dBm = tx_max_power[CC_id];
    

   
#ifdef EXMIMO
      //N_TA_offset
      if (PHY_vars_UE_g[UE_id][CC_id]->lte_frame_parms.frame_type == TDD) {
	if (PHY_vars_UE_g[UE_id][CC_id]->lte_frame_parms.N_RB_DL == 100)
	  PHY_vars_UE_g[UE_id][CC_id]->N_TA_offset = 624;
	else if (PHY_vars_UE_g[UE_id][CC_id]->lte_frame_parms.N_RB_DL == 50)
	  PHY_vars_UE_g[UE_id][CC_id]->N_TA_offset = 624/2;
	else if (PHY_vars_UE_g[UE_id][CC_id]->lte_frame_parms.N_RB_DL == 25)
	  PHY_vars_UE_g[UE_id][CC_id]->N_TA_offset = 624/4;
      }
      else {
	PHY_vars_UE_g[UE_id][CC_id]->N_TA_offset = 0;
      }
#else
      //already taken care of in lte-softmodem
      PHY_vars_UE_g[UE_id][CC_id]->N_TA_offset = 0;
#endif 
    }
    openair_daq_vars.manual_timing_advance = 0;
    openair_daq_vars.rx_gain_mode = DAQ_AGC_ON;
    openair_daq_vars.auto_freq_correction = 0;
    openair_daq_vars.use_ia_receiver = 0;
    
  
    
    //  printf("tx_max_power = %d -> amp %d\n",tx_max_power,get_tx_amp(tx_max_power,tx_max_power));
  }
  else
    { //this is eNB
      PHY_vars_eNB_g = malloc(sizeof(PHY_VARS_eNB**));
      PHY_vars_eNB_g[0] = malloc(sizeof(PHY_VARS_eNB*));
      for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++) {
	PHY_vars_eNB_g[0][CC_id] = init_lte_eNB(frame_parms[CC_id],eNB_id,Nid_cell,cooperation_flag,transmission_mode,abstraction_flag);
	PHY_vars_eNB_g[0][CC_id]->CC_id = CC_id;
       
#ifndef OPENAIR2
	for (i=0;i<NUMBER_OF_UE_MAX;i++) {
	  PHY_vars_eNB_g[0][CC_id]->pusch_config_dedicated[i].betaOffset_ACK_Index = beta_ACK;
	  PHY_vars_eNB_g[0][CC_id]->pusch_config_dedicated[i].betaOffset_RI_Index  = beta_RI;
	  PHY_vars_eNB_g[0][CC_id]->pusch_config_dedicated[i].betaOffset_CQI_Index = beta_CQI;
	  
	  PHY_vars_eNB_g[0][CC_id]->scheduling_request_config[i].sr_PUCCH_ResourceIndex = i;
	  PHY_vars_eNB_g[0][CC_id]->scheduling_request_config[i].sr_ConfigIndex = 7+(i%3);
	  PHY_vars_eNB_g[0][CC_id]->scheduling_request_config[i].dsr_TransMax = sr_n4;
	}
#endif
      
	compute_prach_seq(&PHY_vars_eNB_g[0][CC_id]->lte_frame_parms.prach_config_common,
			  PHY_vars_eNB_g[0][CC_id]->lte_frame_parms.frame_type,
			  PHY_vars_eNB_g[0][CC_id]->X_u);

#ifndef EXMIMO

	PHY_vars_eNB_g[0][CC_id]->rx_total_gain_eNB_dB = (int)rx_gain[CC_id][0];

#else
	PHY_vars_eNB_g[0][CC_id]->rx_total_gain_eNB_dB =  rxg_max[0] + (int)rx_gain[CC_id][0] - 30; //was measured at rxgain=30;

	printf("Setting RX total gain to %d\n",PHY_vars_eNB_g[0][CC_id]->rx_total_gain_eNB_dB);

	// set eNB to max gain
	for (i=0;i<4;i++)
	  rx_gain_mode[CC_id][i] = max_gain;
#endif

#ifdef EXMIMO
	//N_TA_offset
	if (PHY_vars_eNB_g[eNB_id][CC_id]->lte_frame_parms.frame_type == TDD) {
	  if (PHY_vars_eNB_g[eNB_id][CC_id]->lte_frame_parms.N_RB_DL == 100)
	    PHY_vars_eNB_g[eNB_id][CC_id]->N_TA_offset = 624;
	  else if (PHY_vars_eNB_g[eNB_id][CC_id]->lte_frame_parms.N_RB_DL == 50)
	    PHY_vars_eNB_g[eNB_id][CC_id]->N_TA_offset = 624/2;
	  else if (PHY_vars_eNB_g[eNB_id][CC_id]->lte_frame_parms.N_RB_DL == 25)
	    PHY_vars_eNB_g[eNB_id][CC_id]->N_TA_offset = 624/4;
	}
	else {
	  PHY_vars_eNB_g[eNB_id][CC_id]->N_TA_offset = 0;
	}
#else
	//already taken care of in lte-softmodem
	PHY_vars_eNB_g[eNB_id][CC_id]->N_TA_offset = 0;
#endif 
	
      }


      NB_eNB_INST=1;
      NB_INST=1;

      openair_daq_vars.ue_dl_rb_alloc=0x1fff;
      openair_daq_vars.target_ue_dl_mcs=target_dl_mcs;
      openair_daq_vars.ue_ul_nb_rb=6;
      openair_daq_vars.target_ue_ul_mcs=target_ul_mcs;

    }



  dump_frame_parms(frame_parms[0]);

  if(frame_parms[0]->N_RB_DL == 100) {
    sample_rate = 30.72e6;
#ifndef EXMIMO
    samples_per_packets = 2048;
    samples_per_frame = 307200;
    // from usrp_time_offset
    tx_forward_nsamps = 175;
    sf_bounds = sf_bounds_20;
    sf_bounds_tx = sf_bounds_20_tx;
    max_cnt = 150;
    tx_delay = 8;
#endif
  }
  else if(frame_parms[0]->N_RB_DL == 50){
    sample_rate = 15.36e6;
#ifndef EXMIMO
    samples_per_packets = 2048;
    samples_per_frame = 153600;
    tx_forward_nsamps = 95;
    sf_bounds = sf_bounds_10;
    sf_bounds_tx = sf_bounds_10_tx;
    max_cnt = 75;
    tx_delay = 4;
#endif
  }
  else if (frame_parms[0]->N_RB_DL == 25) {
    sample_rate = 7.68e6;
#ifndef EXMIMO
    samples_per_packets = 1024;
    samples_per_frame = 76800;
    tx_forward_nsamps = 70;
    sf_bounds = sf_bounds_5;
    sf_bounds_tx = sf_bounds_5_tx;
    max_cnt = 75;
    tx_delay = 5;
#endif
  }
  

  for (card=0;card<MAX_CARDS;card++) {
    openair0_cfg[card].sample_rate = sample_rate;
    openair0_cfg[card].tx_bw = bw;
    openair0_cfg[card].rx_bw = bw;
    // in the case of the USRP, the following variables need to be initialized before the init
    // since the USRP only supports one CC (for the moment), we initialize all the cards with first CC. 
    // in the case of EXMIMO2, these values are overwirtten in the function setup_eNB/UE_buffer
#ifndef EXMIMO
    openair0_cfg[card].tx_num_channels=1;
    openair0_cfg[card].rx_num_channels=1;
    for (i=0;i<4;i++) {
      openair0_cfg[card].tx_gain[i] = tx_gain[0][i];
      openair0_cfg[card].rx_gain[i] = ((UE_flag==0) ? PHY_vars_eNB_g[0][0]->rx_total_gain_eNB_dB : 
				                      PHY_vars_UE_g[0][0]->rx_total_gain_dB) - 73;  // calibrated for USRP B210 @ 2.6 GHz
      openair0_cfg[card].tx_freq[i] = (UE_flag==0) ? downlink_frequency[0][i] : downlink_frequency[0][i]+uplink_frequency_offset[0][i];
      openair0_cfg[card].rx_freq[i] = (UE_flag==0) ? downlink_frequency[0][i] + uplink_frequency_offset[0][i] : downlink_frequency[0][i];
    }
#endif
  }

  if (openair0_device_init(&openair0, &openair0_cfg[0]) <0) {
    printf("Exiting, cannot initialize device\n");
    exit(-1);
  }

  mac_xface = malloc(sizeof(MAC_xface));

#ifdef OPENAIR2
  int eMBMS_active=0;

  l2_init(frame_parms[0],eMBMS_active,
	  0,// cba_group_active
	  0); // HO flag
  if (UE_flag == 1)
    mac_xface->dl_phy_sync_success (0, 0, 0, 1);
  else
    mac_xface->mrbch_phy_sync_failure (0, 0, 0);
#endif

  mac_xface->macphy_exit = &exit_fun;

#if defined(ENABLE_ITTI)
  if (create_tasks(UE_flag ? 0 : 1, UE_flag ? 1 : 0) < 0) {
    printf("cannot create ITTI tasks\n");
    exit(-1); // need a softer mode
  }
  printf("ITTI tasks created\n");
#endif

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
  init_predef_traffic(UE_flag ? 1 : 0, UE_flag ? 0 : 1);
  //  }
#endif

  //  number_of_cards = openair0_num_detected_cards;

  openair_daq_vars.timing_advance = 0;

  for(CC_id=0;CC_id<MAX_NUM_CCs;CC_id++) {
    rf_map[CC_id].card=0;
    rf_map[CC_id].chain=CC_id+1;
  }

  // connect the TX/RX buffers
  if (UE_flag==1) {
    if (setup_ue_buffers(PHY_vars_UE_g[0],&openair0_cfg[0],rf_map)!=0) {
      printf("Error setting up eNB buffer\n");
      exit(-1);
    }
    printf("Setting UE buffer to all-RX\n");
    // Set LSBs for antenna switch (ExpressMIMO)
    for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++) {
      for (i=0; i<frame_parms[CC_id]->samples_per_tti*10; i++)
	for (aa=0; aa<frame_parms[CC_id]->nb_antennas_tx; aa++)
	  PHY_vars_UE_g[0][CC_id]->lte_ue_common_vars.txdata[aa][i] = 0x00010001;
    }
    //p_exmimo_config->framing.tdd_config = TXRXSWITCH_TESTRX;
  }
  else {
    if (setup_eNB_buffers(PHY_vars_eNB_g[0],&openair0_cfg[0],rf_map)!=0) {
      printf("Error setting up eNB buffer\n");
      exit(-1);
    }
    printf("Setting eNB buffer to all-RX\n");
    // Set LSBs for antenna switch (ExpressMIMO)
    for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++) {
      for (i=0; i<frame_parms[CC_id]->samples_per_tti*10; i++)
	for (aa=0; aa<frame_parms[CC_id]->nb_antennas_tx; aa++)
	  PHY_vars_eNB_g[0][CC_id]->lte_eNB_common_vars.txdata[0][aa][i] = 0x00010001;
    }
  }
#ifdef EXMIMO
  openair0_config(&openair0_cfg[0],UE_flag);
#endif

  /*  
      for (ant=0;ant<4;ant++)
      p_exmimo_config->rf.do_autocal[ant] = 0;
  */

#ifdef EMOS
  error_code = rtf_create(CHANSOUNDER_FIFO_MINOR,CHANSOUNDER_FIFO_SIZE);
  if (error_code==0)
    printf("[OPENAIR][SCHED][INIT] Created EMOS FIFO %d\n",CHANSOUNDER_FIFO_MINOR);
  else if (error_code==ENODEV)
    printf("[OPENAIR][SCHED][INIT] Problem: EMOS FIFO %d is greater than or equal to RTF_NO\n",CHANSOUNDER_FIFO_MINOR);
  else if (error_code==ENOMEM)
    printf("[OPENAIR][SCHED][INIT] Problem: cannot allocate memory for EMOS FIFO %d\n",CHANSOUNDER_FIFO_MINOR);
  else 
    printf("[OPENAIR][SCHED][INIT] Problem creating EMOS FIFO %d, error_code %d\n",CHANSOUNDER_FIFO_MINOR,error_code);
#endif

  mlockall(MCL_CURRENT | MCL_FUTURE);

#ifdef RTAI
  // make main thread LXRT soft realtime
  /* task = */ rt_task_init_schmod(nam2num("MYTASK"), 9, 0, 0, SCHED_FIFO, 0xF);

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
#ifndef EXMIMO
  sync_sem = rt_typed_sem_init(nam2num("syncsem"), 0, BIN_SEM|FIFO_Q);
  if(sync_sem == 0)
    printf("error init sync semphore\n");
#endif
#else
#ifndef EXMIMO
  pthread_cond_init(&sync_cond,NULL);
  pthread_mutex_init(&sync_mutex, NULL);
#endif
#endif



  // this starts the DMA transfers
#ifdef EXMIMO
  if (UE_flag!=1)
    for (card=0;card<openair0_num_detected_cards;card++)
      openair0_start_rt_acquisition(card);
#endif

#ifdef XFORMS
  if (do_forms==1) {
    fl_initialize (&argc, argv, NULL, 0, 0);
    form_stats = create_form_stats_form();
    if (UE_flag==1) {
      form_ue[UE_id] = create_lte_phy_scope_ue();
      sprintf (title, "LTE DL SCOPE UE");
      fl_show_form (form_ue[UE_id]->lte_phy_scope_ue, FL_PLACE_HOTSPOT, FL_FULLBORDER, title);
    } else {
      form_stats_l2 = create_form_stats_form();
      for(UE_id=0;UE_id<scope_enb_num_ue;UE_id++) {
	form_enb[UE_id] = create_lte_phy_scope_enb();
	sprintf (title, "UE%d LTE UL SCOPE eNB",UE_id+1);
	fl_show_form (form_enb[UE_id]->lte_phy_scope_enb, FL_PLACE_HOTSPOT, FL_FULLBORDER, title);
      }
    }
    fl_show_form (form_stats->stats_form, FL_PLACE_HOTSPOT, FL_FULLBORDER, "stats");
    if (UE_flag==0) {
      fl_show_form (form_stats_l2->stats_form, FL_PLACE_HOTSPOT, FL_FULLBORDER, "l2 stats");

      for (UE_id=0;UE_id<scope_enb_num_ue;UE_id++) {
	if (otg_enabled) {
	  fl_set_button(form_enb[UE_id]->button_0,1);
	  fl_set_object_label(form_enb[UE_id]->button_0,"DL Traffic ON");
	}
	else {
	  fl_set_button(form_enb[UE_id]->button_0,0);
	  fl_set_object_label(form_enb[UE_id]->button_0,"DL Traffic OFF");
	}
      }
    }
    else {
      if (openair_daq_vars.use_ia_receiver) {
	fl_set_button(form_ue[UE_id]->button_0,1);
	fl_set_object_label(form_ue[UE_id]->button_0, "IA Receiver ON");
      }
      else {
	fl_set_button(form_ue[UE_id]->button_0,0);
	fl_set_object_label(form_ue[UE_id]->button_0, "IA Receiver OFF");
      }
    }

    ret = pthread_create(&forms_thread, NULL, scope_thread, NULL);
    printf("Scope thread created, ret=%d\n",ret);
  }
#endif

#ifdef EMOS
  ret = pthread_create(&thread3, NULL, emos_thread, NULL);
  printf("EMOS thread created, ret=%d\n",ret);
#endif

#ifdef SPECTRA
  ret = pthread_create(&sensing_thread, NULL, sensing, NULL);
  printf("sensing thread created, ret=%d\n",ret);
#endif

  rt_sleep_ns(10*FRAME_PERIOD);

#ifndef RTAI
  pthread_attr_init (&attr_dlsch_threads);
  pthread_attr_setstacksize(&attr_dlsch_threads,OPENAIR_THREAD_STACK_SIZE);
  sched_param_dlsch.sched_priority = 90;//sched_get_priority_max(SCHED_FIFO); //OPENAIR_THREAD_PRIORITY;
  pthread_attr_setschedparam  (&attr_dlsch_threads, &sched_param_dlsch);
  pthread_attr_setschedpolicy (&attr_dlsch_threads, SCHED_FIFO);

  pthread_attr_init (&attr_UE_init_synch);
  pthread_attr_setstacksize(&attr_UE_init_synch,OPENAIR_THREAD_STACK_SIZE);
  sched_param_UE_init_synch.sched_priority = 90;//sched_get_priority_max(SCHED_FIFO); //OPENAIR_THREAD_PRIORITY;
  pthread_attr_setschedparam  (&attr_UE_init_synch, &sched_param_UE_init_synch);
  pthread_attr_setschedpolicy (&attr_UE_init_synch, SCHED_FIFO);

#endif

  // start the main thread
  if (UE_flag == 1) {
    init_UE_threads();
    sleep(1);
#ifdef RTAI
    main_ue_thread = rt_thread_create(UE_thread, NULL, 100000000);
#else
    error_code = pthread_create(&main_ue_thread, &attr_dlsch_threads, UE_thread, NULL);
    if (error_code!= 0) {
      LOG_D(HW,"[lte-softmodem.c] Could not allocate UE_thread, error %d\n",error_code);
      return(error_code);
    }
    else {
      LOG_D(HW,"[lte-softmodem.c] Allocate UE_thread successful\n");
    }
#endif
#ifdef DLSCH_THREAD
    init_rx_pdsch_thread();
    rt_sleep_ns(FRAME_PERIOD/10);
    init_dlsch_threads();
#endif
    printf("UE threads created\n");
  }
  else {

    if (multi_thread>0) {
      init_eNB_proc();
      sleep(1);
      LOG_D(HW,"[lte-softmodem.c] eNB threads created\n");
    }
    printf("Creating main eNB_thread \n");
#ifdef RTAI
    main_eNB_thread = rt_thread_create(eNB_thread, NULL, OPENAIR_THREAD_STACK_SIZE);
#else
    error_code = pthread_create(&main_eNB_thread, &attr_dlsch_threads, eNB_thread, NULL);
    if (error_code!= 0) {
      LOG_D(HW,"[lte-softmodem.c] Could not allocate eNB_thread, error %d\n",error_code);
      return(error_code);
    }
    else {
      LOG_D(HW,"[lte-softmodem.c] Allocate eNB_thread successful\n");
    }
#endif
  }

  // Sleep to allow all threads to setup
  sleep(1);


#ifndef EXMIMO
#ifndef USRP_DEBUG
  openair0.trx_start_func(&openair0);
  //  printf("returning from usrp start streaming: %llu\n",get_usrp_time(&openair0));
#endif
#ifdef RTAI
  rt_sem_signal(sync_sem);
#else
  pthread_mutex_lock(&sync_mutex);
  printf("Sending sync ...\n");
  pthread_cond_broadcast(&sync_cond);
  pthread_mutex_unlock(&sync_mutex);
#endif
#endif
  // wait for end of program
  printf("TYPE <CTRL-C> TO TERMINATE\n");
  //getchar();

#if defined(ENABLE_ITTI)
  printf("Entering ITTI signals handler\n");
  itti_wait_tasks_end();
  oai_exit=1;
#else
  while (oai_exit==0)
    rt_sleep_ns(FRAME_PERIOD);
#endif

  // stop threads
#ifdef XFORMS
  printf("waiting for XFORMS thread\n");
  if (do_forms==1)
    {
      pthread_join(forms_thread,&status);
      fl_hide_form(form_stats->stats_form);
      fl_free_form(form_stats->stats_form);
      if (UE_flag==1) {
	fl_hide_form(form_ue[UE_id]->lte_phy_scope_ue);
	fl_free_form(form_ue[UE_id]->lte_phy_scope_ue);
      } else {
	fl_hide_form(form_stats_l2->stats_form);
	fl_free_form(form_stats_l2->stats_form);
	for(UE_id=0;UE_id<scope_enb_num_ue;UE_id++) {
	  fl_hide_form(form_enb[UE_id]->lte_phy_scope_enb);
	  fl_free_form(form_enb[UE_id]->lte_phy_scope_enb);
	}
      }
    }
#endif

  printf("stopping MODEM threads\n");
  // cleanup
  if (UE_flag == 1) {
#ifdef EXMIMO
#ifdef RTAI
    rt_thread_join(main_ue_thread); 
#else
    pthread_join(main_ue_thread,&status); 
#endif
#ifdef DLSCH_THREAD
    cleanup_dlsch_threads();
    cleanup_rx_pdsch_thread();
#endif
#endif
  }
  else {
#ifdef DEBUG_THREADS
    printf("Joining eNB_thread ...");
#endif
#ifdef RTAI
    rt_thread_join(main_eNB_thread); 
#else
    pthread_join(main_eNB_thread,(void**)&eNB_thread_status_p); 
#ifdef DEBUG_THREADS
    printf("status %d\n",*eNB_thread_status_p);
#endif
#endif

    if (multi_thread>0) {
      printf("Killing eNB processing threads\n");
      kill_eNB_proc();

    }
  }

#ifdef OPENAIR2
  //cleanup_pdcp_thread();
#endif

#ifndef EXMIMO
#ifdef RTAI
  rt_sem_delete(sync_sem);
  stop_rt_timer();
#else
  pthread_cond_destroy(&sync_cond);
  pthread_mutex_destroy(&sync_mutex);
#endif
#endif

#ifdef EXMIMO
  printf("stopping card\n");
  openair0_stop(0);
  printf("closing openair0_lib\n");
  openair0_close();
#endif

#ifdef EMOS
  printf("waiting for EMOS thread\n");
  pthread_cancel(thread3);
  pthread_join(thread3,&status);
#endif

#ifdef EMOS
  error_code = rtf_destroy(CHANSOUNDER_FIFO_MINOR);
  printf("[OPENAIR][SCHED][CLEANUP] EMOS FIFO closed, error_code %d\n", error_code);
#endif

#ifdef SPECTRA
  printf("waiting for sensing thread\n");
  pthread_cancel(sensing_thread);
  pthread_join(sensing_thread,&status);
#endif

  if (ouput_vcd)
    vcd_signal_dumper_close();

  logClean();

  return 0;
}

int setup_ue_buffers(PHY_VARS_UE **phy_vars_ue, openair0_config_t *openair0_cfg, openair0_rf_map rf_map[MAX_NUM_CCs])
{

#ifndef EXMIMO
  uint16_t N_TA_offset = 0;
#endif

  int i, CC_id;
  LTE_DL_FRAME_PARMS *frame_parms;
  for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++) {
    if (phy_vars_ue[CC_id]) {
      frame_parms = &(phy_vars_ue[CC_id]->lte_frame_parms); 
    }
    else {
      printf("phy_vars_eNB[%d] not initialized\n", CC_id);
      return(-1);
    }


#ifndef EXMIMO
    if (frame_parms->frame_type == TDD) {
      if (frame_parms->N_RB_DL == 100)
	N_TA_offset = 624;
      else if (frame_parms->N_RB_DL == 50)
	N_TA_offset = 624/2;
      else if (frame_parms->N_RB_DL == 25)
	N_TA_offset = 624/4;
    }
#endif
   
#ifdef EXMIMO
    openair0_cfg[CC_id].tx_num_channels = 0;
    openair0_cfg[CC_id].rx_num_channels = 0;

    // replace RX signal buffers with mmaped HW versions
    for (i=0;i<frame_parms->nb_antennas_rx;i++) {
      printf("Mapping UE CC_id %d, rx_ant %d, freq %u on card %d, chain %d\n",CC_id,i,downlink_frequency[CC_id][i],rf_map[CC_id].card,rf_map[CC_id].chain+i);
      free(phy_vars_ue[CC_id]->lte_ue_common_vars.rxdata[i]);
      phy_vars_ue[CC_id]->lte_ue_common_vars.rxdata[i] = (int32_t*) openair0_exmimo_pci[rf_map[CC_id].card].adc_head[rf_map[CC_id].chain+i];
      if (openair0_cfg[rf_map[CC_id].card].rx_freq[rf_map[CC_id].chain+i]) {
	printf("Error with rf_map! A channel has already been allocated!\n");
	return(-1);
      }
      else {
	openair0_cfg[rf_map[CC_id].card].rx_freq[rf_map[CC_id].chain+i] = downlink_frequency[CC_id][i];
	openair0_cfg[rf_map[CC_id].card].rx_gain[rf_map[CC_id].chain+i] = rx_gain[CC_id][i];
	openair0_cfg[rf_map[CC_id].card].rxg_mode[rf_map[CC_id].chain+i] = rx_gain_mode[CC_id][i];
	openair0_cfg[rf_map[CC_id].card].rx_num_channels++;
      }

      printf("rxdata[%d] @ %p\n",i,phy_vars_ue[CC_id]->lte_ue_common_vars.rxdata[i]);
    }
    for (i=0;i<frame_parms->nb_antennas_tx;i++) {
      printf("Mapping UE CC_id %d, tx_ant %d, freq %u on card %d, chain %d\n",CC_id,i,downlink_frequency[CC_id][i],rf_map[CC_id].card,rf_map[CC_id].chain+i);
      free(phy_vars_ue[CC_id]->lte_ue_common_vars.txdata[i]);
      phy_vars_ue[CC_id]->lte_ue_common_vars.txdata[i] = (int32_t*) openair0_exmimo_pci[rf_map[CC_id].card].dac_head[rf_map[CC_id].chain+i];
      if (openair0_cfg[rf_map[CC_id].card].tx_freq[rf_map[CC_id].chain+i]) {
	printf("Error with rf_map! A channel has already been allocated!\n");
	return(-1);
      }
      else {
	openair0_cfg[rf_map[CC_id].card].tx_freq[rf_map[CC_id].chain+i] = downlink_frequency[CC_id][i]+uplink_frequency_offset[CC_id][i];
	openair0_cfg[rf_map[CC_id].card].tx_gain[rf_map[CC_id].chain+i] = tx_gain[CC_id][i];
	openair0_cfg[rf_map[CC_id].card].tx_num_channels++;
      }

      printf("txdata[%d] @ %p\n",i,phy_vars_ue[CC_id]->lte_ue_common_vars.txdata[i]);
    }
  
#else
    // replace RX signal buffers with mmaped HW versions
    for (i=0;i<frame_parms->nb_antennas_rx;i++) {
      printf("Mapping UE CC_id %d, rx_ant %d, freq %u on card %d, chain %d\n",CC_id,i,downlink_frequency[CC_id][i],rf_map[CC_id].card,rf_map[CC_id].chain+i);
      free(phy_vars_ue[CC_id]->lte_ue_common_vars.rxdata[i]);
      rxdata = (int32_t*)malloc16(samples_per_frame*sizeof(int32_t));
      phy_vars_ue[CC_id]->lte_ue_common_vars.rxdata[i] = rxdata-N_TA_offset; // N_TA offset for TDD
    }
    for (i=0;i<frame_parms->nb_antennas_tx;i++) {
      printf("Mapping UE CC_id %d, tx_ant %d, freq %u on card %d, chain %d\n",CC_id,i,downlink_frequency[CC_id][i],rf_map[CC_id].card,rf_map[CC_id].chain+i);
      free(phy_vars_ue[CC_id]->lte_ue_common_vars.txdata[i]);
      txdata = (int32_t*)malloc16(samples_per_frame*sizeof(int32_t));
      phy_vars_ue[CC_id]->lte_ue_common_vars.txdata[i] = txdata;
      memset(txdata, 0, samples_per_frame*sizeof(int32_t));
    }
    
#endif
    
  }
  return(0);

}

/* this function maps the phy_vars_eNB tx and rx buffers to the available rf chains. 
   Each rf chain is is addressed by the card number and the chain on the card. The 
   rf_map specifies for each CC, on which rf chain the mapping should start. Multiple 
   antennas are mapped to successive RF chains on the same card. */
int setup_eNB_buffers(PHY_VARS_eNB **phy_vars_eNB, openair0_config_t *openair0_cfg, openair0_rf_map rf_map[MAX_NUM_CCs]) {

  int i, CC_id;
#ifndef EXMIMO
  uint16_t N_TA_offset = 0;
#else
  int j;
#endif
  LTE_DL_FRAME_PARMS *frame_parms;


  for (CC_id=0; CC_id<MAX_NUM_CCs; CC_id++) {
    if (phy_vars_eNB[CC_id]) {
      frame_parms = &(phy_vars_eNB[CC_id]->lte_frame_parms);
      printf("setup_eNB_buffers: frame_parms = %p\n",frame_parms);
    } 
    else {
      printf("phy_vars_eNB[%d] not initialized\n", CC_id);
      return(-1);
    }

#ifndef EXMIMO
    if (frame_parms->frame_type == TDD) {
      if (frame_parms->N_RB_DL == 100)
	N_TA_offset = 624;
      else if (frame_parms->N_RB_DL == 50)
	N_TA_offset = 624/2;
      else if (frame_parms->N_RB_DL == 25)
	N_TA_offset = 624/4;
    }
#endif
   

   

  
    // replace RX signal buffers with mmaped HW versions
#ifdef EXMIMO
    openair0_cfg[CC_id].tx_num_channels = 0;
    openair0_cfg[CC_id].rx_num_channels = 0;

    for (i=0;i<frame_parms->nb_antennas_rx;i++) {
      printf("Mapping eNB CC_id %d, rx_ant %d, freq %u on card %d, chain %d\n",CC_id,i,downlink_frequency[CC_id][i]+uplink_frequency_offset[CC_id][i],rf_map[CC_id].card,rf_map[CC_id].chain+i);
      free(phy_vars_eNB[CC_id]->lte_eNB_common_vars.rxdata[0][i]);
      phy_vars_eNB[CC_id]->lte_eNB_common_vars.rxdata[0][i] = (int32_t*) openair0_exmimo_pci[rf_map[CC_id].card].adc_head[rf_map[CC_id].chain+i];
      if (openair0_cfg[rf_map[CC_id].card].rx_freq[rf_map[CC_id].chain+i]) {
	printf("Error with rf_map! A channel has already been allocated!\n");
	return(-1);
      }
      else {
	openair0_cfg[rf_map[CC_id].card].rx_freq[rf_map[CC_id].chain+i] = downlink_frequency[CC_id][i]+uplink_frequency_offset[CC_id][i];
	openair0_cfg[rf_map[CC_id].card].rx_gain[rf_map[CC_id].chain+i] = rx_gain[CC_id][i];
	openair0_cfg[rf_map[CC_id].card].rx_num_channels++;
      }
      printf("rxdata[%d] @ %p\n",i,phy_vars_eNB[CC_id]->lte_eNB_common_vars.rxdata[0][i]);
      for (j=0;j<16;j++) {
	printf("rxbuffer %d: %x\n",j,phy_vars_eNB[CC_id]->lte_eNB_common_vars.rxdata[0][i][j]);
	phy_vars_eNB[CC_id]->lte_eNB_common_vars.rxdata[0][i][j] = 16-j;
      }
    }
    for (i=0;i<frame_parms->nb_antennas_tx;i++) {
      printf("Mapping eNB CC_id %d, tx_ant %d, freq %u on card %d, chain %d\n",CC_id,i,downlink_frequency[CC_id][i],rf_map[CC_id].card,rf_map[CC_id].chain+i);
      free(phy_vars_eNB[CC_id]->lte_eNB_common_vars.txdata[0][i]);
      phy_vars_eNB[CC_id]->lte_eNB_common_vars.txdata[0][i] = (int32_t*) openair0_exmimo_pci[rf_map[CC_id].card].dac_head[rf_map[CC_id].chain+i];
      if (openair0_cfg[rf_map[CC_id].card].tx_freq[rf_map[CC_id].chain+i]) {
	printf("Error with rf_map! A channel has already been allocated!\n");
	return(-1);
      }
      else {
	openair0_cfg[rf_map[CC_id].card].tx_freq[rf_map[CC_id].chain+i] = downlink_frequency[CC_id][i];
	openair0_cfg[rf_map[CC_id].card].tx_gain[rf_map[CC_id].chain+i] = tx_gain[CC_id][i];
	openair0_cfg[rf_map[CC_id].card].tx_num_channels++;
      }
      
      printf("txdata[%d] @ %p\n",i,phy_vars_eNB[CC_id]->lte_eNB_common_vars.txdata[0][i]);
      for (j=0;j<16;j++) {
	printf("txbuffer %d: %x\n",j,phy_vars_eNB[CC_id]->lte_eNB_common_vars.txdata[0][i][j]);
	phy_vars_eNB[CC_id]->lte_eNB_common_vars.txdata[0][i][j] = 16-j;
      }
    }
#else // not EXMIMO
    for (i=0;i<frame_parms->nb_antennas_rx;i++) {
      free(phy_vars_eNB[CC_id]->lte_eNB_common_vars.rxdata[0][i]);
      rxdata = (int32_t*)malloc16(samples_per_frame*sizeof(int32_t));
      phy_vars_eNB[CC_id]->lte_eNB_common_vars.rxdata[0][i] = rxdata-N_TA_offset; // N_TA offset for TDD
      memset(rxdata, 0, samples_per_frame*sizeof(int32_t));
      printf("rxdata[%d] @ %p (%p)\n", i, phy_vars_eNB[CC_id]->lte_eNB_common_vars.rxdata[0][i],rxdata);
    }
    for (i=0;i<frame_parms->nb_antennas_tx;i++) {
      free(phy_vars_eNB[CC_id]->lte_eNB_common_vars.txdata[0][i]);
      txdata = (int32_t*)malloc16(samples_per_frame*sizeof(int32_t));
      phy_vars_eNB[CC_id]->lte_eNB_common_vars.txdata[0][i] = txdata;
      memset(txdata, 0, samples_per_frame*sizeof(int32_t));
      printf("txdata[%d] @ %p\n", i, phy_vars_eNB[CC_id]->lte_eNB_common_vars.txdata[0][i]);

    }
#endif
  }
  return(0);
}


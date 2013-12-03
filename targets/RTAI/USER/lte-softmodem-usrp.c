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
#include <time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define __USE_GNU
//#define _GNU_SOURCE
#include <sched.h>
#include <signal.h>
#include <execinfo.h>
#include <getopt.h>
//#include <windows.h>
#include <errno.h>

#include "rt_wrapper.h"

#ifdef EMOS
#include <gps.h>
#endif

#include "PHY/types.h"
#include "PHY/defs.h"
#include "openair0_lib.h"

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

#ifdef SMBV
#include "PHY/TOOLS/smbv.h"
unsigned short config_frames[4] = {2,9,11,13};
#endif
#include "UTIL/LOG/log_extern.h"
#include "UTIL/OTG/otg.h"
#include "UTIL/OTG/otg_vars.h"
#include "UTIL/MATH/oml.h"
#include "UTIL/LOG/vcd_signal_dumper.h"


#include "stats.h"
#include "../../ARCH/USRP/USERSPACE/LIB/def.h"
#include "TOOLS/thread_ipc.c"


#ifdef XFORMS
#include "PHY/TOOLS/lte_phy_scope.h"

// current status is that every UE has a DL scope for a SINGLE eNB (eNB_id=0)
// at eNB 0, an UL scope for every UE
FD_lte_phy_scope_ue  *form_ue[NUMBER_OF_UE_MAX];
FD_lte_phy_scope_enb *form_enb[NUMBER_OF_UE_MAX];
FD_stats_form *form_stats=NULL;
char title[255];
unsigned char scope_enb_num_ue = 1;
#endif //XFORMS

#define FRAME_PERIOD 100000000ULL
#define DAQ_PERIOD 66667ULL

//#define rt_printk printf

#undef MALLOC //there are two conflicting definitions, so we better make sure we don't use it at all

#ifdef RTAI //by super
static SEM *mutex;
//static CND *cond;

static int thread0;
static int thread1;
static int thread_recv;
//static int sync_thread;
#else
pthread_t thread0;
pthread_t thread1;
pthread_t thread_recv;
pthread_attr_t attr_dlsch_threads;
struct sched_param sched_param_dlsch;
#endif

pthread_t  thread2;
pthread_t  thread3;
pthread_t  tid_eNB;
pthread_t  tid_recv;
pthread_t  thread_tx_error;
pthread_attr_t attr;

pthread_mutex_t mutex_eNB;
pthread_cond_t cond_eNB;

int ret;

// ------------------------------------
// functions and variables for USRP
// ------------------------------------
extern int oai_tx_buff[SAMPLES_PER_FRM];
extern int oai_rx_buff[SAMPLES_PER_FRM];
extern void UHD_init(void);
extern void UHD_init_display(void);
extern void Init_send(void);
extern void Init_recv(void);
extern void set_freq(int freq_offset);
extern void send_data(int hw_frm_head_pos);
extern void recv_data(int hw_frm_head_pos);
extern void send_end();
extern void tx_errorcode_handler(void);

int tx_offset = 0;
int hw_frm_head_pos = 0;

extern volatile unsigned long long tx_timestamp,rx_timestamp,clock_usrp;

extern volatile int flag_recv,flag_clock;
extern volatile int recv_ready;
extern volatile int w_pos_usrp_rcv; // value range [0:76800-1]
extern volatile int w_slot_usrp_rcv, r_slot_usrp_send; // slot index for USRP thread. value range [0:19]
extern volatile int r_slot_idx, t_slot_idx; // slot index for eNB thread. value range [0;19]
extern volatile int send_slot_missed;

double time_diff1,time_diff2,time_diff3,time_diff4;
struct timespec time_1, time_2,time_3,time_4,time_5;

// USRP debug output control
extern int g_usrp_debug;

//add for program run limit times
int N_slot_cycle;

// -----------------------------------
// Ctrl+c control
// -----------------------------------
int oai_exit = 0;
void sig_int_handler(void);

int card = 0;
exmimo_config_t *p_exmimo_config;
exmimo_id_t     *p_exmimo_id;
volatile unsigned int *DAQ_MBOX;

//int time_offset[4] = {-138,-138,-138,-138};
//int time_offset[4] = {-145,-145,-145,-145};
int time_offset[4] = {0,0,0,0};

int fs4_test=0;
char UE_flag;
u8  eNB_id=0,UE_id=0;

u32 carrier_freq[4]= {1907600000,1907600000,1907600000,1907600000};

struct timing_info_t {
	//unsigned int frame, hw_slot, last_slot, next_slot;
	RTIME time_min, time_max, time_avg, time_last, time_now;
	//unsigned int mbox0, mbox1, mbox2, mbox_target;
	unsigned int n_samples;
} timing_info;

extern s16* sync_corr_ue0;
extern s16 prach_ifft[4][1024*2];


runmode_t mode;
int rx_input_level_dBm;
#ifdef XFORMS
extern int otg_enabled;
#else
int otg_enabled;
#endif
int number_of_cards = 1;

int mbox_bounds[20] = {8,16,24,30,38,46,54,60,68,76,84,90,98,106,114,120,128,136,144, 0}; ///boundaries of slots in terms ob mbox counter rounded up to even numbers

int init_dlsch_threads(void);
void cleanup_dlsch_threads(void);
s32 init_rx_pdsch_thread(void);
void cleanup_rx_pdsch_thread(void);
int init_ulsch_threads(void);
void cleanup_ulsch_threads(void);

LTE_DL_FRAME_PARMS *frame_parms;

void setup_ue_buffers(PHY_VARS_UE *phy_vars_ue, LTE_DL_FRAME_PARMS *frame_parms, int carrier);
void setup_eNB_buffers(PHY_VARS_eNB *phy_vars_eNB, LTE_DL_FRAME_PARMS *frame_parms, int carrier);
void test_config(int card, int ant, unsigned int rf_mode, int UE_flag);

unsigned int build_rflocal(txi, txq, rxi, rxq)
{
	return (txi + (txq<<6) + (rxi<<12) + (rxq<<18));
}
unsigned int build_rfdc(int dcoff_i_rxfe, int dcoff_q_rxfe)
{
	return (dcoff_i_rxfe + (dcoff_q_rxfe<<8));
}

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
		oai_exit=1;
	}
}

void exit_fun(const char* s)
{
	void *array[10];
	size_t size;

	printf("Exiting: %s\n",s);

	oai_exit=1;
	//rt_sleep_ns(FRAME_PERIOD);

	//exit (-1);
}

int dummy_tx_buffer[3840*4] __attribute__((aligned(16)));

void sig_int_handler(void)
{
	oai_exit = 1;
}

// -----------------------------------
// USRP tx underflow test
// -----------------------------------
// FIXME HL_CHECK is this thread necessary?
void *tx_error_handler(void *ptr)
{
	(void)ptr;
	while (!oai_exit)
	{
		tx_errorcode_handler();
	}
	printf("[USRP monitoring thread] Received oai_exit signal. Ends.\n");
	pthread_exit(NULL);
}

// -----------------------------------
// USRP send and receive thread
// -----------------------------------
void *usrp_thread(void *ptr)
{
	(void)ptr;
	bind_thread2kernel(1);
	int i=0;

	// continuous receiving
	while (!oai_exit)
	{
		// receive one slot-len
		recv_data(hw_frm_head_pos);
		// send one slot
		send_data(hw_frm_head_pos);
		i++; if(i==N_slot_cycle) oai_exit=1;//control run N_slot_cycle slot and exit
	}

	send_end();
	printf("[USRP thread] Received oai_exit signal. Ends. send slot missed: %d\n",send_slot_missed);
	pthread_exit(NULL);
}

/* This is the main eNB thread. It gets woken up by the kernel driver using the RTAI message mechanism (rt_send and rt_receive). */
static void *eNB_thread(void *arg)
	//void eNB_thread(void)
{
	printf("Start eNB Server...\n\n");

	bind_thread2kernel(2);
#ifdef RTAI
	RT_TASK *task;
#endif
	int slot=0,last_slot, next_slot,frame=0;
	unsigned int aa,slot_offset, slot_offset_F;
	int diff = 0;
	int i;
	int num_slot_missed = 0;

#ifdef RTAI
	task = rt_task_init_schmod(nam2num("TASK0"), 0, 0, 0, SCHED_FIFO, 0xF);
	LOG_D(HW,"Started eNB thread (id %p)\n",task);
	//if(task==NULL) printf("RT Task creation failed\n"); else printf("Task success!\n");
#endif
	mlockall(MCL_CURRENT | MCL_FUTURE);

#ifdef HARD_RT
	rt_make_hard_real_time();
#endif

	// initialize reading postition
	r_slot_idx = 0;
	t_slot_idx = 0;
	frame = 0;

	while (!oai_exit)
	{
		format_printf(g_usrp_debug,"[eNB ] r_eNB: %d w_usrp: %d num_slot_missed: %d diff: %d\n",r_slot_idx,w_slot_usrp_rcv, num_slot_missed,diff);

		// Update 'diff'. Handle the frame wrap-arround
		if ((w_slot_usrp_rcv<4)&&(r_slot_idx>=16)){
			diff = r_slot_idx - (w_slot_usrp_rcv + 20) ;
		}
		else if((w_slot_usrp_rcv>=16)&&(r_slot_idx<4)){
			diff = (r_slot_idx + 20) - w_slot_usrp_rcv;
		}
		else{
			diff = r_slot_idx - w_slot_usrp_rcv;
		}

		// Reading is too slow. It's already late.
		if(diff < -4)
		{
			r_slot_idx++;
			num_slot_missed += abs(diff + 4);
			if(r_slot_idx==20){
				r_slot_idx = 0;
				frame++;
			}
			continue;
		}

		// Wait for writing
		while((diff > 0)&& (!oai_exit))
		{
			usleep(500);
			// Update 'diff'. Handle the frame wrap-arround
			if ((w_slot_usrp_rcv<4)&&(r_slot_idx>=16)){
				diff = r_slot_idx - (w_slot_usrp_rcv + 20) ;
			}
			else if((w_slot_usrp_rcv>=16)&&(r_slot_idx<4)){
				diff = (r_slot_idx + 20) - w_slot_usrp_rcv;
			}
			else{
				diff = r_slot_idx - w_slot_usrp_rcv;
			}
		}

		// The slot is ready
		last_slot = r_slot_idx;
		next_slot = (r_slot_idx + N_slot_offset)%LTE_SLOTS_PER_FRAME;
		format_printf(g_usrp_debug, "[eNB ] r_eNB: %d w_usrp: %d last_slot: %d next_slot: %d diff: %d\n\n",r_slot_idx,w_slot_usrp_rcv,last_slot, next_slot,diff);

		//---------------------------
		// eNB procedure
		//---------------------------
		PHY_vars_eNB_g[0]->frame = frame;
		//if (frame>5)
		{
			if (fs4_test==0)
				// -- PHY procedure --
			{
				//phy_procedures_eNB_lte(last_slot, next_slot, PHY_vars_eNB_g[0], 0);//for OAI version berfore 4160
				phy_procedures_eNB_lte(last_slot, next_slot, PHY_vars_eNB_g[0], 0,0,NULL);
#ifndef IFFT_FPGA
				slot_offset_F = (next_slot)*
					(PHY_vars_eNB_g[0]->lte_frame_parms.ofdm_symbol_size)*
					((PHY_vars_eNB_g[0]->lte_frame_parms.Ncp==1) ? 6 : 7);//0for normal 7,1 for extened 6
				slot_offset = (next_slot)*
					(PHY_vars_eNB_g[0]->lte_frame_parms.samples_per_tti>>1);

				if ((subframe_select(&PHY_vars_eNB_g[0]->lte_frame_parms,next_slot>>1)==SF_DL)||
						((subframe_select(&PHY_vars_eNB_g[0]->lte_frame_parms,next_slot>>1)==SF_S)&&((next_slot&1)==0)))
				{

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
			}// end of PHY procedure

#endif //IFFT_FPGA

		}// end of frame>5

		// update slot index
		t_slot_idx = next_slot; // this slot is ready to send
		r_slot_idx++;
		if(r_slot_idx==20)
		{
			r_slot_idx = 0;
			frame++;
		}
	}// end of while

	format_printf(1,"\n[eNB thread] Received oai_exit signal. Ends. Ran %d frames. num_slot_missed: %d\n",frame, num_slot_missed);


#ifdef HARD_RT
	rt_make_soft_real_time();
#endif

	// clean task
#ifdef RTAI
	rt_task_delete(task);
#endif
	return 0;
}


/* This is the main UE thread. Initially it is doing a periodic get_frame. One synchronized it gets woken up by the kernel driver using the RTAI message mechanism (rt_send and rt_receive). */
static void *UE_thread(void *arg)
{
	printf("Start UE Client...\n\n");

	bind_thread2kernel(2);
#ifdef RTAI
	RT_TASK *task;
#endif
	int slot=0,last_slot, next_slot,frame=0;
	unsigned int aa,slot_offset, slot_offset_F;
	int diff = 0;
	int i;
	int num_slot_missed = 0;

	static int is_synchronized = 0;
	static int received_slots = 0;
	static int slot0 = 0;
	static int first_run=1;

#ifdef RTAI
	task = rt_task_init_schmod(nam2num("TASK0"), 0, 0, 0, SCHED_FIFO, 0xF);
	LOG_D(HW,"Started UE thread (id %p)\n",task);
	//if(task==NULL) printf("RT Task creation failed\n"); else printf("Task success!\n");
#endif
	mlockall(MCL_CURRENT | MCL_FUTURE);

#ifdef HARD_RT
	rt_make_hard_real_time();
#endif

	// initialize reading postition
	r_slot_idx = 0;
	t_slot_idx = 0;
	frame = 0;

	openair_daq_vars.freq_offset = 0; //-7500;
	set_freq(openair_daq_vars.freq_offset);

	if (mode == rx_calib_ue) {
		openair_daq_vars.freq_offset = -7500;
		set_freq(openair_daq_vars.freq_offset);
	}

	usleep(10000*(7.68/6.25));//wait frame data fill in buff
	//test synchronized or not
	char test_synchronize = 1;

	while (!oai_exit)
	{

		if (is_synchronized)
		{
			//test is synchronized or not
			if(test_synchronize) {printf("Found cell ,UE synchronized!  hw_frm_head_pos = %d\n",hw_frm_head_pos); test_synchronize = 0;}

			format_printf(g_usrp_debug,"[UE ] r_ue: %d w_usrp: %d num_slot_missed: %d diff: %d\n",r_slot_idx,w_slot_usrp_rcv, num_slot_missed,diff);

			// Update 'diff'. Handle the frame wrap-arround
			if ((w_slot_usrp_rcv<4)&&(r_slot_idx>=16)){
				diff = r_slot_idx - (w_slot_usrp_rcv + 20) ;
			}
			else if((w_slot_usrp_rcv>=16)&&(r_slot_idx<4)){
				diff = (r_slot_idx + 20) - w_slot_usrp_rcv;
			}
			else{
				diff = r_slot_idx - w_slot_usrp_rcv;
			}

			// Reading is too slow. It's already late.
			if(diff < -4)
			{
				r_slot_idx++;
				num_slot_missed += abs(diff + 4);
				if(r_slot_idx==20){
					r_slot_idx = 0;
					frame++;
				}
				continue;
			}

			// Wait for writing
			while((diff > 0)&& (!oai_exit))
			{
				usleep(500);
				// Update 'diff'. Handle the frame wrap-arround
				if ((w_slot_usrp_rcv<4)&&(r_slot_idx>=16)){
					diff = r_slot_idx - (w_slot_usrp_rcv + 20) ;
				}
				else if((w_slot_usrp_rcv>=16)&&(r_slot_idx<4)){
					diff = (r_slot_idx + 20) - w_slot_usrp_rcv;
				}
				else{
					diff = r_slot_idx - w_slot_usrp_rcv;
				}
			}

			// The slot is ready
			last_slot = r_slot_idx;
			next_slot = (r_slot_idx + N_slot_offset)%LTE_SLOTS_PER_FRAME;
			format_printf(g_usrp_debug, "[UE ] r_ue: %d w_usrp: %d last_slot: %d next_slot: %d diff: %d\n\n",r_slot_idx,w_slot_usrp_rcv,last_slot, next_slot,diff);


			PHY_vars_UE_g[0]->frame = frame;

			//phy_procedures_UE_lte (last_slot, next_slot, PHY_vars_UE_g[0], 0, 0,mode);//for OAI version after 4160
			phy_procedures_UE_lte (last_slot, next_slot, PHY_vars_UE_g[0], 0, 0,mode,0,NULL);

			// update slot index
			t_slot_idx = next_slot; // this slot is ready to send
			r_slot_idx++;
			if(r_slot_idx==20)
			{
				r_slot_idx = 0;
				frame++;
			}
		}
		else   // we are not yet synchronized
		{

			hw_frm_head_pos = 0;

			slot = 0;

			if (initial_sync(PHY_vars_UE_g[0],mode)==0) { // acquired synchronization

				if (mode == rx_calib_ue) {
					oai_exit=1;
				}
				else {
					is_synchronized = 1;
					hw_frm_head_pos = PHY_vars_UE_g[0]->rx_offset;
					LOG_D(HW,"Got synch: hw_frm_head_pos %d\n",hw_frm_head_pos);
				}
			}
			else { // Not acquired synchronization. Adjust carrier frequency.
				if (openair_daq_vars.freq_offset >= 0) {
					openair_daq_vars.freq_offset += 100; // FIXME HL_CHECK why add 100Hz here
					openair_daq_vars.freq_offset *= -1;
				}
				else {
					openair_daq_vars.freq_offset *= -1;
				}
				if (abs(openair_daq_vars.freq_offset) > 7500) { // cannot handle the frequency offset larger than 7.5KHz
					LOG_I(PHY,"[initial_sync] No cell synchronization found, abondoning\n");
					mac_xface->macphy_exit("");
					oai_exit = 1;
				}
				else {
					LOG_I(PHY,"[initial_sync] trying carrier off %d Hz\n",openair_daq_vars.freq_offset);
					set_freq(openair_daq_vars.freq_offset);
				}
			}
		}
	}

	LOG_D(HW,"UE_thread: finished, ran %d times.\n",frame);

	format_printf(1,"\n[UE thread] Received oai_exit signal. Ends. Ran %d frames. num_slot_missed: %d\n",frame, num_slot_missed);
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





int main(int argc, char **argv)
{
#ifdef RTAI
	RT_TASK *task;
#endif
	int i,j,aa;
	void *status;

	/*
	   u32 rf_mode_max[4]     = {55759,55759,55759,55759};
	   u32 rf_mode_med[4]     = {39375,39375,39375,39375};
	   u32 rf_mode_byp[4]     = {22991,22991,22991,22991};
	 */
	u32 my_rf_mode = RXEN + TXEN + TXLPFNORM + TXLPFEN + TXLPF25 + RXLPFNORM + RXLPFEN + RXLPF25 + LNA1ON +LNAMax + RFBBNORM + DMAMODE_RX + DMAMODE_TX;
	u32 my_rf_mode2 = RXEN + TXLPFNORM + TXLPFEN + TXLPF25 + RXLPFNORM + RXLPFEN + RXLPF25 + LNA1ON +LNAMax + RFBBNORM + DMAMODE_RX;
	u32 rf_mode[4]     = {my_rf_mode,my_rf_mode2,0,0};
	u32 rf_local[4]    = {8255000,8255000,8255000,8255000}; // UE zepto
	//{8254617, 8254617, 8254617, 8254617}; //eNB khalifa
	//{8255067,8254810,8257340,8257340}; // eNB PETRONAS

	u32 rf_vcocal[4]   = {910,910,910,910};
	u32 rf_vcocal_850[4] = {2015, 2015, 2015, 2015};
	u32 rf_rxdc[4]     = {32896,32896,32896,32896};
	u32 rxgain[4]      = {20,20,20,20};
	u32 txgain[4]      = {25,25,25,25};

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
	char rfdc_fname[100];
	FILE *rxg_fd=NULL;
	FILE *txg_fd=NULL;
	FILE *rflo_fd=NULL;
	FILE *rfdc_fd=NULL;
	unsigned int rxg_max[4]={133,133,133,133}, rxg_med[4]={127,127,127,127}, rxg_byp[4]={120,120,120,120};
	int tx_max_power=0;

	char line[1000];
	int l;
	int ret, ant;

	int error_code;

	const struct option long_options[] = {
		{"calib-ue-rx", required_argument, NULL, 256},
		{"calib-ue-rx-med", required_argument, NULL, 257},
		{"calib-ue-rx-byp", required_argument, NULL, 258},
		{"debug-ue-prach", no_argument, NULL, 259},
		{"no-L2-connect", no_argument, NULL, 260},
		{NULL, 0, NULL, 0}};

	mode = normal_txrx;


	while ((c = getopt_long (argc, argv, "C:ST:UdF:V:Dn:",long_options,NULL)) != -1)
	{
		switch (c)
		{
			case 'D':
				printf("Enable debug output. \n\n");
				g_usrp_debug = 1;
				break;
			case 'V':
				ouput_vcd = 1;
				break;
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
			case 'n':
				N_slot_cycle=atoi(optarg);
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
					printf("%s not found, running with defaults\n",txg_fname);

				sprintf(rflo_fname,"%srflo.lime",optarg);
				rflo_fd = fopen(rflo_fname,"r");
				if (rflo_fd) {
					printf("Loading RF LO parameters from %s\n",rflo_fname);
					fscanf(rflo_fd,"%d %d %d %d",&rf_local[0],&rf_local[1],&rf_local[2],&rf_local[3]);
				}
				else
					printf("%s not found, running with defaults\n",rflo_fname);

				sprintf(rfdc_fname,"%srfdc.lime",optarg);
				rfdc_fd = fopen(rfdc_fname,"r");
				if (rfdc_fd) {
					printf("Loading RF DC parameters from %s\n",rfdc_fname);
					fscanf(rfdc_fd,"%d %d %d %d",&rf_rxdc[0],&rf_rxdc[1],&rf_rxdc[2],&rf_rxdc[3]);
				}
				else
					printf("%s not found, running with defaults\n",rfdc_fname);

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

	if (ouput_vcd) {
		if (UE_flag==1)
			vcd_signal_dumper_init("/tmp/openair_dump_UE.vcd");
		else
			vcd_signal_dumper_init("/tmp/openair_dump_eNB.vcd");
	}

#ifdef NAS_NETLINK
	netlink_init();
#endif

	// to make a graceful exit when ctrl-c is pressed
	//signal(SIGSEGV, signal_handler);
	//signal(SIGINT, signal_handler);

#ifndef RTAI
	check_clock();
#endif

	// init the parameters
	frame_parms = (LTE_DL_FRAME_PARMS*) malloc(sizeof(LTE_DL_FRAME_PARMS));
	frame_parms->N_RB_DL            = 25;
	frame_parms->N_RB_UL            = 25;
	frame_parms->Ncp                = 0;
	frame_parms->Ncp_UL             = 0;
	frame_parms->Nid_cell           = Nid_cell;
	frame_parms->nushift            = 0;
	frame_parms->nb_antennas_tx_eNB = 1; //initial value overwritten by initial sync later
	// frame_parms->nb_antennas_tx     = (UE_flag==0) ? 1 : 1;
	// frame_parms->nb_antennas_rx     = (UE_flag==0) ? 1 : 1;
	frame_parms->nb_antennas_tx     = 1;
	frame_parms->nb_antennas_rx     = 1;
	frame_parms->mode1_flag         = (transmission_mode == 1) ? 1 : 0;
	frame_parms->frame_type         = 1;
	frame_parms->tdd_config         = 3;
	frame_parms->tdd_config_S       = 0;
	frame_parms->phich_config_common.phich_resource = oneSixth;
	frame_parms->phich_config_common.phich_duration = normal;
	// UL RS Config
	frame_parms->pusch_config_common.ul_ReferenceSignalsPUSCH.cyclicShift = 0;//n_DMRS1 set to 0
	frame_parms->pusch_config_common.ul_ReferenceSignalsPUSCH.groupHoppingEnabled = 0;
	frame_parms->pusch_config_common.ul_ReferenceSignalsPUSCH.sequenceHoppingEnabled = 0;
	frame_parms->pusch_config_common.ul_ReferenceSignalsPUSCH.groupAssignmentPUSCH = 0;

	//print [PHY] ... message
	init_ul_hopping(frame_parms);

	//print [PHY][I][INIT]... message
	init_frame_parms(frame_parms,1);

	phy_init_top(frame_parms);
	phy_init_lte_top(frame_parms);

	//init prach for openair1 test
	frame_parms->prach_config_common.rootSequenceIndex=22;
	frame_parms->prach_config_common.prach_ConfigInfo.zeroCorrelationZoneConfig=1;
	frame_parms->prach_config_common.prach_ConfigInfo.prach_ConfigIndex=0;
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
		g_log->log_component[RRC].level = LOG_INFO;
		g_log->log_component[RRC].flag  = LOG_HIGH;

		PHY_vars_UE_g = malloc(sizeof(PHY_VARS_UE*));
		//print [PHY][I][init_lte_eNB]... messages
		PHY_vars_UE_g[0] = init_lte_UE(frame_parms, UE_id,abstraction_flag,transmission_mode);

#ifndef OPENAIR2
		for (i=0;i<NUMBER_OF_eNB_MAX;i++) {
			PHY_vars_UE_g[0]->pusch_config_dedicated[i].betaOffset_ACK_Index = beta_ACK;
			PHY_vars_UE_g[0]->pusch_config_dedicated[i].betaOffset_RI_Index  = beta_RI;
			PHY_vars_UE_g[0]->pusch_config_dedicated[i].betaOffset_CQI_Index = beta_CQI;

			PHY_vars_UE_g[0]->scheduling_request_config[i].sr_PUCCH_ResourceIndex = UE_id;
			PHY_vars_UE_g[0]->scheduling_request_config[i].sr_ConfigIndex = 7+(UE_id%3);
			PHY_vars_UE_g[0]->scheduling_request_config[i].dsr_TransMax = sr_n4;
		}
#endif

		compute_prach_seq(&PHY_vars_UE_g[0]->lte_frame_parms.prach_config_common,
				PHY_vars_UE_g[0]->lte_frame_parms.frame_type,
				PHY_vars_UE_g[0]->X_u);

		PHY_vars_UE_g[0]->lte_ue_pdcch_vars[0]->crnti = 0x1234;
#ifndef OPENAIR2
		PHY_vars_UE_g[0]->lte_ue_pdcch_vars[0]->crnti = 0x1235;
#endif
		NB_UE_INST=1;
		NB_INST=1;

		openair_daq_vars.manual_timing_advance = 0;
		//openair_daq_vars.timing_advance = TIMING_ADVANCE_HW;
		openair_daq_vars.rx_gain_mode = DAQ_AGC_OFF;
		openair_daq_vars.auto_freq_correction = 0;
		openair_daq_vars.use_ia_receiver = 1;

		// if AGC is off, the following values will be used
		//    for (i=0;i<4;i++)
		//    rxgain[i] = 20;
		rxgain[0] = 20;
		rxgain[1] = 20;
		rxgain[2] = 20;
		rxgain[3] = 20;

		for (i=0;i<4;i++) {
			PHY_vars_UE_g[0]->rx_gain_max[i] = rxg_max[i];
			PHY_vars_UE_g[0]->rx_gain_med[i] = rxg_med[i];
			PHY_vars_UE_g[0]->rx_gain_byp[i] = rxg_byp[i];
		}

		if ((mode == normal_txrx) || (mode == rx_calib_ue) || (mode == no_L2_connect) || (mode == debug_prach)) {
			for (i=0; i<4; i++) {
				PHY_vars_UE_g[0]->rx_gain_mode[i]  = max_gain;
				//            frame_parms->rfmode[i] = rf_mode_max[i];
				rf_mode[i] = (rf_mode[i] & (~LNAGAINMASK)) | LNAMax;
			}
			PHY_vars_UE_g[0]->rx_total_gain_dB =  PHY_vars_UE_g[0]->rx_gain_max[0] + rxgain[0] - 30; //-30 because it was calibrated with a 30dB gain
		}
		else if ((mode == rx_calib_ue_med)) {
			for (i=0; i<4; i++) {
				PHY_vars_UE_g[0]->rx_gain_mode[i] = med_gain;
				//            frame_parms->rfmode[i] = rf_mode_med[i];
				rf_mode[i] = (rf_mode[i] & (~LNAGAINMASK)) | LNAMed;
			}
			PHY_vars_UE_g[0]->rx_total_gain_dB =  PHY_vars_UE_g[0]->rx_gain_med[0]  + rxgain[0] - 30; //-30 because it was calibrated with a 30dB gain;
		}
		else if ((mode == rx_calib_ue_byp)) {
			for (i=0; i<4; i++) {
				PHY_vars_UE_g[0]->rx_gain_mode[i] = byp_gain;
				//            frame_parms->rfmode[i] = rf_mode_byp[i];
				rf_mode[i] = (rf_mode[i] & (~LNAGAINMASK)) | LNAByp;
			}
			PHY_vars_UE_g[0]->rx_total_gain_dB =  PHY_vars_UE_g[0]->rx_gain_byp[0]  + rxgain[0] - 30; //-30 because it was calibrated with a 30dB gain;
		}

		PHY_vars_UE_g[0]->tx_power_max_dBm = tx_max_power;

		printf("tx_max_power = %d -> amp %d\n",tx_max_power,get_tx_amp(tx_max_power,tx_max_power));
	}
	else { //this is eNB
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
		g_log->log_component[RRC].level = LOG_INFO;
		g_log->log_component[RRC].flag  = LOG_HIGH;


		PHY_vars_eNB_g = malloc(sizeof(PHY_VARS_eNB*));
		PHY_vars_eNB_g[0] = init_lte_eNB(frame_parms,eNB_id,Nid_cell,cooperation_flag,transmission_mode,abstraction_flag);

#ifndef OPENAIR2
		for (i=0;i<NUMBER_OF_UE_MAX;i++) {
			PHY_vars_eNB_g[0]->pusch_config_dedicated[i].betaOffset_ACK_Index = beta_ACK;
			PHY_vars_eNB_g[0]->pusch_config_dedicated[i].betaOffset_RI_Index  = beta_RI;
			PHY_vars_eNB_g[0]->pusch_config_dedicated[i].betaOffset_CQI_Index = beta_CQI;

			PHY_vars_eNB_g[0]->scheduling_request_config[i].sr_PUCCH_ResourceIndex = i;
			PHY_vars_eNB_g[0]->scheduling_request_config[i].sr_ConfigIndex = 7+(i%3);
			PHY_vars_eNB_g[0]->scheduling_request_config[i].dsr_TransMax = sr_n4;
		}
#endif

		compute_prach_seq(&PHY_vars_eNB_g[0]->lte_frame_parms.prach_config_common,
				PHY_vars_eNB_g[0]->lte_frame_parms.frame_type,
				PHY_vars_eNB_g[0]->X_u);

		NB_eNB_INST=1;
		NB_INST=1;

		openair_daq_vars.ue_dl_rb_alloc=0x1fff;
		openair_daq_vars.target_ue_dl_mcs=20;
		openair_daq_vars.ue_ul_nb_rb=6;
		openair_daq_vars.target_ue_ul_mcs=9;

		// if AGC is off, the following values will be used
		//    for (i=0;i<4;i++)
		//      rxgain[i]=30;
		rxgain[0] = 20;
		rxgain[1] = 20;
		rxgain[2] = 20;
		rxgain[3] = 20;


		// set eNB to max gain
		PHY_vars_eNB_g[0]->rx_total_gain_eNB_dB =  rxg_max[0] + rxgain[0] - 30; //was measured at rxgain=30;
		for (i=0; i<4; i++) {
			//        frame_parms->rfmode[i] = rf_mode_max[i];
			rf_mode[i] = (rf_mode[i] & (~LNAGAINMASK)) | LNAMax;
		}


	}

	dump_frame_parms(frame_parms);

	mac_xface = malloc(sizeof(MAC_xface));

#ifdef OPENAIR2
	int eMBMS_active=0;

	//print [MAC][I]... messages
	//l2_init(frame_parms,eMBMS_active);
	l2_init(frame_parms,eMBMS_active,
		0,// cba_group_active
		0); // HO flag
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

	//USRP INIT
	UHD_init();
	UHD_init_display();
	Init_send();
	Init_recv();
	printf("USRP init DONE...\n");
	//  }
#endif
	// connect the TX/RX buffers
	if (UE_flag==1) {
		setup_ue_buffers(PHY_vars_UE_g[0],frame_parms,0);
		printf("Setting UE buffer to all-RX\n");
	}
	else {
		setup_eNB_buffers(PHY_vars_eNB_g[0],frame_parms,0);
		if (fs4_test==0)
		{
			printf("Setting eNB buffer to all-RX\n");
		}
		else {
			printf("Setting eNB buffer to fs/4 test signal\n");
			for (j=0; j<PHY_vars_eNB_g[0]->lte_frame_parms.samples_per_tti*10; j+=4)
				for (aa=0; aa<frame_parms->nb_antennas_tx; aa++)
				{
					amp = 0x8000;
					((short*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j+1] = 0;
					((short*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j+3] = amp-1;
					((short*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j+5] = 0;
					((short*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j+7] = amp;
					((short*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j] = amp-1;
					((short*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j+2] = 0;
					((short*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j+4] = amp;
					((short*)PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa])[2*j+6] = 0;
				}
		}
	}

	//rt_sleep_ns(10*FRAME_PERIOD);//delay one second

#ifndef RTAI
	pthread_attr_init (&attr_dlsch_threads);
	pthread_attr_setstacksize(&attr_dlsch_threads,OPENAIR_THREAD_STACK_SIZE);
	//attr_dlsch_threads.priority = 1;
	sched_param_dlsch.sched_priority = sched_get_priority_max(SCHED_FIFO); //OPENAIR_THREAD_PRIORITY;
	pthread_attr_setschedparam  (&attr_dlsch_threads, &sched_param_dlsch);
	pthread_attr_setschedpolicy (&attr_dlsch_threads, SCHED_FIFO);
#endif

	// start the main thread
	if (UE_flag == 1) {
#ifdef RTAI
		thread1 = rt_thread_create(UE_thread, NULL, 100000000);
		thread_recv = rt_thread_create(usrp_thread, NULL, 100000000);
#else
		error_code = pthread_create(&thread1, &attr_dlsch_threads, UE_thread, NULL);
		if (error_code!= 0) {
			LOG_D(HW,"[lte-softmodem.c] Could not allocate UE_thread, error %d\n",error_code);
			return(error_code);
		}
		else {
			LOG_D(HW,"[lte-softmodem.c] Allocate UE_thread successful\n");
		}
		pthread_create(&thread_recv, NULL, usrp_thread, NULL);
#endif
#ifdef DLSCH_THREAD//defined
		init_rx_pdsch_thread();
		//rt_sleep_ns(FRAME_PERIOD/10);
		init_dlsch_threads();
#endif
	}
	else {
#ifdef RTAI
		thread0 = rt_thread_create(eNB_thread, NULL, 100000000);
		thread_recv = rt_thread_create(usrp_thread, NULL, 100000000);
#else
		error_code = pthread_create(&thread0, &attr_dlsch_threads, eNB_thread, NULL);
		if (error_code!= 0) {
			LOG_D(HW,"[lte-softmodem.c] Could not allocate eNB_thread, error %d\n",error_code);
			return(error_code);
		}
		else {
			LOG_D(HW,"[lte-softmodem.c] Allocate eNB_thread successful\n");
		}
		pthread_create(&thread_recv, NULL, usrp_thread, NULL);
#endif
#ifdef ULSCH_THREAD//not defined
		init_ulsch_threads();
#endif
	}
	pthread_create(&thread_tx_error, NULL, tx_error_handler, NULL);

	// wait for end of program
	usleep(500);
	printf("TYPE <CTRL-C> TO TERMINATE\n");
	signal(SIGINT, &sig_int_handler);

	while (oai_exit==0)
	    sleep(1);

	// cleanup
	pthread_join(thread_tx_error,&status);

	if (UE_flag == 1) {
#ifdef RTAI
		rt_thread_join(thread1);
		rt_thread_join(thread_recv);
#else
		pthread_join(thread1,&status);
		pthread_join(thread_recv,&status);
#endif
#ifdef DLSCH_THREAD
		printf("waiting for DLSCH_THREAD thread\n");
		cleanup_dlsch_threads();
		cleanup_rx_pdsch_thread();
#endif
	}
	else {
#ifdef RTAI
		printf("Cleanup eNB thread and usrp thread\n");
		rt_thread_join(thread0);
		rt_thread_join(thread_recv);
#else
		pthread_join(thread0,&status);
		pthread_join(thread_recv,&status);
#endif
#ifdef ULSCH_THREAD
		printf("waiting for ULSCH_THREAD thread\n");
		cleanup_ulsch_threads();
#endif
	}


	logClean();

	printf("Program Done!\n");
	return 0;
}

void test_config(int card, int ant, unsigned int rf_mode, int UE_flag)
{
	p_exmimo_config->framing.eNB_flag   = !UE_flag;
	p_exmimo_config->framing.tdd_config = 0;
	p_exmimo_config->framing.resampling_factor = 2;

	p_exmimo_config->rf.rf_freq_rx[ant] = 1907600000;
	p_exmimo_config->rf.rf_freq_tx[ant] = 1907600000;;
	p_exmimo_config->rf.rx_gain[ant][0] = 20;
	p_exmimo_config->rf.tx_gain[ant][0] = 10;
	p_exmimo_config->rf.rf_mode[ant] = rf_mode;

	p_exmimo_config->rf.rf_local[ant] = build_rflocal(20,25,26,04);
	p_exmimo_config->rf.rf_rxdc[ant] = build_rfdc(128, 128);
	p_exmimo_config->rf.rf_vcocal[ant] = (0xE<<6) + 0xE;
}

void setup_ue_buffers(PHY_VARS_UE *phy_vars_ue, LTE_DL_FRAME_PARMS *frame_parms, int carrier) {

	int i;
	if (phy_vars_ue) {

		if ((frame_parms->nb_antennas_rx>1) && (carrier>0)) {
			printf("RX antennas > 1 and carrier > 0 not possible\n");
			exit(-1);
		}

		if ((frame_parms->nb_antennas_tx>1) && (carrier>0)) {
			printf("TX antennas > 1 and carrier > 0 not possible\n");
			exit(-1);
		}

		// replace RX signal buffers with mmaped HW versions
		for (i=0;i<frame_parms->nb_antennas_rx;i++) {
			free(phy_vars_ue->lte_ue_common_vars.rxdata[i]);
			phy_vars_ue->lte_ue_common_vars.rxdata[i] = (s32*) oai_rx_buff;
			printf("rxdata[%d] @ %p\n",i,phy_vars_ue->lte_ue_common_vars.rxdata[i]);
		}
		for (i=0;i<frame_parms->nb_antennas_tx;i++) {
			free(phy_vars_ue->lte_ue_common_vars.txdata[i]);
			phy_vars_ue->lte_ue_common_vars.txdata[i] = (s32*) oai_tx_buff;
			printf("txdata[%d] @ %p\n",i,phy_vars_ue->lte_ue_common_vars.txdata[i]);
		}
	}
}
void setup_eNB_buffers(PHY_VARS_eNB *phy_vars_eNB, LTE_DL_FRAME_PARMS *frame_parms, int carrier) {

	int i;
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
		for (i=0;i<frame_parms->nb_antennas_rx;i++) {
			free(phy_vars_eNB->lte_eNB_common_vars.rxdata[0][i]);
			phy_vars_eNB->lte_eNB_common_vars.rxdata[0][i] = (s32 *) oai_rx_buff;
			printf("rxdata[%d] @ %p\n",i,phy_vars_eNB->lte_eNB_common_vars.rxdata[0][i]);
			/* for (j=0;j<16;j++) {
			   printf("rxbuffer %d: %x\n",j,phy_vars_eNB->lte_eNB_common_vars.rxdata[0][i][j]);
			   phy_vars_eNB->lte_eNB_common_vars.rxdata[0][i][j] = 16-j;
			   }*/
		}
		for (i=0;i<frame_parms->nb_antennas_tx;i++) {
			free(phy_vars_eNB->lte_eNB_common_vars.txdata[0][i]);
			phy_vars_eNB->lte_eNB_common_vars.txdata[0][i] = (s32 *) oai_tx_buff;
			printf("txdata[%d] @ %p\n",i,phy_vars_eNB->lte_eNB_common_vars.txdata[0][i]);
			/*  for (j=0;j<16;j++) {
			    printf("txbuffer %d: %x\n",j,phy_vars_eNB->lte_eNB_common_vars.txdata[0][i][j]);
			    phy_vars_eNB->lte_eNB_common_vars.txdata[0][i][j] = 16-j;
			    }*/
		}
	}
}

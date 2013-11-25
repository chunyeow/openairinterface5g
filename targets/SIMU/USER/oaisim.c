#include <string.h>
#include <math.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cblas.h>
#include <execinfo.h>

#include "SIMULATION/RF/defs.h"
#include "PHY/types.h"
#include "PHY/defs.h"
#include "PHY/LTE_TRANSPORT/proto.h"
#include "PHY/vars.h"
#include "MAC_INTERFACE/vars.h"

#include "SIMULATION/ETH_TRANSPORT/proto.h"

//#ifdef OPENAIR2
#include "LAYER2/MAC/defs.h"
#include "LAYER2/MAC/vars.h"
#include "pdcp.h"
#ifndef CELLULAR
#include "RRC/LITE/vars.h"
#endif
#include "PHY_INTERFACE/vars.h"
//#endif
#include "RRC/NAS/nas_config.h"

#include "ARCH/CBMIMO1/DEVICE_DRIVER/vars.h"

#ifdef IFFT_FPGA
//#include "PHY/LTE_REFSIG/mod_table.h"
#endif //IFFT_FPGA
#include "SCHED/defs.h"
#include "SCHED/vars.h"

#ifdef XFORMS
#include "PHY/TOOLS/lte_phy_scope.h"
#endif

#ifdef SMBV
#include "PHY/TOOLS/smbv.h"
char smbv_fname[] = "smbv_config_file.smbv";
unsigned short smbv_nframes = 4; // how many frames to configure 1,..,4
unsigned short config_frames[4] = {2,9,11,13};
// unsigned short config_frames[4] = {1};
unsigned char smbv_frame_cnt = 0;
u8 config_smbv = 0;
char smbv_ip[16];
#endif

#include "oaisim_functions.h"

#include "oaisim.h"
#include "oaisim_config.h"
#include "UTIL/OCG/OCG_extern.h"
#include "cor_SF_sim.h"
#include "UTIL/OMG/omg_constants.h"
#include "UTIL/FIFO/pad_list.h"

//#ifdef PROC
#include "../PROC/interface.h"
#include "../PROC/channel_sim_proc.h"
#include "../PROC/Tsync.h"
#include "../PROC/Process.h"
//#endif

#include "UTIL/LOG/vcd_signal_dumper.h"
#include "UTIL/OTG/otg_kpi.h"

#if defined(ENABLE_ITTI)
# include "intertask_interface_init.h"
# include "timer.h"
# if defined(ENABLE_USE_MME)
#   include "s1ap_eNB.h"
#   include "sctp_eNB_task.h"
# endif
#endif

#define RF

//#define DEBUG_SIM

#define MCS_COUNT 24//added for PHY abstraction
#define N_TRIALS 1

/*
 DCI0_5MHz_TDD0_t          UL_alloc_pdu;
 DCI1A_5MHz_TDD_1_6_t      CCCH_alloc_pdu;
 DCI2_5MHz_2A_L10PRB_TDD_t DLSCH_alloc_pdu1;
 DCI2_5MHz_2A_M10PRB_TDD_t DLSCH_alloc_pdu2;
 */

#define UL_RB_ALLOC computeRIV(lte_frame_parms->N_RB_UL,0,24)
#define CCCH_RB_ALLOC computeRIV(lte_frame_parms->N_RB_UL,0,3)
#define RA_RB_ALLOC computeRIV(lte_frame_parms->N_RB_UL,0,3)
#define DLSCH_RB_ALLOC 0x1fff

#define DECOR_DIST 100
#define SF_VAR 10

//constant for OAISIM soft realtime calibration
#define SF_DEVIATION_OFFSET_NS 100000 //= 0.1ms : should be as a number of UE
#define SLEEP_STEP_US       100 //  = 0.01ms could be adaptive, should be as a number of UE
#define K 2                  // averaging coefficient 
#define TARGET_SF_TIME_NS 1000000       // 1ms = 1000000 ns
//#ifdef OPENAIR2
//u16 NODE_ID[1];
//u8 NB_INST = 2;
//#endif //OPENAIR2
u32 frame = 0;
//int eMBMS_active = 0;
char stats_buffer[16384];
channel_desc_t *eNB2UE[NUMBER_OF_eNB_MAX][NUMBER_OF_UE_MAX];
channel_desc_t *UE2eNB[NUMBER_OF_UE_MAX][NUMBER_OF_eNB_MAX];
//Added for PHY abstraction
node_desc_t *enb_data[NUMBER_OF_eNB_MAX];
node_desc_t *ue_data[NUMBER_OF_UE_MAX];
// Added for PHY abstraction
extern Node_list ue_node_list;
extern Node_list enb_node_list;
extern int pdcp_period, omg_period;

extern double **s_re, **s_im, **r_re, **r_im, **r_re0, **r_im0;
int map1, map2;
extern double **ShaF;
double snr_dB, sinr_dB, snr_direction; //,sinr_direction;
extern double snr_step;
extern u8 set_sinr; //,set_snr=0;
extern u8 ue_connection_test;
extern u8 set_seed;
u8 cooperation_flag; // for cooperative communication
extern u8 target_dl_mcs;
extern u8 target_ul_mcs;
u8 rate_adaptation_flag;
extern u8 abstraction_flag, ethernet_flag;
extern u16 Nid_cell;
extern struct timespec time_spec;
extern unsigned long time_last, time_now;
extern int td, td_avg, sleep_time_us;
extern LTE_DL_FRAME_PARMS *frame_parms;
//double sinr_bler_map[MCS_COUNT][2][16];
//double sinr_bler_map_up[MCS_COUNT][2][16];
//extern double SINRpost_eff[301];
//extern int mcsPost; 
//extern int  nrbPost; 
//extern int frbPost;
//extern void kpi_gen();

#ifdef XFORMS
int otg_enabled;
#endif

// this should reflect the channel models in openair1/SIMULATION/TOOLS/defs.h
mapping small_scale_names[] = {
  {"custom", custom},
  {"SCM_A", SCM_A},
  {"SCM_B", SCM_B},
  {"SCM_C", SCM_C},
  {"SCM_D", SCM_D},
  {"EPA", EPA},
  {"EVA", EVA},
  {"ETU", ETU},
  {"MBSFN", MBSFN},
  {"Rayleigh8", Rayleigh8},
  {"Rayleigh1", Rayleigh1},
  {"Rayleigh1_800", Rayleigh1_800},
  {"Rayleigh1_corr", Rayleigh1_corr},
  {"Rayleigh1_anticorr", Rayleigh1_anticorr},
  {"Rice8", Rice8},
  {"Rice1", Rice1},
  {"Rice1_corr", Rice1_corr},
  {"Rice1_anticorr", Rice1_anticorr},
  {"AWGN", AWGN},
  {NULL, -1}
};

//static void *sigh(void *arg);
void terminate(void);

void help(void) {
  printf (
      "Usage: oaisim -h -a -F -C tdd_config -V -R N_RB_DL -e -x transmission_mode -m target_dl_mcs -r(ate_adaptation) -n n_frames -s snr_dB -k ricean_factor -t max_delay -f forgetting factor -A channel_model -z cooperation_flag -u nb_local_ue -U UE mobility -b nb_local_enb -B eNB_mobility -M ethernet_flag -p nb_master -g multicast_group -l log_level -c ocg_enable -T traffic model -D multicast network device\n");

  printf ("-h provides this help message!\n");
  printf ("-a Activates PHY abstraction mode\n");
  printf (
      "-A set the multipath channel simulation,  options are: SCM_A, SCM_B, SCM_C, SCM_D, EPA, EVA, ETU, Rayleigh8, Rayleigh1, Rayleigh1_corr,Rayleigh1_anticorr, Rice8,, Rice1, AWGN \n");
  printf ("-b Set the number of local eNB\n");
  printf ("-B Set the mobility model for eNB, options are: STATIC, RWP, RWALK, \n");
  printf (
      "-c [1,2,3,4] Activate the config generator (OCG) to process the scenario descriptor, or give the scenario manually: -c template_1.xml \n");
  printf ("-C [0-6] Sets TDD configuration\n");
  printf ("-e Activates extended prefix mode\n");
  printf ("-E Random number generator seed\n");
  printf ("-f Set the forgetting factor for time-variation\n");
  printf ("-F Activates FDD transmission (TDD is default)\n");
  printf ("-g Set multicast group ID (0,1,2,3) - valid if M is set\n");
  printf ("-G Enable background traffic \n");
  printf ("-H Enable handover operation (default disabled) \n");
  printf ("-I Enable CLI interface (to connect use telnet localhost 1352)\n");
  printf ("-k Set the Ricean factor (linear)\n");
  printf ("-l Set the global log level (8:trace, 7:debug, 6:info, 4:warn, 3:error) \n");
  printf ("-L [0-1] 0 to disable new link adaptation, 1 to enable new link adapatation\n");
  printf ("-m Gives a fixed DL mcs\n");
  printf ("-M Set the machine ID for Ethernet-based emulation\n");
  printf ("-n Set the number of frames for the simulation\n");
  printf ("-O [mme ipv4 address] Enable MME mode\n");
  printf ("-p Set the total number of machine in emulation - valid if M is set\n");
  printf ("-P [trace type] Enable protocol analyzer. Possible values for OPT:\n");
  printf ("    - wireshark: Enable tracing of layers above PHY using an UDP socket\n");
  printf ("    - pcap:      Enable tracing of layers above PHY to a pcap file\n");
  printf ("    - tshark:    Not implemented yet\n");
  printf (
      "-Q Activate and set the MBMS service: 0 : not used (default eMBMS disabled), 1: eMBMS and RRC Connection enabled, 2: eMBMS relaying and RRC Connection enabled, 3: eMBMS enabled, RRC Connection disabled, 4: eMBMS relaying enabled, RRC Connection disabled\n");
  printf ("-R [6,15,25,50,75,100] Sets N_RB_DL\n");
  printf ("-r Activates rate adaptation (DL for now)\n");
  printf ("-s snr_dB set a fixed (average) SNR, this deactivates the openair channel model generator (OCM)\n");
  printf ("-S snir_dB set a fixed (average) SNIR, this deactivates the openair channel model generator (OCM)\n");
  printf ("-t Set the delay spread (microseconds)\n");
  printf ("-T activate the traffic generator: cbr, mcbr, bcbr, mscbr \n");
  printf ("-u Set the number of local UE\n");
  printf ("-U Set the mobility model for UE, options are: STATIC, RWP, RWALK \n");
  printf ("-V Enable VCD dump, file = openair_vcd_dump.vcd\n");
  printf ("-w number of CBA groups, if not specified or zero, CBA is inactive\n");
  printf (
      "-W IP address to connect to SMBV and configure SMBV from config file. Requires compilation with SMBV=1, -W0 uses default IP 192.168.12.201\n");
  printf ("-x Set the transmission mode (1,2,5,6 supported for now)\n");
  printf ("-Y Set the global log verbosity (none, low, medium, high, full) \n");
  printf ("-z Set the cooperation flag (0 for no cooperation, 1 for delay diversity and 2 for distributed alamouti\n");
  printf ("-Z Reserved\n");
}

pthread_t log_thread;

void log_thread_init() {
  //create log_list
  //log_list_init(&log_list);
#ifndef LOG_NO_THREAD

  log_shutdown = 0;

  if ((pthread_mutex_init (&log_lock, NULL) != 0) || (pthread_cond_init (&log_notify, NULL) != 0)) {
    return;
  }

  if (pthread_create (&log_thread, NULL, log_thread_function, (void*) NULL) != 0) {
    log_thread_finalize ();
    return;
  }
#endif

}

//Call it after the last LOG call
int log_thread_finalize() {
  int err = 0;

#ifndef LOG_NO_THREAD

  if (pthread_mutex_lock (&log_lock) != 0) {
    return -1;
  }

  log_shutdown = 1;

  /* Wake up LOG thread */
  if ((pthread_cond_broadcast (&log_notify) != 0) || (pthread_mutex_unlock (&log_lock) != 0)) {
    err = -1;
  }
  if (pthread_join (log_thread, NULL) != 0) {
    err = -1;
  }
  if (pthread_mutex_unlock (&log_lock) != 0) {
    err = -1;
  }

  if (!err) {
    //log_list_free(&log_list);
    pthread_mutex_lock (&log_lock);
    pthread_mutex_destroy (&log_lock);
    pthread_cond_destroy (&log_notify);
  }
#endif

  return err;
}

#ifdef OPENAIR2
int omv_write(int pfd, Node_list enb_node_list, Node_list ue_node_list, Data_Flow_Unit omv_data) {
  int i, j;
  omv_data.end = 0;
  //omv_data.total_num_nodes = NB_UE_INST + NB_eNB_INST;
  for (i = 0; i < NB_eNB_INST; i++) {
    if (enb_node_list != NULL) {
      omv_data.geo[i].x = (enb_node_list->node->X_pos < 0.0) ? 0.0 : enb_node_list->node->X_pos;
      omv_data.geo[i].y = (enb_node_list->node->Y_pos < 0.0) ? 0.0 : enb_node_list->node->Y_pos;
      omv_data.geo[i].z = 1.0;
      omv_data.geo[i].mobility_type = oai_emulation.info.omg_model_enb;
      omv_data.geo[i].node_type = 0; //eNB
      enb_node_list = enb_node_list->next;
      omv_data.geo[i].Neighbors = 0;
      for (j = NB_eNB_INST; j < NB_UE_INST + NB_eNB_INST; j++) {
        if (is_UE_active (i, j - NB_eNB_INST) == 1) {
          omv_data.geo[i].Neighbor[omv_data.geo[i].Neighbors] = j;
          omv_data.geo[i].Neighbors++;
          LOG_D(
              OMG,
              "[eNB %d][UE %d] is_UE_active(i,j) %d geo (x%d, y%d) num neighbors %d\n", i, j-NB_eNB_INST, is_UE_active(i,j-NB_eNB_INST), omv_data.geo[i].x, omv_data.geo[i].y, omv_data.geo[i].Neighbors);
        }
      }
    }
  }
  for (i = NB_eNB_INST; i < NB_UE_INST + NB_eNB_INST; i++) {
    if (ue_node_list != NULL) {
      omv_data.geo[i].x = (ue_node_list->node->X_pos < 0.0) ? 0.0 : ue_node_list->node->X_pos;
      omv_data.geo[i].y = (ue_node_list->node->Y_pos < 0.0) ? 0.0 : ue_node_list->node->Y_pos;
      omv_data.geo[i].z = 1.0;
      omv_data.geo[i].mobility_type = oai_emulation.info.omg_model_ue;
      omv_data.geo[i].node_type = 1; //UE
      //trial
      omv_data.geo[i].state = 1;
      omv_data.geo[i].rnti = 88;
      omv_data.geo[i].connected_eNB = 0;
      omv_data.geo[i].RSRP = 66;
      omv_data.geo[i].RSRQ = 55;
      omv_data.geo[i].Pathloss = 44;
      omv_data.geo[i].RSSI[0] = 33;
      omv_data.geo[i].RSSI[1] = 22;
      if ((sizeof(omv_data.geo[0].RSSI) / sizeof(omv_data.geo[0].RSSI[0])) > 2)
      {
        omv_data.geo[i].RSSI[2] = 11;
      }

      ue_node_list = ue_node_list->next;
      omv_data.geo[i].Neighbors = 0;
      for (j = 0; j < NB_eNB_INST; j++) {
        if (is_UE_active (j, i - NB_eNB_INST) == 1) {
          omv_data.geo[i].Neighbor[omv_data.geo[i].Neighbors] = j;
          omv_data.geo[i].Neighbors++;
          LOG_D(
              OMG,
              "[UE %d][eNB %d] is_UE_active  %d geo (x%d, y%d) num neighbors %d\n", i-NB_eNB_INST, j, is_UE_active(j,i-NB_eNB_INST), omv_data.geo[i].x, omv_data.geo[i].y, omv_data.geo[i].Neighbors);
        }
      }
    }
  }
  LOG_E(OMG, "pfd %d \n", pfd);
  if (write (pfd, &omv_data, sizeof(struct Data_Flow_Unit)) == -1)
    perror ("write omv failed");
  return 1;
}

void omv_end(int pfd, Data_Flow_Unit omv_data) {
  omv_data.end = 1;
  if (write (pfd, &omv_data, sizeof(struct Data_Flow_Unit)) == -1)
    perror ("write omv failed");
}
#endif 

#ifdef OPENAIR2
int pfd[2]; // fd for omv : fixme: this could be a local var
#endif

#ifdef OPENAIR2
static Data_Flow_Unit omv_data;
#endif //ALU
static s32 UE_id = 0, eNB_id = 0;
#ifdef Rel10
static s32 RN_id=0;
#endif

int itti_create_task_successful(void){

#if defined(ENABLE_ITTI)
# if defined(ENABLE_USE_MME)
  if (itti_create_task(TASK_SCTP, sctp_eNB_task, NULL) < 0) {
      LOG_E(EMU, "Create task failed");
      LOG_D(EMU, "Initializing SCTP task interface: FAILED\n");
      return -1;
  }
  if (itti_create_task(TASK_S1AP, s1ap_eNB_task, NULL) < 0) {
      LOG_E(EMU, "Create task failed");
      LOG_D(EMU, "Initializing S1AP task interface: FAILED\n");
      return -1;
  }
# endif

  if (itti_create_task(TASK_ENB_APP, eNB_app_task, NULL) < 0) {
    LOG_E(EMU, "Create task failed");
    LOG_D(EMU, "Initializing eNB APP task interface: FAILED\n");
    return -1;
  }

  if (itti_create_task(TASK_L2L1, l2l1_task, NULL) < 0) {
    LOG_E(EMU, "Create task failed");
    LOG_D(EMU, "Initializing L2L1 task interface: FAILED\n");
    return -1;
  }

#endif  
  return 1;
}
/*
 * later, the enb task will be moved from here
 */
void *eNB_app_task(void *args_p) {
#if defined(ENABLE_ITTI)
  MessageDef *message_p;

# if defined(ENABLE_USE_MME)
    /* Trying to register each eNB */

  for (eNB_id = oai_emulation.info.first_enb_local;
       (eNB_id < (oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local)) && (oai_emulation.info.cli_start_enb[eNB_id] == 1);
       eNB_id++) {
      char *mme_address_v4;
      
      if (EPC_MODE_ENABLED){
	  mme_address_v4 = EPC_MODE_MME_ADDRESS;
      }else {
	mme_address_v4 = "192.168.12.87";
      }
      char *mme_address_v6 = "2001:660:5502:12:30da:829a:2343:b6cf";
      s1ap_register_eNB_t *s1ap_register_eNB;
      uint32_t hash;
      
      //note:  there is an implicit relationship between the data struct and the message name
      message_p = itti_alloc_new_message(TASK_ENB_APP, S1AP_REGISTER_ENB);
      
      s1ap_register_eNB = &message_p->msg.s1ap_register_eNB;
      
      hash = s1ap_generate_eNB_id();

      /* Some default/random parameters */
      s1ap_register_eNB->eNB_id      = eNB_id + (hash & 0xFFFF8);
      s1ap_register_eNB->cell_type   = CELL_MACRO_ENB;
      s1ap_register_eNB->tac         = 0;
      s1ap_register_eNB->mcc         = 208;
      s1ap_register_eNB->mnc         = 34;
      s1ap_register_eNB->default_drx = PAGING_DRX_256;
      s1ap_register_eNB->nb_mme      = 1;
      s1ap_register_eNB->mme_ip_address[0].ipv4 = 1;
      s1ap_register_eNB->mme_ip_address[0].ipv6 = 0;
      memcpy(s1ap_register_eNB->mme_ip_address[0].ipv4_address, mme_address_v4,
	     strlen(mme_address_v4));
      memcpy(s1ap_register_eNB->mme_ip_address[0].ipv6_address, mme_address_v6,
	     strlen(mme_address_v6));
      
      itti_send_msg_to_task(TASK_S1AP, eNB_id, message_p);
    }
# endif

    itti_mark_task_ready (TASK_ENB_APP); // at the end of init for the current task 

  do {
    // Checks if a message has been sent to L2L1 task
    itti_receive_msg (TASK_ENB_APP, &message_p);
    
    if (message_p != NULL) {
      switch (ITTI_MSG_ID(message_p)) {
      case TERMINATE_MESSAGE:
	itti_exit_task ();
	break;
	
      case MESSAGE_TEST:
	LOG_D(EMU, "Received %s\n", ITTI_MSG_NAME(message_p));
	break;

	/*      case MME_REGISTERED:
	LOG_D(EMU, "Received %s\n", ITTI_MSG_NAME(message_p));
	itti_mark_task_ready (TASK_L2L1);
	break;
	*/
      default:
	LOG_E(EMU, "Received unexpected message %s\n", ITTI_MSG_NAME(message_p));
	break;
      }
      
      free (message_p);
    }
  } while(1);

  itti_terminate_tasks(TASK_ENB_APP);

  return NULL;
#endif

}
void *l2l1_task(void *args_p) {
  // Framing variables
  s32 slot, last_slot, next_slot;

#ifdef Rel10
  relaying_type_t r_type=no_relay; // no relaying
#endif

  lte_subframe_t direction;

  char fname[64], vname[64];

#if defined(ENABLE_ITTI)
  MessageDef *message_p;

  itti_mark_task_ready (TASK_L2L1);

        if (EPC_MODE_ENABLED)
        {
            mme_address_v4 = EPC_MODE_MME_ADDRESS;
        }
        else
        {
            /* FIXME: acquire MMEs IP address by XML file or command line */
            mme_address_v4 = "192.168.12.87";
        }
        char *mme_address_v6 = "2001:660:5502:12:30da:829a:2343:b6cf";
        s1ap_register_eNB_t *s1ap_register_eNB;
        uint32_t hash;

        /* FIXME: following parameters should be setup by eNB applicative layer ? */
        message_p = itti_alloc_new_message(TASK_L2L1, S1AP_REGISTER_ENB);

        s1ap_register_eNB = &message_p->ittiMsg.s1ap_register_eNB;

        hash = s1ap_generate_eNB_id();

        /* Some default/random parameters */
        s1ap_register_eNB->eNB_id      = eNB_id + (hash & 0xFFFF8);
        s1ap_register_eNB->cell_type   = CELL_MACRO_ENB;
        s1ap_register_eNB->tac         = 0;
        s1ap_register_eNB->mcc         = 208;
        s1ap_register_eNB->mnc         = 34;
        s1ap_register_eNB->default_drx = PAGING_DRX_256;
        s1ap_register_eNB->nb_mme      = 1;
        s1ap_register_eNB->mme_ip_address[0].ipv4 = 1;
        s1ap_register_eNB->mme_ip_address[0].ipv6 = 0;
        memcpy(s1ap_register_eNB->mme_ip_address[0].ipv4_address, mme_address_v4,
               strlen(mme_address_v4));
        memcpy(s1ap_register_eNB->mme_ip_address[0].ipv6_address, mme_address_v6,
               strlen(mme_address_v6));

        itti_send_msg_to_task(TASK_S1AP, eNB_id, message_p);
    }
#endif

  for (frame = 0; frame < oai_emulation.info.n_frames; frame++) {

#if defined(ENABLE_ITTI)
    do {
      // Checks if a message has been sent to L2L1 task
      itti_poll_msg (TASK_L2L1, &message_p);

      if (message_p != NULL) {
        switch (ITTI_MSG_ID(message_p)) {
          case TERMINATE_MESSAGE:
            itti_exit_task ();
            break;

          case MESSAGE_TEST:
            LOG_I(EMU, "Received %s\n", ITTI_MSG_NAME(message_p));
            break;

          default:
            LOG_E(EMU, "Received unexpected message %s\n", ITTI_MSG_NAME(message_p));
            break;
        }

        free (message_p);
      }
    } while(message_p != NULL);
#endif

    /*
     // Handling the cooperation Flag
     if (cooperation_flag == 2)
     {
     if ((PHY_vars_eNB_g[0]->eNB_UE_stats[0].mode == PUSCH) && (PHY_vars_eNB_g[0]->eNB_UE_stats[1].mode == PUSCH))
     PHY_vars_eNB_g[0]->cooperation_flag = 2;
     }
     */
    if (ue_connection_test == 1) {
      if ((frame % 20) == 0) {
        snr_dB += snr_direction;
        sinr_dB -= snr_direction;
      }
      if (snr_dB == -20) {
        snr_direction = snr_step;
      }
      else
        if (snr_dB == 20) {
          snr_direction = -snr_step;
        }
    }

    oai_emulation.info.frame = frame;
    //oai_emulation.info.time_ms += 1;
    oai_emulation.info.time_s += 0.1; // emu time in s, each frame lasts for 10 ms // JNote: TODO check the coherency of the time and frame (I corrected it to 10 (instead of 0.01)
    // if n_frames not set by the user or is greater than max num frame then set adjust the frame counter
    if ((oai_emulation.info.n_frames_flag == 0) || (oai_emulation.info.n_frames >= 0xffff)) {
      frame %= (oai_emulation.info.n_frames - 1);
    }

    update_omg (); // frequency is defined in the omg_global params configurable by the user

    update_omg_ocm ();

#ifdef OPENAIR2
    // check if pipe is still open
    if ((oai_emulation.info.omv_enabled == 1)) {
      omv_write (pfd[1], enb_node_list, ue_node_list, omv_data);
    }
#endif
#ifdef DEBUG_OMG
    if ((((int) oai_emulation.info.time_s) % 100) == 0) {
      for (UE_id = oai_emulation.info.first_ue_local; UE_id < (oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local); UE_id++) {
        get_node_position (UE, UE_id);
      }
    }
#endif

    update_ocm ();

    for (slot = 0; slot < 20; slot++) {

      wait_for_slot_isr ();

#if defined(ENABLE_ITTI)
      itti_update_lte_time(frame, slot);
#endif

      last_slot = (slot - 1) % 20;
      if (last_slot < 0)
        last_slot += 20;
      next_slot = (slot + 1) % 20;

      oai_emulation.info.time_ms = frame * 10 + (slot >> 1);

      direction = subframe_select (frame_parms, next_slot >> 1);

#ifdef PROC
      if(Channel_Flag==1)
      Channel_Func(s_re2,s_im2,r_re2,r_im2,r_re02,r_im02,r_re0_d,r_im0_d,r_re0_u,r_im0_u,eNB2UE,UE2eNB,enb_data,ue_data,abstraction_flag,frame_parms,slot);

      if(Channel_Flag==0)
#endif
      {
        if ((next_slot % 2) == 0)
          clear_eNB_transport_info (oai_emulation.info.nb_enb_local);

        for (eNB_id = oai_emulation.info.first_enb_local;
            (eNB_id < (oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local))
                && (oai_emulation.info.cli_start_enb[eNB_id] == 1); eNB_id++) {
          //printf ("debug: Nid_cell %d\n", PHY_vars_eNB_g[eNB_id]->lte_frame_parms.Nid_cell);
          //printf ("debug: frame_type %d,tdd_config %d\n", PHY_vars_eNB_g[eNB_id]->lte_frame_parms.frame_type,PHY_vars_eNB_g[eNB_id]->lte_frame_parms.tdd_config);
          LOG_D(
              EMU,
              "PHY procedures eNB %d for frame %d, slot %d (subframe TX %d, RX %d) TDD %d/%d Nid_cell %d\n", eNB_id, frame, slot, next_slot >> 1, last_slot>>1, PHY_vars_eNB_g[eNB_id]->lte_frame_parms.frame_type, PHY_vars_eNB_g[eNB_id]->lte_frame_parms.tdd_config, PHY_vars_eNB_g[eNB_id]->lte_frame_parms.Nid_cell);

#ifdef OPENAIR2
          //Appliation: traffic gen
          update_otg_eNB (eNB_id, oai_emulation.info.time_ms);

          //IP/OTG to PDCP and PDCP to IP operation
          pdcp_run (frame, 1, 0, eNB_id); //PHY_vars_eNB_g[eNB_id]->Mod_id
#endif

          // PHY_vars_eNB_g[eNB_id]->frame = frame;
          phy_procedures_eNB_lte (last_slot, next_slot, PHY_vars_eNB_g[eNB_id], abstraction_flag, no_relay, NULL);

#ifdef PRINT_STATS
          if(last_slot==9 && frame%10==0)
          if(eNB_avg_thr)
          fprintf(eNB_avg_thr,"%d %d\n",PHY_vars_eNB_g[eNB_id]->frame,(PHY_vars_eNB_g[eNB_id]->total_system_throughput)/((PHY_vars_eNB_g[eNB_id]->frame+1)*10));
          if (eNB_stats[eNB_id]) {
            len = dump_eNB_stats(PHY_vars_eNB_g[eNB_id], stats_buffer, 0);
            rewind (eNB_stats[eNB_id]);
            fwrite (stats_buffer, 1, len, eNB_stats[eNB_id]);
            fflush(eNB_stats[eNB_id]);
          }
#ifdef OPENAIR2
          if (eNB_l2_stats) {
            len = dump_eNB_l2_stats (stats_buffer, 0);
            rewind (eNB_l2_stats);
            fwrite (stats_buffer, 1, len, eNB_l2_stats);
            fflush(eNB_l2_stats);
          }
#endif
#endif
        }
        // Call ETHERNET emulation here
        //emu_transport (frame, last_slot, next_slot, direction, oai_emulation.info.frame_type, ethernet_flag);

        if ((next_slot % 2) == 0)
          clear_UE_transport_info (oai_emulation.info.nb_ue_local);

        for (UE_id = oai_emulation.info.first_ue_local;
            (UE_id < (oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local))
                && (oai_emulation.info.cli_start_ue[UE_id] == 1); UE_id++) {
          if (frame >= (UE_id * 20)) { // activate UE only after 20*UE_id frames so that different UEs turn on separately

            LOG_D(
                EMU,
                "PHY procedures UE %d for frame %d, slot %d (subframe TX %d, RX %d)\n", UE_id, frame, slot, next_slot >> 1, last_slot>>1);

            if (PHY_vars_UE_g[UE_id]->UE_mode[0] != NOT_SYNCHED) {
              if (frame > 0) {
                PHY_vars_UE_g[UE_id]->frame = frame;

#ifdef OPENAIR2
                //Application
                update_otg_UE (UE_id, oai_emulation.info.time_ms);

                //Access layer
                pdcp_run (frame, 0, UE_id, 0);
#endif

                phy_procedures_UE_lte (last_slot, next_slot, PHY_vars_UE_g[UE_id], 0, abstraction_flag, normal_txrx,
                                       no_relay, NULL);
                ue_data[UE_id]->tx_power_dBm = PHY_vars_UE_g[UE_id]->tx_power_dBm;
              }
            }
            else {
              if (abstraction_flag == 1) {
                LOG_E(
                    EMU,
                    "sync not supported in abstraction mode (UE%d,mode%d)\n", UE_id, PHY_vars_UE_g[UE_id]->UE_mode[0]);
                exit (-1);
              }
              if ((frame > 0) && (last_slot == (LTE_SLOTS_PER_FRAME - 2))) {
                initial_sync (PHY_vars_UE_g[UE_id], normal_txrx);

                /*
                 write_output("dlchan00.m","dlch00",&(PHY_vars_UE_g[0]->lte_ue_common_vars.dl_ch_estimates[0][0][0]),(6*(PHY_vars_UE_g[0]->lte_frame_parms.ofdm_symbol_size)),1,1);
                 if (PHY_vars_UE_g[0]->lte_frame_parms.nb_antennas_rx>1)
                 write_output("dlchan01.m","dlch01",&(PHY_vars_UE_g[0]->lte_ue_common_vars.dl_ch_estimates[0][1][0]),(6*(PHY_vars_UE_g[0]->lte_frame_parms.ofdm_symbol_size)),1,1);
                 write_output("dlchan10.m","dlch10",&(PHY_vars_UE_g[0]->lte_ue_common_vars.dl_ch_estimates[0][2][0]),(6*(PHY_vars_UE_g[0]->lte_frame_parms.ofdm_symbol_size)),1,1);
                 if (PHY_vars_UE_g[0]->lte_frame_parms.nb_antennas_rx>1)
                 write_output("dlchan11.m","dlch11",&(PHY_vars_UE_g[0]->lte_ue_common_vars.dl_ch_estimates[0][3][0]),(6*(PHY_vars_UE_g[0]->lte_frame_parms.ofdm_symbol_size)),1,1);
                 write_output("rxsig.m","rxs",PHY_vars_UE_g[0]->lte_ue_common_vars.rxdata[0],PHY_vars_UE_g[0]->lte_frame_parms.samples_per_tti*10,1,1);
                 write_output("rxsigF.m","rxsF",PHY_vars_UE_g[0]->lte_ue_common_vars.rxdataF[0],2*PHY_vars_UE_g[0]->lte_frame_parms.symbols_per_tti*PHY_vars_UE_g[0]->lte_frame_parms.ofdm_symbol_size,2,1);
                 write_output("pbch_rxF_ext0.m","pbch_ext0",PHY_vars_UE_g[0]->lte_ue_pbch_vars[0]->rxdataF_ext[0],6*12*4,1,1);
                 write_output("pbch_rxF_comp0.m","pbch_comp0",PHY_vars_UE_g[0]->lte_ue_pbch_vars[0]->rxdataF_comp[0],6*12*4,1,1);
                 write_output("pbch_rxF_llr.m","pbch_llr",PHY_vars_UE_g[0]->lte_ue_pbch_vars[0]->llr,(frame_parms->Ncp==0) ? 1920 : 1728,1,4);
                 */
              }
            }
#ifdef PRINT_STATS
            if(last_slot==2 && frame%10==0)
            if (UE_stats_th[UE_id])
            fprintf(UE_stats_th[UE_id],"%d %d\n",frame, PHY_vars_UE_g[UE_id]->bitrate[0]/1000);
            if (UE_stats[UE_id]) {
              len = dump_ue_stats (PHY_vars_UE_g[UE_id], stats_buffer, 0, normal_txrx, 0);
              rewind (UE_stats[UE_id]);
              fwrite (stats_buffer, 1, len, UE_stats[UE_id]);
              fflush(UE_stats[UE_id]);
            }
#endif
          }
        }
#ifdef Rel10
        for (RN_id=oai_emulation.info.first_rn_local;
            RN_id<oai_emulation.info.first_rn_local+oai_emulation.info.nb_rn_local;
            RN_id++) {
          // UE id and eNB id of the RN
          UE_id= oai_emulation.info.first_ue_local+oai_emulation.info.nb_ue_local + RN_id;// NB_UE_INST + RN_id
          eNB_id= oai_emulation.info.first_enb_local+oai_emulation.info.nb_enb_local + RN_id;// NB_eNB_INST + RN_id
          // currently only works in FDD
          if (oai_emulation.info.eMBMS_active_state == 4) {
            r_type = multicast_relay;
            //LOG_I(EMU,"Activating the multicast relaying\n");
          }
          else {
            LOG_E(EMU,"Not supported eMBMS option when relaying is enabled %d\n", r_type);
            exit(-1);
          }
          PHY_vars_RN_g[RN_id]->frame = frame;
          if ( oai_emulation.info.frame_type == 0) {
            // RN == UE
            if (frame>0) {
              if (PHY_vars_UE_g[UE_id]->UE_mode[0] != NOT_SYNCHED) {
                LOG_D(EMU,"[RN %d] PHY procedures UE %d for frame %d, slot %d (subframe TX %d, RX %d)\n",
                    RN_id, UE_id, frame, slot, next_slot >> 1,last_slot>>1);
                PHY_vars_UE_g[UE_id]->frame = frame;
                phy_procedures_UE_lte (last_slot, next_slot, PHY_vars_UE_g[UE_id], 0, abstraction_flag,normal_txrx,
                    r_type, PHY_vars_RN_g[RN_id]);
              }
              else if (last_slot == (LTE_SLOTS_PER_FRAME-2)) {
                initial_sync(PHY_vars_UE_g[UE_id],normal_txrx);
              }
            }
            // RN == eNB
            LOG_D(EMU,"[RN %d] PHY procedures eNB %d for frame %d, slot %d (subframe TX %d, RX %d)\n",
                RN_id, eNB_id, frame, slot, next_slot >> 1,last_slot>>1);
            phy_procedures_eNB_lte (last_slot, next_slot, PHY_vars_eNB_g[eNB_id], abstraction_flag,
                r_type, PHY_vars_RN_g[RN_id]);
          }
          else {
            LOG_E(EMU,"TDD is not supported for multicast relaying %d\n", r_type);
            exit(-1);
          }
        }
#endif
        emu_transport (frame, last_slot, next_slot, direction, oai_emulation.info.frame_type, ethernet_flag);
        if ((direction == SF_DL) || (frame_parms->frame_type == 0)) {
          // consider only sec id 0
          /*  for (eNB_id=0;eNB_id<NB_eNB_INST;eNB_id++) {
           if (abstraction_flag == 0) {
           do_OFDM_mod(PHY_vars_eNB_g[eNB_id]->lte_eNB_common_vars.txdataF[0],
           PHY_vars_eNB_g[eNB_id]->lte_eNB_common_vars.txdata[0],
           frame,next_slot,
           frame_parms);
           }
           }*/
          for (UE_id = 0; UE_id < NB_UE_INST; UE_id++) {
            do_DL_sig (r_re0, r_im0, r_re, r_im, s_re, s_im, eNB2UE, enb_data, ue_data, next_slot, abstraction_flag,
                       frame_parms, UE_id);
          }
        }
        if ((direction == SF_UL) || (frame_parms->frame_type == 0)) { //if ((subframe<2) || (subframe>4))
          do_UL_sig (r_re0, r_im0, r_re, r_im, s_re, s_im, UE2eNB, enb_data, ue_data, next_slot, abstraction_flag,
                     frame_parms, frame);
          /*
           int ccc;
           fprintf(SINRpost,"SINRdb For eNB New Subframe : \n ");
           for(ccc = 0 ; ccc<301; ccc++)
           {
           fprintf(SINRpost,"_ %f ", SINRpost_eff[ccc]);
           }
           fprintf(SINRpost,"SINRdb For eNB : %f \n ", SINRpost_eff[ccc]);
           */
        }
        if ((direction == SF_S)) { //it must be a special subframe
          if (next_slot % 2 == 0) { //DL part
            /*  for (eNB_id=0;eNB_id<NB_eNB_INST;eNB_id++) {
             if (abstraction_flag == 0) {
             do_OFDM_mod(PHY_vars_eNB_g[eNB_id]->lte_eNB_common_vars.txdataF[0],
             PHY_vars_eNB_g[eNB_id]->lte_eNB_common_vars.txdata[0],
             frame,next_slot,
             frame_parms);
             }
             }*/
            for (UE_id = 0; UE_id < NB_UE_INST; UE_id++) {
              do_DL_sig (r_re0, r_im0, r_re, r_im, s_re, s_im, eNB2UE, enb_data, ue_data, next_slot, abstraction_flag,
                         frame_parms, UE_id);
            }
            /*
             for (aarx=0;aarx<UE2eNB[1][0]->nb_rx;aarx++)
             for (aatx=0;aatx<UE2eNB[1][0]->nb_tx;aatx++)
             for (k=0;k<UE2eNB[1][0]->channel_length;k++)
             printf("SB(%d,%d,%d)->(%f,%f)\n",k,aarx,aatx,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].r,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].i);
             */
          }
          else { // UL part
            do_UL_sig (r_re0, r_im0, r_re, r_im, s_re, s_im, UE2eNB, enb_data, ue_data, next_slot, abstraction_flag,
                       frame_parms, frame);

            /*        int ccc;
             fprintf(SINRpost,"SINRdb For eNB New Subframe : \n ");
             for(ccc = 0 ; ccc<301; ccc++)
             {
             fprintf(SINRpost,"_ %f ", SINRpost_eff[ccc]);
             }
             fprintf(SINRpost,"SINRdb For eNB : %f \n ", SINRpost_eff[ccc]);
             }
             */}
        }
        if ((last_slot == 1) && (frame == 0) && (abstraction_flag == 0) && (oai_emulation.info.n_frames == 1)) {

          write_output ("dlchan0.m", "dlch0", &(PHY_vars_UE_g[0]->lte_ue_common_vars.dl_ch_estimates[0][0][0]),
                        (6 * (PHY_vars_UE_g[0]->lte_frame_parms.ofdm_symbol_size)), 1, 1);
          write_output ("dlchan1.m", "dlch1", &(PHY_vars_UE_g[0]->lte_ue_common_vars.dl_ch_estimates[1][0][0]),
                        (6 * (PHY_vars_UE_g[0]->lte_frame_parms.ofdm_symbol_size)), 1, 1);
          write_output ("dlchan2.m", "dlch2", &(PHY_vars_UE_g[0]->lte_ue_common_vars.dl_ch_estimates[2][0][0]),
                        (6 * (PHY_vars_UE_g[0]->lte_frame_parms.ofdm_symbol_size)), 1, 1);
          write_output ("pbch_rxF_comp0.m", "pbch_comp0", PHY_vars_UE_g[0]->lte_ue_pbch_vars[0]->rxdataF_comp[0],
                        6 * 12 * 4, 1, 1);
          write_output ("pbch_rxF_llr.m", "pbch_llr", PHY_vars_UE_g[0]->lte_ue_pbch_vars[0]->llr,
                        (frame_parms->Ncp == 0) ? 1920 : 1728, 1, 4);
        }
        /*
         if ((last_slot==1) && (frame==1)) {
         write_output("dlsch_rxF_comp0.m","dlsch0_rxF_comp0",PHY_vars_UE->lte_ue_pdsch_vars[eNB_id]->rxdataF_comp[0],300*(-(PHY_vars_UE->lte_frame_parms.Ncp*2)+14),1,1);
         write_output("pdcch_rxF_comp0.m","pdcch0_rxF_comp0",PHY_vars_UE->lte_ue_pdcch_vars[eNB_id]->rxdataF_comp[0],4*300,1,1);
         }
         */

        if (next_slot % 2 == 0) {
          clock_gettime (CLOCK_REALTIME, &time_spec);
          time_last = time_now;
          time_now = (unsigned long) time_spec.tv_nsec;
          td = (int) (time_now - time_last);
          if (td > 0) {
            td_avg = (int) (((K * (long) td) + (((1 << 3) - K) * ((long) td_avg))) >> 3); // in us
            LOG_T(
                EMU,
                "sleep frame %d, time_now %ldus,time_last %ldus,average time difference %ldns, CURRENT TIME DIFF %dus, avgerage difference from the target %dus\n", frame, time_now, time_last, td_avg, td/1000, (td_avg-TARGET_SF_TIME_NS)/1000);
          }
          if (td_avg < (TARGET_SF_TIME_NS - SF_DEVIATION_OFFSET_NS)) {
            sleep_time_us += SLEEP_STEP_US;
            LOG_D(EMU, "Faster than realtime increase the avg sleep time for %d us, frame %d\n", sleep_time_us, frame);
            // LOG_D(EMU,"Faster than realtime increase the avg sleep time for %d us, frame %d, time_now %ldus,time_last %ldus,average time difference %ldns, CURRENT TIME DIFF %dus, avgerage difference from the target %dus\n",    sleep_time_us,frame, time_now,time_last,td_avg, td/1000,(td_avg-TARGET_SF_TIME_NS)/1000);
          }
          else
            if (td_avg > (TARGET_SF_TIME_NS + SF_DEVIATION_OFFSET_NS)) {
              sleep_time_us -= SLEEP_STEP_US;
              LOG_D(
                  EMU,
                  "Slower than realtime reduce the avg sleep time for %d us, frame %d, time_now\n", sleep_time_us, frame);
              //LOG_T(EMU,"Slower than realtime reduce the avg sleep time for %d us, frame %d, time_now %ldus,time_last %ldus,average time difference %ldns, CURRENT TIME DIFF %dus, avgerage difference from the target %dus\n",     sleep_time_us,frame, time_now,time_last,td_avg, td/1000,(td_avg-TARGET_SF_TIME_NS)/1000);
            }
        } // end if next_slot%2
      } // if Channel_Flag==0

    } //end of slot

    if ((frame >= 1) && (frame <= 9) && (abstraction_flag == 0)
#ifdef PROC
        &&(Channel_Flag==0)
#endif
        ) {
      write_output ("UEtxsig0.m", "txs0", PHY_vars_UE_g[0]->lte_ue_common_vars.txdata[0], FRAME_LENGTH_COMPLEX_SAMPLES,
                    1, 1);
      sprintf (fname, "eNBtxsig%d.m", frame);
      sprintf (vname, "txs%d", frame);
      write_output (fname, vname, PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][0], FRAME_LENGTH_COMPLEX_SAMPLES, 1,
                    1);
      write_output (
          "eNBtxsigF0.m", "txsF0", PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdataF[0][0],
          PHY_vars_eNB_g[0]->lte_frame_parms.symbols_per_tti * PHY_vars_eNB_g[0]->lte_frame_parms.ofdm_symbol_size, 1,
          1);

      write_output ("UErxsig0.m", "rxs0", PHY_vars_UE_g[0]->lte_ue_common_vars.rxdata[0], FRAME_LENGTH_COMPLEX_SAMPLES,
                    1, 1);
      write_output ("eNBrxsig0.m", "rxs0", PHY_vars_eNB_g[0]->lte_eNB_common_vars.rxdata[0][0],
                    FRAME_LENGTH_COMPLEX_SAMPLES, 1, 1);
    }

#ifdef XFORMS
    eNB_id = 0;
    for (UE_id = 0; UE_id < NB_UE_INST; UE_id++) {
      phy_scope_UE(form_ue[UE_id],
          PHY_vars_UE_g[UE_id],
          eNB_id,
          UE_id,
          7);

      phy_scope_eNB(form_enb[UE_id],
          PHY_vars_eNB_g[eNB_id],
          UE_id);

    }
#endif

    // calibrate at the end of each frame if there is some time  left
    if ((sleep_time_us > 0) && (ethernet_flag == 0)) {
      LOG_I(EMU, "[TIMING] Adjust average frame duration, sleep for %d us\n", sleep_time_us);
      usleep (sleep_time_us);
      sleep_time_us = 0; // reset the timer, could be done per n SF
    }
#ifdef SMBV
    if ((frame == config_frames[0]) || (frame == config_frames[1]) || (frame == config_frames[2]) || (frame == config_frames[3])) {
      smbv_frame_cnt++;
    }
#endif
  } //end of frame

#if defined(ENABLE_ITTI)
  itti_terminate_tasks(TASK_L2L1);
#endif

  return NULL;
}

Packet_OTG_List *otg_pdcp_buffer;

int main(int argc, char **argv) {

  s32 i;
  // pointers signal buffers (s = transmit, r,r0 = receive)
  clock_t t;

  //FILE *SINRpost;
  //char SINRpost_fname[512];
  // sprintf(SINRpost_fname,"postprocSINR.m");
  //SINRpost = fopen(SINRpost_fname,"w");
  // variables/flags which are set by user on command-line

#ifdef SMBV
  strcpy(smbv_ip,DEFAULT_SMBV_IP);
#endif

  // time calibration for soft realtime mode  

  // u8 awgn_flag = 0;
#ifdef XFORMS
  // current status is that every UE has a DL scope for a SINGLE eNB (eNB_id=0)
  // at eNB 0, an UL scope for every UE 
  FD_lte_phy_scope_ue *form_ue[NUMBER_OF_UE_MAX];
  FD_lte_phy_scope_enb *form_enb[NUMBER_OF_UE_MAX];
  char title[255];
#endif

#ifdef PRINT_STATS
  int len;
  FILE *UE_stats[NUMBER_OF_UE_MAX], *UE_stats_th[NUMBER_OF_UE_MAX], *eNB_stats[NUMBER_OF_eNB_MAX], *eNB_avg_thr, *eNB_l2_stats;
  char UE_stats_filename[255];
  char eNB_stats_filename[255];
  char UE_stats_th_filename[255];
  char eNB_stats_th_filename[255];
#endif

  //time_t t0,t1;
  //clock_t start, stop;

#ifdef PROC
  int node_id;
  int port,Process_Flag=0,wgt,Channel_Flag=0,temp;
#endif
  //double **s_re2[MAX_eNB+MAX_UE], **s_im2[MAX_eNB+MAX_UE], **r_re2[MAX_eNB+MAX_UE], **r_im2[MAX_eNB+MAX_UE], **r_re02, **r_im02;
  //double **r_re0_d[MAX_UE][MAX_eNB], **r_im0_d[MAX_UE][MAX_eNB], **r_re0_u[MAX_eNB][MAX_UE],**r_im0_u[MAX_eNB][MAX_UE];
  //default parameters
  rate_adaptation_flag = 0;
  oai_emulation.info.n_frames = 0xffff; //1024;          //10;
  oai_emulation.info.n_frames_flag = 0; //fixme
  snr_dB = 30;
  cooperation_flag = 0; // default value 0 for no cooperation, 1 for Delay diversity, 2 for Distributed Alamouti

  //Default values if not changed by the user in get_simulation_options();
  pdcp_period = 1;
  omg_period = 1;
  // start thread for log gen
  log_thread_init ();

  init_oai_emulation (); // to initialize everything !!!

  // get command-line options
  get_simulation_options (argc, argv); //Command-line options
  
#if defined(ENABLE_ITTI)
  itti_init(TASK_MAX, THREAD_MAX, MESSAGES_ID_MAX, tasks_info, messages_info, messages_definition_xml, oai_emulation.info.itti_dump_file);
#endif

  // Initialize VCD LOG module
  vcd_signal_dumper_init ("openair_dump.vcd");

  /*  pthread_t sigth;
   sigset_t sigblock;
   sigemptyset(&sigblock);
   sigaddset(&sigblock, SIGHUP);
   sigaddset(&sigblock, SIGINT);
   sigaddset(&sigblock, SIGTERM);
   pthread_sigmask(SIG_BLOCK, &sigblock, NULL);
   if (pthread_create(&sigth, NULL, sigh, NULL)) {
   msg("Pthread for tracing Signals is not created!\n");
   return -1;
   } else {
   msg("Pthread for tracing Signals is created!\n");
   }*/
  // configure oaisim with OCG
  oaisim_config (); // config OMG and OCG, OPT, OTG, OLG

  if (ue_connection_test == 1) {
    snr_direction = -snr_step;
    snr_dB = 20;
    sinr_dB = -20;
  }

#ifdef OPENAIR2
  init_omv ();
#endif
  //Before this call, NB_UE_INST and NB_eNB_INST are not set correctly

  check_and_adjust_params ();

#ifdef PRINT_STATS
  for (UE_id=0;UE_id<NB_UE_INST;UE_id++) {
    sprintf(UE_stats_filename,"UE_stats%d.txt",UE_id);
    UE_stats[UE_id] = fopen (UE_stats_filename, "w");
  }
  for (eNB_id=0;eNB_id<NB_eNB_INST;eNB_id++) {
    sprintf(eNB_stats_filename,"eNB_stats%d.txt",eNB_id);
    eNB_stats[eNB_id] = fopen (eNB_stats_filename, "w");
  }

  if(abstraction_flag==0) {
    for (UE_id=0;UE_id<NB_UE_INST;UE_id++) {
      sprintf(UE_stats_th_filename,"UE_stats_th%d_tx%d.txt",UE_id,oai_emulation.info.transmission_mode);
      UE_stats_th[UE_id] = fopen (UE_stats_th_filename, "w");
    }
    sprintf(eNB_stats_th_filename,"eNB_stats_th_tx%d.txt",oai_emulation.info.transmission_mode);
    eNB_avg_thr = fopen (eNB_stats_th_filename, "w");
  }
  else {
    for (UE_id=0;UE_id<NB_UE_INST;UE_id++) {
      sprintf(UE_stats_th_filename,"UE_stats_abs_th%d_tx%d.txt",UE_id,oai_emulation.info.transmission_mode);
      UE_stats_th[UE_id] = fopen (UE_stats_th_filename, "w");
    }
    sprintf(eNB_stats_th_filename,"eNB_stats_abs_th_tx%d.txt",oai_emulation.info.transmission_mode);
    eNB_avg_thr = fopen (eNB_stats_th_filename, "w");
  }
#ifdef OPENAIR2
  eNB_l2_stats = fopen ("eNB_l2_stats.txt", "w");
  LOG_I(EMU,"eNB_l2_stats=%p\n", eNB_l2_stats);
#endif 

#endif

  set_seed = oai_emulation.emulation_config.seed.value;

  init_otg_pdcp_buffer ();

  init_seed (set_seed);

  init_openair1 ();

  init_openair2 ();

  init_ocm ();

#ifdef XFORMS
  eNB_id = 0;
  for (UE_id = 0; UE_id < NB_UE_INST; UE_id++) {
    // DL scope at UEs
    fl_initialize (&argc, argv, NULL, 0, 0);
    form_ue[UE_id] = create_lte_phy_scope_ue();
    sprintf (title, "LTE DL SCOPE eNB %d to UE %d", eNB_id, UE_id);
    fl_show_form (form_ue[UE_id]->lte_phy_scope_ue, FL_PLACE_HOTSPOT, FL_FULLBORDER, title);

    // UL scope at eNB 0
    fl_initialize (&argc, argv, NULL, 0, 0);
    form_enb[UE_id] = create_lte_phy_scope_enb();
    sprintf (title, "LTE UL SCOPE UE %d to eNB %d", UE_id, eNB_id);
    fl_show_form (form_enb[UE_id]->lte_phy_scope_enb, FL_PLACE_HOTSPOT, FL_FULLBORDER, title);

    if (openair_daq_vars.use_ia_receiver == 1) {
      fl_set_button(form_ue[UE_id]->button_0,1);
      fl_set_object_label(form_ue[UE_id]->button_0, "IA Receiver ON");
      fl_set_object_color(form_ue[UE_id]->button_0, FL_GREEN, FL_GREEN);
    }

  }
#endif

#ifdef SMBV
  smbv_init_config(smbv_fname, smbv_nframes);
  smbv_write_config_from_frame_parms(smbv_fname, &PHY_vars_eNB_g[0]->lte_frame_parms);
#endif

  init_time ();

  init_slot_isr ();

  t = clock ();
  
  LOG_N(EMU, ">>>>>>>>>>>>>>>>>>>>>>>>>>> OAIEMU initialization done <<<<<<<<<<<<<<<<<<<<<<<<<<\n\n");

#if defined(ENABLE_ITTI)
  // Handle signals until all tasks are terminated
  if (itti_create_task_successful())
    itti_wait_tasks_end();
  else 
    exit(-1);
#else
  eNB_app_task(NULL); // do nothing for the moment
  l2l1_task (NULL);
#endif

  t = clock () - t;
  LOG_I (EMU,"Duration of the simulation: %f seconds\n", ((float) t) / CLOCKS_PER_SEC);

  //  fclose(SINRpost);
  LOG_N(EMU, ">>>>>>>>>>>>>>>>>>>>>>>>>>> OAIEMU Ending <<<<<<<<<<<<<<<<<<<<<<<<<<\n\n");

  free (otg_pdcp_buffer);

#ifdef SMBV
  if (config_smbv) {
    smbv_send_config (smbv_fname,smbv_ip);
  }
#endif

  //Perform KPI measurements
  if (oai_emulation.info.otg_enabled == 1)
    kpi_gen ();

  // relase all rx state
  if (ethernet_flag == 1) {
    emu_transport_release ();
  }

#ifdef PROC
  if (abstraction_flag == 0 && Channel_Flag==0 && Process_Flag==0)
#else
  if (abstraction_flag == 0)
#endif
      {
    /*
     #ifdef IFFT_FPGA
     free(txdataF2[0]);
     free(txdataF2[1]);
     free(txdataF2);
     free(txdata[0]);
     free(txdata[1]);
     free(txdata);
     #endif
     */

    for (i = 0; i < 2; i++) {
      free (s_re[i]);
      free (s_im[i]);
      free (r_re[i]);
      free (r_im[i]);
    }
    free (s_re);
    free (s_im);
    free (r_re);
    free (r_im);

    lte_sync_time_free ();
  }
  //  pthread_join(sigth, NULL);

  // added for PHY abstraction
  if (oai_emulation.info.ocm_enabled == 1) {
    for (eNB_id = 0; eNB_id < NUMBER_OF_eNB_MAX; eNB_id++)
      free (enb_data[eNB_id]);

    for (UE_id = 0; UE_id < NUMBER_OF_UE_MAX; UE_id++)
      free (ue_data[UE_id]);
  } //End of PHY abstraction changes

#ifdef OPENAIR2
  mac_top_cleanup ();
#endif 

#ifdef PRINT_STATS
  for (UE_id=0;UE_id<NB_UE_INST;UE_id++) {
    if (UE_stats[UE_id])
    fclose (UE_stats[UE_id]);
    if(UE_stats_th[UE_id])
    fclose (UE_stats_th[UE_id]);
  }
  for (eNB_id=0;eNB_id<NB_eNB_INST;eNB_id++) {
    if (eNB_stats[eNB_id])
    fclose (eNB_stats[eNB_id]);
  }
  if (eNB_avg_thr)
  fclose (eNB_avg_thr);
  if (eNB_l2_stats)
  fclose (eNB_l2_stats);

#endif

  // stop OMG
  stop_mobility_generator (oai_emulation.info.omg_model_ue); //omg_param_list.mobility_type
#ifdef OPENAIR2
  if (oai_emulation.info.omv_enabled == 1)
    omv_end (pfd[1], omv_data);
#endif
  if ((oai_emulation.info.ocm_enabled == 1) && (ethernet_flag == 0) && (ShaF != NULL))
    destroyMat (ShaF, map1, map2);

  if ((oai_emulation.info.opt_enabled == 1))
    terminate_opt ();

  if (oai_emulation.info.cli_enabled)
    cli_server_cleanup ();

  //bring oai if down
  terminate ();
  log_thread_finalize ();
  logClean ();
  vcd_signal_dumper_close ();

  return (0);
}

/*
 static void *sigh(void *arg) {

 int signum;
 sigset_t sigcatch;
 sigemptyset(&sigcatch);
 sigaddset(&sigcatch, SIGHUP);
 sigaddset(&sigcatch, SIGINT);
 sigaddset(&sigcatch, SIGTERM);

 for (;;) {
 sigwait(&sigcatch, &signum);
 switch (signum) {
 case SIGHUP:
 case SIGINT:
 case SIGTERM:
 terminate();
 default:
 break;
 }
 }
 pthread_exit(NULL);
 }
 */

void terminate(void) {
  int i;
  char interfaceName[8];
  for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++)
    if (oai_emulation.info.oai_ifup[i] == 1) {
      sprintf (interfaceName, "oai%d", i);
      bringInterfaceUp (interfaceName, 0);
    }
}


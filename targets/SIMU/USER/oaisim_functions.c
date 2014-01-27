#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <execinfo.h>
#include <time.h>

#include <sys/timerfd.h>

#include "oaisim_functions.h"

#include "PHY/extern.h"
#include "MAC_INTERFACE/extern.h"
#include "LAYER2/MAC/extern.h"
#include "LAYER2/PDCP_v10.1.0/pdcp.h"
#include "LAYER2/PDCP_v10.1.0/pdcp_primitives.h"
#include "RRC/LITE/extern.h"
#include "RRC/L2_INTERFACE/openair_rrc_L2_interface.h"
#include "PHY_INTERFACE/extern.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/extern.h"
#include "SCHED/extern.h"
#include "SIMULATION/ETH_TRANSPORT/proto.h"
#include "UTIL/OCG/OCG_extern.h"
#include "UTIL/LOG/vcd_signal_dumper.h"
#include "UTIL/OPT/opt.h"
#include "UTIL/OTG/otg_config.h"
#include "UTIL/OTG/otg_tx.h"
#include "lteRALenb.h"
#include "lteRALue.h"

#include "cor_SF_sim.h"

#if defined(ENABLE_ITTI)
# include "intertask_interface.h"
#endif

#if defined(ENABLE_USE_MME)
# include "s1ap_eNB.h"
#endif

#ifdef SMBV
extern u8 config_smbv;
extern char smbv_ip[16];
#endif

//constant for OAISIM soft realtime calibration
#define SF_DEVIATION_OFFSET_NS 100000 //= 0.1ms : should be as a number of UE
#define SLEEP_STEP_US       100 //  = 0.01ms could be adaptive, should be as a number of UE
#define K 2                  // averaging coefficient
#define TARGET_SF_TIME_NS 1000000       // 1ms = 1000000 ns

int otg_times = 0;
int if_times = 0;
int for_times = 0;

char *g_conf_config_file_name = NULL;
u16 Nid_cell = 0; //needed by init_lte_vars
int nb_antennas_rx=2; // //
u8 target_dl_mcs = 0;
u8 rate_adaptation_flag = 0;
u8 set_sinr=0;
double snr_dB, sinr_dB;
u8 set_seed=0;
u8 cooperation_flag;          // for cooperative communication
u8 abstraction_flag = 0, ethernet_flag = 0;
double snr_step=1.0;
u8 ue_connection_test=0;
double forgetting_factor=0.0;
u8 beta_ACK=0,beta_RI=0,beta_CQI=2;
u8 target_ul_mcs = 2;
LTE_DL_FRAME_PARMS *frame_parms;
int map1,map2;
double **ShaF= NULL;
// pointers signal buffers (s = transmit, r,r0 = receive)
double **s_re, **s_im, **r_re, **r_im, **r_re0, **r_im0;
Node_list ue_node_list = NULL;
Node_list enb_node_list = NULL;
int pdcp_period, omg_period;


// time calibration for soft realtime mode
struct timespec time_spec;
unsigned long time_last, time_now;
int td, td_avg, sleep_time_us;

#ifdef OPENAIR2
// omv related info
//pid_t omv_pid;
char full_name[200];
extern int pfd[2]; // fd for omv : fixme: this could be a local var
char fdstr[10];
char frames[10];
char num_enb[10];
char num_ue[10];
//area_x, area_y and area_z for omv
char x_area[20];
char y_area[20];
char z_area[20];
char nb_antenna[20];
char frame_type[10];
char tdd_config[10];
#endif

Packet_OTG_List *otg_pdcp_buffer;

extern node_desc_t *enb_data[NUMBER_OF_eNB_MAX];
extern node_desc_t *ue_data[NUMBER_OF_UE_MAX];
extern channel_desc_t *eNB2UE[NUMBER_OF_eNB_MAX][NUMBER_OF_UE_MAX];
extern channel_desc_t *UE2eNB[NUMBER_OF_UE_MAX][NUMBER_OF_eNB_MAX];

extern mapping small_scale_names[];
extern pdcp_mbms_t pdcp_mbms_array[MAX_MODULES][16*29];
//extern int eMBMS_active;

extern void help (void);

enum {
    LONG_OPTION_START = 0x100, /* Start after regular single char options */

    LONG_OPTION_ENB_CONF,

    LONG_OPTION_PDNC_PERIOD,
    LONG_OPTION_OMG_PERIOD,

    LONG_OPTION_ENB_RAL_LISTENING_PORT,
    LONG_OPTION_ENB_RAL_IP_ADDRESS,
    LONG_OPTION_ENB_RAL_LINK_ID,
    LONG_OPTION_ENB_RAL_LINK_ADDRESS,

    LONG_OPTION_ENB_MIHF_REMOTE_PORT,
    LONG_OPTION_ENB_MIHF_IP_ADDRESS,
    LONG_OPTION_ENB_MIHF_ID,

    LONG_OPTION_UE_RAL_LISTENING_PORT,
    LONG_OPTION_UE_RAL_IP_ADDRESS,
    LONG_OPTION_UE_RAL_LINK_ID,
    LONG_OPTION_UE_RAL_LINK_ADDRESS,

    LONG_OPTION_UE_MIHF_REMOTE_PORT,
    LONG_OPTION_UE_MIHF_IP_ADDRESS,
    LONG_OPTION_UE_MIHF_ID,
} long_option_e;

static struct option long_options[] = {
  {"enb-conf",               required_argument, 0, LONG_OPTION_ENB_CONF},

  {"pdcp_period",            required_argument, 0, LONG_OPTION_PDNC_PERIOD},
  {"omg_period",             required_argument, 0, LONG_OPTION_OMG_PERIOD},

  {"enb-ral-listening-port", required_argument, 0, LONG_OPTION_ENB_RAL_LISTENING_PORT},
  {"enb-ral-ip-address",     required_argument, 0, LONG_OPTION_ENB_RAL_IP_ADDRESS},
  {"enb-ral-link-id",        required_argument, 0, LONG_OPTION_ENB_RAL_LINK_ID},
  {"enb-ral-link-address",   required_argument, 0, LONG_OPTION_ENB_RAL_LINK_ADDRESS},

  {"enb-mihf-remote-port",   required_argument, 0, LONG_OPTION_ENB_MIHF_REMOTE_PORT},
  {"enb-mihf-ip-address",    required_argument, 0, LONG_OPTION_ENB_MIHF_IP_ADDRESS},
  {"enb-mihf-id",            required_argument, 0, LONG_OPTION_ENB_MIHF_ID},

  {"ue-ral-listening-port",  required_argument, 0, LONG_OPTION_UE_RAL_LISTENING_PORT},
  {"ue-ral-ip-address",      required_argument, 0, LONG_OPTION_UE_RAL_IP_ADDRESS},
  {"ue-ral-link-id",         required_argument, 0, LONG_OPTION_UE_RAL_LINK_ID},
  {"ue-ral-link-address",    required_argument, 0, LONG_OPTION_UE_RAL_LINK_ADDRESS},

  {"ue-mihf-remote-port",    required_argument, 0, LONG_OPTION_UE_MIHF_REMOTE_PORT},
  {"ue-mihf-ip-address",     required_argument, 0, LONG_OPTION_UE_MIHF_IP_ADDRESS},
  {"ue-mihf-id",             required_argument, 0, LONG_OPTION_UE_MIHF_ID},
  {NULL, 0, NULL, 0}
};

void get_simulation_options(int argc, char *argv[]) {
  int option;

  while ((option = getopt_long (argc, argv, "aA:b:B:c:C:D:d:eE:f:FGg:hHi:IJ:j:k:K:l:L:m:M:n:N:oO:p:P:Q:rR:s:S:t:T:u:U:vV:w:W:x:X:y:Y:z:Z:", long_options, NULL)) != -1) {
    switch (option) {
    case LONG_OPTION_ENB_CONF:
      if (optarg) {
          g_conf_config_file_name = strdup(optarg);
          printf("eNB configuration file is %s\n", g_conf_config_file_name);
      }
      break;

    case LONG_OPTION_PDNC_PERIOD:
      if (optarg) {
        pdcp_period = atoi(optarg);
        printf("PDCP period is %d\n", pdcp_period);
      }
      break;

    case LONG_OPTION_OMG_PERIOD:
      if (optarg) {
        omg_period = atoi(optarg);
        printf("OMG period is %d\n", omg_period);
      }
      break;

#if defined(ENABLE_RAL)
    case LONG_OPTION_ENB_RAL_LISTENING_PORT:
      if (optarg) {
        g_conf_enb_ral_listening_port = strdup(optarg);
        printf("eNB RAL listening port is %s\n", g_conf_enb_ral_listening_port);
      }
      break;

    case LONG_OPTION_ENB_RAL_IP_ADDRESS:
      if (optarg) {
        g_conf_enb_ral_ip_address = strdup(optarg);
        printf("eNB RAL IP address is %s\n", g_conf_enb_ral_ip_address);
      }
      break;

    case LONG_OPTION_ENB_RAL_LINK_ADDRESS:
      if (optarg) {
        g_conf_enb_ral_link_address = strdup(optarg);
        printf("eNB RAL link address is %s\n", g_conf_enb_ral_link_address);
      }
      break;

    case LONG_OPTION_ENB_RAL_LINK_ID:
      if (optarg) {
        g_conf_enb_ral_link_id = strdup(optarg);
        printf("eNB RAL link id is %s\n", g_conf_enb_ral_link_id);
      }
      break;

    case LONG_OPTION_ENB_MIHF_REMOTE_PORT:
      if (optarg) {
        g_conf_enb_mihf_remote_port = strdup(optarg);
        printf("eNB MIH-F remote port is %s\n", g_conf_enb_mihf_remote_port);
      }
      break;

    case LONG_OPTION_ENB_MIHF_IP_ADDRESS:
      if (optarg) {
        g_conf_enb_mihf_ip_address = strdup(optarg);
        printf("eNB MIH-F IP address is %s\n", g_conf_enb_mihf_ip_address);
      }
      break;

    case LONG_OPTION_ENB_MIHF_ID:
      if (optarg) {
        g_conf_enb_mihf_id = strdup(optarg);
        printf("eNB MIH-F id is %s\n", g_conf_enb_mihf_id);
      }
      break;

    case LONG_OPTION_UE_RAL_LISTENING_PORT:
      if (optarg) {
          g_conf_ue_ral_listening_port = strdup(optarg);
          printf("UE RAL listening port is %s\n", g_conf_ue_ral_listening_port);
      }
      break;

    case LONG_OPTION_UE_RAL_IP_ADDRESS:
      if (optarg) {
          g_conf_ue_ral_ip_address = strdup(optarg);
          printf("UE RAL IP address is %s\n", g_conf_ue_ral_ip_address);
      }
      break;

    case LONG_OPTION_UE_RAL_LINK_ID:
      if (optarg) {
          g_conf_ue_ral_link_id = strdup(optarg);
          printf("UE RAL link id is %s\n", g_conf_ue_ral_link_id);
      }
      break;

    case LONG_OPTION_UE_RAL_LINK_ADDRESS:
      if (optarg) {
          g_conf_ue_ral_link_address = strdup(optarg);
          printf("UE RAL link address is %s\n", g_conf_ue_ral_link_address);
      }
      break;

    case LONG_OPTION_UE_MIHF_REMOTE_PORT:
      if (optarg) {
          g_conf_ue_mihf_remote_port = strdup(optarg);
          printf("UE MIH-F remote port is %s\n", g_conf_ue_mihf_remote_port);
      }
      break;

    case LONG_OPTION_UE_MIHF_IP_ADDRESS:
      if (optarg) {
          g_conf_ue_mihf_ip_address = strdup(optarg);
          printf("UE MIH-F IP address is %s\n", g_conf_ue_mihf_ip_address);
      }
      break;

    case LONG_OPTION_UE_MIHF_ID:
      if (optarg) {
          g_conf_ue_mihf_id = strdup(optarg);
          printf("UE MIH-F id is %s\n", g_conf_ue_mihf_id);
      }
      break;
#endif

    case 'a':
      abstraction_flag = 1;
      break;

    case 'A':
      //oai_emulation.info.ocm_enabled=1;
      if (optarg == NULL)
        oai_emulation.environment_system_config.fading.small_scale.selected_option="AWGN";
      else
        oai_emulation.environment_system_config.fading.small_scale.selected_option= optarg;
      //awgn_flag = 1;
      break;

    case 'b':
      oai_emulation.info.nb_enb_local = atoi (optarg);
      break;

    case 'B':
      oai_emulation.topology_config.mobility.eNB_mobility.eNB_mobility_type.selected_option = optarg;
      //oai_emulation.info.omg_model_enb = atoi (optarg);
      break;

    case 'c':
      strcpy(oai_emulation.info.local_server, optarg);
      oai_emulation.info.ocg_enabled=1;
      break;

    case 'C':
      oai_emulation.info.tdd_config = atoi (optarg);
      if (oai_emulation.info.tdd_config > 6) {
        printf("Illegal tdd_config %d (should be 0-6)\n", oai_emulation.info.tdd_config);
        exit (-1);
      }
      break;

    case 'D':
      oai_emulation.info.multicast_ifname = strdup(optarg);
      break;

    case 'e':
      oai_emulation.info.extended_prefix_flag = 1;
      break;

    case 'E':
      set_seed = 1;
      oai_emulation.info.seed = atoi (optarg);
      break;

    case 'f':
      forgetting_factor = atof (optarg);
      break;

    case 'F':                   // set FDD
      printf("Setting Frame to FDD\n");
      oai_emulation.info.frame_type = 0;
      break;

    case 'g':
      oai_emulation.info.multicast_group = atoi (optarg);
      break;

    case 'G' :
      oai_emulation.info.otg_bg_traffic_enabled = 1;
      break;

    case 'h':
      help ();
      exit (1);
      break;

    case 'H':
      oai_emulation.info.handover_active=1;
      printf("Activate the handover procedure at RRC\n");
      break;

    case 'i':
#ifdef PROC
      Process_Flag=1;
      node_id = wgt+atoi(optarg);
      port+=atoi(optarg);
#endif
      break;

    case 'I':
      oai_emulation.info.cli_enabled = 1;
      break;

    case 'j' :
      // number of relay nodes: currently only applicable to eMBMS
      oai_emulation.info.nb_rn_local = atoi (optarg);
      break;

    case 'J':
      ue_connection_test=1;
      oai_emulation.info.ocm_enabled=0;
      snr_step = atof(optarg);
      break;

    case 'k':
      //ricean_factor = atof (optarg);
      printf("[SIM] Option k is no longer supported on the command line. Please specify your channel model in the xml template\n");
      exit(-1);
      break;

    case 'K':
      oai_emulation.info.itti_dump_file = optarg;
      break;

    case 'l':
      oai_emulation.info.g_log_level = atoi(optarg);
      break;

    case 'L':                   // set FDD
      flag_LA = atoi(optarg);
      break;

    case 'm':
      target_dl_mcs = atoi (optarg);
      break;

    case 'M':
      abstraction_flag = 1;
      ethernet_flag = 1;
      oai_emulation.info.ethernet_id = atoi (optarg);
      oai_emulation.info.master_id = oai_emulation.info.ethernet_id;
      oai_emulation.info.ethernet_flag = 1;
      break;

    case 'n':
      oai_emulation.info.n_frames = atoi (optarg);
      //n_frames = (n_frames >1024) ? 1024: n_frames; // adjust the n_frames if higher that 1024
      oai_emulation.info.n_frames_flag = 1;
      break;

    case 'N':
      Nid_cell = atoi (optarg);
      if (Nid_cell > 503) {
        printf("Illegal Nid_cell %d (should be 0 ... 503)\n", Nid_cell);
        exit(-1);
      }
      break;

    case 'O':
      g_conf_config_file_name = optarg;
      break;

    case 'o':
      oai_emulation.info.slot_isr = 1;
      break;

    case 'p':
      oai_emulation.info.nb_master = atoi (optarg);
      break;

    case 'P':
      oai_emulation.info.opt_enabled = 1;

      if (strcmp(optarg, "wireshark") == 0) {
          opt_type = OPT_WIRESHARK;
          printf("Enabling OPT for wireshark\n");
      } else if (strcmp(optarg, "pcap") == 0) {
          opt_type = OPT_PCAP;
          printf("Enabling OPT for pcap\n");
      } else {
          printf("Unrecognized option for OPT module. -> Disabling it\n");
          printf("Possible values are either wireshark or pcap\n");
          opt_type = OPT_NONE;
          oai_emulation.info.opt_enabled = 0;
      }
      oai_emulation.info.opt_mode = opt_type;
      break;

    case 'Q':
      //eMBMS_active=1;
      // 0 : not used (default), 1: eMBMS and RRC enabled, 2: eMBMS relaying and RRC enabled, 3: eMBMS enabled, RRC disabled, 4: eMBMS relaying enabled, RRC disabled
      oai_emulation.info.eMBMS_active_state = atoi (optarg);
      break;

    case 'r':
      rate_adaptation_flag = 1;
      break;

    case 'R':
      oai_emulation.info.N_RB_DL = atoi (optarg);
      if ((oai_emulation.info.N_RB_DL != 6) && (oai_emulation.info.N_RB_DL != 15) && (oai_emulation.info.N_RB_DL != 25)
        && (oai_emulation.info.N_RB_DL != 50) && (oai_emulation.info.N_RB_DL != 75) && (oai_emulation.info.N_RB_DL != 100)) {
        printf("Illegal N_RB_DL %d (should be one of 6,15,25,50,75,100)\n", oai_emulation.info.N_RB_DL);
        exit (-1);
      }
      break;

    case 's':
      snr_dB = atoi (optarg);
      //      set_snr = 1;
      oai_emulation.info.ocm_enabled=0;
      break;

    case 'S':
      sinr_dB = atoi (optarg);
      set_sinr = 1;
      oai_emulation.info.ocm_enabled=0;
      break;

    case 't':
      //Td = atof (optarg);
      printf("[SIM] Option t is no longer supported on the command line. Please specify your channel model in the xml template\n");
      exit(-1);
      break;

    case 'T':
      oai_emulation.info.otg_enabled = 1;
      oai_emulation.info.otg_traffic = optarg;
      break;

    case 'u':
      oai_emulation.info.nb_ue_local = atoi (optarg);
      break;

    case 'U':
      oai_emulation.topology_config.mobility.UE_mobility.UE_mobility_type.selected_option = optarg;
      break;

    case 'v':
      oai_emulation.info.omv_enabled = 1;
      break;

    case 'V':
      ouput_vcd = 1;
      oai_emulation.info.vcd_enabled = 1;
      oai_emulation.info.vcd_file = optarg;
      break;

    case 'w':
      oai_emulation.info.cba_group_active = atoi (optarg);
      break;

    case 'W':
#ifdef SMBV
      config_smbv = 1;
      if(atoi(optarg)!=0)
    strcpy(smbv_ip,optarg);
#endif
      break;

    case 'x':
      oai_emulation.info.transmission_mode = atoi (optarg);
      if ((oai_emulation.info.transmission_mode != 1) &&  (oai_emulation.info.transmission_mode != 2) && (oai_emulation.info.transmission_mode != 5) && (oai_emulation.info.transmission_mode != 6)) {
        printf("Unsupported transmission mode %d\n",oai_emulation.info.transmission_mode);
        exit(-1);
      }
      break;

    case 'X':
#ifdef PROC
      temp=atoi(optarg);
      if(temp==0){
        port=CHANNEL_PORT; Channel_Flag=1; Process_Flag=0; wgt=0;
      }
      else if(temp==1){
        port=eNB_PORT; wgt=0;
      }
      else {
        port=UE_PORT; wgt=MAX_eNB;
      }
#endif
      break;

    case 'y':
      nb_antennas_rx=atoi(optarg);
      if (nb_antennas_rx>4) {
        printf("Cannot have more than 4 antennas\n");
        exit(-1);
      }
      break;

    case 'Y':
      oai_emulation.info.g_log_verbosity_option = strdup(optarg);
      break;

    case 'z':
      cooperation_flag = atoi (optarg);
      break;

    case 'Z':
      /* Sebastien ROUX: Reserved for future use (currently used in ltenow branch) */
      break;

    default:
      help ();
      exit (-1);
      break;
    }
  }
}

void check_and_adjust_params() {

  s32 ret;
  int i,j;

  if (oai_emulation.info.nb_ue_local  + oai_emulation.info.nb_rn_local > NUMBER_OF_UE_MAX) {
    LOG_E(EMU,"Enter fewer than %d UEs/RNs for the moment or change the NUMBER_OF_UE_MAX\n", NUMBER_OF_UE_MAX);
    exit(EXIT_FAILURE);
  }

  if (oai_emulation.info.nb_enb_local + oai_emulation.info.nb_rn_local > NUMBER_OF_eNB_MAX) {
    LOG_E(EMU,"Enter fewer than %d eNBs/RNs for the moment or change the NUMBER_OF_UE_MAX\n", NUMBER_OF_eNB_MAX);
    exit(EXIT_FAILURE);
  }

  if (oai_emulation.info.nb_rn_local > NUMBER_OF_RN_MAX) {
    LOG_E(EMU,"Enter fewer than %d RNs for the moment or change the NUMBER_OF_RN_MAX\n", NUMBER_OF_RN_MAX);
    exit(EXIT_FAILURE);
  }
  // fix ethernet and abstraction with RRC_CELLULAR Flag
#ifdef RRC_CELLULAR
  abstraction_flag = 1;
  ethernet_flag = 1;
#endif

  if (set_sinr == 0)
    sinr_dB = snr_dB - 20;

  // setup netdevice interface (netlink socket)
  LOG_I(EMU,"[INIT] Starting NAS netlink interface\n");
  ret = netlink_init();
  if (ret < 0)
    LOG_W(EMU,"[INIT] Netlink not available, careful ...\n");

  if (ethernet_flag == 1) {
    oai_emulation.info.master[oai_emulation.info.master_id].nb_ue = oai_emulation.info.nb_ue_local + oai_emulation.info.nb_rn_local;
    oai_emulation.info.master[oai_emulation.info.master_id].nb_enb = oai_emulation.info.nb_enb_local + oai_emulation.info.nb_rn_local;
    if (oai_emulation.info.nb_rn_local>0)
      LOG_N(EMU,"Ethernet emulation is not yet tested with the relay nodes\n");
    if (!oai_emulation.info.master_id)
      oai_emulation.info.is_primary_master = 1;
    j = 1;
    for (i = 0; i < oai_emulation.info.nb_master; i++) {
      if (i != oai_emulation.info.master_id)
        oai_emulation.info.master_list = oai_emulation.info.master_list + j;
      LOG_I (EMU, "Index of master id i=%d  MASTER_LIST %d\n", i, oai_emulation.info.master_list);
      j *= 2;
    }
    LOG_I (EMU, " Total number of master %d my master id %d\n", oai_emulation.info.nb_master, oai_emulation.info.master_id);
    init_bypass ();

    while (emu_tx_status != SYNCED_TRANSPORT) {
      LOG_I (EMU, " Waiting for EMU Transport to be synced\n");
      emu_transport_sync ();    //emulation_tx_rx();
    }
  } // ethernet flag

  //
  NB_UE_INST = oai_emulation.info.nb_ue_local + oai_emulation.info.nb_ue_remote;
  NB_eNB_INST = oai_emulation.info.nb_enb_local + oai_emulation.info.nb_enb_remote;
  NB_RN_INST = oai_emulation.info.nb_rn_local + oai_emulation.info.nb_rn_remote;

#if defined(ENABLE_PDCP_NETLINK_FIFO) && defined(OPENAIR2)
  pdcp_netlink_init();
#endif

  if (NB_RN_INST > 0 ) {
    LOG_N(EMU,"Total number of RN %d (local %d, remote %d) mobility (the same as eNB) %s  \n", NB_RN_INST,oai_emulation.info.nb_rn_local,oai_emulation.info.nb_rn_remote, oai_emulation.topology_config.mobility.eNB_mobility.eNB_mobility_type.selected_option);

    LOG_N(EMU,"Adjust the number of eNB inst (%d->%d) and UE inst (%d->%d)\n ",
	  NB_eNB_INST, NB_eNB_INST+NB_RN_INST,
	  NB_UE_INST, NB_UE_INST+NB_RN_INST);
    NB_eNB_INST+=NB_RN_INST;
    NB_UE_INST+=NB_RN_INST;
  }
  LOG_I(EMU,"Total number of UE %d (first local %d , num local %d, remote %d, relay %d) mobility %s \n",
	NB_UE_INST,oai_emulation.info.first_ue_local, oai_emulation.info.nb_ue_local,oai_emulation.info.nb_ue_remote,
	NB_RN_INST,
	oai_emulation.topology_config.mobility.UE_mobility.UE_mobility_type.selected_option);

  LOG_I(EMU,"Total number of eNB %d (local %d, remote %d, relay %d) mobility %s \n",
	NB_eNB_INST,oai_emulation.info.nb_enb_local,oai_emulation.info.nb_enb_remote,
	NB_RN_INST,
	oai_emulation.topology_config.mobility.eNB_mobility.eNB_mobility_type.selected_option);

}

#ifdef OPENAIR2
void init_omv() {
  if (oai_emulation.info.omv_enabled == 1) {

    if(pipe(pfd) == -1)
      perror("pipe error \n");

    sprintf(full_name, "%s/UTIL/OMV/OMV",getenv("OPENAIR2_DIR"));
    LOG_I(EMU,"Stating the OMV path %s pfd[0] %d pfd[1] %d \n", full_name, pfd[0],pfd[1]);

    switch(fork()) {
    case -1 :
      perror("fork failed \n");
      break;
      case 0 : // child is going to be the omv, it is the reader
      if(close(pfd[1]) == -1 ) // we close the write desc.
      perror("close on write\n" );
      sprintf(fdstr, "%d", pfd[0] );
      sprintf(num_enb, "%d", NB_eNB_INST);
      sprintf(num_ue, "%d", NB_UE_INST);
      sprintf(x_area, "%f", oai_emulation.topology_config.area.x_m );
      sprintf(y_area, "%f", oai_emulation.topology_config.area.y_m );
      sprintf(z_area, "%f", 200.0 );
      sprintf(frames, "%d", oai_emulation.info.n_frames);
      sprintf(nb_antenna, "%d", 4);
      sprintf(frame_type, "%s", (oai_emulation.info.frame_type == 0) ? "FDD" : "TDD");
      sprintf(tdd_config, "%d", oai_emulation.info.tdd_config);
// execl is used to launch the visualisor
      execl(full_name,"OMV", fdstr, frames, num_enb, num_ue, x_area, y_area, z_area, nb_antenna, frame_type, tdd_config,NULL );
      perror( "error in execl the OMV" );
    }
    //parent
if(close( pfd[0] ) == -1 ) // we close the write desc.
      perror("close on read\n" );
  }
}
#endif

void init_seed(u8 set_seed) {

  if(set_seed) {

    randominit (oai_emulation.info.seed);
    set_taus_seed (oai_emulation.info.seed);

  } else {
    randominit (0);
    set_taus_seed (0);
  }
}

void init_openair1() {
  s32 UE_id, eNB_id;
#if defined(ENABLE_RAL)
  int list_index;
#endif
  // change the nb_connected_eNB
  init_lte_vars (&frame_parms, oai_emulation.info.frame_type, oai_emulation.info.tdd_config, oai_emulation.info.tdd_config_S,oai_emulation.info.extended_prefix_flag,oai_emulation.info.N_RB_DL, Nid_cell, cooperation_flag, oai_emulation.info.transmission_mode, abstraction_flag,nb_antennas_rx, oai_emulation.info.eMBMS_active_state);

  for (eNB_id=0; eNB_id<NB_eNB_INST;eNB_id++){
      for (UE_id=0; UE_id<NB_UE_INST;UE_id++){
          PHY_vars_eNB_g[eNB_id]->pusch_config_dedicated[UE_id].betaOffset_ACK_Index = beta_ACK;
          PHY_vars_eNB_g[eNB_id]->pusch_config_dedicated[UE_id].betaOffset_RI_Index  = beta_RI;
          PHY_vars_eNB_g[eNB_id]->pusch_config_dedicated[UE_id].betaOffset_CQI_Index = beta_CQI;
          PHY_vars_UE_g[UE_id]->pusch_config_dedicated[eNB_id].betaOffset_ACK_Index = beta_ACK;
          PHY_vars_UE_g[UE_id]->pusch_config_dedicated[eNB_id].betaOffset_RI_Index  = beta_RI;
          PHY_vars_UE_g[UE_id]->pusch_config_dedicated[eNB_id].betaOffset_CQI_Index = beta_CQI;
          ((PHY_vars_UE_g[UE_id]->lte_frame_parms).pdsch_config_common).p_b = (frame_parms->nb_antennas_tx_eNB>1) ? 1 : 0; // rho_a = rhob
          ((PHY_vars_eNB_g[eNB_id]->lte_frame_parms).pdsch_config_common).p_b = (frame_parms->nb_antennas_tx_eNB>1) ? 1 : 0; // rho_a = rhob

      }
  }

  printf ("AFTER init: Nid_cell %d\n", PHY_vars_eNB_g[0]->lte_frame_parms.Nid_cell);
  printf ("AFTER init: frame_type %d,tdd_config %d\n",
          PHY_vars_eNB_g[0]->lte_frame_parms.frame_type,
          PHY_vars_eNB_g[0]->lte_frame_parms.tdd_config);

  number_of_cards = 1;

  openair_daq_vars.rx_rf_mode = 1;
  openair_daq_vars.tdd = 1;
  openair_daq_vars.rx_gain_mode = DAQ_AGC_ON;

  openair_daq_vars.dlsch_transmission_mode = oai_emulation.info.transmission_mode;

  openair_daq_vars.target_ue_dl_mcs = target_dl_mcs;
  openair_daq_vars.target_ue_ul_mcs = target_ul_mcs;
  openair_daq_vars.ue_dl_rb_alloc=0x1fff;
  openair_daq_vars.ue_ul_nb_rb=6;
  openair_daq_vars.dlsch_rate_adaptation = rate_adaptation_flag;
  openair_daq_vars.use_ia_receiver = 0;

  // init_ue_status();
  for (UE_id=0; UE_id<NB_UE_INST;UE_id++) {
    PHY_vars_UE_g[UE_id]->rx_total_gain_dB=130;
    // update UE_mode for each eNB_id not just 0
    if (abstraction_flag == 0)
      PHY_vars_UE_g[UE_id]->UE_mode[0] = NOT_SYNCHED;
    else {
      // 0 is the index of the connected eNB
      PHY_vars_UE_g[UE_id]->UE_mode[0] = PRACH;
    }
    PHY_vars_UE_g[UE_id]->lte_ue_pdcch_vars[0]->crnti = 0x1235 + UE_id;
    PHY_vars_UE_g[UE_id]->current_dlsch_cqi[0] = 10;

    LOG_I(EMU, "UE %d mode is initialized to %d\n", UE_id, PHY_vars_UE_g[UE_id]->UE_mode[0] );
#if defined(ENABLE_RAL)
    PHY_vars_UE_g[UE_id]->ral_thresholds_timed = hashtable_create (64, NULL, NULL);
    for (list_index = 0; list_index < RAL_LINK_PARAM_GEN_MAX; list_index++) {
        SLIST_INIT(&PHY_vars_UE_g[UE_id]->ral_thresholds_gen_polled[list_index]);
    }
    for (list_index = 0; list_index < RAL_LINK_PARAM_LTE_MAX; list_index++) {
        SLIST_INIT(&PHY_vars_UE_g[UE_id]->ral_thresholds_lte_polled[list_index]);
    }
#endif

  }
}

void init_openair2() {
#ifdef OPENAIR2
  s32 i;
  s32 UE_id;

  l2_init (&PHY_vars_eNB_g[0]->lte_frame_parms,
	   oai_emulation.info.eMBMS_active_state,
	   oai_emulation.info.cba_group_active,
	   oai_emulation.info.handover_active);

   for (i = 0; i < NB_eNB_INST; i++)
    mac_xface->mrbch_phy_sync_failure (i, 0, i);

  if (abstraction_flag == 1) {
    for (UE_id = 0; UE_id < NB_UE_INST; UE_id++)
      mac_xface->dl_phy_sync_success (UE_id, 0, 0,1);   //UE_id%NB_eNB_INST);
  }

  mac_xface->macphy_exit = exit_fun;

#endif
}

void init_ocm() {
  s32 UE_id, eNB_id;
  /* Added for PHY abstraction */
  LOG_I(OCM,"Running with frame_type %d, Nid_cell %d, N_RB_DL %d, EP %d, mode %d, target dl_mcs %d, rate adaptation %d, nframes %d, abstraction %d, channel %s\n", oai_emulation.info.frame_type, Nid_cell, oai_emulation.info.N_RB_DL, oai_emulation.info.extended_prefix_flag, oai_emulation.info.transmission_mode,target_dl_mcs,rate_adaptation_flag,oai_emulation.info.n_frames,abstraction_flag,oai_emulation.environment_system_config.fading.small_scale.selected_option);

 if (abstraction_flag) {

    get_beta_map();
#ifdef PHY_ABSTRACTION_UL
    get_beta_map_up();
#endif
    get_MIESM_param();
  }

  for (eNB_id = 0; eNB_id < NB_eNB_INST; eNB_id++) {
    enb_data[eNB_id] = (node_desc_t *)malloc(sizeof(node_desc_t));
    init_enb(enb_data[eNB_id],oai_emulation.environment_system_config.antenna.eNB_antenna);
  }

  for (UE_id = 0; UE_id < NB_UE_INST; UE_id++) {
    ue_data[UE_id] = (node_desc_t *)malloc(sizeof(node_desc_t));
    init_ue(ue_data[UE_id],oai_emulation.environment_system_config.antenna.UE_antenna);
  }

  if ((oai_emulation.info.ocm_enabled == 1)&& (ethernet_flag == 0 ) &&
      (oai_emulation.environment_system_config.fading.shadowing.decorrelation_distance_m>0) &&
      (oai_emulation.environment_system_config.fading.shadowing.variance_dB>0)) {

    // init SF map here!!!
    map1 =(int)oai_emulation.topology_config.area.x_m;
    map2 =(int)oai_emulation.topology_config.area.y_m;
    ShaF = init_SF(map1,map2,oai_emulation.environment_system_config.fading.shadowing.decorrelation_distance_m,oai_emulation.environment_system_config.fading.shadowing.variance_dB);

    // size of area to generate shadow fading map
    LOG_D(EMU,"Simulation area x=%f, y=%f\n",
          oai_emulation.topology_config.area.x_m,
          oai_emulation.topology_config.area.y_m);
  }

  if (abstraction_flag == 0)
    init_channel_vars (frame_parms, &s_re, &s_im, &r_re, &r_im, &r_re0, &r_im0);

  // initialize channel descriptors
  for (eNB_id = 0; eNB_id < NB_eNB_INST; eNB_id++) {
    for (UE_id = 0; UE_id < NB_UE_INST; UE_id++) {

      LOG_D(OCM,"Initializing channel (%s, %d) from eNB %d to UE %d\n", oai_emulation.environment_system_config.fading.small_scale.selected_option,
            map_str_to_int(small_scale_names,oai_emulation.environment_system_config.fading.small_scale.selected_option), eNB_id, UE_id);

      /* if (oai_emulation.info.transmission_mode == 5)
                    eNB2UE[eNB_id][UE_id] = new_channel_desc_scm(PHY_vars_eNB_g[eNB_id]->lte_frame_parms.nb_antennas_tx,
                                                       PHY_vars_UE_g[UE_id]->lte_frame_parms.nb_antennas_rx,
                                                       (UE_id == 0)? Rice1_corr : Rice1_anticorr,
                                                       oai_emulation.environment_system_config.system_bandwidth_MB,
                                                       forgetting_factor,
                                                       0,
                                                       0);

                else
            */

      eNB2UE[eNB_id][UE_id] = new_channel_desc_scm(PHY_vars_eNB_g[eNB_id]->lte_frame_parms.nb_antennas_tx,
                                                   PHY_vars_UE_g[UE_id]->lte_frame_parms.nb_antennas_rx,
                                                   map_str_to_int(small_scale_names,oai_emulation.environment_system_config.fading.small_scale.selected_option),
                                                   oai_emulation.environment_system_config.system_bandwidth_MB,
                                                   forgetting_factor,
                                                   0,
                                                   0);
      random_channel(eNB2UE[eNB_id][UE_id]);
      LOG_D(OCM,"[SIM] Initializing channel (%s, %d) from UE %d to eNB %d\n", oai_emulation.environment_system_config.fading.small_scale.selected_option,
            map_str_to_int(small_scale_names, oai_emulation.environment_system_config.fading.small_scale.selected_option),UE_id, eNB_id);

      UE2eNB[UE_id][eNB_id] = new_channel_desc_scm(PHY_vars_UE_g[UE_id]->lte_frame_parms.nb_antennas_tx,
                                                   PHY_vars_eNB_g[eNB_id]->lte_frame_parms.nb_antennas_rx,
                                                   map_str_to_int(small_scale_names, oai_emulation.environment_system_config.fading.small_scale.selected_option),
                                                   oai_emulation.environment_system_config.system_bandwidth_MB,
                                                   forgetting_factor,
                                                   0,
                                                   0);

      random_channel(UE2eNB[UE_id][eNB_id]);
    }
  }

  // Not needed anymore, done automatically in init_freq_channel upon first call to the function

  // if (abstraction_flag==1)
  //    init_freq_channel(eNB2UE[0][0],PHY_vars_UE_g[0]->lte_frame_parms.N_RB_DL,PHY_vars_UE_g[0]->lte_frame_parms.N_RB_DL*12+1);
  freq_channel(eNB2UE[0][0],PHY_vars_UE_g[0]->lte_frame_parms.N_RB_DL,PHY_vars_UE_g[0]->lte_frame_parms.N_RB_DL*12+1);
}

void init_otg_pdcp_buffer() {
  s32 i;
  otg_pdcp_buffer = malloc((NB_UE_INST + NB_eNB_INST) * sizeof(Packet_OTG_List));

  for (i = 0; i < NB_UE_INST + NB_eNB_INST; i++) {
    pkt_list_init(&(otg_pdcp_buffer[i]));
    //LOG_I(EMU,"HEAD of otg_pdcp_buffer[%d] is %p\n", i, pkt_list_get_head(&(otg_pdcp_buffer[i])));
  }
}

void update_omg () {
  s32 UE_id, eNB_id;
  int new_omg_model;

  if ((frame % omg_period) == 0 ) { // call OMG every 10ms
    update_nodes(oai_emulation.info.time_s);
    display_node_list(enb_node_list);
    display_node_list(ue_node_list);
    if (oai_emulation.info.omg_model_ue >= MAX_NUM_MOB_TYPES){ // mix mobility model
      for(UE_id=oai_emulation.info.first_ue_local; UE_id<(oai_emulation.info.first_ue_local+oai_emulation.info.nb_ue_local);UE_id++){
        new_omg_model = randomGen(STATIC,RWALK);
        LOG_D(OMG, "[UE] Node of ID %d is changing mobility generator ->%d \n", UE_id, new_omg_model);
        // reset the mobility model for a specific node
        set_new_mob_type (UE_id, UE, new_omg_model, oai_emulation.info.time_s);
      }
    }

    if (oai_emulation.info.omg_model_enb >= MAX_NUM_MOB_TYPES) {      // mix mobility model
      for (eNB_id = oai_emulation.info.first_enb_local; eNB_id < (oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local); eNB_id++) {
        new_omg_model = randomGen (STATIC, RWALK);
        LOG_D (OMG,"[eNB] Node of ID %d is changing mobility generator ->%d \n", eNB_id, new_omg_model);
        // reset the mobility model for a specific node
        set_new_mob_type (eNB_id, eNB, new_omg_model, oai_emulation.info.time_s);
      }
    }
  }
}

void update_omg_ocm() {
  enb_node_list = get_current_positions(oai_emulation.info.omg_model_enb, eNB, oai_emulation.info.time_s);
  ue_node_list = get_current_positions(oai_emulation.info.omg_model_ue, UE, oai_emulation.info.time_s);
}

void update_ocm() {
  s32 UE_id, eNB_id;
  for (eNB_id = 0; eNB_id < NB_eNB_INST; eNB_id++)
    enb_data[eNB_id]->tx_power_dBm = PHY_vars_eNB_g[eNB_id]->lte_frame_parms.pdsch_config_common.referenceSignalPower;
  for (UE_id = 0; UE_id < NB_UE_INST; UE_id++)
    ue_data[UE_id]->tx_power_dBm = PHY_vars_UE_g[UE_id]->tx_power_dBm;

  /* check if the openair channel model is activated used for PHY abstraction : path loss*/
  if ((oai_emulation.info.ocm_enabled == 1)&& (ethernet_flag == 0 )) {
    //LOG_D(OMG," extracting position of eNb...\n");
    extract_position(enb_node_list, enb_data, NB_eNB_INST);
    //extract_position_fixed_enb(enb_data, NB_eNB_INST,frame);
    //LOG_D(OMG," extracting position of UE...\n");
    //      if (oai_emulation.info.omg_model_ue == TRACE)
    extract_position(ue_node_list, ue_data, NB_UE_INST);

    /* if (frame % 50 == 0)
      LOG_N(OCM,"Path loss for TTI %d : \n", frame);
    */
    for (eNB_id = 0; eNB_id < NB_eNB_INST; eNB_id++) {
      for (UE_id = 0; UE_id < NB_UE_INST; UE_id++) {
        calc_path_loss (enb_data[eNB_id], ue_data[UE_id], eNB2UE[eNB_id][UE_id], oai_emulation.environment_system_config,ShaF);
        //calc_path_loss (enb_data[eNB_id], ue_data[UE_id], eNB2UE[eNB_id][UE_id], oai_emulation.environment_system_config,0);
        UE2eNB[UE_id][eNB_id]->path_loss_dB = eNB2UE[eNB_id][UE_id]->path_loss_dB;
	//    if (frame % 50 == 0)
	LOG_I(OCM,"Path loss between eNB %d at (%f,%f) and UE %d at (%f,%f) is %f, angle %f\n",
              eNB_id,enb_data[eNB_id]->x,enb_data[eNB_id]->y,UE_id,ue_data[UE_id]->x,ue_data[UE_id]->y,
              eNB2UE[eNB_id][UE_id]->path_loss_dB, eNB2UE[eNB_id][UE_id]->aoa);
      }
    }
  }

  else {
    for (eNB_id = 0; eNB_id < NB_eNB_INST; eNB_id++) {
      for (UE_id = 0; UE_id < NB_UE_INST; UE_id++) {

        //UE2eNB[UE_id][eNB_id]->path_loss_dB = -105 + snr_dB;
        if (eNB_id == (UE_id % NB_eNB_INST)) {
          eNB2UE[eNB_id][UE_id]->path_loss_dB = -105 + snr_dB - PHY_vars_eNB_g[eNB_id]->lte_frame_parms.pdsch_config_common.referenceSignalPower;
          UE2eNB[UE_id][eNB_id]->path_loss_dB = -105 + snr_dB - PHY_vars_eNB_g[eNB_id]->lte_frame_parms.pdsch_config_common.referenceSignalPower; //+20 to offset the difference in tx power of the UE wrt eNB
        }
        else {
          eNB2UE[eNB_id][UE_id]->path_loss_dB = -105 + sinr_dB - PHY_vars_eNB_g[eNB_id]->lte_frame_parms.pdsch_config_common.referenceSignalPower;
          UE2eNB[UE_id][eNB_id]->path_loss_dB = -105 + sinr_dB - PHY_vars_eNB_g[eNB_id]->lte_frame_parms.pdsch_config_common.referenceSignalPower;
        }
	LOG_I(OCM,"Path loss from eNB %d to UE %d => %f dB (eNB TX %d)\n",eNB_id,UE_id,eNB2UE[eNB_id][UE_id]->path_loss_dB,
              PHY_vars_eNB_g[eNB_id]->lte_frame_parms.pdsch_config_common.referenceSignalPower);
        //      printf("[SIM] Path loss from UE %d to eNB %d => %f dB\n",UE_id,eNB_id,UE2eNB[UE_id][eNB_id]->path_loss_dB);
      }
    }
  }
}

#ifdef OPENAIR2
void update_otg_eNB(int module_id, unsigned int ctime) {
#if defined(USER_MODE) && defined(OAI_EMU)
  if (oai_emulation.info.otg_enabled ==1 ) {

    int dst_id, app_id;
    Packet_otg_elt *otg_pkt;


    for (dst_id = 0; dst_id < NUMBER_OF_UE_MAX; dst_id++) {
      for_times += 1;
      // generate traffic if the ue is rrc reconfigured state
      if (mac_get_rrc_status(module_id, 1/*eNB_flag*/, dst_id) > 2 /*RRC_CONNECTED*/ ) {

	for (app_id=0; app_id<MAX_NUM_APPLICATION; app_id++){
	  otg_pkt = malloc (sizeof(Packet_otg_elt));
	  if_times += 1;

	  (otg_pkt->otg_pkt).sdu_buffer = (u8*) packet_gen(module_id, dst_id + NB_eNB_INST, app_id, ctime, &((otg_pkt->otg_pkt).sdu_buffer_size));

	  if ((otg_pkt->otg_pkt).sdu_buffer != NULL) {
	    otg_times += 1;
	    (otg_pkt->otg_pkt).rb_id = dst_id * NB_RB_MAX + DTCH; // app could be binded to a given DRB
	    (otg_pkt->otg_pkt).module_id = module_id;
	    (otg_pkt->otg_pkt).dst_id = dst_id;
	    (otg_pkt->otg_pkt).is_ue = 0;
	    (otg_pkt->otg_pkt).mode = PDCP_DATA_PDU;
	    //Adding the packet to the OTG-PDCP buffer
	    pkt_list_add_tail_eurecom(otg_pkt, &(otg_pdcp_buffer[module_id]));
	    LOG_I(EMU, "[eNB %d] ADD pkt to OTG buffer with size %d for dst %d on rb_id %d for app id %d \n",
		  (otg_pkt->otg_pkt).module_id, otg_pkt->otg_pkt.sdu_buffer_size, (otg_pkt->otg_pkt).dst_id,(otg_pkt->otg_pkt).rb_id, app_id);
	  } else {
	    //LOG_I(EMU, "OTG returns null \n");
	    free(otg_pkt);
	    otg_pkt=NULL;
	  }
	}
	//LOG_T(EMU,"[eNB %d] UE mod id %d is not connected\n", module_id, dst_id);
	//LOG_I(EMU,"HEAD of otg_pdcp_buffer[%d] is %p\n", module_id, pkt_list_get_head(&(otg_pdcp_buffer[module_id])));

      }
    }

#ifdef Rel10
    int service_id, session_id, rb_id;
    // MBSM multicast traffic
    // if (frame >= 50) {// only generate when UE can receive MTCH (need to control this value)
      for (service_id = 0; service_id < 2 ; service_id++) { //maxServiceCount
	for (session_id = 0; session_id < 2; session_id++) { // maxSessionPerPMCH
	  //   LOG_I(OTG,"DUY:frame %d, pdcp_mbms_array[module_id][rb_id].instanciated_instance is %d\n",frame,pdcp_mbms_array[module_id][service_id*maxSessionPerPMCH + session_id].instanciated_instance);
	  if (pdcp_mbms_array[module_id][service_id*maxSessionPerPMCH + session_id].instanciated_instance == module_id + 1){ // this service/session is configured

	    otg_pkt = malloc (sizeof(Packet_otg_elt));
	    // LOG_T(OTG,"multicast packet gen for (service/mch %d, session/lcid %d, rb_id %d)\n", service_id, session_id, service_id*maxSessionPerPMCH + session_id);
	    rb_id = pdcp_mbms_array[module_id][service_id*maxSessionPerPMCH + session_id].rb_id;
	    (otg_pkt->otg_pkt).sdu_buffer = (u8*) packet_gen_multicast(module_id, session_id, ctime, &((otg_pkt->otg_pkt).sdu_buffer_size));
	    if ((otg_pkt->otg_pkt).sdu_buffer != NULL) {
	      (otg_pkt->otg_pkt).rb_id = rb_id;
	      (otg_pkt->otg_pkt).module_id = module_id;
	      (otg_pkt->otg_pkt).dst_id = session_id;
	      (otg_pkt->otg_pkt).is_ue = 0;
	      //Adding the packet to the OTG-PDCP buffer
	      (otg_pkt->otg_pkt).mode = PDCP_TM;
	      pkt_list_add_tail_eurecom(otg_pkt, &(otg_pdcp_buffer[module_id]));
	      LOG_I(EMU, "[eNB %d] ADD packet (%p) multicast to OTG buffer for dst %d on rb_id %d\n",
		    (otg_pkt->otg_pkt).module_id, otg_pkt, (otg_pkt->otg_pkt).dst_id,(otg_pkt->otg_pkt).rb_id);
	    } else {
	      //LOG_I(EMU, "OTG returns null \n");
	      free(otg_pkt);
	      otg_pkt=NULL;
	    }


	    // old version
	   /*	    // MBSM multicast traffic
#ifdef Rel10
	    if (frame >= 46) {// only generate when UE can receive MTCH (need to control this value)
	      for (service_id = 0; service_id < 2 ; service_id++) { //maxServiceCount
		for (session_id = 0; session_id < 2; session_id++) { // maxSessionPerPMCH
		  //   LOG_I(OTG,"DUY:frame %d, pdcp_mbms_array[module_id][rb_id].instanciated_instance is %d\n",frame,pdcp_mbms_array[module_id][service_id*maxSessionPerPMCH + session_id].instanciated_instance);
		  if ((pdcp_mbms_array[module_id][service_id*maxSessionPerPMCH + session_id].instanciated_instance== module_id + 1) && (eNB_flag == 1)){ // this service/session is configured
		    // LOG_T(OTG,"multicast packet gen for (service/mch %d, session/lcid %d)\n", service_id, session_id);
		    // Duy add
		    LOG_I(OTG, "frame %d, multicast packet gen for (service/mch %d, session/lcid %d, rb_id %d)\n",frame, service_id, session_id,service_id*maxSessionPerPMCH + session_id);
		    // end Duy add
		    rb_id = pdcp_mbms_array[module_id][service_id*maxSessionPerPMCH + session_id].rb_id;
		    otg_pkt=(u8*) packet_gen_multicast(module_id, session_id, ctime, &pkt_size);
		    if (otg_pkt != NULL) {
		      LOG_D(OTG,"[eNB %d] sending a multicast packet from module %d on rab id %d (src %d, dst %d) pkt size %d\n", eNB_index, module_id, rb_id, module_id, session_id, pkt_size);
		      pdcp_data_req(module_id, frame, eNB_flag, rb_id, RLC_MUI_UNDEFINED, RLC_SDU_CONFIRM_NO, pkt_size, otg_pkt,PDCP_TM);
		      free(otg_pkt);
		    }
		  }
		}
	      }
	    } // end multicast traffic
#endif
      	    */


	  }
	}
      }
      //    } // end multicast traffic
#endif

    //LOG_I(EMU, "[eNB %d] update OTG nb_elts = %d \n", module_id, otg_pdcp_buffer[module_id].nb_elements);

    //free(otg_pkt);
    //otg_pkt = NULL;
  }
#else
  if (otg_enabled==1) {
    ctime = frame * 100;
    for (dst_id = 0; dst_id < NUMBER_OF_UE_MAX; dst_id++) {
      if (mac_get_rrc_status(eNB_index, eNB_flag, dst_id ) > 2) {
        otg_pkt = malloc (sizeof(Packet_otg_elt));
        (otg_pkt->otg_pkt).sdu_buffer = packet_gen(module_id, dst_id, ctime, &pkt_size);
        if (otg_pkt != NULL) {
          rb_id = dst_id * NB_RB_MAX + DTCH;
          (otg_pkt->otg_pkt).rb_id = rb_id;
          (otg_pkt->otg_pkt).module_id = module_id;
	  (otg_pkt->otg_pkt).is_ue = 0;
	  (otg_pkt->otg_pkt).mode = PDCP_DATA_PDU;
          //Adding the packet to the OTG-PDCP buffer
          pkt_list_add_tail_eurecom(otg_pkt, &(otg_pdcp_buffer[module_id]));
          LOG_I(EMU, "[eNB %d] ADD pkt to OTG buffer for dst %d on rb_id %d\n", (otg_pkt->otg_pkt).module_id, (otg_pkt->otg_pkt).dst_id,(otg_pkt->otg_pkt).rb_id);
        } else {
          //LOG_I(EMU, "OTG returns null \n");
          free(otg_pkt);
          otg_pkt=NULL;
        }
        /*else {
    LOG_I(OTG,"nothing generated (src %d, dst %d)\n",src_id, dst_id);
    }*/
      }
      /*else {
  LOG_I(OTG,"rrc_status (src %d, dst %d) = %d\n",src_id, dst_id, mac_get_rrc_status(src_id, eNB_flag, dst_id ));
  }*/
    }
  }
#endif
}

void update_otg_UE(int UE_id, unsigned int ctime) {
#if defined(USER_MODE) && defined(OAI_EMU)
  if (oai_emulation.info.otg_enabled ==1 ) {
    int dst_id, src_id; //dst_id = eNB_index
    int module_id = UE_id+NB_eNB_INST;

    src_id = module_id;

    for (dst_id=0;dst_id<NUMBER_OF_eNB_MAX;dst_id++) {
      if (mac_get_rrc_status(UE_id, 0/*eNB_flag*/, dst_id ) > 2 /*RRC_CONNECTED*/) {
	Packet_otg_elt *otg_pkt = malloc (sizeof(Packet_otg_elt));
	// Manage to add this packet to the tail of your list
	(otg_pkt->otg_pkt).sdu_buffer = (u8*) packet_gen(src_id, dst_id, 0, ctime, &((otg_pkt->otg_pkt).sdu_buffer_size));

	if ((otg_pkt->otg_pkt).sdu_buffer != NULL) {
	  (otg_pkt->otg_pkt).rb_id = dst_id * NB_RB_MAX + DTCH;
	  (otg_pkt->otg_pkt).module_id = module_id;
	  (otg_pkt->otg_pkt).dst_id = dst_id;
	  (otg_pkt->otg_pkt).is_ue = 1;
	  //Adding the packet to the OTG-PDCP buffer
	  (otg_pkt->otg_pkt).mode = PDCP_DATA_PDU;
	  pkt_list_add_tail_eurecom(otg_pkt, &(otg_pdcp_buffer[module_id]));
	  LOG_I(EMU, "[UE %d] ADD pkt to OTG buffer with size %d for dst %d on rb_id %d \n",
		(otg_pkt->otg_pkt).module_id, otg_pkt->otg_pkt.sdu_buffer_size, (otg_pkt->otg_pkt).dst_id,(otg_pkt->otg_pkt).rb_id);
	} else {
	  free(otg_pkt);
	  otg_pkt=NULL;
	}
      }
    }
  }
#endif
}
#endif

int init_slot_isr(void)
{
    if (oai_emulation.info.slot_isr) {
        struct itimerspec its;

        int sfd;

        sfd = timerfd_create(CLOCK_REALTIME, 0);
        if (sfd == -1) {
            LOG_E(EMU, "Failed in timerfd_create (%d:%s)\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }

        /* Start the timer */
        its.it_value.tv_sec = 0;
        its.it_value.tv_nsec = 500 * 1000;
        its.it_interval.tv_sec = its.it_value.tv_sec;
        its.it_interval.tv_nsec = its.it_value.tv_nsec;

        if (timerfd_settime(sfd, TFD_TIMER_ABSTIME, &its, NULL) == -1) {
            LOG_E(EMU, "Failed in timer_settime (%d:%s)\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }

        oai_emulation.info.slot_sfd = sfd;
    }
    return 0;
}

void wait_for_slot_isr(void)
{
    uint64_t exp;
    ssize_t res;

    if (oai_emulation.info.slot_sfd > 0) {
        res = read(oai_emulation.info.slot_sfd, &exp, sizeof(exp));

        if ((res < 0) || (res != sizeof(exp))) {
            LOG_E(EMU, "Failed in read (%d:%s)\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
}

void exit_fun(const char* s)
{
  void *array[10];
  size_t size;

  size = backtrace(array, 10);
  backtrace_symbols_fd(array, size, 2);

  fprintf(stderr, "Error: %s. Exiting!\n",s);
  exit (-1);
}

void init_time() {
  clock_gettime (CLOCK_REALTIME, &time_spec);
  time_now = (unsigned long) time_spec.tv_nsec;
  td_avg = 0;
  sleep_time_us = SLEEP_STEP_US;
  td_avg = TARGET_SF_TIME_NS;
}


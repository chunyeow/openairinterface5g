/*! \file phy_emulation.h
* \brief specifies the data structure and variable for phy emulation
* \author Navid Nikaein, Raymomd Knopp  and Hicham Anouar
* \date 2011
* \version 1.0 
* \company Eurecom
* \email: navid.nikaein@eurecom.fr
*/ 


//#include "SCHED/defs.h"
#include "proto.h"
//#include "UTIL/OCG/OCG.h"


#ifndef __BYPASS_SESSION_LAYER_DEFS_H__
#    define __BYPASS_SESSION_LAYER_DEFS_H__
//-----------------------------------------------------------------------------
//#include "openair_defs.h"

#define WAIT_PM_TRANSPORT_INFO 0x1
#define WAIT_SM_TRANSPORT_INFO 0x2
#define SYNC_TRANSPORT_INFO 0x3
#define ENB_TRANSPORT_INFO 0X4
#define UE_TRANSPORT_INFO 0X5
#define RELEASE_TRANSPORT_INFO 0x6

#define WAIT_PM_TRANSPORT 1
#define WAIT_SM_TRANSPORT 2
#define SYNC_TRANSPORT 3
#define ENB_TRANSPORT 4
#define UE_TRANSPORT 5
#define RELEASE_TRANSPORT 6

#define WAIT_SYNC_TRANSPORT 1
#define SYNCED_TRANSPORT 2
//#define WAIT_ENB_TRANSPORT 2
//#define WAIT_UE_TRANSPORT 3


#define BYPASS_RX_BUFFER_SIZE 64000
#define BYPASS_TX_BUFFER_SIZE 64000


/*************************************************************/


typedef struct  {
  u32 pbch_flag:1;
  u32 pss:2;
  u32 sss:8;
  u32 cfi:2;
  u32 phich:19; // max 200 bit
  //  u32 pbch_payload:24;
  u32 pbch_payload;
} eNB_cntl;

typedef struct  {
  u8 pucch_flag:3;  // 0,7 = none, 1 = type 1, 2=type 1a, 3=type 1b, 4=type 2, 5=type 2a, 6=type 2b
  u8 pucch_Ncs1:3;  // physical configuration of pucch, for abstraction purposes
  u32 pucch_payload:21;        // ack/nak/cqi information
  u8 sr:1;
  u8 pusch_flag:1;  // 0=none,1=active
  u8 pucch_sel:1; 
  //u32 pusch_uci;     // uci information on pusch
  u8 pusch_uci[MAX_CQI_BYTES];
  u8 uci_format;
  u8 length_uci;
  u8 pusch_ri:2;    // ri information on pusch
  u8 pusch_ack:2;   // ack/nak on pusch
  u8 prach_flag:1;  // 0=none,1=active
  u8 prach_id:6;    // this is the PHY preamble index for the prach
} UE_cntl;
#define MAX_TRANSPORT_BLOCKS_BUFFER_SIZE 16384
#define MAX_NUM_DCI 5
typedef struct {
  eNB_cntl cntl;
  u8 num_common_dci;
  u8 num_ue_spec_dci;
  DCI_ALLOC_t dci_alloc[MAX_NUM_DCI];
  u8 dlsch_type[MAX_NUM_DCI];
  u8 harq_pid[MAX_NUM_DCI];
  u8 ue_id[MAX_NUM_DCI];
  u16 tbs[MAX_NUM_DCI*2];    // times 2 for dual-stream MIMO formats
  u8 transport_blocks[MAX_TRANSPORT_BLOCKS_BUFFER_SIZE]; 
} __attribute__ ((__packed__)) eNB_transport_info_t ;

/*typedef struct {
  eNB_cntl cntl;
  u8 num_common_dci;
  u8 num_ue_spec_dci;
  DCI_ALLOC_t dci_alloc;
  u8 dlsch_info[6*MAX_NUM_DCI + MAX_TRANSPORT_BLOCKS_BUFFER_SIZE];
} eNB_transport_info_rx_t ;
*/

typedef struct {
  UE_cntl cntl;
  u8 num_eNB;
  u16 rnti[NUMBER_OF_CONNECTED_eNB_MAX];
  u8 eNB_id[NUMBER_OF_CONNECTED_eNB_MAX]; 
  u8 harq_pid[NUMBER_OF_CONNECTED_eNB_MAX];
  u16 tbs[NUMBER_OF_CONNECTED_eNB_MAX];
  u8 transport_blocks[MAX_TRANSPORT_BLOCKS_BUFFER_SIZE];//*NUMBER_OF_CONNECTED_eNB_MAX];
} __attribute__ ((__packed__)) UE_transport_info_t ;

/*! \brief */
typedef struct bypass_msg_header {
  unsigned char  Message_type; /*! \brief control or data*/
  //unsigned char  nb_master; /*! \brief */
  unsigned char  master_id; /*! \brief */
  unsigned int   nb_enb; /*! \brief */
  unsigned int   nb_ue; /*! \brief */
  unsigned int   nb_flow; /*! \brief */
  unsigned int   frame;
  unsigned int subframe;
}__attribute__ ((__packed__)) bypass_msg_header_t;

typedef struct bypass_proto2multicast_header_t {
  unsigned int      size;
} bypass_proto2multicast_header_t;


/* // replaced to OCG.h
#define NUMBER_OF_MASTER_MAX   20
//#define NUMBER_OF_UE_MAX 32


typedef struct {
  unsigned char nb_ue;
  unsigned char first_ue;
  unsigned char nb_enb;
  unsigned char first_enb;
}master_info_t;

typedef struct {
  master_info_t master[NUMBER_OF_MASTER_MAX];
  unsigned char nb_ue_local;
  unsigned char nb_ue_remote;
  unsigned char nb_enb_local;
  unsigned char nb_enb_remote;
  unsigned char first_enb_local;
  unsigned char first_ue_local;
  unsigned short master_id;
  unsigned char nb_master;
  unsigned int master_list;
  unsigned int is_primary_master;
  unsigned int ethernet_flag;
  char local_server[128]; // for the oaisim -c option : 0 = EURECOM web portal; -1 = local; 1 - N or filename = running a specific XML configuration file 
  unsigned int offset_ue_inst;
  unsigned char multicast_group;
  unsigned char ocg_enabled;
  unsigned char opt_enabled;
  unsigned char otg_enabled;
  unsigned char omg_model_enb;
  unsigned char omg_model_ue;
  unsigned int seed;
  double time;	

}emu_info_t; 
*/


#endif //



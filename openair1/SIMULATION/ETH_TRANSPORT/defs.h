/*! \file phy_emulation.h
 *  \brief specifies the data structure and variable for phy emulation
 *  \author Navid Nikaein, Raymomd Knopp  and Hicham Anouar
 *  \date 2011
 *  \version 1.1
 *  \company Eurecom
 *  \email: navid.nikaein@eurecom.fr
 */

#ifndef __BYPASS_SESSION_LAYER_DEFS_H__
#    define __BYPASS_SESSION_LAYER_DEFS_H__
//-----------------------------------------------------------------------------
//#include "openair_defs.h"

typedef enum {
    EMU_TRANSPORT_INFO_ERROR    = 0x0,
    EMU_TRANSPORT_INFO_WAIT_PM,
    EMU_TRANSPORT_INFO_WAIT_SM,
    EMU_TRANSPORT_INFO_SYNC,
    EMU_TRANSPORT_INFO_ENB,
    EMU_TRANSPORT_INFO_UE,
    EMU_TRANSPORT_INFO_RELEASE
#if defined(ENABLE_PGM_TRANSPORT)
    ,EMU_TRANSPORT_NACK
#endif
} emu_transport_info_t;

#define WAIT_PM_TRANSPORT 1
#define WAIT_SM_TRANSPORT 2
#define SYNC_TRANSPORT 3
#define ENB_TRANSPORT 4
#define UE_TRANSPORT 5
#define RELEASE_TRANSPORT 6
#if defined(ENABLE_PGM_TRANSPORT)
# define NACK_TRANSPORT 7
#endif

#define WAIT_SYNC_TRANSPORT 1
#define SYNCED_TRANSPORT 2
//#define WAIT_ENB_TRANSPORT 2
//#define WAIT_UE_TRANSPORT 3


#define BYPASS_RX_BUFFER_SIZE 64000
#define BYPASS_TX_BUFFER_SIZE 64000

typedef unsigned int (*tx_handler_t) (unsigned char, char*, unsigned int*, unsigned int*);
typedef unsigned int (*rx_handler_t) (unsigned char, char*, unsigned int);

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
} __attribute__((__packed__)) eNB_transport_info_t ;

typedef struct {
  UE_cntl cntl;
  u8 num_eNB;
  u16 rnti[NUMBER_OF_CONNECTED_eNB_MAX];
  u8 eNB_id[NUMBER_OF_CONNECTED_eNB_MAX]; 
  u8 harq_pid[NUMBER_OF_CONNECTED_eNB_MAX];
  u16 tbs[NUMBER_OF_CONNECTED_eNB_MAX];
  u8 transport_blocks[MAX_TRANSPORT_BLOCKS_BUFFER_SIZE];//*NUMBER_OF_CONNECTED_eNB_MAX];
} __attribute__((__packed__)) UE_transport_info_t ;

/*! \brief */
typedef struct bypass_msg_header {
  unsigned char  Message_type; /*! \brief control or data*/
  //unsigned char  nb_master; /*! \brief */
  unsigned char  master_id; /*! \brief */
  unsigned int   nb_enb; /*! \brief */
  unsigned int   nb_ue; /*! \brief */
  unsigned int   nb_flow; /*! \brief */
  unsigned int   frame;
  unsigned int   subframe;
  uint64_t       seq_num;
  unsigned int   failing_master_id;
} __attribute__((__packed__)) bypass_msg_header_t;

typedef struct bypass_proto2multicast_header_t {
  unsigned int      size;
} bypass_proto2multicast_header_t;

#endif /* __BYPASS_SESSION_LAYER_DEFS_H__ */

/***************************************************************************
                          rrc_ue_L2_interface.c  -
                          -------------------
    begin                : Sept 9, 2008
    copyright            : (C) 2008, 2010 by Eurecom
    created by           : michelle.wetterwald@eurecom.fr
    description
 **************************************************************************
    Entry point for L2 interfaces
 ***************************************************************************/
/********************
//OpenAir definitions
 ********************/
#include "LAYER2/MAC/extern.h"
#include "COMMON/openair_defs.h"
#include "RRC/L2_INTERFACE/openair_rrc_L2_interface.h"
#include "LAYER2/RLC/rlc.h"
#include "COMMON/mac_rrc_primitives.h"
#include "UTIL/MEM/mem_block.h"

//#include "SIMULATION/simulation_defs.h"
//extern EMULATION_VARS *Emul_vars;
//extern CH_MAC_INST *CH_mac_inst;
//extern UE_MAC_INST *UE_mac_inst;
/********************
// RRC definitions
 ********************/
#include "rrc_ue_vars.h"
//-----------------------------------------------------------------------------
#include "rrc_L2_proto.h"
#include "rrc_proto_bch.h"
//-----------------------------------------------------------------------------
extern LCHAN_DESC BCCH_LCHAN_DESC,CCCH_LCHAN_DESC, DCCH_LCHAN_DESC, DTCH_DL_LCHAN_DESC,DTCH_UL_LCHAN_DESC;
extern rlc_info_t Rlc_info_um;
extern rlc_info_t Rlc_info_am_config;

//-----------------------------------------------------------------------------
s8 rrc_L2_data_req_rx (unsigned char Mod_id, unsigned short Srb_id, unsigned char Nb_tb,char *Buffer,u8 CH_index){
//-----------------------------------------------------------------------------
  unsigned char br_size=0;

#ifdef DEBUG_RRC_BROADCAST
  msg ("\n[RRC CELL][L2_INTF] rrc_L2_data_req_rx - begin - UE => Rcve only in BCCH\n");
  msg ("Received parameters Mod_id %d, Srb_id %d, nb_tb %d, CH_index %d\n",Mod_id, Srb_id,Nb_tb,CH_index);
#endif

  if( protocol_ms->rrc.ccch_buffer_size > 0) {
    #ifdef DEBUG_RRC_STATE
    msg ("\n[RRC CELL][L2_INTF] rrc_L2_data_req_rx - begin - Fill CCCH\n");
    msg ("Received parameters Mod_id %d, Srb_id %d, nb_tb %d, CH_index %d\n",Mod_id, Srb_id,Nb_tb,CH_index);
    msg ("CCCH buffer size %d\n",protocol_ms->rrc.ccch_buffer_size);
    #endif
    memcpy(&Buffer[0],&protocol_ms->rrc.ccch_buffer[0],protocol_ms->rrc.ccch_buffer_size);
    br_size = protocol_ms->rrc.ccch_buffer_size;
    protocol_ms->rrc.ccch_buffer_size = 0;
  }
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC CELL][L2_INTF] rrc_L2_data_req_rx - end, return %d\n", br_size);
#endif
  return br_size;
}

//-----------------------------------------------------------------------------
s8 rrc_L2_mac_data_ind_rx (u8 Mod_id, u16 Srb_id, char *Sdu, u16 Sdu_len, u8 eNB_index){
//-----------------------------------------------------------------------------

  int rxStatus=0;

  if((Srb_id & RAB_OFFSET) == BCCH){
    #ifdef DEBUG_RRC_BROADCAST_DETAILS
    msg ("\n[RRC CELL][L2_INTF] rrc_L2_mac_data_ind_rx - begin - BCCH\n");
    //msg ("Received parameters Mod_id %d, Srb_id %d, CH_index %d\n",Mod_id, Srb_id,CH_index);
    #endif
    rrc_broadcast_rx (Sdu);
    #ifdef DEBUG_RRC_BROADCAST_DETAILS
    msg ("\n[RRC CELL][L2_INTF] rrc_L2_mac_data_ind_rx - end\n");
    #endif
  }
  else 
    if( (Srb_id & RAB_OFFSET ) == CCCH){
    #ifdef DEBUG_RRC_STATE
    msg ("\n[RRC CELL][L2_INTF] rrc_L2_mac_data_ind_rx - begin - CCCH\n");
    msg ("Received parameters Mod_id %d, eNB_index %d, SDU length %d\n", Mod_id, eNB_index, Sdu_len);
    #endif
    // temp - establish srb1-srb2
    rrc_ue_config_LTE_srb1();

    }
    else msg ("\n[RRC CELL][L2_INTF] rrc_L2_mac_data_ind_rx, Srb_id %d unexpected \n" , Srb_id);
  return rxStatus;
}

//-----------------------------------------------------------------------------
void rrc_L2_rlc_data_ind_rx (unsigned char Mod_id, unsigned int Srb_id, unsigned int Sdu_size,unsigned char *Buffer){
//-----------------------------------------------------------------------------
#ifdef DEBUG_RRC_DETAILS_2
  msg ("\n[RRC][L2_INTF] rrc_L2_rlc_data_ind_rx - begin\n");
#endif
  //rrc_ue_test_rlc_intf_rcve (Buffer, Srb_id);
  rrc_ue_receive_from_srb_rlc (Buffer, Srb_id, Sdu_size);
}

//-----------------------------------------------------------------------------
void rrc_L2_rlc_confirm_ind_rx (unsigned char Mod_id, unsigned int Srb_id, unsigned int mui){
//-----------------------------------------------------------------------------
#ifdef DEBUG_RRC_DETAILS_2
  msg ("\n[RRC][L2_INTF] rrc_L2_rlc_confirm_ind_rx - begin\n");
#endif
 //void* rrc_srb_confirm (u32 muiP, u8 rb_idP, u8 statusP);
  rrc_ue_srb_confirm (mui, Srb_id, 0);
}

//-----------------------------------------------------------------------------
void rrc_L2_mac_meas_ind_rx (void){
//-----------------------------------------------------------------------------
#ifdef DEBUG_RRC_DETAILS
  msg ("\n[RRC][L2_INTF] rrc_L2_mac_meas_ind_rx - begin\n");
#endif
}

//-----------------------------------------------------------------------------
void rrc_L2_def_meas_ind_rx (unsigned char Mod_id, unsigned char Idx2){
//-----------------------------------------------------------------------------
#ifdef DEBUG_RRC_DETAILS
  msg ("\n[RRC][L2_INTF] rrc_L2_def_meas_ind_rx - begin\n");
#endif
}

//-----------------------------------------------------------------------------
void rrc_L2_sync_ind_rx (u8 Mod_id){
//-----------------------------------------------------------------------------
#ifdef DEBUG_RRC_DETAILS
  if (!protocol_ms->rrc.current_SFN %100 )
     msg ("\n[RRC][L2_INTF] rrc_L2_sync_ind_rx at frame %d \n", protocol_ms->rrc.current_SFN);
#endif
}

//-----------------------------------------------------------------------------
void rrc_L2_out_sync_ind_rx (void){
//-----------------------------------------------------------------------------
#ifdef DEBUG_RRC_DETAILS
  msg ("\n[RRC][L2_INTF] rrrc_L2_out_sync_ind_rx - begin\n");
#endif
}

//-----------------------------------------------------------------------------
int rrc_L2_get_rrc_status(u8 Mod_id,u8 eNB_flag,u8 index){
//-----------------------------------------------------------------------------
#ifdef DEBUG_RRC_DETAILS
    msg ("\n[RRC][L2_INTF] rrc_L2_get_rrc_status - begin\n");
#endif
/*
  if(eNB_flag == 1)
    return(eNB_rrc_inst[Mod_id].Info.Status[index]);
  else
    return(UE_rrc_inst[Mod_id].Info[index].Status);
*/
   return 0;
}

//-----------------------------------------------------------------------------
char rrc_L2_ue_init(u8 Mod_id, unsigned char eNB_index){
//-----------------------------------------------------------------------------
#ifdef DEBUG_RRC_DETAILS
    msg ("\n[RRC][L2_INTF] rrc_L2_ue_init - begin\n");
#endif
    rrc_ue_init (Mod_id);
    return 0;
}


//-----------------------------------------------------------------------------
char rrc_L2_eNB_init(u8 Mod_id){
//-----------------------------------------------------------------------------
#ifdef DEBUG_RRC_DETAILS
    msg ("\n[RRC][L2_INTF] rrc_L2_eNB_init - begin\n");
#endif
    rrc_ue_rglite_init(Mod_id, 0);
    return 0;
}

//-----------------------------------------------------------------------------
// Out of openair_rrc_L2_interface.c
void openair_rrc_lite_top_init(void){
//-----------------------------------------------------------------------------
  //#ifdef DEBUG_RRC_STATE
   msg ("\n[RRC CELL] [L2_INTF] openair_rrc_lite_top_init - Empty function to keep compatibility with RRC LITE\n\n");
  //#endif
}

//-----------------------------------------------------------------------------
RRC_status_t rrc_rx_tx(u8 Mod_id,u32 frame, u8 eNB_flag,u8 index){
//-----------------------------------------------------------------------------
#ifdef DEBUG_RRC_DETAILS
   // msg ("\n[RRC][L2_INTF] rrc_rx_tx - begin %d \n", frame);
#endif
    // call RRC only if new frame number
    if (frame > protocol_ms->rrc.current_SFN){
        protocol_ms->rrc.current_SFN = frame;
        rrc_ue_main_scheduler(Mod_id, protocol_ms->rrc.current_SFN, 0, index);
    }
    return 0;
}

/*------------------------------------------------------------------------------*/
// Dummy function - to keep compatibility with RRC LITE
char rrc_ue_rglite_init(u8 Mod_id, unsigned char eNB_index){
/*------------------------------------------------------------------------------*/
  //#ifdef DEBUG_RRC_STATE
   msg ("\n[RRC CELL] Called rrc_ue_rglite_init - Dummy function - to keep compatibility with RRC LITE\n\n");
  //#endif
  return 0;
}

/*------------------------------------------------------------------------------*/
// Send config to the MAC Layer
void rrc_init_mac_config(void){
/*------------------------------------------------------------------------------*/
  MAC_CONFIG_REQ Mac_config_req;
  int UE_index,Idx;

  // The content of this function has been commented on 23/03/2012
  printk("\n rrc_init_mac_config -- WORK IN PROGRESS\n");

  // Test config on 02/04/2012
  BCCH_LCHAN_DESC.transport_block_size=BCCH_PAYLOAD_SIZE_MAX;
  BCCH_LCHAN_DESC.max_transport_blocks=15;//MAX 16 (4bits for Seq_id)

  DCCH_LCHAN_DESC.transport_block_size=4;
  DCCH_LCHAN_DESC.max_transport_blocks=16;
  DCCH_LCHAN_DESC.Delay_class=1;
  DTCH_DL_LCHAN_DESC.transport_block_size=52;
  DTCH_DL_LCHAN_DESC.max_transport_blocks=20;
  DTCH_DL_LCHAN_DESC.Delay_class=1;
  DTCH_UL_LCHAN_DESC.transport_block_size=52;
  DTCH_UL_LCHAN_DESC.max_transport_blocks=20;
  DTCH_UL_LCHAN_DESC.Delay_class=1;

  // Config copied from RRC LITE on 02/04/2012
  Rlc_info_um.rlc_mode=RLC_UM;
  Rlc_info_um.rlc.rlc_um_info.timer_reordering=0;
  Rlc_info_um.rlc.rlc_um_info.sn_field_length=10;
  Rlc_info_um.rlc.rlc_um_info.is_mXch=0;
  //Rlc_info_um.rlc.rlc_um_info.sdu_discard_mode=16;

  Rlc_info_am_config.rlc_mode=RLC_AM;
  Rlc_info_am_config.rlc.rlc_am_info.max_retx_threshold = 255;
  Rlc_info_am_config.rlc.rlc_am_info.poll_pdu           = 8;
  Rlc_info_am_config.rlc.rlc_am_info.poll_byte          = 1000;
  Rlc_info_am_config.rlc.rlc_am_info.t_poll_retransmit  = 15;
  Rlc_info_am_config.rlc.rlc_am_info.t_reordering       = 5000;
  Rlc_info_am_config.rlc.rlc_am_info.t_status_prohibit  = 10;

}

/*------------------------------------------------------------------------------*/
// Send config to the MAC Layer
void rrc_init_mac_default_param(void){
/*------------------------------------------------------------------------------*/

  // The following code has been commented on 23/03/2012
 printk("\n rrc_init_mac_default_param -- COMMENTED\n");

  /*
  BCCH_LCHAN_DESC.transport_block_size=BCCH_PAYLOAD_SIZE_MAX;
  BCCH_LCHAN_DESC.max_transport_blocks=15;//MAX 16 (4bits for Seq_id)

  CCCH_LCHAN_DESC.transport_block_size=CCCH_PAYLOAD_SIZE_MAX;
  CCCH_LCHAN_DESC.max_transport_blocks=15;//MAX 16 (4bits for Seq_id)

//   BCCH_LCHAN_DESC.transport_block_size=30;//+CH_BCCH_HEADER_SIZE;
//   BCCH_LCHAN_DESC.max_transport_blocks=15;//MAX 16 (4bits for Seq_id)
//   CCCH_LCHAN_DESC.transport_block_size=30;//+CH_CCCH_HEADER_SIZE;
//   CCCH_LCHAN_DESC.max_transport_blocks=15;//MAX 16 (4bits for Seq_id)
   DCCH_LCHAN_DESC.transport_block_size=40;//+CH_BCCH_HEADER_SIZE;
   DCCH_LCHAN_DESC.max_transport_blocks=20;
   DTCH_LCHAN_DESC.transport_block_size=40;//120;//200;//+CH_BCCH_HEADER_SIZE;
   DTCH_LCHAN_DESC.max_transport_blocks=20;
//
   Rlc_info_um.rlc_mode=RLC_UM;
   Rlc_info_um.rlc.rlc_um_info.timer_discard=0;
   Rlc_info_um.rlc.rlc_um_info.sdu_discard_mode=16;
//
  Rlc_info_am.rlc_mode=RLC_AM;
  Rlc_info_am.rlc.rlc_am_info.sdu_discard_mode      = SDU_DISCARD_MODE_RESET;
  Rlc_info_am.rlc.rlc_am_info.timer_poll            = 0;
  Rlc_info_am.rlc.rlc_am_info.timer_poll_prohibit   = 0;
  Rlc_info_am.rlc.rlc_am_info.timer_discard         = 1000*10;
  Rlc_info_am.rlc.rlc_am_info.timer_poll_periodic   = 0;
  Rlc_info_am.rlc.rlc_am_info.timer_status_prohibit = 0;
  Rlc_info_am.rlc.rlc_am_info.timer_status_periodic = 250*10;
  Rlc_info_am.rlc.rlc_am_info.timer_rst             = 100*10;
  Rlc_info_am.rlc.rlc_am_info.max_rst               = 8;
  Rlc_info_am.rlc.rlc_am_info.timer_mrw             = 60*10;

  Rlc_info_am.rlc.rlc_am_info.pdu_size              = 320; // in bits
  //Rlc_info_am.rlc.rlc_am_info.in_sequence_delivery  = 1;//boolean
  Rlc_info_am.rlc.rlc_am_info.max_dat               = 63;
  Rlc_info_am.rlc.rlc_am_info.poll_pdu              = 16;
  Rlc_info_am.rlc.rlc_am_info.poll_sdu              = 1;
  Rlc_info_am.rlc.rlc_am_info.poll_window           = 50;
  Rlc_info_am.rlc.rlc_am_info.tx_window_size        = 128;
  Rlc_info_am.rlc.rlc_am_info.rx_window_size        = 128;
  Rlc_info_am.rlc.rlc_am_info.max_mrw               = 8;

  Rlc_info_am.rlc.rlc_am_info.last_transmission_pdu_poll_trigger   = 1;//boolean
  Rlc_info_am.rlc.rlc_am_info.last_retransmission_pdu_poll_trigger = 1;//boolean
  Rlc_info_am.rlc.rlc_am_info.send_mrw              = 0;//boolean*
  */

  // Config copied from RRC LITE on 02/04/2012
  Rlc_info_um.rlc_mode=RLC_UM;
  Rlc_info_um.rlc.rlc_um_info.timer_reordering=0;
  Rlc_info_um.rlc.rlc_um_info.sn_field_length=10;
  Rlc_info_um.rlc.rlc_um_info.is_mXch=0;
  //Rlc_info_um.rlc.rlc_um_info.sdu_discard_mode=16;

  Rlc_info_am_config.rlc_mode=RLC_AM;
  Rlc_info_am_config.rlc.rlc_am_info.max_retx_threshold = 255;
  Rlc_info_am_config.rlc.rlc_am_info.poll_pdu           = 8;
  Rlc_info_am_config.rlc.rlc_am_info.poll_byte          = 1000;
  Rlc_info_am_config.rlc.rlc_am_info.t_poll_retransmit  = 15;
  Rlc_info_am_config.rlc.rlc_am_info.t_reordering       = 5000;
  Rlc_info_am_config.rlc.rlc_am_info.t_status_prohibit  = 10;
}

/*------------------------------------------------------------------------------*/
//Entry function for RRC - MAC interface  init - Copied from RRC MESH (MW 09/09/2008)
int rrc_init_global_param(void){
  /*------------------------------------------------------------------------------*/
  //  Nb_mod=0;
  //#ifdef DEBUG_RRC_STATE
   msg ("\n[RRC CELL] Called rrc_init_global_param - Begin \n\n");
  //#endif

 /*
#ifdef USER_MODE
  Rrc_xface = (RRC_XFACE*)malloc16(sizeof(RRC_XFACE));
#endif
  Rrc_xface->openair_rrc_top_init = rrc_ue_toplite_init;
  Rrc_xface->openair_rrc_eNB_init = rrc_ue_rglite_init;
  Rrc_xface->openair_rrc_UE_init = rrc_ue_init;
  Rrc_xface->mac_rrc_data_ind = mac_rrc_data_ind;
  Rrc_xface->mac_rrc_data_req = mac_rrc_data_req;
  Rrc_xface->rrc_data_indP    = rlcrrc_data_ind;
  Rrc_xface->rrc_rx_tx        = rrc_ue_main_scheduler;
  Rrc_xface->mac_rrc_meas_ind = mac_rrc_meas_ind;
  Rrc_xface->def_meas_ind     = rrc_L2_def_meas_ind_rx;
  Mac_rlc_xface->mac_out_of_sync_ind = mac_out_of_sync_ind;
  printk("[RRC]INIT_GLOBAL_PARAM: Mac_rlc_xface %p, rrc_rlc_register %p,rlcrrc_data_ind %p\n",Mac_rlc_xface,Mac_rlc_xface->rrc_rlc_register_rrc,rlcrrc_data_ind);
  if (Mac_rlc_xface==NULL || Mac_rlc_xface->rrc_rlc_register_rrc==NULL||rlcrrc_data_ind==NULL)
    return -1;
  //register with rlc -1st function= data_ind/srb_rx, 2nd function = srb_confirm
  Mac_rlc_xface->rrc_rlc_register_rrc(rlcrrc_data_ind ,rrc_L2_rlc_confirm_ind_rx);

  rrc_init_mac_default_param();
  */

  //register with rlc -1st function= data_ind/srb_rx, 2nd function = srb_confirm
  printk("[RRC CELL]INIT_GLOBAL_PARAM: rrc_rlc_register_rrc %p,rlcrrc_data_ind %p, rrc_L2_rlc_confirm_ind_rx %p\n", rrc_rlc_register_rrc, rlcrrc_data_ind, rrc_L2_rlc_confirm_ind_rx );
  if( rrc_rlc_register_rrc==NULL||rlcrrc_data_ind==NULL|| rrc_L2_rlc_confirm_ind_rx==NULL)
    return -1;
  rrc_rlc_register_rrc(rlcrrc_data_ind , rrc_L2_rlc_confirm_ind_rx);

  return 0;
}





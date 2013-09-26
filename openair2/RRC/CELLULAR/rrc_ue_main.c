/***************************************************************************
                          rrc_ue_main.c  -
                          -------------------
    begin                : Tue Jan 15 2002
    copyright            : (C) 2002, 2010 by Eurecom
    created by           : michelle.wetterwald@eurecom.fr
 **************************************************************************
		This file contains the main function of the RRC module
 ***************************************************************************/
//#include "rtos_header.h"
//#include "platform.h"
//#include "protocol_vars_extern.h"
//#include "print.h"
//-----------------------------------------------------------------------------
/********************
//OpenAir definitions
 ********************/
#include "LAYER2/MAC/extern.h"
#include "UTIL/MEM/mem_block.h"
//-----------------------------------------------------------------------------
/********************
// RRC definitions
 ********************/
#include "rrc_ue_vars.h"
//#include "rrc_nas_sap.h"
//#include "rrc_messages.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_int.h"
//#include "rrc_proto_fsm.h"
//#include "rrc_proto_intf.h"
//#include "rrc_proto_bch.h"
#include "rrc_proto_mbms.h"

#include "umts_timer_proto_extern.h"

//-----------------------------------------------------------------------------
// entry point for rrc-ue process
//void rrc_ue_main_scheduler (u8 Mod_id){
int rrc_ue_main_scheduler(u8 Mod_id,u32 frame, u8 eNB_flag,u8 index){
//-----------------------------------------------------------------------------
  mem_block_t *p;
  int Message_Id;

//     while(1)
//     {sleep(2);}

//  protocol_ms->rrc.current_SFN = Mac_rlc_xface->frame;
 // protocol_ms->rrc.current_SFN = frame;

  /*************/
  // TODO TO BE REMOVED TEMP -- stop RRC process action (for early traces) 
  //if (protocol_ms->rrc.current_SFN > 300)
  // return 0;

  #ifdef DEBUG_RRC_DETAILS
  if (protocol_ms->rrc.current_SFN % 50 == 0) {
     msg ("\n\n[RRC][MSG_TEST] System Time : %d\n", protocol_ms->rrc.current_SFN);
     #ifdef  DEBUG_RRC_BROADCAST
     msg ("[RRC_BCH-UE] DEBUG ue_wait_establish_req = %d.\n", protocol_ms->rrc.ue_wait_establish_req );
     #endif
  }
  #endif

  #ifdef ALLOW_MBMS_PROTOCOL
    //     ATTENTION: This must be the first event of RRC process.
    //     Used to find the beginning of the modification period.
    rrc_ue_mbms_scheduling_check();
  #endif

  if (rrc_release_all_ressources) {
  #ifdef DEBUG_RRC_STATE
    msg ("[RRC_UE]rrc_ue_main_scheduler : release_radio_resources() \n");
  #endif
    //mac_remove_all ();
    //rb_remove_all ();
    // Set RRM Functions to remove RBs, TrChs, CCTrChs
    // Put UE in Cell-BCH mode
    /* BYPASS_L1 */

    rrc_release_all_ressources = 0;
  }
  // check L1
  rrc_ue_L1_check ();


  // check if there is some message to transmit to NAS and do it
  if ((p = protocol_ms->rrc.NASMessageToXmit) != NULL) {
    rrc_ue_write_FIFO (p);
  } else {
    if (protocol_ms->rrc.ue_broadcast_counter % 500 == 2) {
      RRC_UE_O_NAS_MEASUREMENT_IND ();
      rrc_ue_write_FIFO (protocol_ms->rrc.NASMessageToXmit);
    }
    // modulo to improve stability
    protocol_ms->rrc.ue_broadcast_counter = (protocol_ms->rrc.ue_broadcast_counter++) % 1000000000;
  }

  // time out for SIB14 - cf RG
  if (protocol_ms->rrc.ue_broadcast_counter % (protocol_ms->rrc.ue_bch_blocks.SIB14_timeout_value) == 2) {
    protocol_ms->rrc.ue_bch_blocks.SIB14_timeout = TRUE;
  }
  // Wait for message in DC FIFO
  //rrc_ue_read_DCin_FIFO ();  // old version before Netlink sockets
  rrc_ue_read_FIFO();
  // check for a time-out event
  // umts_timer_check_time_out (&protocol_ms->rrc.rrc_timers, protocol_ms->frame_tick_milliseconds);
  //umts_timer_check_time_out (&protocol_ms->rrc.rrc_timers, Mac_rlc_xface->frame/RRC_FRAME_DURATION);
  if (protocol_ms->rrc.rrc_ue_t300_target >0){
  #ifdef DEBUG_RRC_STATE
    msg ("[RRC_UE][DEBUG]rrc_ue_main_scheduler: T300 running at frame %d , target %d, msg length %d\n", 
                     protocol_ms->rrc.current_SFN, protocol_ms->rrc.rrc_ue_t300_target, protocol_ms->rrc.ue_msg_infos.msg_length);
  #endif
  }

  if (protocol_ms->rrc.rrc_ue_t300_target == protocol_ms->rrc.current_SFN){
     rrc_ue_t300_timeout();
  }

  // Measurements
  //rrc_ue_meas_loop();   // for test only
  //check if report of measure needed in UE
  rrc_ue_sync_measures (protocol_ms->rrc.current_SFN, &Message_Id);

   // TEMP - 06/02/2013
   // PDCP does not returm ack in AM mode, so it is simulated here to process with the FSM
  if (protocol_ms->rrc.rrc_ue_ackSimu_flag ==1)
     rrc_ue_simu_receive_ack_from_rlc();

  //Force Uplink RLC communication
  rrc_ue_force_uplink ();

  #ifdef USER_MODE
   fflush(stdout);
  #endif

  return 0;
}

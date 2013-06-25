/***************************************************************************
                          rrc_rg_main.c  -
                          -------------------
    begin                : Tue Jan 15 2002
    copyright            : (C) 2002, 2010 by Eurecom
    created by           : michelle.wetterwald@eurecom.fr
 **************************************************************************
  This file contains the main function of the RRC module
 ***************************************************************************/
/********************
// OpenAir definitions
 ********************/
#include "LAYER2/MAC/extern.h"
#include "UTIL/MEM/mem_block.h"

/********************
// RRC definitions
 ********************/
#include "rrc_rg_vars.h"

//#include "rrc_sap.h"
//#include "rrc_messages.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_int.h"
//#include "rrc_proto_intf.h"
#include "rrc_proto_bch.h"
#include "rrc_proto_mbms.h"
#include "rrc_proto_rrm.h"

//#include "umts_timer_proto_extern.h"

//-----------------------------------------------------------------------------
// entry point for RRC-RG process
//void rrc_rg_main_scheduler (u8 Mod_id){
int rrc_rg_main_scheduler(u8 Mod_id,u32 frame, u8 eNB_flag,u8 index){
//-----------------------------------------------------------------------------
  mem_block_t *p;
  int i;
  #ifdef TEST_MEDIEVAL_DEMO3
  int ix;
  #endif

  //protocol_bs->rrc.current_SFN = frame;
  //  if (protocol_bs->rrc.current_SFN % 50 == 0) {
  #ifdef DEBUG_RRC_DETAILS
  if (protocol_bs->rrc.current_SFN % 50 == 0) {
     msg ("\n\n[RRC][MSG_TEST] System Time : %d\n", protocol_bs->rrc.current_SFN);
  }
  #endif

  // check RRM interface is connected - otherwise, wait for connection
  if (rrc_rrm_main_proc() == RRC_CONNECTED_TO_RRM) {
/*  }else{
  } 
 */     //msg ("\n\n[RRC][TIME_TEST] System Time : %d, RRC Time : %d\n", Mac_rlc_xface->frame, protocol_bs->rrc.current_SFN);

      #ifdef ALLOW_MBMS_PROTOCOL
      //ATTENTION: This must be the first event of RRC process.
      #ifdef DEBUG_RRC_MBMS_SFN
        //msg("[RRC][DEBUG_RRC_MBMS_SFN] 1 - ACTIVITY   frame %d\n",Mac_rlc_xface->frame);
      #endif
      if (p_rg_mbms) // only if MBMS initialized
         rrc_rg_mbms_scheduling_check();
      //rrc_rg_mbms_scenario_check();
      #endif

      // check L1
      rrc_rg_L1_check ();

      // check if there is some message to transmit to NAS and do it (one at a time)
      if ((p = protocol_bs->rrc.NASMessageToXmit) != NULL){
        rrc_rg_write_FIFO (p);
      } else {
        if ((protocol_bs->rrc.eNB_measures_flag == 1) && (protocol_bs->rrc.rg_broadcast_counter % 500 == 2)) {
           RRC_RG_O_NAS_ENB_MEASUREMENT_IND ();
           rrc_rg_write_FIFO (protocol_bs->rrc.NASMessageToXmit);
        }
      }

      //#define TEST_MEDIEVAL_DEMO3
      #ifdef TEST_MEDIEVAL_DEMO3
      if (protocol_bs->rrc.current_SFN >= 16000){
        for (ix=0; ix<maxUsers; ix++){
          protocol_bs->rrc.conf_rlcBufferOccupancy[ix] = 60 - (30*ix);
          protocol_bs->rrc.conf_scheduledPRB[ix] = 500 - (200*ix);
          protocol_bs->rrc.conf_totalDataVolume[ix] = 640000 + (160000*ix);
        }
      } else if ((protocol_bs->rrc.current_SFN > 8000)&& (protocol_bs->rrc.current_SFN < 16000)){
        for (ix=0; ix<maxUsers; ix++){
          protocol_bs->rrc.conf_rlcBufferOccupancy[ix] = 100 - (30*ix);
          protocol_bs->rrc.conf_scheduledPRB[ix] = 300 + (200*ix);
          protocol_bs->rrc.conf_totalDataVolume[ix] = 480000 + (160000*ix);
        }
      }
      #endif
      // Check time-out for SIB14
      // Temp - 256 = Exp time factor (8) * SIB14_Rep (32)
      if (protocol_bs->rrc.rg_broadcast_counter % (protocol_bs->rrc.rg_bch_blocks.SIB14_timeout) == 2) {
        rrc_fill_sib14 ();
        rrc_init_sib14 ();
      }
	  // modulo to improve stability
      protocol_bs->rrc.rg_broadcast_counter = (protocol_bs->rrc.rg_broadcast_counter++) % 1000000000;

      // Read any message in FIFOs from NAS
      rrc_rg_read_FIFO();
      // Read any message in DC FIFO -- To be improved: read only used FIFOs
      for (i = 0; i < maxUsers; i++) {
        //rrc_rg_read_DCin_FIFO (i);
        // Check Timeout on Conn_Setup_Complete
        if (protocol_bs->rrc.Mobile_List[i].conn_complete_timer)
          rrc_rg_temp_checkConnection(i);
      }

      // Read any message in GC FIFO
      //rrc_rg_read_GC_FIFO ();
      // Read any message in NT FIFO
      //rrc_rg_read_NT_FIFO ();

      // Measurements
      //i = rrc_rg_meas_loop();  // for test only
      //check if report of measure needed in RG
      if (protocol_bs->rrc.current_SFN % 800 == 0) {
        #ifdef DEBUG_RRC_DETAILS
        msg ("\n[RRC_MEAS] System Measurement Time : %d\n", protocol_bs->rrc.current_SFN);
        #endif
      }
      if (protocol_bs->rrc.current_SFN > 0)
        rrc_rg_sync_measures (protocol_bs->rrc.current_SFN);
      //
      #ifdef ALLOW_MBMS_PROTOCOL
      if (p_rg_mbms){ // only if MBMS initialized
        rrc_rg_mbms_MCCH_tx();
        // ATTENTION: This must be the last line of RRC process
        rrc_rg_mbms_end_modification_period_check();
      }
      #endif

      //TEST RLC communication
      //rrc_rg_test_lchannels();

      // TODO TO BE REMOVED TEMP -- stop the loop 
      //if (protocol_bs->rrc.current_SFN > 10000){
      // msg ("\n\n[RRC][MSG_TEST] Stop at System Time : %d\n", protocol_bs->rrc.current_SFN);
      //exit(1);
      // }

      #ifdef USER_MODE
        fflush(stdout);
      #endif

  }
  return 0;
}


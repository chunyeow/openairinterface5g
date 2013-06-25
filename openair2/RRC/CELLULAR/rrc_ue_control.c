/***************************************************************************
                          rrc_ue_control.c  -  description
                             -------------------
    copyright            : (C) 2002, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Procedures associated with the control of the UE protocol stack
 ***************************************************************************/
/********************
//OpenAir definitions
 ********************/
#include "LAYER2/MAC/extern.h"
#include "UTIL/MEM/mem_block.h"

/********************
// RRC definitions
 ********************/
#include "rrc_ue_vars.h"
#include "rrc_messages.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_fsm.h"
#include "rrc_proto_intf.h"
//#include "rrc_rrm_proto.h"
#include "rrc_proto_msg.h"
//-----------------------------------------------------------------------------
//#include "rlc.h"

//-----------------------------------------------------------------------------
void rrc_ue_CRLC_Status_rx (int rb_id, int evcP){
//-----------------------------------------------------------------------------
  // temp - Esterel to be updated
  rrc_ue_fsm_control (UE_CRLC_STATUS);
}

// Indications from L1
//-----------------------------------------------------------------------------
void rrc_ue_CPHY_Synch_rx (int CCTrCH){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
   msg ("[RRC-CPHY] In-synch received from PHY for CCTrCH %d \n", CCTrCH);
  #endif
  protocol_ms->rrc.rrc_ue_synch_ind = TRUE;
  protocol_ms->rrc.rrc_ue_cctrch_synch[CCTrCH] = TRUE;
}

//-----------------------------------------------------------------------------
void rrc_ue_CPHY_Out_Synch_rx (int CCTrCH){
//-----------------------------------------------------------------------------
   msg ("[RRC-CPHY] Outsynch received from PHY for CCTrCH %d \n", CCTrCH);
  protocol_ms->rrc.rrc_ue_outsynch_ind = TRUE;
  protocol_ms->rrc.rrc_ue_cctrch_outsynch[CCTrCH] = TRUE;
}

/* Check if an event arrived from L1 */
//-------------------------------------------------------------------
void rrc_ue_L1_check (void){
//-------------------------------------------------------------------
  int  data_ccTrCh;
  int  trch;

  /*
  if (rrc_ue_tick > RRC_UE_TICK_THRESHOLD) {
    rrc_ue_tick=0;
    msg ("[RRC_RG][CPHY] Mobile OUT due to lack of mesures %d \n\n\n \n");
    protocol_ms->rrc.rrc_ue_outsynch_ind=TRUE;
    for (data_ccTrCh = 0; data_ccTrCh < maxCCTrCH_rg; data_ccTrCh++) {
    protocol_ms->rrc.rrc_ue_cctrch_outsynch[data_ccTrCh] = TRUE;
    }
    #ifndef BYPASS_L1
    for (trch = 0; trch < 5; trch++)
      L1L_vars->L1_stats.trch_suc_errors[trch] = 3000;
    #endif
  }
  */
  // First step is to test InSynch conditions
  if (protocol_ms->rrc.rrc_ue_synch_ind == TRUE) {
    //insynch detected- read it
    protocol_ms->rrc.rrc_ue_synch_ind = FALSE;
    for (data_ccTrCh = 0; data_ccTrCh < maxCCTrCH; data_ccTrCh++) {
      if (protocol_ms->rrc.rrc_ue_cctrch_synch[data_ccTrCh] == TRUE) {
        #ifdef DEBUG_RRC_STATE
         msg ("[RRC_UE][CPHY] In-Synch detected for CCTRCH %d \n", data_ccTrCh);
        #endif
        protocol_ms->rrc.rrc_ue_cctrch_synch[data_ccTrCh] = FALSE;
        protocol_ms->rrc.rrc_ue_cctrch[data_ccTrCh] = TRUE;
        rrc_ue_fsm_control (UE_PHY_SETUP);
      }
    }
  }
  // Next step is to test OutOfSynch conditions
  if (protocol_ms->rrc.rrc_ue_outsynch_ind == TRUE) {
    //outsynch detected- read it
    protocol_ms->rrc.rrc_ue_outsynch_ind = FALSE;
    for (data_ccTrCh = 0; data_ccTrCh < maxCCTrCH; data_ccTrCh++) {
      if (protocol_ms->rrc.rrc_ue_cctrch_outsynch[data_ccTrCh] == TRUE) {
        //  #ifdef DEBUG_RRC_STATE
        msg ("[RRC_UE][CPHY] Out-Synch detected for CCTRCH %d \n", data_ccTrCh);
        //   #endif
        protocol_ms->rrc.rrc_ue_cctrch_outsynch[data_ccTrCh] = FALSE;
        rrc_ue_fsm_control (UE_CONN_LOSS);
        if (data_ccTrCh == 0) {
          msg ("[RRC_UE] Restarting 3GPP AS\n");
          //wcdma_handle_error (WCDMA_ERROR_L1_OUT_OF_SYNC);
        }
      }
    }
  }
}

/* Read cell system configuration from SIB18 */
//-------------------------------------------------------------------
void rrc_ue_read_neighboring_cells_info (void){
//-------------------------------------------------------------------
  int i;
  struct rrc_ue_nas_neighbor_list *p;

  // store number of neighboring bases
  p = (struct rrc_ue_nas_neighbor_list *)protocol_ms->rrc.ue_bch_blocks.currSIB18.cellIdentities.data;
  protocol_ms->rrc.rg_ngbr_list.num_cells = p->num_rg;
  if (p->num_rg <= maxCells) {
    // store cell information in list
    for (i = 0; i < p->num_rg; i++) {
      protocol_ms->rrc.rg_ngbr_list.cell_info[i].cell_id = i + 1;
      protocol_ms->rrc.rg_ngbr_list.cell_info[i].pccpch_code_group = protocol_ms->rrc.ue_bch_blocks.currSIB18.cellCodegroups.data[i];
    }
    #ifdef DEBUG_RRC_STATE
     msg ("[RRC_UE][SIB18] Neighbor cells. Number %d \n", protocol_ms->rrc.rg_ngbr_list.num_cells);
    #endif
    #ifdef DEBUG_RRC_BROADCAST
     for (i = 0; i < p->num_rg; i++)
       msg ("[RRC_UE][SIB18] Cell %d #%d , codegroup  %d\n", i + 1, protocol_ms->rrc.rg_ngbr_list.cell_info[i].cell_id, protocol_ms->rrc.rg_ngbr_list.cell_info[i].pccpch_code_group);
    #endif
  } else {
    protocol_ms->rrc.rg_ngbr_list.num_cells = 1;        // Error in config, force standalone cell
    msg ("\n\n ERROR IN RT CONFIGURATION - RG num cells > Max Number of cells \n\n");
  }
}
// TEMP OPEMAIR : The next 4 functions provide some dummy values for L1 and L2 measurements.
//                To be replaced when interface is set-up.

//-------------------------------------------------------------------
// Performs the Intra-Frequency measurements in L1
void rrc_ue_measure_if (void){
//-------------------------------------------------------------------
//    u16 num_measurement_counter;
//    u16 identity;
//    u16 meas_results_type;
//    u16 if_num_cells;
//    u16 if_cell_id[maxCells];
//    u16 if_cell_parms_id[maxCells];
//    u16 if_BCH_RSCP[maxCells];
//    u16 if_pathloss[maxCells];
//    u16 if_slot_iscp[maxCells][maxTS];
  int i,j;

  protocol_ms->rrc.rrc_ue_last_measurement = IX_ifM;

  protocol_ms->rrc.ue_meas_rep[IX_ifM].identity =IX_ifM;
  protocol_ms->rrc.ue_meas_rep[IX_ifM].meas_results_type =MR_intraFreqMeasuredResultsList;

  // Intra-Freq measurements - Temp dummy values
  protocol_ms->rrc.ue_meas_rep[IX_ifM].if_num_cells =MAXMEASCELLS;
  for (i=0; i<protocol_ms->rrc.ue_meas_rep[IX_ifM].if_num_cells; i++){
    protocol_ms->rrc.ue_meas_rep[IX_ifM].if_cell_id[i]=i;
    protocol_ms->rrc.ue_meas_rep[IX_ifM].if_cell_parms_id[i]=16+(4*i); //0-127
    protocol_ms->rrc.ue_meas_rep[IX_ifM].if_BCH_RSCP[i]=100+(10*i);    //0-127
    protocol_ms->rrc.ue_meas_rep[IX_ifM].if_pathloss[i]=46+(100*i);    //46-173
    for (j=0; j<14; j++)
       protocol_ms->rrc.ue_meas_rep[IX_ifM].if_slot_iscp[i][j]=50+(40*i)+j;    //0-127
  }
}

//-------------------------------------------------------------------
// Performs the Internal UE measurements in L1
void rrc_ue_measure_tv (void){
//-------------------------------------------------------------------
  int i, numRBs;

  protocol_ms->rrc.rrc_ue_last_measurement = IX_tvM;

  protocol_ms->rrc.ue_meas_rep[IX_tvM].identity = IX_tvM;
  protocol_ms->rrc.ue_meas_rep[IX_tvM].meas_results_type = MR_trafficVolumeMeasuredResultsList;

  // Traffic Volume measurements - Temp dummy values
  numRBs = MAXMEASRBS;
  protocol_ms->rrc.ue_meas_rep[IX_tvM].tv_num_rbs = numRBs;
  for (i = 0; i < numRBs; i++) {
    protocol_ms->rrc.ue_meas_rep[IX_tvM].tv_rbid[i] = i;
    protocol_ms->rrc.ue_meas_rep[IX_tvM].tv_rb_payload[i] = 2 * i;      //0-19
    protocol_ms->rrc.ue_meas_rep[IX_tvM].tv_rb_average[i] = (2 * i) + 5;        //0-19
    protocol_ms->rrc.ue_meas_rep[IX_tvM].tv_rb_variance[i] = i + 2;     //0-104
  }
}

//-------------------------------------------------------------------
// Performs the Internal UE measurements in L1
void rrc_ue_measure_q (void){
//-------------------------------------------------------------------
//		u16 q_num_TrCH;
//    u16 q_dl_TrCH_id[MAXMEASTRCH];
//    u16 q_dl_TrCH_BLER[MAXMEASTRCH];
//    u16 q_num_tfcs;
//    u16 q_tfcs_id[MAXMEASTFCS];
//		u16 q_sir[MAXMEASTFCS][maxTS];
  int i,j,numTrCH=MAXMEASTRCH,numTFCS=MAXMEASTFCS;

  protocol_ms->rrc.rrc_ue_last_measurement = IX_qM;

  protocol_ms->rrc.ue_meas_rep[IX_qM].identity =IX_qM;
  protocol_ms->rrc.ue_meas_rep[IX_qM].meas_results_type = MR_qualityMeasuredResults;

  // Quality measurements - Temp dummy values
  numTrCH = MAXMEASTRCH;
  protocol_ms->rrc.ue_meas_rep[IX_qM].q_num_TrCH = numTrCH;
  for (i=0; i<numTrCH; i++){
      protocol_ms->rrc.ue_meas_rep[IX_qM].q_dl_TrCH_id[i]=i;
      protocol_ms->rrc.ue_meas_rep[IX_qM].q_dl_TrCH_BLER[i]=((i+1)*4)-1;  //0-63
  }
  numTFCS = MAXMEASTFCS;
  protocol_ms->rrc.ue_meas_rep[IX_qM].q_num_tfcs = numTFCS;
  for (i=0; i<numTFCS; i++){
      protocol_ms->rrc.ue_meas_rep[IX_qM].q_tfcs_id[i]=i;
      for (j=0; j<14; j++)
         protocol_ms->rrc.ue_meas_rep[IX_qM].q_sir[i][j]=(5*(i+1))+j+10;    //0-63
  }
}


//-------------------------------------------------------------------
// Performs the Internal UE measurements in L1
void rrc_ue_measure_int_ue (void){
//-------------------------------------------------------------------
//    u16 num_measurement_counter;
//    u16 identity;
//    u16 meas_results_type;
//    u16 int_xmit_power[maxTS];
//    u16 int_timing_advance;
  int i;

  protocol_ms->rrc.rrc_ue_last_measurement = IX_iueM;

  protocol_ms->rrc.ue_meas_rep[IX_iueM].identity =IX_iueM;
  protocol_ms->rrc.ue_meas_rep[IX_iueM].meas_results_type = MR_ue_InternalMeasuredResults;

  // internal UE measurements - Temp dummy values
  for (i=0; i<14; i++)
      protocol_ms->rrc.ue_meas_rep[IX_iueM].int_xmit_power[i]=25+(6*i);  //0-104
  protocol_ms->rrc.ue_meas_rep[IX_iueM].int_timing_advance =63;      //0-63
}

//-------------------------------------------------------------------
// stop and clean the measurement process
void  rrc_ue_cleanup_meas_ctl (int m_id){
//-------------------------------------------------------------------
  memset (&(protocol_ms->rrc.ue_meas_cmd[m_id]), 0, sizeof (struct rrc_ue_meas_cmd));
}

//-------------------------------------------------------------------
// Activates the measurement process
void rrc_ue_trigger_measure (void){
//-------------------------------------------------------------------
  int  m_id;
  m_id = protocol_ms->rrc.rrc_ue_meas_to_activate;
  switch (protocol_ms->rrc.ue_meas_cmd[m_id].command) {
    case MC_setup:
    case MC_modify:
      #ifdef DEBUG_RRC_STATE
        msg ("[RRC_MSG] UE activates measurement: %d\n\n", m_id);
      #endif
      protocol_ms->rrc.ue_meas_rep[m_id].meas_counter = protocol_ms->rrc.ue_meas_cmd[m_id].rep_amount;
      protocol_ms->rrc.ue_meas_rep[m_id].meas_interval = protocol_ms->rrc.ue_meas_cmd[m_id].rep_interval / 10;
      protocol_ms->rrc.ue_meas_rep[m_id].meas_start = protocol_ms->rrc.current_SFN % protocol_ms->rrc.ue_meas_rep[m_id].meas_interval;
      if (!protocol_ms->rrc.ue_meas_rep[m_id].meas_counter)
        protocol_ms->rrc.ue_meas_rep[m_id].meas_counter = -1;
      break;
    case MC_release:
      #ifdef DEBUG_RRC_STATE
      msg ("[RRC_MSG] UE stops measurement: %d\n\n", m_id);
      #endif
      protocol_ms->rrc.ue_meas_rep[m_id].meas_start = 0;
      protocol_ms->rrc.ue_meas_rep[m_id].meas_counter = 0;
      protocol_ms->rrc.ue_meas_rep[m_id].meas_interval = 0;
      rrc_ue_cleanup_meas_ctl (m_id);
      break;
    default:
      msg ("[RRC_MSG] Invalid Measure command received: %d\n\n", protocol_ms->rrc.ue_meas_cmd[m_id].command);
  }
}

//-------------------------------------------------------------------
// Triggers the necessary measurements in L1
void rrc_ue_sync_measures (int time, int *message_id){
//-------------------------------------------------------------------
  int i;
  for (i = 0; i < MAXMEASTYPES; i++) {
    if ((protocol_ms->rrc.ue_meas_rep[i].meas_counter) && (protocol_ms->rrc.ue_meas_rep[i].meas_start == time % protocol_ms->rrc.ue_meas_rep[i].meas_interval)) {
      switch (i) {
        case IX_ifM:
          // Measurement Report - Intra-frequency
          #ifdef DEBUG_RRC_MEASURE_REPORT_MAIN
            msg ("[RRC_MSG] UE sends Measurement Report (Intra-freq) message at time : %d\n\n", time);
          #endif
          rrc_ue_measure_if ();
          break;
        case IX_tvM:
          // Measurement Report - Traffic Volume
          #ifdef DEBUG_RRC_MEASURE_REPORT_MAIN
            msg ("[RRC_MSG] UE sends Measurement Report (Traffic Volume) message at time : %d\n\n", time);
          #endif
          rrc_ue_measure_tv ();
          break;
        case IX_qM:
          // Measurement Report - Quality
          #ifdef DEBUG_RRC_MEASURE_REPORT_MAIN
            msg ("[RRC_MSG] UE sends Measurement Report (Quality) message at time : %d\n\n", time);
          #endif
          rrc_ue_measure_q ();
          break;
        case IX_iueM:
          // Measurement Report - internal UE
          #ifdef DEBUG_RRC_MEASURE_REPORT_MAIN
            msg ("[RRC_MSG] UE sends Measurement Report (Int UE) message at time : %d\n\n", time);
          #endif
          rrc_ue_measure_int_ue ();
          break;
      }
      protocol_ms->rrc.ue_meas_rep[i].meas_counter--;
      rrc_ue_msg_measrep (message_id);
      if (protocol_ms->rrc.ue_meas_cmd[i].xfer_mode == acknowledgedModeRLC)
        RRC_UE_O_O_SEND_DCCH_AM (*message_id);
      else
        RRC_UE_O_O_SEND_DCCH_UM (*message_id);
      return;
      if (!protocol_ms->rrc.ue_meas_rep[i].meas_counter)
        rrc_ue_cleanup_meas_ctl (i);
    }
  }
}

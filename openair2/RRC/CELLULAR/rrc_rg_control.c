/***************************************************************************
                          rrc_rg_control.c  -  description
                             -------------------
    begin                : November 25, 2002
    copyright            : (C) 2002, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
    email                : Lionel.gauthier@eurecom.fr
 **************************************************************************
  Procedures associated with the control of the RG protocol stack
 ***************************************************************************/
/********************
//OpenAir definitions
 ********************/
#include "LAYER2/MAC/extern.h"
#include "UTIL/MEM/mem_block.h"

/********************
// RRC definitions
 ********************/
//#include "rlc.h"
#include "rrc_rg_vars.h"
#include "rrc_messages.h"
//#include "rrm_as_sap.h"
#include "rrc_qos_classes.h"
//#include "rrc_rrm_primitives.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_intf.h"
#include "rrc_proto_fsm.h"
#include "rrc_proto_int.h"
#include "rrc_proto_mbms.h"
#include "rrc_proto_msg.h"

//-----------------------------------------------------------------------------
// These proto already in rrc_rg.c (ESTEREL file)
//void RRC_RG_O_O_SEND_DCCH_AM (int msgId);

//-----------------------------------------------------------------------------
void rrc_rg_CRLC_Status_rx (int rb_id, int evcP){
//-----------------------------------------------------------------------------
  int             srb_id;
  int             UE_Id;
  srb_id = rb_id % maxRB;
  UE_Id = (rb_id - srb_id) / maxRB;
  msg ("[RRC-CRLC] Outsynch received from RLC for Rb %d , mobile %d\n", rb_id, UE_Id);

  rrc_rg_fsm_control (UE_Id, RG_CRLC_STATUS);
}

// Indications from L1
//-----------------------------------------------------------------------------
void rrc_rg_CPHY_Synch_rx (int CCTrCH){
//-----------------------------------------------------------------------------
#    ifdef DEBUG_RRC_STATE
  msg ("[RRC-CPHY] Insynch received from PHY for CCTrCH %d \n", CCTrCH);
#    endif
  protocol_bs->rrc.rrc_rg_synch_ind = TRUE;
  protocol_bs->rrc.rrc_rg_cctrch_synch[CCTrCH] = TRUE;
}

//-----------------------------------------------------------------------------
void rrc_rg_CPHY_Out_Synch_rx (int CCTrCH){
//-----------------------------------------------------------------------------
  msg ("[RRC-CPHY] Outsynch received from PHY for CCTrCH %d \n", CCTrCH);
  protocol_bs->rrc.rrc_rg_outsynch_ind = TRUE;
  protocol_bs->rrc.rrc_rg_cctrch_outsynch[CCTrCH] = TRUE;
}

/* Check if an event arrived from L1 */
//-------------------------------------------------------------------
void rrc_rg_L1_check (void){
//-------------------------------------------------------------------
/**** Original version - A revoir - UE_ID is rrc_rg_pending_UE
  int UE_Id=0;  // Temp - hard coded
  int data_ccTrCh; // Temp - hard coded
  // First step is to test InSynch conditions
  if (protocol_bs->rrc.rrc_rg_synch_ind==TRUE){
  //insynch detected- read it
     protocol_bs->rrc.rrc_rg_synch_ind=FALSE;
     for (data_ccTrCh=0; data_ccTrCh<maxCCTrCH_rg; data_ccTrCh++){
        if (protocol_bs->rrc.rrc_rg_cctrch_synch[data_ccTrCh]==TRUE){
   #ifdef DEBUG_RRC_STATE
           msg("[RRC_RG][CPHY] In-Synch detected for mobile %d and CCTRCH %d \n",UE_Id, data_ccTrCh);
   #endif
           protocol_bs->rrc.rrc_rg_cctrch_synch[data_ccTrCh]=FALSE;
   			   UE_Id = protocol_bs->rrc.rrc_rg_pending_UE;
   			   protocol_bs->rrc.rrc_rg_cctrch_crnti[data_ccTrCh]= UE_Id;
           //rrc_RG_PHY_Synch_rx(UE_Id);  Now in FSM
           rrc_rg_fsm_control(UE_Id, UE_PHY_SYNCH);
        }
     }
  }

  // Next step is to test OutOfSynch conditions
  if (protocol_bs->rrc.rrc_rg_outsynch_ind==TRUE){
  //outsynch detected- read it
     protocol_bs->rrc.rrc_rg_outsynch_ind=FALSE;
     for (data_ccTrCh=0; data_ccTrCh<maxCCTrCH_rg; data_ccTrCh++){
        if (protocol_bs->rrc.rrc_rg_cctrch_outsynch[data_ccTrCh]==TRUE){
   #ifdef DEBUG_RRC_STATE
           msg("[RRC_RG][CPHY] Out-Synch detected for mobile %d and CCTRCH %d \n",UE_Id, data_ccTrCh);
   #endif
           protocol_bs->rrc.rrc_rg_cctrch_outsynch[data_ccTrCh]=FALSE;
   			   UE_Id = protocol_bs->rrc.rrc_rg_cctrch_crnti[data_ccTrCh];
           //rrc_RG_PHY_Connection_Loss_rx(UE_Id); Now in FSM
           rrc_rg_fsm_control(UE_Id, RRC_CONNECT_LOSS);
        }
     }
  }
*/


  int UE_Id = 0;    // Temp - hard coded
  int data_ccTrCh;  // Temp - hard coded
  // First step is to test InSynch conditions

  // for (UE_Id =0; UE_Id <maxUsers ; UE_Id ++){
  //
  /*
  if (rrc_rg_tick[UE_Id] > 2000) {   // LG WAS 15000
    rrc_rg_tick[UE_Id] = 0;
    msg ("[RRC_RG][CPHY] Mobile %d OUT due to lack of mesures \n\n\n\n", UE_Id);
    protocol_bs->rrc.rrc_rg_outsynch_ind = TRUE;
    for (data_ccTrCh = 0; data_ccTrCh < maxCCTrCH_rg; data_ccTrCh++) {
      if (UE_Id == protocol_bs->rrc.rrc_rg_cctrch_crnti[data_ccTrCh])
        protocol_bs->rrc.rrc_rg_cctrch_outsynch[data_ccTrCh] = TRUE;
    }
  }
  */
  // if (rrc_rg_tick[UE_Id ]>1000){ 
  //   rrc_rg_fsm_control (UE_Id, RRC_CONNECT_LOSS);
  //   #ifdef DEBUG_RRC_STATE
  //   msg ("[RRC_RG][CPHY] Mobile  %d \n\n\n \n", UE_Id);
  //  #endif
  // }
  // }
  if (protocol_bs->rrc.rrc_rg_synch_ind == TRUE) {
    //insynch detected- read it
    protocol_bs->rrc.rrc_rg_synch_ind = FALSE;
    for (data_ccTrCh = 0; data_ccTrCh < maxCCTrCH_rg; data_ccTrCh++) {
      if (protocol_bs->rrc.rrc_rg_cctrch_synch[data_ccTrCh] == TRUE) {
        #ifdef DEBUG_RRC_STATE
        msg ("[RRC_RG][CPHY] In-Synch detected for mobile %d and CCTRCH %d \n", UE_Id, data_ccTrCh);
        #endif
        protocol_bs->rrc.rrc_rg_cctrch_synch[data_ccTrCh] = FALSE;
        UE_Id = protocol_bs->rrc.rrc_rg_pending_UE;
        protocol_bs->rrc.rrc_rg_cctrch_crnti[data_ccTrCh] = UE_Id;
        //rrc_RG_PHY_Synch_rx(UE_Id);  Now in FSM
        rrc_rg_fsm_control (UE_Id, UE_PHY_SYNCH);
      }
    }
  }
  // Next step is to test OutOfSynch conditions
  if (protocol_bs->rrc.rrc_rg_outsynch_ind == TRUE) {
    //outsynch detected- read it
    protocol_bs->rrc.rrc_rg_outsynch_ind = FALSE;
    for (data_ccTrCh = 0; data_ccTrCh < maxCCTrCH_rg; data_ccTrCh++) {
      if (protocol_bs->rrc.rrc_rg_cctrch_outsynch[data_ccTrCh] == TRUE) {
        #ifdef DEBUG_RRC_STATE
        msg ("[RRC_RG][CPHY] Out-Synch detected for mobile %d and CCTRCH %d \n", UE_Id, data_ccTrCh);
        #endif
        protocol_bs->rrc.rrc_rg_cctrch_outsynch[data_ccTrCh] = FALSE;
        UE_Id = protocol_bs->rrc.rrc_rg_cctrch_crnti[data_ccTrCh];
        //rrc_RG_PHY_Connection_Loss_rx(UE_Id); Now in FSM
        rrc_rg_fsm_control (UE_Id, RRC_CONNECT_LOSS);
      }
    }
  }
}

//-------------------------------------------------------------------
void rrc_process_sib18 (void){
//-------------------------------------------------------------------
  struct rrc_rg_nas_neighbor_list *p;

  p = (struct rrc_rg_nas_neighbor_list *)protocol_bs->rrc.rg_bch_blocks.currSIB18.cellIdentities.data;
//  (char *) p = protocol_bs->rrc.rg_bch_blocks.currSIB18.cellIdentities.data;
  protocol_bs->rrc.num_cells = p->num_rg;
#ifdef DEBUG_RRC_STATE
  msg ("[RRC_RG][SIB18] Number of cells %d \n", protocol_bs->rrc.num_cells);
  if (protocol_bs->rrc.num_cells < protocol_bs->rrc.rg_cell_id)
    msg ("\n\n NETWORK CONFIGURATION - RG cell_id %d,  Number of cells %d\n\n", protocol_bs->rrc.rg_cell_id, protocol_bs->rrc.num_cells);
#endif
}

// TEMP OPEMAIR : The 3 next functions provide some dummy values for L1 and L2 measurements.
//                To be replaced when interface is set-up.

//-------------------------------------------------------------------
// Performs the BS Traffic Volume measurements in L1
void rrc_rg_measure_tv (int meas_ix){
//-------------------------------------------------------------------
  int i, numRBs;

  protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[meas_ix].identity = meas_ix;
  protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[meas_ix].meas_results_type = IX_tvbM;

  // Traffic Volume measurements - Temp dummy values
  numRBs = MAXMEASRBS;
  protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[meas_ix].tv_num_rbs = numRBs;
  for (i = 0; i < numRBs; i++) {
    protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[meas_ix].tv_rbid[i] = i;
    protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[meas_ix].tv_rb_payload[i] = 2 * i;      //0-19
    protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[meas_ix].tv_rb_average[i] = (2 * i) + 5;        //0-19
    protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[meas_ix].tv_rb_variance[i] = i + 2;     //0-104
  }
}

//-------------------------------------------------------------------
// Performs the Quality measurements in L1
void rrc_rg_measure_q (int meas_ix){
//-------------------------------------------------------------------
//    u16 q_num_TrCH;
//    u16 q_dl_TrCH_id[MAXMEASTRCH];
//    u16 q_dl_TrCH_BLER[MAXMEASTRCH];
//    u16 q_num_tfcs;
//    u16 q_tfcs_id[MAXMEASTFCS];
//    u16 q_sir[MAXMEASTFCS][maxTS];
  int i,j,numTrCH=MAXMEASTRCH,numTFCS=MAXMEASTFCS;

  protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[meas_ix].identity =meas_ix;
  protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[meas_ix].meas_results_type = IX_qbM;

  // Quality measurements - Temp dummy values
  numTrCH = MAXMEASTRCH;
  protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[meas_ix].q_num_TrCH = numTrCH;
  for (i=0; i<numTrCH; i++){
      protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[meas_ix].q_dl_TrCH_id[i]=i;
      protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[meas_ix].q_dl_TrCH_BLER[i]=((i+1)*4)-1;  //0-63
  }
  numTFCS = MAXMEASTFCS;
  protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[meas_ix].q_num_tfcs = numTFCS;
  for (i=0; i<numTFCS; i++){
      protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[meas_ix].q_tfcs_id[i]=i;
      for (j=0; j<14; j++)
         protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[meas_ix].q_sir[i][j]=(5*(i+1))+j+10;    //0-63
  }
}

//-------------------------------------------------------------------
// Performs the Internal BS measurements in L1
void rrc_rg_measure_int_bs (int meas_ix){
//-------------------------------------------------------------------
//    u16 num_measurement_counter;
//    u16 identity;
//    u16 meas_results_type;
//    u16 int_xmit_power[numANTENNAS][maxTS];
//    u16 int_rscp[maxTS][MAXCH];
  int i,j,numCH;

  protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[meas_ix].identity =meas_ix;
  protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[meas_ix].meas_results_type = IX_ibsM;

  numCH=MAXCH;
  // internal UE measurements - Temp dummy values
  for (i=0; i<14; i++){
     for (j=0; j<numANTENNAS; j++)
       protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[meas_ix].int_xmit_power[j][i]=25+(6*i)+(2*j);  //0-104
     for (j=0; j<numCH; j++)
       protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[meas_ix].int_rscp[i][j]=12+(3*i)+(2*j);  //range ??
  }
}

//-------------------------------------------------------------------
// Triggers the necessary measurements in L1
void rrc_rg_sync_measures (int time){
//-------------------------------------------------------------------
  int i;

  for (i = 0; i < 2 * maxUsers + 1; i++) {
    //check if measure needed
    if (protocol_bs->rrc.rg_meas_blocks.rrc_rg_bs_meas_next[i] == time) {
      // perform the measurement
      switch (protocol_bs->rrc.rg_meas_blocks.bs_meas_cmd[i].type) {
          case IX_tvbM:
            // Measurement Report - Traffic Volume
#ifdef DEBUG_RRC_MEASURE_REPORT_MAIN
            msg ("[RRC_MSG] RG prepares Measurement Report (Traffic Volume) at time : %d\n\n", time);
#endif
            rrc_rg_measure_tv (i);
            break;
          case IX_qbM:
            // Measurement Report - Quality
#ifdef DEBUG_RRC_MEASURE_REPORT_MAIN
            msg ("[RRC_MSG] RG prepares Measurement Report (Quality) at time : %d\n\n", time);
#endif
            rrc_rg_measure_q (i);
            break;
          case IX_ibsM:
            // Measurement Report - internal UE
#ifdef DEBUG_RRC_MEASURE_REPORT_MAIN
            msg ("[RRC_MSG] RG prepares Measurement Report (Int BS) at time : %d\n\n", time);
#endif
            rrc_rg_measure_int_bs (i);
            break;
      }
      // Print measurements
#ifdef DEBUG_RRC_MEASURE_REPORT_MAIN
      rrc_rg_print_bs_meas_report (&(protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[i]));
#endif

      // Send Measurement to RRM
      rrc_rg_send_bs_meas_report (i);

      // Prepare next measurement if needed
      if (--protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[i].meas_counter) {
        //set next measure in table
        protocol_bs->rrc.rg_meas_blocks.rrc_rg_bs_meas_next[i] = time + protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[i].meas_interval;
      } else {
        // clean up measurement control blocks
        protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[i].meas_start = 0;
        protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[i].meas_counter = 0;
        protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[i].meas_interval = 0;
        memset (&(protocol_bs->rrc.rg_meas_blocks.bs_meas_cmd[i]), 0, sizeof (struct rrc_rg_bs_meas_cmd));
        protocol_bs->rrc.rg_meas_blocks.rrc_rg_bs_meas_next[i] = 0;
      }
    }
  }
}

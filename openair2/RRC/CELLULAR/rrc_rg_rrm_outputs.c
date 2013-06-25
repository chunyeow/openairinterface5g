/***************************************************************************
                          rrc_rg_rrm_intf.c  -  description
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
#include "rtos_header.h"

/********************
// RRC definitions
 ********************/
//#include "rlc.h"
#include "rrc_rg_vars.h"
//#include "rrc_messages.h"
//#include "rrm_as_sap.h"
#include "rrc_qos_classes.h"
#include "rrc_qos_definitions.h"
//-----------------------------------------------------------------------------
//#include "rrc_proto_intf.h"
//#include "rrc_proto_fsm.h"
#include "rrc_proto_int.h"
//#include "rrc_proto_mbms.h"
//#include "rrc_proto_msg.h"
//-----------------------------------------------------------------------------

//-------------------------------------------------------------------
void rrc_rg_init_check_qos_classes(void){
//-------------------------------------------------------------------
  int i;

  msg("\n\n***********************************************\n");
  msg("***      [CHECK QOS CLASSES]               ****\n");
  msg("***********************************************\n");

  for (i=0;i<RRC_QOS_LAST; i++){
   msg("\n Traffic Class %d \n",i+1);
   printk("\trab.traffic_class = %s \n",rrc_traffic_class_names[(rrc_qos_classes_definition [i][0])-1]);
   printk("\trab.guaranted_bit_rate_uplink   = %d \n",rrc_qos_classes_definition [i][1]);
   printk("\trab.max_bit_rate_uplink         = %d \n",rrc_qos_classes_definition [i][2]);
   printk("\trab.guaranted_bit_rate_downlink = %d \n",rrc_qos_classes_definition [i][3]);
   printk("\trab.max_bit_rate_downlink       = %d \n",rrc_qos_classes_definition [i][4]);
  }
}

//-------------------------------------------------------------------
// Request computation of the configuration from RRM
int rrc_rg_compute_configuration (int UE_Id, u8 action){
//-------------------------------------------------------------------
  rpc_message     rpc_mess;
  add_user_request add_user;
  remove_user_request remove_user;
  add_radio_access_bearer_request add_rab;
  remove_radio_access_bearer_request remove_rab;
  int result = SUCCESS;
  int rab_qos_class_ix;
  int count;

  if (protocol_bs->rrc.rrc_currently_updating != FALSE) {
    result = FAILURE;
    return result;
  }

  msg ("[RRC][REQUEST] -> RRM rrm_config_change_request : Transaction ID %d\n", ++(protocol_bs->rrc.curr_transaction_id));

  switch (action) {
      case E_ADD_MT:
        msg ("[RRC][REQUEST] RRC -> RRM ADD_MOBILE %d \n", UE_Id);
        rpc_mess.type = RPC_ADD_USER_REQUEST;
        rpc_mess.length = sizeof (add_user_request);
        rpc_mess.frame = protocol_bs->rrc.current_SFN;
        add_user.equipment_id = protocol_bs->rrc.rc_rrm.equipment_id;
/*        add_user.user_id = config_request.mobile_id;
        add_user.tx_id = config_request.transaction_id;*/
        add_user.user_id = UE_Id;
        add_user.tx_id = protocol_bs->rrc.curr_transaction_id;

        count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & rpc_mess, sizeof (rpc_message));
        count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & add_user, sizeof (struct add_user_request));
        break;
      case E_REL_MT:
        msg ("[RRC][REQUEST] RRC -> RRM REMOVE_MOBILE %d \n", UE_Id);
        rpc_mess.type = RPC_REMOVE_USER_REQUEST;
        rpc_mess.length = sizeof (remove_user_request);
        rpc_mess.frame = protocol_bs->rrc.current_SFN;
        remove_user.equipment_id = protocol_bs->rrc.rc_rrm.equipment_id;
 /*       remove_user.user_id = config_request.mobile_id;
        remove_user.tx_id = config_request.transaction_id;*/
        remove_user.user_id = UE_Id;
        remove_user.tx_id = protocol_bs->rrc.curr_transaction_id;

        count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & rpc_mess, sizeof (rpc_message));
        count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & remove_user, sizeof (struct remove_user_request));
        break;
      case E_ADD_RB:
        rpc_mess.type = RPC_ADD_RADIO_ACCESS_BEARER_REQUEST;
        rpc_mess.length = sizeof (add_radio_access_bearer_request);
        rpc_mess.frame = protocol_bs->rrc.current_SFN;
        add_rab.equipment_id = protocol_bs->rrc.rc_rrm.equipment_id;
/*        add_rab.user_id = config_request.rb_list[0] / maxRB;
        add_rab.tx_id = config_request.transaction_id;
        add_rab.rab_id = config_request.rb_list[0];
        rab_qos_class_ix = config_request.qos_class_list[0].umts_qos_class-1;*/
        add_rab.user_id = UE_Id;
        add_rab.tx_id = protocol_bs->rrc.curr_transaction_id;
        // HARD CODED request.num_rbs = 1; ONLY ONE RB AT A TIME
        add_rab.rab_id = protocol_bs->rrc.Mobile_List[UE_Id].requested_rbId;
        rab_qos_class_ix = (protocol_bs->rrc.Mobile_List[UE_Id].requested_QoSclass)-1;
        if ((rab_qos_class_ix<0)||(rab_qos_class_ix>=RRC_QOS_LAST))
              msg ("\n[RRC][REQUEST] RRC -> RRM MOBILE %d ADD_RADIO_BEARER QOS NOT UNDERSTOOD NO ACTION \n", UE_Id);
        else {
          add_rab.traffic_class = rrc_qos_classes_definition [rab_qos_class_ix][0];
          add_rab.guaranted_bit_rate_uplink   = rrc_qos_classes_definition [rab_qos_class_ix][1];
          add_rab.max_bit_rate_uplink         = rrc_qos_classes_definition [rab_qos_class_ix][2];
          add_rab.guaranted_bit_rate_downlink = rrc_qos_classes_definition [rab_qos_class_ix][3];
          add_rab.max_bit_rate_downlink       = rrc_qos_classes_definition [rab_qos_class_ix][4];
          msg ("[RRC][REQUEST] RRC -> RRM MOBILE %d ADD_RADIO_BEARER %d %s GBR %d Kbits DL %d Kbits UL\n",
                   add_rab.user_id, add_rab.rab_id, rrc_traffic_class_names[add_rab.traffic_class-1],
                   add_rab.guaranted_bit_rate_downlink, add_rab.guaranted_bit_rate_uplink);
        }
        count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & rpc_mess, sizeof (rpc_message));
        count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & add_rab, sizeof (struct add_radio_access_bearer_request));
        break;
      case E_REL_RB:
        msg ("[RRC][REQUEST] RRC -> RRM MOBILE %d REMOVE_RADIO_BEARER %d \n", UE_Id, protocol_bs->rrc.Mobile_List[UE_Id].requested_rbId);
        rpc_mess.type = RPC_REMOVE_RADIO_ACCESS_BEARER_REQUEST;
        rpc_mess.length = sizeof (remove_radio_access_bearer_request);
        rpc_mess.frame = protocol_bs->rrc.current_SFN;
        remove_rab.equipment_id = protocol_bs->rrc.rc_rrm.equipment_id;
/*        remove_rab.tx_id = config_request.transaction_id;
        remove_rab.rab_id = config_request.rb_list[0];
        remove_rab.user_id = config_request.mobile_id;*/
        remove_rab.user_id = UE_Id;
        remove_rab.tx_id = protocol_bs->rrc.curr_transaction_id;
        remove_rab.rab_id = protocol_bs->rrc.Mobile_List[UE_Id].requested_rbId;
        count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & rpc_mess, sizeof (rpc_message));
        count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & remove_rab, sizeof (struct remove_radio_access_bearer_request));
        break;
      default:
        msg ("[RRC][REQUEST] RRC -> RRM Invalid config_request.action %d \n", action);
  }
    protocol_bs->rrc.rrc_currently_updating = TRUE;
    protocol_bs->rrc.rrc_UE_updating = UE_Id;
    protocol_bs->rrc.Mobile_List[UE_Id].requested_sapid = 3;    //Temp - will later be read from a table
    #ifdef DEBUG_RRC_STATE
    msg ("[RRC_RG][RRM] Configuration requested: mobile_id %d , rb_id %d, Qos %d \n",
         UE_Id, protocol_bs->rrc.Mobile_List[UE_Id].requested_rbId, protocol_bs->rrc.Mobile_List[UE_Id].requested_QoSclass);
    #endif
    return result;
}

//-------------------------------------------------------------------
// Report MOBILE CONNECTION COMPLETE
void rrm_add_user_confirm (int ue_idP){
//-------------------------------------------------------------------
  rpc_message     rpc_mess;
  add_user_confirm confirm;
  int count;

  msg ("[RRC-RRM-INTF] RPC_ADD_USER_CONFIRM --> RRM\n ");

  rpc_mess.type = RPC_ADD_USER_CONFIRM;
  rpc_mess.length = sizeof (add_user_confirm);
  confirm.equipment_id = protocol_bs->rrc.rc_rrm.equipment_id;
  confirm.user_id = ue_idP;

  count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & rpc_mess, sizeof (rpc_message));
  count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & confirm, sizeof (add_user_confirm));
}


//-------------------------------------------------------------------
// Report MOBILE CONNECTION COMPLETE
void rrm_add_radio_access_bearer_confirm (int ue_idP, int rb_idP){
//-------------------------------------------------------------------
  rpc_message     rpc_mess;
  add_radio_access_bearer_confirm confirm;
  int count;

  msg ("[RRC-RRM-INTF] RPC_ADD_RADIO_ACCESS_BEARER_CONFIRM --> RRM\n ");

  rpc_mess.type = RPC_ADD_RADIO_ACCESS_BEARER_CONFIRM;
  rpc_mess.length = sizeof (add_radio_access_bearer_confirm);
  confirm.equipment_id = protocol_bs->rrc.rc_rrm.equipment_id;
  confirm.user_id = ue_idP;
  confirm.rab_id = rb_idP;

  count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & rpc_mess, sizeof (rpc_message));
  count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & confirm, sizeof (add_radio_access_bearer_confirm));
}

// Measurement report to RRM
// 1 - from Mobile Terminal
//-------------------------------------------------------------------
// Report Intra-frequency measurements from MT
void rrm_meas_report_mt_if (struct rrc_rrm_meas_report_mt_if *pmeas){
//-------------------------------------------------------------------
  rpc_message     rpc_mess;
  l1_measurement_mt_intra_frequency_report measurement_report;
  int count;

#ifdef DEBUG_RRC_MEASURE_REPORT
  msg ("[RRC][MEASUREMENT REPORT] INTRA FREQUENCY MT frame %d\n", protocol_bs->rrc.current_SFN);
#endif
  rpc_mess.type = RPC_L1_MEASUREMENT_MT_INTRA_FREQUENCY_REPORT;
  rpc_mess.length = sizeof (l1_measurement_mt_intra_frequency_report);
//  rpc_mess.frame = frame;
  rpc_mess.frame = protocol_bs->rrc.current_SFN;
  measurement_report.equipment_id = protocol_bs->rrc.rc_rrm.equipment_id;
  memcpy (&measurement_report.measurements, pmeas, sizeof (struct rrc_rrm_meas_report_mt_if));

  count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & rpc_mess, sizeof (rpc_message));
  count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & measurement_report, sizeof (l1_measurement_mt_intra_frequency_report));

}

//-------------------------------------------------------------------
// Report Traffic Volume measurements from MT
void rrm_meas_report_mt_tv (struct rrc_rrm_meas_report_mt_tv *pmeas){
//-------------------------------------------------------------------
  rpc_message     rpc_mess;
  l1_measurement_mt_traffic_volume_report measurement_report;
  int count;

#ifdef DEBUG_RRC_MEASURE_REPORT
  //msg("[RRC][MEASUREMENT REPORT] TRAFFIC VOLUME MT frame %d\n", protocol_bs->rrc.current_SFN);
#endif
  rpc_mess.type = RPC_L1_MEASUREMENT_MT_TRAFFIC_VOLUME_REPORT;
  rpc_mess.length = sizeof (l1_measurement_mt_traffic_volume_report);
//  rpc_mess.frame = frame;
  rpc_mess.frame = protocol_bs->rrc.current_SFN;
  measurement_report.equipment_id = protocol_bs->rrc.rc_rrm.equipment_id;
  memcpy (&measurement_report.measurements, pmeas, sizeof (struct rrc_rrm_meas_report_mt_tv));

  count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & rpc_mess, sizeof (rpc_message));
  count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & measurement_report, sizeof (l1_measurement_mt_traffic_volume_report));
}

//-------------------------------------------------------------------
// Report Quality measurements from MT
void rrm_meas_report_mt_q (struct rrc_rrm_meas_report_mt_q *pmeas){
//-------------------------------------------------------------------
  rpc_message     rpc_mess;
  l1_measurement_mt_quality_report measurement_report;
  int count;

#ifdef DEBUG_RRC_MEASURE_REPORT
  msg ("[RRC][MEASUREMENT REPORT] QUALITY MT frame %d\n", protocol_bs->rrc.current_SFN);
#endif
  rpc_mess.type = RPC_L1_MEASUREMENT_MT_QUALITY_REPORT;
  rpc_mess.length = sizeof (l1_measurement_mt_quality_report);
//  rpc_mess.frame = frame;
  rpc_mess.frame = protocol_bs->rrc.current_SFN;
  measurement_report.equipment_id = protocol_bs->rrc.rc_rrm.equipment_id;
  memcpy (&measurement_report.measurements, pmeas, sizeof (struct rrc_rrm_meas_report_mt_q));

  count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & rpc_mess, sizeof (rpc_message));
  count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & measurement_report, sizeof (l1_measurement_mt_quality_report));
}

//-------------------------------------------------------------------
// Report UE Internal measurements from MT
void rrm_meas_report_mt_int (struct rrc_rrm_meas_report_mt_int *pmeas){
//-------------------------------------------------------------------
  rpc_message     rpc_mess;
  l1_measurement_mt_internal_report measurement_report;
  int count;

#ifdef DEBUG_RRC_MEASURE_REPORT
  msg ("[RRC][MEASUREMENT REPORT] INTERNAL MT frame %d\n", protocol_bs->rrc.current_SFN);
#endif
  rpc_mess.type = RPC_L1_MEASUREMENT_MT_INTERNAL_REPORT;
  rpc_mess.length = sizeof (l1_measurement_mt_internal_report);
//  rpc_mess.frame = frame;
  rpc_mess.frame = protocol_bs->rrc.current_SFN;
  measurement_report.equipment_id = protocol_bs->rrc.rc_rrm.equipment_id;
  memcpy (&measurement_report.measurements, pmeas, sizeof (struct rrc_rrm_meas_report_mt_int));

  count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & rpc_mess, sizeof (rpc_message));
  count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & measurement_report, sizeof (l1_measurement_mt_internal_report));
}

// 2 - from Base station
//-------------------------------------------------------------------
// Report Traffic Volume measurements from RG
void rrm_meas_report_bs_tv (struct rrc_rrm_meas_report_bs_tv *pmeas){
//-------------------------------------------------------------------
  rpc_message     rpc_mess;
  l1_measurement_rg_traffic_volume_report measurement_report;
  int count;

#ifdef DEBUG_RRC_MEASURE_REPORT
  //msg("[RRC][MEASUREMENT REPORT] TRAFFIC VOLUME RG frame %d\n", protocol_bs->rrc.current_SFN);
#endif
  rpc_mess.type = RPC_L1_MEASUREMENT_RG_TRAFFIC_VOLUME_REPORT;
  rpc_mess.length = sizeof (l1_measurement_rg_traffic_volume_report);
//  rpc_mess.frame = frame;
  rpc_mess.frame = protocol_bs->rrc.current_SFN;
  measurement_report.equipment_id = protocol_bs->rrc.rc_rrm.equipment_id;
  memcpy (&measurement_report.measurements, pmeas, sizeof (struct rrc_rrm_meas_report_bs_tv));

  count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & rpc_mess, sizeof (rpc_message));
  count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & measurement_report, sizeof (struct l1_measurement_rg_traffic_volume_report));
}

//-------------------------------------------------------------------
// Report Quality measurements from RG
void rrm_meas_report_bs_q (struct rrc_rrm_meas_report_bs_q *pmeas){
//-------------------------------------------------------------------
  rpc_message     rpc_mess;
  l1_measurement_rg_internal_report measurement_report;
  int count;

#ifdef DEBUG_RRC_MEASURE_REPORT
  //msg("[RRC][MEASUREMENT REPORT] QUALITY RG frame %d\n", protocol_bs->rrc.current_SFN);
#endif
  rpc_mess.type = RPC_L1_MEASUREMENT_RG_QUALITY_REPORT;
  rpc_mess.length = sizeof (l1_measurement_rg_quality_report);
//  rpc_mess.frame = frame;
  rpc_mess.frame = protocol_bs->rrc.current_SFN;
  measurement_report.equipment_id = protocol_bs->rrc.rc_rrm.equipment_id;
  memcpy (&measurement_report.measurements, pmeas, sizeof (struct rrc_rrm_meas_report_bs_q));

  count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & rpc_mess, sizeof (rpc_message));
  count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & measurement_report, sizeof (struct l1_measurement_rg_quality_report));
}

//-------------------------------------------------------------------
// Report BS Internal measurements from RG
void rrm_meas_report_bs_int (struct rrc_rrm_meas_report_bs_int *pmeas){
//-------------------------------------------------------------------
  rpc_message rpc_mess;
  l1_measurement_rg_internal_report measurement_report;
#ifdef DEBUG_RRC_MEASURE_REPORT
  int slot;
#endif
  int count;

  rpc_mess.type = RPC_L1_MEASUREMENT_RG_INTERNAL_REPORT;
  rpc_mess.length = sizeof (l1_measurement_rg_internal_report);
//  rpc_mess.frame = frame;
  rpc_mess.frame = protocol_bs->rrc.current_SFN;
  measurement_report.equipment_id = protocol_bs->rrc.rc_rrm.equipment_id;
  memcpy (&measurement_report.measurements, pmeas, sizeof (struct rrc_rrm_meas_report_bs_int));

  //msg("[RRC][MEASUREMENT REPORT] INTERNAL RG frame %d length %d\n", protocol_bs->rrc.current_SFN, rpc_mess.length);

#ifdef DEBUG_RRC_MEASURE_REPORT
  msg ("[RG][MEASUREMENT REPORT]\n");
  for (slot = 0; slot < JRRM_SLOTS_PER_FRAME; slot++) {
    msg (" Slot %d  \tRSSI RRM %d", slot, measurement_report.measurements.int_rssi_rf[0][slot]);
    msg ("\tRSSI RRC %d\n", pmeas->int_rssi_rf[0][slot]);
  }
#endif

  count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & rpc_mess, sizeof (rpc_message));
  count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & measurement_report, sizeof (struct l1_measurement_rg_internal_report));
}

//-------------------------------------------------------------------
// Forward MT measurement to RRM
void rrc_rg_fwd_meas_report (int UE_Id){
//-------------------------------------------------------------------
  struct rrc_rg_mt_meas_rep *p;
  union rrc_rrm_meas_report_mt_rrc meas_mt;
  int i, j;

  p = (struct rrc_rg_mt_meas_rep *)&(protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[protocol_bs->rrc.Mobile_List[UE_Id].rrc_rg_last_measurement]);

  switch (p->meas_results_type) {
      case MR_intraFreqMeasuredResultsList:
        meas_mt.meas_rep_mt_if.UE_Id = UE_Id;
        meas_mt.meas_rep_mt_if.if_num_cells = p->if_num_cells;
        for (i = 0; i < p->if_num_cells; i++) {
          meas_mt.meas_rep_mt_if.if_cell_id[i] = p->if_cell_id[i];
          meas_mt.meas_rep_mt_if.if_cell_parms_id[i] = p->if_cell_parms_id[i];
          meas_mt.meas_rep_mt_if.if_BCH_RSCP[i] = p->if_BCH_RSCP[i];
          meas_mt.meas_rep_mt_if.if_pathloss[i] = p->if_pathloss[i];
          for (j = 0; j < JRRM_SLOTS_PER_FRAME; j++)
            meas_mt.meas_rep_mt_if.if_slot_iscp[i][j] = p->if_slot_iscp[i][j];
        }
#ifdef DEBUG_RRC_FORWARD_MT_MEASURE_REPORT
        msg ("[RRC_RG][MEAS]Reporting MT Intra Frequency Measurement to RRM at time %d\n", protocol_bs->rrc.current_SFN);
#endif
        rrm_meas_report_mt_if (&(meas_mt.meas_rep_mt_if));
        break;
      case MR_trafficVolumeMeasuredResultsList:
        meas_mt.meas_rep_mt_tv.UE_Id = UE_Id;
        meas_mt.meas_rep_mt_tv.tv_num_rbs = p->tv_num_rbs;
        for (i = 0; i < p->tv_num_rbs; i++) {
          meas_mt.meas_rep_mt_tv.tv_rbid[i] = p->tv_rbid[i];
          meas_mt.meas_rep_mt_tv.tv_rb_payload[i] = p->tv_rb_payload[i];
          meas_mt.meas_rep_mt_tv.tv_rb_average[i] = p->tv_rb_average[i];
          meas_mt.meas_rep_mt_tv.tv_rb_variance[i] = p->tv_rb_variance[i];
        }
#ifdef DEBUG_RRC_FORWARD_MT_MEASURE_REPORT
        msg ("[RRC_RG][MEAS]Reporting MT Traffic Volume Measurement to RRM at time %d\n", protocol_bs->rrc.current_SFN);
#endif
        rrm_meas_report_mt_tv (&(meas_mt.meas_rep_mt_tv));
        break;
      case MR_qualityMeasuredResults:
        meas_mt.meas_rep_mt_q.UE_Id = UE_Id;
        meas_mt.meas_rep_mt_q.q_num_TrCH = p->q_num_TrCH;
        for (i = 0; i < p->q_num_TrCH; i++) {
          meas_mt.meas_rep_mt_q.q_dl_TrCH_id[i] = p->q_dl_TrCH_id[i];
          meas_mt.meas_rep_mt_q.q_dl_TrCH_BLER[i] = p->q_dl_TrCH_BLER[i];
        }
        meas_mt.meas_rep_mt_q.q_num_tfcs = p->q_num_tfcs;
        for (i = 0; i < p->q_num_tfcs; i++) {
#ifdef DEBUG_RRC_FORWARD_MT_MEASURE_REPORT
          msg ("[RRC_RG][MEAS] Reporting MT  %d  SIR(dB) for slots  \n", UE_Id);
#endif
          meas_mt.meas_rep_mt_q.q_tfcs_id[i] = p->q_tfcs_id[i];
          for (j = 0; j < JRRM_SLOTS_PER_FRAME; j++)
            meas_mt.meas_rep_mt_q.q_sir[i][j] = p->q_sir[i][j];
#ifdef DEBUG_RRC_FORWARD_MT_MEASURE_REPORT
          msg ("%d:%d-", j, meas_mt.meas_rep_mt_q.q_sir[i][j]);
#endif
        }
#ifdef DEBUG_RRC_FORWARD_MT_MEASURE_REPORT
        msg ("\n[RRC_RG][MEAS]Reporting MT Quality Measurement to RRM at time %d\n", protocol_bs->rrc.current_SFN);
#endif
        rrm_meas_report_mt_q (&(meas_mt.meas_rep_mt_q));
        break;
      case MR_ue_InternalMeasuredResults:
        meas_mt.meas_rep_mt_int.UE_Id = UE_Id;
        for (i = 0; i < JRRM_SLOTS_PER_FRAME; i++) {
          meas_mt.meas_rep_mt_int.int_xmit_power[i] = p->int_xmit_power[i];
        }
        meas_mt.meas_rep_mt_int.int_timing_advance = p->int_timing_advance;
#ifdef DEBUG_RRC_FORWARD_MT_MEASURE_REPORT
        msg ("[RRC_RG][MEAS]Reporting UE internal Measurement to RRM at time %d\n", protocol_bs->rrc.current_SFN);
#endif
        rrm_meas_report_mt_int (&(meas_mt.meas_rep_mt_int));
        break;
  }
}

//-----------------------------------------------------------------------------
void rrc_rg_send_bs_meas_report (int index){
//-----------------------------------------------------------------------------
  int i, j;
  struct rrc_rg_bs_meas_rep *p;
  union rrc_rrm_meas_report_bs_rrc meas_bs;

  p = (struct rrc_rg_bs_meas_rep *)&(protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[index]);

  switch (p->meas_results_type) {
      case IX_tvbM:
        meas_bs.meas_rep_bs_tv.UE_Id = index - 1;
        meas_bs.meas_rep_bs_tv.tv_num_rbs = p->tv_num_rbs;
        for (i = 0; i < p->tv_num_rbs; i++) {
          meas_bs.meas_rep_bs_tv.tv_rbid[i] = p->tv_rbid[i];
          meas_bs.meas_rep_bs_tv.tv_rb_payload[i] = p->tv_rb_payload[i];
          meas_bs.meas_rep_bs_tv.tv_rb_average[i] = p->tv_rb_average[i];
          meas_bs.meas_rep_bs_tv.tv_rb_variance[i] = p->tv_rb_variance[i];
        }
#ifdef DEBUG_RRC_MEASURE_REPORT
        msg ("[RRC_RG][MEAS]Reporting BS Traffic Volume Measurement to RRM at time %d\n", protocol_bs->rrc.current_SFN);
#endif
        rrm_meas_report_bs_tv (&(meas_bs.meas_rep_bs_tv));
        break;
      case IX_qbM:
        meas_bs.meas_rep_bs_q.UE_Id = index - maxUsers - 1;
        meas_bs.meas_rep_bs_q.q_num_TrCH = p->q_num_TrCH;
        for (i = 0; i < p->q_num_TrCH; i++) {
          meas_bs.meas_rep_bs_q.q_dl_TrCH_id[i] = p->q_dl_TrCH_id[i];
          meas_bs.meas_rep_bs_q.q_dl_TrCH_BLER[i] = p->q_dl_TrCH_BLER[i];
        }
        meas_bs.meas_rep_bs_q.q_num_tfcs = p->q_num_tfcs;
        for (i = 0; i < p->q_num_tfcs; i++) {
          meas_bs.meas_rep_bs_q.q_tfcs_id[i] = p->q_tfcs_id[i];
          for (j = 0; j < JRRM_SLOTS_PER_FRAME; j++)
            meas_bs.meas_rep_bs_q.q_sir[i][j] = p->q_sir[i][j];
        }
#ifdef DEBUG_RRC_MEASURE_REPORT
        msg ("[RRC_RG][MEAS]Reporting BS Quality Measurement to RRM at time %d\n", protocol_bs->rrc.current_SFN);
#endif
        rrm_meas_report_bs_q (&(meas_bs.meas_rep_bs_q));
        break;
      case IX_ibsM:
        for (i = 0; i < numANTENNAS; i++) {
          for (j = 0; j < JRRM_SLOTS_PER_FRAME; j++) {
            meas_bs.meas_rep_bs_int.int_xmit_power[i][j] = p->int_xmit_power[i][j];
            meas_bs.meas_rep_bs_int.int_rssi_rf[i][j] = p->int_rssi_rf[i][j];
          }
        }
        for (i = 0; i < JRRM_SLOTS_PER_FRAME; i++) {
          for (j = 0; j < MAXCH; j++)
            meas_bs.meas_rep_bs_int.int_rscp[i][j] = p->int_rscp[i][j];
        }
        for (i = 0; i < NUMSPARE; i++) {
          meas_bs.meas_rep_bs_int.int_spare[i] = p->int_spare[i];
        }
#ifdef DEBUG_RRC_MEASURE_REPORT
        msg ("[RRC_RG][MEAS]Reporting BS Internal Measurement to RRM at time %d\n", protocol_bs->rrc.current_SFN);
#endif
        rrm_meas_report_bs_int (&(meas_bs.meas_rep_bs_int));
        break;
  }
}


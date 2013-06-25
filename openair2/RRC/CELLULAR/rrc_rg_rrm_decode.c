/***************************************************************************
                          rrc_rg_rrm_message.c
                          -------------------
    copyright            : (C) 2010 by Eurecom
    created by	         : Lionel.Gauthier@eurecom.fr	
    modified by          : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Message handling for RRM interface
 **************************************************************************/
/********************
//OpenAir definitions
 ********************/
#include "LAYER2/MAC/extern.h"
#include "UTIL/MEM/mem_block.h"

/********************
// RRC definitions
 ********************/
#include "rrc_rg_vars.h"
//#include "rrc_mbms_constant.h"
#include "rrc_messages.h"
#include "rrc_rrm_primitives.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_rrm.h"
#include "rrc_proto_int.h"
#include "rrc_proto_fsm.h"
#include "rrc_proto_intf.h"
// #include "rrc_proto_bch.h"
#include "rrc_proto_mbms.h"
//-----------------------------------------------------------------------------

/****************************************
 * Processes to execute messages from RRM
 ****************************************/
//-------------------------------------------------------------------
void rrc_config_indication (int transaction_Id, int return_code){
//-------------------------------------------------------------------
  int UE_Id;

  UE_Id = protocol_bs->rrc.rrc_UE_updating;
  //Check transaction_id to validate the configuration
  if ((transaction_Id == protocol_bs->rrc.curr_transaction_id) && (!return_code)) {
    // trigger event in the FSM
    #ifdef DEBUG_RRC_STATE
    msg ("[RRC_RG][RRM] Config_indication : RRM transaction_Id received: %d , return code %d \n", transaction_Id, return_code);
    #endif

    if ( protocol_bs->rrc.rrc_multicast_bearer == FALSE){
       // continue unicast bearer procedure
       rrc_rg_fsm_control (UE_Id, RRM_CFG);
    }else{
       // continue MBMS bearer procedure
       protocol_bs->rrc.mbms.l12ConfigurationChanged = TRUE;
       //Update the content for Notification and Unmodif message, set the flags if having changes
       protocol_bs->rrc.mbms.flags.modifiedServicesInformation |= rrc_rg_mbms_addModifService(-1, protocol_bs->rrc.mbms.nas_serviceId, Mod_acquirePTM_RBInfo);
       // increment the number of active services in the RG (TEMP Max =1)
       protocol_bs->rrc.mbms.mbms_num_active_service ++;
       //No need to copy the RG configuration.  Same as in unicast
       //Save MT configuration for later sending in PTMCurrentCellConfiguration
       msg("[RRC_RG][MBMS] compress & copy from  rrm_config->mt_config  to curr_l12Config_data, size %d\n", protocol_bs->rrc.mbms.curr_l12Config_lgth);
       rrc_compress_config ((char *) &(rrm_config->mt_config[UE_Id]), &(protocol_bs->rrc.mbms.curr_l12Config_data), &(protocol_bs->rrc.mbms.curr_l12Config_lgth));
       protocol_bs->rrc.mbms.nas_status = ACCEPTED;
       if (p_rg_mbms->ptm_requested_action == E_ADD_RB ){
          RRC_RG_O_O_NAS_MBMS_RB_ESTAB_CNF (); // send response only if activate
          p_rg_mbms->ptm_requested_action = 0;
       }
    }
  }else{
    #ifdef DEBUG_RRC_STATE
    msg ("[RRC_RG][RRM] RRM transaction_Id received: %d , expected %d \n", transaction_Id, protocol_bs->rrc.curr_transaction_id);
    msg ("[RRC_RG][RRM] RRM error return code received: %d \n", return_code);
    #endif
    if ( protocol_bs->rrc.rrc_multicast_bearer == FALSE){
       // if not multicast drop the computed configuration procedure and
       // inform NAS
       rrc_rg_fsm_control (UE_Id, RRM_FAILURE);
    }else{
       // clean variables
       protocol_bs->rrc.mbms.nas_status = FAILURE;
       RRC_RG_O_O_NAS_MBMS_RB_ESTAB_CNF ();
       protocol_bs->rrc.rrc_multicast_bearer = FALSE;
    }
  }
    protocol_bs->rrc.rrc_currently_updating = FALSE;
}

//-------------------------------------------------------------------
// Function called when a Measure Command from RRM is received by RRC
void rrc_rrm_measure_request (struct rrc_rrm_measure_ctl rrm_control){
//-------------------------------------------------------------------
  int i, UE_Id;
  int mesg;
  struct rrc_rg_bs_meas_cmd *pCmd;

  protocol_bs->rrc.current_SFN = Mac_rlc_xface->frame;
  if (!rrm_control.BaseStation) {
#ifdef DEBUG_RRC_MEASURE_CONTROL
    msg ("\n[RRC_RG_RRM] RRM requests measurement in Mobile Station\n");
#endif
    if (rrm_control.UE_Id >= maxUsers)
      return;
    UE_Id = rrm_control.UE_Id;
    //send it only if connected mode
    if ((protocol_bs->rrc.Mobile_List[UE_Id].state != CELL_FACH_Connected)
        && (protocol_bs->rrc.Mobile_List[UE_Id].state != CELL_DCH_Connected)) {
#ifdef DEBUG_RRC_MEASURE_CONTROL
      msg ("[RRC_RG_RRM] Mobile Station is not connected. Command dropped.\n");
#endif
      return;
    }
    protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.command = rrm_control.command;
    protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.criteria_type = RC_PeriodicalReportingCriteria;
    protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.rep_amount = rrm_control.amount;
    protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.rep_interval = rrm_control.period;
    protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.identity = rrm_control.type;
    //  reporting mode  UM-Periodical
    protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.xfer_mode = unacknowledgedModeRLC;
    protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.trigger = periodical;

    if (rrm_control.command == RRC_RRM_SETUP) {
      switch (rrm_control.type) {
          case IX_ifM:
            // measurementType  intraFrequencyMeasurement
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.type = MT_intraFrequencyMeasurement;
            // intraFrequency Measurement parameters
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.if_coeff = coeff0;  //default value
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.if_cellIdentity_ri = TRUE;
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.if_timeslotISCP_ri = TRUE;
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.if_BCH_RSCP_ri = TRUE;
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.if_pathloss_ri = TRUE;
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.if_validity = mv_all_States;
            break;
          case IX_tvM:
            // measurementType  trafficVolumeMeasurement
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.type = MT_trafficVolumeMeasurement;
            // trafficVolumeMeasurement parameters
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.tv_validity = mv_all_States;
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.tv_payload_ri = TRUE;
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.tv_average_ri = TRUE;
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.tv_variance_ri = TRUE;
            break;
          case IX_qM:
            // measurementType  qualityMeasurement
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.type = MT_qualityMeasurement;
            // qualityMeasurement parameters
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.q_dl_trch_bler_ri = TRUE;
            for (i = 0; i < MAXMEASTFCS; i++)
              protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.q_sir_TFCSid[i] = i + 1;
            break;
          case IX_iueM:
            // measurementType  ue_InternalMeasurement
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.type = MT_ue_InternalMeasurement;
            // ue_InternalMeasurement parameters
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.int_quantity = ue_TransmittedPower;
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.int_coeff = coeff0; //default value
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.int_rep_ue_TransmittedPower = TRUE;
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.int_rep_appliedTA = TRUE;
            break;
          default:
            msg ("\n[RRC_RG_RRM] RRM : Invalid type of measurement requested for Mobile Terminal\n");
            return;
      }
    }
    //send to UE
    rrc_rg_msg_measctl (UE_Id, &mesg);
    RRC_RG_O_O_SEND_DCCH_AM (mesg);
  } else {
#ifdef DEBUG_RRC_MEASURE_CONTROL
    msg ("\n[RRC_RG_RRM] RRM requests measurement in Base Station\n");
#endif
    i = 0;
    switch (rrm_control.type) {
        case IX_ibsM:          // index = 0
          i = 0;
          break;
        case IX_tvbM:          // index = 1 -> maxUsers
          if (rrm_control.UE_Id >= maxUsers)
            return;
          i = rrm_control.UE_Id + 1;
          break;
        case IX_qbM:           // index = maxUsers+1 -> 2*maxUsers
          if (rrm_control.UE_Id >= maxUsers)
            return;
          i = maxUsers + rrm_control.UE_Id + 1;
          break;
        default:
          msg ("\n[RRC_RG_RRM] RRM : Invalid type of measurement requested in Base Station\n");
          return;
    }
    pCmd = &(protocol_bs->rrc.rg_meas_blocks.bs_meas_cmd[i]);
    pCmd->identity = i;
    pCmd->command = rrm_control.command;
    pCmd->UE_Id = rrm_control.UE_Id;


    // activate or stop measurement
    switch (pCmd->command) {
        case MC_setup:
        case MC_modify:
          pCmd->trigger = periodical;
          pCmd->type = rrm_control.type;
          pCmd->criteria_type = RC_PeriodicalReportingCriteria;
          pCmd->rep_amount = rrm_control.amount;
          pCmd->rep_interval = rrm_control.period;
#ifdef DEBUG_RRC_MEASURE_CONTROL
          rrc_rg_print_meas_bs_control (pCmd);
          msg ("\n[RRC_MSG] BS activates measurement: %d, type: %d for Mobile: %d.\n\n", pCmd->identity, pCmd->type, pCmd->UE_Id);
#endif
          protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[i].meas_counter = pCmd->rep_amount;
          protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[i].meas_interval = pCmd->rep_interval / 10;
          protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[i].meas_start = protocol_bs->rrc.current_SFN % protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[i].meas_interval;
          if (!protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[i].meas_counter)
            protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[i].meas_counter = -1;
          protocol_bs->rrc.rg_meas_blocks.rrc_rg_bs_meas_next[i] = protocol_bs->rrc.current_SFN;
#ifdef DEBUG_RRC_DETAILS
          msg ("\n[RRC_MSG] Next measure at time : %d \n\n", protocol_bs->rrc.rg_meas_blocks.rrc_rg_bs_meas_next[i]);
#endif
          break;
        case MC_release:
#ifdef DEBUG_RRC_MEASURE_CONTROL
          rrc_rg_print_meas_bs_control (pCmd);
          msg ("\n[RRC_MSG] BS stops measurement: %d\n\n", pCmd->identity);
#endif
          protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[i].meas_start = 0;
          protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[i].meas_counter = 0;
          protocol_bs->rrc.rg_meas_blocks.bs_meas_rep[i].meas_interval = 0;
          memset (pCmd, 0, sizeof (struct rrc_rg_bs_meas_cmd));
          protocol_bs->rrc.rg_meas_blocks.rrc_rg_bs_meas_next[i] = 0;
          break;
        default:
          msg ("[RRC_MSG] Invalid Measure command received: %d\n\n", pCmd->command);
    }
  }
}

/****************************************
 * Decode Message from RRM
 ****************************************/
/*--------------------------------------------------------------------*/
void rrc_connection_response (void *mP, int lengthP){
/*--------------------------------------------------------------------*/
  connection_response *cr;
  cr = (connection_response *) mP;

  switch (cr->status) {
      case STATUS_CONNECTION_ACCEPTED:
        msg ("[RRC-RRM-INTF] RX CONNECTION_RESPONSE ACCEPTED eid %d @frame %d\n", cr->equipment_id, protocol_bs->rrc.current_SFN);
        if (protocol_bs->rrc.rc_rrm.connected_to_rrm != RRC_CONNECTED_TO_RRM) {
          rrc_rrm_rcve_config ((u8 *) & cr[1], lengthP - sizeof (connection_response));
          rrc_rg_rrm_connected_init();
          protocol_bs->rrc.rc_rrm.connected_to_rrm = RRC_CONNECTED_TO_RRM;
          protocol_bs->rrc.rc_rrm.equipment_id = cr->equipment_id;
        } else {
          msg ("[RRC-RRM-INTF][WARNING] RX CONNECTION_RESPONSE ACCEPTED eid %d\n BUT ALREADY CONNECTED\n", cr->equipment_id);
        }
        break;

      case STATUS_CONNECTION_REFUSED_ALREADY_CONNECTED:
        msg ("[RRC-RRM-INTF] RX STATUS_CONNECTION_REFUSED_ALREADY_CONNECTED  %d\n", cr->status);
        break;

      case STATUS_CONNECTION_REFUSED_TOO_MANY_RG:
        msg ("[RRC-RRM-INTF] RX STATUS_CONNECTION_REFUSED_TOO_MANY_RG  %d\n", cr->status);
        //rrc_event(&cx.connection_state, CX_EVT_CX_REFUSED);
        break;

      default:
        msg ("[RRC-RRM-INTF] RX CONNECTION_RESPONSE UNKNOWN STATUS %d\n", cr->status);
  }
}

/*--------------------------------------------------------------------*/
void rrc_add_user_response (void *mP, int lengthP){
    /*--------------------------------------------------------------------*/
  add_user_response *r;

  r = (add_user_response *) mP;

  msg ("---------------------------------------------------------------------------\n");
  msg ("[RRC-RRM-INTF] RX ADD_USER_RESPONSE %d TXID %d status %d %d bytes frame %d\n", r->user_id, r->tx_id, r->status, lengthP, protocol_bs->rrc.current_SFN);
  msg ("---------------------------------------------------------------------------\n");
  if (r->status == ADD_USER_SUCCESSFUL) {
    rrc_rrm_rcve_config ((u8 *) & r[1], lengthP - sizeof (add_user_response));
    rrc_config_indication (r->tx_id, 0);
  } else {
    rrc_config_indication (r->tx_id, 1);
  }
}

/*--------------------------------------------------------------------*/
void rrc_remove_user_response (void *mP, int lengthP){
/*--------------------------------------------------------------------*/
  remove_user_response *r;

  r = (remove_user_response *) mP;

  msg ("---------------------------------------------------------------------------\n");
  msg ("[RRC-RRM-INTF] RX REMOVE_USER_RESPONSE %d TXID %d status %d %d bytes @frame %d\n", r->user_id, r->tx_id, r->status, lengthP, protocol_bs->rrc.current_SFN);
  msg ("---------------------------------------------------------------------------\n");
  if (r->status == REMOVE_USER_SUCCESSFUL) {
    rrc_rrm_rcve_config ((u8 *) & r[1], lengthP - sizeof (remove_user_response));
    rrc_config_indication (r->tx_id, 0);
  } else {
    rrc_config_indication (r->tx_id, 1);
  }
}

/*--------------------------------------------------------------------*/
void rrc_add_radio_access_bearer_response (void *mP, int lengthP){
    /*--------------------------------------------------------------------*/
  add_radio_access_bearer_response *r;

  r = (add_radio_access_bearer_response *) mP;
  msg ("[RRC-RRM-INTF] rc.rx_bytes=%d\n", protocol_bs->rrc.rc_rrm.rx_bytes);
  msg ("---------------------------------------------------------------------------\n");
  msg ("[RRC-RRM-INTF] RX ADD_RADIO_ACCESS_BEARER_RESPONSE TXID %d status %d %d bytes @frame %d\n", r->tx_id, r->status, lengthP, protocol_bs->rrc.current_SFN);
  msg ("---------------------------------------------------------------------------\n");

  if (r->status == ADD_RADIO_ACCESS_BEARER_SUCCESSFUL) {
    rrc_rrm_rcve_config ((u8 *) & r[1], lengthP - sizeof (add_radio_access_bearer_response));
    rrc_config_indication (r->tx_id, 0);
  } else {
    rrc_config_indication (r->tx_id, 1);
  }
}
/*--------------------------------------------------------------------*/
void rrc_remove_radio_access_bearer_response (void *mP, int lengthP){
    /*--------------------------------------------------------------------*/
  remove_radio_access_bearer_response *r;

  r = (remove_radio_access_bearer_response *) mP;

  msg ("---------------------------------------------------------------------------\n");
  msg ("[RRC-RRM-INTF] RX REMOVE_RADIO_ACCESS_BEARER_RESPONSE TXID %d status %d %d bytes @frame %d\n", r->tx_id, r->status, lengthP, protocol_bs->rrc.current_SFN);
  msg ("---------------------------------------------------------------------------\n");
  if (r->status == REMOVE_RADIO_ACCESS_BEARER_SUCCESSFUL) {
    rrc_rrm_rcve_config ((u8 *) & r[1], lengthP - sizeof (remove_radio_access_bearer_response));
    rrc_config_indication (r->tx_id, 0);
  } else {
    rrc_config_indication (r->tx_id, 1);
  }
}

/*--------------------------------------------------------------------*/
void rrc_measurement_request (void *mP, int lengthP){
    /*--------------------------------------------------------------------*/
  measurement_request *r;
  struct rrc_rrm_measure_ctl rrm_control;

  r = (measurement_request *) mP;
  rrm_control.type = r->measurement_type;
  rrm_control.command = r->measurement_command;
  if (r->equipment_type == MOBILE_TERMINAL) {
    rrm_control.BaseStation = 0;
  } else {
    rrm_control.BaseStation = 1;
  }
  rrm_control.UE_Id = r->equipment_id;
  rrm_control.amount = r->amount;
  if (r->period == 12800){
    rrm_control.period = 2000;    //r->period * 10; Coming from RRM
  }else{
    rrm_control.period = r->period;    //r->period * 10; Coming from test tool
  }
  msg ("---------------------------------------------------------------------------\n");
  msg ("[RRC-RRM-INTF] RX MEASUREMENT_REQUEST  @frame %d  length %d type %d command %d isRG %d id %d amount %d period %d\n",
       protocol_bs->rrc.current_SFN, lengthP, rrm_control.type, rrm_control.command, rrm_control.BaseStation, rrm_control.UE_Id, rrm_control.amount, rrm_control.period);
  msg ("---------------------------------------------------------------------------\n");
  rrc_rrm_measure_request (rrm_control);
}

/****************************************
 * identify message from RRM and call decoder
 ****************************************/
/*--------------------------------------------------------------------*/
void rrc_rrm_decode_message (void){
    /*--------------------------------------------------------------------*/
  rpc_message *m;
  #ifdef DEBUG_RRC_RRM_INTF
  msg ("[RRC-RRM-INTF][DECODE MESSAGE]   rrc_rrm_decode_message - begin \n");
  msg ("  size rx_bytes %d, size (rpc_message) %d \n", protocol_bs->rrc.rc_rrm.rx_bytes, sizeof (rpc_message));
  #endif
  while ((protocol_bs->rrc.rc_rrm.rx_bytes) > sizeof (rpc_message)) {
    m = (rpc_message *)protocol_bs->rrc.rc_rrm.rx_buffer;
    #ifdef DEBUG_RRC_RRM_INTF
    msg("[RRC-RRM-INTF][DECODE MESSAGE]  PACKET  length %d (header removed)\n", m->length);
    #endif
    memset (protocol_bs->rrc.rc_rrm.rx_message, 0, RRM_MAX_MESSAGE_SIZE);
    switch (m->type) {
      case RPC_CONNECTION_RESPONSE:
        memcpy (protocol_bs->rrc.rc_rrm.rx_message, &protocol_bs->rrc.rc_rrm.rx_buffer[sizeof (rpc_message)], m->length);
        protocol_bs->rrc.rc_rrm.rx_bytes = protocol_bs->rrc.rc_rrm.rx_bytes - m->length - sizeof (rpc_message);
        #ifdef DEBUG_RRC_RRM_INTF
        msg("[RRC-RRM-INTF][DECODE MESSAGE] Remaining message in queue, length %d\n", protocol_bs->rrc.rc_rrm.rx_bytes);
        #endif
        rrc_connection_response (protocol_bs->rrc.rc_rrm.rx_message, m->length);
        break;
      case RPC_ADD_USER_RESPONSE:
        memcpy (protocol_bs->rrc.rc_rrm.rx_message, &protocol_bs->rrc.rc_rrm.rx_buffer[sizeof (rpc_message)], m->length);
        protocol_bs->rrc.rc_rrm.rx_bytes = protocol_bs->rrc.rc_rrm.rx_bytes - m->length - sizeof (rpc_message);
        #ifdef DEBUG_RRC_RRM_INTF
        msg("[RRC-RRM-INTF][DECODE MESSAGE] Remaining message in queue, length %d\n", protocol_bs->rrc.rc_rrm.rx_bytes);
        #endif
        rrc_add_user_response (protocol_bs->rrc.rc_rrm.rx_message, m->length);
        break;
      case RPC_REMOVE_USER_RESPONSE:
        memcpy (protocol_bs->rrc.rc_rrm.rx_message, &protocol_bs->rrc.rc_rrm.rx_buffer[sizeof (rpc_message)], m->length);
        protocol_bs->rrc.rc_rrm.rx_bytes = protocol_bs->rrc.rc_rrm.rx_bytes - m->length - sizeof (rpc_message);
        #ifdef DEBUG_RRC_RRM_INTF
        msg("[RRC-RRM-INTF][DECODE MESSAGE] Remaining message in queue, length %d\n", protocol_bs->rrc.rc_rrm.rx_bytes);
        #endif
        rrc_remove_user_response (protocol_bs->rrc.rc_rrm.rx_message, m->length);
        break;
      case RPC_ADD_RADIO_ACCESS_BEARER_RESPONSE:
        memcpy (protocol_bs->rrc.rc_rrm.rx_message, &protocol_bs->rrc.rc_rrm.rx_buffer[sizeof (rpc_message)], m->length);
        protocol_bs->rrc.rc_rrm.rx_bytes = protocol_bs->rrc.rc_rrm.rx_bytes - m->length - sizeof (rpc_message);
        #ifdef DEBUG_RRC_RRM_INTF
        msg("[RRC-RRM-INTF][DECODE MESSAGE] Remaining message in queue, length %d\n", protocol_bs->rrc.rc_rrm.rx_bytes);
        #endif
        rrc_add_radio_access_bearer_response (protocol_bs->rrc.rc_rrm.rx_message, m->length);
        break;
      case RPC_REMOVE_RADIO_ACCESS_BEARER_RESPONSE:
        memcpy (protocol_bs->rrc.rc_rrm.rx_message, &protocol_bs->rrc.rc_rrm.rx_buffer[sizeof (rpc_message)], m->length);
        protocol_bs->rrc.rc_rrm.rx_bytes = protocol_bs->rrc.rc_rrm.rx_bytes - m->length - sizeof (rpc_message);
        #ifdef DEBUG_RRC_RRM_INTF
        msg("[RRC-RRM-INTF][DECODE MESSAGE] Remaining message in queue, length %d\n", protocol_bs->rrc.rc_rrm.rx_bytes);
        #endif
        rrc_remove_radio_access_bearer_response (protocol_bs->rrc.rc_rrm.rx_message, m->length);
        break;
      case RPC_MEASUREMENT_REQUEST:
        memcpy (protocol_bs->rrc.rc_rrm.rx_message, &protocol_bs->rrc.rc_rrm.rx_buffer[sizeof (rpc_message)], m->length);
        protocol_bs->rrc.rc_rrm.rx_bytes = protocol_bs->rrc.rc_rrm.rx_bytes - m->length - sizeof (rpc_message);
        #ifdef DEBUG_RRC_RRM_INTF
        msg("[RRC-RRM-INTF][DECODE MESSAGE] Remaining message in queue, length %d\n", protocol_bs->rrc.rc_rrm.rx_bytes);
        #endif
        rrc_measurement_request (protocol_bs->rrc.rc_rrm.rx_message, m->length);
        break;
      case RPC_CONFIG_REQUEST:
      case RPC_CONNECTION_CLOSE:
      case RPC_UPLINK_PHYSICAL_CHANNEL_CONTROL:
      case RPC_ACK:
        memcpy (protocol_bs->rrc.rc_rrm.rx_message, &protocol_bs->rrc.rc_rrm.rx_buffer[sizeof (rpc_message)], m->length);
        protocol_bs->rrc.rc_rrm.rx_bytes = protocol_bs->rrc.rc_rrm.rx_bytes - m->length - sizeof (rpc_message);
        #ifdef DEBUG_RRC_RRM_INTF
        msg("[RRC-RRM-INTF][DECODE MESSAGE] Remaining message in queue, length %d\n", protocol_bs->rrc.rc_rrm.rx_bytes);
        #endif
        msg ("[RRC-RRM-INTF][WARNING] rrc_rrm_decode_message IGNORED MESSAGE %d\n", m->type);
        break;
      default:
        msg ("[RRC-RRM-INTF][WARNING] rrc_rrm_decode_message UNKNOWN MESSAGE %d FLUSHING BUFFER\n", m->type);
        rrc_connection_response_TEMPdummy();
        protocol_bs->rrc.rc_rrm.rx_bytes = 0;
        return;
    }
  }
}


/*--------------------------------------------------------------------*/
void rrc_connection_response_TEMPdummy (void){
/*--------------------------------------------------------------------*/

  if (protocol_bs->rrc.rc_rrm.connected_to_rrm != RRC_CONNECTED_TO_RRM) {
          rrc_rg_rrm_connected_init();
          protocol_bs->rrc.rc_rrm.connected_to_rrm = RRC_CONNECTED_TO_RRM;
          protocol_bs->rrc.rc_rrm.equipment_id = 0;
          msg ("[RRC-RRM-INTF] TEMP DUMMY RX CONNECTION_RESPONSE ACCEPTED eid 0 @frame %d\n",  protocol_bs->rrc.current_SFN);

  }
}


/***************************************************************************
                          rrc_rg_msg_decode.c  -  description
                             -------------------
    begin                : June 17, 2002
    copyright            : (C) 2002, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Decodes messages received in one of the signalling Radio Bearer
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
#include "rrc_rg_vars.h"
#include "rrc_messages.h"
#include "rrc_qos_classes.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_int.h"
#include "rrc_proto_fsm.h"
#include "rrc_proto_intf.h"
#include "rrc_proto_msg.h"
#include "rrc_proto_bch.h"
#include "rrc_proto_mbms.h"
//-----------------------------------------------------------------------------
// For FIFOS interface
#ifdef USER_MODE
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif


//#define DEBUG_RRC_MEASURE_REPORT

//-----------------------------------------------------------------------------
// Generic function to decode RRC messages received on CCCH
//void rrc_rg_srb0_decode (mem_block_t * sduP, int length){
void rrc_rg_srb0_decode (char * sduP, int length){
//-----------------------------------------------------------------------------
  int UE_Id;
  UL_CCCH_Message *ul_ccch_msg;
  int status = SUCCESS;
  #ifdef DEBUG_RRC_STATE
  //    msg("[RRC_RG][DECODE]Decoding from Srb0 --%d-- \n", &sduP->data[length]);
   msg ("\n[RRC_RG][DECODE]Decoding from Srb0 -- start of message: \n");
   rrc_print_buffer ((char*)&sduP[length], 15);
  #endif
    //msg ("[RRC][SRB-RG] frame received: %s\n", (char*)&sduP[sdu_offset]);
   //ul_ccch_msg = (UL_CCCH_Message *) (&sduP->data[length]);
   ul_ccch_msg = (UL_CCCH_Message *) (&sduP[length]);
  switch (ul_ccch_msg->message.type) {
    case UL_CCCH_rrcConnectionRequest:
      status = rrc_rg_msg_connreq (&UE_Id, ul_ccch_msg);
      if (status == SUCCESS)
        rrc_rg_fsm_control (UE_Id, RRC_CONNECT_REQ);
      break;
    case UL_CCCH_cellUpdate:
      status = rrc_rg_msg_cellUpdate (&UE_Id, ul_ccch_msg);
      if (status == SUCCESS)
        rrc_rg_fsm_control (UE_Id, UE_CELLU);
      break;
    default:
      status = FAILURE;
  }
  #ifdef DEBUG_RRC_STATE
   if (status != SUCCESS)
     msg ("\n[RRC_RG][DECODE]Message from Srb0 could not be decoded. No call to FSM \n");
  #endif

}

//-----------------------------------------------------------------------------
// Generic function to decode RRC messages received on DCCH-UM
void rrc_rg_srb1_decode (int UE_Id, char * sduP, int length){
//-----------------------------------------------------------------------------
  UL_DCCH_Message *ul_dcch_msg;
  int status = SUCCESS;

  #ifdef DEBUG_RRC_STATE
  // msg("[RRC_RG][DECODE]Decoding from Srb1 --%s-- \n", &sduP->data[length]);
   msg ("\n[RRC_RG][DECODE]Decoding from Srb1 -- start of message: \n");
   rrc_print_buffer (&sduP[length], 15);
  #endif
  ul_dcch_msg = (UL_DCCH_Message *) (&sduP[length]);
  switch (ul_dcch_msg->message.type) {
    case UL_DCCH_rrcConnectionReleaseComplete:       //not in 1st step
      status = FAILURE;
      break;
    case UL_DCCH_measurementReport:
      status = rrc_rg_msg_measrepr (UE_Id, ul_dcch_msg);
      if (status == SUCCESS){
        // forward to RRM;
        rrc_rg_fwd_meas_report (UE_Id);
        //rrc_rg_tick[UE_Id] = 0;
      }
      #ifdef DEBUG_RRC_MEASURE_REPORT
       rrc_rg_print_meas_report (&(protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[protocol_bs->rrc.Mobile_List[UE_Id].rrc_rg_last_measurement]));
      #endif
      break;
    default:
      status = FAILURE;
  }
  #ifdef DEBUG_RRC_STATE
   if (status != SUCCESS)
     msg ("\n[RRC_RG][DECODE]Message from Srb1 could not be decoded. No call to FSM \n");
  #endif

}

//-----------------------------------------------------------------------------
// Generic function to decode RRC messages received on DCCH-AM
void rrc_rg_srb2_decode (int UE_Id, char * sduP, int length){
//-----------------------------------------------------------------------------
  UL_DCCH_Message *ul_dcch_msg;
  int status = SUCCESS;

  #ifdef DEBUG_RRC_STATE
   // msg("[RRC_RG][DECODE]Decoding from Srb2 --%s-- \n", &sduP->data[length]);
   msg ("\n[RRC_RG][DECODE]Decoding from Srb2 -- start of message: \n");
   rrc_print_buffer (&sduP[length], 15);
  #endif
  ul_dcch_msg = (UL_DCCH_Message *) (&sduP[length]);
  switch (ul_dcch_msg->message.type) {
      case UL_DCCH_rrcConnectionSetupComplete:
        status = rrc_rg_msg_connsucompl (UE_Id, ul_dcch_msg);
        if (status == SUCCESS)
          #ifdef DEBUG_RRC_STATE
          msg("\n[RRC_RG][DECODE] conn_complete_timer = %d , Resetting \n", protocol_bs->rrc.Mobile_List[UE_Id].conn_complete_timer);
          #endif
        protocol_bs->rrc.Mobile_List[UE_Id].conn_complete_timer =0;
        rrc_rg_fsm_control (UE_Id, RRC_CONN_SETUP_COMP);
        protocol_bs->rrc.num_connected_UEs++;
        break;
      case UL_DCCH_rrcConnectionReleaseComplete:       // Not in 1st step
        status = FAILURE;
        break;
      case UL_DCCH_rrcConnectionReleaseUL:
        status = rrc_rg_msg_connrelUL (UE_Id, ul_dcch_msg);
        if (status == SUCCESS)
          rrc_rg_fsm_control (UE_Id, RRC_CONNECT_RELUL);
        break;
      case UL_DCCH_radioBearerSetupComplete:
        status = rrc_rg_msg_rbsetupcompl (UE_Id, ul_dcch_msg);
        if (status == SUCCESS)
          rrc_rg_config_LTE_default_drb(0);
          #ifdef ALLOW_MBMS_PROTOCOL
          rrc_rg_config_LTE_srb2(0);
          #endif
          rrc_rg_fsm_control (UE_Id, UE_RB_SU_CMP);
        break;
      case UL_DCCH_radioBearerSetupFailure:
        status = rrc_rg_msg_rbsetupfail (UE_Id, ul_dcch_msg);
        if (status == SUCCESS)
          rrc_rg_fsm_control (UE_Id, UE_RB_SU_FAIL);
        break;
      case UL_DCCH_radioBearerReleaseComplete:
        status = rrc_rg_msg_rbreleasecompl (UE_Id, ul_dcch_msg);
        if (status == SUCCESS)
          rrc_rg_config_LTE_default_drb(0);
          rrc_rg_fsm_control (UE_Id, UE_RB_REL_CMP);
        break;
      case UL_DCCH_radioBearerReleaseFailure:
        status = rrc_rg_msg_rbreleasefail (UE_Id, ul_dcch_msg);
        if (status == SUCCESS)
          rrc_rg_fsm_control (UE_Id, UE_RB_REL_FAIL);
        break;
      case UL_DCCH_measurementReport:
        status = rrc_rg_msg_measrepr (UE_Id, ul_dcch_msg);
        if (status == SUCCESS)
          rrc_rg_fwd_meas_report (UE_Id);
        #ifdef DEBUG_RRC_MEASURE_REPORT
         rrc_rg_print_meas_report (&(protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[protocol_bs->rrc.Mobile_List[UE_Id].rrc_rg_last_measurement]));
        #endif
        break;
      case UL_DCCH_ueCapabilityInformation:
        status = rrc_rg_msg_ueCapInfo(UE_Id, ul_dcch_msg);
        if (status == SUCCESS)
          rrc_rg_fsm_control(UE_Id, UE_CAP_INFO);
        break;
      default:
        status = FAILURE;
  }
  #ifdef DEBUG_RRC_STATE
   if (status != SUCCESS)
     msg ("\n[RRC_RG][DECODE]Message from Srb2 could not be decoded. No call to FSM \n");
  #endif
}

//-----------------------------------------------------------------------------
// Generic function to decode RRC messages received on srb3 (NAS messages)
void rrc_rg_srb3_decode (int UE_Id, char * sduP, int length){
//-----------------------------------------------------------------------------
  UL_DCCH_Message *ul_dcch_msg;
  int status = SUCCESS;

  #ifdef DEBUG_RRC_STATE
    // msg("[RRC_RG][DECODE]Decoding from Srb3 --%s-- \n", &sduP->data[length]);
    msg ("\n[RRC_RG][DECODE]Decoding from Srb3 -- start of message: \n");
    rrc_print_buffer (&sduP[length], 15);
  #endif
  ul_dcch_msg = (UL_DCCH_Message *) (&sduP[length]);
  switch (ul_dcch_msg->message.type) {
    case UL_DCCH_initialDirectTransfer:
      status = rrc_rg_msg_initXfer (UE_Id, ul_dcch_msg);
      break;
    case UL_DCCH_uplinkDirectTransfer:
      status = rrc_rg_msg_ulXfer (UE_Id, ul_dcch_msg);
      break;
    default:
      status = FAILURE;
  }
  #ifdef DEBUG_RRC_STATE
   if (status != SUCCESS)
     msg ("\n[RRC_RG][DECODE]Message from Srb3 could not be processed. \n");
  #endif
  if (status == SUCCESS)
    RRC_RG_O_NAS_DATA_IND (UE_Id);
}

//-----------------------------------------------------------------------------
// Read data in DC FIFO
int rrc_rg_read_DCin_FIFO (int UE_Id, u8 *buffer, int count){
//-----------------------------------------------------------------------------
  //int maxlen = NAS_MAX_LENGTH;
  int Message_Id;
  int data_length;
  //u8  rcve_buffer[maxlen];
  struct nas_rg_dc_element *p;
  int prim_length;
  int prim_type;
  #ifdef ALLOW_MBMS_PROTOCOL
   int i;
  #endif

  if (count > 0) {
    #ifdef DEBUG_RRC_STATE
     msg ("[RRC_RG] Message Received from NAS: -%hx- for UE %d\n", buffer[0], UE_Id);
    #endif
    p = (struct nas_rg_dc_element *) buffer;
    prim_length = (int) (p->length);
    prim_type = (int) (p->type);
    #ifdef DEBUG_RRC_STATE
     msg ("[RRC_RG] Primitive Type %d,\t Primitive length %d \n", prim_type, prim_length);
    #endif
    //get the rest of the primitive
    //count += rtf_get (protocol_bs->rrc.rrc_rg_DCIn_fifo[UE_Id], &(rcve_buffer[NAS_TL_SIZE]), prim_length - NAS_TL_SIZE);

    //switch (rcve_buffer[0]) {
    switch (prim_type) {
      case CONN_ESTABLISH_CNF:
        if (protocol_bs->rrc.Mobile_List[UE_Id].local_connection_ref == (int) (p->nasRGDCPrimitive.conn_establish_conf.localConnectionRef)) {
          protocol_bs->rrc.establishment_cause = p->nasRGDCPrimitive.conn_establish_conf.status;
          //Temp FFS
          //protocol_bs->rrc.Mobile_List[UE_Id].rb_id = (int)(p->nasRGDCPrimitive.conn_establish_conf.num_RBs);
          #ifdef DEBUG_RRC_STATE
           msg ("[RRC_RG] primitive length: %d\t", (int) (p->length));
           msg ("Local Connection reference: %d\t", p->nasRGDCPrimitive.conn_establish_conf.localConnectionRef);
           msg ("Establishment status: %d\n", (int) (protocol_bs->rrc.establishment_cause));
          #endif
          rrc_rg_fsm_control (UE_Id, NAS_CONN_CNF);
        }
        break;
      case DATA_TRANSFER_REQ:
        if (protocol_bs->rrc.Mobile_List[UE_Id].local_connection_ref == (int) (p->nasRGDCPrimitive.data_transfer_req.localConnectionRef)) {
          data_length = (u16) p->nasRGDCPrimitive.data_transfer_req.nasDataLength;
          protocol_bs->rrc.Mobile_List[UE_Id].dl_nas_message_ptr = get_free_mem_block (data_length);
          protocol_bs->rrc.Mobile_List[UE_Id].dl_nas_message_lgth = data_length;
          //get the associated data
          #ifndef RRC_NETLINK
          count += rtf_get (protocol_bs->rrc.rrc_rg_DCIn_fifo[UE_Id], (protocol_bs->rrc.Mobile_List[UE_Id].dl_nas_message_ptr)->data, data_length);
          // memcpy((protocol_bs->rrc.Mobile_List[UE_Id].dl_nas_message_ptr)->data,&(rcve_buffer[p->length]),data_length);
          #else
          count += rrc_rg_read_data_from_nlh ((char *)(protocol_bs->rrc.Mobile_List[UE_Id].dl_nas_message_ptr)->data, data_length, (int) (p->length));
          #endif
          #ifdef DEBUG_RRC_STATE
            rrc_print_buffer ((char *)buffer, count);
            msg ("[RRC_RG] DATA_TRANSFER_REQ primitive length: %d\n", (int) (p->length));
            msg ("[RRC_RG] Local Connection reference: %d\n", p->nasRGDCPrimitive.data_transfer_req.localConnectionRef);
            msg ("[RRC_RG] Priority (not used yet): %d\n", p->nasRGDCPrimitive.data_transfer_req.priority);
            msg ("[RRC_RG] Data length: %d\n", data_length);
            rrc_print_buffer ((char *) (protocol_bs->rrc.Mobile_List[UE_Id].dl_nas_message_ptr)->data, data_length);
          #endif
          rrc_rg_msg_dlXfer (UE_Id, &Message_Id);
          RRC_RG_SEND_AM_SRB3 (UE_Id);
          free_mem_block (protocol_bs->rrc.Mobile_List[UE_Id].dl_nas_message_ptr);
        }
        break;
      case RB_ESTABLISH_REQ:
        if (protocol_bs->rrc.Mobile_List[UE_Id].local_connection_ref == (int) (p->nasRGDCPrimitive.rb_establish_req.localConnectionRef)) {
          #ifdef DEBUG_RRC_STATE
            rrc_print_buffer ((char *)buffer, count);
            msg ("[RRC_RG] RB_ESTABLISH_REQ primitive length: %d\n", (int) (p->length));
            msg ("[RRC_RG] Local Connection reference: %d\n", p->nasRGDCPrimitive.rb_establish_req.localConnectionRef);
            msg ("[RRC_RG] RB Id: %d ", p->nasRGDCPrimitive.rb_establish_req.rbId);
            msg ("         QoS Class: %d", p->nasRGDCPrimitive.rb_establish_req.QoSclass);
            msg ("         IP DSCP: %d\n", p->nasRGDCPrimitive.rb_establish_req.dscp);
          #endif
          protocol_bs->rrc.Mobile_List[UE_Id].requested_rbId = p->nasRGDCPrimitive.rb_establish_req.rbId;
          protocol_bs->rrc.Mobile_List[UE_Id].requested_MTrbId = protocol_bs->rrc.Mobile_List[UE_Id].requested_rbId - (UE_Id * maxRB);
          protocol_bs->rrc.Mobile_List[UE_Id].requested_QoSclass = p->nasRGDCPrimitive.rb_establish_req.QoSclass;
          protocol_bs->rrc.Mobile_List[UE_Id].requested_dscp = p->nasRGDCPrimitive.rb_establish_req.dscp;
          if ((p->nasRGDCPrimitive.rb_establish_req.QoSclass >= 1) || (p->nasRGDCPrimitive.rb_establish_req.QoSclass <= RRC_QOS_LAST)) {
            rrc_rg_fsm_control (UE_Id, NAS_RB_ESTAB);
          }else{
            RRC_RG_O_O_NAS_RB_Failure (UE_Id);
          }
        }
        break;
      case RB_RELEASE_REQ:
        if (protocol_bs->rrc.Mobile_List[UE_Id].local_connection_ref == (int) (p->nasRGDCPrimitive.rb_release_req.localConnectionRef)) {
          #ifdef DEBUG_RRC_STATE
            rrc_print_buffer ((char *)buffer, count);
            msg ("[RRC_RG] RB_RELEASE_REQ primitive length: %d\n", (int) (p->length));
            msg ("[RRC_RG] Local Connection reference: %d\n", p->nasRGDCPrimitive.rb_release_req.localConnectionRef);
            msg ("[RRC_RG] RB Id: %d \n", p->nasRGDCPrimitive.rb_release_req.rbId);
            #endif
          protocol_bs->rrc.Mobile_List[UE_Id].requested_rbId = p->nasRGDCPrimitive.rb_establish_req.rbId;
          protocol_bs->rrc.Mobile_List[UE_Id].requested_MTrbId = protocol_bs->rrc.Mobile_List[UE_Id].requested_rbId - (UE_Id * maxRB);
          rrc_rg_fsm_control (UE_Id, NAS_RB_RELEASE);
        }
        break;
      #ifdef ALLOW_MBMS_PROTOCOL
      case MBMS_UE_NOTIFY_REQ:
        if (protocol_bs->rrc.Mobile_List[UE_Id].local_connection_ref  == (int)(p->nasRGDCPrimitive.mbms_ue_notify_req.localConnectionRef)){
          //Copy primitive information into the control block
          p_rg_mbms->nas_ueID = (int)(p->nasRGDCPrimitive.mbms_ue_notify_req.localConnectionRef);
          for (i = 0; i<MAX_MBMS_SERVICES; i++){
            p_rg_mbms->nas_joinedServices[i] = p->nasRGDCPrimitive.mbms_ue_notify_req.joined_services[i].mbms_serviceId;
            p_rg_mbms->nas_leftServices[i] = p->nasRGDCPrimitive.mbms_ue_notify_req.left_services[i].mbms_serviceId;
          }
          #ifdef DEBUG_RRC_STATE
            rrc_print_buffer((char *)buffer,count);
            msg("[RRC_RG][MBMS] MBMS_UE_NOTIFY_REQ primitive length: %d\n",(int)(p->length));
            msg("[RRC_RG][MBMS] Local Connection reference: %d\n",p->nasRGDCPrimitive.mbms_ue_notify_req.localConnectionRef);
            // msg("[RRC_RG][MBMS] Lists of joined services and left services are not shown \n");
          #endif
          // rrc_rg_mbms_NAS_Notification_rx(UE_Id); //send notify to this MT
          rrc_rg_mbms_NAS_Notification_rx(); //send notify to this MT
        }
        break;
      #endif
      // end MBMS  
      // Next message to be transferred to GC FIFO.
      case ENB_MEASUREMENT_REQ:
        // TEMP : CELL_ID coordination to be checked
        //if (protocol_bs->rrc.rg_cell_id  == (int)(p->nasRGDCPrimitive.eNBmeasurement_req.cell_id)){
          protocol_bs->rrc.eNB_measures_flag = 1;
          #ifdef DEBUG_RRC_STATE
            rrc_print_buffer((char *)buffer,count);
            msg("[RRC_RG] ENB_MEASUREMENT_REQ primitive length: %d\n",(int)(p->length));
            msg("[RRC_RG] ENB measurement started for Cell_ID: %d\n",p->nasRGDCPrimitive.eNBmeasurement_req.cell_id);
          #endif
        //}
        break;

      default:
        msg ("[RRC_RG] Invalid message received in DC SAP\n");
        rrc_print_buffer ((char *)buffer, count);
        count = -1;
        break;
    }
  }
  return count;
return 0;
}

//-----------------------------------------------------------------------------
int rrc_rg_read_GC_FIFO (u8 *buffer, int count){
//-----------------------------------------------------------------------------
  int data_length, category, new_period;
  int remaining_data = 0;
  //u8  rcve_buffer[maxlen];
  struct nas_rg_gc_element *p;
  int prim_length;
  int prim_type;

  if (count > 0) {
    #ifdef DEBUG_RRC_BROADCAST
     msg ("[RRC_RG] Message Received from NAS - GC SAP: -%hx- \n", buffer[0]);
    #endif
    #ifdef DEBUG_RRC_BROADCAST_DETAILS
     rrc_print_buffer ((char *)buffer, count);
    #endif
    p = (struct nas_rg_gc_element *) buffer;
    prim_length = (int) (p->length);
    prim_type = (int) (p->type);
    #ifdef DEBUG_RRC_BROADCAST
     msg ("[RRC_RG] Primitive Type %d,\t Primitive length %d \n", prim_type, prim_length);
    #endif
    //get the rest of the primitive
    //count += rtf_get (protocol_bs->rrc.rrc_rg_GC_fifo, &(rcve_buffer[NAS_TL_SIZE]), prim_length - NAS_TL_SIZE);

    switch (prim_type) {
      case INFO_BROADCAST_REQ:
        data_length = (u16) p->nasRGGCPrimitive.broadcast_req.nasDataLength;
        category = p->nasRGGCPrimitive.broadcast_req.category;
        new_period = p->nasRGGCPrimitive.broadcast_req.period;
        switch (category) {
          case NAS_SIB1:
            // size is limited - truncate if too large
            if (data_length > maxSIB1NAS) {
              remaining_data = data_length - maxSIB1NAS;
              data_length = maxSIB1NAS;
            }
            protocol_bs->rrc.rg_bch_blocks.currSIB1.subnet_NAS_SysInfo.numocts = data_length;
            //get the associated data
            #ifndef RRC_NETLINK
            count += rtf_get (protocol_bs->rrc.rrc_rg_GC_fifo, protocol_bs->rrc.rg_bch_blocks.currSIB1.subnet_NAS_SysInfo.data, data_length);
            #else
            count += rrc_rg_read_data_from_nlh ((char *)(protocol_bs->rrc.rg_bch_blocks.currSIB1.subnet_NAS_SysInfo.data), data_length, (int) (p->length));
            #endif
            if (new_period == 0)
              new_period = 100;     //Temp, block one-time shot
            #ifndef USER_MODE
            rrc_update_SIB1_period (new_period);
            #endif
            rrc_init_sib1 ();
            break;
          case NAS_SIB18:
            // size is limited - truncate if too large
            if (data_length > maxSIBNAS) {
              remaining_data = data_length - maxSIBNAS;
              data_length = maxSIBNAS;
            }
            protocol_bs->rrc.rg_bch_blocks.currSIB18.cellIdentities.numocts = data_length;
            //get the associated data
            #ifndef RRC_NETLINK
            count += rtf_get (protocol_bs->rrc.rrc_rg_GC_fifo, protocol_bs->rrc.rg_bch_blocks.currSIB18.cellIdentities.data, data_length);
            #else
            count += rrc_rg_read_data_from_nlh ((char *)(protocol_bs->rrc.rg_bch_blocks.currSIB18.cellIdentities.data), data_length, (int) (p->length));
            #endif
            if (new_period == 0)
              new_period = 500;     //Temp, block one-time shot
            rrc_process_sib18 ();
            #ifndef USER_MODE
            rrc_update_SIB18_period (new_period);
            #endif
            rrc_init_sib18 ();
            break;
          default:
            msg ("[RRC_RG] Invalid categogy received from NAS in Broadcast_req: %d \n", category);
        }
        #ifdef DEBUG_RRC_BROADCAST_DETAILS
          msg ("[RRC_RG] INFO_BROADCAST_REQ primitive length: %d\n", (int) (p->length));
          // rrc_print_buffer(rcve_buffer,100);
          msg ("[RRC_RG] Period: %d\t", new_period);
          msg ("  Category: %d\t", category);
          msg ("  Data length: %d\n", data_length);
          switch (category) {
              case NAS_SIB1:
                msg ("[RRC_RG] Data SIB1 : %s\n", (char *) protocol_bs->rrc.rg_bch_blocks.currSIB1.subnet_NAS_SysInfo.data);
                rrc_print_buffer ((char *) protocol_bs->rrc.rg_bch_blocks.currSIB1.subnet_NAS_SysInfo.data, data_length);
                break;
              case NAS_SIB18:
                msg ("[RRC_RG] Data SIB18 : %s\n", (char *) protocol_bs->rrc.rg_bch_blocks.currSIB18.cellIdentities.data);
                rrc_print_buffer ((char *) protocol_bs->rrc.rg_bch_blocks.currSIB18.cellIdentities.data, data_length);
                break;
              default:
                msg ("[RRC_RG] Unknown category for data in INFO_BROADCAST_REQ primitive %d\n", category);
                break;
          }
        #endif
        //read remaining data if any
        #ifndef RRC_NETLINK
        if (remaining_data > 0 && remaining_data < NAS_MAX_LENGTH) {
          rtf_get (protocol_bs->rrc.rrc_rg_GC_fifo, &(buffer[0]), remaining_data);
        }
        #endif
        break;
        //
      #ifdef ALLOW_MBMS_PROTOCOL
      case MBMS_BEARER_ESTABLISH_REQ:
        // Temp : Only one service and one bearer at a time
        #ifdef DEBUG_RRC_STATE
        rrc_print_buffer((char *)buffer,100);
        msg("[RRC_RG][MBMS] MBMS_BEARER_ESTABLISH_REQ primitive length: %d\n",(int)(p->length));
        msg("[RRC_RG][MBMS] Service Id: %d\n", p->nasRGGCPrimitive.mbms_establish_req.mbms_serviceId);
        msg("[RRC_RG][MBMS] Session Id: %d \n",p->nasRGGCPrimitive.mbms_establish_req.mbms_sessionId);
        msg("[RRC_RG][MBMS] RB Id: %d \n",p->nasRGGCPrimitive.mbms_establish_req.mbms_rbId);
        msg("[RRC_RG][MBMS] QoS Class: %d\n",p->nasRGGCPrimitive.mbms_establish_req.mbms_QoSclass);
        msg("[RRC_RG][MBMS] Sap Id: %d\n",p->nasRGGCPrimitive.mbms_establish_req.mbms_sapId);
        msg("[RRC_RG][MBMS] Duration: %d\n",p->nasRGGCPrimitive.mbms_establish_req.mbms_duration);
        #endif
        if (p_rg_mbms->mbms_num_active_service == 0){
            //Copy all received information into the control block.
            p_rg_mbms->nas_serviceId = p->nasRGGCPrimitive.mbms_establish_req.mbms_serviceId;
            p_rg_mbms->nas_sessionId = p->nasRGGCPrimitive.mbms_establish_req.mbms_sessionId;
            p_rg_mbms->nas_rbId = p->nasRGGCPrimitive.mbms_establish_req.mbms_rbId;
            p_rg_mbms->nas_QoSclass = p->nasRGGCPrimitive.mbms_establish_req.mbms_QoSclass;
            p_rg_mbms->nas_sapId = p->nasRGGCPrimitive.mbms_establish_req.mbms_sapId;
            p_rg_mbms->nas_duration = p->nasRGGCPrimitive.mbms_establish_req.mbms_duration;
            // rrc_rg_mbms_NAS_ServStart_rx((MBMS_ServiceIdentity*) &p->nasRGGCPrimitive.mbms_establish_req.mbms_serviceId);
            rrc_rg_mbms_NAS_ServStart_rx();
        }else{
            // No specific action is planned - Only log an error message
            msg("\n[RRC_RG][MBMS] ERROR - Service Id: %d cannot be activated. Already one Service active.\n\n", p->nasRGGCPrimitive.mbms_establish_req.mbms_serviceId);
            p_rg_mbms->nas_status = FAILURE;
            RRC_RG_O_O_NAS_MBMS_RB_ESTAB_CNF ();
        }
        break;
      case MBMS_BEARER_RELEASE_REQ:
        //Copy all other information into the control block.
        p_rg_mbms->nas_serviceId = p->nasRGGCPrimitive.mbms_release_req.mbms_serviceId;
        p_rg_mbms->nas_sessionId = p->nasRGGCPrimitive.mbms_release_req.mbms_sessionId;
        p_rg_mbms->nas_rbId      = p->nasRGGCPrimitive.mbms_release_req.mbms_rbId;
        #ifdef DEBUG_RRC_STATE
        rrc_print_buffer((char *)buffer,100);
        msg("[RRC_RG][MBMS] MBMS_BEARER_RELEASE_REQ primitive length: %d\n",(int)(p->length));
        msg("[RRC_RG][MBMS] Service Id: %d\n", p->nasRGGCPrimitive.mbms_release_req.mbms_serviceId);
        msg("[RRC_RG][MBMS] Session Id: %d \n",p->nasRGGCPrimitive.mbms_release_req.mbms_sessionId);
        msg("[RRC_RG][MBMS] RB Id: %d \n",p->nasRGGCPrimitive.mbms_release_req.mbms_rbId);
        #endif
        // rrc_rg_mbms_NAS_ServStop_rx((MBMS_ServiceIdentity*) &p->nasRGGCPrimitive.mbms_release_req.mbms_serviceId);
        rrc_rg_mbms_NAS_ServStop_rx();
        break;
      #endif
      // end MBMS
      default:
        // TODO : remove all remaining data
        msg ("[RRC_RG] Invalid message received on GC SAP\n");
        //rrc_print_buffer(rcve_buffer,count);
        count = -1;
        break;
    }
  }
  return count;
}

//-----------------------------------------------------------------------------
int rrc_rg_read_NT_FIFO (u8 *buffer, int count){
//-----------------------------------------------------------------------------
  int data_length;
  int Message_Id;
  int UE_Id;
  struct nas_rg_nt_element *p;
  int prim_length;
  int prim_type;

  if (count > 0) {
    #ifdef DEBUG_RRC_STATE
     msg ("[RRC_RG] Message Received from NAS - NT SAP: -%hx- \n", buffer[0]);
    #endif
    p = (struct nas_rg_nt_element *) buffer;
    prim_length = (int) (p->length);
    prim_type = (int) (p->type);
    #ifdef DEBUG_RRC_STATE
     msg ("[RRC_RG] Primitive Type %d,\t Primitive length %d \n", prim_type, prim_length);
    #endif
    //rrc_print_buffer ((char *)rcve_buffer, 100);
    switch (prim_type) {
      case PAGING_REQ:
        data_length = (u16) p->nasRGNTPrimitive.paging_req.nasDataLength;
        UE_Id = p->nasRGNTPrimitive.paging_req.UeId;
        protocol_bs->rrc.Mobile_List[UE_Id].paging_message_ptr = get_free_mem_block (data_length);
        protocol_bs->rrc.Mobile_List[UE_Id].paging_message_lgth = data_length;
        //get the associated data
        #ifndef RRC_NETLINK
         count += rtf_get (protocol_bs->rrc.rrc_rg_NT_fifo, (protocol_bs->rrc.Mobile_List[UE_Id].paging_message_ptr)->data, data_length);
        #else
         count += rrc_rg_read_data_from_nlh ((char *)(protocol_bs->rrc.Mobile_List[UE_Id].paging_message_ptr)->data, data_length, (int) (p->length));
        #endif
        // memcpy((protocol_bs->rrc.Mobile_List[UE_Id].paging_message_ptr)->data, &(rcve_buffer[p->length]),data_length);
        #ifdef DEBUG_RRC_STATE
          //rrc_print_buffer ((char *)rcve_buffer, 100);
          msg ("[RRC_RG] PAGING_REQ primitive length: %d\n", (int) (p->length));
          msg ("[RRC_RG] UE Id: %d\n", p->nasRGNTPrimitive.paging_req.UeId);
          msg ("[RRC_RG] Data length: %d\n", data_length);
          msg ("[RRC_RG] Data %s\n", (protocol_bs->rrc.Mobile_List[UE_Id].paging_message_ptr)->data);
          rrc_print_buffer ((char *) (protocol_bs->rrc.Mobile_List[UE_Id].paging_message_ptr)->data, data_length);
        #endif
        // Send paging to MS
        rrc_rg_msg_pagingt2 (UE_Id, &Message_Id);
        RRC_RG_O_O_SEND_DCCH_AM (UE_Id);
        free_mem_block (protocol_bs->rrc.Mobile_List[UE_Id].paging_message_ptr);
        break;
      default:
        msg ("[RRC_RG] Invalid message received on NT SAP\n");
        rrc_print_buffer ((char *)buffer, count);
        count = -1;
        break;
    }
  }
  return count;
}

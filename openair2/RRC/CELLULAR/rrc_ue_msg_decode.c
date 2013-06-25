/***************************************************************************
                          rrc_ue_msg_decode.c  -  description
                             -------------------
    begin                : June 21, 2002
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
#include "rrc_ue_vars.h"
#include "rrc_messages.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_fsm.h"
#include "rrc_proto_int.h"
#include "rrc_proto_msg.h"
#include "rrc_proto_mbms.h"
//-----------------------------------------------------------------------------
// For FIFOS interface
#ifdef USER_MODE
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

//-----------------------------------------------------------------------------
// Generic function to decode RRC messages received on CCCH
void rrc_ue_srb0_decode (char * sduP, int length){
//-----------------------------------------------------------------------------
  DL_CCCH_Message *dl_ccch_msg;
  int status = SUCCESS;
  #ifdef DEBUG_RRC_STATE
   msg ("\n[RRC_UE][DECODE]Decoding from Srb0b -- start of message: \n");
   rrc_print_buffer (sduP, 100);
  #endif

  dl_ccch_msg = (DL_CCCH_Message *) (sduP);
   // TEMPComment - OPENAIR
  switch (dl_ccch_msg->message.type) {
    case DL_CCCH_rrcConnectionSetup:
      status = rrc_ue_msg_connsetup (dl_ccch_msg);
      if (status == SUCCESS)
        rrc_ue_fsm_control (RRC_CONNECT_SETUP);
      break;
    case DL_CCCH_rrcConnectionRelease:       // not in 1st step
      status = FAILURE;
      break;
    case DL_CCCH_rrcConnectionReject:
      status = rrc_ue_msg_connreject (dl_ccch_msg);
      if (status == SUCCESS)
        rrc_ue_fsm_control (RRC_CONNECT_REJECT);
      break;
    case DL_CCCH_cellUpdateConfirm:
      status = rrc_ue_msg_cellupdatecnfccch (dl_ccch_msg);
      if (status == SUCCESS)
        rrc_ue_fsm_control (CELLU_CNF);
      break;
    default:
      status = FAILURE;
  }
  if (status != SUCCESS)
    msg ("\n[RRC_UE][DECODE]Message from Srb0 could not be decoded. No call to FSM. %d \n",dl_ccch_msg->message.type);
}

//-----------------------------------------------------------------------------
// Generic function to decode RRC messages received on DCCH-UM
void rrc_ue_srb1_decode (char * sduP, int offset){
//-----------------------------------------------------------------------------
  DL_DCCH_Message *dl_dcch_msg;
  int status = SUCCESS;
  #ifdef DEBUG_RRC_STATE
   msg ("\n[RRC_UE][DECODE]Decoding from Srb1 -- start of message: \n");
   rrc_print_buffer (&sduP[offset], 15);
  #endif

  dl_dcch_msg = (DL_DCCH_Message *) (&sduP[offset]);
    // TEMPComment - OPENAIR
  switch (dl_dcch_msg->message.type) {
    case DL_DCCH_rrcConnectionRelease:       // not in 1st step
      status = FAILURE;
      break;
    case DL_DCCH_cellUpdateConfirm:  // not in 1st step
      status = FAILURE;
      break;
    // MBMS - Receive Notification
    #ifdef ALLOW_MBMS_PROTOCOL
    case DL_DCCH_mbmsModifiedServicesInformation:
      #ifdef DEBUG_RRC_STATE
      msg("\033[0;32m[RRC-UE][MBMS][RX] ModifiedServicesInformation, DCCH-UM(SRB1), frame %d \n\033[0m", protocol_ms->rrc.current_SFN);
      #endif
      status = rrc_ue_mbms_DCCH_decode(dl_dcch_msg);
      break;
    #endif
    default:
      status = FAILURE;
  }
  if (status != SUCCESS)
    msg ("\n[RRC_UE][DECODE]Message from Srb1 could not be decoded. No call to FSM. %d \n", dl_dcch_msg->message.type);

}

//-----------------------------------------------------------------------------
// Generic function to decode RRC messages received on DCCH-AM
void rrc_ue_srb2_decode (char * sduP, int length){
//-----------------------------------------------------------------------------
  DL_DCCH_Message *dl_dcch_msg;
  int status = SUCCESS;
  #ifdef DEBUG_RRC_STATE
  // msg("[RRC_UE][DECODE]Decoding from Srb2 --%s-- \n", &sduP->data[length]);
  msg ("\n[RRC_UE][DECODE]Decoding from Srb2 -- start of message: \n");
  rrc_print_buffer (&sduP[length], 15);
  #endif

  dl_dcch_msg = (DL_DCCH_Message *) (&sduP[length]);
  msg ("\n[RRC_UE][DECODE]Decoding from Srb2 -- Message type: %d\n", dl_dcch_msg->message.type);
    // TEMPComment - OPENAIR
  switch (dl_dcch_msg->message.type) {
    case DL_DCCH_radioBearerSetup:
      status = rrc_ue_msg_rbsetup (dl_dcch_msg);
      if (status == SUCCESS)
        rrc_ue_fsm_control (UE_RB_SETUP);
      break;
    case DL_DCCH_radioBearerRelease:
      status = rrc_ue_msg_rbrelease (dl_dcch_msg);
      if (status == SUCCESS)
        rrc_ue_fsm_control (UE_RB_RELEASE);
      break;
    case DL_DCCH_pagingType2:
      status = rrc_ue_msg_pagingt2 (dl_dcch_msg);
      if (status == SUCCESS)
        RRC_UE_O_NAS_NOTIFICATION_IND ();
      break;
    case DL_DCCH_measurementControl:
      status = rrc_ue_msg_measctl (dl_dcch_msg);
      if (status == SUCCESS) {
        #ifdef DEBUG_RRC_MEASURE_CONTROL
        rrc_ue_print_meas_control (&(protocol_ms->rrc.ue_meas_cmd[protocol_ms->rrc.rrc_ue_meas_to_activate]));
        #endif
        rrc_ue_trigger_measure ();
      }
      break;
    case DL_DCCH_ueCapabilityInformationConfirm:
      status = rrc_ue_msg_ueCapInfoCnf(dl_dcch_msg);
      if (status == SUCCESS)
        rrc_ue_fsm_control(UE_CAP_INFO_CNF);
      break;
    default:
      status = FAILURE;
  }
  if (status != SUCCESS)
    msg ("\n[RRC_UE][DECODE]Message from Srb2 could not be decoded. No call to FSM. %d \n", dl_dcch_msg->message.type);
}

//-----------------------------------------------------------------------------
// Generic function to decode RRC messages received on DCCH-AM
void rrc_ue_srb3_decode (char * sduP, int length){
//-----------------------------------------------------------------------------
  DL_DCCH_Message *dl_dcch_msg;
  int status = SUCCESS;

  #ifdef DEBUG_RRC_STATE
  // msg("[RRC_UE][DECODE]Decoding from Srb3 --%s-- \n", &sduP->data[length]);
  msg ("\n[RRC_UE][DECODE]Decoding from Srb3 -- start of message: \n");
  rrc_print_buffer (&sduP[length], 15);
  #endif

  dl_dcch_msg = (DL_DCCH_Message *) (&sduP[length]);
    // TEMPComment - OPENAIR
  switch (dl_dcch_msg->message.type) {
      case DL_DCCH_downlinkDirectTransfer:
        status = rrc_ue_msg_dlXfer (dl_dcch_msg);
        if (status == SUCCESS)
          RRC_UE_O_NAS_DATA_IND ();
        break;
      default:
        status = FAILURE;
  }
  if (status != SUCCESS)
    msg ("\n[RRC_UE][DECODE]Message from Srb3 could not be decoded. \n");
}

//-----------------------------------------------------------------------------
void rrc_ue_read_DCin_FIFO (struct nas_ue_dc_element *p, int count){
//-----------------------------------------------------------------------------
  //int count = 0;
  int maxlen = NAS_MAX_LENGTH;
  u8  rcve_buffer[maxlen];
  u16 data_length;
  int Message_Id;
  //struct nas_ue_dc_element *p;
  //int prim_length;
  //int prim_type;
  //int status;

//   memset (rcve_buffer, 0, maxlen);
//   if ((count = rtf_get (protocol_ms->rrc.rrc_ue_DCIn_fifo, rcve_buffer, NAS_TL_SIZE)) > 0) {
// 
//     #ifdef DEBUG_RRC_STATE
//      msg ("[RRC_UE] Message Received from NAS: -%hx- \n", rcve_buffer[0]);
//     #endif
//     p = (struct nas_ue_dc_element *) rcve_buffer;
//     prim_length = (int) (p->length);
//     prim_type = (int) (p->type);
//     #ifdef DEBUG_RRC_STATE
//      msg ("[RRC_UE] Primitive Type %d,\t Primitive length %d \n", prim_type, prim_length);
//     #endif
//     //get the rest of the primitive
//     count += rtf_get (protocol_ms->rrc.rrc_ue_DCIn_fifo, &(rcve_buffer[NAS_TL_SIZE]), prim_length - NAS_TL_SIZE);

    // Decode the primitive
//    switch (rcve_buffer[0]) {
    switch (p->type) {
      case CONN_ESTABLISH_REQ:
        protocol_ms->rrc.local_connection_ref = (int) (p->nasUEDCPrimitive.conn_establish_req.localConnectionRef);
        //Temp
        //protocol_ms->rrc.cell_id = (int)(p->nasUEDCPrimitive.conn_establish_req.cellId);
        #ifdef DEBUG_RRC_STATE
        msg ("[RRC_UE] CONN_ESTABLISH_REQ primitive length: %d\n", (int) (p->length));
        msg ("[RRC_UE] Local Connection reference: %d\n", (int) (protocol_ms->rrc.local_connection_ref));
        msg ("[RRC_UE] Requested Cell_id: %d\n", (int) (protocol_ms->rrc.cell_id));
        #endif
        rrc_ue_fsm_control (NAS_CONN_REQ);
        break;
      case CONN_RELEASE_REQ:
        protocol_ms->rrc.local_connection_ref = (int) (p->nasUEDCPrimitive.conn_release_req.localConnectionRef);
        protocol_ms->rrc.release_cause = p->nasUEDCPrimitive.conn_release_req.releaseCause;
        #ifdef DEBUG_RRC_STATE
        msg ("[RRC_UE] CONN_RELEASE_REQ primitive length: %d\n", (int) (p->length));
        msg ("[RRC_UE] Local Connection reference: %d\n", (int) (protocol_ms->rrc.local_connection_ref));
        msg ("[RRC_UE] Establishment status: %d\n", (int) (protocol_ms->rrc.release_cause));
        #endif
        rrc_ue_fsm_control (NAS_REL_REQ);
        break;
      case DATA_TRANSFER_REQ:
        if (protocol_ms->rrc.local_connection_ref == (int) (p->nasUEDCPrimitive.data_transfer_req.localConnectionRef)) {
          data_length = (u16) p->nasUEDCPrimitive.data_transfer_req.nasDataLength;
          protocol_ms->rrc.ul_nas_message_ptr = get_free_mem_block (data_length);
          protocol_ms->rrc.ul_nas_message_lgth = data_length;
          //get the associated data
          #ifndef RRC_NETLINK
          count += rtf_get (protocol_ms->rrc.rrc_ue_DCIn_fifo, (protocol_ms->rrc.ul_nas_message_ptr)->data, data_length);
          //memcpy((protocol_ms->rrc.ul_nas_message_ptr)->data,&(rcve_buffer[p->length]),data_length);
          #else
          count += rrc_ue_read_data_from_nlh ((char *)(protocol_ms->rrc.ul_nas_message_ptr)->data, data_length, (int) (p->length));
          #endif
          #ifdef DEBUG_RRC_STATE
          rrc_print_buffer (rcve_buffer, 100);
          msg ("[RRC_UE] DATA_TRANSFER_REQ primitive length: %d\n", (int) (p->length));
          msg ("[RRC_UE] Local Connection reference: %d\n", p->nasUEDCPrimitive.data_transfer_req.localConnectionRef);
          msg ("[RRC_UE] Priority (not used yet): %d\n", p->nasUEDCPrimitive.data_transfer_req.priority);
          msg ("[RRC_UE] Data length: %d\n", data_length);
          rrc_print_buffer ((char *) (protocol_ms->rrc.ul_nas_message_ptr)->data, data_length);
          #endif
          //send it only if connected mode
          if (protocol_ms->rrc.protocol_state & CONNECTED) {
            if (protocol_ms->rrc.idata_xfer) {
              rrc_ue_msg_initXfer (&Message_Id);
              protocol_ms->rrc.idata_xfer = FALSE;
            } else {
              rrc_ue_msg_ulXfer (&Message_Id);
            }
            RRC_UE_SEND_AM_SRB3 (Message_Id);
          } else {
            msg ("[RRC_UE_ERROR] DATA_TRANSFER_REQ primitive received in invalid state: %d\n", protocol_ms->rrc.protocol_state);
          }
          free_mem_block (protocol_ms->rrc.ul_nas_message_ptr);
        }
        break;
      default:
        msg ("[RRC_UE] Invalid message received\n");
        rrc_print_buffer (rcve_buffer, count);
        count = -1;
        break;
    }
  return count;

}

/***************************************************************************
                          rlc_am_reset.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
/* From 3GPP TS 25.322 V5.0.0 (2002-03)
The RLC reset procedure is used to reset two RLC peer entities, which are operating in acknowledged mode.
During the reset procedure the hyper frame numbers (HFN) in UTRAN and UE are synchronised. Two HFNs
used for ciphering needs to be synchronised, DL HFN in downlink and UL HFN in uplink.
 In the reset procedure, the highest UL HFN and DL HFN used by the RLC
entity in the transmitting sides, i.e. the HFNs associated with AMD PDUs of "Sequence Number"=VT(S)-1 if at least
one AMD PDU had been transmitted or of "Sequence Number"=0 if no AMD PDU had been transmitted, are
exchanged between UE and UTRAN.
The RESET PDUs and the RESET ACK PDUs have higher priority than AMD PDUs.
*/
#define RLC_AM_C
#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "rlc.h"
#include "rlc_am_entity.h"
#include "rlc_am_structs.h"
#include "rlc_am_constants.h"
#include "rlc_am_control_primitives_proto_extern.h"
#include "rlc_am_fsm_proto_extern.h"
#include "list.h"
#include "mem_block.h"
#include "umts_timer_proto_extern.h"
#include  "LAYER2/MAC/extern.h"
#define DEBUG_RESET
//-----------------------------------------------------------------------------
void            send_reset_ack_pdu (u8_t rsnP, struct rlc_am_entity *rlcP);
void            send_reset_pdu (struct rlc_am_entity *rlcP);
void            reset_rlc_am (struct rlc_am_entity *rlcP);
void            process_reset_ack (mem_block_t * pduP, struct rlc_am_reset_header *controlP, struct rlc_am_entity *rlcP);
void            process_reset (mem_block_t * pduP, struct rlc_am_reset_header *controlP, struct rlc_am_entity *rlcP);
//-----------------------------------------------------------------------------
void
rlc_am_reset_time_out (struct rlc_am_entity *rlcP, mem_block_t * not_usedP)
{
//-----------------------------------------------------------------------------
  /* from 3GPP TS 25.322 V5.0.0 (2002-03)
     If Timer_RST expires before the reset procedure is terminated, the Sender shall:
     - if VT(RST)<MaxRST-1:
     - set the RESET PDU as previously transmitted (even if additional SDUs were discarded in the mean-time);
     - transmit RESET PDU;
     - increment VT(RST) by one;
     - restart Timer_RST.
     Unrecoverable error (VT(RST)   MaxRST)
     The Sender shall:
     - if VT(RST) becomes larger than or equal to MaxRST:
     - indicate unrecoverable error to upper layer.
   */
  rlcP->timer_rst = NULL;
#ifdef DEBUG_RESET
  msg ("\n******************************************************************\n");
  msg ("[RLC_AM][RB %d]  RESET TIME OUT VT(RST) = %d  frame %d\n", rlcP->rb_id, rlcP->vt_rst, Mac_rlc_xface->frame);
  msg ("\n******************************************************************\n");
#endif
  if (rlcP->protocol_state & RLC_RESET_PENDING_STATE) {
    if (rlcP->vt_rst < rlcP->max_rst - 1) {
      send_reset_pdu (rlcP);
      rlcP->vt_rst += 1;
    } else {
      //rrc_indication_unrecoverable_error_from_rlc_am(rlcP);
      // notification not raised to RRC, continue. (TO DO)
      msg ("\n******************************************************************\n");
      msg ("[RLC_AM][RB %d]  RESET TIME OUT VT(RST) >= max_rst frame %d REPORT TO RRC\n", rlcP->rb_id, Mac_rlc_xface->frame);
      msg ("\n******************************************************************\n");
      send_reset_pdu (rlcP);

      rlcP->vt_rst += 1;
      rlc_data_conf (0, rlcP->rb_id, 0,RLC_TX_CONFIRM_FAILURE, rlcP->data_plane);    // mui, rb_ib, status
#ifdef NODE_MT
#warning  rrc_ue_CPHY_Out_Synch_rx should be called
      //rrc_ue_CPHY_Out_Synch_rx (1);     //  Temporary hack to disconnect mobile upon RLC AM timeout
#endif /* NODE_MT */
    }
  }
}

//-----------------------------------------------------------------------------
void
send_reset_pdu (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------
  mem_block_t      *pdu;
  struct rlc_am_reset_header *header;

  /* From TS 25.322 V5.0.0 (2002-03)
     RESET PDU contents to set
     The Sender shall:
     - set the HFNI field to the currently highest used HFN (DL HFN when the RESET PDU is sent by UTRAN or UL
     HFN when the RESET PDU is sent by the UE);
     - set the RSN field to the sequence number of the RESET PDU. The sequence number of the first RESET PDU
     after the AM entity is established or re-established shall be "0". This sequence number is incremented every time
     a new RESET PDU is transmitted, but not when a RESET PDU is retransmitted.
   */
  if (!(rlcP->timer_rst)) {

    if (rlcP->vt_rst == 0) {    // first transmission of reset PDU
      rlcP->reset_sequence_number ^= 1;
    }

    rlcP->vt_rst += 1;
    pdu = get_free_mem_block (rlcP->pdu_size + sizeof (struct rlc_am_tx_control_pdu_allocation) + GUARD_CRC_LIH_SIZE);
    if ((pdu)) {
      ((struct rlc_am_tx_control_pdu_management *) (pdu->data))->rlc_tb_type = RLC_AM_RESET_PDU_TYPE;
      header = (struct rlc_am_reset_header *) (&pdu->data[sizeof (struct rlc_am_tx_control_pdu_allocation)]);
      header->byte1 = RLC_PDU_TYPE_RESET | (rlcP->reset_sequence_number << 3);


#ifdef DEBUG_RESET
      msg ("\n******************************************************************\n");
      msg ("[RLC_AM][RB %d] TX RESET  RSN %d VT(RST) %d frame %d\n", rlcP->rb_id, rlcP->reset_sequence_number, rlcP->vt_rst, Mac_rlc_xface->frame);
      msg ("\n******************************************************************\n");
#endif
      // HFNI NOT IMPLEMENTED NOW needed only for cyphering
      /*header->hfni[0] = 0;
         header->hfni[1] = 0;
         header->hfni[2] = 0;
       */

      list_add_head (pdu, &rlcP->control);
      rlc_am_fsm_notify_event (rlcP, RLC_AM_TRANSMIT_RESET_EVENT);      // not yet submitted to lower layers
    } else {
      msg ("[RLC_AM][RB %d] ERROR SEND RESET OUT OF MEMORY ERROR\n", rlcP->rb_id);
    }
  }
}

//-----------------------------------------------------------------------------
void
send_reset_ack_pdu (u8_t rsnP, struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------
  mem_block_t      *pdu;
  struct rlc_am_reset_header *header;

  /* From 3GPP TS 25.322 V5.0.0 (2002-03)
     RESET ACK PDU contents to set
     The Receiver shall:
     - set the hyper frame number indicator field (HFNI) to the currently highest used HFN (DL HFN when the RESET
     ACK PDU is sent by UTRAN or UL HFN when the RESET ACK PDU is sent by the UE);
     - set the RSN field to the same value as in the corresponding received RESET PDU.
   */
  if ((pdu = get_free_mem_block (rlcP->pdu_size + sizeof (struct rlc_am_tx_control_pdu_allocation) + GUARD_CRC_LIH_SIZE))) {
#ifdef DEBUG_RESET
    msg ("\n******************************************************************\n");
    msg ("[RLC_AM][RB %d] TX RESET ACK RSN %d frame %d\n", rlcP->rb_id, rsnP, Mac_rlc_xface->frame);
    msg ("\n******************************************************************\n");
#endif
    ((struct rlc_am_tx_control_pdu_management *) (pdu->data))->rlc_tb_type = RLC_AM_RESET_ACK_PDU_TYPE;
    header = (struct rlc_am_reset_header *) (&pdu->data[sizeof (struct rlc_am_tx_control_pdu_allocation)]);
    header->byte1 = RLC_PDU_TYPE_RESET_ACK | (rsnP << 3);

    list_add_head (pdu, &rlcP->control);
  }
#ifdef DEBUG_RESET
  else {
    msg ("[RLC_AM][RB %d] SEND RESET ACK OUT OF MEMORY ERROR\n", rlcP->rb_id);
  }
#endif
}

//-----------------------------------------------------------------------------
void
process_reset_ack (mem_block_t * pduP, struct rlc_am_reset_header *controlP, struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------

  /* from 3GPP TS 25.322 V5.0.0 (2002-03)
     Upon reception of a RESET ACK PDU, the Sender shall:
     - if the Sender has already transmitted a RESET PDU which has not been yet acknowledged by a RESET ACK
     PDU:
     - if the received RSN value is the same as the one in the corresponding RESET PDU:
     - set the HFN value (DL HFN when the RESET ACK PDU is received in UE or UL HFN when the RESET
     ACK PDU is received in UTRAN) to the HFNI field of the received RESET ACK PDU;
     - reset the state variables described in subclause 9.4 to their initial values;
     - stop all the timers described in subclause 9.5;
     - reset configurable parameters to their configured values;
     - discard all RLC PDUs in the receiving side of the AM RLC entity;
     - discard all RLC SDUs that were transmitted before the reset in the transmitting side of the AM RLC
     entity;
     - increase with one the UL HFN and DL HFN, and the updated HFN values shall be used for the first
     transmitted and received AMD PDUs after the reset procedure;

     - otherwise (if the received RSN value is not the same as the one in the corresponding RESET PDU):
     - discard the RESET ACK PDU;

     - otherwise (if the Sender has not transmitted a RESET PDU which has not been yet acknowledged by a RESET
     ACK PDU):
     - discard the RESET ACK PDU.

     NOTE: If the TFC selection exchange has been initiated by sending the RLC Entity Info parameter to MAC, the
     RLC entity may delay the RLC SDUs discard in the transmitting side until the end of the next TTI.
   */
  if (rlc_am_fsm_notify_event (rlcP, RLC_AM_RECEIVE_RESET_ACK_EVENT)) {
    if (((controlP->byte1 & RLC_AM_RESET_SEQUENCE_NUMBER_MASK) >> 3) == rlcP->reset_sequence_number) {

#ifdef DEBUG_RESET
      msg ("\n******************************************************************\n");
      msg ("[RLC_AM][RB %d] RX RESET ACK RSN %d frame %d\n", rlcP->rb_id, ((controlP->byte1 & RLC_AM_RESET_SEQUENCE_NUMBER_MASK) >> 3), Mac_rlc_xface->frame);
      msg ("\n******************************************************************\n");
#endif
      rlc_am_reset_state_variables (rlcP);
      reset_rlc_am (rlcP);
      umts_stop_all_timers (&rlcP->rlc_am_timer_list);
      rlcP->timer_rst = NULL;
    }
#ifdef DEBUG_RESET
    else {
      msg ("\n******************************************************************\n");
      msg ("[RLC_AM][RB %d] RX RESET ACK WRONG RSN %d != %d frame %d\n", rlcP->rb_id, (controlP->byte1 & RLC_AM_RESET_SEQUENCE_NUMBER_MASK) >> 3, rlcP->reset_sequence_number, Mac_rlc_xface->frame);
     msg ("\n******************************************************************\n");
    }
#endif
  }
#ifdef DEBUG_RESET
  else {
    msg ("\n******************************************************************\n");
    msg ("[RLC_AM][RB %d] RX RESET ACK NOT EXPECTED frame %d\n", rlcP->rb_id, Mac_rlc_xface->frame);
    msg ("\n******************************************************************\n");
  }
#endif
  msg("RLC AM RESET EXIT\n");
}

//-----------------------------------------------------------------------------
void
process_reset (mem_block_t * pduP, struct rlc_am_reset_header *controlP, struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------
  u8_t              rsn;
  u8_t              saved_vt_rst;

/* From 25.322 V5.0.0 (2002-03)
   Reception of the RESET PDU by the Receiver
     Upon reception of a RESET PDU the Receiver shall:
     - if the RSN value in the RESET PDU is the same as the RSN value in the last received RESET PDU:
       - only submit a RESET ACK PDU to the lower layer with the contents set exactly as in the last transmitted
         RESET ACK PDU (i.e., in this case the RLC entity is not reset).

     - if the RESET PDU is the first RESET PDU received since the entity was (re-)established or the RSN value is
     different from the RSN value in the last received RESET PDU:
       - submit a RESET ACK PDU to the lower layer with the content set as specified in subclause 11.4.3.1;
       - reset the state variables described in subclause 9.4 except VT(RST) to their initial values;
       - stop all the timers described in subclause 9.5 except Timer_RST;
       - reset configurable parameters to their configured values;
       - discard all RLC PDUs in the receiving side of the AM RLC entity;
       - discard all RLC SDUs that were transmitted before the reset in the transmitting side of the AM RLC entity;
       - set the HFN (DL HFN when the RESET PDU is received in UE or UL HFN when the RESET PDU is
       received in UTRAN) equal to the HFNI field in the received RESET PDU;
       - increase with one the UL HFN and DL HFN, and the updated HFN values shall be used for the first
       transmitted and received AMD PDUs after the reset procedure.
       NOTE: If the TFC selection exchange has been initiated by sending the RLC Entity Info parameter to MAC, the
       RLC entity may delay the RLC SDUs discard in the transmitting side of the AM RLC entity until the end
       of the next TTI.

   Reception of the RESET PDU by the Sender
     Upon reception of a RESET PDU, the Sender shall:
       - submit a RESET ACK PDU to the lower layer with the content set as specified in subclause 11.4.3.1;
       - reset the state variables described in subclause 9.4 except VT(RST) to their initial values;
       - stop all the timers described in subclause 9.5 except Timer_RST;
       - reset configurable parameters to their configured values;
       - discard all RLC PDUs in the receiving side of the AM RLC entity;
       - discard all RLC SDUs that were transmitted before the reset in the transmitting side of the AM RLC entity;
       - set the HFN (DL HFN when the RESET PDU is received in UE or UL HFN when the RESET PDU is received
       in UTRAN) equal to the HFNI field in the received RESET PDU;
       - increase with one the UL HFN and DL HFN, and the updated HFN values shall be used for the first transmitted
       and received AMD PDUs after the reset procedure.
       NOTE: If the TFC selection exchange has been initiated by sending the RLC Entity Info parameter to MAC, the
       RLC entity may delay the RLC SDUs discard in the transmitting side until the end of the next TTI.

*/
  rsn = (controlP->byte1 & RLC_AM_RESET_SEQUENCE_NUMBER_MASK) >> 3;

  if ((rlcP->protocol_state & RLC_RESET_PENDING_STATE)) {
#ifdef DEBUG_RESET
    msg ("\n******************************************************************\n");
    msg ("[RLC_AM][RB %d] RX RESET RSN %d frame %d\n", rlcP->rb_id, rsn, Mac_rlc_xface->frame);
    msg ("\n******************************************************************\n");
#endif
    rlc_am_fsm_notify_event (rlcP, RLC_AM_RECEIVE_RESET_EVENT);
    // sender
    saved_vt_rst = rlcP->vt_rst;
    reset_rlc_am (rlcP);
    rlcP->vt_rst = saved_vt_rst;
    rlcP->last_received_rsn = rsn;
    send_reset_ack_pdu (rsn, rlcP);
    // NOT IN SPECS, BUT IF SENDER RECEIVE A RESET, IT DISCARDS ALL CONTROL PDU IN ITS BUFFER
    // SO IF THE RESET PDU WAS STILL IN THE RLC BUFFERS...IT IS DROPPED AND THE RESET TIMER CAN NOT
    // BE STARTED (BY SUCCESSFULL TX ACK OF MAC LAYER), SO REARM THE TIMER AND SCHEDULE A NEW RESET
    // PDU FOR TRANSMISSION
    if (!(rlcP->timer_rst)) {
      umts_stop_all_timers (&rlcP->rlc_am_timer_list);
      rlc_am_reset_time_out (rlcP, NULL);
    } else {
      umts_stop_all_timers_except (&rlcP->rlc_am_timer_list, rlc_am_reset_time_out);
    }
  } else {
    // receiver
#ifdef DEBUG_RESET
    msg ("\n******************************************************************\n");
    msg ("[RLC_AM][RB %d] RX RESET RSN %d frame %d\n", rlcP->rb_id, rsn, Mac_rlc_xface->frame);
    msg ("\n******************************************************************\n");
#endif
    if (rlcP->last_received_rsn == rsn) {       //last_received_rsn initialized to -1 when RLC start
#ifdef DEBUG_RESET
      msg ("\n******************************************************************\n");
      msg ("[RLC_AM][RB %d] RX RESET RSN %d SAME RSN RECEIVED LAST RESET, SEND RESET ACK frame %d \n", rlcP->rb_id, rsn, Mac_rlc_xface->frame);
     msg ("\n******************************************************************\n");
#endif
      send_reset_ack_pdu (rsn, rlcP);   // rsn start to zero
      return;
    }
    rlc_am_fsm_notify_event (rlcP, RLC_AM_RECEIVE_RESET_EVENT);
    saved_vt_rst = rlcP->vt_rst;
    reset_rlc_am (rlcP);
    rlcP->vt_rst = saved_vt_rst;
    rlcP->last_received_rsn = rsn;
    send_reset_ack_pdu (rsn, rlcP);
    umts_stop_all_timers (&rlcP->rlc_am_timer_list);
    rlcP->timer_rst = NULL;
  }
}

//-----------------------------------------------------------------------------
void
reset_rlc_am (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------
  int             index;

  for (index = 0; index < rlcP->size_input_sdus_buffer; index++) {
    if (rlcP->input_sdus[index]) {

      if (!(rlcP->data_plane)) {
#ifdef DEBUG_RLC_AM_SEND_CONFIRM
        msg ("[RLC_AM][RB %d][CONFIRM] SDU MUI %d LOST IN RESET\n", rlcP->rb_id, ((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[index]->data))->mui);
#endif
        rlc_data_conf (0, rlcP->rb_id, ((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[index]->data))->mui, RLC_TX_CONFIRM_FAILURE, rlcP->data_plane);
      }
      free_mem_block (rlcP->input_sdus[index]);
      rlcP->input_sdus[index] = NULL;
    }
  }
  rlc_am_reset_state_variables (rlcP);
  rlc_am_discard_all_pdus (rlcP);
#ifdef DEBUG_RESET
  msg ("[RLC_AM][RB %d] RESETED\n", rlcP->rb_id);
  //display_mem_load();
  //msg("sdu_conf_segmented : %p\n"   , rlcP->sdu_conf_segmented.head);
  //msg("sdu_discard_segmented : %p\n", rlcP->sdu_discard_segmented.head);
  //msg("sdu_discarded : %p\n"        , rlcP->sdu_discarded.head);
  //msg("discard_procedures : %p\n"   , rlcP->discard_procedures.head);
  //check_mem_area(mem);
#endif
}

/***************************************************************************
                          rlc_am_discard_rx.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "rlc_am_receiver_proto_extern.h"
#include "rlc_primitives.h"
#include "rlc_am_status_proto_extern.h"
#include "rlc_am_receiver_proto_extern.h"
#include "rlc_am_discard_tx_proto_extern.h"
#include "rlc_am_discard_notif_proto_extern.h"
#include "rlc_am_retrans_proto_extern.h"
#include "rlc_am_util_proto_extern.h"
#include "rlc_am_proto_extern.h"
#include "umts_timer_struct.h"
#include "umts_timer_proto_extern.h"
#include "mem_block.h"
//-----------------------------------------------------------------------------
void            rlc_am_received_sufi_ack_check_discard_procedures (struct rlc_am_entity *rlcP);
void            rlc_am_free_discard_procedure (mem_block_t * mb_current_procedureP);
inline void     rlc_am_discard_free_receiver_buffer (struct rlc_am_entity *rlcP, u16_t sn_mrw_iP, u8_t nlengthP);
u8_t             *retransmission_buffer_management_mrw (struct rlc_am_entity *rlcP, u8 * byte1P, u8 * byte_alignedP);
u8_t             *retransmission_buffer_management_mrw_ack (struct rlc_am_entity *rlcP, u8 * byte1P, u8 * byte_alignedP);
//-----------------------------------------------------------------------------
void
rlc_am_received_sufi_ack_check_discard_procedures (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------
  mem_block_t      *mb_proc;
  mem_block_t      *tmp_proc;
  struct rlc_am_discard_procedure *procedure;
  /* From 3GPP 25.322 V5.0.0 (2002-03)
     The Sender shall terminate the SDU discard with explicit signalling procedure if one of the
     following criteria is fulfilled:

     -      a STATUS PDU/piggybacked STATUS PDU containing an MRW_ACK SUFI is received, and the SN_ACK      : DONE in retransmission_buffer_management_mrw_ack(..)
     field in the received MRW_ACK SUFI > the SN_MRWLENGTH field in the transmitted MRW_SUFI,
     and the N field in the received MRW_ACK SUFI is set equal to "0000";

     -      a STATUS PDU/piggybacked STATUS PDU containing an MRW_ACK SUFI is received, and the SN_ACK      : DONE in retransmission_buffer_management_mrw_ack(..)
     field in the received MRW_ACK SUFI = the SN_MRWLENGTH field in the transmitted MRW_SUFI,
     and the N field in the received MRW_ACK SUFI is set equal to the NLENGTH field in the
     transmitted MRW SUFI;

     -      a STATUS PDU/piggybacked STATUS PDU containing an ACK SUFI is received, and the LSN field       : DONE HERE
     in the received ACK SUFI > the SN_MRWLENGTH field in the transmitted MRW SUFI.

     Upon termination of the SDU discard with explicit signalling procedure, the Sender shall:
     -      stop the timer Timer_MRW;
     -      update VT(A) and VT(MS) according to the received STATUS PDU/piggybacked STATUS PDU;
     The Sender shall not confirm to upper layers the SDUs that are requested to be discarded.
   */


  mb_proc = rlcP->discard_procedures.head;

  while ((mb_proc)) {
    procedure = (struct rlc_am_discard_procedure *) (mb_proc->data);
    if (rlc_am_comp_sn (rlcP, rlcP->vt_a, rlcP->vt_a, procedure->last_pdu_sn) > 0) {
#ifdef DEBUG_RLC_AM_DISCARD
      msg ("[RLC_AM][RB %d] DISCARD  TRANSMIT  NEW VT(A) 0x%04X REMOVE PROCEDURE SN_MRWlength 0x%04X\n", rlcP->rb_id, rlcP->vt_a, procedure->last_pdu_sn);
      msg ("[RLC_AM][RB %d] DISCARD  TRANSMIT ACK\n", rlcP->rb_id);
#endif

      tmp_proc = mb_proc->next;
      list2_remove_element (mb_proc, &rlcP->discard_procedures);
      // if procedure is running, free timer resources
      if ((procedure->running) && (rlcP->timer_mrw)) {
        if (rlcP->timer_mrw == list2_remove_element (rlcP->timer_mrw, &rlcP->rlc_am_timer_list)) {
#ifdef DEBUG_RLC_AM_DISCARD
          msg ("[RLC_AM][RB %d] DISCARD  TRANSMIT ACK->TERMINATE PROCEDURE: REMOVE TIMER mrw id %p\n", rlcP->rb_id, ((struct timer_unit *) (rlcP->timer_mrw->data))->timer_id);
#endif
          free_mem_block (rlcP->timer_mrw);
          rlcP->timer_mrw = NULL;
        } else {
#ifdef DEBUG_RLC_AM_DISCARD
          msg ("[RLC_AM][RB %d] DISCARD  TRANSMIT REMOVE TIMER FAILED\n", rlcP->rb_id);
#endif
        }
      }
      rlc_am_free_discard_procedure (mb_proc);

      mb_proc = tmp_proc;
      rlcP->vt_mrw = 0;
    } else {
      mb_proc = mb_proc->next;
    }
  }
  // may be other discard procedures to run
  rlc_am_schedule_procedure (rlcP);
}

//-----------------------------------------------------------------------------
void
rlc_am_free_discard_procedure (mem_block_t * mb_current_procedureP)
{
//-----------------------------------------------------------------------------
  // be carefull : the timer is not free here, it should be done before
  struct rlc_am_discard_procedure *procedure;

  if (mb_current_procedureP) {
    procedure = (struct rlc_am_discard_procedure *) (mb_current_procedureP->data);
    list_free (&procedure->sdu_list);
    if (procedure->control_pdu) {
      free_mem_block (procedure->control_pdu);
    }
    free_mem_block (mb_current_procedureP);
  }
}

//-----------------------------------------------------------------------------
inline void
rlc_am_discard_free_receiver_buffer (struct rlc_am_entity *rlcP, u16_t sn_mrw_iP, u8_t nlengthP)
{
//-----------------------------------------------------------------------------
  u16_t             working_sn;
  u16_t             working_sn_index;     // index in buffer

  // should start reassembly with sn working_sn
  working_sn = rlcP->last_reassemblied_sn;
  working_sn_index = working_sn % rlcP->recomputed_configured_rx_window_size;

#ifdef DEBUG_RLC_AM_DISCARD
  display_receiver_buffer (rlcP);
#endif
  while (working_sn != sn_mrw_iP) {

#ifdef DEBUG_RLC_AM_DISCARD
    msg ("[RLC_AM][RB %d] DISCARD  RECEIVER   FREE RECEIVER BUFFER pdu sn 0x%04X VR(R) 0x%04X\n", rlcP->rb_id, working_sn, rlcP->vr_r);
#endif
    free_receiver_buffer (rlcP, working_sn_index);
    rlcP->last_reassemblied_sn = working_sn;
    working_sn = (working_sn + 1) & SN_12BITS_MASK;
    working_sn_index = working_sn % rlcP->recomputed_configured_rx_window_size;
  }

  // From 3GPP TS 25.322 V5.0.0 (2002-03) page 35
  // NLENGTH is used together with SN_MRWLENGTH to indicate the end of the last SDU to be discarded in the receiver.
  // NLENGTH indicates which LI in the PDU with sequence number SN_MRWLENGTH corresponds to the last SDU to be discarded in the receiver.
  // NLENGTH = 0 indicates that the last SDU ended in the PDU with sequence number SN_MRWLENGTH -1 and that the first data octet
  // in the PDU with sequence number SN_MRWLENGTH is the first data octet to be reassembled next.

  // erase previous sdu in construction
  rlcP->output_sdu_size_to_write = 0;
  if ((nlengthP)) {
    rlcP->discard_reassembly_after_li = nlengthP;       // will be used by process_receiver_buffer
  } else {
#ifdef DEBUG_RLC_AM_DISCARD
    msg ("[RLC_AM][RB %d] DISCARD  RECEIVER  rlc_am_discard_free_receiver_buffer pdu sn 0x%04X (nlength=0)\n", rlcP->rb_id, working_sn);
#endif
    free_receiver_buffer (rlcP, working_sn_index);
    rlcP->discard_reassembly_after_li = RLC_AM_DISCARD_REASSEMBLY_AT_LI_INDEX_0;
    rlcP->last_reassemblied_sn = working_sn;
  }
}

//-----------------------------------------------------------------------------
u8_t             *
retransmission_buffer_management_mrw (struct rlc_am_entity *rlcP, u8_t * byte1P, u8 * byte_alignedP)
{
//-----------------------------------------------------------------------------
  mem_block_t      *mb;
  u8_t             *p8;
  u16_t             sn_mrw_i[15];
  u16_t             new_vr_r;
  u16_t             working_sn;
  u16_t             working_sn_index;     // index in buffer
  u8_t              sn_mrw_i_index;
  u8_t              mrw_length;
  u8_t              nlength;
#ifdef DEBUG_RLC_AM_DISCARD
  u8_t              i;
#endif
  /* From 3GPP TS 25.322 V5.0.0 (2002-03)
     Upon reception of the STATUS PDU/piggybacked STATUS PDU containing an MRW SUFI, the Receiver shall:
     -  if the LENGTH field in the received MRW SUFI is "0000":
     -  consider SN_MRW1 to be above or equal to VR(R).
     -  otherwise:
     -  consider SN_MRW1 to be less than VR(MR);
     -  consider all the SN_MRWis other than SN_MRW1 to be in sequential order within the list and
     sequentially above or equal to SN_MRWi-1.
     -  discard AMD PDUs up to and including the PDU with sequence number SN_MRWLENGTH-1;
     -  if the NLENGTH field in the received MRW SUFI is "0000":
     -  reassemble from the first data octet of the AMD PDU with sequence number SN_MRWLENGTH
     after the discard.
     -  otherwise:
     -  discard further the data octets in the AMD PDU with sequence number SN_MRWLENGTH up to and
     including the octet indicated by the NLENGTH:th LI field of the PDU with sequence number
     SN_MRWLENGTH;
     -  reassemble from the succeeding data octet in the AMD PDU with sequence number SN_MRWLENGTH
     after the discard;
     -  if "Send MRW" is configured:
     -  inform upper layers about all of the discarded SDUs that were not previously delivered to
     upper layer or discarded by other MRW SUFIs;
     -  update the state variables VR(R), VR(H) and VR(MR) according to the received
     STATUS PDU/piggybacked STATUS PDU;
     -  assemble a MRW_ACK SUFI
     - schedule and submit to lower layer a STATUS PDU/piggybacked STATUS PDU containing the MRW_ACK
     SUFI.

     The Receiver shall:
     -  set the SN_ACK field in the MRW_ACK SUFI to the new value of VR(R), updated after reception
     of the MRW SUFI;
     -  if the SN_ACK field in the MRW_ACK SUFI is set equal to the SN_MRWLENGTH field in the
     received MRW SUFI:
     -  set the N field in the MRW_ACK SUFI to the NLENGTH field in the received MRW SUFI.
     -  otherwise:
     -  set the N field in the MRW_ACK SUFI to "0000";
     -  include the MRW_ACK SUFI in the next STATUS PDU/piggybacked STATUS PDU to be transmitted,
     according to subclause 11.5.2.
   */

  p8 = byte1P;
  sn_mrw_i_index = 0;

  if (*byte_alignedP) {
    mrw_length = *p8++ & 0X0F;  // number of SN_MRWi
  } else {
    p8 = p8 + 1;
    mrw_length = (*p8 & 0XF0) >> 4;     // number of SN_MRWi
  }

  if (!(mrw_length)) {
    if (*byte_alignedP) {

      // sn_mrw_i is SN_MRW length
      sn_mrw_i[0] = (*p8++) << 4;
      sn_mrw_i[0] += (*p8) >> 4;
      *byte_alignedP = 0;
    } else {
      // sn_mrw_i is SN_MRW length
      sn_mrw_i[0] = (*p8++) << 8;
      sn_mrw_i[0] += *p8++;
      nlength = (*p8 & 0xF0) >> 4;
      *byte_alignedP = 1;
    }
    sn_mrw_i_index += 1;
  } else {
    // get all sn_mrw_i
    while (mrw_length != sn_mrw_i_index) {
      if (*byte_alignedP) {
        sn_mrw_i[sn_mrw_i_index] = (*p8++) << 4;
        sn_mrw_i[sn_mrw_i_index] += (*p8) >> 4;
        *byte_alignedP = 0;
      } else {
        sn_mrw_i[sn_mrw_i_index] = ((*p8++) & 0x0F) << 8;
        sn_mrw_i[sn_mrw_i_index] += (*p8++);
        *byte_alignedP = 1;
      }
      sn_mrw_i_index += 1;
    }
  }
  // get nlength
  if (*byte_alignedP) {
    nlength = (*p8 & 0xF0) >> 4;
    *byte_alignedP = 0;
  } else {
    nlength = *p8++ & 0x0F;
    *byte_alignedP = 1;
  }
#ifdef DEBUG_RLC_AM_DISCARD
  msg ("[RLC_AM][RB %d] DISCARD  RECEIVER RECEIVED MRW LENGTH %d ", rlcP->rb_id, mrw_length);
  for (i = 0; i < sn_mrw_i_index; i++) {
    msg ("sn_mrw%d 0x%04X ", i + 1, sn_mrw_i[i]);
  }
  msg ("Nlength %d\n", nlength);
#endif

  /********************************************************************
  *                   M R W     D I S C A R D E D                     *
  ********************************************************************/
  /* From 3GPP TS 25.322 V5.0.0
     Reception of obsolete/corrupted MRW SUFI by the Receiver
     If the received MRW SUFI contains outdated information about the receiving window
     (receiving window already moved further than MRW SUFI is indicating), the Receiver shall:
     -  discard the MRW SUFI;
     -  set the SN_ACK field in the MRW_ACK SUFI to the current value of VR(R);
     -  set the N field in the MRW_ACK SUFI to "0000";
     -  include the MRW_ACK SUFI in the next STATUS PDU/piggybacked STATUS PDU to be transmitted.
   */
  if (rlc_am_comp_sn (rlcP, rlcP->vr_r, sn_mrw_i[sn_mrw_i_index - 1], rlcP->vr_r) < 0) {
#ifdef DEBUG_RLC_AM_DISCARD
    msg ("[RLC_AM][RB %d] DISCARD  RECEIVER ERROR received OBSOLETE/CORRUPTED MRW command  VR(R) 0x%04X VR(H) 0x%04X VR(MR) 0x%04X SN_MRWlength 0x%04X\n",
         rlcP->rb_id, rlcP->vr_r, rlcP->vr_h, rlcP->vr_mr, sn_mrw_i[sn_mrw_i_index - 1]);
#endif
    // send status pdu mrw_ack
    if ((mb = rlc_am_create_status_pdu_mrw_ack (rlcP, 0, rlcP->vr_r))) {
      list_add_tail_eurecom (mb, &rlcP->control);
    }
    return p8;

  } else {
    /********************************************************************
    *                   M R W   N O T   D I S C A R D E D               *
    ********************************************************************/
    new_vr_r = sn_mrw_i[sn_mrw_i_index - 1];
    /***********************************
    *    DISCARD PDUS IN THE RECEIVER  *
    ***********************************/
    // should start discard with sn working_sn
    working_sn = rlcP->last_reassemblied_sn;
    working_sn_index = working_sn % rlcP->recomputed_configured_rx_window_size;
    // discard all pdus except the last one
#ifdef DEBUG_RLC_AM_DISCARD
    display_receiver_buffer (rlcP);
#endif
    while (working_sn != new_vr_r) {

#ifdef DEBUG_RLC_AM_DISCARD
      msg ("[RLC_AM][RB %d] DISCARD  RECEIVER FREE RECEIVER BUFFER pdu sn 0x%04X VR(R) 0x%04X\n", rlcP->rb_id, working_sn, rlcP->vr_r);
#endif
      free_receiver_buffer (rlcP, working_sn_index);
      rlcP->last_reassemblied_sn = working_sn;
      working_sn = (working_sn + 1) & SN_12BITS_MASK;
      working_sn_index = working_sn % rlcP->recomputed_configured_rx_window_size;
    }
    // From 3GPP TS 25.322 V5.0.0 (2002-03) page 35
    // NLENGTH is used together with SN_MRWLENGTH to indicate the end of the last SDU to be discarded in the receiver.
    // NLENGTH indicates which LI in the PDU with sequence number SN_MRWLENGTH corresponds to the last SDU to be discarded in the receiver.
    // NLENGTH = 0 indicates that the last SDU ended in the PDU with sequence number SN_MRWLENGTH -1 and that the first data octet
    // in the PDU with sequence number SN_MRWLENGTH is the first data octet to be reassembled next.

    // erase previous sdu in construction
    rlcP->output_sdu_size_to_write = 0;

    rlcP->discard_reassembly_start_sn = new_vr_r;
    rlcP->discard_reassembly_after_li = nlength;        // will be used by process_receiver_buffer

    if (rlc_am_comp_sn (rlcP, rlcP->vr_r, new_vr_r, rlcP->vr_r) > 0) {
#ifdef DEBUG_RLC_AM_DISCARD
      msg ("[RLC_AM][RB %d] DISCARD  RECEIVER... VR(R) : 0x%04X -> 0x%04X  (VR(H)=0x%04X)\n", rlcP->rb_id, rlcP->vr_r, new_vr_r, rlcP->vr_h);
#endif
      if (rlc_am_comp_sn (rlcP, rlcP->vr_r, new_vr_r, rlcP->vr_h) > 0) {
#ifdef DEBUG_RLC_AM_DISCARD
        msg ("[RLC_AM][RB %d] DISCARD  RECEIVER... VR(H) : 0x%04X -> 0x%04X\n", rlcP->rb_id, rlcP->vr_h, new_vr_r);
#endif
        rlcP->vr_h = new_vr_r;
      }
      rlcP->vr_r = new_vr_r;
      rlcP->ack.vr_r_modified = 1;      // for status generation
      rlcP->vr_mr = (rlcP->vr_r + rlcP->configured_rx_window_size - 1) & SN_12BITS_MASK;
    }

    if ((rlcP->send_mrw & RLC_AM_SEND_MRW_ON)) {
      //send tu upper layers discarded sdus infos !!
      // TO DO !!!!!!!!
    }

    if (rlcP->pdu_size <= 126) {
      process_receiver_buffer_7 (rlcP);
    } else {
      process_receiver_buffer_15 (rlcP);
    }
    /* From 3GPP 25.322 V5.0.0 (2002-03)
       The Receiver shall:
       -        set the SN_ACK field in the MRW_ACK SUFI to the new value of VR(R), updated after reception
       of the MRW SUFI;
       -        if the SN_ACK field in the MRW_ACK SUFI is set equal to the SN_MRWLENGTH field in the
       received MRW SUFI:
       -        set the N field in the MRW_ACK SUFI to the NLENGTH field in the received MRW SUFI.
       -        otherwise:
       -        set the N field in the MRW_ACK SUFI to "0000";
       -        include the MRW_ACK SUFI in the next STATUS PDU/piggybacked STATUS PDU to be transmitted,
       according to subclause 11.5.2. */

    if (sn_mrw_i[sn_mrw_i_index - 1] == rlcP->vr_r) {
      mb = rlc_am_create_status_pdu_mrw_ack (rlcP, nlength, rlcP->vr_r);
    } else {
      mb = rlc_am_create_status_pdu_mrw_ack (rlcP, 0, rlcP->vr_r);
    }
    /*   SEND STATUS PDU MRW ACK   */
    if (mb) {
      list_add_tail_eurecom (mb, &rlcP->control);
    }

    return p8;
  }
}

//-----------------------------------------------------------------------------
u8_t             *
retransmission_buffer_management_mrw_ack (struct rlc_am_entity * rlcP, u8_t * byte1P, u8 * byte_alignedP)
{
//-----------------------------------------------------------------------------
  mem_block_t      *mb;
  mem_block_t      *mb_current_procedure;
  struct rlc_am_tx_data_pdu_management *rlc_header;
  struct rlc_am_discard_procedure *procedure;
  u8_t             *p8;

  u16_t             sn_ack;
  u16_t             index;
  u8_t              n;            // field of mrw_ack sufi

  //-------------------------------------
  // DECODE SUFI MRW_ACK
  //-------------------------------------
  p8 = byte1P;

  if (*byte_alignedP) {
    n = *p8++ & 0X0F;
    // sn_mrw_i is SN_MRW length
    sn_ack = (*p8++) << 4;
    sn_ack += (*p8) >> 4;
    *byte_alignedP = 0;
  } else {
    p8 = p8 + 1;
    n = (*p8 & 0XF0) >> 4;
    // sn_mrw_i is SN_MRW length
    sn_ack = ((*p8++) & 0x0F) << 8;
    sn_ack += *p8++;
    *byte_alignedP = 1;
  }

#ifdef DEBUG_RLC_AM_DISCARD
  msg ("\n[RLC_AM][RB %d] DISCARD  TRANSMIT RX MRW_ACK N %d SN_ACK 0x%04X VT(A) 0x%04X VT(S) 0x%04X\n", rlcP->rb_id, n, sn_ack, rlcP->vt_a, rlcP->vt_s);
  //display_retransmission_buffer(rlcP);
#endif

  // compare to current running procedure
  if ((mb_current_procedure = rlcP->discard_procedures.head)) {

    procedure = (struct rlc_am_discard_procedure *) (mb_current_procedure->data);

#ifdef DEBUG_RLC_AM_DISCARD
    msg ("[RLC_AM][RB %d] DISCARD  TRANSMIT ... procedure->last_pdu_sn 0x%04X procedure->nlength 0x%04X \n", rlcP->rb_id, procedure->last_pdu_sn, procedure->nlength);
#endif
    /* From 3GPP TS 25.322 V5.0.0
       The Sender shall discard the received MRW_ACK SUFI if one of the following cases occurs:

       -        the timer Timer_MRW is not active;
       or
       -        the SN_ACK field in the received MRW_ACK SUFI < the SN_MRWLENGTH field in the transmitted MRW SUFI;
       or
       -        the SN_ACK field in the received MRW_ACK SUFI = the SN_MRWLENGTH field in the transmitted MRW SUFI,
       and the N field in the received MRW_ACK SUFI is not equal to the NLENGTH field in the transmitted MRW SUFI;
       or
       -        the SN_ACK field in the received MRW_ACK SUFI > the SN_MRWLENGTH field in the transmitted MRW SUFI,
       and the N field in the received MRW_ACK SUFI is not equal to "0000".
     */
    if ((rlcP->timer_mrw == NULL) ||    // if timer_MRW is not active
        (rlc_am_comp_sn (rlcP, rlcP->vt_a, procedure->last_pdu_sn, sn_ack) > 0) ||
        ((rlc_am_comp_sn (rlcP, rlcP->vt_a, procedure->last_pdu_sn, sn_ack) == 0) && (n != procedure->nlength)) ||
        ((rlc_am_comp_sn (rlcP, rlcP->vt_a, procedure->last_pdu_sn, sn_ack) < 0) && (n != 0))) {

#ifdef DEBUG_RLC_AM_DISCARD
      msg ("[RLC_AM][RB %d] DISCARD  TRANSMIT  ERROR  RX OBSOLETE MRW_ACK   VT(A) 0x%04X  VT(S) 0x%04X SN_MRWlength 0x%04X  N %d SN_ACK 0x%04X\n",
           rlcP->rb_id, rlcP->vt_a, rlcP->vt_s, procedure->last_pdu_sn, n, sn_ack);
#endif
    } else if (
                /* From 3GPP 25.322 V5.0.0 (2002-03)
                   The Sender shall terminate the SDU discard with explicit signalling procedure if one of the
                   following criteria is fulfilled:

                   -      a STATUS PDU/piggybacked STATUS PDU containing an MRW_ACK SUFI is received, and the SN_ACK      : DONE HERE
                   field in the received MRW_ACK SUFI > the SN_MRWLENGTH field in the transmitted MRW_SUFI,
                   and the N field in the received MRW_ACK SUFI is set equal to "0000";

                   -      a STATUS PDU/piggybacked STATUS PDU containing an MRW_ACK SUFI is received, and the SN_ACK      : DONE HERE
                   field in the received MRW_ACK SUFI = the SN_MRWLENGTH field in the transmitted MRW_SUFI,
                   and the N field in the received MRW_ACK SUFI is set equal to the NLENGTH field in the
                   transmitted MRW SUFI;

                   -      a STATUS PDU/piggybacked STATUS PDU containing an ACK SUFI is received, and the LSN field       : DONE in rlc_am_received_sufi_ack_check_discard_procedures(...)
                   in the received ACK SUFI > the SN_MRWLENGTH field in the transmitted MRW SUFI.

                   Upon termination of the SDU discard with explicit signalling procedure, the Sender shall:
                   -      stop the timer Timer_MRW;
                   -      update VT(A) and VT(MS) according to the received STATUS PDU/piggybacked STATUS PDU;
                   The Sender shall not confirm to upper layers the SDUs that are requested to be discarded.
                 */
                (rlc_am_comp_sn (rlcP, rlcP->vt_a, sn_ack, procedure->last_pdu_sn) > 0) || ((sn_ack == procedure->last_pdu_sn) && (n == procedure->nlength))
      ) {
#ifdef DEBUG_RLC_AM_DISCARD
      msg ("[RLC_AM][RB %d] DISCARD PROCEDURE %p TERMINATED\n", rlcP->rb_id, mb_current_procedure);
#endif

      rlcP->vt_mrw = 0;
      // remove timer
      if ((rlcP->timer_mrw)) {
        if (rlcP->timer_mrw == list2_remove_element (rlcP->timer_mrw, &rlcP->rlc_am_timer_list)) {
#ifdef DEBUG_RLC_AM_DISCARD
          msg ("[RLC_AM][RB %d] DISCARD  TRANSMIT  TERMINATE PROCEDURE: REMOVE TIMER mrw id %p\n", rlcP->rb_id, ((struct timer_unit *) (rlcP->timer_mrw->data))->timer_id);
#endif
          free_mem_block (rlcP->timer_mrw);
          rlcP->timer_mrw = NULL;
        }
#ifdef DEBUG_RLC_AM_DISCARD
        else {
          msg ("[RLC_AM][RB %d] DISCARD  TRANSMIT  ERROR TERMINATION PROCEDURE : TIMER MRW IS WRONG\n", rlcP->rb_id);   // in fact : no, the lists are not protected against an element that is not inside the list
        }
#endif
      }
#ifdef DEBUG_RLC_AM_DISCARD
      else {
        msg ("[RLC_AM][RB %d] DISCARD  TRANSMIT  ERROR TERMINATION PROCEDURE : NO TIMER MRW WAS FOUND\n", rlcP->rb_id);
      }
#endif

      mb_current_procedure = list2_remove_head (&rlcP->discard_procedures);

      //update VT(A)
      if (rlc_am_comp_sn (rlcP, rlcP->vt_a, sn_ack, rlcP->vt_a) > 0) {

        // free resources that may be in retransmission buffer
        while (rlcP->vt_a != sn_ack) {

          index = rlcP->vt_a % rlcP->recomputed_configured_tx_window_size;
          if ((mb = rlcP->retransmission_buffer[index])) {

            rlc_header = (struct rlc_am_tx_data_pdu_management *) (mb->data);
            if (rlc_header->nb_sdu > 0) {

#ifdef DEBUG_RLC_AM_DISCARD
              msg ("[RLC_AM][RB %d] DISCARD  TRANSMIT  CASE 1 MRW_ACK\n", rlcP->rb_id);
#endif
              // the sender of MRW_ACK changed sn_mrw_length to vr_r, so it is normal to free
              // pdus that are linked with sdus
              free_retransmission_buffer_no_confirmation (rlcP, index);

            } else {
#ifdef DEBUG_RLC_AM_DISCARD
              msg ("[RLC_AM][RB %d] DISCARD  TRANSMIT  CASE 2 MRW_ACK FREE REMAINING RETRANS PDU 0x%04X\n", rlcP->rb_id, rlcP->vt_a);
#endif
              // if this pdu has been retransmitted, remove its size from buffer occupancy
              if (rlc_header->vt_dat > 0) {
                rlcP->buffer_occupancy_retransmission_buffer -= 1;
              }
              free_mem_block (mb);
              rlcP->retransmission_buffer[index] = NULL;
            }
          }
          rlcP->vt_a = (rlcP->vt_a + 1) & SN_12BITS_MASK;
        }
#ifdef DEBUG_RLC_AM_DISCARD
        msg ("[RLC_AM][RB %d] DISCARD  TRANSMIT  MRW_ACK  VT(A) UPDATED 0x%04X  VT(S) 0x%04X\n", rlcP->rb_id, rlcP->vt_a, rlcP->vt_s);
#endif
      }
      rlc_am_free_discard_procedure (mb_current_procedure);
      rlc_am_schedule_procedure (rlcP);
    } else {
      msg ("[RLC_AM][RB %d] DISCARD  TRANSMIT ERROR RX MRW_ACK CASE NOT TAKEN IN ACCOUNT : NO ACTION !\n", rlcP->rb_id);
    }
  }
#ifdef DEBUG_RLC_AM_DISCARD
  //display_mem_load();
  //display_retransmission_buffer(rlcP);
#endif
  return p8;
}

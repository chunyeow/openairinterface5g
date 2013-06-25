/***************************************************************************
                          rlc_am_receiver.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "rlc_am_entity.h"
#include "rlc_def.h"
#include "rlc_am_structs.h"
#include "rlc_am_constants.h"
#include "rlc_am_reassembly_proto_extern.h"
#include "rlc_am_proto_extern.h"
#include "rlc_am_util_proto_extern.h"
#include "mem_block.h"

//#define DEBUG_RECEIVER_BUFFER
//#define DEBUG_REASSEMBLY
//#define DEBUG_STATUS
//-----------------------------------------------------------------------------
void            receiver_retransmission_management (struct rlc_am_entity *rlcP, mem_block_t * pduP, struct rlc_am_pdu_header *rlc_headerP);
void            free_receiver_buffer (struct rlc_am_entity *rlcP, u16_t indexP);
void            insert_into_receiver_buffer (struct rlc_am_entity *rlcP, u16_t indexP, mem_block_t * pduP);
void            process_receiver_buffer_15 (struct rlc_am_entity *rlcP);
void            process_receiver_buffer_7 (struct rlc_am_entity *rlcP);
//-----------------------------------------------------------------------------
void
receiver_retransmission_management (struct rlc_am_entity *rlcP, mem_block_t * pduP, struct rlc_am_pdu_header *rlc_headerP)
{
//-----------------------------------------------------------------------------

  u16_t             id;
  u16_t             id_index;

  /* From TS25.3222 V5.0.0
     Upon reception of an AMD PDU, the Receiver shall:
     - update VR(R), VR(H) and VR(MR) state variables for each received AMD PDU (see clause 9.4);
     - if a received AMD PDU includes a "Polling bit" set to "1", or "Missing PDU Indicator" is configured and the
     Receiver detects that a PDU is missing:
     - initiate the STATUS PDU transfer procedure;
     - reassemble the received AMD PDUs into RLC SDUs;
     - if "In-Sequence Delivery" is configured:
     - deliver the RLC SDUs in-sequence (i.e. in the same order as the RLC SDUs were originally transmitted by
     the peer entity) to upper layers through the AM-SAP.
     - otherwise:  (NOT IMPLEMENTED)
     - deliver the RLC SDUs in arbitrary order to upper layers through the AM-SAP. */

#ifdef DEBUG_RECEIVER_BUFFER
  msg("[RLC_AM][DEBUG] receiver_retransmission_management() received PDU %p\n", pduP);
  display_protocol_vars_rlc_am(rlcP);
#endif

  id = (((u16_t) (rlc_headerP->byte1 & RLC_AM_SN_1ST_PART_MASK)) << 5) | ((rlc_headerP->byte2 & RLC_AM_SN_2ND_PART_MASK) >> 3);

  // general case
  ((struct rlc_am_rx_pdu_management *) (pduP->data))->sn = id;

  id_index = id % rlcP->recomputed_configured_rx_window_size;

#ifdef DEBUG_RECEIVER_BUFFER
     msg("[RLC][RB %d][DEBUG] receiver_retransmission_management() received PDU %04X :\n", rlcP->rb_id, id);
  //display_protocol_vars_rlc_am(rlcP);
#endif

  // test poll bit for status pdu transmission;
  if ((rlc_headerP->byte2 & RLC_AM_P_STATUS_REPORT_MASK) == RLC_AM_P_STATUS_REPORT_REQUESTED) {
    rlcP->send_status_pdu_requested = 1;
#ifdef DEBUG_STATUS
    msg ("[RLC_AM][RB %d][RECEIVER_BUFFER] STATUS REQUESTED BY PEER\n", rlcP->rb_id);
#endif
  }

  if (id == rlcP->vr_r) {

#ifdef DEBUG_RECEIVER_BUFFER
    msg ("[RLC_AM][RB %d][RECEIVER_BUFFER] RECEIVED %p VR(R)=0x%04X id_index=0x%02X VR(H)=0x%04X\n", rlcP->rb_id, pduP, rlcP->vr_r, id_index, rlcP->vr_h);
#endif
    insert_into_receiver_buffer (rlcP, id_index, pduP);
    // update vr_h;
    if (rlcP->vr_h == id) {
      rlcP->vr_h = (id + 1) & SN_12BITS_MASK;
    }
    adjust_vr_r_mr (rlcP);
  } else {

    rlcP->send_status_pdu_requested = 1;
#ifdef DEBUG_RECEIVER_BUFFER
    msg ("[RLC_AM][RB %d][RECEIVER_BUFFER] RECEIVED %p sn=0x%04X id_index=0x%02X VR(R)=0x%04X VR(H)=0x%04X WILL SEND STATUS\n", rlcP->rb_id, pduP, id, id_index, rlcP->vr_r, rlcP->vr_h);
#endif
    //------------------------------------;
    // CASE VR(R)<VR(H)<VR(MR);
    //------------------------------------;
    if ((rlcP->vr_r <= rlcP->vr_h) && (rlcP->vr_h <= rlcP->vr_mr) && (id < rlcP->vr_mr) && (id > rlcP->vr_r)) {
#ifdef DEBUG_RECEIVER_BUFFER
      msg ("[RLC_AM][RB %d][RECEIVER_BUFFER] RECEIVED %p sn 0x%04X IN WINDOW\n", rlcP->rb_id, pduP, id);
#endif
      insert_into_receiver_buffer (rlcP, id_index, pduP);

      // CASE VR(R)<VR(H)<id<VR(MR);
      if (id >= rlcP->vr_h) {
        rlcP->vr_h = (id + 1) & SN_12BITS_MASK;
      }
      //------------------------------------;
      // CASE id<VR(MR)<VR(R)<VR(H);
      //------------------------------------;
      // CASE VR(MR)<VR(R)<VR(H)<id;
    } else if (((rlcP->vr_mr < rlcP->vr_r) && (rlcP->vr_h >= rlcP->vr_r) && (id < rlcP->vr_mr)) || ((rlcP->vr_mr < rlcP->vr_r) && (rlcP->vr_h >= rlcP->vr_r) && (id > rlcP->vr_r))) {

#ifdef DEBUG_RECEIVER_BUFFER
      msg ("[RLC_AM][RB %d][RECEIVER_BUFFER] RECEIVED %p sn 0x%04X IN REVERSED WINDOW\n", rlcP->rb_id, pduP, id);
#endif
      insert_into_receiver_buffer (rlcP, id_index, pduP);
      if ((id >= rlcP->vr_h) || (id < rlcP->vr_mr)) {
        rlcP->vr_h = (id + 1) & SN_12BITS_MASK;
      }
      //------------------------------------;
      // CASE VR(H)<VR(MR)<VR(R);
      //------------------------------------;
    } else if ((rlcP->vr_h <= rlcP->vr_mr) && (rlcP->vr_mr < rlcP->vr_r)) {
      if (id < rlcP->vr_mr) {
#ifdef DEBUG_RECEIVER_BUFFER
        msg ("[RLC_AM][RB %d][RECEIVER_BUFFER] RECEIVED %p sn 0x%04X IN REVERSED WINDOW\n", rlcP->rb_id, pduP, id);
#endif
        insert_into_receiver_buffer (rlcP, id_index, pduP);

        if (id >= rlcP->vr_h) {
          rlcP->vr_h = (id + 1) & SN_12BITS_MASK;
        }
      } else if (id > rlcP->vr_r) {
#ifdef DEBUG_RECEIVER_BUFFER
        msg ("[RLC_AM][RB %d][RECEIVER_BUFFER] RECEIVED %p sn 0x%04X IN REVERSED WINDOW\n", rlcP->rb_id, pduP, id);
#endif
        insert_into_receiver_buffer (rlcP, id_index, pduP);
      } else {
        // discard this PDU;
#ifdef DEBUG_RECEIVER_BUFFER
        msg ("[RLC_AM][RB %d][RECEIVER_BUFFER] RECEIVED %p sn 0x%04X OUT OF WINDOW\n", rlcP->rb_id, pduP, id);
#endif
       rlcP->stat_rx_data_pdu_out_of_window += 1;
        free_mem_block (pduP);
      }
      //------------------------------------;
      // REJECT;
      //------------------------------------;
    } else {
      // discard this PDU;
#ifdef DEBUG_RECEIVER_BUFFER
      msg ("[RLC_AM][RB %d][RECEIVER_BUFFER] RECEIVED %p sn 0x%04X OUT OF WINDOW\n", rlcP->rb_id, pduP, id);
#endif
       rlcP->stat_rx_data_pdu_out_of_window += 1;
      free_mem_block (pduP);
    }
  }
}

//-----------------------------------------------------------------------------
void
free_receiver_buffer (struct rlc_am_entity *rlcP, u16_t indexP)
{
//-----------------------------------------------------------------------------
  if (indexP < rlcP->recomputed_configured_rx_window_size) {
    if (rlcP->receiver_buffer[indexP]) {
#ifdef DEBUG_FREE_RECEIVER_BUFFER
      msg ("[RLC_AM][RB %d][RECEIVER_BUFFER] FREE PDU %p index 0x%4X\n", rlcP->rb_id, rlcP->receiver_buffer[indexP], indexP);
#endif
      free_mem_block (rlcP->receiver_buffer[indexP]);
      rlcP->receiver_buffer[indexP] = NULL;
    }
    return;
  }
  msg ("[RLC_AM][RB %d][RECEIVER_BUFFER] ERROR free_receiver_buffer() invalid index in array 0x%2X\n", rlcP->rb_id, indexP);
}

//-----------------------------------------------------------------------------
void
insert_into_receiver_buffer (struct rlc_am_entity *rlcP, u16_t indexP, mem_block_t * pduP)
{
//-----------------------------------------------------------------------------
  if (pduP) {
    if (indexP < rlcP->recomputed_configured_rx_window_size) {
#ifdef DEBUG_RECEIVER_BUFFER
      if (rlcP->receiver_buffer[indexP] != NULL) {
        msg ("[RLC_AM][RB %d][RECEIVER_BUFFER] ERROR insert_into_receiver_buffer() location is not NULL, index in array = 0x%2X\n", rlcP->rb_id, indexP);
        display_protocol_vars_rlc_am (rlcP);
      }
#endif
      // last received is good
      if (rlcP->receiver_buffer[indexP] != NULL) {
        free_mem_block (rlcP->receiver_buffer[indexP]);
      }
      rlcP->receiver_buffer[indexP] = pduP;
      return;
    }
    msg ("[RLC_AM][RB %d][RECEIVER_BUFFER] ERROR insert_into_receiver_buffer() invalid index in array 0x%2X\n", rlcP->rb_id, indexP);
    return;
  }
  msg ("[RLC_AM][RB %d][RECEIVER_BUFFER] ERROR insert_into_receiver_buffer() pdu is NULL index 0x%2X\n", rlcP->rb_id, indexP);
}

//-----------------------------------------------------------------------------
void
process_receiver_buffer_15 (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------

  struct rlc_am_pdu_header *rlc_header;
  mem_block_t      *pdu;
  u16_t             vr_r;
  u16_t             working_sn;
  u16_t             working_sn_index;     // index in buffer
  u8_t             *data_pdu;
  u16_t             li[RLC_AM_SEGMENT_NB_MAX_LI_PER_PDU];
  u16_t             remaining_data_size;
  u8_t              nb_li;
  u8_t              li_index;
  u8_t              li_start_index;
  u8_t              reassembly_after_discard;


  // should start reassembly with sn working_sn
  working_sn = (rlcP->last_reassemblied_sn + 1) & SN_12BITS_MASK;
  working_sn_index = working_sn % rlcP->recomputed_configured_rx_window_size;

  vr_r = rlcP->vr_r;
  reassembly_after_discard = 0;
  while (rlcP->receiver_buffer[working_sn_index]) {

    pdu = rlcP->receiver_buffer[working_sn_index];
    nb_li = 0;
    li_index = 0;
    rlc_header = (struct rlc_am_pdu_header *) (((struct rlc_am_rx_pdu_management *) (pdu->data))->first_byte);

    if ((rlcP->discard_reassembly_start_sn == ((struct rlc_am_rx_pdu_management *) (pdu->data))->sn)) {

      rlcP->output_sdu_size_to_write = 0;
      reassembly_after_discard = 1;     // keep trace of entering in this block

      if (!(rlc_header->byte2 & RLC_HE_MASK) == RLC_HE_SUCC_BYTE_CONTAINS_DATA) {
        while ((li[nb_li] = ((((u16_t) rlc_header->li_data_7[nb_li << 1]) << 8) + rlc_header->li_data_7[(nb_li << 1) + 1])) & RLC_E_NEXT_FIELD_IS_LI_E) {

          li[nb_li] = li[nb_li] & (~(u16_t) RLC_E_NEXT_FIELD_IS_LI_E);
          nb_li++;
        }
        nb_li++;                // count the first li
      }
      // this variable may be changed by the reception of a mrw sufi
      //li_start_index = rlcP->discard_reassembly_after_li + 1; // reassembly will start at this index
      li_start_index = rlcP->discard_reassembly_after_li;       // reassembly will start at this index
      // after reception of sufi mrw the starting index for li will be 0
      rlcP->discard_reassembly_after_li = RLC_AM_DISCARD_REASSEMBLY_AT_LI_INDEX_0;      // =-1


      remaining_data_size = rlcP->pdu_size - 2 - (nb_li << 1);
      data_pdu = (u8_t *) (&rlc_header->li_data_7[nb_li << 1]);

      while (li_index < nb_li) {
        switch (li[li_index]) {
            case RLC_LI_LAST_PDU_EXACTLY_FILLED:
              if (li_index >= li_start_index) {
                send_sdu (rlcP);
              }
              break;
            case RLC_LI_LAST_PDU_ONE_BYTE_SHORT:
              if (li_index >= li_start_index) {
                rlcP->output_sdu_size_to_write -= 1;
                send_sdu (rlcP);
              }
              break;
            case RLC_LI_PDU_PIGGY_BACKED_STATUS:       // ignore
            case RLC_LI_PDU_PADDING:
              remaining_data_size = 0;
              break;
            default:           // li is length
              remaining_data_size = remaining_data_size - (li[li_index] >> 1);
              if (li_index >= li_start_index) {
                reassembly (data_pdu, (li[li_index] >> 1), rlcP);
                send_sdu (rlcP);
              }
              data_pdu = (u8_t *) ((u32_t) data_pdu + (li[li_index] >> 1));
        }
        li_index++;
      }
      if ((remaining_data_size)) {
        reassembly (data_pdu, remaining_data_size, rlcP);
        remaining_data_size = 0;
      }


      free_receiver_buffer (rlcP, working_sn_index);
      rlcP->discard_reassembly_start_sn = RLC_AM_SN_INVALID;
      rlcP->last_reassemblied_sn = working_sn;
      working_sn = (rlcP->last_reassemblied_sn + 1) & SN_12BITS_MASK;
      working_sn_index = working_sn % rlcP->recomputed_configured_rx_window_size;

      // patch to make the protocol working
      // return 1 if sn1 > sn2
      // return 0 if sn1 = sn2
      // return -1 if sn1 < sn2
      if (rlc_am_comp_sn (rlcP, rlcP->vr_r, rlcP->vr_r, working_sn) < 0) {
        rlcP->vr_r = working_sn;
        rlcP->vr_mr = (rlcP->vr_r + rlcP->configured_rx_window_size - 1) & SN_12BITS_MASK;
      }

    } else {
      // exploit HE field info
      if ((rlc_header->byte2 & RLC_HE_MASK) == RLC_HE_SUCC_BYTE_CONTAINS_DATA) {
        reassembly ((u8_t *) (rlc_header->li_data_7), rlcP->pdu_size - 2, rlcP);
      } else {
        while ((li[nb_li] = ((((u16_t) rlc_header->li_data_7[nb_li << 1]) << 8) + rlc_header->li_data_7[(nb_li << 1) + 1])) & RLC_E_NEXT_FIELD_IS_LI_E) {
          //while ((li[nb_li] = (rlc_header->li.li_data_15[nb_li].optional)) & RLC_E_NEXT_FIELD_IS_LI_E) {
          li[nb_li] = li[nb_li] & (~(u16_t) RLC_E_NEXT_FIELD_IS_LI_E);
          nb_li++;
        }
        nb_li++;

        remaining_data_size = rlcP->pdu_size - 2 - (nb_li << 1);
        data_pdu = (u8_t *) (&rlc_header->li_data_7[nb_li << 1]);

        while (li_index < nb_li) {
          switch (li[li_index]) {
              case RLC_LI_LAST_PDU_EXACTLY_FILLED:
                send_sdu (rlcP);
                break;
              case RLC_LI_LAST_PDU_ONE_BYTE_SHORT:
                rlcP->output_sdu_size_to_write -= 1;
                send_sdu (rlcP);
                break;
              case RLC_LI_PDU_PIGGY_BACKED_STATUS:     // ignore
              case RLC_LI_PDU_PADDING:
                remaining_data_size = 0;
                break;
              default:         // li is length
                remaining_data_size = remaining_data_size - (li[li_index] >> 1);
                reassembly (data_pdu, (li[li_index] >> 1), rlcP);
                data_pdu = (u8_t *) ((u32_t) data_pdu + (li[li_index] >> 1));
                send_sdu (rlcP);
          }
          li_index++;
        }
        if ((remaining_data_size)) {
          reassembly (data_pdu, remaining_data_size, rlcP);
          remaining_data_size = 0;
        }
      }
      free_receiver_buffer (rlcP, working_sn_index);
      rlcP->last_reassemblied_sn = working_sn;
      working_sn = (rlcP->last_reassemblied_sn + 1) & SN_12BITS_MASK;
      working_sn_index = working_sn % rlcP->recomputed_configured_rx_window_size;

      if ((reassembly_after_discard)) {
        // return 1 if vr_r > working_sn
        // return 0 if vr_r = working_sn
        // return -1 if vr_r < working_sn
        if (rlc_am_comp_sn (rlcP, rlcP->vr_r, rlcP->vr_r, working_sn) < 0) {
          rlcP->vr_r = working_sn;
#ifdef DEBUG_REASSEMBLY
          msg ("[RLC_AM][RB %d][REASSEMBLY] DETECTED PDU AFTER DISCARD ADJUST VR(R) 0x%04X\n", rlcP->rb_id, rlcP->vr_r);
#endif

          rlcP->vr_mr = (rlcP->vr_r + rlcP->configured_rx_window_size - 1) & SN_12BITS_MASK;
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
void
process_receiver_buffer_7 (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------
  struct rlc_am_pdu_header *rlc_header;
  mem_block_t      *pdu;
  u16_t             vr_r;
  u16_t             working_sn;
  u16_t             working_sn_index;     // index in buffer
  u8_t             *data_pdu;
  u8_t              li[RLC_AM_SEGMENT_NB_MAX_LI_PER_PDU];
  u16_t             remaining_data_size;
  s8_t              nb_li;
  s8_t              li_index;
  s8_t              li_start_index;
  u8_t              reassembly_after_discard;
  u8_t              sdu_sent=0;

  // should start reassembly with sn working_sn
  working_sn = (rlcP->last_reassemblied_sn + 1) & SN_12BITS_MASK;
  working_sn_index = working_sn % rlcP->recomputed_configured_rx_window_size;

  vr_r = rlcP->vr_r;

  reassembly_after_discard = 0;

  while (rlcP->receiver_buffer[working_sn_index]) {

    pdu = rlcP->receiver_buffer[working_sn_index];
    nb_li = 0;
    li_index = 0;
    rlc_header = (struct rlc_am_pdu_header *) (((struct rlc_am_rx_pdu_management *) (pdu->data))->first_byte);

#ifdef DEBUG_REASSEMBLY
    msg ("[RLC_AM][RB %d][REASSEMBLY] PDU %p SN 0x%04X (sdu_sent %d)\n", rlcP->rb_id, pdu, working_sn,sdu_sent);
#endif
    if ((rlcP->discard_reassembly_start_sn == ((struct rlc_am_rx_pdu_management *) (pdu->data))->sn)) {
      rlcP->output_sdu_size_to_write = 0;
      reassembly_after_discard = 1;     // keep trace of entering in this block

      if (!(rlc_header->byte2 & RLC_HE_MASK) == RLC_HE_SUCC_BYTE_CONTAINS_DATA) {
        while ((li[nb_li] = (rlc_header->li_data_7[nb_li])) & RLC_E_NEXT_FIELD_IS_LI_E) {
          li[nb_li] = li[nb_li] & (~(u8_t) RLC_E_NEXT_FIELD_IS_LI_E);
          nb_li++;
        }
        nb_li++;                // count the first li
      }
#ifdef DEBUG_REASSEMBLY
      msg ("[RLC_AM][RB %d][REASSEMBLY] DETECTED PDU TO BE REASSEMBLIED AFTER DISCARD, START SN 0x%04X hex VR(R) 0x%04X\n", rlcP->rb_id, rlcP->discard_reassembly_start_sn, rlcP->vr_r);
#endif

      // this variable may be changed by the reception of a mrw sufi
      //li_start_index = rlcP->discard_reassembly_after_li + 1; // reassembly will start at this index
      li_start_index = rlcP->discard_reassembly_after_li;       // reassembly will start at this index
      // after reception of sufi mrw the starting index for li will be 0
      rlcP->discard_reassembly_after_li = RLC_AM_DISCARD_REASSEMBLY_AT_LI_INDEX_0;      // =-1

      remaining_data_size = rlcP->pdu_size - 2 - (nb_li);
      data_pdu = (u8_t *) (&rlc_header->li_data_7[nb_li]);

      while (li_index < nb_li) {
        switch (li[li_index]) {
            case (u8_t) RLC_LI_LAST_PDU_EXACTLY_FILLED:
#ifdef DEBUG_REASSEMBLY
              msg ("[RLC_AM][RB %d][REASSEMBLY] PDU SN 0x%04X GET LI RLC_LI_LAST_PDU_EXACTLY_FILLED REMAINING DATA SIZE %d, li_index %d, li_start_index %d\n", rlcP->rb_id, working_sn, remaining_data_size, li_index, li_start_index);
#endif
              if (li_index >= li_start_index) {
                send_sdu (rlcP);
		sdu_sent=1;
              }
              break;
            case (u8_t) RLC_LI_PDU_PIGGY_BACKED_STATUS:  // ignore
            case (u8_t) RLC_LI_PDU_PADDING:
#ifdef DEBUG_REASSEMBLY
              msg ("[RLC_AM][RB %d][REASSEMBLY] PDU SN 0x%04X GET LI RLC_LI_PDU_PADDING\n", rlcP->rb_id, working_sn);
#endif
              remaining_data_size = 0;
              break;
            default:           // li is length
#ifdef DEBUG_REASSEMBLY
              msg ("[RLC_AM][RB %d][REASSEMBLY] PDU SN 0x%04X GET LI SIZE %d\n", rlcP->rb_id, working_sn, li[li_index] >> 1);
#endif
              remaining_data_size = remaining_data_size - (li[li_index] >> 1);
              if (li_index >= li_start_index) {
                reassembly (data_pdu, (li[li_index] >> 1), rlcP);
                send_sdu (rlcP);
		sdu_sent=1;
              }
              data_pdu = (u8_t *) ((u32_t) data_pdu + (li[li_index] >> 1));
        }
        li_index++;
      }

      if ((remaining_data_size)) {
        reassembly (data_pdu, remaining_data_size, rlcP);
        remaining_data_size = 0;
      }

      free_receiver_buffer (rlcP, working_sn_index);
      rlcP->discard_reassembly_start_sn = RLC_AM_SN_INVALID;
      rlcP->last_reassemblied_sn = working_sn;
      working_sn = (rlcP->last_reassemblied_sn + 1) & SN_12BITS_MASK;
      working_sn_index = working_sn % rlcP->recomputed_configured_rx_window_size;

      // return 1 if vr_r > working_sn
      // return 0 if vr_r = working_sn
      // return -1 if vr_r < working_sn
      if (rlc_am_comp_sn (rlcP, rlcP->vr_r, rlcP->vr_r, working_sn) < 0) {
        rlcP->vr_r = working_sn;
#ifdef DEBUG_REASSEMBLY
        msg ("[RLC_AM][RB %d][REASSEMBLY] DETECTED PDU AFTER DISCARD ADJUST VR(R) 0x%04X hex\n", rlcP->rb_id, rlcP->vr_r);
#endif

        rlcP->vr_mr = (rlcP->vr_r + rlcP->configured_rx_window_size - 1) & SN_12BITS_MASK;
      }

    } else {
#ifdef DEBUG_REASSEMBLY
      msg("[RLC][AM] Reassembly, No discard. rlc_header->byte2 %d\n");
#endif
      // exploit HE field info
      if ((rlc_header->byte2 & RLC_HE_MASK) == RLC_HE_SUCC_BYTE_CONTAINS_DATA) {
        msg("WILL REASSEMBLY INDEX %04X\n", working_sn_index);
        reassembly ((u8_t *) (rlc_header->li_data_7), rlcP->pdu_size - 2, rlcP);
      } else {
        while ((li[nb_li] = (rlc_header->li_data_7[nb_li])) & RLC_E_NEXT_FIELD_IS_LI_E) {
          li[nb_li] = li[nb_li] & (~(u8_t) RLC_E_NEXT_FIELD_IS_LI_E);
          nb_li++;
        }
        nb_li++;

        remaining_data_size = rlcP->pdu_size - 2 - nb_li;
        data_pdu = (u8_t *) (&rlc_header->li_data_7[nb_li]);

        while (li_index < nb_li) {

          switch (li[li_index]) {
              case (u8_t) RLC_LI_LAST_PDU_EXACTLY_FILLED:
#ifdef DEBUG_REASSEMBLY
                msg ("[RLC_AM][RB %d][REASSEMBLY] PDU SN 0x%04X GET LI RLC_LI_LAST_PDU_EXACTLY_FILLED NUM LI %d\n", rlcP->rb_id, working_sn,nb_li);
#endif
                send_sdu (rlcP);

		sdu_sent=1;
                break;
              case (u8_t) RLC_LI_PDU_PIGGY_BACKED_STATUS:        // ignore
              case (u8_t) RLC_LI_PDU_PADDING:
#ifdef DEBUG_REASSEMBLY
                msg ("[RLC_AM][RB %d][REASSEMBLY] PDU SN 0x%04X GET LI RLC_LI_PDU_PADDING\n", rlcP->rb_id, working_sn);
#endif
                remaining_data_size = 0;
                break;
              default:         // li is length
#ifdef DEBUG_REASSEMBLY
                msg ("[RLC_AM][RB %d][REASSEMBLY] PDU SN 0x%04X GET LI SIZE %d Bytes\n", rlcP->rb_id, working_sn, li[li_index] >> 1);
#endif
                remaining_data_size = remaining_data_size - (li[li_index] >> 1);

                reassembly (data_pdu, (li[li_index] >> 1), rlcP);
                data_pdu = (u8_t *) ((u32_t) data_pdu + (li[li_index] >> 1));
                send_sdu (rlcP);
		sdu_sent=1;
          }
          li_index++;
        }
        if ((remaining_data_size)) {
          //msg("WILL REASSEMBLY INDEX %04X\n", working_sn_index);
          reassembly (data_pdu, remaining_data_size, rlcP);
          remaining_data_size = 0;
        }
      }
      free_receiver_buffer (rlcP, working_sn_index);
      rlcP->last_reassemblied_sn = working_sn;
      working_sn = (rlcP->last_reassemblied_sn + 1) & SN_12BITS_MASK;
      working_sn_index = working_sn % rlcP->recomputed_configured_rx_window_size;

      if ((reassembly_after_discard)) {
        // return 1 if vr_r > working_sn
        // return 0 if vr_r = working_sn
        // return -1 if vr_r < working_sn
        if (rlc_am_comp_sn (rlcP, rlcP->vr_r, rlcP->vr_r, working_sn) < 0) {
          rlcP->vr_r = working_sn;
#ifdef DEBUG_REASSEMBLY
          msg ("[RLC_AM][RB %d][REASSEMBLY] DETECTED PDU AFTER DISCARD ADJUST VR(R) 0x%04X\n", rlcP->rb_id, rlcP->vr_r);
#endif

          rlcP->vr_mr = (rlcP->vr_r + rlcP->configured_rx_window_size - 1) & SN_12BITS_MASK;
        }
      }
    }
    msg("receiver_buffer[%d] %p (sdu_sent %d)\n",working_sn_index,rlcP->receiver_buffer[working_sn_index],sdu_sent);
  }
  //  if (sdu_sent == 0) {
  //    msg("[RLC_AM][RB %d][REASSEMBLY] Forcing send_sdu (sent_sdu == 0)\n",rlcP->rb_id);
  //    send_sdu(rlcP);
  //  }
}

/***************************************************************************
                          rlc_am_status.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "rlc_am_entity.h"
#include "rlc_am_retrans_proto_extern.h"
#include "rlc_am_status_proto_extern.h"
#include "rlc_am_discard_rx_proto_extern.h"
#include "rlc_am_proto_extern.h"
#include "rlc_am_util_proto_extern.h"
#include "LAYER2/MAC/extern.h"
//#define DEBUG_CREATE_STATUS
//#define DEBUG_CREATE_STATUS_SUFI
//#define PROCESS_STATUS

void            rlc_am_write_sufi_no_more_in_control_pdu (u8_t * dataP, u8 byte_alignedP);
void            rlc_am_write_sufi_ack_in_control_pdu (u8_t * dataP, u16_t snP, u8 byte_alignedP);
int              rlc_am_send_status (struct rlc_am_entity *rlcP);
int                 rlc_am_create_status_pdu (struct rlc_am_entity *rlcP, list_t* listP);
void            rlc_am_find_holes (struct rlc_am_entity *rlcP);
//-----------------------------------------------------------------------------
void
rlc_am_write_sufi_no_more_in_control_pdu (u8_t * dataP, u8 byte_alignedP)
{
//-----------------------------------------------------------------------------

#ifdef DEBUG_CREATE_STATUS_SUFI
  msg ("[RLC_AM][STATUS]  GENERATE SUFI NO_MORE\n");
#endif
  if (!byte_alignedP) {
    *dataP = (*dataP & 0xF0) | RLC_AM_SUFI_NO_MORE;
  } else {
    *dataP = RLC_AM_SUFI_NO_MORE << 4;
  }
}

//-----------------------------------------------------------------------------
void
rlc_am_write_sufi_ack_in_control_pdu (u8_t * dataP, u16_t snP, u8 byte_alignedP)
{
//-----------------------------------------------------------------------------

#ifdef DEBUG_CREATE_STATUS_SUFI
  msg ("[RLC_AM][STATUS]  GENERATE SUFI ACK 0x%04X\n", snP);
#endif
  if (!byte_alignedP) {
    *dataP = (*dataP & 0xF0) | RLC_AM_SUFI_ACK;
    dataP++;
    *dataP++ = (u8_t) (snP >> 4);
    *dataP = (u8_t) (snP << 4);
  } else {
    *dataP = RLC_AM_SUFI_ACK << 4;
    *dataP = *dataP | (u8_t) (snP >> 8);
    dataP++;
    *dataP = (u8_t) (snP);
  }
}

//-----------------------------------------------------------------------------
int
rlc_am_send_status (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------

#ifdef DEBUG_CREATE_STATUS
  display_receiver_buffer (rlcP);
#endif

  if (rlcP->timer_status_prohibit > 0) {
    if (rlcP->running_timer_status_prohibit >  Mac_rlc_xface->frame) {
        //msg ("[RLC_AM][RB %d][STATUS] PREVENTED CREATE_STATUS_PDU %d %d\n", rlcP->rb_id, rlcP->running_timer_status_prohibit, Mac_rlc_xface->frame);

      return 0;
    } else {
        //msg ("[RLC_AM][RB %d][STATUS] AUTHORIZE CREATE_STATUS_PDU frame %d  timer %d timeout %d\n", rlcP->rb_id, Mac_rlc_xface->frame, rlcP->running_timer_status_prohibit, rlcP->timer_status_prohibit);
      rlcP->running_timer_status_prohibit = rlcP->timer_status_prohibit/10 + Mac_rlc_xface->frame;
    }
  }

  // sufis are created in a array of sufi in rlc struct;
  rlc_am_find_holes (rlcP);

  // create pdu with array of sufi previously generated;
  return rlc_am_create_status_pdu (rlcP, &rlcP->control);
}

//-----------------------------------------------------------------------------
int
rlc_am_create_status_pdu (struct rlc_am_entity *rlcP, list_t * listP)
{
//-----------------------------------------------------------------------------
  mem_block_t      *mb = NULL;
  struct rlc_am_status_header *pdu;
  u8_t             *p8;
  u8_t             *tmp;
  int             hole_index = 0;
  signed int      pdu_remaining_size;   // remaining size for SUFIs BITMAP, LIST, RLIST, WINDOW
  int             current_sn;
  signed int      remaining_sn;
  int             pdu_status_count = 0;
  int             length_sufi;
  s16_t             last_hole_sn = -1;
  u8_t              byte_aligned;


  remaining_sn = rlcP->vr_h - rlcP->vr_r;
  if (remaining_sn < 0) {
    remaining_sn = remaining_sn + SN_12BITS_MASK + 1;
  }

  while ((rlcP->holes[hole_index].valid) && (remaining_sn)) {


    if (!(mb)) {
      mb = get_free_mem_block (rlcP->pdu_size + sizeof (struct rlc_am_tx_control_pdu_allocation) + GUARD_CRC_LIH_SIZE);
      if (mb == NULL) {
        msg ("[MEM_MNGT] ERROR create_status_pdu() no free blocks\n");
        return 0;
      } else {
        pdu_status_count += 1;
#ifdef DEBUG_CREATE_STATUS
        msg ("[RLC_AM][RB %d][STATUS] CREATE_STATUS_PDU %d\n", rlcP->rb_id, pdu_status_count);
#endif
        memset (mb->data, 0, rlcP->pdu_size + sizeof (struct rlc_am_tx_control_pdu_allocation));
        if (pdu_status_count == 1) {
          ((struct rlc_am_tx_control_pdu_management *) (mb->data))->rlc_tb_type = RLC_AM_FIRST_STATUS_PDU_TYPE;
        } else {
          ((struct rlc_am_tx_control_pdu_management *) (mb->data))->rlc_tb_type = RLC_AM_STATUS_PDU_TYPE;
        }

        pdu = (struct rlc_am_status_header *) (&mb->data[sizeof (struct rlc_am_tx_control_pdu_allocation)]);
        pdu->byte1 = RLC_PDU_TYPE_STATUS;
        p8 = &(pdu->byte1);
        // if first pdu of status include ack field
        if (pdu_status_count == 1) {
          pdu_remaining_size = (rlcP->pdu_size << 3) - 16;      //-4(PDU_TYPE) -12(SUFI ACK)
          current_sn = rlcP->ack_sn;
          remaining_sn = rlcP->vr_h - rlcP->vr_r;
          if (remaining_sn < 0) {
            remaining_sn = remaining_sn + SN_12BITS_MASK + 1;
          }
        } else {
          pdu_remaining_size = (rlcP->pdu_size << 3) - 8;       //-4(PDU_TYPE) - 4(SUFI NO_MORE)
        }
        byte_aligned = 0;
      }
    }
    // From 3GPP TS 25.322 V4.2.0 :
    // Which SUFI fields to use is implementation dependent,...
    // IMPLEMENTATION : KEEP GENERATION OF SUFI SIMPLE : GENERATE ONLY LIST
    if (pdu_remaining_size >= RLC_AM_SUFI_LIST_SIZE_MIN) {
      //----------------------------------
      // generate LIST
      //----------------------------------
      if (!byte_aligned) {
        *p8 = *p8 | RLC_AM_SUFI_LIST;
        p8 = p8 + 1;
        tmp = p8;               //(*tmp = length_sufi << 4) when we will know length
        length_sufi = 0;
        pdu_remaining_size -= 8;
        while ((length_sufi < 15) && (pdu_remaining_size >= 16) && (rlcP->holes[hole_index].valid) && (rlcP->nb_missing_pdus)) {
#ifdef DEBUG_CREATE_STATUS
          msg ("[RLC_AM][RB %d][STATUS] GENERATE SUFI LIST HOLE START 0x%04X ", rlcP->rb_id, rlcP->holes[hole_index].fsn);
#endif
          *p8 = *p8 | (rlcP->holes[hole_index].fsn >> 8);
          p8 += 1;
          *p8 = rlcP->holes[hole_index].fsn;
          p8 += 1;
          if (rlcP->holes[hole_index].length <= 15) {
            *p8 = rlcP->holes[hole_index].length << 4;
#ifdef DEBUG_CREATE_STATUS
            msg ("LENGTH %d\n", rlcP->holes[hole_index].length);
#endif
            rlcP->nb_missing_pdus -= rlcP->holes[hole_index].length;
            current_sn = (rlcP->holes[hole_index].fsn + rlcP->holes[hole_index].length) & SN_12BITS_MASK;
            last_hole_sn = (rlcP->holes[hole_index].fsn + rlcP->holes[hole_index].length) & SN_12BITS_MASK;
            hole_index += 1;
          } else {
#ifdef DEBUG_CREATE_STATUS
            msg ("LENGTH 15\n");
#endif
            *p8 = 0xF0;
            rlcP->nb_missing_pdus -= 15;
            rlcP->holes[hole_index].length -= 15;
            rlcP->holes[hole_index].fsn = (rlcP->holes[hole_index].fsn + 15) & SN_12BITS_MASK;
            current_sn = rlcP->holes[hole_index].fsn;
          }
          length_sufi += 1;
          pdu_remaining_size -= 16;
        }
        *tmp = *tmp | (length_sufi << 4);
      } else {
        *p8 = RLC_AM_SUFI_LIST << 4;
        tmp = p8;               //(*tmp = length_sufi) when we will know length
        p8 = p8 + 1;
        length_sufi = 0;
        pdu_remaining_size -= 8;

        while ((length_sufi < 15) && (pdu_remaining_size >= 16) && (rlcP->holes[hole_index].valid) && (rlcP->nb_missing_pdus)) {
#ifdef DEBUG_CREATE_STATUS
          msg ("[RLC_AM][RB %d][STATUS] GENERATE SUFI LIST HOLE START 0x%04X ", rlcP->rb_id, rlcP->holes[hole_index].fsn);
#endif
          *p8 = rlcP->holes[hole_index].fsn >> 4;
          p8 += 1;
          *p8 = rlcP->holes[hole_index].fsn << 4;
          if (rlcP->holes[hole_index].length <= 15) {
#ifdef DEBUG_CREATE_STATUS
            msg ("LENGTH %d\n", rlcP->holes[hole_index].length);
#endif
            *p8 = *p8 | rlcP->holes[hole_index].length;
            p8 += 1;
            rlcP->nb_missing_pdus -= rlcP->holes[hole_index].length;
            current_sn = (rlcP->holes[hole_index].fsn + rlcP->holes[hole_index].length) & SN_12BITS_MASK;
            last_hole_sn = (rlcP->holes[hole_index].fsn + rlcP->holes[hole_index].length) & SN_12BITS_MASK;
            hole_index += 1;
          } else {
#ifdef DEBUG_CREATE_STATUS
            msg ("LENGTH 15\n");
#endif
            *p8 = *p8 | 15;
            p8 += 1;
            rlcP->nb_missing_pdus -= 15;
            rlcP->holes[hole_index].length -= 15;
            rlcP->holes[hole_index].fsn = (rlcP->holes[hole_index].fsn + 15) & SN_12BITS_MASK;
          }
          length_sufi += 1;
          pdu_remaining_size -= 16;
        }
        *tmp = *tmp | length_sufi;
      }
    } else {
      // it is not possible to write a sufi LIST in this control pdu, so close this pdu (write ACK or NO_MORE)
      if (pdu_status_count == 1) {
        //rlc_am_write_sufi_ack_in_control_pdu(p8, current_sn, byte_aligned);
        if (last_hole_sn > 0) {
          rlc_am_write_sufi_ack_in_control_pdu (p8, last_hole_sn, byte_aligned);
        } else {
          rlc_am_write_sufi_ack_in_control_pdu (p8, current_sn, byte_aligned);
        }
      } else {
        rlc_am_write_sufi_no_more_in_control_pdu (p8, byte_aligned);
      }
      //hole_index += 1;
      list_add_tail_eurecom (mb, listP);
      mb = NULL;
    }
    remaining_sn = rlcP->vr_h - current_sn;
    if (remaining_sn < 0) {
      remaining_sn = remaining_sn + SN_12BITS_MASK + 1;
    }
  }

  if ((mb == NULL) && !(pdu_status_count)) {
    mb = get_free_mem_block (rlcP->pdu_size + sizeof (struct rlc_am_tx_control_pdu_allocation) + GUARD_CRC_LIH_SIZE);
    if (mb == NULL) {
      msg ("[MEM_MNGT] ERROR create_status_pdu() no free blocks\n");
      return 0;
    } else {
#ifdef DEBUG_CREATE_STATUS
      msg ("[RLC_AM][RB %d][STATUS] CREATE_STATUS_PDU %d\n", rlcP->rb_id, pdu_status_count + 1);
#endif
      memset (mb->data, 0, rlcP->pdu_size + sizeof (struct rlc_am_tx_control_pdu_allocation));
      ((struct rlc_am_tx_control_pdu_management *) (mb->data))->rlc_tb_type = RLC_AM_FIRST_STATUS_PDU_TYPE | RLC_AM_LAST_STATUS_PDU_TYPE;

      pdu = (struct rlc_am_status_header *) (&mb->data[sizeof (struct rlc_am_tx_control_pdu_allocation)]);
      pdu->byte1 = RLC_PDU_TYPE_STATUS;
      p8 = &(pdu->byte1);
      byte_aligned = 0;
      rlc_am_write_sufi_ack_in_control_pdu (p8, rlcP->ack_sn, byte_aligned);
      list_add_tail_eurecom (mb, listP);
    }
  } else if ((mb)) {
    // if first pdu insert ack field updated to last sn referenced by a sufi in this pdu
    // else insert no more field
    ((struct rlc_am_tx_control_pdu_management *) (mb->data))->rlc_tb_type |= RLC_AM_LAST_STATUS_PDU_TYPE;
    if (pdu_status_count == 1) {
      if (last_hole_sn > 0) {
        rlc_am_write_sufi_ack_in_control_pdu (p8, last_hole_sn, byte_aligned);
      } else {
        rlc_am_write_sufi_ack_in_control_pdu (p8, current_sn, byte_aligned);
      }
    } else {
      rlc_am_write_sufi_no_more_in_control_pdu (p8, byte_aligned);
    }
    list_add_tail_eurecom (mb, listP);
  }
   return 1;
}

//-----------------------------------------------------------------------------
void
rlc_am_find_holes (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------


  u16_t             working_sn, working_sn_index;
  u16_t             end_sn;
  u16_t             distance;
  int             hole_index;

  //--------------------------------------------------
  // FIND HOLES
  //--------------------------------------------------
  hole_index = 0;
  // ACK
  rlcP->ack_sn = rlcP->vr_r;
  // HOLES
  rlcP->holes[0].valid = 0;
  rlcP->nb_missing_pdus = 0;

  working_sn = rlcP->vr_r;
  working_sn_index = working_sn % rlcP->recomputed_configured_rx_window_size;
  end_sn = rlcP->vr_h;

  while (working_sn != end_sn) {
    if ((rlcP->receiver_buffer[working_sn_index])) {
      // UPDATE ACK FIELD :
      if (rlcP->ack_sn == working_sn) {
        rlcP->ack_sn = (working_sn + 1) & SN_12BITS_MASK;
      }
      // STOP PREVIOUS HOLE IF ANY
      if (rlcP->holes[hole_index].valid) {
        hole_index += 1;
        rlcP->holes[hole_index].valid = 0;
      }
    } else {

      // CONTINUE PREVIOUS HOLE IF ANY
      if (rlcP->holes[hole_index].valid) {
        rlcP->holes[hole_index].length += 1;
        rlcP->nb_missing_pdus += 1;

        // START HOLE IF ANY
      } else {
#ifdef DEBUG_RLC_AM_FIND_HOLE
        msg ("[RLC_AM %p] FOUND HOLE %d START 0x%04x(hex)\n", rlcP, hole_index, working_sn);
#endif
        if (hole_index > 0) {
          distance = working_sn - rlcP->holes[hole_index - 1].fsn;
          if (distance & 0x8000) {      // < 0
            rlcP->holes[hole_index - 1].dist_to_next = distance + SN_12BITS_MASK + 1;
          } else {
            rlcP->holes[hole_index - 1].dist_to_next = distance;
          }
        }
        rlcP->holes[hole_index].valid = 1;
        rlcP->holes[hole_index + 1].valid = 0;
        rlcP->holes[hole_index].fsn = working_sn;
        rlcP->holes[hole_index].length = 1;
        rlcP->nb_missing_pdus += 1;
      }
    }
    working_sn = (working_sn + 1) & SN_12BITS_MASK;
    working_sn_index = working_sn % rlcP->recomputed_configured_rx_window_size;
  }
  // compute the number of holes
  if (rlcP->holes[hole_index].valid) {
    rlcP->nb_holes = hole_index;
  } else {
    if (hole_index > 0) {
      rlcP->nb_holes = hole_index - 1;
    } else {
      rlcP->nb_holes = 0;
    }
  }
}

//-----------------------------------------------------------------------------
void
rlc_am_process_status_info (struct rlc_am_entity *rlcP, u8_t * statusP)
{
//-----------------------------------------------------------------------------

  u8_t             *byte1;
  u8_t              byte_aligned; // 1: quartet starts on bit 7, 0: quartet starts on bit 3 (of a byte)
  u8_t              sufi_type;
  u8_t              end_process = 0;
  s16_t             ack_sn = -1;
  s16_t             first_error_indicated_sn = -1;
  s16_t             sn_index;
  s16_t             current_sn;
  s16_t             current_index;

  byte1 = statusP;

#ifdef DEBUG_PROCESS_STATUS
  display_retransmission_buffer (rlcP);
#endif

  if ((*byte1 & RLC_PIGGY_PDU_TYPE_MASK) == RLC_PIGGY_PDU_TYPE_STATUS) {
    sufi_type = *byte1 & 0x0F;
    byte_aligned = 0;

    // process all bytes of the PDU
    while (!end_process) {
      // reset may have been triggered during processing of status, so exit
      //if (rlcP->protocol_state == RLC_DATA_TRANSFER_READY_STATE){
      switch (sufi_type) {

          case RLC_AM_SUFI_NO_MORE:
            // end of pdu
            end_process = 1;
            break;

          case RLC_AM_SUFI_WINDOW:
            msg ("[RLC_AM %p]PROCESS_STATUS ERROR process_status_info() RLC_AM_SUFI_WINDOW not implemented\n", rlcP);
            end_process = 1;
            break;

          case RLC_AM_SUFI_ACK:

#ifdef  DEBUG_PROCESS_STATUS
            msg ("[RLC_AM %p]PROCESS_STATUS  ACK\n", rlcP);
#endif
            ack_sn = retransmission_buffer_management_ack (rlcP, byte1, byte_aligned, &first_error_indicated_sn);

#ifdef  DEBUG_PROCESS_STATUS
            msg ("[RLC_AM %p]PROCESS_STATUS  ACK WAS LSN %d(b10) %4X(b16)\n", rlcP, ack_sn, ack_sn);
#endif
            // DISCARD : A discard procedure is terminated in the sender on the reception of a status pdu which contains
            // an ACK SUFI indicating VR(R) > SN_MRWlength
            rlc_am_received_sufi_ack_check_discard_procedures (rlcP);

            // end of pdu, no NO_MORE sufi;
            end_process = 1;
#ifdef  DEBUG_PROCESS_STATUS
            display_protocol_vars_rlc_am (rlcP);
#endif
            break;

          case RLC_AM_SUFI_LIST:

#ifdef  DEBUG_PROCESS_STATUS
            msg ("[RLC_AM %p]PROCESS_STATUS  LIST\n", rlcP);
#endif
            byte1 = retransmission_buffer_management_list (rlcP, byte1, byte_aligned, &first_error_indicated_sn);
            if (byte1 == NULL) {
              // error in processing;
              end_process = 1;
              break;
            }
            // byte aligned is not changed for this sufi;
            if (!byte_aligned) {
              sufi_type = *byte1 & 0X0F;
            } else {
              sufi_type = (*byte1 & 0xF0) >> 4;
            }
            break;

          case RLC_AM_SUFI_BITMAP:

#ifdef  DEBUG_PROCESS_STATUS
            msg ("[RLC_AM %p]PROCESS_STATUS BITMAP %p 0x%02X\n", rlcP, byte1, *byte1);
#endif
            byte1 = retransmission_buffer_management_bitmap (rlcP, byte1, byte_aligned, &first_error_indicated_sn);
            if (byte1 == NULL) {
              // error in processing;
              end_process = 1;
              break;
            }
            // the length of this sufi is always n bytes + 4;
            // so invert byte_aligned;
            if (byte_aligned) {
              byte_aligned = 0;
              sufi_type = *byte1 & 0X0F;
            } else {
              byte_aligned = 1;
              sufi_type = (*byte1 & 0xF0) >> 4;
            }
            break;

          case RLC_AM_SUFI_RLIST:
            msg ("[RLC_AM %p]PROCESS_STATUS ERROR SUFI RLIST NOT IMPLEMENTED\n", rlcP);
            end_process = 1;
            break;

          case RLC_AM_SUFI_MRW:
#ifdef  DEBUG_PROCESS_STATUS
            msg ("[RLC_AM %p]PROCESS_STATUS MRW %p 0x%02X\n", rlcP, byte1, *byte1);
#endif
            byte1 = retransmission_buffer_management_mrw (rlcP, byte1, &byte_aligned);
            if (byte1 == NULL) {
              // error in processing;
              end_process = 1;
              break;
            }
            // the length of this sufi is always n bytes + 4;
            // so invert byte_aligned;
            if (byte_aligned) {
              sufi_type = (*byte1 & 0xF0) >> 4;
            } else {
              sufi_type = *byte1 & 0X0F;
            }
            break;

          case RLC_AM_SUFI_MRW_ACK:
#ifdef  DEBUG_PROCESS_STATUS
            msg ("[RLC_AM %p]PROCESS_STATUS MRW_ACK %p 0x%02X\n", rlcP, byte1, *byte1);
#endif
            byte1 = retransmission_buffer_management_mrw_ack (rlcP, byte1, &byte_aligned);
            if (byte1 == NULL) {
              // error in processing;
              end_process = 1;
              break;
            }
            // the length of this sufi is always n bytes + 4;
            // so invert byte_aligned;
            if (byte_aligned) {
              sufi_type = (*byte1 & 0xF0) >> 4;
            } else {
              sufi_type = *byte1 & 0X0F;
            }
            break;

          default:
            msg ("[RLC_AM %p]PROCESS_STATUS ERROR SUFI UNKNOWN 0x%02X\n", rlcP, *byte1);
            end_process = 1;
      }
      /*} else {
         return;
         } */
    }

    //-------------------------
    // CLEAR ALL EVENTS RECEIVED;
    //-------------------------
    current_sn = rlcP->vt_a;
    current_index = rlcP->vt_a % rlcP->recomputed_configured_tx_window_size;
    sn_index = rlcP->vt_s % rlcP->recomputed_configured_tx_window_size;

    while (sn_index != current_index) {
      if (rlcP->retransmission_buffer[current_index] != NULL) {

        ((struct rlc_am_tx_data_pdu_management *) (rlcP->retransmission_buffer[current_index]->data))->ack = RLC_AM_PDU_ACK_NO_EVENT;
      }
      current_sn = (current_sn + 1) & SN_12BITS_MASK;
      current_index = current_sn % rlcP->recomputed_configured_tx_window_size;
    }

  } else {
    msg ("[RLC_AM %p][PROCESS_STATUS] ERROR process_piggybacked_status_info() PDU type field is not STATUS\n", rlcP);
  }
#ifdef DEBUG_PROCESS_STATUS
  display_retransmission_buffer (rlcP);
#endif
}

//-----------------------------------------------------------------------------
mem_block_t      *
rlc_am_create_status_pdu_mrw_ack (struct rlc_am_entity *rlcP, u8_t nP, u16_t sn_ackP)
{
//-----------------------------------------------------------------------------
  mem_block_t      *le;
  struct rlc_am_status_header *pdu;
  u8_t             *p8;

#ifdef DEBUG_STATUS
  msg ("[RLC_AM %p][STATUS] rlc_am_mrw_send_ack(N=%d, sn_ack=0x%04X)\n", rlcP, nP, sn_ackP);
#endif
  le = get_free_mem_block (rlcP->pdu_size + sizeof (struct rlc_am_tx_control_pdu_allocation) + GUARD_CRC_LIH_SIZE);
  if (le == NULL) {
    msg ("[MEM_MNGT][ERROR] rlc_am_mrw_send_ack() no free blocks\n");
    return NULL;
  } else {
    ((struct rlc_am_tx_control_pdu_management *) (le->data))->rlc_tb_type = RLC_AM_STATUS_PDU_TYPE;
    pdu = (struct rlc_am_status_header *) (&le->data[sizeof (struct rlc_am_tx_control_pdu_allocation)]);
    pdu->byte1 = RLC_PDU_TYPE_STATUS;
    p8 = &(pdu->byte1);
    *p8 = *p8 | RLC_AM_SUFI_MRW_ACK;
    p8 = p8 + 1;
    *p8 = (nP << 4) | (sn_ackP >> 8);
    p8 = p8 + 1;
    *p8 = sn_ackP;
    p8 = p8 + 1;
    *p8 = RLC_AM_SUFI_NO_MORE << 4;
    return le;
  }
}

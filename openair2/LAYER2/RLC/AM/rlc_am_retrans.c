/***************************************************************************
                          rlc_am_retrans.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#define RLC_AM_C
#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "rlc.h"
#include "rlc_am_entity.h"
#include "rlc_am_errno.h"
#include "rlc_am_constants.h"
#include "rlc_am_structs.h"
#include "rlc_am_discard_notif_proto_extern.h"
#include "rlc_am_util_proto_extern.h"
#include "rlc_am_reset_proto_extern.h"
#include "rlc_am_proto_extern.h"
#include "rlc_am_util_proto_extern.h"
#include "rlc_primitives.h"
#include "list.h"
#include  "LAYER2/MAC/extern.h"
//#define DEBUG_RETRANSMISSION
//#define DEBUG_ACK
//#define DEBUG_RESET
//#define DEBUG_BITMAP
//#define DEBUG_LIST
//-----------------------------------------------------------------------------
inline s16_t      retransmission_buffer_management_ack (struct rlc_am_entity *rlcP, u8_t * sufiP, u8 byte_alignedP, s16 * first_error_indicated_snP);
inline u8_t      *retransmission_buffer_management_bitmap (struct rlc_am_entity *rlcP, u8 * bitmap_sufiP, u8 byte_alignedP, s16_t * first_error_indicated_snP);
inline u8_t      *retransmission_buffer_management_list (struct rlc_am_entity *rlcP, u8 * bitmap_sufiP, u8 byte_alignedP, s16_t * first_error_indicated_snP);
inline void     free_retransmission_buffer (struct rlc_am_entity *rlcP, u16_t indexP);
inline void     free_retransmission_buffer_no_confirmation (struct rlc_am_entity *rlcP, u16_t indexP);
inline void     insert_into_retransmission_buffer (struct rlc_am_entity *rlcP, u16_t indexP, mem_block_t * pduP);
inline u8_t       retransmit_pdu (struct rlc_am_entity *rlcP, u16_t snP);
inline u8_t       add_to_transmission_buffer_unack_pdu (struct rlc_am_entity *rlcP, s16_t * nb_pdu_to_transmit);
//-----------------------------------------------------------------------------
/*
 * remove all mem_block_t(s) having sn <= snP
 */
s16_t
retransmission_buffer_management_ack (struct rlc_am_entity *rlcP, u8_t * sufiP, u8 byte_alignedP, s16_t * first_error_indicated_snP)
{
//-----------------------------------------------------------------------------

  u16_t             lsn;          // sequence number acknowledged
  u16_t             current_sn;
  u16_t             upper_bound;
  s16_t             current_index;
  u8_t              tmp;

  // get LSN field;
  if (byte_alignedP) {
    // get next 12 bits;
    lsn = (*sufiP++ & 0x0F);
    lsn = lsn << 8;
    lsn += *sufiP;
  } else {
    // get next 12 bits;
    sufiP++;
    lsn = *sufiP++;
    lsn = lsn << 4;
    tmp = *sufiP;
    lsn |= (tmp >> 4);
  }

#ifdef DEBUG_ACK
  msg ("[RLC_AM][RB %d][ACK] RX SUFI ACK  0x%04X VT(A) 0x%04X VT(S) 0x%04X VT(MS) 0x%04X\n", rlcP->rb_id, lsn, rlcP->vt_a, rlcP->vt_s, rlcP->vt_ms);
#endif

  current_sn = rlcP->vt_a;
  // From 3GPP TS 25.322 V4.3.0
  // LSN acknowledges the reception of all PDUs with "Sequence Number" < LSN (Last Sequence Number)
  // that are not indicated to be erroneous in earlier parts of the STATUS PDU.
  // This means that if the LSN is set to a value greater than VR(R), all erroneous PDUs shall
  // be included in the same STATUS PDU
  // and if the LSN is set to VR(R), the erroneous PDUs can be split into several STATUS PDUs.
  // At the transmitter, if the value of the LSN =< the value of the first error indicated
  // in the STATUS PDU, VT(A) will be updated according to the LSN,
  // otherwise VT(A) will be updated according to the first error indicated in the STATUS PDU.
  // VT(A) is only updated based on STATUS PDUs where ACK SUFI (or MRW_ACK SUFI) is included.
  // The LSN shall not be set to a value > VR(H) nor < VR(R).

  //-------------------------;
  // if ack lsn <= first error => vt_a = lsn;
  // else vt_a = first error indicated;

  // check lsn is valid
  if (rlc_am_comp_sn (rlcP, rlcP->vt_a, lsn, rlcP->vt_a) >= 0) {

    if (*first_error_indicated_snP >= 0) {
      if (rlc_am_comp_sn (rlcP, rlcP->vt_a, lsn, (u16_t) * first_error_indicated_snP) <= 0) {
#ifdef DEBUG_ACK
        msg ("[RLC_AM][RB %d][ACK] VT(A) 0x%04X -> 0x%04X    VT(S) 0x%04X CASE LSN <= FIRST PDU IN ERROR IN STATUS PDU\n", rlcP->rb_id, rlcP->vt_a, lsn, rlcP->vt_s);
#endif
        rlcP->vt_a = lsn;
      } else {
#ifdef DEBUG_ACK
        msg ("[RLC_AM][RB %d][ACK] VT(A) 0x%04X -> 0x%04X    VT(S) 0x%04X CASE LSN > FIRST PDU IN ERROR IN STATUS PDU \n", rlcP->rb_id, rlcP->vt_a, (u16_t) * first_error_indicated_snP, rlcP->vt_s);
#endif
        rlcP->vt_a = (u16_t) * first_error_indicated_snP;
      }
    } else {
#ifdef DEBUG_ACK
      msg ("[RLC_AM][RB %d][ACK] VT(A) 0x%04X -> 0x%04X    VT(S) 0x%04X CASE SUFI ACK ALONE IN CONTROL PDU\n", rlcP->rb_id, rlcP->vt_a, lsn, rlcP->vt_s);
#endif
      rlcP->vt_a = lsn;
    }
    rlcP->vt_ms = (rlcP->vt_a + rlcP->vt_ws - 1) & SN_12BITS_MASK;

    current_index = current_sn % (u16_t) rlcP->recomputed_configured_tx_window_size;
    upper_bound = lsn % (u16_t) rlcP->recomputed_configured_tx_window_size;

    // remove all matching pdus from retransmission buffer;
    while (upper_bound != current_index) {
      // free all pdus that have not been stamped as missing;
      if (rlcP->retransmission_buffer[current_index] != NULL) {
        if (((struct rlc_am_tx_data_pdu_management *) (rlcP->retransmission_buffer[current_index]->data))->ack != RLC_AM_PDU_NACK_EVENT) {
#ifdef DEBUG_ACK
          msg ("[RLC_AM][RB %d][ACK]  FREE PDU SN 0x%04X BECAUSE FIELD ACK=%d\n", rlcP->rb_id, current_sn,
               ((struct rlc_am_tx_data_pdu_management *) (rlcP->retransmission_buffer[current_index]->data))->ack);
#endif
          free_retransmission_buffer (rlcP, (u16_t) current_index);
        } else {
#ifdef DEBUG_ACK
          msg ("[RLC_AM][RB %d][ACK] CLEAR NACK EVENT (%d) PDU SN 0x%04X\n", rlcP->rb_id, ((struct rlc_am_tx_data_pdu_management *) (rlcP->retransmission_buffer[current_index]->data))->ack,
               current_sn);
#endif
          ((struct rlc_am_tx_data_pdu_management *) (rlcP->retransmission_buffer[current_index]->data))->ack = RLC_AM_PDU_ACK_NO_EVENT;
        }
      }
      current_sn = (current_sn + 1) & SN_12BITS_MASK;
      current_index = current_sn % (u16_t) rlcP->recomputed_configured_tx_window_size;
    }

    /*if (rlcP->retransmission_buffer[current_index] != NULL) {
       if (((struct rlc_am_tx_data_pdu_management*)(&rlcP->retransmission_buffer[current_index]->data[sizeof(struct rlc_am_tx_data_pdu_allocation)]))->ack
       == RLC_AM_PDU_NACK_EVENT) {
       #ifdef DEBUG_ACK
       msg("[RLC_AM %p][ACK] CLEAR ACK EVENT PDU SN %04X\n",rlcP, current_sn);
       #endif
       ((struct rlc_am_tx_data_pdu_management*)(&rlcP->retransmission_buffer[current_index]->data[sizeof(struct rlc_am_tx_data_pdu_allocation)]))->ack = RLC_AM_PDU_ACK_NO_EVENT;
       }
       }
     */
    return lsn;

  } else {
#ifdef DEBUG_RESET
    msg ("[RLC_AM][RB %d][ACK] ERROR SUFI ACK INVALID lsn  0x%02X  vt(a) 0x%04X vt(s) 0x%04X -> RESET\n", rlcP->rb_id, lsn, rlcP->vt_a, rlcP->vt_s);
    //display_protocol_vars_rlc_am(rlcP);
#endif
    send_reset_pdu (rlcP);
    return (s16_t) (-1);
  }
}

//-----------------------------------------------------------------------------
/*
 * remove all mem_block_t(s)  correctly received
 * retransmit all mem_block_t(s) not correctly received
 * @param bitmap_sufiP pointer on byte containing field "sufi type"
 * @param byte_alignedP tells if sufi type quartet is on MSByte (1) or LSByte (0)
 */
inline u8_t      *
retransmission_buffer_management_bitmap (struct rlc_am_entity * rlcP, u8_t * bitmap_sufiP, u8 byte_alignedP, s16_t * first_error_indicated_snP)
{
  //-----------------------------------------------------------------------------

  u16_t             id_index;
  u16_t             fsn;
  u8_t              length, tmp;
  u8_t              one_pdu_removed = 0;  //optim: update vt_a only at the end of the proc
  u8_t              bit_mask;
  u8_t              end_bit;


  if (byte_alignedP) {
    // get next 4 bits : length
    length = (*bitmap_sufiP++ & 0x0F) + 1;
    // get next 12 bits : FSN
    fsn = *bitmap_sufiP++;
    fsn = fsn << 4;
    tmp = *bitmap_sufiP;
    fsn += tmp >> 4;
    bit_mask = 8;
  } else {
    bitmap_sufiP++;
    // get 4 bits length
    length = (*bitmap_sufiP >> 4) + 1;
    // get next 12 bits : FSN
    fsn = *bitmap_sufiP++ & 0x0F;
    fsn = fsn << 8;
    fsn = fsn + *bitmap_sufiP;
    bitmap_sufiP++;

    bit_mask = 128;
  }

  end_bit = length << 3;        // *8

#ifdef DEBUG_BITMAP
  msg ("[RLC_AM][RB %d][BITMAP] SUFI BITMAP fsn 0x%04X length 0x%04X\n", rlcP->rb_id, fsn, length);
#endif

  if (rlc_am_comp_sn (rlcP, rlcP->vt_a, fsn, rlcP->vt_a) >= 0) {

    while (end_bit) {

      id_index = fsn % (u16_t) (rlcP->recomputed_configured_tx_window_size);
      // found a matching pdu
      if (*bitmap_sufiP & bit_mask) {
        // pdu correctly received
        // remove it from retransmission buffer
        free_retransmission_buffer (rlcP, id_index);

        one_pdu_removed = 1;
#ifdef DEBUG_BITMAP
        msg ("[RLC_AM][RB %d][BITMAP] SUFI BITMAP  ACK 0x%04x \n", rlcP->rb_id, fsn);
#endif
      } else {
        // pdu not correctly received;
        // retransmit it;
#ifdef DEBUG_BITMAP
        msg ("[RLC_AM][RB %d][BITMAP] SUFI BITMAP NACK 0x%04x\n", rlcP->rb_id, fsn);
#endif

        if (rlcP->retransmission_buffer[id_index]) {
          ((struct rlc_am_tx_data_pdu_management *) (rlcP->retransmission_buffer[id_index]->data))->ack = RLC_AM_PDU_NACK_EVENT;
          retransmit_pdu (rlcP, fsn);
             rlcP->stat_tx_retransmit_pdu_by_status += 1;
/*
          if (*first_error_indicated_snP == -1) {
            *first_error_indicated_snP = fsn;
          }
*/
        }
        if (*first_error_indicated_snP == -1) {
#ifdef DEBUG_BITMAP
          msg ("[RLC_AM][RB %d][BITMAP] FIRST ERROR INDICATED  0x%04x\n", rlcP->rb_id, fsn);
#endif
          *first_error_indicated_snP = fsn;
        }
      }
      // inc searched sequence number
      fsn++;
      fsn = fsn & SN_12BITS_MASK;

      bit_mask = bit_mask >> 1;

      // remaining nb bits to process
      end_bit--;
      if (!bit_mask) {
        bit_mask = 128;
        bitmap_sufiP++;
      }
    }
    return bitmap_sufiP;
  } else {
#ifdef DEBUG_RESET
    msg ("[RLC_AM][RB %d][BITMAP] ERROR SUFI BITMAP INVALID sn 0x%02X  vt(a) 0x%04X vt(s) 0x%04X send RESET requested\n", rlcP->rb_id, fsn, rlcP->vt_a, rlcP->vt_s);
#endif
    display_protocol_vars_rlc_am (rlcP);
    send_reset_pdu (rlcP);
    return bitmap_sufiP;
  }
}

//-----------------------------------------------------------------------------
/*
 * retransmit all mem_block_t(s) not correctly received
 * @param bitmap_sufiP pointer on byte containing field "sufi type"
 * @param byte_alignedP tells if sufi type quartet is on MSByte (1) or LSByte (0)
 */
inline u8_t      *
retransmission_buffer_management_list (struct rlc_am_entity * rlcP, u8_t * bitmap_sufiP, u8 byte_alignedP, s16_t * first_error_indicated_snP)
{
  //-----------------------------------------------------------------------------

  u8_t             *p8;
  u16_t             start_marking_sn;
  u8_t              nb_missing_pdu;
  u8_t              nb_pairs;

  p8 = bitmap_sufiP;


  if (byte_alignedP) {
    nb_pairs = *p8++ & 0X0F;    // number of (SNi, Li) pairs in the sufi type LIST the value 0000
    // is invalid and the list is discarded
    if (!nb_pairs) {
      p8 = p8 + 2;
#ifdef DEBUG_RESET
      msg ("[RLC_AM][RB %d][LIST] ERROR SUFI LIST nb pairs is 0 send RESET requested\n", rlcP->rb_id);
#endif
      send_reset_pdu (rlcP);
#ifdef DEBUG_LIST
      display_protocol_vars_rlc_am (rlcP);
#endif
      return p8;
      return p8;
    } else {
      while (nb_pairs) {
        start_marking_sn = ((u16_t) (*p8++)) << 4;
        start_marking_sn = start_marking_sn | (*p8 >> 4);
        nb_missing_pdu = *p8++ & 0X0F;

        if (*first_error_indicated_snP == -1) {
          *first_error_indicated_snP = start_marking_sn;
#ifdef  DEBUG_LIST
          msg ("[RLC_AM][RB %d][LIST] FIRST ERROR INDICATED 0x%04X VT(A) 0x%04X VT(S) 0x%04X\n", rlcP->rb_id, *first_error_indicated_snP, rlcP->vt_a, rlcP->vt_s);
#endif
        }
#ifdef DEBUG_LIST
        msg ("[RLC_AM][RB %d][LIST] SUFI LIST SN 0x%04X MISSING %d  VT(A) 0x%04X VT(S) 0x%04X\n", rlcP->rb_id, start_marking_sn, nb_missing_pdu, rlcP->vt_a, rlcP->vt_s);
#endif

        while (nb_missing_pdu) {
          if (rlc_am_comp_sn (rlcP, rlcP->vt_a, start_marking_sn, rlcP->vt_a) >= 0) {

            if (rlcP->retransmission_buffer[start_marking_sn % rlcP->recomputed_configured_tx_window_size] != NULL) {

#ifdef DEBUG_LIST
              msg ("[RLC_AM][RB %d][LIST] MARK SN 0x%04X INDEX 0x%04X TO RLC_AM_PDU_NACK_EVENT\n", rlcP->rb_id, start_marking_sn, start_marking_sn % rlcP->recomputed_configured_tx_window_size);
#endif
              ((struct rlc_am_tx_data_pdu_management *) (rlcP->retransmission_buffer[start_marking_sn % rlcP->recomputed_configured_tx_window_size]->data))->ack = RLC_AM_PDU_NACK_EVENT;
              retransmit_pdu (rlcP, start_marking_sn);
             rlcP->stat_tx_retransmit_pdu_by_status += 1;
            }
          } else {
            send_reset_pdu (rlcP);
#ifdef DEBUG_RESET
            msg ("[RLC_AM][RB %d][LIST] ERROR SUFI LIST INVALID SN 0x%02X  VT(A) 0x%04X VT(S) 0x%04X send RESET requested \n", rlcP->rb_id, start_marking_sn, rlcP->vt_a, rlcP->vt_s);
#endif
#ifdef  DEBUG_LIST
            display_protocol_vars_rlc_am (rlcP);
#endif
            return p8;
          }
          start_marking_sn = (start_marking_sn + 1) & SN_12BITS_MASK;
          nb_missing_pdu--;
        }
        nb_pairs--;
      }
      return p8;
    }
  } else {
    p8++;
    nb_pairs = *p8 >> 4;        // number of (SNi, Li) pairs in the sufi type LIST the value 0000
    // is invalid and the list is discarded
    if (!nb_pairs) {
      p8 = p8 + 2;
#ifdef DEBUG_RESET
      msg ("[RLC_AM][RB %d][LIST] ERROR SUFI LIST nb pairs is 0 send RESET requested vt(a) 0x%04X vt(s) 0x%04X\n", rlcP->rb_id, rlcP->vt_a, rlcP->vt_s);
#endif
      send_reset_pdu (rlcP);
#ifdef DEBUG_LIST
      display_protocol_vars_rlc_am (rlcP);
#endif

      return p8;
    } else {
      while (nb_pairs) {
        start_marking_sn = ((u16_t) (*p8++) & 0x0F) << 8;
        start_marking_sn = start_marking_sn | *p8++;
        nb_missing_pdu = *p8 >> 4;

        if (*first_error_indicated_snP == -1) {
          *first_error_indicated_snP = start_marking_sn;
#ifdef  DEBUG_LIST
          msg ("[RLC_AM][RB %d][LIST] FIRST ERROR INDICATED 0x%04X VT(A) 0x%04X VT(S) 0x%04X\n", rlcP->rb_id, *first_error_indicated_snP, rlcP->vt_a, rlcP->vt_s);
#endif
        }
#ifdef  DEBUG_LIST
        msg ("[RLC_AM][RB %d][RETRANSMISSION] SUFI LIST SN 0x%04X  MISSING %d VT(A) 0x%04X VT(S) 0x%04X\n", rlcP->rb_id, start_marking_sn, nb_missing_pdu, rlcP->vt_a, rlcP->vt_s);
#endif
        while (nb_missing_pdu) {
          if (rlc_am_comp_sn (rlcP, rlcP->vt_a, start_marking_sn, rlcP->vt_a) >= 0) {

            if (rlcP->retransmission_buffer[start_marking_sn % rlcP->recomputed_configured_tx_window_size] != NULL) {
#ifdef DEBUG_LIST
              msg ("[RLC_AM][RB %d][LIST] MARK SN 0x%04X INDEX 0x%04X TO RLC_AM_PDU_NACK_EVENT\n", rlcP->rb_id, start_marking_sn, start_marking_sn % rlcP->recomputed_configured_tx_window_size);
#endif
              ((struct rlc_am_tx_data_pdu_management *) (rlcP->retransmission_buffer[start_marking_sn % rlcP->recomputed_configured_tx_window_size]->data))->ack = RLC_AM_PDU_NACK_EVENT;

              retransmit_pdu (rlcP, start_marking_sn);
             rlcP->stat_tx_retransmit_pdu_by_status += 1;
            }
          } else {
            send_reset_pdu (rlcP);
#ifdef DEBUG_RESET
            msg ("[RLC_AM][RB %d][LIST] ERROR SUFI LIST invalid sn  0x%02X send RESET requested vt(a) 0x%04X vt(s) 0x%04X\n", rlcP->rb_id, start_marking_sn, rlcP->vt_a, rlcP->vt_s);
#endif
#ifdef  DEBUG_LIST
            display_protocol_vars_rlc_am (rlcP);
#endif
            return p8;
          }
          start_marking_sn = (start_marking_sn + 1) & SN_12BITS_MASK;
          nb_missing_pdu--;
        }
        nb_pairs--;
      }
      return p8;
    }
  }
}

//-----------------------------------------------------------------------------
inline void
free_retransmission_buffer (struct rlc_am_entity *rlcP, u16_t indexP)
{
//-----------------------------------------------------------------------------

  mem_block_t      *sdu_confirm;
  struct rlc_am_tx_data_pdu_management *pdu;
  mem_block_t      *le = NULL;

  unsigned int    index;
  unsigned int    sdu_index;

  index = indexP;

  if (index < (u16_t) rlcP->recomputed_configured_tx_window_size) {

    le = rlcP->retransmission_buffer[index];

    if (le != NULL) {
      // ???
      pdu = (struct rlc_am_tx_data_pdu_management *) (le->data);

      // test if the sender of the sdu requested a confirmation of the reception of the sdu by the receiving RLC
      // or if mode discard is on
      for (sdu_index = 0; sdu_index < pdu->nb_sdu; sdu_index++) {
        if (pdu->sdu[sdu_index] != -1) {        // may be discarded
          ((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[pdu->sdu[sdu_index]]->data))->nb_pdus_ack += 1;

          if ((((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[pdu->sdu[sdu_index]]->data))->nb_pdus_ack ==
               ((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[pdu->sdu[sdu_index]]->data))->nb_pdus) &&
              (((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[pdu->sdu[sdu_index]]->data))->segmented)) {

            if (((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[pdu->sdu[sdu_index]]->data))->confirm) {
              // send confirmation to upper layers

#ifdef DEBUG_RLC_AM_SEND_CONFIRM
                msg ("[RLC_AM][RB %d][CONFIRM] SDU MUI %d ACK BY PEER\n", rlcP->rb_id, ((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[pdu->sdu[sdu_index]]->data))->mui);
#endif
                rlc_data_conf (0, rlcP->rb_id, ((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[pdu->sdu[sdu_index]]->data))->mui, RLC_TX_CONFIRM_SUCCESSFULL, rlcP->data_plane);
            }
            // FREE SDU : we can remove the sdu if timer based discard and all pdus submitted to lower layers and no confirm running
#ifdef DEBUG_RLC_AM_FREE_SDU
            msg ("[RLC_AM][RB %d][RETRANS] FREE PDU INDEX 0x%04X FREE_SDU INDEX 0x%3X\n", rlcP->rb_id, indexP, pdu->sdu[sdu_index]);
#endif
            free_mem_block (rlcP->input_sdus[pdu->sdu[sdu_index]]);
            rlcP->input_sdus[pdu->sdu[sdu_index]] = NULL;
          }
        }
      }
      // now check if a copy of the pdu is not present in the retransmission_buffer_to_send
      if ((pdu->copy)) {
        list2_remove_element (pdu->copy, &rlcP->retransmission_buffer_to_send);
        free_mem_block (pdu->copy);
      }
#ifdef  DEBUG_BUFFER_RETRANSMISSION
      msg ("[RLC_AM][RB %d][RETRANSMISSION] FREE PDU INDEX=0x%04X %p\n", rlcP->rb_id, index, le);
#endif
      // if this pdu has been retransmitted, remove its size from buffer occupancy
      if (pdu->vt_dat > 0) {
        rlcP->buffer_occupancy_retransmission_buffer -= 1;
      }

      free_mem_block (le);
      rlcP->retransmission_buffer[index] = NULL;
      return;
    }
#ifdef  DEBUG_BUFFER_RETRANSMISSION
    //msg("[RLC_AM %p][RETRANSMISSION] ERROR free_retransmission_buffer() pointer to free is NULL, index in array = %04X\n", rlcP,index);
#endif
    return;
  }
  msg ("[RLC_AM][RB %d][RETRANSMISSION] ERROR free_retransmission_buffer() invalid index in array decimal %d \n", rlcP->rb_id, index);
}

//-----------------------------------------------------------------------------
inline void
free_retransmission_buffer_no_confirmation (struct rlc_am_entity *rlcP, u16_t indexP)
{
//-----------------------------------------------------------------------------
  struct rlc_am_tx_data_pdu_management *pdu;
  mem_block_t      *le = NULL;
  unsigned int    index;
  unsigned int    sdu_index;

  index = indexP;

  if (index < (u16_t) rlcP->recomputed_configured_tx_window_size) {

    le = rlcP->retransmission_buffer[index];

    if (le != NULL) {
      pdu = (struct rlc_am_tx_data_pdu_management *) (le->data);

      // test if the sender of the sdu requested a confirmation of the reception of the sdu by the receiving RLC
      // or if mode discard is on
      for (sdu_index = 0; sdu_index < pdu->nb_sdu; sdu_index++) {
        if (pdu->sdu[sdu_index] != -1) {        // may be discarded
          ((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[pdu->sdu[sdu_index]]->data))->nb_pdus_ack += 1;

          if ((((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[pdu->sdu[sdu_index]]->data))->nb_pdus_ack ==
               ((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[pdu->sdu[sdu_index]]->data))->nb_pdus) &&
              (((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[pdu->sdu[sdu_index]]->data))->segmented)) {

            // FREE SDU : we can remove the sdu if timer based discard and all pdus submitted to lower layers and no confirm running
#ifdef DEBUG_RLC_AM_FREE_SDU
            msg ("[RLC_AM][RB %d][RETRANS] FREE PDU NO_CONF INDEX 0x%04X FREE_SDU INDEX %d\n", rlcP->rb_id, indexP, pdu->sdu[sdu_index]);
#endif
            free_mem_block (rlcP->input_sdus[pdu->sdu[sdu_index]]);
            rlcP->input_sdus[pdu->sdu[sdu_index]] = NULL;
          }
        }
      }
      // now check if a copy of the pdu is not present in the retransmission_buffer_to_send
      if ((pdu->copy)) {
        list2_remove_element (pdu->copy, &rlcP->retransmission_buffer_to_send);
        free_mem_block (pdu->copy);
      }
#ifdef  DEBUG_BUFFER_RETRANSMISSION
      msg ("[RLC_AM][RB %d][RETRANSMISSION] FREE PDU NO_CONF INDEX=0x%04X %p\n", rlcP->rb_id, index, le);
#endif
      // if this pdu has been retransmitted, remove its size from buffer occupancy
      if (pdu->vt_dat > 0) {
        rlcP->buffer_occupancy_retransmission_buffer -= 1;
      }

      free_mem_block (le);
      rlcP->retransmission_buffer[index] = NULL;
      return;
    }
#ifdef  DEBUG_BUFFER_RETRANSMISSION
    //msg("[RLC_AM %p][RETRANSMISSION] ERROR free_retransmission_buffer() pointer to free is NULL, index in array = %04X\n", rlcP,index);
#endif
    return;
  }
  msg ("[RLC_AM][RB %d][RETRANSMISSION] ERROR free_retransmission_buffer() invalid index in array decimal %d \n", rlcP->rb_id, index);
}

//-----------------------------------------------------------------------------
inline void
insert_into_retransmission_buffer (struct rlc_am_entity *rlcP, u16_t indexP, mem_block_t * pduP)
{
//-----------------------------------------------------------------------------
  u16_t             index;
  index = indexP;

  if (index < (u16_t) (rlcP->recomputed_configured_tx_window_size)) {
    if (rlcP->retransmission_buffer[index]) {
#ifdef  DEBUG_BUFFER_RETRANSMISSION
      msg ("[RLC_AM][RB %d][RETRANSMISSION] INSERT PDU ERROR SLOT NOT EMPTY %d=0x%04X\n", rlcP->rb_id, index, index);
      display_retransmission_buffer (rlcP);
#endif
      free_mem_block (rlcP->retransmission_buffer[index]);
    }
    rlcP->retransmission_buffer[index] = pduP;
#ifdef  DEBUG_BUFFER_RETRANSMISSION
    msg ("[RLC_AM][RB %d][RETRANSMISSION] INSERT PDU INDEX=0x%04X %p\n", rlcP->rb_id, indexP, pduP);
#endif
    return;
  }
  free_mem_block (pduP);
  //#ifdef  DEBUG_BUFFER_RETRANSMISSION
  msg ("[RLC_AM][RB %d][RETRANSMISSION] ERROR INSERT RETRANS INVALID INDEX in array decimal %d\n", rlcP->rb_id, index);
  //#endif
}

//-----------------------------------------------------------------------------
inline          u8_t
retransmit_pdu (struct rlc_am_entity * rlcP, u16_t snP)
{
//-----------------------------------------------------------------------------
  mem_block_t      *pdu, *copy;
  struct rlc_am_tx_data_pdu_management *pdu_mngt;
  u16_t             index;

  if ((rlcP->protocol_state & RLC_DATA_TRANSFER_READY_STATE)) {
    index = snP % rlcP->recomputed_configured_tx_window_size;
    pdu = rlcP->retransmission_buffer[index];

    if (pdu) {
      pdu_mngt = (struct rlc_am_tx_data_pdu_management *) (pdu->data);
      pdu_mngt->vt_dat += 1;

      // if first retransmission, add to buffer occupancy
      if (pdu_mngt->vt_dat == 1) {
        rlcP->buffer_occupancy_retransmission_buffer += 1;
      }

      if ((pdu_mngt->vt_dat < rlcP->max_dat) || ((rlcP->sdu_discard_mode & SDU_DISCARD_MODE_TIMER_BASED_EXPLICIT))) {

        // now check if a copy of the pdu is not already present in the retransmission buffer
        if (pdu_mngt->copy == NULL) {
          copy = get_free_mem_block (rlcP->pdu_size + sizeof (struct rlc_am_tx_data_pdu_allocation) + GUARD_CRC_LIH_SIZE);
          if (copy) {
            memcpy (copy->data, pdu->data, rlcP->pdu_size + sizeof (struct rlc_am_tx_data_pdu_allocation));
            list2_add_tail (copy, &rlcP->retransmission_buffer_to_send);
            // make link between the original pdu and its copy
            pdu_mngt->copy = copy;
            ((struct rlc_am_tx_data_pdu_management *) (copy->data))->copy = pdu;


#ifdef BENCH_QOS_L2
            fprintf (bench_l2, "[PDU RETRANS] FRAME %d SN %d VT_DAT %d RLC-AM %p\n", Mac_rlc_xface->frame, snP, pdu_mngt->vt_dat, rlcP);
#endif

#ifdef DEBUG_RETRANSMISSION
            msg ("[RLC_AM][RB %d][RETRANSMISSION] PDU SN = 0x%04X hex VT(DAT) %d  frame %d\n", rlcP->rb_id, snP, pdu_mngt->vt_dat, Mac_rlc_xface->frame);
#endif
            return 0;
          } else {
            msg ("[RLC_AM][RB %d][RETRANSMISSION] ERROR retransmit_pdu() retransmited copy of pdu is NULL index in array %04X\n", rlcP->rb_id, index);
            return RLC_AM_OUT_OF_MEMORY_ERROR;
          }
        } else {
#ifdef DEBUG_RETRANSMISSION
          msg ("[RLC_AM][RB %d][RETRANSMISSION] PDU SN = 0x%04X hex VT(DAT) %d already in retransmission buffer to send\n", rlcP->rb_id, snP, pdu_mngt->vt_dat);
#endif
          return RLC_AM_RETRANS_REQ_PDU_DONE_BEFORE;
        }
      } else {
        if ((rlcP->sdu_discard_mode & SDU_DISCARD_MODE_TIMER_BASED_EXPLICIT)) {
          // DO NOTHING
        } else if ((rlcP->sdu_discard_mode & RLC_SDU_DISCARD_MAX_DAT_RETRANSMISSION)) {
#ifdef DEBUG_RETRANSMISSION
          msg ("[RLC_AM][RB %d][RETRANSMISSION]  VT(DAT) %d REACHED MAX_DAT, DISCARD PROCEDURE WILL BE STARTED frame %d\n", rlcP->rb_id, pdu_mngt->vt_dat, Mac_rlc_xface->frame);
#endif
          rlc_am_discard_notify_max_dat_pdu (rlcP, pdu);
        } else {                // if No_discard after MaxDat number of retransmissions is used, the sender shall initiate the
          // RLC reset procedure when VT(DAT) >= MaxDAT (from 3GPP TS 25.322 V5.0.0 )
          // max transmission reached for this PDU, reset RLC entities
#ifdef DEBUG_RESET
          msg ("[RLC_AM][RB %d][RETRANSMISSION]  VT(DAT) %d REACHED MAX_DAT, RESET frame %d\n", rlcP->rb_id, pdu_mngt->vt_dat, Mac_rlc_xface->frame);
#endif
          send_reset_pdu (rlcP);
        }
        return -1;
      }
    } else {
      return RLC_AM_RETRANS_REQ_PDU_NULL;
    }
  }
  return -1;
}

//-----------------------------------------------------------------------------
void
rlc_am_get_not_acknowledged_pdu_optimized (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------
  u16             vt_dat_min = 255;
  u16             sn;
  u16             sn_min;
  mem_block_t      *pdu;
  struct rlc_am_tx_data_pdu_management *pdu_mngt;

  sn   = rlcP->vt_s;
  sn_min = sn;
  while (sn != rlcP->vt_a){
    sn = (sn - 1) & SN_12BITS_MASK;
    pdu = rlcP->retransmission_buffer[sn % rlcP->recomputed_configured_tx_window_size];
    if ((pdu)) {
      pdu_mngt = (struct rlc_am_tx_data_pdu_management *) (pdu->data);
      if (pdu_mngt->vt_dat < vt_dat_min) {
        sn_min = sn;
        vt_dat_min = pdu_mngt->vt_dat;
      }
    }
  }
  if (rlcP->retransmission_buffer[sn_min % rlcP->recomputed_configured_tx_window_size]) {
      retransmit_pdu (rlcP, sn_min);
      rlcP->stat_tx_retransmit_pdu_unblock += 1;
  }
}


//-----------------------------------------------------------------------------
void
rlc_am_get_not_acknowledged_pdu (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------
  u16_t             sn;
  sn = rlcP->vt_s;
  while (sn != rlcP->vt_a) {
    sn = (sn - 1) & SN_12BITS_MASK;
    if (rlcP->retransmission_buffer[sn % rlcP->recomputed_configured_tx_window_size]) {
      retransmit_pdu (rlcP, sn);
      rlcP->stat_tx_retransmit_pdu_unblock += 1;
      return;
    }
  }
}

//-----------------------------------------------------------------------------
void
rlc_am_get_not_acknowledged_pdu_vt_s_minus_1 (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------
  if (rlcP->retransmission_buffer[((rlcP->vt_s - 1) & SN_12BITS_MASK) % rlcP->recomputed_configured_tx_window_size]) {
    retransmit_pdu (rlcP, (rlcP->vt_s - 1) & SN_12BITS_MASK);
      rlcP->stat_tx_retransmit_pdu_unblock += 1;
  }
}

/***************************************************************************
                          rlc_am_discard_notif.c  -
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
#include "rlc_primitives.h"
#include "rlc_am_status_proto_extern.h"
#include "umts_timer_struct.h"
#include "umts_timer_proto_extern.h"
#include "rlc_am_discard_tx_proto_extern.h"
#include "rlc_am_reset_proto_extern.h"
#include "LAYER2/MAC/extern.h"
//-----------------------------------------------------------------------------
void            rlc_am_discard_notify_mrw_ack_time_out (struct rlc_am_entity *rlcP, mem_block_t * discard_procedureP);
void            rlc_am_discard_notify_sdu_time_out (struct rlc_am_entity *rlcP, mem_block_t * sduP);
void            rlc_am_discard_notify_max_dat_pdu (struct rlc_am_entity *rlcP, mem_block_t * mbP);
//-----------------------------------------------------------------------------
// handler for timer
//-----------------------------------------------------------------------------
void
rlc_am_discard_notify_mrw_ack_time_out (struct rlc_am_entity *rlcP, mem_block_t * discard_procedureP)
{
//-----------------------------------------------------------------------------
  /* from 3GPP TS 25.322 V5.5.0
     If Timer_MRW expires before the discard procedure is terminated, the Sender shall:
     - if VT(MRW)<MaxMRW-1:
     - set the MRW SUFI as previously transmitted (even if additional SDUs were discarded in the mean-time);
     - include the MRW SUFI in a new status report (if other SUFIs are included, their contents shall be updated);
     - transmit the status report by either including it in a STATUS PDU or piggybacked in an AMD PDU;
     - increment VT(MRW) by one;
     - restart Timer_MRW for this discard procedure.

     If the number of retransmission of an MRW SUFI (i.e. VT(MRW)) equals MaxMRW, the Sender shall:
     -  terminate the SDU discard with explicit signalling procedure;
     -  stop the timer Timer_MRW;
     -  deliver an error indication to upper layers;
     -  initiate the RLC RESET procedure */

  rlcP->timer_mrw = NULL;
#ifdef DEBUG_RLC_AM_DISCARD
  msg ("[RLC_AM][RB %d][DISCARD] NOTIF MRW_ACK TIME OUT VT(MRW) %d PROCEDURE %p\n", rlcP->rb_id, rlcP->vt_mrw, discard_procedureP);
#endif

  if ((rlcP->protocol_state & (RLC_RESET_PENDING_STATE | RLC_RESET_AND_SUSPEND_STATE)) == 0) {
    // check the number of retransmission of the status pdu
    if (rlcP->vt_mrw < (rlcP->max_mrw - 1)) {
      rlc_am_schedule_procedure (rlcP);
#ifdef DEBUG_RLC_AM_DISCARD
      msg ("[RLC_AM %p][DISCARD] NOTIF MRW_ACK TIME OUT REARM TIMER\n", rlcP);
#endif
      rlcP->vt_mrw += 1;
    } else {
      // TO DO :  "deliver an error indication to upper layers"
#ifdef DEBUG_RESET
      msg ("\n[RLC_AM][RB %d][DISCARD] NOTIF MRW_ACK TIME OUT MAX_MRW REACHED -> RESET\n", rlcP->rb_id);
#endif
      send_reset_pdu (rlcP);
    }
  }
}

// handler for timer
//-----------------------------------------------------------------------------
void
rlc_am_discard_check_sdu_time_out (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------
  mem_block_t      *sdu;
  mem_block_t      *pdu;
  int             discard_go_on;
  int             last_removed_pdu_sn;
  int             sn;

  // TIMER BASED DISCARD
  if ((rlcP->sdu_discard_mode & RLC_SDU_DISCARD_TIMER_BASED_EXPLICIT) && ((rlcP->protocol_state & (RLC_RESET_PENDING_STATE | RLC_RESET_AND_SUSPEND_STATE)) == 0)) {
    /* from 3GPP TS 25.322 V5.0.0 p68
       - if "Timer based SDU discard with explicit signalling" is configured:
       - discard all SDUs up to and including the SDU for which the timer Timer_Discard expired.
       - discard all AMD PDUs including segments of the discarded SDUs, unless they also carry a segment of a SDU
       whose timer has not expired;
       - if more than 15 discarded SDUs are to be informed to the Receiver:
       - if "Send MRW" is not configured:
       - assemble an MRW SUFI with the discard information of the SDUs.
       - otherwise ("Send MRW" is configured):
       - assemble an MRW SUFI with the discard information of the first 15 SDUs; and
       - include the discard information of the rest SDUs in another MRW SUFI which shall be sent by the next
       SDU discard with explicit signalling procedure (after the current SDU discard with explicit signalling
       procedure is terminated).
       - otherwise (less than or equal to 15 discarded SDUs are to be informed to the Receiver):
       - assemble an MRW SUFI with the discard information of the SDUs.
       - schedule and submit to lower layer a STATUS PDU/piggybacked STATUS PDU containing the MRW SUFI;
       - if SN_MRWLENGTH in the MRW SUFI >VT(S):
       - update VT(S) to SN_MRWLENGTH.
       - start a timer Timer_MRW
       If a new SDU discard with explicit signalling procedure is triggered when the timer Timer_MRW is active, no new
       MRW SUFIs shall be sent before the current SDU discard with explicit signalling procedure is terminated by one of the
       termination criteria.
     */

    discard_go_on = 1;
    last_removed_pdu_sn = -1;

    while ((sdu = rlcP->input_sdus[rlcP->current_sdu_index]) && discard_go_on) {
      if ((*rlcP->frame_tick_milliseconds - ((struct rlc_am_tx_sdu_management *) (sdu->data))->sdu_creation_time) >= rlcP->timer_discard_init) {
        // buffer occupancy is not updated at each generation of pdu, it is only updated for a sdu when the
        // segmentation of this one is finished.
        rlcP->buffer_occupancy -= ((struct rlc_am_tx_sdu_management *) (sdu->data))->sdu_remaining_size;
        rlcP->nb_sdu -= 1;

        if ((rlcP->send_mrw & RLC_AM_SEND_MRW_ON) ||
            // the condition says if the sdu has generated one or more pdus
            (((struct rlc_am_tx_sdu_management *) (sdu->data))->sdu_size != ((struct rlc_am_tx_sdu_management *) (sdu->data))->sdu_remaining_size)) {

          ((struct rlc_am_tx_sdu_management *) (sdu->data))->last_pdu_sn = rlcP->vt_s;
          ((struct rlc_am_tx_sdu_management *) (sdu->data))->no_new_sdu_segmented_in_last_pdu = 1;
          rlcP->vt_s = (rlcP->vt_s + 1) & SN_12BITS_MASK;

          list2_add_tail (sdu, &rlcP->sdu_discarded);
#ifdef DEBUG_RLC_AM_DISCARD
          msg ("[RLC_AM][RB %d] SDU DISCARDED SIGNALLING YES, TIMED OUT %ld ms frame %d ", rlcP->rb_id,
               (*rlcP->frame_tick_milliseconds - ((struct rlc_am_tx_sdu_management *) (sdu->data))->sdu_creation_time), Mac_rlc_xface->frame);
          msg ("BO %d, NB SDU %d\n", rlcP->buffer_occupancy, rlcP->nb_sdu);
          display_protocol_vars_rlc_am (rlcP);
#endif
          if (((struct rlc_am_tx_sdu_management *) (sdu->data))->sdu_size != ((struct rlc_am_tx_sdu_management *) (sdu->data))->sdu_remaining_size) {
            // some pdu have to be removed if a sdu discarded generated almost one pdu
            if (last_removed_pdu_sn == -1) {
              sn = rlcP->vt_a;
            } else {
              sn = last_removed_pdu_sn;
            }
            while (sn != rlcP->vt_s) {
              pdu = rlcP->retransmission_buffer[sn % rlcP->recomputed_configured_tx_window_size];
              if ((pdu)) {

                // now check if a copy of the pdu is not present in the retransmission_buffer_to_send
                if ((((struct rlc_am_tx_data_pdu_management *) (pdu->data))->copy)) {
                  list2_remove_element (((struct rlc_am_tx_data_pdu_management *) (pdu->data))->copy, &rlcP->retransmission_buffer_to_send);
                  free_mem_block (((struct rlc_am_tx_data_pdu_management *) (pdu->data))->copy);
                }
                // if this pdu has been retransmitted, remove its size from buffer occupancy
                if (((struct rlc_am_tx_data_pdu_management *) (pdu->data))->vt_dat > 0) {
                  rlcP->buffer_occupancy_retransmission_buffer -= 1;
                }

                free_mem_block (rlcP->retransmission_buffer[sn % rlcP->recomputed_configured_tx_window_size]);
                rlcP->retransmission_buffer[sn % rlcP->recomputed_configured_tx_window_size] = NULL;
              }
              sn = (sn + 1) & SN_12BITS_MASK;
            }
            last_removed_pdu_sn = sn;
          }
        } else {
#ifdef DEBUG_RLC_AM_DISCARD
          msg ("[RLC_AM][RB %d] SDU DISCARDED SIGNALLING NO, TIMED OUT %ld ms ", rlcP->rb_id, (*rlcP->frame_tick_milliseconds - ((struct rlc_am_tx_sdu_management *) (sdu->data))->sdu_creation_time));
          msg ("BO %d, NB SDU %d\n", rlcP->buffer_occupancy, rlcP->nb_sdu);
#endif
          free_mem_block (sdu);
        }

        if (!(rlcP->data_plane)) {
#ifdef DEBUG_RLC_AM_SEND_CONFIRM
          msg ("[RLC_AM][RB %d][CONFIRM] SDU MUI %d LOST IN DISCARD\n", rlcP->rb_id, ((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[rlcP->current_sdu_index]->data))->mui);
#endif
          rlc_data_conf (0, rlcP->rb_id, ((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[rlcP->current_sdu_index]->data))->mui, RLC_TX_CONFIRM_FAILURE, rlcP->data_plane);
        }

        rlcP->input_sdus[rlcP->current_sdu_index] = NULL;

        rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % rlcP->size_input_sdus_buffer;

        // reset variables for segmentation
        rlcP->li_exactly_filled_to_add_in_next_pdu = 0;
        rlcP->li_one_byte_short_to_add_in_next_pdu = 0;

      } else {
        discard_go_on = 0;
      }
    }
  }
}

//-----------------------------------------------------------------------------
void
rlc_am_discard_notify_max_dat_pdu (struct rlc_am_entity *rlcP, mem_block_t * pduP)
{
//-----------------------------------------------------------------------------

  struct rlc_am_tx_data_pdu_management *pdu_mngt;
  struct rlc_am_tx_sdu_management *sdu_mngt;
  mem_block_t      *pdu;
  mem_block_t      *pdu2;
  mem_block_t      *sdu;
  int             sdu_index;
  int             sdu_index2;
  int             pdu_index;
  int             last_sdu_index;
  int             sn;

  pdu_mngt = (struct rlc_am_tx_data_pdu_management *) pduP->data;

  // should never occur
  //if (pdu_mngt->nb_sdu == 0) return;

  // discard previous SDUS

#ifdef DEBUG_RLC_AM_DISCARD_MAX_DAT
  msg ("[RLC_AM][RB %d] DISCARD  MAX DAT PDU FRAME %d SN 0x%03X CONTAINS SDU INDEX ", rlcP->rb_id, Mac_rlc_xface->frame, pdu_mngt->sn);
  sdu_index = 0;
  while (sdu_index < pdu_mngt->nb_sdu) {
    msg ("%d ", pdu_mngt->sdu[sdu_index]);
    sdu_index += 1;
  }
  msg ("\n");

#endif

  // From 3GPP TS25.322 V5.0.0 (2002-03) page 68
  // -  if "SDU discard after MaxDAT number of retransmissions" is configured:
  //    -       discard all SDUs that have segments in AMD PDUs with SN inside the interval
  //      VT(A) <= SN <= X, where X is the value of the SN of the AMD PDU with VT(DAT) >= MaxDAT;
  //    -       if requested
  //      -     inform the upper layers of the discarded SDUs.

  //---------------------------------------------------------------
  // here delete all SDUs before the last sdu that have segments in the pdu discarded
  sdu_index = rlcP->next_sdu_index;
  last_sdu_index = pdu_mngt->sdu[pdu_mngt->nb_sdu - 1];

  while (sdu_index != last_sdu_index) {
    if ((sdu = rlcP->input_sdus[sdu_index])) {
      list2_add_tail (sdu, &rlcP->sdu_discarded);

      if (!(rlcP->data_plane)) {
#ifdef DEBUG_RLC_AM_SEND_CONFIRM

        msg ("[RLC_AM][RB %d][CONFIRM] SDU MUI %d LOST IN DISCARD\n", rlcP->rb_id, ((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[sdu_index]->data))->mui);
#endif
        rlc_data_conf (0, rlcP->rb_id, ((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[sdu_index]->data))->mui, RLC_TX_CONFIRM_FAILURE, rlcP->data_plane);
      }
      rlcP->input_sdus[sdu_index] = NULL;
      rlcP->nb_sdu -= 1;

#ifdef DEBUG_RLC_AM_FREE_SDU
      msg ("[RLC_AM][RB %d] DISCARD  MAX DAT FREE_SDU INDEX %d\n", rlcP->rb_id, sdu_index);
#endif
    }
    sdu_index = (sdu_index + 1) % rlcP->size_input_sdus_buffer;
  }

  //---------------------------------------------------------------
  // here delete all PDUs up to and except the last of the last sdu discarded
  sdu = rlcP->input_sdus[sdu_index];
  sdu_mngt = (struct rlc_am_tx_sdu_management *) (sdu->data);

  sn = rlcP->vt_a;
  while (sn != sdu_mngt->last_pdu_sn) {
    if ((pdu2 = rlcP->retransmission_buffer[sn % rlcP->recomputed_configured_tx_window_size])) {
#ifdef DEBUG_RLC_AM_DISCARD
      msg ("[RLC_AM][RB %d] DISCARD  FREE PDU SN 0x%03X\n", rlcP->rb_id, sn);
#endif
      // check if a copy of the pdu is not present in the retransmission_buffer_to_send
      if ((((struct rlc_am_tx_data_pdu_management *) (pdu2->data))->copy)) {
#ifdef DEBUG_RLC_AM_DISCARD
        msg ("[RLC_AM][RB %d] FREE  PDU COPY ALSO\n", rlcP->rb_id);
#endif
        list2_remove_element (((struct rlc_am_tx_data_pdu_management *) (pdu2->data))->copy, &rlcP->retransmission_buffer_to_send);
        free_mem_block (((struct rlc_am_tx_data_pdu_management *) (pdu2->data))->copy);
      }
      // if this pdu has been retransmitted, remove its size from buffer occupancy
      if (((struct rlc_am_tx_data_pdu_management *) (pdu2->data))->vt_dat > 0) {
        rlcP->buffer_occupancy_retransmission_buffer -= 1;
      }
      free_mem_block (pdu2);
      rlcP->retransmission_buffer[sn % rlcP->recomputed_configured_tx_window_size] = NULL;
    }
    sn = (sn + 1) & SN_12BITS_MASK;
  }

  //----------------------------------------------
  // Now for the last pdu of the last sdu discarded, check if it contains other segments of
  // sdu newer than the discarded
  // if the pdu cannot be found : it is OK, nothing to do
  pdu = rlcP->retransmission_buffer[sdu_mngt->last_pdu_sn % rlcP->recomputed_configured_tx_window_size];
  if ((pdu)) {
    // search the index of the sdu passed in parameter
    pdu_mngt = (struct rlc_am_tx_data_pdu_management *) pdu->data;
    if (pdu_mngt->sdu[pdu_mngt->nb_sdu - 1] == sdu_index) {
      //----------------------------------------------
      // now check if a copy of the pdu is not present in the retransmission_buffer_to_send
      if ((pdu_mngt->copy)) {
        list2_remove_element (pdu_mngt->copy, &rlcP->retransmission_buffer_to_send);
        free_mem_block (pdu_mngt->copy);
      }
#ifdef DEBUG_RLC_AM_DISCARD
      msg ("[RLC_AM][RB %d] DISCARD  LAST PDU SN 0x%03X\n", rlcP->rb_id, sn);
      rlc_am_display_data_pdu7 (pdu);
#endif
      //----------------
      // discard the pdu
      // if this pdu has been retransmitted, remove its size from buffer occupancy
      if (((struct rlc_am_tx_data_pdu_management *) (pdu->data))->vt_dat > 0) {
        rlcP->buffer_occupancy_retransmission_buffer -= 1;
      }
      pdu_index = pdu_mngt->sn % rlcP->recomputed_configured_tx_window_size;
      free_mem_block (rlcP->retransmission_buffer[pdu_index]);
      rlcP->retransmission_buffer[pdu_index] = NULL;
    } else {
      // if this pdu is not discarded, mark the sdu discarded by writing "-1" for their index : used in retransmission
#ifdef DEBUG_RLC_AM_DISCARD
      msg ("[RLC_AM][RB %d] DISCARD  LAST PDU SN 0x%03X, CONTAINS OTHER SDUS (LAST SDU INDEX=%d): NOT CLEARED\n", rlcP->rb_id, sdu_mngt->last_pdu_sn, pdu_mngt->sdu[pdu_mngt->nb_sdu - 1]);
#endif
      sdu_index2 = 0;
      while (pdu_mngt->sdu[sdu_index2] != sdu_index) {

#ifdef DEBUG_RLC_AM_DISCARD
        msg ("[RLC_AM][RB %d] DISCARD  LAST PDU SN 0x%03X, MARK SDU index % AS DISCARDED\n", rlcP->rb_id, pdu_mngt->sn, pdu_mngt->sdu[sdu_index2]);
#endif
        pdu_mngt->sdu[sdu_index2] = -1;
        sdu_index2 += 1;
      }
#ifdef DEBUG_RLC_AM_DISCARD
      msg ("[RLC_AM][RB %d] DISCARD  LAST PDU SN 0x%03X, MARK SDU index %d AS DISCARDED\n", rlcP->rb_id, pdu_mngt->sn, pdu_mngt->sdu[sdu_index2]);
#endif
      pdu_mngt->sdu[sdu_index2] = -1;
    }
  }
#ifdef DEBUG_RLC_AM_DISCARD
  else {
    msg ("[RLC_AM][RB %d] DISCARD  LAST PDU SN 0x%03X, ALREADY CLEARED\n", rlcP->rb_id, sdu_mngt->last_pdu_sn);
  }
#endif

  //----------------
  // discard the sdu
  if ((rlcP->input_sdus[sdu_index])) {  // may be removed by "free_retransmission_buffer_no_confirmation"

    if (sdu_index == rlcP->current_sdu_index) {
      // sdu under segmentation
      rlcP->buffer_occupancy -= ((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[sdu_index]->data))->sdu_remaining_size;
      ((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[sdu_index]->data))->no_new_sdu_segmented_in_last_pdu = 1;
      rlcP->li_exactly_filled_to_add_in_next_pdu = 0;
      rlcP->li_one_byte_short_to_add_in_next_pdu = 0;
#ifdef DEBUG_RLC_AM_DISCARD
      msg ("[RLC_AM][RB %d] DISCARD  THE SDU DISCARDED WAS UNDER SEGMENTATION (index %d)\n", rlcP->rb_id, sdu_index);
#endif
    }

    list2_add_tail (rlcP->input_sdus[sdu_index], &rlcP->sdu_discarded);
#ifdef DEBUG_RLC_AM_FREE_SDU
    msg ("[RLC_AM][RB %d] DISCARD  MAX DAT FREE_SDU INDEX %d\n", rlcP - rb_id, sdu_index);
#endif
    rlcP->input_sdus[sdu_index] = NULL;
    rlcP->nb_sdu -= 1;
  }
}

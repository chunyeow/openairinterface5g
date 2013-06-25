/*
                                rlc_um.c
                             -------------------

  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr



 ***************************************************************************/
#ifndef USER_MODE
#    define __NO_VERSION__

#    include "rt_compat.h"

#    ifdef RTAI
#        include <rtai.h>
#        include <rtai_posix.h>
#        include <rtai_fifos.h>
#    else
      /* RTLINUX */
#        include <rtl.h>
#        include <time.h>
#        include <rtl_sched.h>
#        include <rtl_sync.h>
#        include <pthread.h>
#        include <rtl_debug.h>
#        include <rtl_core.h>
#        include <rtl_fifo.h>
#    endif

#    include <asm/page.h>

#else
#    include <stdlib.h>
#    include <stdio.h>
#    include <fcntl.h>
#    include <signal.h>
#    include <sys/types.h>
#    include <sys/stat.h>
#    include <pthread.h>
#endif

#include "print.h"
#include "lists_proto_extern.h"
#include "rlc_um_entity.h"
#include "rlc_um_structs.h"
#include "rlc_def.h"
#include "rlc_primitives.h"
#include "mac_primitives.h"
#include "rlc_um_segment_proto_extern.h"
#include "rlc_um_receiver_proto_extern.h"
#include "protocol_vars_extern.h"
#include "debug_l2.h"
#include "LAYER2/MAC/extern.h"
#define DEBUG_RLC_UM_DISPLAY_ASCII_DATA
//-----------------------------------------------------------------------------
u32_t             rlc_um_get_buffer_occupancy (struct rlc_um_entity *rlcP);
void            rlc_um_get_pdus (void *argP);
void            rlc_um_rx (void *argP, struct mac_data_ind data_indP);
struct mac_status_resp rlc_um_mac_status_indication (void *rlcP, u16_t no_tbP, u16 tb_sizeP, struct mac_status_ind tx_statusP);
struct mac_data_req rlc_um_mac_data_request (void *rlcP);
void            rlc_um_mac_data_indication (void *rlcP, struct mac_data_ind data_indP);
void            rlc_um_data_req (void *rlcP, struct mem_block *sduP);
//-----------------------------------------------------------------------------
u32_t
rlc_um_get_buffer_occupancy (struct rlc_um_entity *rlcP)
{
//-----------------------------------------------------------------------------
  return rlcP->buffer_occupancy;
}

//-----------------------------------------------------------------------------
void
rlc_um_get_pdus (void *argP)
{
//-----------------------------------------------------------------------------
  struct rlc_um_entity *rlc = (struct rlc_um_entity *) argP;

  switch (rlc->protocol_state) {

      case RLC_NULL_STATE:
        // from 3GPP TS 25.322 V4.2.0
        // In the NULL state the RLC entity does not exist and therefore it is not possible to transfer any data through it.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating establishment, the RLC entity:
        // -      is created; and
        // -      enters the DATA_TRANSFER_READY state.
        break;

      case RLC_DATA_TRANSFER_READY_STATE:
        // from 3GPP TS 25.322 V4.2.0
        // In the DATA_TRANSFER_READY state, unacknowledged mode data can be exchanged between the entities according to subclause 11.2.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating release, the RLC entity:
        // -      enters the NULL state; and
        // -      is considered as being terminated.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating modification, the RLC entity:
        // -      stays in the DATA_TRANSFER_READY state;
        // -      modifies only the protocol parameters and timers as indicated by upper layers.
        // Upon reception of a CRLC-SUSPEND-Req from upper layers, the RLC entity:
        // -      enters the LOCAL_SUSPEND state.
        // SEND DATA TO MAC
#ifndef NO_THREAD_SAFE
        pthread_mutex_lock (&rlc->mutex_input_buffer);
#endif
        if (rlc->data_pdu_size > 125) {
          rlc_um_segment_15 (rlc);
        } else {
          rlc_um_segment_7 (rlc);
        }
#ifndef NO_THREAD_SAFE
        pthread_mutex_unlock (&rlc->mutex_input_buffer);
#endif
        break;

      case RLC_LOCAL_SUSPEND_STATE:
        // from 3GPP TS 25.322 V4.2.0
        // In the LOCAL_SUSPEND state, the RLC entity is suspended, i.e. it does not send UMD PDUs with SN greater than equal to certain specified value (see subclause 9.7.5).
        // Upon reception of a CRLC-RESUME-Req from upper layers, the RLC entity:
        // -      enters the DATA_TRANSFER_READY state; and
        // -      resumes the data transmission.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating modification, the RLC entity:
        // -      stays in the LOCAL_SUSPEND state;
        // -      modifies only the protocol parameters and timers as indicated by upper layers.

        // TO DO TAKE CARE OF SN : THE IMPLEMENTATION OF THIS FUNCTIONNALITY IS NOT CRITICAL
        break;

      default:
        msg ("[RLC_UM %p] MAC_DATA_REQ UNKNOWN PROTOCOL STATE %02X hex\n", rlc, rlc->protocol_state);
  }
}

//-----------------------------------------------------------------------------
void
rlc_um_rx (void *argP, struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------

  struct rlc_um_entity *rlc = (struct rlc_um_entity *) argP;

  switch (rlc->protocol_state) {

      case RLC_NULL_STATE:
        // from 3GPP TS 25.322 V4.2.0
        // In the NULL state the RLC entity does not exist and therefore it is not possible to transfer any data through it.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating establishment, the RLC entity:
        // -      is created; and
        // -      enters the DATA_TRANSFER_READY state.
        msg ("[RLC_UM_RX %p] ERROR MAC_DATA_IND IN RLC_NULL_STATE\n", argP);
        free_up (&data_indP.data);
        break;

      case RLC_DATA_TRANSFER_READY_STATE:
        // from 3GPP TS 25.322 V4.2.0
        // In the DATA_TRANSFER_READY state, unacknowledged mode data can be exchanged between the entities according to subclause 11.2.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating release, the RLC entity:
        // -      enters the NULL state; and
        // -      is considered as being terminated.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating modification, the RLC entity:
        // -      stays in the DATA_TRANSFER_READY state;
        // -      modifies only the protocol parameters and timers as indicated by upper layers.
        // Upon reception of a CRLC-SUSPEND-Req from upper layers, the RLC entity:
        // -      enters the LOCAL_SUSPEND state.
        data_indP.tb_size = (data_indP.tb_size + 7) >> 3;       // from bits to bytes
        if (data_indP.tb_size <= 125) {
          rlc_um_receive_7 (rlc, data_indP);
        } else {
          rlc_um_receive_15 (rlc, data_indP);
        }
        break;

      case RLC_LOCAL_SUSPEND_STATE:
        // from 3GPP TS 25.322 V4.2.0
        // In the LOCAL_SUSPEND state, the RLC entity is suspended, i.e. it does not send UMD PDUs with SN greater than equal to certain specified value (see subclause 9.7.5).
        // Upon reception of a CRLC-RESUME-Req from upper layers, the RLC entity:
        // -      enters the DATA_TRANSFER_READY state; and
        // -      resumes the data transmission.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating modification, the RLC entity:
        // -      stays in the LOCAL_SUSPEND state;
        // -      modifies only the protocol parameters and timers as indicated by upper layers.
        msg ("[RLC_UM_RX %p] RLC_LOCAL_SUSPEND_STATE\n", argP);
        break;

      default:
        msg ("[RLC_UM_RX][RB %d] TX UNKNOWN PROTOCOL STATE %02X hex\n", rlc->rb_id, rlc->protocol_state);
  }
}

//-----------------------------------------------------------------------------
struct mac_status_resp
rlc_um_mac_status_indication (void *rlcP, u16_t no_tbP, u16 tb_sizeP, struct mac_status_ind tx_statusP)
{
//-----------------------------------------------------------------------------
  struct mac_status_resp status_resp;

  ((struct rlc_um_entity *) rlcP)->nb_pdu_requested_by_mac = no_tbP - ((struct rlc_um_entity *) rlcP)->pdus_to_mac_layer.nb_elements;
  ((struct rlc_um_entity *) rlcP)->data_pdu_size = (tb_sizeP + 7) >> 3;
  ((struct rlc_um_entity *) rlcP)->data_pdu_size_in_bits = tb_sizeP;

  status_resp.buffer_occupancy_in_bytes = rlc_um_get_buffer_occupancy ((struct rlc_um_entity *) rlcP);
  status_resp.buffer_occupancy_in_pdus = status_resp.buffer_occupancy_in_bytes / ((struct rlc_um_entity *) rlcP)->data_pdu_size;
  status_resp.rlc_info.rlc_protocol_state = ((struct rlc_um_entity *) rlcP)->protocol_state;
#ifdef DEBUG_RLC_UM_TX_STATUS
  if (((struct rlc_um_entity *) rlcP)->rb_id > 0) {
    msg ("[RLC_UM][RB %d] MAC_STATUS_INDICATION (DATA) %d TBs -> %d TBs\n", ((struct rlc_um_entity *) rlcP)->rb_id, no_tbP, status_resp.buffer_occupancy_in_pdus);
    if ((tx_statusP.tx_status == MAC_TX_STATUS_SUCCESSFUL) && (tx_statusP.no_pdu)) {
      msg ("[RLC_UM][RB %d] MAC_STATUS_INDICATION  TX STATUS   SUCCESSFUL %d PDUs\n", ((struct rlc_um_entity *) rlcP)->rb_id, tx_statusP.no_pdu);
    }
    if ((tx_statusP.tx_status == MAC_TX_STATUS_UNSUCCESSFUL) && (tx_statusP.no_pdu)) {
      msg ("[RLC_UM][RB %d] MAC_STATUS_INDICATION  TX STATUS UNSUCCESSFUL %d PDUs\n", ((struct rlc_um_entity *) rlcP)->rb_id, tx_statusP.no_pdu);
    }
  }
#endif
  return status_resp;
}

//-----------------------------------------------------------------------------
struct mac_data_req
rlc_um_mac_data_request (void *rlcP)
{
//-----------------------------------------------------------------------------
  struct mac_data_req data_req;

  rlc_um_get_pdus (rlcP);

  init_cnt_up (&data_req.data, NULL);
  add_cnt_up (&((struct rlc_um_entity *) rlcP)->pdus_to_mac_layer, &data_req.data);
#ifdef DEBUG_RLC_STATS
  ((struct rlc_um_entity *) rlcP)->tx_pdus += data_req.data.nb_elements;
#endif

#ifdef DEBUG_RLC_UM_MAC_DATA_REQUEST
  if (((struct rlc_um_entity *) rlcP)->rb_id > 0) {
    // msg ("[RLC_UM][RB %d] MAC_DATA_REQUEST %d TBs\n", ((struct rlc_um_entity *) rlcP)->rb_id, data_req.data.nb_elements);
  }
#endif
  data_req.buffer_occupancy_in_bytes = rlc_um_get_buffer_occupancy ((struct rlc_um_entity *) rlcP);
  data_req.buffer_occupancy_in_pdus = data_req.buffer_occupancy_in_bytes / ((struct rlc_um_entity *) rlcP)->data_pdu_size;
  data_req.rlc_info.rlc_protocol_state = ((struct rlc_um_entity *) rlcP)->protocol_state;
  return data_req;
}

//-----------------------------------------------------------------------------
void
rlc_um_mac_data_indication (void *rlcP, struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------
  rlc_um_rx (rlcP, data_indP);
}

//-----------------------------------------------------------------------------
void
rlc_um_data_req (void *rlcP, struct mem_block *sduP)
{
//-----------------------------------------------------------------------------
  struct rlc_um_entity *rlc = (struct rlc_um_entity *) rlcP;
  u8_t              use_special_li;
  u8_t              insert_sdu = 0;
#ifdef DEBUG_RLC_UM_DISCARD_SDU
  int             index;
#else
#    if DEBUG_RLC_UM_DISPLAY_ASCII_DATA
  int             index;
#    endif
#endif

#ifndef NO_THREAD_SAFE
  pthread_mutex_lock (&rlc->mutex_input_buffer);
#endif

#ifdef DEBUG_RLC_UM_DATA_REQUEST
  //msg ("[RLC_UM][RB  %d] RLC_UM_DATA_REQ size %d Bytes, BO %ld , NB SDU %d current_sdu_index=%d next_sdu_index=%d\n",
  //   rlc->rb_id, ((struct rlc_um_data_req *) (sduP->data))->data_size, rlc->buffer_occupancy, rlc->nb_sdu, rlc->current_sdu_index, rlc->next_sdu_index);
#endif
  if (rlc->input_sdus[rlc->next_sdu_index] == NULL) {
    insert_sdu = 1;
  } else {
    // from 3GPP TS 25.322 V4.2.0
    // If SDU discard has not been configured for an unacknowledged mode RLC entity, SDUs in the
    // transmitter shall not be discarded unless the Transmission buffer is full.
    // When the Transmission buffer in an unacknowledged mode RLC entity is full, the Sender may:
    // -        if segments of the SDU to be discarded have been submitted to lower layer:
    //     -    discard the SDU without explicit signalling according to subclause 11.2.4.3.
    // -        otherwise, if no segments of the SDU to be discarded have been submitted to lower layer:
    //     -    remove the SDU from the Transmission buffer without utilising any of the discard procedures.
    if ((rlc->sdu_discard_mode & RLC_SDU_DISCARD_NOT_CONFIGURED)) {
      if ((rlc->input_sdus[rlc->current_sdu_index])) {
#ifdef DEBUG_RLC_UM_DISCARD_SDU
        msg ("[RLC_UM][RB %d] SDU DISCARDED : BUFFER OVERFLOW, BO %ld , NB SDU %d\n", rlc->rb_id, rlc->buffer_occupancy, rlc->nb_sdu);
#endif
        if (((struct rlc_um_tx_sdu_management *) (rlc->input_sdus[rlc->current_sdu_index]->data))->sdu_remaining_size !=
            ((struct rlc_um_tx_sdu_management *) (rlc->input_sdus[rlc->current_sdu_index]->data))->sdu_size) {
#ifdef DEBUG_RLC_UM_VT_US
          msg ("[RLC_UM][RB %d] Inc VT(US) in rlc_um_data_req()/discarding SDU\n", rlc->rb_id);
#endif
          rlc->vt_us = (rlc->vt_us + 1) & 0x7F;
          rlc->li_one_byte_short_to_add_in_next_pdu = 0;
          rlc->li_exactly_filled_to_add_in_next_pdu = 1;
          rlc->buffer_occupancy -= ((struct rlc_um_tx_sdu_management *) (rlc->input_sdus[rlc->current_sdu_index]->data))->sdu_remaining_size;
        } else {
          rlc->buffer_occupancy -= ((struct rlc_um_tx_sdu_management *) (rlc->input_sdus[rlc->current_sdu_index]->data))->sdu_size;
        }
        rlc->nb_sdu -= 1;
        free_mem_block (rlc->input_sdus[rlc->current_sdu_index]);
        rlc->input_sdus[rlc->current_sdu_index] = NULL;
        insert_sdu = 1;
        rlc->current_sdu_index = (rlc->current_sdu_index + 1) % rlc->size_input_sdus_buffer;
#ifdef DEBUG_RLC_UM_DISCARD_SDU
        msg ("[RLC_UM][RB %d] DISCARD RESULT:\n", rlc->rb_id);
        //msg ("[RLC_UM][RB %d] size input buffer=%d current_sdu_index=%d next_sdu_index=%d\n", rlc->rb_id, rlc->size_input_sdus_buffer, rlc->current_sdu_index, rlc->next_sdu_index);
        for (index = 0; index < rlc->size_input_sdus_buffer; index++) {
          //msg ("[RLC_UM][RB %d] BUFFER[%d]=%p\n", rlc->rb_id, index, rlc->input_sdus[index]);
        }
#endif
      } else {
#ifdef DEBUG_RLC_UM_DISCARD_SDU
        msg ("[RLC_UM][RB %d] DISCARD : BUFFER OVERFLOW ERROR : SHOULD FIND A SDU\n", rlc->rb_id);
        //msg ("[RLC_UM][RB %d] size input buffer=%d current_sdu_index=%d next_sdu_index=%d\n", rlc->rb_id, rlc->size_input_sdus_buffer, rlc->current_sdu_index, rlc->next_sdu_index);
        for (index = 0; index < rlc->size_input_sdus_buffer; index++) {
          msg ("[RLC_UM][rb %d] BUFFER[%d]=%p\n", rlc->rb_id, index, rlc->input_sdus[index]);
        }
#endif
      }
    }
  }
  if ((insert_sdu)) {
#ifdef BENCH_QOS_L2
    fprintf (bench_l2, "[SDU REQUEST] FRAME %d SIZE %d RB %d RLC-UM %p\n", mac_xface->frame, ((struct rlc_um_data_req *) (sduP->data))->data_size, rlc->rb_id, rlc);
#endif
    rlc->input_sdus[rlc->next_sdu_index] = sduP;
    // IMPORTANT : do not change order of affectations
    use_special_li = ((struct rlc_um_data_req *) (sduP->data))->use_special_li;
    ((struct rlc_um_tx_sdu_management *) (sduP->data))->sdu_size = ((struct rlc_um_data_req *) (sduP->data))->data_size;
    ((struct rlc_um_tx_sdu_management *) (sduP->data))->use_special_li = use_special_li;
    rlc->buffer_occupancy += ((struct rlc_um_tx_sdu_management *) (sduP->data))->sdu_size;
    rlc->nb_sdu += 1;
    ((struct rlc_um_tx_sdu_management *) (sduP->data))->first_byte = &sduP->data[sizeof (struct rlc_um_data_req_alloc)];
    ((struct rlc_um_tx_sdu_management *) (sduP->data))->sdu_remaining_size = ((struct rlc_um_tx_sdu_management *) (sduP->data))->sdu_size;
    ((struct rlc_um_tx_sdu_management *) (sduP->data))->sdu_segmented_size = 0;
    ((struct rlc_um_tx_sdu_management *) (sduP->data))->sdu_creation_time = *rlc->frame_tick_milliseconds;
    rlc->next_sdu_index = (rlc->next_sdu_index + 1) % rlc->size_input_sdus_buffer;
#ifdef DEBUG_RLC_UM_DISPLAY_ASCII_DATA
    //msg ("[RLC_UM][RB %d]SDU REQUEST DATA :", rlc->rb_id);
    for (index = 0; index < ((struct rlc_um_tx_sdu_management *) (sduP->data))->sdu_remaining_size; index++) {
      msg ("%02X-", ((struct rlc_um_tx_sdu_management *) (sduP->data))->first_byte[index]);
    }
    msg ("\n");
#endif

  } else {
    free_mem_block (sduP);
  }
#ifndef NO_THREAD_SAFE
  pthread_mutex_unlock (&rlc->mutex_input_buffer);
#endif
}

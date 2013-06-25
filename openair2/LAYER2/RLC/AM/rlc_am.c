/***************************************************************************
                          rlc_am.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr
 ***************************************************************************/
#include "rtos_header.h"
#include "platform_types.h"
#include "platform_constants.h"
//-----------------------------------------------------------------------------
#include "rlc_am_entity.h"
#include "rlc_am_mux_proto_extern.h"
#include "rlc_am_segment_proto_extern.h"
#include "rlc_am_receiver_proto_extern.h"
#include "rlc_am_demux_proto_extern.h"
#include "rlc_am_reset_proto_extern.h"
#include "rlc_am_status_proto_extern.h"
#include "rlc_am_errno.h"
#include "umts_timer_proto_extern.h"
#include "rlc_am_discard_rx_proto_extern.h"
#include "rlc_am_discard_tx_proto_extern.h"
#include "rlc_am_discard_notif_proto_extern.h"
#include "mac_primitives.h"
#include "rlc_primitives.h"
#include "rlc_am_control_primitives_proto_extern.h"
#include "rlc_am_util_proto_extern.h"
#include "list.h"
#include "LAYER2/MAC/extern.h"
//#define DEBUG_RLC_AM_DATA_REQUEST
//#define DEBUG_RLC_AM_TX
//#define DEBUG_RLC_AM_RX
//#define DEBUG_RLC_AM_BO
//-----------------------------------------------------------------------------
void            display_protocol_vars_rlc_am (struct rlc_am_entity *rlcP);
u32_t             rlc_am_get_buffer_occupancy_in_pdus_for_ch1 (struct rlc_am_entity *rlcP);
u32_t             rlc_am_get_buffer_occupancy_in_pdus_for_ch2 (struct rlc_am_entity *rlcP);
void           *rlc_am_tx (void *argP);
void            rlc_am_rx (void *argP, struct mac_data_ind data_indP);
//-----------------------------------------------------------------------------
u32_t
rlc_am_get_buffer_occupancy_in_bytes_ch1 (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------
  u32_t             sum = 0;

  if (rlcP->buffer_occupancy > 0) {
  sum += (rlcP->pdus_to_mac_layer_ch1.nb_elements * rlcP->pdu_size);
  sum += (rlcP->buffer_occupancy_retransmission_buffer * rlcP->pdu_size);
  sum += rlcP->control.nb_elements * rlcP->pdu_size; // TEST LG + HA 23/10/2008
  sum += rlcP->buffer_occupancy;        // approximation because of headers of pdus
  } else {
    if ((rlcP->li_one_byte_short_to_add_in_next_pdu) ||
        (rlcP->li_exactly_filled_to_add_in_next_pdu)) {
        // WARNING LG WE ASSUME TRANSPORT BLOCKS ARE < 125 bytes
        sum = rlcP->pdu_size; // 4 bytes; // so this is the exact size of the next TB to be sent (SN + 2LIs)
    }
  }


#ifdef DEBUG_RLC_AM_BO
  msg ("[RLC_AM][RB %d] DTCH BO: %d bytes \n", rlcP->rb_id,sum);
#endif
  return sum;
}

//-----------------------------------------------------------------------------
u32_t
rlc_am_get_buffer_occupancy_in_bytes_ch2 (struct rlc_am_entity * rlcP)
{
//-----------------------------------------------------------------------------
#ifdef DEBUG_RLC_AM_BO
  msg ("[RLC_AM][RB %d] DCCH BO: CONTROL %d DCCH %d\n", rlcP->rb_id, rlcP->control.nb_elements * rlcP->pdu_size, rlcP->pdus_to_mac_layer_ch2.nb_elements * rlcP->pdu_size);
#endif
  return rlcP->control.nb_elements * rlcP->pdu_size + rlcP->pdus_to_mac_layer_ch2.nb_elements * rlcP->pdu_size;
}

//-----------------------------------------------------------------------------
u32_t
rlc_am_get_buffer_occupancy_in_pdus_ch1 (struct rlc_am_entity * rlcP)
{
//-----------------------------------------------------------------------------
  u32_t             sum = 0;

  sum += rlcP->pdus_to_mac_layer_ch1.nb_elements;
  sum += rlcP->buffer_occupancy_retransmission_buffer;
  sum += rlcP->control.nb_elements; // TEST LG + HA 23/10/2008
  sum += (rlcP->buffer_occupancy / (rlcP->pdu_size - 2));       // minus 2 = size min for pdu header

#ifdef DEBUG_RLC_AM_BO
  msg ("[RLC_AM][RB %d] BO : CH1            %d blocks \n", rlcP->rb_id, rlcP->pdus_to_mac_layer_ch1.nb_elements);
  msg ("[RLC_AM][RB %d] BO : RETRANS BUFFER %d blocks \n", rlcP->rb_id, rlcP->buffer_occupancy_retransmission_buffer);
  msg ("[RLC_AM][RB %d] BO : BUFFER  SDU    %d blocks \n", rlcP->rb_id, rlcP->buffer_occupancy / (rlcP->pdu_size - 2));
#endif
  return sum;
}
//-----------------------------------------------------------------------------
u32_t
rlc_am_get_buffer_occupancy_in_pdus_ch2 (struct rlc_am_entity * rlcP)
{
//-----------------------------------------------------------------------------
#ifdef DEBUG_RLC_AM_BO
  msg ("[RLC_AM][RB %d] BO : CH2 CONTROL    %d DCCH %d\n", rlcP->rb_id, rlcP->control.nb_elements, rlcP->pdus_to_mac_layer_ch2.nb_elements);
#endif
  return rlcP->control.nb_elements + rlcP->pdus_to_mac_layer_ch2.nb_elements;
}

//-----------------------------------------------------------------------------
void
rlc_am_get_pdus (void *argP, u8_t traffic_typeP)
{
//-----------------------------------------------------------------------------

  struct rlc_am_entity *rlc = (struct rlc_am_entity *) argP;

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
        // In the DATA_TRANSFER_READY state, acknowledged mode data can be exchanged between the entities according to subclause 11.3.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating release, the RLC entity:
        // -      enters the NULL state; and
        // -      is considered as being terminated.
        // Upon detection of an initiating condition for the RLC reset procedure described in subclause 11.4.2, the RLC entity:
        // -      initiates the RLC reset procedure (see subclause 11.4); and
        // -      enters the RESET_PENDING state.
        // Upon reception of a RESET PDU, the RLC entity responds according to subclause 11.4.3.
        // Upon reception of a RESET ACK PDU, the RLC entity takes no action.
        // Upon reception of CRLC-SUSPEND-Req from upper layer, the RLC entity is suspended and enters the LOCAL_SUSPEND state.
        // SEND DATA TO MAC

        umts_timer_check_time_out (&rlc->rlc_am_timer_list, *rlc->frame_tick_milliseconds);
        if ((rlc->send_status_pdu_requested)) {
          // may be timer status prohibit
          if (rlc_am_send_status (rlc) > 0) {
          rlc->send_status_pdu_requested = 0;
          }
        } else if ((rlc->timer_status_periodic) && ((Mac_rlc_xface->frame % (rlc->timer_status_periodic / 10)) == 0) && (rlc->last_tx_status_frame != Mac_rlc_xface->frame)) {
          // may be MAC can poll RLC more than once in a time frame
          //msg ("[RLC_AM][RB %d]  SEND STATUS PERIODIC frame %d\n", rlc->rb_id, Mac_rlc_xface->frame);
          if (rlc_am_send_status (rlc) > 0) {
          rlc->send_status_pdu_requested = 0;
          }
         rlc->last_tx_status_frame = Mac_rlc_xface->frame;
        }
#ifdef NODE_MT
        rlc_am_mux_ue (rlc, RLC_AM_TRAFFIC_ALLOWED_FOR_STATUS | RLC_AM_TRAFFIC_ALLOWED_FOR_DATA);
#else
        rlc_am_mux_rg (rlc, RLC_AM_TRAFFIC_ALLOWED_FOR_STATUS | RLC_AM_TRAFFIC_ALLOWED_FOR_DATA);
#endif

        break;

      case RLC_RESET_PENDING_STATE:
        // from 3GPP TS 25.322 V4.2.0
        // In the RESET_PENDING state the entity waits for a response from its peer entity and no data can be exchanged between the entities.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating release, the RLC entity:
        // -      enters the NULL state; and
        // -      is considered as being terminated.
        // Upon reception of a RESET ACK PDU with the same RSN value as in the corresponding RESET PDU, the RLC entity:
        // -      acts according to subclause 11.4.4; and
        // -      enters the DATA_TRANSFER_READY state.
        // Upon reception of a RESET ACK PDU with a different RSN value as in the corresponding RESET PDU, the RLC entity:
        // -      discards the RESET ACK PDU (see subclause 11.4.4); and
        // -      stays in the RESET_PENDING state.
        // Upon reception of a RESET PDU, the RLC entity:
        // -      responds according to subclause 11.4.3; and
        // -      stays in the RESET_PENDING state.
        // Upon reception of CRLC-SUSPEND-Req from upper layer, the RLC entity:
        // -      enters the RESET_AND_SUSPEND state.
        umts_timer_check_time_out (&rlc->rlc_am_timer_list, *rlc->frame_tick_milliseconds);
#ifdef NODE_MT
        rlc_am_mux_ue (rlc, RLC_AM_TRAFFIC_NOT_ALLOWED);
#else
        rlc_am_mux_rg (rlc, RLC_AM_TRAFFIC_NOT_ALLOWED);
#endif
        break;

      case RLC_RESET_AND_SUSPEND_STATE:
        // In the RESET_ AND_SUSPEND state, the entity waits for a response from its peer entity or a primitive (CRLC-RESUME-Req) from its upper layer and no data can be exchanged between the entities.
        // Upon reception of CRLC-CONFIG-Req from upper layer indicating release, the RLC entity:
        // -      enters the NULL state; and
        // -      is considered as being terminated.
        // Upon reception of a RESET ACK PDU with the same RSN value as in the corresponding RESET PDU, the RLC entity:
        // -      acts according to subclause 11.4.4; and
        // -      enters the LOCAL_SUSPEND state.
        // Upon reception of CRLC-RESUME-Req from upper layer in this state, the RLC entity:
        // -      is resumed, i.e. releases the suspend constraint; and
        // -      enters the RESET_PENDING state.
#ifdef NODE_MT
        rlc_am_mux_ue (rlc, RLC_AM_TRAFFIC_NOT_ALLOWED);
#else
        rlc_am_mux_rg (rlc, RLC_AM_TRAFFIC_NOT_ALLOWED);
#endif
        break;

      case RLC_LOCAL_SUSPEND_STATE:
        // from 3GPP TS 25.322 V4.2.0
        // In the LOCAL_SUSPEND state, the RLC entity is suspended, i.e. it does not send AMD PDUs with SN greater than or equal to certain specified value (see subclause 9.7.5).
        // Upon reception of CRLC-RESUME-Req from upper layers in this state, the RLC entity:
        // -      resumes the data transmission; and
        // -      enters the DATA_TRANSFER_READY state.
        // Upon reception of CRLC-CONFIG-Req from upper layers indicating release, the RLC entity:
        // -      enters the NULL state; and
        // -      is considered as being terminated.
        // Upon detection of an initiating condition for RLC reset procedure described in subclause 11.4.2, the RLC entity:
        // -      initiates the RLC reset procedure (see subclause 11.4); and
        // -      enters the RESET_AND_SUSPEND state.

        // TO DO TAKE CARE OF SN : THE IMPLEMENTATION OF THIS FUNCTIONNALITY IS NOT CRITICAL
        if ((rlc->send_status_pdu_requested)) {
          rlc_am_send_status (rlc);
          rlc->send_status_pdu_requested = 0;
        }
#ifdef NODE_MT
        rlc_am_mux_ue (rlc, RLC_AM_TRAFFIC_ALLOWED_FOR_STATUS);
#else
        rlc_am_mux_rg (rlc, RLC_AM_TRAFFIC_ALLOWED_FOR_STATUS);
#endif
        break;

      default:
        msg ("[RLC_AM][RB %d] MAC_DATA_REQ UNKNOWN PROTOCOL STATE 0x%02X\n", rlc->rb_id, rlc->protocol_state);
  }
}

//-----------------------------------------------------------------------------
void
rlc_am_rx (void *argP, struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------

  struct rlc_am_entity *rlc = (struct rlc_am_entity *) argP;

#ifdef DEBUG_RLC_AM_RX
  msg("[RLC][AM] In rlc_am_rx\n");
#endif

  switch (rlc->protocol_state) {

      case RLC_NULL_STATE:
        // from 3GPP TS 25.322 V4.2.0
        // In the NULL state the RLC entity does not exist and therefore it is not possible to transfer any data through it.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating establishment, the RLC entity:
        // -      is created; and
        // -      enters the DATA_TRANSFER_READY state.
        msg ("[RLC_AM %p] ERROR MAC_DATA_IND IN RLC_NULL_STATE\n", argP);
        list_free (&data_indP.data);
        break;

      case RLC_DATA_TRANSFER_READY_STATE:
        // from 3GPP TS 25.322 V4.2.0
        // In the DATA_TRANSFER_READY state, acknowledged mode data can be exchanged between the entities according to subclause 11.3.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating release, the RLC entity:
        // -      enters the NULL state; and
        // -      is considered as being terminated.
        // Upon detection of an initiating condition for the RLC reset procedure described in subclause 11.4.2, the RLC entity:
        // -      initiates the RLC reset procedure (see subclause 11.4); and
        // -      enters the RESET_PENDING state.
        // Upon reception of a RESET PDU, the RLC entity responds according to subclause 11.4.3.
        // Upon reception of a RESET ACK PDU, the RLC entity takes no action.
        // Upon reception of CRLC-SUSPEND-Req from upper layer, the RLC entity is suspended and enters the LOCAL_SUSPEND state.
        if ((rlc->rlc_am_timer_list.head)) {
          umts_timer_check_time_out (&rlc->rlc_am_timer_list, *rlc->frame_tick_milliseconds);
        }
        rlc_am_demux_routing (rlc, RLC_AM_TRAFFIC_ALLOWED_FOR_STATUS | RLC_AM_TRAFFIC_ALLOWED_FOR_DATA, data_indP);
        break;

      case RLC_RESET_PENDING_STATE:
      case RLC_RESET_AND_SUSPEND_STATE:
      case RLC_LOCAL_SUSPEND_STATE:
        // from 3GPP TS 25.322 V4.2.0
        // In the RESET_PENDING state the entity waits for a response from its peer entity and no data can be exchanged between the entities.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating release, the RLC entity:
        // -      enters the NULL state; and
        // -      is considered as being terminated.
        // Upon reception of a RESET ACK PDU with the same RSN value as in the corresponding RESET PDU, the RLC entity:
        // -      acts according to subclause 11.4.4; and
        // -      enters the DATA_TRANSFER_READY state.
        // Upon reception of a RESET ACK PDU with a different RSN value as in the corresponding RESET PDU, the RLC entity:
        // -      discards the RESET ACK PDU (see subclause 11.4.4); and
        // -      stays in the RESET_PENDING state.
        // Upon reception of a RESET PDU, the RLC entity:
        // -      responds according to subclause 11.4.3; and
        // -      stays in the RESET_PENDING state.
        // Upon reception of CRLC-SUSPEND-Req from upper layer, the RLC entity:
        // -      enters the RESET_AND_SUSPEND state.

        // In the RESET_ AND_SUSPEND state, the entity waits for a response from its peer entity or a primitive (CRLC-RESUME-Req) from its upper layer and no data can be exchanged between the entities.
        // Upon reception of CRLC-CONFIG-Req from upper layer indicating release, the RLC entity:
        // -      enters the NULL state; and
        // -      is considered as being terminated.
        // Upon reception of a RESET ACK PDU with the same RSN value as in the corresponding RESET PDU, the RLC entity:
        // -      acts according to subclause 11.4.4; and
        // -      enters the LOCAL_SUSPEND state.
        // Upon reception of CRLC-RESUME-Req from upper layer in this state, the RLC entity:
        // -      is resumed, i.e. releases the suspend constraint; and
        // -      enters the RESET_PENDING state.

        // from 3GPP TS 25.322 V4.2.0
        // In the LOCAL_SUSPEND state, the RLC entity is suspended, i.e. it does not send AMD PDUs with SN greater than or equal to certain specified value (see subclause 9.7.5).
        // Upon reception of CRLC-RESUME-Req from upper layers in this state, the RLC entity:
        // -      resumes the data transmission; and
        // -      enters the DATA_TRANSFER_READY state.
        // Upon reception of CRLC-CONFIG-Req from upper layers indicating release, the RLC entity:
        // -      enters the NULL state; and
        // -      is considered as being terminated.
        // Upon detection of an initiating condition for RLC reset procedure described in subclause 11.4.2, the RLC entity:
        // -      initiates the RLC reset procedure (see subclause 11.4); and
        // -      enters the RESET_AND_SUSPEND state.

        if ((rlc->rlc_am_timer_list.head)) {
          umts_timer_check_time_out (&rlc->rlc_am_timer_list, *rlc->frame_tick_milliseconds);
        }
        rlc_am_demux_routing (rlc, RLC_AM_TRAFFIC_NOT_ALLOWED, data_indP);
        break;

      default:
        msg ("[RLC_AM %p] TX UNKNOWN PROTOCOL STATE 0x%02X\n", rlc, rlc->protocol_state);
  }
}

//-----------------------------------------------------------------------------
struct mac_status_resp
rlc_am_mac_status_indication (void *rlcP, u16_t no_tbP, u16 tb_sizeP, struct mac_status_ind tx_statusP)
{
//-----------------------------------------------------------------------------
  struct mac_status_resp status_resp;
  struct rlc_am_entity *rlc = (struct rlc_am_entity *) rlcP;


  rlc_am_discard_check_sdu_time_out (rlc);

  // call only for knowing if control pdu to transmit
  // special case: no data at startup on this side, but peer entity tx full sspeed
  // peer entity wqit for acks
  rlc_am_get_pdus (rlcP, RLC_AM_TRAFFIC_ALLOWED_FOR_STATUS);

  rlc->nb_pdu_requested_by_mac_on_ch1 = no_tbP;
  //((struct rlc_am_entity*)rlcP)->rlc_pdu_size = tb_sizeP; no modification of pdu_size for RLC AM

  status_resp.buffer_occupancy_in_pdus  = rlc_am_get_buffer_occupancy_in_pdus_ch1 (rlc);
  status_resp.buffer_occupancy_in_pdus += rlc_am_get_buffer_occupancy_in_pdus_ch2 (rlc);
  status_resp.buffer_occupancy_in_pdus += rlc->control.nb_elements;
  status_resp.buffer_occupancy_in_bytes = status_resp.buffer_occupancy_in_pdus * (rlc)->pdu_size;
  status_resp.rlc_info.rlc_protocol_state = (rlc)->protocol_state;
#ifdef DEBUG_RLC_AM_TX
  msg ("[RLC_AM][RB %d] MAC_STATUS_INDICATION %d TBs -> MAC_STATUS_RESPONSE %d TBs FRAME %d\n", rlc->rb_id, no_tbP, status_resp.buffer_occupancy_in_pdus,Mac_rlc_xface->frame);
#endif
  return status_resp;
}

//-----------------------------------------------------------------------------
struct mac_status_resp
rlc_am_mac_status_indication_on_first_channel (void *rlcP, u16_t no_tbP, u16 tb_sizeP, struct mac_status_ind tx_statusP)
{
//-----------------------------------------------------------------------------
  struct mac_status_resp status_resp;

  rlc_am_discard_check_sdu_time_out ((struct rlc_am_entity *) rlcP);
  ((struct rlc_am_entity *) rlcP)->nb_pdu_requested_by_mac_on_ch1 = no_tbP;
  //((struct rlc_am_entity*)rlcP)->rlc_pdu_size = tb_sizeP; no modification of pdu_size for RLC AM


  status_resp.buffer_occupancy_in_pdus = rlc_am_get_buffer_occupancy_in_pdus_ch1 ((struct rlc_am_entity *) rlcP);
  status_resp.buffer_occupancy_in_bytes = status_resp.buffer_occupancy_in_pdus * ((struct rlc_am_entity *) rlcP)->pdu_size;
  status_resp.rlc_info.rlc_protocol_state = ((struct rlc_am_entity *) rlcP)->protocol_state;
#ifdef DEBUG_RLC_AM_TX
  msg ("[RLC_AM][RB %d] CHANNEL 1 MAC_STATUS_INDICATION (DATA) %d TBs -> MAC_STATUS_RESPONSE %d TBs\n", ((struct rlc_am_entity *) rlcP)->rb_id, no_tbP, status_resp.buffer_occupancy_in_pdus);
#endif
  return status_resp;
}

//-----------------------------------------------------------------------------
struct mac_status_resp
rlc_am_mac_status_indication_on_second_channel (void *rlcP, u16_t no_tbP, u16 tb_sizeP, struct mac_status_ind tx_statusP)
{
//-----------------------------------------------------------------------------
  struct mac_status_resp status_resp;

  rlc_am_discard_check_sdu_time_out ((struct rlc_am_entity *) rlcP);

  ((struct rlc_am_entity *) rlcP)->nb_pdu_requested_by_mac_on_ch2 = no_tbP;
  //((struct rlc_am_entity*)rlcP)->rlc_pdu_size = tb_sizeP; no modification of pdu_size for RLC AM

  status_resp.buffer_occupancy_in_pdus = rlc_am_get_buffer_occupancy_in_pdus_ch2 ((struct rlc_am_entity *) rlcP);
  status_resp.buffer_occupancy_in_bytes = status_resp.buffer_occupancy_in_pdus * ((struct rlc_am_entity *) rlcP)->pdu_size;
  status_resp.rlc_info.rlc_protocol_state = ((struct rlc_am_entity *) rlcP)->protocol_state;
#ifdef DEBUG_RLC_AM_TX
  msg ("[RLC_AM][RB %d] CHANNEL 2 MAC_STATUS_INDICATION (CONTROL) %d TBs -> MAC_STATUS_RESPONSE %d TBs\n", ((struct rlc_am_entity *) rlcP)->rb_id, no_tbP, status_resp.buffer_occupancy_in_pdus);
#endif
  return status_resp;
}

//-----------------------------------------------------------------------------
struct mac_data_req
rlc_am_mac_data_request (void *rlcP)
{
//-----------------------------------------------------------------------------
  struct mac_data_req data_req;
  mem_block_t      *pdu;
  signed int nb_pdu_to_transmit = ((struct rlc_am_entity *) rlcP)->nb_pdu_requested_by_mac_on_ch1 ;
  //rlc_am_get_pdus (rlcP, RLC_AM_TRAFFIC_ALLOWED_FOR_STATUS | RLC_AM_TRAFFIC_ALLOWED_FOR_DATA);
  rlc_am_get_pdus (rlcP, RLC_AM_TRAFFIC_ALLOWED_FOR_DATA);

  list_init (&data_req.data, NULL);

  //list_add_list (&((struct rlc_am_entity *) rlcP)->pdus_to_mac_layer_ch1, &data_req.data);
   while (nb_pdu_to_transmit > 0 ) {
      pdu = list_remove_head (&((struct rlc_am_entity *) rlcP)->pdus_to_mac_layer_ch1);
        if (pdu != null) {
         list_add_tail_eurecom (pdu, &data_req.data);
        } else {
          //msg ("[RLC_AM][RB %d] WARNING  MAC_DATA_REQUEST CANNOT GIVE A PDU REQUESTED BY MAC\n", ((struct rlc_am_entity *) rlcP)->rb_id);
        }
      nb_pdu_to_transmit = nb_pdu_to_transmit - 1;
  }
  //list_add_list (&((struct rlc_am_entity *) rlcP)->pdus_to_mac_layer_ch2, &data_req.data);

#ifdef DEBUG_RLC_AM_TX
  msg ("[RLC_AM][RB %d] MAC_DATA_REQUEST %d TBs (REQUESTED %d) Frame %d\n", ((struct rlc_am_entity *) rlcP)->rb_id, data_req.data.nb_elements,  ((struct rlc_am_entity *) rlcP)->nb_pdu_requested_by_mac_on_ch1 , Mac_rlc_xface->frame);
#endif
  data_req.buffer_occupancy_in_pdus = rlc_am_get_buffer_occupancy_in_pdus_ch1 ((struct rlc_am_entity *) rlcP);
  data_req.buffer_occupancy_in_pdus += rlc_am_get_buffer_occupancy_in_pdus_ch2 ((struct rlc_am_entity *) rlcP);
  data_req.buffer_occupancy_in_bytes = data_req.buffer_occupancy_in_pdus * ((struct rlc_am_entity *) rlcP)->pdu_size;
  data_req.rlc_info.rlc_protocol_state = ((struct rlc_am_entity *) rlcP)->protocol_state;
  return data_req;
}

//-----------------------------------------------------------------------------
struct mac_data_req
rlc_am_mac_data_request_on_first_channel (void *rlcP)
{
//-----------------------------------------------------------------------------
  struct mac_data_req data_req;
  // from TS25.322 V4.2.0 p13
  // In case two logical channels are configured in the uplink, AMD PDUs are transmitted
  // on the first logical channel, and control PDUs are transmitted on the second logical
  // channel. In case two logical channels are configured in the downlink, AMD and Control
  // PDUs can be transmitted on any of the two logical channels.
#ifdef NODE_RG
  rlc_am_get_pdus (rlcP, RLC_AM_TRAFFIC_ALLOWED_FOR_STATUS | RLC_AM_TRAFFIC_ALLOWED_FOR_DATA);
#else
  rlc_am_get_pdus (rlcP, RLC_AM_TRAFFIC_ALLOWED_FOR_DATA);
#endif
  list_init (&data_req.data, NULL);
  list_add_list  (&((struct rlc_am_entity *) rlcP)->pdus_to_mac_layer_ch1, &data_req.data);

#ifdef DEBUG_RLC_AM_TX
  msg ("[RLC_AM][RB %d] MAC_DATA_REQUEST (DATA) %d TBs\n", ((struct rlc_am_entity *) rlcP)->rb_id, data_req.data.nb_elements);
#endif
  data_req.buffer_occupancy_in_pdus = rlc_am_get_buffer_occupancy_in_pdus_ch1 ((struct rlc_am_entity *) rlcP);
  data_req.buffer_occupancy_in_bytes = rlc_am_get_buffer_occupancy_in_bytes_ch1 ((struct rlc_am_entity *) rlcP);
  data_req.rlc_info.rlc_protocol_state = ((struct rlc_am_entity *) rlcP)->protocol_state;
  return data_req;
}

//-----------------------------------------------------------------------------
struct mac_data_req
rlc_am_mac_data_request_on_second_channel (void *rlcP)
{
//-----------------------------------------------------------------------------
  struct mac_data_req data_req;

  // from TS25.322 V4.2.0 p13
  // In case two logical channels are configured in the uplink, AMD PDUs are transmitted
  // on the first logical channel, and control PDUs are transmitted on the second logical
  // channel. In case two logical channels are configured in the downlink, AMD and Control
  // PDUs can be transmitted on any of the two logical channels.
#ifdef NODE_RG
  rlc_am_get_pdus (rlcP, RLC_AM_TRAFFIC_ALLOWED_FOR_STATUS | RLC_AM_TRAFFIC_ALLOWED_FOR_DATA);
#else
  rlc_am_get_pdus (rlcP, RLC_AM_TRAFFIC_ALLOWED_FOR_STATUS);
#endif
  list_init (&data_req.data, NULL);
  list_add_list  (&((struct rlc_am_entity *) rlcP)->pdus_to_mac_layer_ch2, &data_req.data);

#ifdef DEBUG_RLC_AM_TX
  msg ("[RLC_AM][RB %d] MAC_DATA_REQUEST (CONTROL) %d TBs\n", ((struct rlc_am_entity *) rlcP)->rb_id, data_req.data.nb_elements);
#endif
  data_req.buffer_occupancy_in_pdus = rlc_am_get_buffer_occupancy_in_pdus_ch2 ((struct rlc_am_entity *) rlcP);
  data_req.buffer_occupancy_in_bytes = rlc_am_get_buffer_occupancy_in_bytes_ch2 ((struct rlc_am_entity *) rlcP);
  data_req.rlc_info.rlc_protocol_state = ((struct rlc_am_entity *) rlcP)->protocol_state;
  return data_req;
}

//-----------------------------------------------------------------------------
void
rlc_am_mac_data_indication (void *rlcP, struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------
  rlc_am_rx (rlcP, data_indP);
}

//-----------------------------------------------------------------------------
void
rlc_am_data_req (void *rlcP, mem_block_t * sduP)
{
//-----------------------------------------------------------------------------
  struct rlc_am_entity *rlc = (struct rlc_am_entity *) rlcP;
  u32_t             mui;
  u16_t             data_offset;
  u16_t             data_size;
  u8_t              conf;


  if ((rlc->input_sdus[rlc->next_sdu_index] == NULL) && (((rlc->next_sdu_index + 1) % rlc->size_input_sdus_buffer) != rlc->current_sdu_index)) {

    rlc->stat_tx_pdcp_sdu += 1;

    rlc->input_sdus[rlc->next_sdu_index] = sduP;
    mui = ((struct rlc_am_data_req *) (sduP->data))->mui;
    data_offset = ((struct rlc_am_data_req *) (sduP->data))->data_offset;
    data_size = ((struct rlc_am_data_req *) (sduP->data))->data_size;
    conf = ((struct rlc_am_data_req *) (sduP->data))->conf;

    ((struct rlc_am_tx_sdu_management *) (sduP->data))->mui = mui;
    ((struct rlc_am_tx_sdu_management *) (sduP->data))->sdu_size = data_size;
    ((struct rlc_am_tx_sdu_management *) (sduP->data))->confirm = conf;

    rlc->buffer_occupancy += ((struct rlc_am_tx_sdu_management *) (sduP->data))->sdu_size;
    rlc->nb_sdu += 1;

    ((struct rlc_am_tx_sdu_management *) (sduP->data))->first_byte = &sduP->data[data_offset];
    ((struct rlc_am_tx_sdu_management *) (sduP->data))->sdu_remaining_size = ((struct rlc_am_tx_sdu_management *) (sduP->data))->sdu_size;
    ((struct rlc_am_tx_sdu_management *) (sduP->data))->sdu_segmented_size = 0;
    ((struct rlc_am_tx_sdu_management *) (sduP->data))->sdu_creation_time = *rlc->frame_tick_milliseconds;
    ((struct rlc_am_tx_sdu_management *) (sduP->data))->nb_pdus = 0;
    ((struct rlc_am_tx_sdu_management *) (sduP->data))->nb_pdus_ack = 0;
    ((struct rlc_am_tx_sdu_management *) (sduP->data))->nb_pdus_time = 0;
    ((struct rlc_am_tx_sdu_management *) (sduP->data))->nb_pdus_internal_use = 0;
    ((struct rlc_am_tx_sdu_management *) (sduP->data))->segmented = 0;
    ((struct rlc_am_tx_sdu_management *) (sduP->data))->discarded = 0;
    ((struct rlc_am_tx_sdu_management *) (sduP->data))->li_index_for_discard = -1;
    ((struct rlc_am_tx_sdu_management *) (sduP->data))->no_new_sdu_segmented_in_last_pdu = 0;
    rlc->next_sdu_index = (rlc->next_sdu_index + 1) % rlc->size_input_sdus_buffer;
#ifdef DEBUG_RLC_AM_DATA_REQUEST
    msg ("[RLC_AM][RB %d] RLC_AM_DATA_REQ size %d Bytes,  NB SDU %d current_sdu_index=%d next_sdu_index=%d conf %d mui %d ",
         rlc->rb_id, data_size, rlc->nb_sdu, rlc->current_sdu_index, rlc->next_sdu_index, conf, mui);
    msg ("BO=%ld Bytes\n", rlc->buffer_occupancy);
#endif
  } else {
#ifdef DEBUG_RLC_AM_DATA_REQUEST
    msg ("[RLC_AM][RB %d] RLC_AM_DATA_REQ BUFFER FULL, NB SDU %d current_sdu_index=%d next_sdu_index=%d size_input_sdus_buffer=%d\n",
         rlc->rb_id, rlc->nb_sdu, rlc->current_sdu_index, rlc->next_sdu_index, rlc->size_input_sdus_buffer);
#endif
    rlc->stat_tx_pdcp_sdu_discarded += 1;
    free_mem_block (sduP);
  }
}

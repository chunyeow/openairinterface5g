/***************************************************************************
                          rlc_am_control_primitives.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "rlc_am_entity.h"
#include "rlc_am_reset_proto_extern.h"
#include "rlc_am_errno.h"
#include "umts_timer_proto_extern.h"
#include "rlc_primitives.h"
#include "rlc_am_fsm_proto_extern.h"
#include "rlc_am_segment_proto_extern.h"
#include "rrm_config_structs.h"
#include "LAYER2/MAC/extern.h"
//-----------------------------------------------------------------------------
void            config_req_rlc_am (struct rlc_am_entity *rlcP, module_id_t module_idP, rlc_am_info_t * config_amP, u8_t rb_idP, rb_type_t rb_typeP);
void            send_rlc_am_control_primitive (struct rlc_am_entity *rlcP, module_id_t module_idP, mem_block_t * cprimitiveP);
void            init_rlc_am (struct rlc_am_entity *rlcP);
void            rlc_am_reset_state_variables (struct rlc_am_entity *rlcP);
void            rlc_am_alloc_buffers_after_establishment (struct rlc_am_entity *rlcP);
void            rlc_am_discard_all_pdus (struct rlc_am_entity *rlcP);
void            rlc_am_stop_all_timers (struct rlc_am_entity *rlcP);
void            rlc_am_free_all_resources (struct rlc_am_entity *rlcP);
void            rlc_am_set_configured_parameters (struct rlc_am_entity *rlcP, mem_block_t * cprimitiveP);
//void            rlc_am_probing_get_buffer_occupancy_measurements (struct rlc_am_entity *rlcP, probing_report_traffic_rb_parameters *reportP, int measurement_indexP);
//-----------------------------------------------------------------------------
/*void
rlc_am_probing_get_buffer_occupancy_measurements (struct rlc_am_entity *rlcP, probing_report_traffic_rb_parameters *reportP, int measurement_indexP)
{
//-----------------------------------------------------------------------------
  if (rlcP->protocol_state != RLC_NULL_STATE) {
      reportP->incoming_sdu[measurement_indexP]   = rlcP->buffer_occupancy;
      reportP->retransmission[measurement_indexP] = rlcP->buffer_occupancy_retransmission_buffer * rlcP->pdu_size;
      reportP->ready_to_send[measurement_indexP]  = (rlcP->pdus_to_mac_layer_ch1.nb_elements + rlcP->pdus_to_mac_layer_ch2.nb_elements) * rlcP->pdu_size;
  }
}*/
//-----------------------------------------------------------------------------
void
config_req_rlc_am (struct rlc_am_entity *rlcP, module_id_t module_idP, rlc_am_info_t * config_amP, u8_t rb_idP, rb_type_t rb_typeP)
{
//-----------------------------------------------------------------------------
  mem_block_t      *mb;

  mb = get_free_mem_block (sizeof (struct crlc_primitive));
  ((struct crlc_primitive *) mb->data)->type = CRLC_CONFIG_REQ;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.e_r = RLC_E_R_ESTABLISHMENT;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.stop = 0;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.cont = 1;

  // timers
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.timer_poll = config_amP->timer_poll;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.timer_poll_prohibit = config_amP->timer_poll_prohibit;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.timer_discard = config_amP->timer_discard;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.timer_poll_periodic = config_amP->timer_poll_periodic;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.timer_status_prohibit = config_amP->timer_status_prohibit;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.timer_status_periodic = config_amP->timer_status_periodic;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.timer_rst = config_amP->timer_rst;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.max_rst = config_amP->max_rst;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.timer_mrw = config_amP->timer_mrw;

  // protocol_parameters
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.pdu_size = config_amP->pdu_size;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.max_dat = config_amP->max_dat;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.poll_pdu = config_amP->poll_pdu;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.poll_sdu = config_amP->poll_sdu;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.poll_window = config_amP->poll_window;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.configured_tx_window_size = config_amP->tx_window_size;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.configured_rx_window_size = config_amP->rx_window_size;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.max_mrw = config_amP->max_mrw;

  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.last_transmission_pdu_poll_trigger = config_amP->last_transmission_pdu_poll_trigger;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.last_retransmission_pdu_poll_trigger = config_amP->last_retransmission_pdu_poll_trigger;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.sdu_discard_mode = config_amP->sdu_discard_mode;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.send_mrw = config_amP->send_mrw;

  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.frame_tick_milliseconds = &mac_xface->frame;

  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.am_parameters.rb_id = rb_idP;
  send_rlc_am_control_primitive (rlcP, module_idP, mb);
  if (rb_typeP != SIGNALLING_RADIO_BEARER) {
    rlcP->data_plane = 1;
    msg ("[RLC AM][RB %d] DATA PLANE\n", rlcP->rb_id);
  } else {
    rlcP->data_plane = 0;
    msg ("[RLC AM][RB %d] CONTROL PLANE\n", rlcP->rb_id);
  }
}
//-----------------------------------------------------------------------------
void
send_rlc_am_control_primitive (struct rlc_am_entity *rlcP, module_id_t module_idP, mem_block_t * cprimitiveP)
{
//-----------------------------------------------------------------------------
  rlcP->module_id = module_idP;
  switch (((struct crlc_primitive *) cprimitiveP->data)->type) {
      case CRLC_CONFIG_REQ:

        switch (((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.e_r) {

            case RLC_E_R_ESTABLISHMENT:
              if (rlc_am_fsm_notify_event (rlcP, RLC_AM_RECEIVE_CRLC_CONFIG_REQ_ENTER_DATA_TRANSFER_READY_STATE_EVENT)) {
                rlc_am_set_configured_parameters (rlcP, cprimitiveP);   // the order of the calling of procedures...
                rlc_am_reset_state_variables (rlcP);    // ...must not ...
                rlc_am_alloc_buffers_after_establishment (rlcP);        // ...be changed
              }
              break;

            case RLC_E_R_RE_ESTABLISHMENT:
              // from 3GPP TS 25.322 V4.2.0(2001-09)
              // The RLC re-establishment function is applicable for AM and UM and is used when upper layers request an RLC entity to be re-established.
              // When an RLC entity is re-established by upper layers, the RLC entity shall:
              // -   reset the state variables to their initial value;
              // -   set the configurable parameters to their configured value;
              // -   set the hyper frame number (HFN) in UL and DL to the value configured by upper layers;
              // -   if the RLC entity is operating in unacknowledged mode:
              // -   if it is a receiving UM RLC entity:
              // -   discard all UMD PDUs;
              // -   if it is a transmitting UM RLC entity:
              // -   discard the RLC SDUs for which one or more segments have been submitted to a lower layer;
              // -   otherwise if the RLC entity is operating in acknowledged mode:
              // -   discard all AMD PDUs in the Receiver and Sender.
              if (rlc_am_fsm_notify_event (rlcP, RLC_AM_RECEIVE_CRLC_CONFIG_REQ_ENTER_DATA_TRANSFER_READY_STATE_EVENT)) {
                rlc_am_free_all_resources (rlcP);
                rlc_am_set_configured_parameters (rlcP, cprimitiveP);   // the order of the calling of procedures...
                rlc_am_reset_state_variables (rlcP);    // ...must not ...
                rlc_am_alloc_buffers_after_establishment (rlcP);        // ...be changed
              }
              break;

            case RLC_E_R_MODIFICATION:
              msg ("[RLC_AM][RB %d] WARNING CRLC_CONFIG_REQ:RLC_AM_E_R_MODIFICATION NOT IMPLEMENTED AT ALL: MAY BE BUGGY IF CHANGING WINDOWS SIZE\n", rlcP->rb_id);
              rlc_am_set_configured_parameters (rlcP, cprimitiveP);
              break;

            case RLC_E_R_RELEASE:
              if (rlc_am_fsm_notify_event (rlcP, RLC_AM_RECEIVE_CRLC_CONFIG_REQ_ENTER_NULL_STATE_EVENT)) {
                rlc_am_free_all_resources (rlcP);
              }
              break;

            default:
              msg ("[RLC_AM][ERROR] control_rlc_am(CRLC_CONFIG_REQ) unknown parameter E_R\n");
        }
        break;

      case CRLC_SUSPEND_REQ:
        // from 3GPP TS 25.322 V4.2.0(2001-09)
        // When an RLC entity operating in acknowledged mode is suspended by upper layers with the parameter N, the RLC entity shall:
        // -      acknowledge the suspend request with a confirmation containing the current value of VT(S);
        // -      not send AMD PDUs with sequence number SNVT(S)+N.
        if (rlc_am_fsm_notify_event (rlcP, RLC_AM_RECEIVE_CRLC_SUSPEND_REQ_EVENT)) {
          if ((rlcP->protocol_state == RLC_LOCAL_SUSPEND_STATE) || (rlcP->protocol_state == RLC_LOCAL_SUSPEND_STATE)) {

            // TO DO IN FUTURE, BUT IT SEEMS THERE IS NO NEED TO IMPLEMENT THIS FUNCTION IN THE CURRENTLY DEFINED ARCHITECTURE
            // SO DISPLAY ERROR MESSAGE
            msg ("[RLC_AM[RB %d] ERROR RECEIVED CRLC_SUSPEND_REQ NOT IMPLEMENTED AT ALL, RLC_AM MAY NOT WORK AS EXPECTED IN THE 3GPP SPECIFICATION\n", rlcP->rb_id);
          }
        }
        break;

      case CRLC_RESUME_REQ:
        // from 3GPP TS 25.322 V4.2.0(2001-09)
        // When an RLC entity operating in acknowledged mode is resumed by upper layers, the RLC entity shall:
        // -      if the RLC entity is suspended and a RLC Reset procedure is not ongoing:
        // -      resume data transfer procedure.
        // -      otherwise, if the RLC entity is suspended and a RLC Reset procedure is ongoing:
        // -      remove the suspend constraint;
        // -      resume the RLC reset procedure according to subclause 11.4.
        if (rlc_am_fsm_notify_event (rlcP, RLC_AM_RECEIVE_CRLC_RESUME_REQ_EVENT)) {
          if ((rlcP->protocol_state == RLC_RESET_PENDING_STATE) || (rlcP->protocol_state == RLC_DATA_TRANSFER_READY_STATE)) {

            // TO DO IN FUTURE, BUT IT SEEMS THERE IS NO NEED TO IMPLEMENT THIS FUNCTION IN THE CURRENTLY DEFINED ARCHITECTURE
            // SO DISPLAY ERROR MESSAGE
            msg ("[RLC_AM][RB %d] ERROR RECEIVED CRLC_RESUME_REQ NOT IMPLEMENTED AT ALL, RLC_AM MAY NOT WORK AS EXPECTED IN THE 3GPP SPECIFICATION\n", rlcP->rb_id);
          }
        }
        break;
      default:
        msg ("[RLC_AM][RB %d][ERROR] control_rlc_am(UNKNOWN CPRIMITIVE)\n", rlcP->rb_id);
  }
  free_mem_block (cprimitiveP);
}

//-----------------------------------------------------------------------------
void
init_rlc_am (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------
  memset (rlcP, 0, sizeof (struct rlc_am_entity));


  list2_init (&rlcP->retransmission_buffer_to_send, NULL);
  list_init  (&rlcP->pdus_to_mac_layer_ch1, NULL);
  list_init  (&rlcP->pdus_to_mac_layer_ch2, NULL);
  list_init  (&rlcP->pdus_from_mac_layer_ch1, NULL);
  list_init  (&rlcP->pdus_from_mac_layer_ch2, NULL);
  list_init  (&rlcP->control, NULL);
  list2_init (&rlcP->sdu_conf_segmented, NULL);
  list2_init (&rlcP->sdu_discard_segmented, NULL);
  list2_init (&rlcP->sdu_discarded, NULL);
  list2_init (&rlcP->rlc_am_timer_list, NULL);

  rlcP->protocol_state = RLC_NULL_STATE;

  rlcP->next_sdu_index = 0;
  rlcP->current_sdu_index = 0;
  rlcP->last_received_rsn = 0xFF;

  //--------------------------------------------------------------------------
  rlcP->discard_reassembly_after_li = RLC_AM_DISCARD_REASSEMBLY_AT_LI_INDEX_0;
  rlcP->discard_reassembly_start_sn = RLC_AM_SN_INVALID;        // =>not activated

  rlcP->stat_tx_pdcp_sdu                           = 0;
  rlcP->stat_tx_pdcp_sdu_discarded           = 0;
  rlcP->stat_tx_retransmit_pdu_unblock      = 0;
  rlcP->stat_tx_retransmit_pdu_by_status   = 0;
  rlcP->stat_tx_data_pdu                           = 0;
  rlcP->stat_tx_control_pdu                        = 0;
  rlcP->stat_rx_sdu                                   = 0;
  rlcP->stat_rx_error_pdu                          = 0;
  rlcP->stat_rx_data_pdu                           = 0;
  rlcP->stat_rx_data_pdu_out_of_window    = 0;
  rlcP->stat_rx_control_pdu                        = 0;
}

//-----------------------------------------------------------------------------
void
rlc_am_alloc_buffers_after_establishment (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------
  rlcP->recomputed_configured_tx_window_size = 1;
  while (rlcP->recomputed_configured_tx_window_size < rlcP->configured_tx_window_size) {
    rlcP->recomputed_configured_tx_window_size = rlcP->recomputed_configured_tx_window_size << 1;
  }
  rlcP->retransmission_buffer_alloc = get_free_mem_block (rlcP->recomputed_configured_tx_window_size * sizeof (mem_block_t *));
  rlcP->retransmission_buffer = (mem_block_t **) (rlcP->retransmission_buffer_alloc->data);
  //memset (rlcP->retransmission_buffer, 0, rlcP->recomputed_configured_tx_window_size * sizeof (mem_block_t *));

  rlcP->recomputed_configured_rx_window_size = 1;
  while (rlcP->recomputed_configured_rx_window_size < rlcP->configured_rx_window_size) {
    rlcP->recomputed_configured_rx_window_size = rlcP->recomputed_configured_rx_window_size << 1;
  }
  rlcP->receiver_buffer_alloc = get_free_mem_block (rlcP->recomputed_configured_rx_window_size * sizeof (mem_block_t *));
  rlcP->receiver_buffer = (mem_block_t **) (rlcP->receiver_buffer_alloc->data);
  msg("[RLC AM][RB %d] Window size %d\n",rlcP->rb_id,rlcP->recomputed_configured_rx_window_size);
  //memset (rlcP->receiver_buffer, 0, rlcP->recomputed_configured_rx_window_size * sizeof (mem_block_t *));

  rlcP->holes_alloc = get_free_mem_block ((rlcP->recomputed_configured_rx_window_size * sizeof (struct rlc_am_hole)) >> 1);
  rlcP->holes = (struct rlc_am_hole *) (rlcP->holes_alloc->data);
  //memset (rlcP->holes_alloc->data, 0, (rlcP->recomputed_configured_rx_window_size * sizeof (mem_block_t *)) >> 1);

  rlcP->size_input_sdus_buffer = rlcP->recomputed_configured_tx_window_size * 4;
  if ((rlcP->input_sdus_alloc == NULL)) {
    rlcP->input_sdus_alloc = get_free_mem_block (rlcP->size_input_sdus_buffer * sizeof (void *));
    rlcP->input_sdus = (mem_block_t **) (rlcP->input_sdus_alloc->data);
    //memset (rlcP->input_sdus, 0, rlcP->size_input_sdus_buffer * sizeof (void *));
  }
}

//-----------------------------------------------------------------------------
void
rlc_am_reset_state_variables (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------
  rlcP->vt_s = 0;
  rlcP->vt_a = 0;
  rlcP->vt_pdu = 0;
  //................................................................
  rlcP->vt_sdu = 0;
  rlcP->vt_mrw = 0;
  rlcP->vt_ms = rlcP->configured_tx_window_size - 1;
  //................................................................
  rlcP->vt_ws = rlcP->configured_tx_window_size;
  rlcP->vr_r = 0;
  rlcP->vr_h = 0;
  //................................................................
  rlcP->vr_mr = rlcP->configured_rx_window_size - 1;
  rlcP->vt_rst = 0;
  //................................................................
  rlcP->send_status_pdu_requested = 0;
  rlcP->first_li_in_next_pdu = RLC_LI_UNDEFINED;
  rlcP->last_reassemblied_sn = SN_12BITS_MASK;
  rlcP->li_exactly_filled_to_add_in_next_pdu = 0;
  rlcP->li_one_byte_short_to_add_in_next_pdu = 0;
  //................................................................
  rlcP->sufi_to_insert_index = 0;
  rlcP->ack.vr_r_modified = 0;
  rlcP->ack.ack_other_vr_r = 0;
  //................................................................
  rlcP->output_sdu_size_to_write = 0;
  rlcP->discard_reassembly_after_li = RLC_AM_DISCARD_REASSEMBLY_AT_LI_INDEX_0;
  rlcP->discard_reassembly_start_sn = RLC_AM_SN_INVALID;        // =>not activated
  //................................................................
  rlcP->timer_mrw = NULL;
  //................................................................
  rlcP->buffer_occupancy = 0;
  rlcP->nb_sdu = 0;
  rlcP->buffer_occupancy_retransmission_buffer = 0;
  rlcP->next_sdu_index = 0;
  rlcP->current_sdu_index = 0;
  rlcP->nb_pdu_requested_by_mac_on_ch1 = 0;
  rlcP->nb_pdu_requested_by_mac_on_ch2 = 0;
  
  rlcP->running_timer_status_prohibit = rlcP->timer_status_prohibit/10;
}

//-----------------------------------------------------------------------------
void
rlc_am_stop_all_timers (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------
  list2_free (&rlcP->rlc_am_timer_list);
}

//-----------------------------------------------------------------------------
void
rlc_am_discard_all_pdus (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------
  u16_t             index;

  index = 0;
  while (index < rlcP->recomputed_configured_rx_window_size) {
    if (rlcP->receiver_buffer[index] != NULL) {
      free_mem_block (rlcP->receiver_buffer[index]);
      rlcP->receiver_buffer[index] = NULL;
    }
    index++;
  }
  index = 0;
  while (index < rlcP->recomputed_configured_tx_window_size) {
    if (rlcP->retransmission_buffer[index] != NULL) {
      free_mem_block (rlcP->retransmission_buffer[index]);
      rlcP->retransmission_buffer[index] = NULL;
    }
    index++;
  }
  list2_free (&rlcP->retransmission_buffer_to_send);
  list_free (&rlcP->pdus_to_mac_layer_ch1);
  list_free (&rlcP->pdus_to_mac_layer_ch2);
  list_free (&rlcP->control);
  if ((rlcP->output_sdu_in_construction)) {
    free_mem_block (rlcP->output_sdu_in_construction);
    rlcP->output_sdu_in_construction = NULL;
  }
}

//-----------------------------------------------------------------------------
void
rlc_am_free_all_resources (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------
  int             index;
  for (index = 0; index < rlcP->size_input_sdus_buffer; index++) {
    if (rlcP->input_sdus[index]) {
      free_mem_block (rlcP->input_sdus[index]);
      rlcP->input_sdus[index] = NULL;
    }
  }

  rlc_am_discard_all_pdus (rlcP);
  if (rlcP->retransmission_buffer_alloc) {
    free_mem_block (rlcP->retransmission_buffer_alloc);
  }
  if (rlcP->receiver_buffer_alloc) {
    free_mem_block (rlcP->receiver_buffer_alloc);
  }

  if (rlcP->input_sdus_alloc) {
    free_mem_block (rlcP->input_sdus_alloc);
  }
  if (rlcP->holes_alloc) {
    free_mem_block (rlcP->holes_alloc);
  }
  rlcP->holes_alloc = NULL;
  rlcP->input_sdus_alloc = NULL;
  rlcP->receiver_buffer_alloc = NULL;
  rlcP->retransmission_buffer_alloc = NULL;
  rlc_am_stop_all_timers (rlcP);
}

//-----------------------------------------------------------------------------
void
rlc_am_set_configured_parameters (struct rlc_am_entity *rlcP, mem_block_t * cprimitiveP)
{
//-----------------------------------------------------------------------------
  // timers
  //rlcP->timer_poll_trigger = ((struct crlc_primitive *)cprimitiveP->data)->cprimitive.c_config_req.parameters.am_parameters.timer_poll;
  //rlcP-> = ((struct crlc_primitive *)cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.timer_poll_prohibit;
  rlcP->timer_discard_init = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.timer_discard;
  rlcP->timer_poll_periodic_init = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.timer_poll_periodic;
  rlcP-> timer_status_prohibit    = ((struct crlc_primitive *)cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.timer_status_prohibit;
  rlcP->timer_status_periodic = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.timer_status_periodic;
  rlcP->timer_rst_init = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.timer_rst;
  rlcP->max_rst = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.max_rst;
  rlcP->timer_mrw_init = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.timer_mrw;

  // protocol_parameters
  rlcP->pdu_size = (((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.pdu_size + 7) >> 3;
  rlcP->max_dat = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.max_dat;
  rlcP->missing_pdu_indicator = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.missing_pdu_indicator;
  rlcP->poll_pdu_trigger = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.poll_pdu;
  rlcP->poll_sdu_trigger = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.poll_sdu;
  rlcP->last_transmission_pdu_poll_trigger = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.last_transmission_pdu_poll_trigger;
  rlcP->last_retransmission_pdu_poll_trigger = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.last_retransmission_pdu_poll_trigger;
  rlcP->poll_window_trigger = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.poll_window;
  rlcP->configured_tx_window_size = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.configured_tx_window_size;
  rlcP->configured_rx_window_size = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.configured_rx_window_size;
  rlcP->max_mrw = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.max_mrw;
  rlcP->sdu_discard_mode = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.sdu_discard_mode;
  rlcP->send_mrw = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.send_mrw;

  // SPARE : not 3GPP
  rlcP->rb_id = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.rb_id;
  rlcP->frame_tick_milliseconds = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.am_parameters.frame_tick_milliseconds;
  rlcP->nb_logical_channels_per_rlc =1;

  if (rlcP->pdu_size > 126) {
    rlcP->rlc_segment = rlc_am_segment_15;
  } else {
    rlcP->rlc_segment = rlc_am_segment_7;
  }
  if (rlcP->sdu_discard_mode == SDU_DISCARD_MODE_RESET) {
    msg ("[RLC AM][RB %d] SDU DISCARD RESET CONFIGURED\n", rlcP->rb_id);
  } else if (rlcP->sdu_discard_mode == SDU_DISCARD_MODE_TIMER_BASED_EXPLICIT) {
    msg ("[RLC AM][RB %d] SDU DISCARD TIMER BASED EXPLICIT SIGNALING CONFIGURED\n", rlcP->rb_id);
  } else if (rlcP->sdu_discard_mode == SDU_DISCARD_MODE_MAX_DAT_RETRANSMISSION) {
    msg ("[RLC AM][RB %d] SDU DISCARD MAX RETRANSMISSION CONFIGURED\n", rlcP->rb_id);
  } else if (rlcP->sdu_discard_mode == SDU_DISCARD_MODE_NOT_CONFIGURED) {
    msg ("[RLC AM][RB %d] SDU DISCARD NOT CONFIGURED\n", rlcP->rb_id);
  }

}

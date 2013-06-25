/***************************************************************************
                          rlc_um_control_primitives.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "rlc_um_entity.h"
#include "rlc_um_constants.h"
#include "rlc_um_structs.h"
#include "rlc_primitives.h"
#include "rlc_um_fsm_proto_extern.h"
#include "list.h"
#include "rrm_config_structs.h"
#include "LAYER2/MAC/extern.h"
//-----------------------------------------------------------------------------
void            config_req_rlc_um (struct rlc_um_entity *rlcP, module_id_t module_idP, rlc_um_info_t * config_umP, u8_t rb_idP, rb_type_t rb_typeP);
void            send_rlc_um_control_primitive (struct rlc_um_entity *rlcP, module_id_t module_idP, mem_block_t *cprimitiveP);
void            init_rlc_um (struct rlc_um_entity *rlcP);
void            rlc_um_reset_state_variables (struct rlc_um_entity *rlcP);
void            rlc_um_free_all_resources (struct rlc_um_entity *rlcP);
void            rlc_um_set_configured_parameters (struct rlc_um_entity *rlcP, mem_block_t *cprimitiveP);
//void            rlc_um_probing_get_buffer_occupancy_measurements (struct rlc_um_entity *rlcP, probing_report_traffic_rb_parameters *reportP, int measurement_indexP);
//-----------------------------------------------------------------------------
/*void
rlc_um_probing_get_buffer_occupancy_measurements (struct rlc_um_entity *rlcP, probing_report_traffic_rb_parameters *reportP, int measurement_indexP)
{
//-----------------------------------------------------------------------------
  if (rlcP->protocol_state != RLC_NULL_STATE) {
      reportP->incoming_sdu[measurement_indexP]   = rlcP->buffer_occupancy;
      reportP->retransmission[measurement_indexP] = 0;
      reportP->ready_to_send[measurement_indexP]  = 0;
  }
}*/
//-----------------------------------------------------------------------------
void
config_req_rlc_um (struct rlc_um_entity *rlcP, module_id_t module_idP, rlc_um_info_t * config_umP, u8_t rb_idP, rb_type_t rb_typeP)
{
//-----------------------------------------------------------------------------
  mem_block_t *mb;

  mb = get_free_mem_block (sizeof (struct crlc_primitive));
  ((struct crlc_primitive *) mb->data)->type = CRLC_CONFIG_REQ;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.um_parameters.e_r  = RLC_E_R_ESTABLISHMENT;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.um_parameters.stop = 0;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.um_parameters.cont = 1;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.um_parameters.timer_discard    = config_umP->timer_discard;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.um_parameters.sdu_discard_mode = config_umP->sdu_discard_mode;

  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.um_parameters.frame_tick_milliseconds = &Mac_rlc_xface->frame;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.um_parameters.size_input_sdus_buffer = 256;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.um_parameters.rb_id = rb_idP;
  send_rlc_um_control_primitive (rlcP, module_idP, mb);
  if (rb_typeP != SIGNALLING_RADIO_BEARER) {
    rlcP->data_plane = 1;
  } else {
    rlcP->data_plane = 0;
  }
}
//-----------------------------------------------------------------------------
void
send_rlc_um_control_primitive (struct rlc_um_entity *rlcP, module_id_t module_idP, mem_block_t *cprimitiveP)
{
//-----------------------------------------------------------------------------

  switch (((struct crlc_primitive *) cprimitiveP->data)->type) {

      case CRLC_CONFIG_REQ:
       

        switch (((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.um_parameters.e_r) {
            case RLC_E_R_ESTABLISHMENT:
              rlcP->module_id = module_idP;
              if (rlc_um_fsm_notify_event (rlcP, RLC_UM_RECEIVE_CRLC_CONFIG_REQ_ENTER_DATA_TRANSFER_READY_STATE_EVENT)) {
                rlc_um_set_configured_parameters (rlcP, cprimitiveP);   // the order of the calling of procedures...
                rlc_um_reset_state_variables (rlcP);    // ...must not ...
              }
              break;

            case RLC_E_R_MODIFICATION:
              msg ("[RLC_UM_LITE][ERROR] send_rlc_um_control_primitive(CRLC_CONFIG_REQ) RLC_AM_E_R_MODIFICATION not handled\n");
              break;

            case RLC_E_R_RELEASE:
              if (rlc_um_fsm_notify_event (rlcP, RLC_UM_RECEIVE_CRLC_CONFIG_REQ_ENTER_NULL_STATE_EVENT)) {
                rlc_um_free_all_resources (rlcP);
              }
              break;

            default:
              msg ("[RLC_UM_LITE][ERROR] send_rlc_um_control_primitive(CRLC_CONFIG_REQ) unknown parameter E_R\n");
        }
        break;

      case CRLC_RESUME_REQ:
        msg ("[RLC_UM_LITE][ERROR] send_rlc_um_control_primitive(CRLC_RESUME_REQ) cprimitive not handled\n");
        break;

      default:
        msg ("[RLC_UM_LITE][RB %d][ERROR] send_rlc_um_control_primitive(UNKNOWN CPRIMITIVE)\n", rlcP->rb_id);
  }
  free_mem_block (cprimitiveP);
}

//-----------------------------------------------------------------------------
void
init_rlc_um (struct rlc_um_entity *rlcP)
{
//-----------------------------------------------------------------------------

  memset (rlcP, 0, sizeof (struct rlc_um_entity));
  // TX SIDE
  list_init (&rlcP->pdus_to_mac_layer, NULL);

  rlcP->protocol_state = RLC_NULL_STATE;
  rlcP->nb_sdu = 0;
  rlcP->next_sdu_index = 0;
  rlcP->current_sdu_index = 0;

  rlcP->li_one_byte_short_to_add_in_next_pdu = 0;
  rlcP->li_exactly_filled_to_add_in_next_pdu = 0;
  rlcP->li_length_15_was_used_for_previous_pdu = 0;

  rlcP->vt_us = 0;
  rlcP->first_li_in_next_pdu = RLC_LI_UNDEFINED;

  // RX SIDE
  list_init (&rlcP->pdus_from_mac_layer, NULL);
  rlcP->vr_us = 0;
  rlcP->output_sdu_size_to_write = 0;
  rlcP->output_sdu_in_construction = NULL;
  
  
  rlcP->tx_pdcp_sdu                 = 0;
  rlcP->tx_pdcp_sdu_discarded          = 0;
  rlcP->tx_data_pdu          = 0;
  rlcP->rx_sdu          = 0;
  rlcP->rx_error_pdu          = 0;  
  rlcP->rx_data_pdu          = 0;
  rlcP->rx_data_pdu_out_of_window          = 0;
  
}

//-----------------------------------------------------------------------------
void
rlc_um_reset_state_variables (struct rlc_um_entity *rlcP)
{
//-----------------------------------------------------------------------------
  // TX SIDE
  rlcP->buffer_occupancy = 0;
  rlcP->nb_sdu = 0;
  rlcP->next_sdu_index = 0;
  rlcP->current_sdu_index = 0;
  rlcP->vt_us = 0;

  rlcP->li_one_byte_short_to_add_in_next_pdu = 0;
  rlcP->li_exactly_filled_to_add_in_next_pdu = 0;
  rlcP->li_length_15_was_used_for_previous_pdu = 0;

  // RX SIDE
  rlcP->last_reassemblied_sn = 127;
  rlcP->vr_us = 0;
}

//-----------------------------------------------------------------------------
void
rlc_um_free_all_resources (struct rlc_um_entity *rlcP)
{
//-----------------------------------------------------------------------------
  int             index;

  // TX SIDE
  list_free (&rlcP->pdus_to_mac_layer);

  if (rlcP->input_sdus_alloc) {
    for (index = 0; index < rlcP->size_input_sdus_buffer; index++) {
      if (rlcP->input_sdus[index]) {
        free_mem_block (rlcP->input_sdus[index]);
      }
    }
    free_mem_block (rlcP->input_sdus_alloc);
    rlcP->input_sdus_alloc = NULL;
  }
  // RX SIDE
  list_free (&rlcP->pdus_from_mac_layer);
  if ((rlcP->output_sdu_in_construction)) {
    free_mem_block (rlcP->output_sdu_in_construction);
  }
}

//-----------------------------------------------------------------------------
void
rlc_um_set_configured_parameters (struct rlc_um_entity *rlcP, mem_block_t *cprimitiveP)
{
//-----------------------------------------------------------------------------
  // timers
  rlcP->timer_discard_init = ((struct crlc_primitive *)
                              cprimitiveP->data)->primitive.c_config_req.parameters.um_parameters.timer_discard;

  // protocol_parameters
  rlcP->sdu_discard_mode = ((struct crlc_primitive *)
                            cprimitiveP->data)->primitive.c_config_req.parameters.um_parameters.sdu_discard_mode;

  // SPARE : not 3GPP
  rlcP->frame_tick_milliseconds = ((struct crlc_primitive *)
                                   cprimitiveP->data)->primitive.c_config_req.parameters.um_parameters.frame_tick_milliseconds;
  rlcP->size_input_sdus_buffer = ((struct crlc_primitive *)
                                  cprimitiveP->data)->primitive.c_config_req.parameters.um_parameters.size_input_sdus_buffer;
  rlcP->rb_id = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.um_parameters.rb_id;

  if ((rlcP->input_sdus_alloc == NULL) && (rlcP->size_input_sdus_buffer > 0)) {
    rlcP->input_sdus_alloc = get_free_mem_block (rlcP->size_input_sdus_buffer * sizeof (void *));
    rlcP->input_sdus = (mem_block_t **) (rlcP->input_sdus_alloc->data);
    memset (rlcP->input_sdus, 0, rlcP->size_input_sdus_buffer * sizeof (void *));
  }

  rlcP->first_pdu = 1;
#ifdef RLC_UM_LITE_FOR_RNRT_COSINUS
  // crc on header only for radio access bearers
  if ((rlcP->data_plane)) {
    rlcP->crc_on_header = 1;
  } else {
    rlcP->crc_on_header = 0;
  }
#else
  rlcP->crc_on_header = 0;
#endif

#ifdef DEBUG_LOAD_CONFIG
  if (rlcP->sdu_discard_mode == RLC_SDU_DISCARD_TIMER_BASED_NO_EXPLICIT) {
    msg ("[RLC UM][RB %d] SDU_DISCARD_TIMER_BASED_NO_EXPLICIT time out %d\n", rlcP->rb_id, rlcP->timer_discard_init);
  } else if (rlcP->sdu_discard_mode == RLC_SDU_DISCARD_NOT_CONFIGURED) {
    msg ("[RLC UM][RB %d] SDU_DISCARD_NOT_CONFIGURED\n", rlcP->rb_id);
  } else {
    msg ("[RLC UM][RB %d][ERROR] sdu discard mode not configured\n", rlcP->rb_id);
  }
#endif

}

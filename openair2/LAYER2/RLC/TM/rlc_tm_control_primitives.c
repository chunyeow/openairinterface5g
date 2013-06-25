/*******************************************************************************

Eurecom OpenAirInterface 2
Copyright(c) 1999 - 2010 Eurecom

This program is free software; you can redistribute it and/or modify it
under the terms and conditions of the GNU General Public License,
version 2, as published by the Free Software Foundation.

This program is distributed in the hope it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

The full GNU General Public License is included in this distribution in
the file called "COPYING".

Contact Information
Openair Admin: openair_admin@eurecom.fr
Openair Tech : openair_tech@eurecom.fr
Forums       : http://forums.eurecom.fsr/openairinterface
Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/
/***************************************************************************
                          rlc_tm_control_primitives.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr
 ***************************************************************************/
//#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "rlc_tm_entity.h"
#include "rlc_primitives.h"
#include "rlc_tm_fsm_proto_extern.h"
#include "rlc_tm_segment_proto_extern.h"
#include "rlc_tm_proto_extern.h"
#include "list.h"
#include "mem_block.h"
#include "rrm_config_structs.h"
//-----------------------------------------------------------------------------
void            config_req_rlc_tm (struct rlc_tm_entity *rlcP, module_id_t module_idP, rlc_tm_info_t * config_tmP, rb_id_t rb_idP, rb_type_t rb_typeP);
void            send_rlc_tm_control_primitive (struct rlc_tm_entity *rlcP, module_id_t module_idP, mem_block_t *cprimitiveP);
void            init_rlc_tm (struct rlc_tm_entity *rlcP);
void            rlc_tm_reset_state_variables (struct rlc_tm_entity *rlcP);
void            rlc_tm_free_all_resources (struct rlc_tm_entity *rlcP);
void            rlc_tm_set_configured_parameters (struct rlc_tm_entity *rlcP, mem_block_t *cprimitiveP);
//-----------------------------------------------------------------------------
void
config_req_rlc_tm (struct rlc_tm_entity *rlcP, module_id_t module_idP, rlc_tm_info_t * config_tmP, rb_id_t rb_idP, rb_type_t rb_typeP)
{
//-----------------------------------------------------------------------------
  mem_block_t *mb;

  mb = get_free_mem_block (sizeof (struct crlc_primitive));
  ((struct crlc_primitive *) mb->data)->type = CRLC_CONFIG_REQ;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.tm_parameters.e_r = RLC_E_R_ESTABLISHMENT;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.tm_parameters.timer_discard = config_tmP->timer_discard;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.tm_parameters.sdu_discard_mode = config_tmP->sdu_discard_mode;
#warning frame_tick_milliseconds
#ifdef NODE_RG
  //((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.tm_parameters.frame_tick_milliseconds = &protocol_bs->frame_tick_milliseconds;
#else
  //((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.tm_parameters.frame_tick_milliseconds = &protocol_ms->frame_tick_milliseconds;
#endif
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.tm_parameters.segmentation_indication = config_tmP->segmentation_indication;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.tm_parameters.delivery_of_erroneous_sdu = config_tmP->delivery_of_erroneous_sdu;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.tm_parameters.size_input_sdus_buffer = 128;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.tm_parameters.rb_id = rb_idP;
  send_rlc_tm_control_primitive (rlcP, module_idP, mb);
  if (rb_typeP != SIGNALLING_RADIO_BEARER) {
    rlcP->data_plane = 1;
  } else {
    rlcP->data_plane = 0;
  }
}

//-----------------------------------------------------------------------------
void
send_rlc_tm_control_primitive (struct rlc_tm_entity *rlcP, module_id_t module_idP, mem_block_t *cprimitiveP)
{
//-----------------------------------------------------------------------------
  switch (((struct crlc_primitive *) cprimitiveP->data)->type) {

      case CRLC_CONFIG_REQ:
        rlcP->module_id = module_idP;
        switch (((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.tm_parameters.e_r) {

            case RLC_E_R_ESTABLISHMENT:
              if (rlc_tm_fsm_notify_event (rlcP, RLC_TM_RECEIVE_CRLC_CONFIG_REQ_ENTER_DATA_TRANSFER_READY_STATE_EVENT)) {
                rlc_tm_set_configured_parameters (rlcP, cprimitiveP);   // the order of the calling of procedures...
                rlc_tm_reset_state_variables (rlcP);    // ...must not ...
              }
              break;

            case RLC_E_R_RELEASE:
              if (rlc_tm_fsm_notify_event (rlcP, RLC_TM_RECEIVE_CRLC_CONFIG_REQ_ENTER_NULL_STATE_EVENT)) {
                rlc_tm_free_all_resources (rlcP);
              }
              break;

            default:
              msg ("[RLC_TM][ERROR] send_rlc_tm_control_primitive(CRLC_CONFIG_REQ) unknown parameter E_R\n");
        }
        break;

      default:
        msg ("[RLC_TM %p][ERROR] send_rlc_tm_control_primitive(UNKNOWN CPRIMITIVE)\n", rlcP);
  }
  free_mem_block (cprimitiveP);
}

//-----------------------------------------------------------------------------
void
init_rlc_tm (struct rlc_tm_entity *rlcP)
{
//-----------------------------------------------------------------------------

  memset (rlcP, 0, sizeof (struct rlc_tm_entity));
  list_init (&rlcP->pdus_to_mac_layer, NULL);
  list_init (&rlcP->pdus_from_mac_layer, NULL);
  rlcP->protocol_state = RLC_NULL_STATE;
  rlcP->nb_sdu = 0;
  rlcP->next_sdu_index = 0;
  rlcP->current_sdu_index = 0;
}

//-----------------------------------------------------------------------------
void
rlc_tm_reset_state_variables (struct rlc_tm_entity *rlcP)
{
//-----------------------------------------------------------------------------
  rlcP->output_sdu_size_to_write = 0;
  rlcP->buffer_occupancy = 0;
  rlcP->nb_sdu = 0;
  rlcP->next_sdu_index = 0;
  rlcP->current_sdu_index = 0;
  rlcP->last_tti = 0xFFFFFFFF;
}

//-----------------------------------------------------------------------------
void
rlc_tm_free_all_resources (struct rlc_tm_entity *rlcP)
{
//-----------------------------------------------------------------------------
  int             index;

  list_free (&rlcP->pdus_to_mac_layer);
  list_free (&rlcP->pdus_from_mac_layer);

  if (rlcP->input_sdus_alloc) {
    for (index = 0; index < rlcP->size_input_sdus_buffer; index++) {
      if ((rlcP->input_sdus[index])) {
        free_mem_block (rlcP->input_sdus[index]);
      }
    }
    free_mem_block (rlcP->input_sdus_alloc);
    rlcP->input_sdus_alloc = NULL;
  }
  if ((rlcP->output_sdu_in_construction)) {
    free_mem_block (rlcP->output_sdu_in_construction);
  }
}

//-----------------------------------------------------------------------------
void
rlc_tm_set_configured_parameters (struct rlc_tm_entity *rlcP, mem_block_t *cprimitiveP)
{
//-----------------------------------------------------------------------------
  // timers
  rlcP->timer_discard_init = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.tm_parameters.timer_discard;

  // protocol_parameters
  rlcP->sdu_discard_mode = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.tm_parameters.sdu_discard_mode;
  rlcP->segmentation_indication = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.tm_parameters.segmentation_indication;
  rlcP->delivery_of_erroneous_sdu = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.tm_parameters.delivery_of_erroneous_sdu;
  // SPARE : not 3GPP
  rlcP->frame_tick_milliseconds = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.tm_parameters.frame_tick_milliseconds;
  rlcP->size_input_sdus_buffer = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.tm_parameters.size_input_sdus_buffer;
  rlcP->rb_id = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.tm_parameters.rb_id;

  if ((rlcP->input_sdus_alloc == NULL) && (rlcP->size_input_sdus_buffer > 0)) {
    rlcP->input_sdus_alloc = get_free_mem_block (rlcP->size_input_sdus_buffer * sizeof (void *));
    rlcP->input_sdus = (mem_block_t **) (rlcP->input_sdus_alloc->data);
    memset (rlcP->input_sdus, 0, rlcP->size_input_sdus_buffer * sizeof (void *));
  }

  if (rlcP->segmentation_indication == RLC_TM_SEGMENTATION_ALLOWED) {
    rlcP->segmentation = rlc_tm_segment;
    rlcP->rx = rlc_tm_rx_segment;
  } else {
    rlcP->segmentation = rlc_tm_no_segment;
    rlcP->rx = rlc_tm_rx_no_segment;
  }
}

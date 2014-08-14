/*******************************************************************************
    OpenAirInterface
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is
   included in this distribution in the file called "COPYING". If not,
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr

  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

/******************************************************************************
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr
*******************************************************************************/

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
//-----------------------------------------------------------------------------
#include "print.h"
#include "rlc_um_entity.h"
#include "rlc_um_constants.h"
#include "rlc_um_structs.h"
#include "rlc_primitives.h"
#include "protocol_vars_extern.h"
#include "rlc_um_fsm_proto_extern.h"
#include "lists_proto_extern.h"
#include "rrm_config_structs.h"
#include "debug_l2.h"

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
config_req_rlc_um (struct rlc_um_entity *rlcP, UM * config_umP, void *upper_layerP, void *(*data_indP) (void *, struct mem_block * sduP), uint8_t rb_idP)
{
//-----------------------------------------------------------------------------
  struct mem_block *mb;

  mb = get_free_mem_block (sizeof (struct crlc_primitive));
  ((struct crlc_primitive *) mb->data)->type = CRLC_CONFIG_REQ;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.um_parameters.e_r = RLC_E_R_ESTABLISHMENT;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.um_parameters.stop = 0;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.um_parameters.cont = 1;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.um_parameters.timer_discard = config_umP->timer_discard;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.um_parameters.sdu_discard_mode = config_umP->sdu_discard_mode;
#ifdef NODE_RG
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.um_parameters.frame_tick_milliseconds = &protocol_bs->frame_tick_milliseconds;
#else
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.um_parameters.frame_tick_milliseconds = &protocol_ms->frame_tick_milliseconds;
#endif
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.um_parameters.upper_layer = upper_layerP;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.um_parameters.size_input_sdus_buffer = 128;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.um_parameters.rlc_data_ind = data_indP;
  ((struct crlc_primitive *) mb->data)->primitive.c_config_req.parameters.um_parameters.rb_id = rb_idP;
  send_rlc_um_control_primitive (rlcP, mb);
}

//-----------------------------------------------------------------------------
void
send_rlc_um_control_primitive (struct rlc_um_entity *rlcP, struct mem_block *cprimitiveP)
{
//-----------------------------------------------------------------------------

  switch (((struct crlc_primitive *) cprimitiveP->data)->type) {

      case CRLC_CONFIG_REQ:

        switch (((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.um_parameters.e_r) {

            case RLC_E_R_ESTABLISHMENT:
              if (rlc_um_fsm_notify_event (rlcP, RLC_UM_RECEIVE_CRLC_CONFIG_REQ_ENTER_DATA_TRANSFER_READY_STATE_EVENT)) {
                rlc_um_set_configured_parameters (rlcP, cprimitiveP);   // the order of the calling of procedures...
                rlc_um_reset_state_variables (rlcP);    // ...must not ...
              }
              break;

            case RLC_E_R_MODIFICATION:
              msg ("[RLC_UM][ERROR] send_rlc_um_control_primitive(CRLC_CONFIG_REQ) RLC_AM_E_R_MODIFICATION not handled\n");
              break;

            case RLC_E_R_RELEASE:
              if (rlc_um_fsm_notify_event (rlcP, RLC_UM_RECEIVE_CRLC_CONFIG_REQ_ENTER_NULL_STATE_EVENT)) {
                rlc_um_free_all_resources (rlcP);
              }
              break;

            default:
              msg ("[RLC_UM][ERROR] send_rlc_um_control_primitive(CRLC_CONFIG_REQ) unknown parameter E_R\n");
        }
        break;

      case CRLC_RESUME_REQ:
        msg ("[RLC_UM][ERROR] send_rlc_um_control_primitive(CRLC_RESUME_REQ) cprimitive not handled\n");
        break;

      default:
        msg ("[RLC_UM %p][ERROR] send_rlc_um_control_primitive(UNKNOWN CPRIMITIVE)\n", rlcP);
  }
  free_mem_block (cprimitiveP);
}

//-----------------------------------------------------------------------------
void
init_rlc_um (struct rlc_um_entity *rlcP)
{
//-----------------------------------------------------------------------------
#ifndef NO_THREAD_SAFE
  pthread_mutexattr_t attr;
  int             error_code;
#endif

  memset (rlcP, 0, sizeof (struct rlc_um_entity));
  // TX SIDE
  init_cnt_up (&rlcP->pdus_to_mac_layer, NULL);

#ifndef NO_THREAD_SAFE
  // init mutex protect input sdu buffer
  pthread_mutexattr_init (&attr);
#    ifdef USER_MODE
  pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE_NP);
#    else
  pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_NORMAL);
#    endif
  if ((error_code = pthread_mutex_init (&rlcP->mutex_input_buffer, &attr))) {
    msg ("[RLC_UM %p][ERROR] init mutex input buffer %d\n", rlcP, error_code);
  }
#    ifndef USER_MODE
  //pthread_mutexattr_setprotocol(&rlcP->mutex_input_buffer, PTHREAD_PRIO_INHERIT);
#    endif
#endif

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
  init_up (&rlcP->pdus_from_mac_layer, NULL);
  rlcP->vr_us = 0;
  rlcP->output_sdu_size_to_write = 0;
  rlcP->output_sdu_in_construction = NULL;
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
  free_cnt_up (&rlcP->pdus_to_mac_layer);

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
  free_up (&rlcP->pdus_from_mac_layer);
  if ((rlcP->output_sdu_in_construction)) {
    free_mem_block (rlcP->output_sdu_in_construction);
  }
}

//-----------------------------------------------------------------------------
void
rlc_um_set_configured_parameters (struct rlc_um_entity *rlcP, struct mem_block *cprimitiveP)
{
//-----------------------------------------------------------------------------
  // timers
  rlcP->timer_discard_init = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.um_parameters.timer_discard;

  // protocol_parameters
  rlcP->sdu_discard_mode = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.um_parameters.sdu_discard_mode;

  // SPARE : not 3GPP
  rlcP->frame_tick_milliseconds = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.um_parameters.frame_tick_milliseconds;
  rlcP->size_input_sdus_buffer = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.um_parameters.size_input_sdus_buffer;
  rlcP->upper_layer = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.um_parameters.upper_layer;
  rlcP->rb_id = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.um_parameters.rb_id;

  if ((rlcP->input_sdus_alloc == NULL) && (rlcP->size_input_sdus_buffer > 0)) {
    rlcP->input_sdus_alloc = get_free_mem_block (rlcP->size_input_sdus_buffer * sizeof (void *));
    rlcP->input_sdus = (struct mem_block **) (rlcP->input_sdus_alloc->data);
    memset (rlcP->input_sdus, 0, rlcP->size_input_sdus_buffer * sizeof (void *));
  }

  rlcP->rlc_data_ind = ((struct crlc_primitive *) cprimitiveP->data)->primitive.c_config_req.parameters.um_parameters.rlc_data_ind;
  rlcP->first_pdu = 1;

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

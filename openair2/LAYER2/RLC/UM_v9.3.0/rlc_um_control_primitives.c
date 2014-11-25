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
#define RLC_UM_MODULE
#define RLC_UM_CONTROL_PRIMITIVES_C
#include "platform_types.h"
#include "assertions.h"
//-----------------------------------------------------------------------------
#include "rlc_um.h"
#include "rlc_primitives.h"
#include "list.h"
#include "rrm_config_structs.h"
#include "LAYER2/MAC/extern.h"
#include "UTIL/LOG/log.h"

#include "rlc_um_control_primitives.h"
#include "T-Reordering.h"

//-----------------------------------------------------------------------------
void config_req_rlc_um (
    const module_id_t     enb_module_idP,
    const module_id_t     ue_module_idP,
    const frame_t         frameP,
    const eNB_flag_t      eNB_flagP,
    const srb_flag_t      srb_flagP,
    const rlc_um_info_t  * const config_um_pP,
    const rb_id_t         rb_idP)
{
  //-----------------------------------------------------------------------------
  rlc_union_t     *rlc_union_p  = NULL;
  rlc_um_entity_t *rlc_p        = NULL;
  hash_key_t       key          = RLC_COLL_KEY_VALUE(enb_module_idP, ue_module_idP, eNB_flagP, rb_idP, srb_flagP);
  hashtable_rc_t   h_rc;

  h_rc = hashtable_get(rlc_coll_p, key, (void**)&rlc_union_p);
  if (h_rc == HASH_TABLE_OK) {
      rlc_p = &rlc_union_p->rlc.um;
      LOG_D(RLC, "[FRAME %05d][%s][RRC][MOD %u/%u][][--- CONFIG_REQ timer_reordering=%d sn_field_length=%d is_mXch=%d --->][RLC_UM][MOD %u/%u][RB %u]    \n",
          frameP,
          (eNB_flagP) ? "eNB" : "UE",
          enb_module_idP,
          ue_module_idP,
          config_um_pP->timer_reordering,
          config_um_pP->sn_field_length,
          config_um_pP->is_mXch,
          enb_module_idP,
          ue_module_idP,
          rb_idP);

      rlc_um_init(rlc_p);
      if (rlc_um_fsm_notify_event (rlc_p, RLC_UM_RECEIVE_CRLC_CONFIG_REQ_ENTER_DATA_TRANSFER_READY_STATE_EVENT)) {
          rlc_um_set_debug_infos(rlc_p, enb_module_idP, ue_module_idP, frameP, eNB_flagP, srb_flagP, rb_idP);
          rlc_um_configure(rlc_p,
              frameP,
              config_um_pP->timer_reordering,
              config_um_pP->sn_field_length,
              config_um_pP->sn_field_length,
              config_um_pP->is_mXch);
      }
  } else {
      LOG_E(RLC, "[FRAME %05d][%s][RRC][MOD %u/%u][][--- CONFIG_REQ --->][RLC_UM][MOD %u/%u][RB %u]  RLC NOT FOUND\n",
          frameP,
          (eNB_flagP) ? "eNB" : "UE",
          enb_module_idP,
          ue_module_idP,
          enb_module_idP,
          ue_module_idP,
          rb_idP);  }
}
//-----------------------------------------------------------------------------
const uint32_t const t_Reordering_tab[T_Reordering_spare1] = {0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,110,120,130,140,150,160,170,180,190,200};

void config_req_rlc_um_asn1 (
    const module_id_t         enb_module_idP,
    const module_id_t         ue_module_idP,
    const frame_t             frameP,
    const eNB_flag_t          eNB_flagP,
    const srb_flag_t          srb_flagP,
    const MBMS_flag_t         mbms_flagP,
    const mbms_session_id_t   mbms_session_idP,
    const mbms_service_id_t   mbms_service_idP,
    const UL_UM_RLC_t       * const ul_rlc_pP,
    const DL_UM_RLC_t       * const dl_rlc_pP,
    const rb_id_t             rb_idP)
{
  uint32_t         ul_sn_FieldLength   = 0;
  uint32_t         dl_sn_FieldLength   = 0;
  uint32_t         t_Reordering        = 0;
  rlc_union_t     *rlc_union_p         = NULL;
  rlc_um_entity_t *rlc_p               = NULL;
  hash_key_t       key                 = RLC_COLL_KEY_VALUE(enb_module_idP, ue_module_idP, eNB_flagP, rb_idP, srb_flagP);
  hashtable_rc_t   h_rc;

#if defined(Rel10)
  if (mbms_flagP) {
      AssertFatal(dl_rlc_pP, "No RLC UM DL config");
      AssertFatal(ul_rlc_pP == NULL, "RLC UM UL config present");
      key = RLC_COLL_KEY_MBMS_VALUE(enb_module_idP, ue_module_idP, eNB_flagP, mbms_service_idP, mbms_session_idP);
      h_rc = hashtable_get(rlc_coll_p, key, (void**)&rlc_union_p);
      AssertFatal (h_rc == HASH_TABLE_OK, "RLC NOT FOUND enb id %u ue id %i enb flag %u service id %u, session id %u",
          enb_module_idP,
          ue_module_idP,
          eNB_flagP,
          mbms_service_idP,
          mbms_session_idP);
      rlc_p = &rlc_union_p->rlc.um;
  }
  else
#endif
  {
      key  = RLC_COLL_KEY_VALUE(enb_module_idP, ue_module_idP, eNB_flagP, rb_idP, srb_flagP);
      h_rc = hashtable_get(rlc_coll_p, key, (void**)&rlc_union_p);
      AssertFatal (h_rc == HASH_TABLE_OK, "RLC NOT FOUND enb id %u ue id %i enb flag %u rb id %u, srb flag %u",
          enb_module_idP,
          ue_module_idP,
          eNB_flagP,
          rb_idP,
          srb_flagP);
      rlc_p = &rlc_union_p->rlc.um;
  }

  //-----------------------------------------------------------------------------
  LOG_D(RLC, "[FRAME %05d][%s][RRC][MOD %u/%u][][--- CONFIG_REQ timer_reordering=%dms sn_field_length=  --->][RLC_UM][MOD %u/%u][RB %u]    \n",
      frameP,
      (eNB_flagP) ? "eNB" : "UE",
      enb_module_idP,
      ue_module_idP,
      (dl_rlc_pP->t_Reordering<31)?t_Reordering_tab[dl_rlc_pP->t_Reordering]:-1,
      enb_module_idP,
      ue_module_idP,
      rb_idP);

  rlc_um_init(rlc_p);
  if (rlc_um_fsm_notify_event (rlc_p, RLC_UM_RECEIVE_CRLC_CONFIG_REQ_ENTER_DATA_TRANSFER_READY_STATE_EVENT)) {
      rlc_um_set_debug_infos(rlc_p, enb_module_idP, ue_module_idP, frameP, eNB_flagP, srb_flagP, rb_idP);
      if (ul_rlc_pP != NULL) {
          switch (ul_rlc_pP->sn_FieldLength) {
          case SN_FieldLength_size5:
            ul_sn_FieldLength = 5;
            break;
          case SN_FieldLength_size10:
            ul_sn_FieldLength = 10;
            break;
          default:
            LOG_E(RLC,"[FRAME %05d][%s][RLC_UM][MOD %u/%u][RB %u][CONFIGURE] INVALID Uplink sn_FieldLength %d, RLC NOT CONFIGURED\n",
                frameP,
                (rlc_p->is_enb) ? "eNB" : "UE",
                    rlc_p->enb_module_id,
                    rlc_p->ue_module_id,
                    rlc_p->rb_id,
                    ul_rlc_pP->sn_FieldLength);
            return;
          }
      }

      if (dl_rlc_pP != NULL) {
          switch (dl_rlc_pP->sn_FieldLength) {
          case SN_FieldLength_size5:
            dl_sn_FieldLength = 5;
            break;
          case SN_FieldLength_size10:
            dl_sn_FieldLength = 10;
            break;
          default:
            LOG_E(RLC,"[FRAME %05d][%s][RLC_UM][MOD %u/%u][RB %u][CONFIGURE] INVALID Downlink sn_FieldLength %d, RLC NOT CONFIGURED\n",
                frameP,
                (rlc_p->is_enb) ? "eNB" : "UE",
                    rlc_p->enb_module_id,
                    rlc_p->ue_module_id,
                    rlc_p->rb_id,
                    dl_rlc_pP->sn_FieldLength);
            return;
          }
          if (dl_rlc_pP->t_Reordering<T_Reordering_spare1) {
              t_Reordering = t_Reordering_tab[dl_rlc_pP->t_Reordering];
          } else {
              LOG_E(RLC,"[FRAME %05d][%s][RLC_UM][MOD %u/%u][RB %u][CONFIGURE] INVALID T_Reordering %d, RLC NOT CONFIGURED\n",
                  frameP,
                  (rlc_p->is_enb) ? "eNB" : "UE",
                      rlc_p->enb_module_id,
                      rlc_p->ue_module_id,
                      rlc_p->rb_id,
                      dl_rlc_pP->t_Reordering);
              return;
          }
      }
      if (eNB_flagP > 0) {
          rlc_um_configure(rlc_p,
              frameP,
              t_Reordering,
              ul_sn_FieldLength,
              dl_sn_FieldLength,
              mbms_flagP);
      } else {
          rlc_um_configure(rlc_p,
              frameP,
              t_Reordering,
              dl_sn_FieldLength,
              ul_sn_FieldLength,
              mbms_flagP);
      }
  }
}
//-----------------------------------------------------------------------------
void
rlc_um_init (rlc_um_entity_t * const rlc_pP)
{
  //-----------------------------------------------------------------------------

  AssertFatal(rlc_pP, "Bad RLC UM pointer (NULL)");
  if (rlc_pP->initialized) {
      LOG_D(RLC, "[FRAME XXXXX][RLC_UM][MOD XX][RB XX][INIT] ALREADY DONE, DOING NOTHING\n");
  } else {
      LOG_D(RLC, "[FRAME XXXXX][RLC_UM][MOD XX][RB XX][INIT] STATE VARIABLES, BUFFERS, LISTS\n");
      memset (rlc_pP, 0, sizeof (rlc_um_entity_t));
      // TX SIDE
      list_init (&rlc_pP->pdus_to_mac_layer, NULL);
      pthread_mutex_init(&rlc_pP->lock_input_sdus, NULL);
      list_init (&rlc_pP->input_sdus, NULL);

      rlc_pP->protocol_state = RLC_NULL_STATE;

      //rlc_pP->vt_us = 0;

      // RX SIDE
      list_init (&rlc_pP->pdus_from_mac_layer, NULL);
      //rlc_pP->vr_ur = 0;
      //rlc_pP->vr_ux = 0;
      //rlc_pP->vr_uh = 0;
      //rlc_pP->output_sdu_size_to_write = 0;
      //rlc_pP->output_sdu_in_construction = NULL;

      rlc_pP->rx_sn_length          = 10;
      rlc_pP->rx_header_min_length_in_bytes = 2;
      rlc_pP->tx_sn_length          = 10;
      rlc_pP->tx_header_min_length_in_bytes = 2;


      if (rlc_pP->dar_buffer == NULL) {
          rlc_pP->dar_buffer = calloc (1, 1024 * sizeof (void *));
      }

      rlc_pP->first_pdu = 1;
      rlc_pP->initialized = TRUE;
  }
}
//-----------------------------------------------------------------------------
void
rlc_um_reset_state_variables (rlc_um_entity_t * const rlc_pP)
{
  //-----------------------------------------------------------------------------
  rlc_pP->buffer_occupancy = 0;


  // TX SIDE
  rlc_pP->vt_us = 0;
  // RX SIDE
  rlc_pP->vr_ur = 0;
  rlc_pP->vr_ux = 0;
  rlc_pP->vr_uh = 0;
}
//-----------------------------------------------------------------------------
void
rlc_um_cleanup (rlc_um_entity_t * const rlc_pP)
{
  //-----------------------------------------------------------------------------
  int             index;
  // TX SIDE
  list_free (&rlc_pP->pdus_to_mac_layer);
  pthread_mutex_destroy(&rlc_pP->lock_input_sdus);
  list_free (&rlc_pP->input_sdus);

  // RX SIDE
  list_free (&rlc_pP->pdus_from_mac_layer);
  if ((rlc_pP->output_sdu_in_construction)) {
      free_mem_block (rlc_pP->output_sdu_in_construction);
  }
  if (rlc_pP->dar_buffer) {
      for (index = 0; index < 1024; index++) {
          if (rlc_pP->dar_buffer[index]) {
              free_mem_block (rlc_pP->dar_buffer[index]);
          }
      }
      free (rlc_pP->dar_buffer);
      rlc_pP->dar_buffer = NULL;
  }
  memset(rlc_pP, 0, sizeof(rlc_um_entity_t));
}

//-----------------------------------------------------------------------------
void rlc_um_configure(
    rlc_um_entity_t * const rlc_pP,
    const frame_t          frameP,
    const uint32_t         timer_reorderingP,
    const uint32_t         rx_sn_field_lengthP,
    const uint32_t         tx_sn_field_lengthP,
    const uint32_t         is_mXchP)
//-----------------------------------------------------------------------------
{
  if (rx_sn_field_lengthP == 10) {
      rlc_pP->rx_sn_length                  = 10;
      rlc_pP->rx_sn_modulo                  = RLC_UM_SN_10_BITS_MODULO;
      rlc_pP->rx_um_window_size             = RLC_UM_WINDOW_SIZE_SN_10_BITS;
      rlc_pP->rx_header_min_length_in_bytes = 2;
  } else if (rx_sn_field_lengthP == 5) {
      rlc_pP->rx_sn_length                  = 5;
      rlc_pP->rx_sn_modulo                  = RLC_UM_SN_5_BITS_MODULO;
      rlc_pP->rx_um_window_size             = RLC_UM_WINDOW_SIZE_SN_5_BITS;
      rlc_pP->rx_header_min_length_in_bytes = 1;
  } else if (rx_sn_field_lengthP != 0) {
      LOG_E(RLC, "[FRAME %05d][%s][RLC_UM][MOD %u/%u][RB %u][CONFIGURE] INVALID RX SN LENGTH %d BITS NOT IMPLEMENTED YET, RLC NOT CONFIGURED\n",
          frameP,
          (rlc_pP->is_enb) ? "eNB" : "UE",
              rlc_pP->enb_module_id,
              rlc_pP->ue_module_id,
              rlc_pP->rb_id,
              rx_sn_field_lengthP);
      return;
  }

  if (tx_sn_field_lengthP == 10) {
      rlc_pP->tx_sn_length                  = 10;
      rlc_pP->tx_sn_modulo                  = RLC_UM_SN_10_BITS_MODULO;
      rlc_pP->tx_um_window_size             = RLC_UM_WINDOW_SIZE_SN_10_BITS;
      rlc_pP->tx_header_min_length_in_bytes = 2;
  } else if (tx_sn_field_lengthP == 5) {
      rlc_pP->tx_sn_length                  = 5;
      rlc_pP->tx_sn_modulo                  = RLC_UM_SN_5_BITS_MODULO;
      rlc_pP->tx_um_window_size             = RLC_UM_WINDOW_SIZE_SN_5_BITS;
      rlc_pP->tx_header_min_length_in_bytes = 1;
  } else if (tx_sn_field_lengthP != 0) {
      LOG_E(RLC, "[FRAME %05d][%s][RLC_UM][MOD %02d/%02][RB %u][CONFIGURE] INVALID RX SN LENGTH %d BITS NOT IMPLEMENTED YET, RLC NOT CONFIGURED\n",
          frameP,
          (rlc_pP->is_enb) ? "eNB" : "UE",
              rlc_pP->enb_module_id,
              rlc_pP->ue_module_id,
              rlc_pP->rb_id,
              tx_sn_field_lengthP);
      return;
  }

  if (is_mXchP > 0) {
      rlc_pP->tx_um_window_size = 0;
      rlc_pP->rx_um_window_size = 0;
  }
  rlc_pP->is_mxch = is_mXchP;

  rlc_pP->last_reassemblied_sn  = rlc_pP->rx_sn_modulo - 1;
  rlc_pP->last_reassemblied_missing_sn  = rlc_pP->rx_sn_modulo - 1;
  rlc_pP->reassembly_missing_sn_detected = 0;
  // timers
  rlc_um_init_timer_reordering(rlc_pP, timer_reorderingP);

  rlc_pP->first_pdu = 1;

  rlc_um_reset_state_variables (rlc_pP);
}
//-----------------------------------------------------------------------------
void rlc_um_set_debug_infos(
    rlc_um_entity_t *rlc_pP,
    const module_id_t      enb_module_idP,
    const module_id_t      ue_module_idP,
    const frame_t          frameP,
    const eNB_flag_t       eNB_flagP,
    const srb_flag_t       srb_flagP,
    const rb_id_t          rb_idP)
//-----------------------------------------------------------------------------
{
  LOG_D(RLC, "[FRAME %05d][%s][RLC_UM][SET DEBUG INFOS] enb_module_id %u ue_module_id %u rb_id %d srb_flag %d\n",
      frameP,
      (eNB_flagP) ? "eNB" : "UE",
      enb_module_idP,
      ue_module_idP,
      rb_idP,
      srb_flagP);

  rlc_pP->enb_module_id = enb_module_idP;
  rlc_pP->ue_module_id  = ue_module_idP;
  rlc_pP->rb_id         = rb_idP;
  if (srb_flagP) {
      rlc_pP->is_data_plane = 0;
  } else {
      rlc_pP->is_data_plane = 1;
  }
  rlc_pP->is_enb = eNB_flagP;
}

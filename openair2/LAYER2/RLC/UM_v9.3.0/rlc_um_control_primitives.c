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
#define RLC_UM_MODULE
#define RLC_UM_CONTROL_PRIMITIVES_C
//#include "rtos_header.h"
#include "platform_types.h"
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
void config_req_rlc_um (frame_t         frameP,
                        eNB_flag_t      eNB_flagP,
                        module_id_t     enb_module_idP,
                        module_id_t     ue_module_idP,
                        rlc_um_info_t  *config_um_pP,
                        rb_id_t         rb_idP,
                        rb_type_t       rb_typeP)
{
  //-----------------------------------------------------------------------------
  rlc_um_entity_t *rlc_p = NULL;

  if (eNB_flagP) {
      rlc_p = &rlc_array_eNB[enb_module_idP][ue_module_idP][rb_idP].rlc.um;
  } else {
      rlc_p = &rlc_array_ue[ue_module_idP][rb_idP].rlc.um;
  }
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
      rlc_um_set_debug_infos(rlc_p, frameP, eNB_flagP, enb_module_idP, ue_module_idP, rb_idP, rb_typeP);
      rlc_um_configure(rlc_p,
          frameP,
          config_um_pP->timer_reordering,
          config_um_pP->sn_field_length,
          config_um_pP->sn_field_length,
          config_um_pP->is_mXch);
  }
}
//-----------------------------------------------------------------------------
uint32_t t_Reordering_tab[T_Reordering_spare1] = {0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,110,120,130,140,150,160,170,180,190,200};

void config_req_rlc_um_asn1 (frame_t            frameP,
                             eNB_flag_t         eNB_flagP,
                             MBMS_flag_t        mbms_flagP,
                             module_id_t        enb_module_idP,
                             module_id_t        ue_module_idP,
                             mbms_session_id_t  mbms_session_idP,
                             mbms_service_id_t  mbms_service_idP,
                             UL_UM_RLC_t       *ul_rlc_pP,
                             DL_UM_RLC_t       *dl_rlc_pP,
                             rb_id_t            rb_idP,
                             rb_type_t          rb_typeP)
{
  uint32_t            ul_sn_FieldLength = 0;
  uint32_t            dl_sn_FieldLength = 0;
  uint32_t            t_Reordering;
  rlc_um_entity_t *rlc_p               = NULL;
#if defined(Rel10)
  if (mbms_flagP) {
      if (eNB_flagP) {
          rlc_p = &rlc_mbms_array_eNB[enb_module_idP][mbms_service_idP][mbms_session_idP].um;
            LOG_D(RLC,"eNB config_req_rlc_um_asn1 rlc_um_p : %p RB %u service %u session %u",
                  rlc_p,
                  rb_idP,
                  mbms_service_idP,
                  mbms_session_idP
                 );
      } else {
          rlc_p = &rlc_mbms_array_ue[ue_module_idP][mbms_service_idP][mbms_session_idP].um;
            LOG_D(RLC,"UE config_req_rlc_um_asn1 rlc_um_p : %p RB %u service %u session %u",
                  rlc_p,
                  rb_idP,
                  mbms_service_idP,
                  mbms_session_idP
                 );
      }
  }
  else
#endif
  {
      if (eNB_flagP) {
          rlc_p = &rlc_array_eNB[enb_module_idP][ue_module_idP][rb_idP].rlc.um;
      } else {
          rlc_p = &rlc_array_ue[ue_module_idP][rb_idP].rlc.um;
      }
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
      rlc_um_set_debug_infos(rlc_p, frameP, eNB_flagP, enb_module_idP, ue_module_idP, rb_idP, rb_typeP);
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
      if (mbms_flagP == MBMS_FLAG_YES) {
          rlc_p->allocation = TRUE;
      }
  }
}
//-----------------------------------------------------------------------------
void
rlc_um_init (rlc_um_entity_t *rlc_pP)
{
  //-----------------------------------------------------------------------------

  int saved_allocation = rlc_pP->allocation;
  memset (rlc_pP, 0, sizeof (rlc_um_entity_t));
  rlc_pP->allocation = saved_allocation;
  // TX SIDE
  list_init (&rlc_pP->pdus_to_mac_layer, NULL);

  rlc_pP->protocol_state = RLC_NULL_STATE;
  //rlc_pP->nb_sdu           = 0;
  //rlc_pP->next_sdu_index   = 0;
  //rlc_pP->current_sdu_index = 0;

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

  // SPARE : not 3GPP
  rlc_pP->size_input_sdus_buffer =128;

  if ((rlc_pP->input_sdus_alloc == NULL) && (rlc_pP->size_input_sdus_buffer > 0)) {
      rlc_pP->input_sdus_alloc = get_free_mem_block (rlc_pP->size_input_sdus_buffer * sizeof (void *));
      rlc_pP->input_sdus = (mem_block_t **) (rlc_pP->input_sdus_alloc->data);
      memset (rlc_pP->input_sdus, 0, rlc_pP->size_input_sdus_buffer * sizeof (void *));
  }
  if (rlc_pP->dar_buffer_alloc == NULL) {
      rlc_pP->dar_buffer_alloc = get_free_mem_block (1024 * sizeof (void *));
      rlc_pP->dar_buffer = (mem_block_t **) (rlc_pP->dar_buffer_alloc->data);
      memset (rlc_pP->dar_buffer, 0, 1024 * sizeof (void *));
  }

  rlc_pP->first_pdu = 1;
}
//-----------------------------------------------------------------------------
void
rlc_um_reset_state_variables (rlc_um_entity_t *rlc_pP)
{
  //-----------------------------------------------------------------------------
  rlc_pP->buffer_occupancy = 0;
  rlc_pP->nb_sdu = 0;
  rlc_pP->next_sdu_index = 0;
  rlc_pP->current_sdu_index = 0;

  // TX SIDE
  rlc_pP->vt_us = 0;
  // RX SIDE
  rlc_pP->vr_ur = 0;
  rlc_pP->vr_ux = 0;
  rlc_pP->vr_uh = 0;
}
//-----------------------------------------------------------------------------
void
rlc_um_cleanup (rlc_um_entity_t *rlc_pP)
{
  //-----------------------------------------------------------------------------
  int             index;
  // TX SIDE
  list_free (&rlc_pP->pdus_to_mac_layer);

  if (rlc_pP->input_sdus_alloc) {
      for (index = 0; index < rlc_pP->size_input_sdus_buffer; index++) {
          if (rlc_pP->input_sdus[index]) {
              free_mem_block (rlc_pP->input_sdus[index]);
          }
      }
      free_mem_block (rlc_pP->input_sdus_alloc);
      rlc_pP->input_sdus_alloc = NULL;
  }
  // RX SIDE
  list_free (&rlc_pP->pdus_from_mac_layer);
  if ((rlc_pP->output_sdu_in_construction)) {
      free_mem_block (rlc_pP->output_sdu_in_construction);
  }
  if (rlc_pP->dar_buffer_alloc) {
      for (index = 0; index < 1024; index++) {
          if (rlc_pP->dar_buffer[index]) {
              free_mem_block (rlc_pP->dar_buffer[index]);
          }
      }
      free_mem_block (rlc_pP->dar_buffer_alloc);
      rlc_pP->dar_buffer_alloc = NULL;
  }
  memset(rlc_pP, 0, sizeof(rlc_um_entity_t));
}

//-----------------------------------------------------------------------------
void rlc_um_configure(rlc_um_entity_t *rlc_pP,
    frame_t          frameP,
    uint32_t            timer_reorderingP,
    uint32_t            rx_sn_field_lengthP,
    uint32_t            tx_sn_field_lengthP,
    uint32_t            is_mXchP)
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
void rlc_um_set_debug_infos(rlc_um_entity_t *rlc_pP,
    frame_t          frameP,
    eNB_flag_t       eNB_flagP,
    module_id_t      enb_module_idP,
    module_id_t      ue_module_idP,
    rb_id_t          rb_idP,
    rb_type_t        rb_typeP)
//-----------------------------------------------------------------------------
{
  LOG_D(RLC, "[FRAME %05d][%s][RLC_UM][SET DEBUG INFOS] enb_module_id %u ue_module_id %u rb_id %d rb_type %d\n",
      frameP,
      (eNB_flagP) ? "eNB" : "UE",
      enb_module_idP,
      ue_module_idP,
      rb_idP,
      rb_typeP);

  rlc_pP->enb_module_id = enb_module_idP;
  rlc_pP->ue_module_id  = ue_module_idP;
  rlc_pP->rb_id         = rb_idP;
  if (rb_typeP == RADIO_ACCESS_BEARER) {
      rlc_pP->is_data_plane = 1;
  } else {
      rlc_pP->is_data_plane = 0;
  }
  rlc_pP->is_enb = eNB_flagP;
}

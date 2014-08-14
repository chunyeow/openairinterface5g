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
#define RLC_TM_MODULE
#define RLC_TM_INIT_C
//-----------------------------------------------------------------------------
#include "rlc_tm.h"
#include "LAYER2/MAC/extern.h"
//-----------------------------------------------------------------------------
void config_req_rlc_tm (
    const module_id_t enb_module_idP,
    const module_id_t ue_module_idP,
    const frame_t     frameP,
    const eNB_flag_t  eNB_flagP,
    const srb_flag_t  srb_flagP,
    const rlc_tm_info_t * const config_tmP,
    const rb_id_t     rb_idP)
{
//-----------------------------------------------------------------------------
    rlc_union_t     *rlc_union_p  = NULL;
    rlc_tm_entity_t *rlc_p        = NULL;
    hash_key_t       key          = RLC_COLL_KEY_VALUE(enb_module_idP, ue_module_idP, eNB_flagP, rb_idP, srb_flagP);
    hashtable_rc_t   h_rc;

    h_rc = hashtable_get(rlc_coll_p, key, (void**)&rlc_union_p);
    if (h_rc == HASH_TABLE_OK) {
        rlc_p = &rlc_union_p->rlc.tm;
        LOG_D(RLC, "[FRAME %05d][%s][RRC][MOD %u/%u][][--- CONFIG_REQ (is_uplink_downlink=%d) --->][RLC_TM][MOD %u/%u][RB %u]\n",
            frameP,
            ( eNB_flagP > 0) ? "eNB":"UE",
            enb_module_idP,
            ue_module_idP,
            config_tmP->is_uplink_downlink,
            enb_module_idP,
            ue_module_idP,
            rb_idP);

        rlc_tm_init(rlc_p);
        rlc_p->protocol_state = RLC_DATA_TRANSFER_READY_STATE;
        rlc_tm_set_debug_infos(rlc_p, frameP, eNB_flagP, enb_module_idP, ue_module_idP, rb_idP, srb_flagP);
        rlc_tm_configure(rlc_p, config_tmP->is_uplink_downlink);
    } else {
        LOG_E(RLC, "[FRAME %05d][%s][RRC][MOD %u/%u][][--- CONFIG_REQ  --->][RLC_TM][MOD %u/%u][RB %u], RLC NOT FOUND\n",
            frameP,
            ( eNB_flagP > 0) ? "eNB":"UE",
            enb_module_idP,
            ue_module_idP,
            enb_module_idP,
            ue_module_idP,
            rb_idP);
    }
}

//-----------------------------------------------------------------------------
void rlc_tm_init (rlc_tm_entity_t * const rlcP)
{
//-----------------------------------------------------------------------------
    int saved_allocation = rlcP->allocation;
    memset (rlcP, 0, sizeof (struct rlc_tm_entity));
    rlcP->allocation = saved_allocation;
    // TX SIDE
    list_init (&rlcP->pdus_to_mac_layer, NULL);

    rlcP->protocol_state    = RLC_NULL_STATE;
    rlcP->nb_sdu            = 0;
    rlcP->next_sdu_index    = 0;
    rlcP->current_sdu_index = 0;

    rlcP->output_sdu_size_to_write = 0;
    rlcP->buffer_occupancy  = 0;

    // SPARE : not 3GPP
    rlcP->size_input_sdus_buffer = 16;

    if ((rlcP->input_sdus_alloc == NULL) && (rlcP->size_input_sdus_buffer > 0)) {
        rlcP->input_sdus_alloc = get_free_mem_block (rlcP->size_input_sdus_buffer * sizeof (void *));
        rlcP->input_sdus = (mem_block_t **) (rlcP->input_sdus_alloc->data);
        memset (rlcP->input_sdus, 0, rlcP->size_input_sdus_buffer * sizeof (void *));
    }
}

//-----------------------------------------------------------------------------
void rlc_tm_reset_state_variables (struct rlc_tm_entity * const rlcP)
{
//-----------------------------------------------------------------------------
  rlcP->output_sdu_size_to_write = 0;
  rlcP->buffer_occupancy = 0;
  rlcP->nb_sdu = 0;
  rlcP->next_sdu_index = 0;
  rlcP->current_sdu_index = 0;
}
//-----------------------------------------------------------------------------
void
rlc_tm_cleanup (rlc_tm_entity_t * const rlcP)
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
    if ((rlcP->output_sdu_in_construction)) {
        free_mem_block (rlcP->output_sdu_in_construction);
    }
    memset(rlcP, 0, sizeof(rlc_tm_entity_t));
}

//-----------------------------------------------------------------------------
void rlc_tm_configure(
    rlc_tm_entity_t * const rlcP,
    const boolean_t is_uplink_downlinkP)
{
    //-----------------------------------------------------------------------------
    rlcP->is_uplink_downlink = is_uplink_downlinkP;
    rlc_tm_reset_state_variables (rlcP);
}

//-----------------------------------------------------------------------------
void rlc_tm_set_debug_infos(
    rlc_tm_entity_t * const rlcP,
    const module_id_t enb_module_idP,
    const module_id_t ue_module_idP,
    const frame_t     frameP,
    const eNB_flag_t  eNB_flagP,
    const srb_flag_t  srb_flagP,
    const rb_id_t     rb_idP)
//-----------------------------------------------------------------------------
{
    msg ("[FRAME %05d][%s][RLC_TM][MOD %02u/%02u][RB %u][SET DEBUG INFOS] enb module_id %d ue module_id %d rb_id %d srb_flag %d\n",
          frameP,
          (eNB_flagP) ? "eNB" : "UE",
          enb_module_idP,
          ue_module_idP,
          rb_idP,
          enb_module_idP,
          ue_module_idP,
          rb_idP,
          srb_flagP);

    rlcP->enb_module_id = enb_module_idP;
    rlcP->ue_module_id  = ue_module_idP;
    rlcP->rb_id     = rb_idP;
    if (srb_flagP) {
        rlcP->is_data_plane = 0;
    } else {
        rlcP->is_data_plane = 1;
    }
    rlcP->is_enb = eNB_flagP;
}

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
#define RLC_TM_MODULE
#define RLC_TM_INIT_C
//-----------------------------------------------------------------------------
#include "rlc_tm.h"
#include "LAYER2/MAC/extern.h"
//-----------------------------------------------------------------------------
void config_req_rlc_tm (rlc_tm_entity_t *rlcP, u32_t frame, u8_t eNB_flagP, module_id_t module_idP, rlc_tm_info_t * config_tmP, rb_id_t rb_idP, rb_type_t rb_typeP)
{
//-----------------------------------------------------------------------------
    rlc_tm_init(rlcP);
    rlcP->protocol_state = RLC_DATA_TRANSFER_READY_STATE;
    rlc_tm_set_debug_infos(rlcP, frame, eNB_flagP, module_idP, rb_idP, rb_typeP);
    rlc_tm_configure(rlcP, config_tmP->is_uplink_downlink);
}

//-----------------------------------------------------------------------------
void rlc_tm_init (rlc_tm_entity_t *rlcP)
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
void rlc_tm_reset_state_variables (struct rlc_tm_entity *rlcP)
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
rlc_tm_cleanup (rlc_tm_entity_t *rlcP)
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
}

//-----------------------------------------------------------------------------
void rlc_tm_configure(rlc_tm_entity_t *rlcP, u8_t is_uplink_downlinkP)
{
    //-----------------------------------------------------------------------------
    rlcP->is_uplink_downlink = is_uplink_downlinkP;
    rlc_tm_reset_state_variables (rlcP);
}

//-----------------------------------------------------------------------------
void rlc_tm_set_debug_infos(rlc_tm_entity_t *rlcP, u32_t frame, u8_t eNB_flagP, module_id_t module_idP, rb_id_t rb_idP, rb_type_t rb_typeP)
//-----------------------------------------------------------------------------
{
    msg ("[FRAME %05d][RLC_TM][MOD %02d][RB %02d][SET DEBUG INFOS] module_id %d rb_id %d rb_type %d\n", frame, module_idP, rb_idP, module_idP, rb_idP, rb_typeP);

    rlcP->module_id = module_idP;
    rlcP->rb_id     = rb_idP;
    if (rb_typeP != SIGNALLING_RADIO_BEARER) {
        rlcP->is_data_plane = 1;
    } else {
        rlcP->is_data_plane = 0;
    }
    rlcP->is_enb = eNB_flagP;
}

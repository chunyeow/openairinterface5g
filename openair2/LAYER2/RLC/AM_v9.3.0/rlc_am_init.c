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
#define RLC_AM_MODULE
#define RLC_AM_INIT_C
#ifdef USER_MODE
#include <string.h>
#endif
//-----------------------------------------------------------------------------
#include "rlc_am.h"
#include "LAYER2/MAC/extern.h"
#include "UTIL/LOG/log.h"
//-----------------------------------------------------------------------------
void rlc_am_init(rlc_am_entity_t *rlcP,u32_t frame)
//-----------------------------------------------------------------------------
{
    int saved_allocation = rlcP->allocation;
    LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD XX][RB XX][INIT] STATE VARIABLES, BUFFERS, LISTS\n", frame);
    memset(rlcP, 0, sizeof(rlc_am_entity_t));
    rlcP->allocation = saved_allocation;

    list2_init(&rlcP->receiver_buffer,      "RX BUFFER");
    list_init(&rlcP->pdus_to_mac_layer,     "PDUS TO MAC");
    list_init(&rlcP->control_pdu_list,      "CONTROL PDU LIST");
    list_init(&rlcP->segmentation_pdu_list, "SEGMENTATION PDU LIST");
    //LOG_D(RLC,"RLC_AM_SDU_CONTROL_BUFFER_SIZE %d sizeof(rlc_am_tx_sdu_management_t) %d \n",  RLC_AM_SDU_CONTROL_BUFFER_SIZE, sizeof(rlc_am_tx_sdu_management_t));
    
    rlcP->input_sdus_alloc         = get_free_mem_block(RLC_AM_SDU_CONTROL_BUFFER_SIZE*sizeof(rlc_am_tx_sdu_management_t));
    rlcP->input_sdus               = (rlc_am_tx_sdu_management_t*)((rlcP->input_sdus_alloc)->data);
    rlcP->pdu_retrans_buffer_alloc = get_free_mem_block((u16_t)((unsigned int)RLC_AM_PDU_RETRANSMISSION_BUFFER_SIZE*(unsigned int)sizeof(rlc_am_tx_data_pdu_management_t)));
    rlcP->pdu_retrans_buffer       = (rlc_am_tx_data_pdu_management_t*)((rlcP->pdu_retrans_buffer_alloc)->data);
    LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD XX][RB XX][INIT] input_sdus[] = %p  element size=%d\n", frame, rlcP->input_sdus,sizeof(rlc_am_tx_sdu_management_t));
    LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD XX][RB XX][INIT] pdu_retrans_buffer[] = %p element size=%d\n", frame, rlcP->pdu_retrans_buffer,sizeof(rlc_am_tx_data_pdu_management_t));

    // TX state variables
    //rlcP->vt_a    = 0;
    rlcP->vt_ms   = rlcP->vt_a + RLC_AM_WINDOW_SIZE;
    //rlcP->vt_s    = 0;
    //rlcP->poll_sn = 0;
    // TX counters
    //rlcP->c_pdu_without_poll  = 0;
    //rlcP->c_byte_without_poll = 0;
    // RX state variables
    //rlcP->vr_r    = 0;
    rlcP->vr_mr   = rlcP->vr_r + RLC_AM_WINDOW_SIZE;
    //rlcP->vr_x    = 0;
    //rlcP->vr_ms   = 0;
    //rlcP->vr_h    = 0;

    rlcP->last_frame_status_indication = 123456; // any value > 1
    rlcP->first_retrans_pdu_sn         = -1;
}
//-----------------------------------------------------------------------------
void rlc_am_cleanup(rlc_am_entity_t *rlcP,u32_t frame)
//-----------------------------------------------------------------------------
{
    LOG_I(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][CLEANUP]\n", frame, rlcP->module_id, rlcP->rb_id);

    list2_free(&rlcP->receiver_buffer);
    list_free(&rlcP->pdus_to_mac_layer);
    list_free(&rlcP->control_pdu_list);
    list_free(&rlcP->segmentation_pdu_list);


    if (rlcP->output_sdu_in_construction != NULL) {
        free_mem_block(rlcP->output_sdu_in_construction);
        rlcP->output_sdu_in_construction = NULL;
    }
    unsigned int i;
    if (rlcP->input_sdus_alloc != NULL) {
        for (i=0; i < RLC_AM_SDU_CONTROL_BUFFER_SIZE; i++) {
            if (rlcP->input_sdus[i].mem_block != NULL) {
                free_mem_block(rlcP->input_sdus[i].mem_block);
                rlcP->input_sdus[i].mem_block = NULL;
            }
        }
        free_mem_block(rlcP->input_sdus_alloc);
        rlcP->input_sdus_alloc = NULL;
        rlcP->input_sdus       = NULL;
    }
    if (rlcP->pdu_retrans_buffer_alloc != NULL) {
        for (i=0; i < RLC_AM_PDU_RETRANSMISSION_BUFFER_SIZE; i++) {
            if (rlcP->pdu_retrans_buffer[i].mem_block != NULL) {
                free_mem_block(rlcP->pdu_retrans_buffer[i].mem_block);
                rlcP->pdu_retrans_buffer[i].mem_block = NULL;
            }
        }
        free_mem_block(rlcP->pdu_retrans_buffer_alloc);
        rlcP->pdu_retrans_buffer_alloc = NULL;
        rlcP->pdu_retrans_buffer       = NULL;
    }
}
//-----------------------------------------------------------------------------
void rlc_am_configure(rlc_am_entity_t *rlcP,
		      u32_t frame,
                      u16_t max_retx_thresholdP,
                      u16_t poll_pduP,
                      u16_t poll_byteP,
                      u32_t t_poll_retransmitP,
                      u32_t t_reorderingP,
                      u32_t t_status_prohibitP)
//-----------------------------------------------------------------------------
{
    LOG_I(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][CONFIGURE] max_retx_threshold %d poll_pdu %d poll_byte %d t_poll_retransmit %d t_reordering %d t_status_prohibit %d\n", frame, rlcP->module_id, rlcP->rb_id, max_retx_thresholdP, poll_pduP, poll_byteP, t_poll_retransmitP, t_reorderingP, t_status_prohibitP);

    rlcP->max_retx_threshold = max_retx_thresholdP;
    rlcP->poll_pdu           = poll_pduP;
    rlcP->poll_byte          = poll_byteP;
    rlcP->protocol_state     = RLC_DATA_TRANSFER_READY_STATE;

    rlc_am_init_timer_poll_retransmit(rlcP, t_poll_retransmitP);
    rlc_am_init_timer_reordering     (rlcP, t_reorderingP);
    rlc_am_init_timer_status_prohibit(rlcP, t_status_prohibitP);
}
//-----------------------------------------------------------------------------
void rlc_am_set_debug_infos(rlc_am_entity_t *rlcP, u32 frame, u8_t eNB_flagP, module_id_t module_idP, rb_id_t rb_idP, rb_type_t rb_typeP)
//-----------------------------------------------------------------------------
{
    LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SET DEBUG INFOS] module_id %d rb_id %d rb_type %d\n", frame, module_idP, rb_idP, module_idP, rb_idP, rb_typeP);

    rlcP->module_id = module_idP;
    rlcP->rb_id     = rb_idP;
    if (rb_typeP != SIGNALLING_RADIO_BEARER) {
      rlcP->is_data_plane = 1;
    } else {
      rlcP->is_data_plane = 0;
    }
    rlcP->is_enb = eNB_flagP;
}

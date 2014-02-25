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
void rlc_am_init(rlc_am_entity_t *rlc_pP, frame_t frameP)
//-----------------------------------------------------------------------------
{
    int saved_allocation = rlc_pP->allocation;
    LOG_D(RLC, "[FRAME %5u][RLC_AM][MOD XX][RB XX][INIT] STATE VARIABLES, BUFFERS, LISTS\n", frameP);
    memset(rlc_pP, 0, sizeof(rlc_am_entity_t));
    rlc_pP->allocation = saved_allocation;

    list2_init(&rlc_pP->receiver_buffer,      "RX BUFFER");
    list_init(&rlc_pP->pdus_to_mac_layer,     "PDUS TO MAC");
    list_init(&rlc_pP->control_pdu_list,      "CONTROL PDU LIST");
    list_init(&rlc_pP->segmentation_pdu_list, "SEGMENTATION PDU LIST");
    //LOG_D(RLC,"RLC_AM_SDU_CONTROL_BUFFER_SIZE %d sizeof(rlc_am_tx_sdu_management_t) %d \n",  RLC_AM_SDU_CONTROL_BUFFER_SIZE, sizeof(rlc_am_tx_sdu_management_t));
    
    rlc_pP->input_sdus_alloc         = get_free_mem_block(RLC_AM_SDU_CONTROL_BUFFER_SIZE*sizeof(rlc_am_tx_sdu_management_t));
    rlc_pP->input_sdus               = (rlc_am_tx_sdu_management_t*)((rlc_pP->input_sdus_alloc)->data);
    rlc_pP->pdu_retrans_buffer_alloc = get_free_mem_block((u16_t)((unsigned int)RLC_AM_PDU_RETRANSMISSION_BUFFER_SIZE*(unsigned int)sizeof(rlc_am_tx_data_pdu_management_t)));
    rlc_pP->pdu_retrans_buffer       = (rlc_am_tx_data_pdu_management_t*)((rlc_pP->pdu_retrans_buffer_alloc)->data);
    LOG_D(RLC, "[FRAME %5u][RLC_AM][MOD XX][RB XX][INIT] input_sdus[] = %p  element size=%d\n", frameP, rlc_pP->input_sdus,sizeof(rlc_am_tx_sdu_management_t));
    LOG_D(RLC, "[FRAME %5u][RLC_AM][MOD XX][RB XX][INIT] pdu_retrans_buffer[] = %p element size=%d\n", frameP, rlc_pP->pdu_retrans_buffer,sizeof(rlc_am_tx_data_pdu_management_t));

    // TX state variables
    //rlc_pP->vt_a    = 0;
    rlc_pP->vt_ms   = rlc_pP->vt_a + RLC_AM_WINDOW_SIZE;
    //rlc_pP->vt_s    = 0;
    //rlc_pP->poll_sn = 0;
    // TX counters
    //rlc_pP->c_pdu_without_poll  = 0;
    //rlc_pP->c_byte_without_poll = 0;
    // RX state variables
    //rlc_pP->vr_r    = 0;
    rlc_pP->vr_mr   = rlc_pP->vr_r + RLC_AM_WINDOW_SIZE;
    //rlc_pP->vr_x    = 0;
    //rlc_pP->vr_ms   = 0;
    //rlc_pP->vr_h    = 0;

    rlc_pP->last_frame_status_indication = 123456; // any value > 1
    rlc_pP->first_retrans_pdu_sn         = -1;
}
//-----------------------------------------------------------------------------
void rlc_am_cleanup(rlc_am_entity_t *rlc_pP, frame_t frameP)
//-----------------------------------------------------------------------------
{
    LOG_I(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u][CLEANUP]\n",
          frameP,
          (rlc_pP->is_enb) ? "eNB" : "UE",
          rlc_pP->enb_module_id,
          rlc_pP->ue_module_id,
          rlc_pP->rb_id);

    list2_free(&rlc_pP->receiver_buffer);
    list_free(&rlc_pP->pdus_to_mac_layer);
    list_free(&rlc_pP->control_pdu_list);
    list_free(&rlc_pP->segmentation_pdu_list);


    if (rlc_pP->output_sdu_in_construction != NULL) {
        free_mem_block(rlc_pP->output_sdu_in_construction);
        rlc_pP->output_sdu_in_construction = NULL;
    }
    unsigned int i;
    if (rlc_pP->input_sdus_alloc != NULL) {
        for (i=0; i < RLC_AM_SDU_CONTROL_BUFFER_SIZE; i++) {
            if (rlc_pP->input_sdus[i].mem_block != NULL) {
                free_mem_block(rlc_pP->input_sdus[i].mem_block);
                rlc_pP->input_sdus[i].mem_block = NULL;
            }
        }
        free_mem_block(rlc_pP->input_sdus_alloc);
        rlc_pP->input_sdus_alloc = NULL;
        rlc_pP->input_sdus       = NULL;
    }
    if (rlc_pP->pdu_retrans_buffer_alloc != NULL) {
        for (i=0; i < RLC_AM_PDU_RETRANSMISSION_BUFFER_SIZE; i++) {
            if (rlc_pP->pdu_retrans_buffer[i].mem_block != NULL) {
                free_mem_block(rlc_pP->pdu_retrans_buffer[i].mem_block);
                rlc_pP->pdu_retrans_buffer[i].mem_block = NULL;
            }
        }
        free_mem_block(rlc_pP->pdu_retrans_buffer_alloc);
        rlc_pP->pdu_retrans_buffer_alloc = NULL;
        rlc_pP->pdu_retrans_buffer       = NULL;
    }
    memset(rlc_pP, 0, sizeof(rlc_am_entity_t));
}
//-----------------------------------------------------------------------------
void rlc_am_configure(rlc_am_entity_t *rlc_pP,
		      frame_t          frameP,
                      u16_t            max_retx_thresholdP,
                      u16_t            poll_pduP,
                      u16_t            poll_byteP,
                      u32_t            t_poll_retransmitP,
                      u32_t            t_reorderingP,
                      u32_t            t_status_prohibitP)
//-----------------------------------------------------------------------------
{
    LOG_I(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u][CONFIGURE] max_retx_threshold %d poll_pdu %d poll_byte %d t_poll_retransmit %d t_reordering %d t_status_prohibit %d\n",
          frameP,
          (rlc_pP->is_enb) ? "eNB" : "UE",
          rlc_pP->enb_module_id,
          rlc_pP->ue_module_id,
          rlc_pP->rb_id,
          max_retx_thresholdP,
          poll_pduP,
          poll_byteP,
          t_poll_retransmitP,
          t_reorderingP,
          t_status_prohibitP);

    rlc_pP->max_retx_threshold = max_retx_thresholdP;
    rlc_pP->poll_pdu           = poll_pduP;
    rlc_pP->poll_byte          = poll_byteP;
    rlc_pP->protocol_state     = RLC_DATA_TRANSFER_READY_STATE;

    rlc_am_init_timer_poll_retransmit(rlc_pP, t_poll_retransmitP);
    rlc_am_init_timer_reordering     (rlc_pP, t_reorderingP);
    rlc_am_init_timer_status_prohibit(rlc_pP, t_status_prohibitP);
}
//-----------------------------------------------------------------------------
void rlc_am_set_debug_infos(rlc_am_entity_t *rlc_pP,
                            frame_t          frameP,
                            eNB_flag_t       eNB_flagP,
                            module_id_t      enb_module_idP,
                            module_id_t      ue_module_idP,
                            rb_id_t          rb_idP,
                            rb_type_t        rb_typeP)
//-----------------------------------------------------------------------------
{
    LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u][SET DEBUG INFOS] module_id %d rb_id %d rb_type %d\n",
          frameP,
          (rlc_pP->is_enb) ? "eNB" : "UE",
          rlc_pP->enb_module_id,
          rlc_pP->ue_module_id,
          rb_idP,
          enb_module_idP,
          ue_module_idP,
          rb_idP,
          rb_typeP);

    rlc_pP->enb_module_id = enb_module_idP;
    rlc_pP->ue_module_id  = ue_module_idP;
    rlc_pP->rb_id         = rb_idP;
    if (rb_typeP != SIGNALLING_RADIO_BEARER) {
      rlc_pP->is_data_plane = 1;
    } else {
      rlc_pP->is_data_plane = 0;
    }
    rlc_pP->is_enb = eNB_flagP;
}

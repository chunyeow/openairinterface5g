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
#define RLC_TM_C
//-----------------------------------------------------------------------------
#include "platform_types.h"
#include "platform_constants.h"
//-----------------------------------------------------------------------------
#include "rlc_tm.h"
#include "mac_primitives.h"
#include "rlc_primitives.h"
#include "list.h"
#include "LAYER2/MAC/extern.h"
//-----------------------------------------------------------------------------
void
rlc_tm_send_sdu (rlc_tm_entity_t *rlcP, u32_t frame, u8_t eNB_flag, u8_t error_indicationP, u8 * srcP, u16_t length_in_bitsP)
{
    //-----------------------------------------------------------------------------
    int             length_in_bytes;
    #ifdef DEBUG_RLC_TM_DISPLAY_ASCII_DATA
    int             index;
    #endif
    #ifdef DEBUG_RLC_TM_REASSEMBLY
    msg ("[RLC_TM %p][SEND_SDU] %d bits\n", rlcP, length_in_bitsP);
    #endif
    length_in_bytes = (length_in_bitsP + 7) >> 3;
    if (rlcP->output_sdu_in_construction == NULL) {
        rlcP->output_sdu_in_construction = get_free_mem_block (length_in_bytes);
    }
    if ((rlcP->output_sdu_in_construction)) {
        u8_t eNB_id;
        u8_t UE_id;
        if (eNB_flag == 0) {
          /* FIXME: force send on first eNB */
          eNB_id = 0;
          UE_id = rlcP->module_id - NB_eNB_INST;
        } else {
          UE_id = rlcP->rb_id / NB_RB_MAX;
          eNB_id = rlcP->module_id;
        }
        #ifdef DEBUG_RLC_TM_DISPLAY_ASCII_DATA
        msg ("[RLC_TM %p][SEND_SDU] DATA :", rlcP);
        for (index = 0; index < length_in_bytes; index++) {
            msg ("%c", srcP[index]);
        }
        msg ("\n");
        #endif

        memcpy (&rlcP->output_sdu_in_construction->data[rlcP->output_sdu_size_to_write], srcP, length_in_bytes);

        rlc_data_ind (rlcP->module_id, eNB_id, UE_id, frame, eNB_flag, RLC_MBMS_NO, rlcP->rb_id, length_in_bytes, rlcP->output_sdu_in_construction, rlcP->is_data_plane);
        rlcP->output_sdu_in_construction = NULL;
    } else {
        msg ("[RLC_TM %p][SEND_SDU] ERROR  OUTPUT SDU IS NULL\n", rlcP);
    }
}
//-----------------------------------------------------------------------------
void
rlc_tm_no_segment (rlc_tm_entity_t *rlcP)
{
    //-----------------------------------------------------------------------------
    mem_block_t *pdu;
    struct rlc_tm_tx_sdu_management *sdu_mngt;
    struct rlc_tm_tx_pdu_management *pdu_mngt;
    int             nb_pdu_to_transmit;

    nb_pdu_to_transmit = 1;
    pdu = NULL;

    // only one SDU per TTI
    while ((rlcP->input_sdus[rlcP->current_sdu_index]) && (nb_pdu_to_transmit > 0)) {

        sdu_mngt = ((struct rlc_tm_tx_sdu_management *) (rlcP->input_sdus[rlcP->current_sdu_index]->data));
        //PRINT_RLC_TM_SEGMENT("[RLC_TM %p] SEGMENT GET NEW SDU %p AVAILABLE SIZE %d Bytes\n", rlcP, sdu_mngt, sdu_mngt->sdu_remaining_size);

        if (!(pdu = get_free_mem_block (((rlcP->rlc_pdu_size + 7) >> 3) + sizeof (struct rlc_tm_tx_data_pdu_struct) + GUARD_CRC_LIH_SIZE))) {
            msg ("[RLC_TM %p][SEGMENT] ERROR COULD NOT GET NEW PDU, EXIT\n", rlcP);
            return;
        }
        // SHOULD BE OPTIMIZED...SOON
        pdu_mngt = (struct rlc_tm_tx_pdu_management *) (pdu->data);
        memset (pdu->data, 0, sizeof (struct rlc_tm_tx_pdu_management));
        pdu_mngt->first_byte = (u8_t*)&pdu->data[sizeof (struct rlc_tm_tx_data_pdu_struct)];

        memcpy (pdu_mngt->first_byte, sdu_mngt->first_byte, ((rlcP->rlc_pdu_size + 7) >> 3));
        ((struct mac_tb_req *) (pdu->data))->rlc = NULL;
        ((struct mac_tb_req *) (pdu->data))->data_ptr = pdu_mngt->first_byte;
        ((struct mac_tb_req *) (pdu->data))->first_bit = 0;
        ((struct mac_tb_req *) (pdu->data))->tb_size_in_bits = rlcP->rlc_pdu_size;
        list_add_tail_eurecom (pdu, &rlcP->pdus_to_mac_layer);

        rlcP->buffer_occupancy -= (sdu_mngt->sdu_size >> 3);
        free_mem_block (rlcP->input_sdus[rlcP->current_sdu_index]);
        rlcP->input_sdus[rlcP->current_sdu_index] = NULL;
        rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % rlcP->size_input_sdus_buffer;
        rlcP->nb_sdu -= 1;
    }
}
//-----------------------------------------------------------------------------
void
rlc_tm_rx (void *argP, u32_t frame, u8_t eNB_flag, struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------

  rlc_tm_entity_t *rlc = (rlc_tm_entity_t *) argP;
  mem_block_t *tb;
  u8_t             *first_byte;

    rlc->output_sdu_size_to_write = 0;      // size of sdu reassemblied
    while ((tb = list_remove_head (&data_indP.data))) {
        first_byte = ((struct mac_tb_ind *) (tb->data))->data_ptr;

        ((struct rlc_tm_rx_pdu_management *) (tb->data))->first_byte = first_byte;

        rlc_tm_send_sdu (rlc,  frame, eNB_flag, (((struct mac_tb_ind *) (tb->data))->error_indication), first_byte, data_indP.tb_size);
        free_mem_block (tb);
    }
}

//-----------------------------------------------------------------------------
struct mac_status_resp
rlc_tm_mac_status_indication (void *rlcP, u16 tb_sizeP, struct mac_status_ind tx_statusP)
{
//-----------------------------------------------------------------------------
  struct mac_status_resp status_resp;

  ((rlc_tm_entity_t *) rlcP)->rlc_pdu_size = tb_sizeP;

  status_resp.buffer_occupancy_in_bytes = ((rlc_tm_entity_t *) rlcP)->buffer_occupancy;
  status_resp.buffer_occupancy_in_pdus = status_resp.buffer_occupancy_in_bytes / ((rlc_tm_entity_t *) rlcP)->rlc_pdu_size;
  status_resp.rlc_info.rlc_protocol_state = ((rlc_tm_entity_t *) rlcP)->protocol_state;
  return status_resp;
}

//-----------------------------------------------------------------------------
struct mac_data_req
rlc_tm_mac_data_request (void *rlcP, u32_t frame)
{
//-----------------------------------------------------------------------------
  rlc_tm_entity_t *l_rlc = (rlc_tm_entity_t *) rlcP;
  struct mac_data_req data_req;

  rlc_tm_no_segment (l_rlc);
  list_init (&data_req.data, NULL);
  list_add_list (&l_rlc->pdus_to_mac_layer, &data_req.data);

  data_req.buffer_occupancy_in_bytes = l_rlc->buffer_occupancy;
  data_req.buffer_occupancy_in_pdus = data_req.buffer_occupancy_in_bytes / l_rlc->rlc_pdu_size;
  data_req.rlc_info.rlc_protocol_state = l_rlc->protocol_state;
  if (data_req.data.nb_elements > 0) {
      LOG_D(RLC, "[RLC_TM][MOD %d][RB %d][FRAME %05d] MAC_DATA_REQUEST %d TBs\n", l_rlc->module_id, l_rlc->rb_id, frame, data_req.data.nb_elements);
      mem_block_t *tb;
      rlc[l_rlc->module_id].m_mscgen_trace_length = sprintf(rlc[l_rlc->module_id].m_mscgen_trace, "[MSC_MSG][FRAME %05d][RLC_UM][MOD %02d][RB %02d][--- MAC_DATA_REQ/ %d TB(s) ",
              frame,
              l_rlc->module_id,
              l_rlc->rb_id,
              data_req.data.nb_elements);

      tb = data_req.data.head;
      while (tb != NULL) {
          rlc[l_rlc->module_id].m_mscgen_trace_length += sprintf(&rlc[l_rlc->module_id].m_mscgen_trace[rlc[l_rlc->module_id].m_mscgen_trace_length], "%d Bytes ",
                                                                 ((struct mac_tb_req *) (tb->data))->tb_size_in_bits>>3);
          tb = tb->next;
      }
      rlc[l_rlc->module_id].m_mscgen_trace_length += sprintf(&rlc[l_rlc->module_id].m_mscgen_trace[rlc[l_rlc->module_id].m_mscgen_trace_length], "BO=%d --->][MAC_%s][MOD %02d][]\n",
            data_req.buffer_occupancy_in_bytes,
            (l_rlc->is_enb) ? "eNB":"UE",
            l_rlc->module_id);
      rlc[l_rlc->module_id].m_mscgen_trace[rlc[l_rlc->module_id].m_mscgen_trace_length] = 0;
      LOG_D(RLC, "%s", rlc[l_rlc->module_id].m_mscgen_trace);
  }

  return data_req;
}

//-----------------------------------------------------------------------------
void
rlc_tm_mac_data_indication (void *rlcP, u32_t frame, u8_t eNB_flag, struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------
    rlc_tm_entity_t *l_rlc = (rlc_tm_entity_t *) rlcP;
    mem_block_t     *tb;

    if (data_indP.data.nb_elements > 0) {
        LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] MAC_DATA_IND %d TBs\n", l_rlc->module_id, l_rlc->rb_id, frame, data_indP.data.nb_elements);
        rlc[l_rlc->module_id].m_mscgen_trace_length = sprintf(rlc[l_rlc->module_id].m_mscgen_trace, "[MSC_MSG][FRAME %05d][MAC_%s][MOD %02d][][--- MAC_DATA_IND/ %d TB(s) ",
              frame,
              (l_rlc->is_enb) ? "eNB":"UE",
              l_rlc->module_id,
              data_indP.data.nb_elements);

        tb = data_indP.data.head;
        while (tb != NULL) {
            rlc[l_rlc->module_id].m_mscgen_trace_length += sprintf(&rlc[l_rlc->module_id].m_mscgen_trace[rlc[l_rlc->module_id].m_mscgen_trace_length], "%d Bytes ",
                                                                 ((struct mac_tb_ind *) (tb->data))->size);
            tb = tb->next;
        }
        rlc[l_rlc->module_id].m_mscgen_trace_length += sprintf(&rlc[l_rlc->module_id].m_mscgen_trace[rlc[l_rlc->module_id].m_mscgen_trace_length], " --->][RLC_TM][MOD %02d][RB %02d]\n",
            l_rlc->module_id,
            l_rlc->rb_id);

        rlc[l_rlc->module_id].m_mscgen_trace[rlc[l_rlc->module_id].m_mscgen_trace_length] = 0;
        LOG_D(RLC, "%s", rlc[l_rlc->module_id].m_mscgen_trace);
    }
    rlc_tm_rx (rlcP, frame, eNB_flag, data_indP);
}

//-----------------------------------------------------------------------------
void
rlc_tm_data_req (void *rlcP, mem_block_t *sduP)
{
//-----------------------------------------------------------------------------
  rlc_tm_entity_t *rlc = (rlc_tm_entity_t *) rlcP;

  #ifdef DEBUG_RLC_TM_DATA_REQUEST
  msg ("[RLC_TM %p] RLC_TM_DATA_REQ size %d Bytes, BO %ld , NB SDU %d current_sdu_index=%d next_sdu_index=%d\n", rlc, ((struct rlc_um_data_req *) (sduP->data))->data_size, rlc->buffer_occupancy, rlc->nb_sdu, rlc->current_sdu_index, rlc->next_sdu_index);
  #endif

  // not in 3GPP specification but the buffer may be full if not correctly configured
  if (rlc->input_sdus[rlc->next_sdu_index] == NULL) {
    ((struct rlc_tm_tx_sdu_management *) (sduP->data))->sdu_size = ((struct rlc_tm_data_req *) (sduP->data))->data_size;
    rlc->buffer_occupancy += ((struct rlc_tm_tx_sdu_management *) (sduP->data))->sdu_size >> 3;
    rlc->nb_sdu += 1;
    ((struct rlc_tm_tx_sdu_management *) (sduP->data))->first_byte = (u8*)&sduP->data[sizeof (struct rlc_tm_data_req_alloc)];
    rlc->input_sdus[rlc->next_sdu_index] = sduP;
    rlc->next_sdu_index = (rlc->next_sdu_index + 1) % rlc->size_input_sdus_buffer;
  } else {
    free_mem_block (sduP);
  }
}

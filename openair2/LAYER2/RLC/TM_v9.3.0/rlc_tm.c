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
rlc_tm_send_sdu (
        rlc_tm_entity_t * const rlc_pP,
        const frame_t           frameP,
        const eNB_flag_t        eNB_flag,
        const boolean_t         error_indicationP,
        uint8_t * const         srcP,
        const sdu_size_t        length_in_bitsP)
{
    //-----------------------------------------------------------------------------
    int             length_in_bytes;
    #ifdef DEBUG_RLC_TM_DISPLAY_ASCII_DATA
    int             index;
    #endif
    #ifdef DEBUG_RLC_TM_REASSEMBLY
    msg ("[RLC_TM %p][SEND_SDU] %d bits\n", rlc_pP, length_in_bitsP);
    #endif
    length_in_bytes = (length_in_bitsP + 7) >> 3;
    if (rlc_pP->output_sdu_in_construction == NULL) {
        rlc_pP->output_sdu_in_construction = get_free_mem_block (length_in_bytes);
    }
    if ((rlc_pP->output_sdu_in_construction)) {
        #ifdef DEBUG_RLC_TM_DISPLAY_ASCII_DATA
        msg ("[RLC_TM %p][SEND_SDU] DATA :", rlc_pP);
        for (index = 0; index < length_in_bytes; index++) {
            msg ("%c", srcP[index]);
        }
        msg ("\n");
        #endif

        memcpy (&rlc_pP->output_sdu_in_construction->data[rlc_pP->output_sdu_size_to_write], srcP, length_in_bytes);

        rlc_data_ind (
            rlc_pP->enb_module_id,
            rlc_pP->ue_module_id,
            frameP,
            eNB_flag,
            BOOL_NOT(rlc_pP->is_data_plane),
            MBMS_FLAG_NO,
            rlc_pP->rb_id,
            length_in_bytes,
            rlc_pP->output_sdu_in_construction);
        rlc_pP->output_sdu_in_construction = NULL;
    } else {
        msg ("[RLC_TM %p][SEND_SDU] ERROR  OUTPUT SDU IS NULL\n", rlc_pP);
    }
}
//-----------------------------------------------------------------------------
void
rlc_tm_no_segment (
        rlc_tm_entity_t *const rlc_pP
        )
{
    //-----------------------------------------------------------------------------
    mem_block_t                     *pdu_p               = NULL;
    struct rlc_tm_tx_sdu_management *sdu_mngt_p          = NULL;
    struct rlc_tm_tx_pdu_management *pdu_mngt_p          = NULL;
    int                              nb_pdu_to_transmit  = 1;

    // only one SDU per TTI
    while ((rlc_pP->input_sdus[rlc_pP->current_sdu_index]) && (nb_pdu_to_transmit > 0)) {

        sdu_mngt_p = ((struct rlc_tm_tx_sdu_management *) (rlc_pP->input_sdus[rlc_pP->current_sdu_index]->data));
        //PRINT_RLC_TM_SEGMENT("[RLC_TM %p] SEGMENT GET NEW SDU %p AVAILABLE SIZE %d Bytes\n", rlc_pP, sdu_mngt_p, sdu_mngt_p->sdu_remaining_size);

        if (!(pdu_p = get_free_mem_block (((rlc_pP->rlc_pdu_size + 7) >> 3) + sizeof (struct rlc_tm_tx_data_pdu_struct) + GUARD_CRC_LIH_SIZE))) {
            msg ("[RLC_TM %p][SEGMENT] ERROR COULD NOT GET NEW PDU, EXIT\n", rlc_pP);
            return;
        }
        // SHOULD BE OPTIMIZED...SOON
        pdu_mngt_p = (struct rlc_tm_tx_pdu_management *) (pdu_p->data);
        memset (pdu_p->data, 0, sizeof (struct rlc_tm_tx_pdu_management));
        pdu_mngt_p->first_byte = (uint8_t*)&pdu_p->data[sizeof (struct rlc_tm_tx_data_pdu_struct)];

        memcpy (pdu_mngt_p->first_byte, sdu_mngt_p->first_byte, ((rlc_pP->rlc_pdu_size + 7) >> 3));
        ((struct mac_tb_req *) (pdu_p->data))->rlc = NULL;
        ((struct mac_tb_req *) (pdu_p->data))->data_ptr = pdu_mngt_p->first_byte;
        ((struct mac_tb_req *) (pdu_p->data))->first_bit = 0;
        ((struct mac_tb_req *) (pdu_p->data))->tb_size = rlc_pP->rlc_pdu_size >> 3;
        list_add_tail_eurecom (pdu_p, &rlc_pP->pdus_to_mac_layer);

        rlc_pP->buffer_occupancy -= (sdu_mngt_p->sdu_size >> 3);
        free_mem_block (rlc_pP->input_sdus[rlc_pP->current_sdu_index]);
        rlc_pP->input_sdus[rlc_pP->current_sdu_index] = NULL;
        rlc_pP->current_sdu_index = (rlc_pP->current_sdu_index + 1) % rlc_pP->size_input_sdus_buffer;
        rlc_pP->nb_sdu -= 1;
    }
}
//-----------------------------------------------------------------------------
void
rlc_tm_rx (
        void *const         argP,
        const frame_t       frameP,
        const eNB_flag_t    eNB_flagP,
        struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------

  rlc_tm_entity_t     * const rlc_p = (rlc_tm_entity_t *) argP;
  mem_block_t         *tb_p;
  uint8_t             *first_byte_p;

    rlc_p->output_sdu_size_to_write = 0;      // size of sdu reassemblied
    while ((tb_p = list_remove_head (&data_indP.data))) {
        first_byte_p = ((struct mac_tb_ind *) (tb_p->data))->data_ptr;

        ((struct rlc_tm_rx_pdu_management *) (tb_p->data))->first_byte = first_byte_p;

        rlc_tm_send_sdu (rlc_p,  frameP, eNB_flagP, (((struct mac_tb_ind *) (tb_p->data))->error_indication), first_byte_p, data_indP.tb_size);
        free_mem_block (tb_p);
    }
}

//-----------------------------------------------------------------------------
struct mac_status_resp
rlc_tm_mac_status_indication (
        void *const           rlc_pP,
        const tb_size_t       tb_sizeP,
        struct mac_status_ind tx_statusP)
{
//-----------------------------------------------------------------------------
  struct mac_status_resp status_resp;

  ((rlc_tm_entity_t *) rlc_pP)->rlc_pdu_size = tb_sizeP;

  status_resp.buffer_occupancy_in_bytes = ((rlc_tm_entity_t *) rlc_pP)->buffer_occupancy;
  status_resp.buffer_occupancy_in_pdus = status_resp.buffer_occupancy_in_bytes / ((rlc_tm_entity_t *) rlc_pP)->rlc_pdu_size;
  status_resp.rlc_info.rlc_protocol_state = ((rlc_tm_entity_t *) rlc_pP)->protocol_state;
  return status_resp;
}

//-----------------------------------------------------------------------------
struct mac_data_req
rlc_tm_mac_data_request (
        void * const rlc_pP,
        const frame_t frameP)
{
//-----------------------------------------------------------------------------
  rlc_tm_entity_t    *l_rlc_p = (rlc_tm_entity_t *) rlc_pP;
  struct mac_data_req data_req;

  rlc_tm_no_segment (l_rlc_p);
  list_init (&data_req.data, NULL);
  list_add_list (&l_rlc_p->pdus_to_mac_layer, &data_req.data);

  data_req.buffer_occupancy_in_bytes = l_rlc_p->buffer_occupancy;
  data_req.buffer_occupancy_in_pdus = data_req.buffer_occupancy_in_bytes / l_rlc_p->rlc_pdu_size;
  data_req.rlc_info.rlc_protocol_state = l_rlc_p->protocol_state;
  if (data_req.data.nb_elements > 0) {
      LOG_D(RLC, "[RLC_TM][%s][MOD %02u/%02u][RB %d][FRAME %05d] MAC_DATA_REQUEST %d TBs\n",
            (l_rlc_p->is_enb) ? "eNB" : "UE",
            l_rlc_p->enb_module_id,
            l_rlc_p->ue_module_id,
            l_rlc_p->rb_id,
            frameP,
            data_req.data.nb_elements);
  }

  return data_req;
}

//-----------------------------------------------------------------------------
void
rlc_tm_mac_data_indication (
        void * const        rlc_pP,
        const frame_t       frameP,
        const eNB_flag_t    eNB_flag,
        struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------
    rlc_tm_entity_t *l_rlc_p = (rlc_tm_entity_t *) rlc_pP;

    if (data_indP.data.nb_elements > 0) {
        LOG_D(RLC, "[RLC_TM][%s][MOD %02u/%02u][RB %d][FRAME %05d] MAC_DATA_IND %d TBs\n",
              (l_rlc_p->is_enb) ? "eNB" : "UE",
              l_rlc_p->enb_module_id,
              l_rlc_p->ue_module_id,
              l_rlc_p->rb_id,
              frameP,
              data_indP.data.nb_elements);
    }
    rlc_tm_rx (rlc_pP, frameP, eNB_flag, data_indP);
}

//-----------------------------------------------------------------------------
void
rlc_tm_data_req (
        void *const rlc_pP,
        mem_block_t *const sdu_pP)
{
//-----------------------------------------------------------------------------
  rlc_tm_entity_t *rlc_p = (rlc_tm_entity_t *) rlc_pP;

  #ifdef DEBUG_RLC_TM_DATA_REQUEST
  LOG_D (RLC, "[RLC_TM][%s][MOD %02u/%02u] RLC_TM_DATA_REQ size %d Bytes, BO %ld , NB SDU %d current_sdu_index=%d next_sdu_index=%d\n",
         (l_rlc_p->is_enb) ? "eNB" : "UE",
         l_rlc_p->enb_module_id,
         l_rlc_p->ue_module_id,
         ((struct rlc_um_data_req *) (sdu_pP->data))->data_size,
         rlc_p->buffer_occupancy,
         rlc_p->nb_sdu,
         rlc_p->current_sdu_index,
        rlc_p->next_sdu_index);
  #endif

  // not in 3GPP specification but the buffer may be full if not correctly configured
  if (rlc_p->input_sdus[rlc_p->next_sdu_index] == NULL) {
    ((struct rlc_tm_tx_sdu_management *) (sdu_pP->data))->sdu_size = ((struct rlc_tm_data_req *) (sdu_pP->data))->data_size;
    rlc_p->buffer_occupancy += ((struct rlc_tm_tx_sdu_management *) (sdu_pP->data))->sdu_size >> 3;
    rlc_p->nb_sdu += 1;
    ((struct rlc_tm_tx_sdu_management *) (sdu_pP->data))->first_byte = (uint8_t*)&sdu_pP->data[sizeof (struct rlc_tm_data_req_alloc)];
    rlc_p->input_sdus[rlc_p->next_sdu_index] = sdu_pP;
    rlc_p->next_sdu_index = (rlc_p->next_sdu_index + 1) % rlc_p->size_input_sdus_buffer;
  } else {
    free_mem_block (sdu_pP);
  }
}

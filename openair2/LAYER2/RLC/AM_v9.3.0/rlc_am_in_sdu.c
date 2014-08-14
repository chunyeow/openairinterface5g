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
/***************************************************************************
                          rlc_am_in_sdu.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr
 ***************************************************************************/
#define RLC_AM_MODULE
#define RLC_AM_IN_SDU_C
//-----------------------------------------------------------------------------
#include "rlc_am.h"
#include "LAYER2/MAC/extern.h"
#include "UTIL/LOG/log.h"

#define TRACE_RLC_AM_FREE_SDU
//-----------------------------------------------------------------------------
void rlc_am_free_in_sdu(
        rlc_am_entity_t *const rlcP,
        const frame_t frameP,
        const unsigned int index_in_bufferP)
//-----------------------------------------------------------------------------
{
    if (index_in_bufferP <= RLC_AM_SDU_CONTROL_BUFFER_SIZE) {
        if (rlcP->input_sdus[index_in_bufferP].mem_block != NULL) {
            free_mem_block(rlcP->input_sdus[index_in_bufferP].mem_block);
            rlcP->input_sdus[index_in_bufferP].mem_block = NULL;
            rlcP->nb_sdu_no_segmented -= 1;
            rlcP->input_sdus[index_in_bufferP].sdu_remaining_size = 0;
        }
        rlcP->nb_sdu -= 1;
        memset(&rlcP->input_sdus[index_in_bufferP], 0, sizeof(rlc_am_tx_sdu_management_t));
        rlcP->input_sdus[index_in_bufferP].flags.transmitted_successfully = 1;

        if (rlcP->current_sdu_index == index_in_bufferP) {
            rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % RLC_AM_SDU_CONTROL_BUFFER_SIZE;
        }
        while ((rlcP->current_sdu_index != rlcP->next_sdu_index) &&
            (rlcP->input_sdus[rlcP->current_sdu_index].flags.transmitted_successfully == 1)) {
            rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % RLC_AM_SDU_CONTROL_BUFFER_SIZE;
        }
    }
#ifdef TRACE_RLC_AM_FREE_SDU
    LOG_D(RLC, "[FRAME %05d][%s][RLC_AM][MOD %u/%u][RB %u][FREE SDU] SDU INDEX %03d current_sdu_index=%d next_sdu_index=%d nb_sdu_no_segmented=%d\n",
          frameP,
          (rlcP->is_enb) ? "eNB" : "UE",
          rlcP->enb_module_id,
          rlcP->ue_module_id,
          rlcP->rb_id,
          index_in_bufferP,
          rlcP->current_sdu_index,
          rlcP->next_sdu_index,
          rlcP->nb_sdu_no_segmented);
#endif
}
// called when segmentation is done
//-----------------------------------------------------------------------------
void rlc_am_free_in_sdu_data(
        rlc_am_entity_t *const rlcP,
        const unsigned int index_in_bufferP)
//-----------------------------------------------------------------------------
{
    if (index_in_bufferP <= RLC_AM_SDU_CONTROL_BUFFER_SIZE) {
        if (rlcP->input_sdus[index_in_bufferP].mem_block != NULL) {
            free_mem_block(rlcP->input_sdus[index_in_bufferP].mem_block);
            rlcP->input_sdus[index_in_bufferP].mem_block = NULL;
            rlcP->input_sdus[index_in_bufferP].sdu_remaining_size = 0;
            rlcP->nb_sdu_no_segmented -= 1;
        }
    }
}
//-----------------------------------------------------------------------------
signed int rlc_am_in_sdu_is_empty(rlc_am_entity_t *const rlcP)
//-----------------------------------------------------------------------------
{
    if (rlcP->nb_sdu == 0) {
            return 1;
    }
    return 0;
}

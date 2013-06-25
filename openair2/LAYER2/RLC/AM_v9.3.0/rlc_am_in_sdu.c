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
void rlc_am_free_in_sdu(rlc_am_entity_t *rlcP, u32_t frame, unsigned int index_in_bufferP)
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
    LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][FREE SDU] SDU INDEX %03d current_sdu_index=%d next_sdu_index=%d nb_sdu_no_segmented=%d\n", frame, rlcP->module_id, rlcP->rb_id, index_in_bufferP, rlcP->current_sdu_index, rlcP->next_sdu_index, rlcP->nb_sdu_no_segmented);
#endif
}
// called when segmentation is done
//-----------------------------------------------------------------------------
void rlc_am_free_in_sdu_data(rlc_am_entity_t *rlcP, unsigned int index_in_bufferP)
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
signed int rlc_am_in_sdu_is_empty(rlc_am_entity_t *rlcP)
//-----------------------------------------------------------------------------
{
    if (rlcP->nb_sdu == 0) {
            return 1;
    }
    return 0;
}

  //u8_t                       in_sdu_data_ring_buffer   [RLC_AM_SDU_DATA_BUFFER_SIZE];
  //rlc_am_in_sdu_control_t    in_sdu_control_ring_buffer[RLC_AM_SDU_CONTROL_BUFFER_SIZE];

  //signed   int               in_sdu_data_buffer_index_start;
  //signed   int               in_sdu_data_buffer_index_end;
  //signed   int               in_sdu_data_buffer_index_next;

  //signed   int               in_sdu_control_buffer_index_start;
  //signed   int               in_sdu_control_buffer_index_end;
  //signed   int               in_sdu_control_buffer_index_next;

/*//-----------------------------------------------------------------------------
signed int rlc_am_in_sdu_data_get_available_size(rlc_am_entity_t *rlcP)
//-----------------------------------------------------------------------------
{
    signed int index_diff =  rlcP->in_sdu_data_buffer_index_next - rlcP->in_sdu_data_buffer_index_start;
    if (index_diff > 0) {
        return RLC_AM_SDU_DATA_BUFFER_SIZE - index_diff;
    } else {
        return 0 - index_diff;
    }
}
//-----------------------------------------------------------------------------
signed int rlc_am_in_sdu_control_get_available_size(rlc_am_entity_t *rlcP)
//-----------------------------------------------------------------------------
{
    signed int index_diff =  rlcP->in_sdu_control_buffer_index_next - rlcP->in_sdu_control_buffer_index_start;
    if (index_diff > 0) {
        return RLC_AM_SDU_CONTROL_BUFFER_SIZE - index_diff;
    } else {
        return 0 - index_diff;
    }
}
//-----------------------------------------------------------------------------
void rlc_am_in_sdu_data_copy_sdu(rlc_am_entity_t *rlcP, char* sourceP, unsigned int sizeP, unsigned int muiP)
//-----------------------------------------------------------------------------
{
    signed int index_diff =  rlcP->in_sdu_data_buffer_index_next - rlcP->in_sdu_data_buffer_index_start;

    if (index_diff > 0) {
        signed int available_size_first_write = RLC_AM_SDU_DATA_BUFFER_SIZE - rlcP->in_sdu_data_buffer_index_next;
        if (sizeP <= available_size_first_write) {
            memcpy(&rlcP->in_sdu_data_ring_buffer[rlcP->in_sdu_data_buffer_index_next],
                   sourceP,
                   sizeP);
        } else {
            memcpy(&rlcP->in_sdu_data_ring_buffer[rlcP->in_sdu_data_buffer_index_next],
                   sourceP,
                   available_size_first_write);

            memcpy(&rlcP->in_sdu_data_ring_buffer[0],
                   &sourceP[available_size_first_write],
                   sizeP - available_size_first_write);
        }
    } else {
        memcpy(&rlcP->in_sdu_data_ring_buffer[rlcP->in_sdu_data_buffer_index_next],
                   sourceP,
                   sizeP);
    }

    rlcP->in_sdu_control_ring_buffer[rlcP->in_sdu_control_buffer_index_next].sdu_data_buffer_index_start = rlcP->in_sdu_data_buffer_index_next;
    rlcP->in_sdu_control_ring_buffer[rlcP->in_sdu_control_buffer_index_next].sdu_data_buffer_index_end   = (rlcP->in_sdu_data_buffer_index_end + sizeP) % RLC_AM_SDU_DATA_BUFFER_SIZE;
    rlcP->in_sdu_control_ring_buffer[rlcP->in_sdu_control_buffer_index_next].sdu_size                    = sizeP;
    rlcP->in_sdu_control_ring_buffer[rlcP->in_sdu_control_buffer_index_next].sdu_mui                     = muiP;
    rlcP->in_sdu_control_ring_buffer[rlcP->in_sdu_control_buffer_index_next].sdu_segmented_size          = 0;
    rlcP->in_sdu_control_ring_buffer[rlcP->in_sdu_control_buffer_index_next].nb_pdus                     = 0;
    rlcP->in_sdu_control_ring_buffer[rlcP->in_sdu_control_buffer_index_next].nb_pdus_ack                 = 0;
    rlcP->in_sdu_control_ring_buffer[rlcP->in_sdu_control_buffer_index_next].segmented                   = 0;
    rlcP->in_sdu_control_ring_buffer[rlcP->in_sdu_control_buffer_index_next].discarded                   = 0;

    rlcP->in_sdu_data_buffer_index_end     = (rlcP->in_sdu_data_buffer_index_end + sizeP) % RLC_AM_SDU_DATA_BUFFER_SIZE;
    rlcP->in_sdu_data_buffer_index_next    = (rlcP->in_sdu_data_buffer_index_end + 1)     % RLC_AM_SDU_DATA_BUFFER_SIZE;
    rlcP->in_sdu_control_buffer_index_end  = (rlcP->in_sdu_control_buffer_index_end + 1)  % RLC_AM_SDU_CONTROL_BUFFER_SIZE;
    rlcP->in_sdu_control_buffer_index_next = (rlcP->in_sdu_control_buffer_index_end + 1)  % RLC_AM_SDU_CONTROL_BUFFER_SIZE;

    //rlc->stat_tx_pdcp_sdu += 1;
}
//-----------------------------------------------------------------------------
void rlc_am_in_sdu_data_req (rlc_am_entity_t *rlcP, mem_block_t * sduP)
//-----------------------------------------------------------------------------
{
  u32_t             mui;
  u16_t             data_offset;
  u16_t             data_size;

  if (rlcP->protocol_state == RLC_NULL_STATE) {
#ifdef DEBUG_RLC_AM_DATA_REQUEST
      msg ("[RLC_AM] RLC_AM_DATA_REQ RLC NOT INITIALIZED, DISCARD SDU\n");
#endif
    free_mem_block (sduP);
  }

  signed int size_available_in_data_buffer    = rlc_am_in_sdu_data_get_available_size();
  signed int size_available_in_control_buffer = rlc_am_in_sdu_control_get_available_size();

  if ((size_available_in_control_buffer > 0) && (size_available_in_control_buffer > 0)){
      // parameters from SDU
      mui = ((struct rlc_am_data_req *) (sduP->data))->mui;
      data_offset = ((struct rlc_am_data_req *) (sduP->data))->data_offset;
      data_size = ((struct rlc_am_data_req *) (sduP->data))->data_size;


      rlc_am_in_sdu_data_copy_sdu(rlcP, &sduP->data[data_offset], data_size, mui);

  } else {
#ifdef DEBUG_RLC_AM_DATA_REQUEST
      msg ("[RLC_AM][RB %d] RLC_AM_DATA_REQ BUFFER FULL, NB SDU %d current_sdu_index=%d next_sdu_index=%d size_input_sdus_buffer=%d\n",
           rlcP->rb_id, size_available_in_control_buffer, rlcP->in_sdu_control_buffer_index_start, rlcP->in_sdu_control_buffer_index_end, size_available_in_data_buffer);
#endif
    rlc->stat_tx_pdcp_sdu_discarded += 1;
    free_mem_block (sduP);
  }
}*/

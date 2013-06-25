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
                          rlc_tm_segment.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
//#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "list.h"
#include "rlc_tm_entity.h"
#include "rlc_tm_structs.h"
#include "rlc_primitives.h"
#include "mem_block.h"
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
rlc_tm_no_segment (struct rlc_tm_entity *rlcP)
{
//-----------------------------------------------------------------------------
  mem_block_t *pdu;
  struct rlc_tm_tx_sdu_management *sdu_mngt;
  struct rlc_tm_tx_pdu_management *pdu_mngt;
  int             discard_go_on;
  int             nb_pdu_to_transmit;

  nb_pdu_to_transmit = rlcP->nb_pdu_requested_by_mac;
  pdu = NULL;

  if ((rlcP->sdu_discard_mode & RLC_SDU_DISCARD_TIMER_BASED_NO_EXPLICIT)) {
    discard_go_on = 1;
    while ((rlcP->input_sdus[rlcP->current_sdu_index]) && discard_go_on) {
      if ((*rlcP->frame_tick_milliseconds - ((struct rlc_tm_tx_sdu_management *) (rlcP->input_sdus[rlcP->current_sdu_index]->data))->sdu_creation_time) >= rlcP->timer_discard_init) {
        #ifdef DEBUG_RLC_TM_DISCARD_SDU
        msg("[RLC_TM %p] SDU DISCARDED  TIMED OUT %ld ms ", rlcP, (*rlcP->frame_tick_milliseconds - ((struct rlc_tm_tx_sdu_management *) (rlcP->input_sdus[rlcP->current_sdu_index]->data))->sdu_creation_time));
        msg("BO %d, NB SDU %d\n", rlcP->buffer_occupancy, rlcP->nb_sdu);
        #endif
        rlcP->buffer_occupancy -= (((struct rlc_tm_tx_sdu_management *) (rlcP->input_sdus[rlcP->current_sdu_index]->data))->sdu_size >> 3);
        rlcP->nb_sdu -= 1;
        free_mem_block (rlcP->input_sdus[rlcP->current_sdu_index]);
        rlcP->input_sdus[rlcP->current_sdu_index] = NULL;
        rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % rlcP->size_input_sdus_buffer;
      } else {
        discard_go_on = 0;
      }
    }
  }
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
rlc_tm_segment (struct rlc_tm_entity *rlcP)
{
//-----------------------------------------------------------------------------
  mem_block_t *pdu;
  struct rlc_tm_tx_sdu_management *sdu_mngt;
  struct rlc_tm_tx_pdu_management *pdu_mngt;
  int             discard_go_on;
  int             nb_pdu_to_transmit;

  nb_pdu_to_transmit = rlcP->nb_pdu_requested_by_mac;
  pdu = NULL;

  if ((rlcP->sdu_discard_mode & RLC_SDU_DISCARD_TIMER_BASED_NO_EXPLICIT)) {
    discard_go_on = 1;
    while ((rlcP->input_sdus[rlcP->current_sdu_index]) && discard_go_on) {
      if ((*rlcP->frame_tick_milliseconds - ((struct rlc_tm_tx_sdu_management *) (rlcP->input_sdus[rlcP->current_sdu_index]->data))->sdu_creation_time) >= rlcP->timer_discard_init) {
        #ifdef DEBUG_RLC_TM_DISCARD_SDU
        msg("[RLC_TM %p] SDU DISCARDED  TIMED OUT %ld ms ", rlcP,(*rlcP->frame_tick_milliseconds - ((struct rlc_tm_tx_sdu_management *) (rlcP->input_sdus[rlcP->current_sdu_index]->data))->sdu_creation_time));
        msg ("BO %d, NB SDU %d\n", rlcP->buffer_occupancy, rlcP->nb_sdu);
        #endif
        rlcP->nb_sdu -= 1;
        free_mem_block (rlcP->input_sdus[rlcP->current_sdu_index]);
        rlcP->input_sdus[rlcP->current_sdu_index] = NULL;
        rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % rlcP->size_input_sdus_buffer;
      } else {
        discard_go_on = 0;
      }
    }
  }
  // only one SDU per TTI
  if ((rlcP->input_sdus[rlcP->current_sdu_index])) {
    sdu_mngt = (struct rlc_tm_tx_sdu_management *) (rlcP->input_sdus[rlcP->current_sdu_index]->data);
    //PRINT_RLC_TM_SEGMENT("[RLC_TM %p] SEGMENT GET NEW SDU %p AVAILABLE SIZE %d Bytes\n", rlcP, sdu_mngt, sdu_mngt->sdu_remaining_size);
    while ((nb_pdu_to_transmit > 0) && (sdu_mngt->sdu_segmented_size < sdu_mngt->sdu_size)) {

      if (!(pdu = get_free_mem_block (((rlcP->rlc_pdu_size + 7) >> 3) + 1 + sizeof (struct rlc_tm_tx_data_pdu_struct) + GUARD_CRC_LIH_SIZE))) {
        msg ("[RLC_TM %p][SEGMENT] ERROR COULD NOT GET NEW PDU, EXIT\n", rlcP);
        return;
      }
      // SHOULD BE OPTIMIZED...SOON
      pdu_mngt = (struct rlc_tm_tx_pdu_management *) (pdu->data);
      memset (pdu->data, 0, sizeof (struct rlc_tm_tx_pdu_management));
      pdu_mngt->first_byte = (u8_t*)&pdu->data[sizeof (struct rlc_tm_tx_data_pdu_struct)];

      memcpy (pdu_mngt->first_byte, &sdu_mngt->first_byte[sdu_mngt->sdu_segmented_size >> 3], ((rlcP->rlc_pdu_size + 7) >> 3));

      ((struct mac_tb_req *) (pdu->data))->rlc = NULL;
      ((struct mac_tb_req *) (pdu->data))->data_ptr = pdu_mngt->first_byte;
      ((struct mac_tb_req *) (pdu->data))->first_bit = sdu_mngt->sdu_segmented_size & 0x07;
      ((struct mac_tb_req *) (pdu->data))->tb_size_in_bits = rlcP->rlc_pdu_size;
      list_add_tail_eurecom (pdu, &rlcP->pdus_to_mac_layer);
      sdu_mngt->sdu_segmented_size += rlcP->rlc_pdu_size;
      nb_pdu_to_transmit -= 1;
    }
    free_mem_block (rlcP->input_sdus[rlcP->current_sdu_index]);
    rlcP->input_sdus[rlcP->current_sdu_index] = NULL;
    rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % rlcP->size_input_sdus_buffer;
    rlcP->nb_sdu -= 1;
  }
  if ((rlcP->input_sdus[rlcP->current_sdu_index])) {
    rlcP->buffer_occupancy = ((struct rlc_tm_tx_sdu_management *) (rlcP->input_sdus[rlcP->current_sdu_index]->data))->sdu_size >> 3;
  } else {
    rlcP->buffer_occupancy = 0;
  }
}

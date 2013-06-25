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
                          rlc_tm.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "rlc_tm_entity.h"
#include "mac_primitives.h"
#include "rlc_primitives.h"
#include "rlc_tm_control_primitives_proto_extern.h"
#include "rlc_tm_segment_proto_extern.h"
#include "rlc_tm_reassembly_proto_extern.h"
#include "list.h"
#include "mem_block.h"
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void           *rlc_tm_tx (void *argP);
void            rlc_tm_rx_no_segment (void *argP, struct mac_data_ind data_indP);
void            rlc_tm_rx_segment (void *argP, struct mac_data_ind data_indP);
//-----------------------------------------------------------------------------
void
rlc_tm_get_pdus (void *argP)
{
//-----------------------------------------------------------------------------

  struct rlc_tm_entity *rlc = (struct rlc_tm_entity *) argP;

  switch (rlc->protocol_state) {

      case RLC_NULL_STATE:
        // from 3GPP TS 25.322 V4.2.0
        // In the NULL state the RLC entity does not exist and therefore it is not possible to transfer any data through it.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating establishment, the RLC entity:
        // -      is created; and
        // -      enters the DATA_TRANSFER_READY state.
        break;

      case RLC_DATA_TRANSFER_READY_STATE:

        rlc->segmentation (rlc);
        break;

      default:
        msg ("[RLC_TM %p] MAC_DATA_REQ UNKNOWN PROTOCOL STATE %02X hex\n", rlc, rlc->protocol_state);
  }
}

//-----------------------------------------------------------------------------
void
rlc_tm_rx_no_segment (void *argP, struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------

  struct rlc_tm_entity *rlc = (struct rlc_tm_entity *) argP;
  mem_block_t *tb;
  u8_t             *first_byte;
  u8_t              tb_size_in_bytes;
  u8_t              first_bit;
  u8_t              bits_to_shift;
  u8_t              bits_to_shift_last_loop;

  switch (rlc->protocol_state) {

      case RLC_NULL_STATE:
        // from 3GPP TS 25.322 V4.2.0
        // In the NULL state the RLC entity does not exist and therefore it is not possible to transfer any data through it.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating establishment, the RLC entity:
        // -      is created; and
        // -      enters the DATA_TRANSFER_READY state.
        msg ("[RLC_TM %p] ERROR MAC_DATA_IND IN RLC_NULL_STATE\n", argP);
        list_free (&data_indP.data);
        break;

      case RLC_DATA_TRANSFER_READY_STATE:
        rlc->output_sdu_size_to_write = 0;      // size of sdu reassemblied
        while ((tb = list_remove_head (&data_indP.data))) {
          first_byte = ((struct mac_tb_ind *) (tb->data))->data_ptr;

          tb_size_in_bytes = (data_indP.tb_size + 7) >> 3;
          first_bit = ((struct mac_tb_ind *) (tb->data))->first_bit;
          if (first_bit > 0) {
            // shift data of transport_block TO CHECK
            bits_to_shift_last_loop = 0;
            first_byte[tb_size_in_bytes] &= (0xFF << (8 - first_bit));
            while ((tb_size_in_bytes)) {
              bits_to_shift = first_byte[tb_size_in_bytes - 1] >> (8 - first_bit);
              first_byte[tb_size_in_bytes - 1] = (first_byte[tb_size_in_bytes - 1] << first_bit) | (bits_to_shift_last_loop);
              tb_size_in_bytes -= 1;
              bits_to_shift_last_loop = bits_to_shift;
            }
          }

          ((struct rlc_tm_rx_pdu_management *) (tb->data))->first_byte = first_byte;

          if (rlc->delivery_of_erroneous_sdu == RLC_TM_DELIVERY_OF_ERRONEOUS_SDU_NO_DETECT) {
            rlc_tm_send_sdu_no_segment (rlc, 0, first_byte, data_indP.tb_size);
          } else if (rlc->delivery_of_erroneous_sdu == RLC_TM_DELIVERY_OF_ERRONEOUS_SDU_YES) {
            rlc_tm_send_sdu_no_segment (rlc, (((struct mac_tb_ind *) (tb->data))->error_indication), first_byte, data_indP.tb_size);
          } else {              //RLC_TM_DELIVERY_OF_ERRONEOUS_SDU_NO
            if (!(((struct mac_tb_ind *) (tb->data))->error_indication)) {
              rlc_tm_send_sdu_no_segment (rlc, 0, first_byte, data_indP.tb_size);
            }
          }
          free_mem_block (tb);
        }
        break;

      default:
        msg ("[rlc_tm %p] TX UNKNOWN PROTOCOL STATE %02X hex\n", rlc, rlc->protocol_state);
  }
}

//-----------------------------------------------------------------------------
void
rlc_tm_rx_segment (void *argP, struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------

  struct rlc_tm_entity *rlc = (struct rlc_tm_entity *) argP;
  mem_block_t *tb;
  s32_t             tb_size_in_bits;
  u8_t             *first_byte;
  u8_t              error_in_sdu;
  u8_t              tb_size_in_bytes;
  u8_t              first_bit;
  u8_t              byte;


  //just for debug
  u8_t             *debug;

  switch (rlc->protocol_state) {

      case RLC_NULL_STATE:
        // from 3GPP TS 25.322 V4.2.0
        // In the NULL state the RLC entity does not exist and therefore it is not possible to transfer any data through it.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating establishment, the RLC entity:
        // -      is created; and
        // -      enters the DATA_TRANSFER_READY state.
        msg ("[RLC_TM %p] ERROR MAC_DATA_IND IN RLC_NULL_STATE\n", argP);
        list_free (&data_indP.data);
        break;

      case RLC_DATA_TRANSFER_READY_STATE:
        rlc->output_sdu_size_to_write = 0;      // size of sdu reassemblied
        switch (rlc->delivery_of_erroneous_sdu) {

            case RLC_TM_DELIVERY_OF_ERRONEOUS_SDU_NO_DETECT:
              break;

            case RLC_TM_DELIVERY_OF_ERRONEOUS_SDU_YES:
              error_in_sdu = 0;
              while ((tb = list_remove_head (&data_indP.data))) {

                if ((((struct mac_tb_ind *) (tb->data))->error_indication)) {
                  error_in_sdu = 1;
                }
              }
              break;

            case RLC_TM_DELIVERY_OF_ERRONEOUS_SDU_NO:
              error_in_sdu = 0;
              if (rlc->output_sdu_in_construction == NULL) {
                rlc->output_sdu_in_construction = get_free_mem_block (RLC_SDU_MAX_SIZE_DATA_PLANE);
                rlc->output_sdu_size_to_write = 0;
                rlc->last_bit_position_reassemblied = 0;
              }
              debug = (u8_t*)&rlc->output_sdu_in_construction->data[0];

              while ((tb = list_remove_head (&data_indP.data))) {

                if ((((struct mac_tb_ind *) (tb->data))->error_indication) && (error_in_sdu)) {
                  error_in_sdu = 1;
                } else {
                  first_byte = ((struct mac_tb_ind *) (tb->data))->data_ptr;

                  tb_size_in_bytes = (data_indP.tb_size + 7) >> 3;
                  tb_size_in_bits = data_indP.tb_size;
                  first_bit = ((struct mac_tb_ind *) (tb->data))->first_bit;

                  while (tb_size_in_bits > 0) {

                    if ((first_bit == 0) && (rlc->last_bit_position_reassemblied == 0)) {
                      memcpy (&rlc->output_sdu_in_construction->data[rlc->output_sdu_size_to_write >> 3], first_byte, tb_size_in_bytes);
                      rlc->output_sdu_size_to_write += tb_size_in_bits;
                      rlc->last_bit_position_reassemblied = rlc->output_sdu_size_to_write & 0x07;
                      if ((rlc->last_bit_position_reassemblied)) {
                        rlc->output_sdu_in_construction->data[(rlc->output_sdu_size_to_write) >> 3] &= (0xFF << (8 - rlc->last_bit_position_reassemblied));
                      }
                      tb_size_in_bits = 0;
                    } else {

                      if (rlc->last_bit_position_reassemblied == 0) {
                        byte = first_byte[0];
                        rlc->output_sdu_in_construction->data[((rlc->output_sdu_size_to_write + 7) >> 3) ]
                          = (byte & (0xFF >> first_bit)) << first_bit;
                        rlc->output_sdu_size_to_write += 8 - first_bit;
                        rlc->last_bit_position_reassemblied = rlc->output_sdu_size_to_write & 0x07;
                        tb_size_in_bits -= (8 - first_bit);
                        first_bit = 0;
                        first_byte += 1;

                      } else if (first_bit == 0) {
                        byte = first_byte[0];
                        rlc->output_sdu_in_construction->data[((rlc->output_sdu_size_to_write) >> 3) ]
                          |= (byte >> rlc->last_bit_position_reassemblied);
                        if ((8 - rlc->last_bit_position_reassemblied) < tb_size_in_bits) {
                          rlc->output_sdu_size_to_write += (8 - rlc->last_bit_position_reassemblied);
                        } else {
                          rlc->output_sdu_size_to_write += tb_size_in_bits;
                        }
                        first_bit = 8 - rlc->last_bit_position_reassemblied;
                        tb_size_in_bits -= (8 - rlc->last_bit_position_reassemblied);
                        rlc->last_bit_position_reassemblied = rlc->output_sdu_size_to_write & 0x07;

                        // NEVER GO IN THAT BLOCK
                      } else if ((rlc->last_bit_position_reassemblied <= first_bit) && (rlc->last_bit_position_reassemblied > 0)) {
                        byte = first_byte[0];
                        rlc->output_sdu_in_construction->data[((rlc->output_sdu_size_to_write + 7) >> 3)]
                          |= ((byte & (0xFF >> first_bit)) << (first_bit - rlc->last_bit_position_reassemblied));
                        rlc->output_sdu_size_to_write += (8 - first_bit);
                        rlc->last_bit_position_reassemblied = rlc->output_sdu_size_to_write & 0x07;
                        tb_size_in_bits -= (8 - first_bit);
                        first_bit = 0;
                        first_byte += 1;
                        // now the case is  last_bit_position_reassemblied != 0 &&  first_bit = 0

                        // NEVER GO IN THAT BLOCK
                      } else if ((rlc->last_bit_position_reassemblied > first_bit) && (rlc->last_bit_position_reassemblied > 0)) {
                        byte = first_byte[0];
                        rlc->output_sdu_in_construction->data[((rlc->output_sdu_size_to_write + 7) >> 3)]
                          |= ((byte >> (rlc->last_bit_position_reassemblied - first_bit)) & (0xFF >> rlc->last_bit_position_reassemblied));
                        rlc->output_sdu_size_to_write += (8 - rlc->last_bit_position_reassemblied);
                        first_bit = first_bit - (8 - rlc->last_bit_position_reassemblied);
                        tb_size_in_bits -= (8 - rlc->last_bit_position_reassemblied);
                        rlc->last_bit_position_reassemblied = rlc->output_sdu_size_to_write & 0x07;
                        // now the case is  last_bit_position_reassemblied = 0 &&  first_bit != 0
                      }
                    }
                  }
                }
                free_mem_block (tb);
              }
              if (!(error_in_sdu)) {
                rlc_tm_send_sdu_segment (rlc, 0);
              }
              break;

            default:;
        }
        break;                  //RLC_DATA_TRANSFER_READY_STATE

      default:
        msg ("[rlc_tm %p] TX UNKNOWN PROTOCOL STATE %02X hex\n", rlc, rlc->protocol_state);
  }
}

//-----------------------------------------------------------------------------
struct mac_status_resp
rlc_tm_mac_status_indication (void *rlcP, u16 tb_sizeP, struct mac_status_ind tx_statusP)
{
//-----------------------------------------------------------------------------
  struct mac_status_resp status_resp;

  ((struct rlc_tm_entity *) rlcP)->nb_pdu_requested_by_mac = 1;
  ((struct rlc_tm_entity *) rlcP)->rlc_pdu_size = tb_sizeP;

  status_resp.buffer_occupancy_in_bytes = ((struct rlc_tm_entity *) rlcP)->buffer_occupancy;
  status_resp.buffer_occupancy_in_pdus = status_resp.buffer_occupancy_in_bytes / ((struct rlc_tm_entity *) rlcP)->rlc_pdu_size;
  status_resp.rlc_info.rlc_protocol_state = ((struct rlc_tm_entity *) rlcP)->protocol_state;
  return status_resp;
}

//-----------------------------------------------------------------------------
struct mac_data_req
rlc_tm_mac_data_request (void *rlcP)
{
//-----------------------------------------------------------------------------
  struct mac_data_req data_req;

  rlc_tm_get_pdus (rlcP);
  list_init (&data_req.data, NULL);
  list_add_list (&((struct rlc_tm_entity *) rlcP)->pdus_to_mac_layer, &data_req.data);

  data_req.buffer_occupancy_in_bytes = ((struct rlc_tm_entity *) rlcP)->buffer_occupancy;
  data_req.buffer_occupancy_in_pdus = data_req.buffer_occupancy_in_bytes / ((struct rlc_tm_entity *) rlcP)->rlc_pdu_size;
  data_req.rlc_info.rlc_protocol_state = ((struct rlc_tm_entity *) rlcP)->protocol_state;
  ((struct rlc_tm_entity *) rlcP)->last_tti = *((struct rlc_tm_entity *) rlcP)->frame_tick_milliseconds;
  return data_req;
}

//-----------------------------------------------------------------------------
void
rlc_tm_mac_data_indication (void *rlcP, struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------
  ((struct rlc_tm_entity *) rlcP)->rx (rlcP, data_indP);
}

//-----------------------------------------------------------------------------
void
rlc_tm_data_req (void *rlcP, mem_block_t *sduP)
{
//-----------------------------------------------------------------------------
  struct rlc_tm_entity *rlc = (struct rlc_tm_entity *) rlcP;
  u8_t              discard_go_on;


  #ifdef DEBUG_RLC_TM_DATA_REQUEST
  msg ("[RLC_TM %p] RLC_TM_DATA_REQ size %d Bytes, BO %ld , NB SDU %d current_sdu_index=%d next_sdu_index=%d\n", rlc, ((struct rlc_um_data_req *) (sduP->data))->data_size, rlc->buffer_occupancy, rlc->nb_sdu, rlc->current_sdu_index, rlc->next_sdu_index);
  #endif

  // From 3GPP TS 25.322 V4.2.0
  // If SDU discard has not been configured for a transparent mode RLC entity, the Sender shall upon reception
  // of new SDUs from upper layer:
  // -  discard all SDUs received from upper layer in previous TTIs that are not yet submitted to lower layer;
  // -  submit the new SDUs in the first possible TTI.
  if ((rlc->sdu_discard_mode & RLC_SDU_DISCARD_NOT_CONFIGURED)) {
    discard_go_on = 1;
    while ((rlc->input_sdus[rlc->current_sdu_index]) && discard_go_on) {
      if (rlc->last_tti >= ((struct rlc_tm_tx_sdu_management *) (rlc->input_sdus[rlc->current_sdu_index]->data))->sdu_creation_time) {
        #ifdef DEBUG_RLC_TM_DISCARD_SDU
        msg ("[RLC_TM %p] SDU DISCARDED NOT SUBMITTED IN THIS TTI %ld ms > ", rlc, rlc->last_tti);
        msg ("%ld ms ", ((struct rlc_tm_tx_sdu_management *) (rlc->input_sdus[rlc->current_sdu_index]->data))->sdu_creation_time);
        msg ("BO %d, NB SDU %d\n", rlc->buffer_occupancy, rlc->nb_sdu);
        #endif

        rlc->nb_sdu -= 1;
        if (!(rlc->segmentation_indication & RLC_TM_SEGMENTATION_ALLOWED)) {
          rlc->buffer_occupancy -= ((struct rlc_tm_tx_sdu_management *) (rlc->input_sdus[rlc->current_sdu_index]->data))->sdu_size;
        }
        free_mem_block (rlc->input_sdus[rlc->current_sdu_index]);
        rlc->input_sdus[rlc->current_sdu_index] = NULL;
        rlc->current_sdu_index = (rlc->current_sdu_index + 1) % rlc->size_input_sdus_buffer;
      } else {
        discard_go_on = 0;
      }
    }
  }
  // not in 3GPP specification but the buffer may be full if not correctly configured
  if (rlc->input_sdus[rlc->next_sdu_index] == NULL) {
    ((struct rlc_tm_tx_sdu_management *) (sduP->data))->sdu_size = ((struct rlc_tm_data_req *) (sduP->data))->data_size;
    if ((rlc->segmentation_indication & RLC_TM_SEGMENTATION_ALLOWED)) {
      rlc->buffer_occupancy = ((struct rlc_tm_tx_sdu_management *) (sduP->data))->sdu_size >> 3;
    } else {
      rlc->buffer_occupancy += (((struct rlc_tm_tx_sdu_management *) (sduP->data))->sdu_size >> 3);
    }
    rlc->nb_sdu += 1;
    ((struct rlc_tm_tx_sdu_management *) (sduP->data))->first_byte = (u8*)&sduP->data[sizeof (struct rlc_tm_data_req_alloc)];
    ((struct rlc_tm_tx_sdu_management *) (sduP->data))->sdu_segmented_size = 0;
    ((struct rlc_tm_tx_sdu_management *) (sduP->data))->sdu_creation_time = *rlc->frame_tick_milliseconds;
    rlc->input_sdus[rlc->next_sdu_index] = sduP;
    rlc->next_sdu_index = (rlc->next_sdu_index + 1) % rlc->size_input_sdus_buffer;
  } else {
    free_mem_block (sduP);
  }
}

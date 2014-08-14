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

/******************************************************************************
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr
*******************************************************************************/
#ifndef USER_MODE
#    define __NO_VERSION__

#    ifdef RTAI
#        include <rtai.h>
#    else
      /* RTLINUX */
#        include <rtl.h>
#    endif

#endif
//-----------------------------------------------------------------------------
#include "rlc_um_entity.h"
#include "rlc_um_constants.h"
#include "rlc_um_structs.h"
#include "rlc_primitives.h"
#include "rlc_def.h"
#include "mac_primitives.h"
#include "lists_proto_extern.h"
#include "rlc_um_reassembly_proto_extern.h"
#include "print.h"
#include "protocol_vars_extern.h"
#include "debug_l2.h"
#include "LAYER2/MAC/extern.h"
//-----------------------------------------------------------------------------
#ifdef DEBUG_RLC_UM_RX_DECODE_LI
#    define   PRINT_RLC_UM_RX_DECODE_LI msg
#else
#    define   PRINT_RLC_UM_RX_DECODE_LI //
#endif
#ifdef DEBUG_RLC_UM_RX
#    define   PRINT_RLC_UM_RX msg
#else
#    define   PRINT_RLC_UM_RX   //
#endif
//-----------------------------------------------------------------------------
inline void
rlc_um_receive_15_process_waited_pdu (struct rlc_um_entity             *rlc_pP,
                                      struct rlc_um_rx_pdu_management  *pdu_mngt_pP,
                                      struct rlc_um_rx_data_pdu_struct *data_pP,
                                      sdu_size_t                        tb_sizeP)
{
//-----------------------------------------------------------------------------
  uint8_t           *data_pdu_p              = NULL;
  sdu_size_t      remaining_data_size   = 0;
  int             nb_li                 = 0;
  int             li_index              = 0;
  sdu_size_t      li[32];

  if ((data_pP->byte1 & RLC_E_MASK) == (uint8_t) RLC_E_NEXT_FIELD_IS_DATA) {
    rlc_um_reassembly ((uint8_t *) (&data_pP->li_data_7[0]), tb_sizeP - 1, rlc_pP, 0);
  } else {
    while ((li[nb_li] = ((((uint16_t) data_pP->li_data_7[nb_li << 1]) << 8) + data_pP->li_data_7[(nb_li << 1) + 1])) & RLC_E_NEXT_FIELD_IS_LI_E) {
      li[nb_li] = li[nb_li] & (~(uint16_t) RLC_E_NEXT_FIELD_IS_LI_E);
      nb_li++;
    }
    nb_li++;                    // count the last li

    remaining_data_size = tb_sizeP - 1 - (nb_li << 1);
    data_pdu_p = (uint8_t *) (&data_pP->li_data_7[nb_li << 1]);

    while (li_index < nb_li) {
      switch (li[li_index]) {
          case RLC_LI_LAST_PDU_EXACTLY_FILLED:
            rlc_um_send_sdu (rlc_pP);
            break;
          case RLC_LI_LAST_PDU_ONE_BYTE_SHORT:
            rlc_pP->output_sdu_size_to_write -= 1;
            rlc_um_send_sdu (rlc_pP);
            break;
          case RLC_LI_PDU_PIGGY_BACKED_STATUS: // ignore
          case RLC_LI_PDU_PADDING:
            remaining_data_size = 0;
            break;
          case RLC_LI_1ST_BYTE_SDU_IS_1ST_BYTE_PDU:
            rlc_pP->output_sdu_size_to_write = 0;
            break;
          default:             // li is length
            remaining_data_size = remaining_data_size - (li[li_index] >> 1);
            rlc_um_reassembly (data_pdu_p, (li[li_index] >> 1), rlc_pP);
            data_pdu_p = (uint8_t *) ((uint64_t) data_pdu_p + (li[li_index] >> 1));
            rlc_um_send_sdu (rlc_pP);
      }
      li_index++;
    }
    if ((remaining_data_size)) {
      rlc_um_reassembly (data_pdu_p, remaining_data_size, rlc_pP);
      remaining_data_size = 0;
    }
  }
  rlc_pP->vr_us = (rlc_pP->vr_us + 1) & 127;
}

//-----------------------------------------------------------------------------
inline void
rlc_um_receive_7_process_waited_pdu (struct rlc_um_entity *rlc_pP, struct rlc_um_rx_pdu_management *pdu_mngtP, struct rlc_um_rx_data_pdu_struct *dataP, uint16_t tb_sizeP)
{
//-----------------------------------------------------------------------------
  uint8_t           *data_pdu_p            = NULL;
  sdu_size_t      remaining_data_size   = 0;
  int             nb_li                 = 0;
  int             li_index              = 0;
  sdu_size_t      li[32];

  if ((dataP->byte1 & RLC_E_MASK) == (uint8_t) RLC_E_NEXT_FIELD_IS_DATA) {
    rlc_um_reassembly ((uint8_t *) (&dataP->li_data_7[0]), tb_sizeP - 1, rlc_pP, 0);
  } else {
    while ((li[nb_li] = ((uint16_t) dataP->li_data_7[nb_li])) & RLC_E_NEXT_FIELD_IS_LI_E) {
      li[nb_li] = li[nb_li] & (~(uint8_t) RLC_E_NEXT_FIELD_IS_LI_E);
      nb_li++;
    }
    nb_li++;                    // count the last li

    remaining_data_size = tb_sizeP - 1 - nb_li;
    data_pdu_p = (uint8_t *) (&dataP->li_data_7[nb_li]);

    while (li_index < nb_li) {
      switch (li[li_index]) {
          case (uint8_t) RLC_LI_LAST_PDU_EXACTLY_FILLED:
            PRINT_RLC_UM_RX_DECODE_LI ("[RLC_UM %p] RX_7 PDU %p Li RLC_LI_LAST_PDU_EXACTLY_FILLED\n", rlc_pP, pdu_mngtP);
            rlc_um_send_sdu (rlc_pP);
            break;

          case (uint8_t) RLC_LI_LAST_PDU_ONE_BYTE_SHORT:
            PRINT_RLC_UM_RX_DECODE_LI ("[RLC_UM %p] RX_7 PDU %p Li RLC_LI_LAST_PDU_ONE_BYTE_SHORT\n", rlc_pP, pdu_mngtP);
            rlc_pP->output_sdu_size_to_write -= 1;
            rlc_um_send_sdu (rlc_pP);
            break;

          case (uint8_t) RLC_LI_PDU_PIGGY_BACKED_STATUS:    // ignore
          case (uint8_t) RLC_LI_PDU_PADDING:
            PRINT_RLC_UM_RX_DECODE_LI ("[RLC_UM %p] RX_7 PDU %p Li RLC_LI_PDU_PADDING\n", rlc_pP, pdu_mngtP);
            remaining_data_size = 0;
            break;

          case (uint8_t) RLC_LI_1ST_BYTE_SDU_IS_1ST_BYTE_PDU:
            PRINT_RLC_UM_RX_DECODE_LI ("[RLC_UM %p] RX_7 PDU %p Li RLC_LI_1ST_BYTE_SDU_IS_1ST_BYTE_PDU\n", rlc_pP, pdu_mngtP);
            rlc_pP->output_sdu_size_to_write = 0;
            break;

          default:             // li is length
            PRINT_RLC_UM_RX_DECODE_LI ("[RLC_UM %p] RX_7 PDU %p Li LI_SIZE %d Bytes\n", rlc_pP, pdu_mngtP, li[li_index] >> 1);
            remaining_data_size = remaining_data_size - (li[li_index] >> 1);
            rlc_um_reassembly (data_pdu_p, (li[li_index] >> 1), rlc_pP);
            data_pdu_p = (uint8_t *) ((uint64_t) data_pdu_p + (li[li_index] >> 1));
            rlc_um_send_sdu (rlc_pP);
      }
      li_index++;
    }
    if ((remaining_data_size)) {
      rlc_um_reassembly (data_pdu_p, remaining_data_size, rlc_pP);
      remaining_data_size = 0;
    }
  }
  rlc_pP->vr_us = (rlc_pP->vr_us + 1) & 127;
}

//-----------------------------------------------------------------------------
inline void
rlc_um_receive_15_process_unsynchronized_pdu (struct rlc_um_entity             *rlc_pP,
                                              struct rlc_um_rx_pdu_management  *pdu_mngt_pP,
                                              struct rlc_um_rx_data_pdu_struct *data_pP,
                                              sdu_size_t                       tb_sizeP)
{
//-----------------------------------------------------------------------------
  uint8_t             *data_pdu_p            = NULL;
  int               nb_li                 = 0;
  int               li_index              = 0;
  sdu_size_t        remaining_data_size   = 0;
  sdu_size_t        li[32];
  uint8_t              start_processing      = 0;

  rlc_pP->output_sdu_size_to_write = 0;

  if ((data_pP->byte1 & RLC_E_MASK) == (uint8_t) RLC_E_NEXT_FIELD_IS_DATA) {
    return;
  } else {
    while ((li[nb_li] = ((((uint16_t) data_pP->li_data_7[nb_li << 1]) << 8) + data_pP->li_data_7[(nb_li << 1) + 1])) & RLC_E_NEXT_FIELD_IS_LI_E) {
      li[nb_li] = li[nb_li] & (~(uint16_t) RLC_E_NEXT_FIELD_IS_LI_E);
      nb_li++;
    }
    nb_li++;                    // count the last li

    remaining_data_size = tb_sizeP - 1 - (nb_li << 1);
    data_pdu_p = (uint8_t *) (&data_pP->li_data_7[nb_li << 1]);

    while (li_index < nb_li) {
      switch (li[li_index]) {
          case RLC_LI_LAST_PDU_ONE_BYTE_SHORT:
          case RLC_LI_LAST_PDU_EXACTLY_FILLED:
            start_processing = 1;
            break;
          case RLC_LI_PDU_PIGGY_BACKED_STATUS: // ignore
          case RLC_LI_PDU_PADDING:
            remaining_data_size = 0;
            break;
          case RLC_LI_1ST_BYTE_SDU_IS_1ST_BYTE_PDU:
            start_processing = 1;
            rlc_pP->output_sdu_size_to_write = 0;
            break;
          default:             // li is length
            remaining_data_size = remaining_data_size - (li[li_index] >> 1);
            if ((start_processing)) {
              rlc_um_reassembly (data_pdu_p, (li[li_index] >> 1), rlc_pP, 0);
              rlc_um_send_sdu (rlc_pP);
            }
            start_processing = 1;
            data_pdu_p = (uint8_t *) ((uint64_t) data_pdu_p + (li[li_index] >> 1));
      }
      li_index++;
    }
    if ((remaining_data_size)) {
      rlc_um_reassembly (data_pdu_p, remaining_data_size, rlc_pP);
      remaining_data_size = 0;
    }
  }
  rlc_pP->vr_us = (pdu_mngt_pP->sn + 1) & 127;
}

//-----------------------------------------------------------------------------
inline void
rlc_um_receive_7_process_unsynchronized_pdu (struct rlc_um_entity            *rlc_pP,
                                            struct rlc_um_rx_pdu_management  *pdu_mngt_pP,
                                            struct rlc_um_rx_data_pdu_struct *data_pP,
                                            sdu_size_t                        tb_sizeP)
{
//-----------------------------------------------------------------------------
  uint8_t             *data_pdu_p            = NULL;
  int               nb_li                 = 0;
  int               li_index              = 0;
  sdu_size_t        remaining_data_size   = 0;
  sdu_size_t        li[32];
  uint8_t              start_processing      = 0;

  rlc_pP->output_sdu_size_to_write = 0;

  if ((data_pP->byte1 & RLC_E_MASK) == (uint8_t) RLC_E_NEXT_FIELD_IS_DATA) {
    return;
  } else {
    while ((li[nb_li] = ((uint16_t) data_pP->li_data_7[nb_li])) & RLC_E_NEXT_FIELD_IS_LI_E) {
      li[nb_li] = li[nb_li] & (~(uint8_t) RLC_E_NEXT_FIELD_IS_LI_E);
      nb_li++;
    }
    nb_li++;                    // count the last li

    remaining_data_size = tb_sizeP - 1 - nb_li;

    data_pdu_p = (uint8_t *) (&data_pP->li_data_7[nb_li]);

    while (li_index < nb_li) {
      switch (li[li_index]) {
          case (uint8_t) RLC_LI_LAST_PDU_ONE_BYTE_SHORT:
          case (uint8_t) RLC_LI_LAST_PDU_EXACTLY_FILLED:
            start_processing = 1;
            break;
          case (uint8_t) RLC_LI_PDU_PIGGY_BACKED_STATUS:    // ignore
          case (uint8_t) RLC_LI_PDU_PADDING:
            remaining_data_size = 0;
            break;
          case (uint8_t) RLC_LI_1ST_BYTE_SDU_IS_1ST_BYTE_PDU:
            start_processing = 1;
            rlc_pP->output_sdu_size_to_write = 0;
            break;
          default:             // li is length
            remaining_data_size = remaining_data_size - (li[li_index] >> 1);
            if ((start_processing)) {
              rlc_um_reassembly (data_pdu_p, (li[li_index] >> 1), rlc_pP, 0);
              rlc_um_send_sdu (rlc_pP);
            }
            start_processing = 1;
            data_pdu_p = (uint8_t *) ((uint64_t) data_pdu_p + (li[li_index] >> 1));
      }
      li_index++;
    }
    if ((remaining_data_size)) {
      rlc_um_reassembly (data_pdu_p, remaining_data_size, rlc_pP);
      remaining_data_size = 0;
    }
  }
  rlc_pP->vr_us = (pdu_mngt_pP->sn + 1) & 127;
}

//-----------------------------------------------------------------------------
void
rlc_um_receive_15 (struct rlc_um_entity *rlc_pP, struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------

  struct rlc_um_rx_data_pdu_struct *data_p                   = NULL;
  struct rlc_um_rx_pdu_management  *pdu_mngt_p               = NULL;
  struct mem_block                 *tb_p                     = NULL;
  uint8_t                             *first_byte_p             = NULL;
  sdu_size_t                        tb_size_in_bytes         = 0;
  uint8_t                              first_bit                = 0;
  uint8_t                              bits_to_shift            = 0;
  uint8_t                              bits_to_shift_last_loop  = 0;

  while ((tb_p = remove_up_head (&data_indP.data))) {

    if (!(((struct mac_tb_ind *) (tb_p->data))->error_indication)) {

      first_byte_p = ((struct mac_tb_ind *) (tb_p->data))->data_ptr;
      pdu_mngt_p   = (struct rlc_um_rx_pdu_management *) (tb_p->data);

      tb_size_in_bytes = data_indP.tb_size;
      first_bit = ((struct mac_tb_ind *) (tb_p->data))->first_bit;
      if (first_bit > 0) {
        // shift data of transport_block TO CHECK
        bits_to_shift_last_loop = 0;
        while ((tb_size_in_bytes)) {
          bits_to_shift = first_byte_p[tb_size_in_bytes] >> (8 - first_bit);
          first_byte_p[tb_size_in_bytes] = (first_byte_p[tb_size_in_bytes] << first_bit) | (bits_to_shift_last_loop);
          tb_size_in_bytes -= 1;
          bits_to_shift_last_loop = bits_to_shift;
        }
        first_byte_p[0] = (first_byte_p[0] << first_bit) | (bits_to_shift_last_loop);
      }

      pdu_mngt_p->first_byte = first_byte_p;
      data_p = (struct rlc_um_rx_data_pdu_struct *) (first_byte_p);
      pdu_mngt_p->sn = data_p->byte1 >> 1;
      PRINT_RLC_UM_RX ("[RLC_UM][RB %d] RX PDU SN %02X hex\n", rlc_pP->rb_id, pdu_mngt_p->sn);

      if (pdu_mngt_p->sn == rlc_pP->vr_us) {
        rlc_um_receive_15_process_waited_pdu (rlc_pP, pdu_mngt_p, data_p, data_indP.tb_size);
      } else {
        rlc_um_receive_15_process_unsynchronized_pdu (rlc_pP, pdu_mngt_p, data_p, data_indP.tb_size);
      }
    } else {
      PRINT_RLC_UM_RX ("[RLC_UM][RB %d] RX PDU WITH ERROR INDICATED BY LOWER LAYERS -> GARBAGE\n", rlc_pP->rb_id);
    }
    free_mem_block (tb_p);
  }
}

//-----------------------------------------------------------------------------
void
rlc_um_receive_7 (struct rlc_um_entity *rlc_pP, struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------

  struct rlc_um_rx_data_pdu_struct *data_p                   = NULL;
  struct rlc_um_rx_pdu_management  *pdu_mngt_p               = NULL;
  struct mem_block                 *tb_p                     = NULL;
  uint8_t                             *first_byte_p             = NULL;
  sdu_size_t                        tb_size_in_bytes         = 0;
  uint8_t                              first_bit                = 0;
  uint8_t                              bits_to_shift            = 0;
  uint8_t                              bits_to_shift_last_loop  = 0;

  while ((tb_p = remove_up_head (&data_indP.data))) {


    if (!(((struct mac_tb_ind *) (tb_p->data))->error_indication)) {

#ifdef DEBUG_RLC_STATS
      rlc_pP->rx_pdus += 1;
#endif
      first_byte_p = ((struct mac_tb_ind *) (tb_p->data))->data_ptr;
      pdu_mngt_p = (struct rlc_um_rx_pdu_management *) (tb_p->data);

      tb_size_in_bytes = data_indP.tb_size;
      first_bit = ((struct mac_tb_ind *) (tb_p->data))->first_bit;
      if (first_bit > 0) {
        // shift data of transport_block TO CHECK
        bits_to_shift_last_loop = 0;
        while ((tb_size_in_bytes)) {
          bits_to_shift = first_byte_p[tb_size_in_bytes] >> (8 - first_bit);
          first_byte_p[tb_size_in_bytes] = (first_byte_p[tb_size_in_bytes] << first_bit) | (bits_to_shift_last_loop);
          tb_size_in_bytes -= 1;
          bits_to_shift_last_loop = bits_to_shift;
        }
        first_byte_p[0] = (first_byte_p[0] << first_bit) | (bits_to_shift_last_loop);
      }

      pdu_mngt_p->first_byte = first_byte_p;
      data_p = (struct rlc_um_rx_data_pdu_struct *) (first_byte_p);
      pdu_mngt_p->sn = data_p->byte1 >> 1;
      PRINT_RLC_UM_RX ("[RLC_UM][RB %d] RX7 PDU SN %d VR(US) %d\n", rlc_pP->rb_id, pdu_mngt_p->sn, rlc_pP->vr_us);

      if (pdu_mngt_p->sn == rlc_pP->vr_us) {
        rlc_um_receive_7_process_waited_pdu (rlc_pP, pdu_mngt_p, data_p, data_indP.tb_size);
      } else {
        rlc_um_receive_7_process_unsynchronized_pdu (rlc_pP, pdu_mngt_p, data_p, data_indP.tb_size);
      }
    } else {
#ifdef DEBUG_RLC_STATS
      rlc_pP->rx_pdus_in_error += 1;
#endif
#ifdef DEBUG_RLC_UM_RX
      pdu_mngt_p = (struct rlc_um_rx_pdu_management *) (tb->data);
      PRINT_RLC_UM_RX ("[RLC_UM][RB %d] RX7 PDU SN %02X hex??? WITH ERROR INDICATED BY LOWER LAYERS -> GARBAGE\n", rlc_pP->rb_id, pdu_mngt_p->sn);
#endif
    }
    free_mem_block (tb_p);
  }
}

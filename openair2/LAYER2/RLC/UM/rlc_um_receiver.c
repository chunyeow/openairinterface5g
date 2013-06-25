/*
                             rlc_um_receiver.c
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
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
rlc_um_receive_15_process_waited_pdu (struct rlc_um_entity *rlcP, struct rlc_um_rx_pdu_management *pdu_mngt, struct rlc_um_rx_data_pdu_struct *dataP, u16_t tb_sizeP)
{
//-----------------------------------------------------------------------------
  u8_t             *data_pdu;
  int             nb_li = 0;
  int             li_index = 0;
  int             remaining_data_size;
  u16_t             li[32];

  if ((dataP->byte1 & RLC_E_MASK) == (u8_t) RLC_E_NEXT_FIELD_IS_DATA) {
    rlc_um_reassembly ((u8_t *) (&dataP->li_data_7[0]), tb_sizeP - 1, rlcP);
  } else {
    while ((li[nb_li] = ((((u16_t) dataP->li_data_7[nb_li << 1]) << 8) + dataP->li_data_7[(nb_li << 1) + 1])) & RLC_E_NEXT_FIELD_IS_LI_E) {
      li[nb_li] = li[nb_li] & (~(u16_t) RLC_E_NEXT_FIELD_IS_LI_E);
      nb_li++;
    }
    nb_li++;                    // count the last li

    remaining_data_size = tb_sizeP - 1 - (nb_li << 1);
    data_pdu = (u8_t *) (&dataP->li_data_7[nb_li << 1]);

    while (li_index < nb_li) {
      switch (li[li_index]) {
          case RLC_LI_LAST_PDU_EXACTLY_FILLED:
            rlc_um_send_sdu (rlcP);
            break;
          case RLC_LI_LAST_PDU_ONE_BYTE_SHORT:
            rlcP->output_sdu_size_to_write -= 1;
            rlc_um_send_sdu (rlcP);
            break;
          case RLC_LI_PDU_PIGGY_BACKED_STATUS: // ignore
          case RLC_LI_PDU_PADDING:
            remaining_data_size = 0;
            break;
          case RLC_LI_1ST_BYTE_SDU_IS_1ST_BYTE_PDU:
            rlcP->output_sdu_size_to_write = 0;
            break;
          default:             // li is length
            remaining_data_size = remaining_data_size - (li[li_index] >> 1);
            rlc_um_reassembly (data_pdu, (li[li_index] >> 1), rlcP);
            data_pdu = (u8_t *) ((u32_t) data_pdu + (li[li_index] >> 1));
            rlc_um_send_sdu (rlcP);
      }
      li_index++;
    }
    if ((remaining_data_size)) {
      rlc_um_reassembly (data_pdu, remaining_data_size, rlcP);
      remaining_data_size = 0;
    }
  }
  rlcP->vr_us = (rlcP->vr_us + 1) & 127;
}

//-----------------------------------------------------------------------------
inline void
rlc_um_receive_7_process_waited_pdu (struct rlc_um_entity *rlcP, struct rlc_um_rx_pdu_management *pdu_mngtP, struct rlc_um_rx_data_pdu_struct *dataP, u16_t tb_sizeP)
{
//-----------------------------------------------------------------------------
  u8_t             *data_pdu;
  int             remaining_data_size;
  int             nb_li = 0;
  int             li_index = 0;
  u16_t             li[32];

  if ((dataP->byte1 & RLC_E_MASK) == (u8_t) RLC_E_NEXT_FIELD_IS_DATA) {
    rlc_um_reassembly ((u8_t *) (&dataP->li_data_7[0]), tb_sizeP - 1, rlcP);
  } else {
    while ((li[nb_li] = ((u16_t) dataP->li_data_7[nb_li])) & RLC_E_NEXT_FIELD_IS_LI_E) {
      li[nb_li] = li[nb_li] & (~(u8_t) RLC_E_NEXT_FIELD_IS_LI_E);
      nb_li++;
    }
    nb_li++;                    // count the last li

    remaining_data_size = tb_sizeP - 1 - nb_li;
    data_pdu = (u8_t *) (&dataP->li_data_7[nb_li]);

    while (li_index < nb_li) {
      switch (li[li_index]) {
          case (u8_t) RLC_LI_LAST_PDU_EXACTLY_FILLED:
            PRINT_RLC_UM_RX_DECODE_LI ("[RLC_UM %p] RX_7 PDU %p Li RLC_LI_LAST_PDU_EXACTLY_FILLED\n", rlcP, pdu_mngtP);
            rlc_um_send_sdu (rlcP);
            break;

          case (u8_t) RLC_LI_LAST_PDU_ONE_BYTE_SHORT:
            PRINT_RLC_UM_RX_DECODE_LI ("[RLC_UM %p] RX_7 PDU %p Li RLC_LI_LAST_PDU_ONE_BYTE_SHORT\n", rlcP, pdu_mngtP);
            rlcP->output_sdu_size_to_write -= 1;
            rlc_um_send_sdu (rlcP);
            break;

          case (u8_t) RLC_LI_PDU_PIGGY_BACKED_STATUS:    // ignore
          case (u8_t) RLC_LI_PDU_PADDING:
            PRINT_RLC_UM_RX_DECODE_LI ("[RLC_UM %p] RX_7 PDU %p Li RLC_LI_PDU_PADDING\n", rlcP, pdu_mngtP);
            remaining_data_size = 0;
            break;

          case (u8_t) RLC_LI_1ST_BYTE_SDU_IS_1ST_BYTE_PDU:
            PRINT_RLC_UM_RX_DECODE_LI ("[RLC_UM %p] RX_7 PDU %p Li RLC_LI_1ST_BYTE_SDU_IS_1ST_BYTE_PDU\n", rlcP, pdu_mngtP);
            rlcP->output_sdu_size_to_write = 0;
            break;

          default:             // li is length
            PRINT_RLC_UM_RX_DECODE_LI ("[RLC_UM %p] RX_7 PDU %p Li LI_SIZE %d Bytes\n", rlcP, pdu_mngtP, li[li_index] >> 1);
            remaining_data_size = remaining_data_size - (li[li_index] >> 1);
            rlc_um_reassembly (data_pdu, (li[li_index] >> 1), rlcP);
            data_pdu = (u8_t *) ((u32_t) data_pdu + (li[li_index] >> 1));
            rlc_um_send_sdu (rlcP);
      }
      li_index++;
    }
    if ((remaining_data_size)) {
      rlc_um_reassembly (data_pdu, remaining_data_size, rlcP);
      remaining_data_size = 0;
    }
  }
  rlcP->vr_us = (rlcP->vr_us + 1) & 127;
}

//-----------------------------------------------------------------------------
inline void
rlc_um_receive_15_process_unsynchronized_pdu (struct rlc_um_entity *rlcP, struct rlc_um_rx_pdu_management *pdu_mngt, struct rlc_um_rx_data_pdu_struct *dataP, u16_t tb_sizeP)
{
//-----------------------------------------------------------------------------
  u8_t             *data_pdu;
  int             nb_li = 0;
  int             li_index = 0;
  int             remaining_data_size;
  u16_t             li[32];
  u8_t              start_processing = 0;

  rlcP->output_sdu_size_to_write = 0;

  if ((dataP->byte1 & RLC_E_MASK) == (u8_t) RLC_E_NEXT_FIELD_IS_DATA) {
    return;
  } else {
    while ((li[nb_li] = ((((u16_t) dataP->li_data_7[nb_li << 1]) << 8) + dataP->li_data_7[(nb_li << 1) + 1])) & RLC_E_NEXT_FIELD_IS_LI_E) {
      li[nb_li] = li[nb_li] & (~(u16_t) RLC_E_NEXT_FIELD_IS_LI_E);
      nb_li++;
    }
    nb_li++;                    // count the last li

    remaining_data_size = tb_sizeP - 1 - (nb_li << 1);
    data_pdu = (u8_t *) (&dataP->li_data_7[nb_li << 1]);

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
            rlcP->output_sdu_size_to_write = 0;
            break;
          default:             // li is length
            remaining_data_size = remaining_data_size - (li[li_index] >> 1);
            if ((start_processing)) {
              rlc_um_reassembly (data_pdu, (li[li_index] >> 1), rlcP);
              rlc_um_send_sdu (rlcP);
            }
            start_processing = 1;
            data_pdu = (u8_t *) ((u32_t) data_pdu + (li[li_index] >> 1));
      }
      li_index++;
    }
    if ((remaining_data_size)) {
      rlc_um_reassembly (data_pdu, remaining_data_size, rlcP);
      remaining_data_size = 0;
    }
  }
  rlcP->vr_us = (pdu_mngt->sn + 1) & 127;
}

//-----------------------------------------------------------------------------
inline void
rlc_um_receive_7_process_unsynchronized_pdu (struct rlc_um_entity *rlcP, struct rlc_um_rx_pdu_management *pdu_mngt, struct rlc_um_rx_data_pdu_struct *dataP, u16_t tb_sizeP)
{
//-----------------------------------------------------------------------------
  u8_t             *data_pdu;
  int             nb_li = 0;
  int             li_index = 0;
  int             remaining_data_size;
  u16_t             li[32];
  u8_t              start_processing = 0;

  rlcP->output_sdu_size_to_write = 0;

  if ((dataP->byte1 & RLC_E_MASK) == (u8_t) RLC_E_NEXT_FIELD_IS_DATA) {
    return;
  } else {
    while ((li[nb_li] = ((u16_t) dataP->li_data_7[nb_li])) & RLC_E_NEXT_FIELD_IS_LI_E) {
      li[nb_li] = li[nb_li] & (~(u8_t) RLC_E_NEXT_FIELD_IS_LI_E);
      nb_li++;
    }
    nb_li++;                    // count the last li

    remaining_data_size = tb_sizeP - 1 - nb_li;

    data_pdu = (u8_t *) (&dataP->li_data_7[nb_li]);

    while (li_index < nb_li) {
      switch (li[li_index]) {
          case (u8_t) RLC_LI_LAST_PDU_ONE_BYTE_SHORT:
          case (u8_t) RLC_LI_LAST_PDU_EXACTLY_FILLED:
            start_processing = 1;
            break;
          case (u8_t) RLC_LI_PDU_PIGGY_BACKED_STATUS:    // ignore
          case (u8_t) RLC_LI_PDU_PADDING:
            remaining_data_size = 0;
            break;
          case (u8_t) RLC_LI_1ST_BYTE_SDU_IS_1ST_BYTE_PDU:
            start_processing = 1;
            rlcP->output_sdu_size_to_write = 0;
            break;
          default:             // li is length
            remaining_data_size = remaining_data_size - (li[li_index] >> 1);
            if ((start_processing)) {
              rlc_um_reassembly (data_pdu, (li[li_index] >> 1), rlcP);
              rlc_um_send_sdu (rlcP);
            }
            start_processing = 1;
            data_pdu = (u8_t *) ((u32_t) data_pdu + (li[li_index] >> 1));
      }
      li_index++;
    }
    if ((remaining_data_size)) {
      rlc_um_reassembly (data_pdu, remaining_data_size, rlcP);
      remaining_data_size = 0;
    }
  }
  rlcP->vr_us = (pdu_mngt->sn + 1) & 127;
}

//-----------------------------------------------------------------------------
void
rlc_um_receive_15 (struct rlc_um_entity *rlcP, struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------

  struct rlc_um_rx_data_pdu_struct *data;
  struct rlc_um_rx_pdu_management *pdu_mngt;
  struct mem_block *tb;
  u8_t             *first_byte;
  u8_t              tb_size_in_bytes;
  u8_t              first_bit;
  u8_t              bits_to_shift;
  u8_t              bits_to_shift_last_loop;

  while ((tb = remove_up_head (&data_indP.data))) {

    if (!(((struct mac_tb_ind *) (tb->data))->error_indication)) {

      first_byte = ((struct mac_tb_ind *) (tb->data))->data_ptr;
      pdu_mngt = (struct rlc_um_rx_pdu_management *) (tb->data);

      tb_size_in_bytes = data_indP.tb_size;
      first_bit = ((struct mac_tb_ind *) (tb->data))->first_bit;
      if (first_bit > 0) {
        // shift data of transport_block TO CHECK
        bits_to_shift_last_loop = 0;
        while ((tb_size_in_bytes)) {
          bits_to_shift = first_byte[tb_size_in_bytes] >> (8 - first_bit);
          first_byte[tb_size_in_bytes] = (first_byte[tb_size_in_bytes] << first_bit) | (bits_to_shift_last_loop);
          tb_size_in_bytes -= 1;
          bits_to_shift_last_loop = bits_to_shift;
        }
        first_byte[0] = (first_byte[0] << first_bit) | (bits_to_shift_last_loop);
      }

      pdu_mngt->first_byte = first_byte;
      data = (struct rlc_um_rx_data_pdu_struct *) (first_byte);
      pdu_mngt->sn = data->byte1 >> 1;
#ifdef BENCH_QOS_L2
      fprintf (bench_l2, "[PDU RX] FRAME %d SN %d RLC-UM %p\n", mac_xface->frame, pdu_mngt->sn, rlcP);
#endif
      PRINT_RLC_UM_RX ("[RLC_UM][RB %d] RX PDU SN %02X hex\n", rlcP->rb_id, pdu_mngt->sn);

      if (pdu_mngt->sn == rlcP->vr_us) {
        rlc_um_receive_15_process_waited_pdu (rlcP, pdu_mngt, data, data_indP.tb_size);
      } else {
        rlc_um_receive_15_process_unsynchronized_pdu (rlcP, pdu_mngt, data, data_indP.tb_size);
      }
    } else {
      PRINT_RLC_UM_RX ("[RLC_UM][RB %d] RX PDU WITH ERROR INDICATED BY LOWER LAYERS -> GARBAGE\n", rlcP->rb_id);
    }
    free_mem_block (tb);
  }
}

//-----------------------------------------------------------------------------
void
rlc_um_receive_7 (struct rlc_um_entity *rlcP, struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------

  struct rlc_um_rx_data_pdu_struct *data;
  struct rlc_um_rx_pdu_management *pdu_mngt;
  struct mem_block *tb;
  u8_t             *first_byte;
  u8_t              tb_size_in_bytes;
  u8_t              first_bit;
  u8_t              bits_to_shift;
  u8_t              bits_to_shift_last_loop;

  while ((tb = remove_up_head (&data_indP.data))) {


    if (!(((struct mac_tb_ind *) (tb->data))->error_indication)) {

#ifdef DEBUG_RLC_STATS
      rlcP->rx_pdus += 1;
#endif
      first_byte = ((struct mac_tb_ind *) (tb->data))->data_ptr;
      pdu_mngt = (struct rlc_um_rx_pdu_management *) (tb->data);

      tb_size_in_bytes = data_indP.tb_size;
      first_bit = ((struct mac_tb_ind *) (tb->data))->first_bit;
      if (first_bit > 0) {
        // shift data of transport_block TO CHECK
        bits_to_shift_last_loop = 0;
        while ((tb_size_in_bytes)) {
          bits_to_shift = first_byte[tb_size_in_bytes] >> (8 - first_bit);
          first_byte[tb_size_in_bytes] = (first_byte[tb_size_in_bytes] << first_bit) | (bits_to_shift_last_loop);
          tb_size_in_bytes -= 1;
          bits_to_shift_last_loop = bits_to_shift;
        }
        first_byte[0] = (first_byte[0] << first_bit) | (bits_to_shift_last_loop);
      }

      pdu_mngt->first_byte = first_byte;
      data = (struct rlc_um_rx_data_pdu_struct *) (first_byte);
      pdu_mngt->sn = data->byte1 >> 1;
      PRINT_RLC_UM_RX ("[RLC_UM][RB %d] RX7 PDU SN %d VR(US) %d\n", rlcP->rb_id, pdu_mngt->sn, rlcP->vr_us);
#ifdef BENCH_QOS_L2
      fprintf (bench_l2, "[PDU RX] FRAME %d SN %d RLC-UM %p\n", mac_xface->frame, pdu_mngt->sn, rlcP);
#endif

      if (pdu_mngt->sn == rlcP->vr_us) {
        rlc_um_receive_7_process_waited_pdu (rlcP, pdu_mngt, data, data_indP.tb_size);
      } else {
        rlc_um_receive_7_process_unsynchronized_pdu (rlcP, pdu_mngt, data, data_indP.tb_size);
      }
    } else {
#ifdef DEBUG_RLC_STATS
      rlcP->rx_pdus_in_error += 1;
#endif
#ifdef BENCH_QOS_L2
      fprintf (bench_l2, "[PDU RX ERROR] FRAME %d RLC-UM %p\n", mac_xface->frame, rlcP);
#endif
#ifdef DEBUG_RLC_UM_RX
      pdu_mngt = (struct rlc_um_rx_pdu_management *) (tb->data);
      PRINT_RLC_UM_RX ("[RLC_UM][RB %d] RX7 PDU SN %02X hex??? WITH ERROR INDICATED BY LOWER LAYERS -> GARBAGE\n", rlcP->rb_id, pdu_mngt->sn);
#endif
    }
    free_mem_block (tb);
  }
}

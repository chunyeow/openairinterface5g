/*
                             rlc_um_receiver.c
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#define RLC_UM_C
#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "rlc.h"
#include "rlc_um_entity.h"
#include "rlc_um_constants.h"
#include "rlc_um_structs.h"
#include "rlc_primitives.h"
#include "rlc_def.h"
#include "mac_primitives.h"
#include "list.h"
#include "rlc_um_reassembly_proto_extern.h"
#include "rlc_um_segment_proto_extern.h"
//#define DEBUG_RLC_UM_RX 1
//#define DEBUG_RLC_UM_RX_DECODE_LI 1
//#define DEBUG_RLC_UM_DISPLAY_TB_DATA 1
//#define TEST_EXIT_HICHAM 1
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void
rlc_um_receive_process_pdu (struct rlc_um_entity *rlcP, struct rlc_um_rx_pdu_management *pdu_mngtP, struct
                            rlc_um_rx_data_pdu_struct *dataP, u16_t tb_sizeP, u16 bad_crc_l1P)
{
//-----------------------------------------------------------------------------
  u8_t             *data_pdu;
  s32_t             remaining_data_size;
  u32_t             rlc_um_crc;
  int             nb_li;
  int             li_index;
  u16_t             li[RLC_UM_SEGMENT_NB_MAX_LI_PER_PDU];
  u16_t             li_synched;
  u16_t             rlc_crc_to_check;

  pdu_mngtP->sn = dataP->byte1 >> 1;

  
  #ifdef DEBUG_RLC_UM_RX
  msg ("[RLC_UM_LITE][MOD ID %d][RB %d] RX PDU SN %d VR(US) %d TBsize %d\n", rlcP->module_id, rlcP->rb_id, pdu_mngtP->sn, rlcP->vr_us,tb_sizeP);
  #endif
  
  if (!(bad_crc_l1P)) {
    rlc_crc_to_check = 0;
    if (pdu_mngtP->sn == rlcP->vr_us) {
      li_synched = 1;
    } else {
      // if the sn is not synched, then garbage the previous SDU in construction if any
      //msg ("[RLC_UM_LITE][RB %d] RX7 OUT OF SYNC -> GARBAGE\n", rlcP->rb_id);
      rlc_um_clear_rx_sdu (rlcP);
      li_synched = 0;

      //#ifdef TEST_EXIT_HICHAM
      //#warning TEST_EXIT_HICHAM
      //#ifdef USER_MODE
      //      mac_xface->macphy_exit ("[RLC][UM_RECEIVER]SYNCHRO LOST");
      //#endif
      //#endif
    }
  } else if ((rlcP->crc_on_header)) {
    rlc_crc_to_check = 1;
    if (pdu_mngtP->sn == rlcP->vr_us) {
      li_synched = 1;
    } else {
      // if the sn is not synched, then garbage the previous SDU in construction if any
    //msg ("[RLC_UM_LITE][RB %d] RX7 OUT OF SYNC -> GARBAGE\n", rlcP->rb_id);
      rlc_um_clear_rx_sdu (rlcP);
      li_synched = 0;
    }
  } else {
#ifdef DEBUG_RLC_UM_RX
    msg ("[RLC_UM_LITE][RB %d] RX7 PDU SN %02X hex??? WITH ERROR INDICATED BY LOWER LAYERS -> GARBAGE\n", rlcP->rb_id, pdu_mngtP->sn);
#endif
    return;
  }

  //---------------------------------
  // NO LENGTH INDICATOR IN PDU
  //---------------------------------
  if ((dataP->byte1 & RLC_E_MASK) == (u8_t) RLC_E_NEXT_FIELD_IS_DATA) {
    if ((li_synched)) {
      if ((rlcP->crc_on_header)) {
        if ((rlc_crc_to_check)) {
          // may be the sn was not good
          rlc_um_crc = rlc_um_crc8_xor (&dataP->byte1, 1); // >> 24;
          if (rlc_um_crc != dataP->li_data_7[tb_sizeP - 2]) {
            msg ("[RLC_UM_LITE][RB %d] BAD CRC ON RLC HEADER 1 byte crc computed %d, crc %d, byte %d \n", rlcP->rb_id, rlc_um_crc, dataP->li_data_7[tb_sizeP - 2], dataP->byte1);
            return;
          } else {
            msg ("[RLC_UM_LITE][RB %d] GOOD CRC ON RLC HEADER 1 byte\n", rlcP->rb_id);
          }
        }
        rlc_um_reassembly ((u8_t *) (&dataP->li_data_7[0]), tb_sizeP - 2, rlcP);
      } else {
        rlc_um_reassembly ((u8_t *) (&dataP->li_data_7[0]), tb_sizeP - 1, rlcP);
      }
    }
    //---------------------------------
    // 1 OR MORE LENGTH INDICATOR IN PDU
    //---------------------------------
  } else {
    nb_li = 0;
    if (tb_sizeP <= 125) {
      while (((li[nb_li] = ((u16_t) dataP->li_data_7[nb_li])) & RLC_E_NEXT_FIELD_IS_LI_E)
             && (nb_li < RLC_UM_SEGMENT_NB_MAX_LI_PER_PDU)) {
        li[nb_li] = li[nb_li] & (~(u8_t) RLC_E_NEXT_FIELD_IS_LI_E);
        nb_li++;
      }
      nb_li++;                  // count the last li
    } else {
      while ((li[nb_li] = ((((u16_t) dataP->li_data_7[nb_li << 1]) << 8) + dataP->li_data_7[(nb_li << 1) + 1]))
             & RLC_E_NEXT_FIELD_IS_LI_E) {
        li[nb_li] = li[nb_li] & (~(u16_t) RLC_E_NEXT_FIELD_IS_LI_E);
        nb_li++;
      }
      nb_li++;                  // count the last li
    }



    //---------------------------------
    // CHECK CRC ON RLC UM HEADER
    //---------------------------------
    if ((rlc_crc_to_check)) {
      if (nb_li > RLC_UM_SEGMENT_NB_MAX_LI_PER_PDU) {
        return;                 // must be corrupted not necessary to check CRC
      } else {
        if (tb_sizeP <= 125) {
          rlc_um_crc = rlc_um_crc8_xor (&dataP->byte1, nb_li + 1) ; //>> 24;
        } else {
          rlc_um_crc = rlc_um_crc8_xor (&dataP->byte1, (nb_li << 1) + 1); // >> 24;
        }
        if (rlc_um_crc != dataP->li_data_7[tb_sizeP - 2]) {
          msg ("[RLC_UM_LITE][RB %d] BAD CRC ON RLC HEADER %d LI TB size %d\n", rlcP->rb_id, nb_li, tb_sizeP);
          return;
        } else {
          msg ("[RLC_UM_LITE][RB %d] GOOD CRC ON RLC HEADER %d LI TB size %d\n", rlcP->rb_id, nb_li, tb_sizeP);
          if (pdu_mngtP->sn == rlcP->vr_us) {
            li_synched = 1;
          } else {
            // if the sn is not synched, then garbage the previous SDU in construction if any
            rlc_um_clear_rx_sdu (rlcP);
            li_synched = 0;
          }
        }
      }
    }

    if ((rlcP->crc_on_header)) {
      if (tb_sizeP <= 125) {
        remaining_data_size = tb_sizeP - 2 - nb_li;
        data_pdu = (u8_t *) (&dataP->li_data_7[nb_li]);
      } else {
        remaining_data_size = tb_sizeP - 2 - (nb_li << 1);
        data_pdu = (u8_t *) (&dataP->li_data_7[nb_li << 1]);
      }
    } else {
      if (tb_sizeP <= 125) {
        remaining_data_size = tb_sizeP - 1 - nb_li;
        data_pdu = (u8_t *) (&dataP->li_data_7[nb_li]);
      } else {
        remaining_data_size = tb_sizeP - 1 - (nb_li << 1);
        data_pdu = (u8_t *) (&dataP->li_data_7[nb_li << 1]);
      }
    }

    li_index = 0;
    while (li_index < nb_li) {
      switch (li[li_index]) {
          case (u8_t) RLC_LI_LAST_PDU_EXACTLY_FILLED:
#ifdef DEBUG_RLC_UM_RX_DECODE_LI
            msg ("[RLC_UM_LITE][RB %d] RX_7 PDU %p Li RLC_LI_LAST_PDU_EXACTLY_FILLED\n", rlcP->rb_id, pdu_mngtP);
#endif
            if ((li_synched)) {
              rlc_um_send_sdu (rlcP);
            }
            li_synched = 1;
            break;

          case (u8_t) RLC_LI_LAST_PDU_ONE_BYTE_SHORT:
#ifdef DEBUG_RLC_UM_RX_DECODE_LI
            msg ("[RLC_UM_LITE][RB %d] RX_7 PDU %p Li RLC_LI_LAST_PDU_ONE_BYTE_SHORT\n", rlcP->rb_id, pdu_mngtP);
#endif
            if ((li_synched)) {
              rlc_um_send_sdu_minus_1_byte (rlcP);
            }
            li_synched = 1;
            break;

          case (u8_t) RLC_LI_PDU_PIGGY_BACKED_STATUS:    // ignore for RLC-AM
          case (u8_t) RLC_LI_PDU_PADDING:
#ifdef DEBUG_RLC_UM_RX_DECODE_LI
            msg ("[RLC_UM_LITE][RB %d] RX_7 PDU %p Li RLC_LI_PDU_PADDING\n", rlcP->rb_id, pdu_mngtP);
#endif
            remaining_data_size = 0;
            break;

          case (u8_t) RLC_LI_1ST_BYTE_SDU_IS_1ST_BYTE_PDU:
#ifdef DEBUG_RLC_UM_RX_DECODE_LI
            msg ("[RLC_UM_LITE][RB %d] RX_7 PDU %p Li RLC_LI_1ST_BYTE_SDU_IS_1ST_BYTE_PDU\n", rlcP->rb_id, pdu_mngtP);
#endif
            rlc_um_clear_rx_sdu (rlcP);
            li_synched = 1;
            break;

          default:             // li is length
#ifdef DEBUG_RLC_UM_RX_DECODE_LI
            msg ("[RLC_UM_LITE][RB %d] RX_7 PDU %p Li LI_SIZE %d Bytes\n", rlcP->rb_id, pdu_mngtP, li[li_index] >> 1);
#endif
            remaining_data_size = remaining_data_size - (li[li_index] >> 1);
            if ((li_synched)) {
              rlc_um_reassembly (data_pdu, (li[li_index] >> 1), rlcP);
              rlc_um_send_sdu (rlcP);
            }
            li_synched = 1;
            data_pdu = (u8_t *) ((u32_t) data_pdu + (li[li_index] >> 1));
      }
      li_index++;
    }
    if ((remaining_data_size > 0)) {
      rlc_um_reassembly (data_pdu, remaining_data_size, rlcP);
      remaining_data_size = 0;
    }
  }
  rlcP->vr_us = (pdu_mngtP->sn + 1) & 127;
}

//-----------------------------------------------------------------------------
void
rlc_um_receive (struct rlc_um_entity *rlcP, struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------

  struct rlc_um_rx_data_pdu_struct *data;
  struct rlc_um_rx_pdu_management *pdu_mngt;
  mem_block_t *tb;
  u8_t             *first_byte;
  u8_t              tb_size_in_bytes;
  u8_t              first_bit;
  u8_t              bits_to_shift;
  u8_t              bits_to_shift_last_loop;


  while ((tb = list_remove_head (&data_indP.data))) {
#ifdef DEBUG_RLC_STATS
    rlcP->rx_pdus += 1;
#endif


#ifdef DEBUG_RLC_UM_DISPLAY_TB_DATA
    //    if (rlcP->rb_id == 4) {
    //    if ((((struct mac_tb_ind *) (tb->data))->error_indication) && (rlcP->crc_on_header)) {
      msg ("[RLC_UM_LITE][RB %d] DUMP RX PDU:", rlcP->rb_id);
      for (tb_size_in_bytes = 0; tb_size_in_bytes < data_indP.tb_size; tb_size_in_bytes++) {
        msg ("%02X.", ((struct mac_tb_ind *) (tb->data))->data_ptr[tb_size_in_bytes]);
      }
      msg("\n");
      //    }
    //       }
#endif
    if (!(((struct mac_tb_ind *) (tb->data))->error_indication) || (rlcP->crc_on_header)) {
//msg("YES\n");
      //----------------------------------
      // align on byte boundary if not
      //----------------------------------
      first_byte = ((struct mac_tb_ind *) (tb->data))->data_ptr;
      pdu_mngt = (struct rlc_um_rx_pdu_management *) (tb->data);

      tb_size_in_bytes = data_indP.tb_size;
      //      msg("[RLC_RECEIVE] RX_size %d\n",data_indP.tb_size);
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


      rlc_um_receive_process_pdu (rlcP, pdu_mngt, data, data_indP.tb_size, ((struct mac_tb_ind *) (tb->data))->error_indication);
    } else {
//msg("NO\n");
#ifdef DEBUG_RLC_STATS
      rlcP->rx_pdus_in_error += 1;
#endif
#ifdef DEBUG_RLC_UM_RX
      msg ("[RLC_UM_LITE][RB %d] RX PDU  WITH ERROR INDICATED BY LOWER LAYERS -> GARBAGE\n", rlcP->rb_id);
#endif
    }
    free_mem_block (tb);
  }                             // end while
}

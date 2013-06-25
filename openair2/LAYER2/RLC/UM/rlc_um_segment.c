/*
                              rlc_um_segment.c
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

#include "print.h"
#include "lists_proto_extern.h"
#include "rlc_um_entity.h"
#include "rlc_um_structs.h"
#include "rlc_primitives.h"
#include "rlc_def.h"
#include "rlc_um_constants.h"
#include "debug_l2.h"

#ifdef DEBUG_RLC_UM_SEGMENT
#    define   PRINT_RLC_UM_SEGMENT msg
#else
#    define   PRINT_RLC_UM_SEGMENT
                                //
#endif
#ifdef DEBUG_RLC_UM_SEGMENT_ENCODE
#    define   PRINT_RLC_UM_SEGMENT_ENCODE msg
#else
#    define   PRINT_RLC_UM_SEGMENT_ENCODE
                                        //
#endif
#ifdef DEBUG_RLC_UM_SEGMENT_FILL_DATA
#    define   PRINT_RLC_UM_SEGMENT_FILL_DATA msg
#else
#    define   PRINT_RLC_UM_SEGMENT_FILL_DATA    //
#endif
#ifdef DEBUG_RLC_UM_DISCARD_SDU
#    define   PRINT_RLC_UM_DISCARD_SDU msg
#else
#    define   PRINT_RLC_UM_DISCARD_SDU  //
#endif
//-----------------------------------------------------------------------------
mem_block      *rlc_um_build_pdu_with_only_2li (struct rlc_um_entity *rlcP, u16_t li0P, u16 li1P);
inline void     rlc_um_encode_pdu_15 (struct rlc_um_entity *rlcP, struct rlc_um_tx_data_pdu_struct *rlc_headerP, struct rlc_um_tx_pdu_management *pdu_mngtP, u16_t * li_arrayP, u8_t nb_liP);
inline void     rlc_um_encode_pdu_7 (struct rlc_um_entity *rlcP, struct rlc_um_tx_data_pdu_struct *rlc_headerP, struct rlc_um_tx_pdu_management *pdu_mngtP, u16_t * li_arrayP, u8_t nb_liP);
inline void     rlc_um_fill_pdus (struct rlc_um_entity *rlcP, list_up * pdusP, list_up * segmented_sdusP);
void            rlc_um_segment_15 (struct rlc_um_entity *rlcP);
//-----------------------------------------------------------------------------
mem_block      *
rlc_um_build_pdu_with_only_2li (struct rlc_um_entity *rlcP, u16_t li0P, u16 li1P)
{
//-----------------------------------------------------------------------------
  struct rlc_um_tx_data_pdu_struct *rlc_header;
  struct rlc_um_tx_pdu_management *pdu_mngt;
  mem_block      *pdu;

  if (!(pdu = get_free_mem_block (rlcP->data_pdu_size + sizeof (struct rlc_um_tx_data_pdu_management) + GUARD_CRC_LIH_SIZE))) {
    msg ("[RLC_UM][RB %d] BUILD PDU 2 LI ERROR COULD NOT GET NEW PDU, EXIT...BEFORE CRASH\n", rlcP->rb_id);
    return NULL;
  }
  PRINT_RLC_UM_SEGMENT ("[RLC_UM][RB %d] SEGMENT PDU SN %d SET LIs %04X %04X\n", rlcP->rb_id, rlcP->vt_us, li0P, li1P);
  rlc_header = (struct rlc_um_tx_data_pdu_struct *) (&pdu->data[sizeof (struct rlc_um_tx_data_pdu_management)]);
  pdu_mngt = (struct rlc_um_tx_pdu_management *) (pdu->data);
  memset (pdu->data, 0, sizeof (struct rlc_um_tx_pdu_management));

  if (rlcP->data_pdu_size > 125) {
    rlc_header->li_data_7[0] = (u8_t) (li0P >> 8);
    rlc_header->li_data_7[1] = (u8_t) (li0P | RLC_E_NEXT_FIELD_IS_LI_E);
    rlc_header->li_data_7[2] = (u8_t) (li1P >> 8);
    rlc_header->li_data_7[3] = (u8_t) li1P;
    rlcP->li_length_15_was_used_for_previous_pdu = 1;
  } else {
    rlc_header->li_data_7[0] = (u8_t) (li0P | RLC_E_NEXT_FIELD_IS_LI_E);
    rlc_header->li_data_7[1] = (u8_t) li1P;
    rlcP->li_length_15_was_used_for_previous_pdu = 0;
  }
  rlc_header->byte1 = (rlcP->vt_us << 1) | RLC_E_NEXT_FIELD_IS_LI_E;
  // inc vt_us
#ifdef DEBUG_RLC_UM_VT_US
  msg ("[RLC_UM][RB %d] Inc VT(US) in rlc_um_build_pdu_with_only_2li()\n", rlcP->rb_id);
#endif
  rlcP->vt_us = (rlcP->vt_us + 1) & 0x7F;

  pdu_mngt->first_byte = (u8_t *) rlc_header;
  pdu_mngt->data_size = 0;
  pdu_mngt->payload = NULL;

  return pdu;
}

//-----------------------------------------------------------------------------
inline void
rlc_um_encode_pdu_15 (struct rlc_um_entity *rlcP, struct rlc_um_tx_data_pdu_struct *rlc_headerP, struct rlc_um_tx_pdu_management *pdu_mngtP, u16_t * li_arrayP, u8_t nb_liP)
{
//-----------------------------------------------------------------------------
  u8_t              li_index;

#ifdef DEBUG_RLC_UM_SEGMENT
  int             index;
  msg ("[RLC_UM %p] SEGMENT_15 PDU SN %d SET LIs: ", rlcP, rlcP->vt_us);
  for (index = 0; index < nb_liP; index++) {
    msg ("%04X ", li_arrayP[index]);
  }
  msg ("\n");
#endif
  rlc_headerP->byte1 = (rlcP->vt_us << 1);
  // inc vt_us
#ifdef DEBUG_RLC_UM_VT_US
  msg ("[RLC_UM %p] Inc VT(US) in rlc_um_encode_pdu_15\n", rlcP);
#endif
  rlcP->vt_us = (rlcP->vt_us + 1) & 127;

  nb_liP = nb_liP << 1;
  pdu_mngtP->payload = (u8_t *) ((u32_t) (&rlc_headerP->li_data_7[nb_liP]));

  if (nb_liP) {
    rlc_headerP->byte1 |= RLC_E_NEXT_FIELD_IS_LI_E;
    li_index = 0;
    // COPY LI
    while (li_index < nb_liP) {
      rlc_headerP->li_data_7[li_index] = li_arrayP[li_index >> 1] >> 8;
      li_index += 1;
      rlc_headerP->li_data_7[li_index] = li_arrayP[li_index >> 1] | RLC_E_NEXT_FIELD_IS_LI_E;
      li_index += 1;
    }
    rlc_headerP->li_data_7[li_index - 1] = rlc_headerP->li_data_7[li_index - 1] ^ (u8_t) RLC_E_NEXT_FIELD_IS_LI_E;
  }
}

//-----------------------------------------------------------------------------
inline void
rlc_um_encode_pdu_7 (struct rlc_um_entity *rlcP, struct rlc_um_tx_data_pdu_struct *rlc_headerP, struct rlc_um_tx_pdu_management *pdu_mngtP, u16_t * li_arrayP, u8_t nb_liP)
{
//-----------------------------------------------------------------------------
  u8_t              li_index;

#ifdef DEBUG_RLC_UM_SEGMENT_ENCODE
  int             index;
  msg ("[RLC_UM %p] SEGMENT_7 PDU %p SN %d SET LIs: ", rlcP, pdu_mngtP, rlcP->vt_us);
  for (index = 0; index < nb_liP; index++) {
    msg ("%04X ", li_arrayP[index]);
  }
  msg ("\n");
#endif

  rlc_headerP->byte1 = (rlcP->vt_us << 1);
  // inc vt_us
#ifdef DEBUG_RLC_UM_VT_US
  msg ("[RLC_UM %p] Inc VT(US) in rlc_um_encode_pdu_7()\n", rlcP);
#endif
  rlcP->vt_us = (rlcP->vt_us + 1) & 127;

  pdu_mngtP->payload = (u8_t *) ((u32_t) (&rlc_headerP->li_data_7[nb_liP]));

  if (nb_liP) {
    rlc_headerP->byte1 |= RLC_E_NEXT_FIELD_IS_LI_E;
    li_index = 0;
    // COPY LI
    while (li_index < nb_liP) {
      rlc_headerP->li_data_7[li_index] = (u8_t) (li_arrayP[li_index]) | RLC_E_NEXT_FIELD_IS_LI_E;
      li_index += 1;
    }
    rlc_headerP->li_data_7[li_index - 1] = rlc_headerP->li_data_7[li_index - 1] ^ (u8_t) RLC_E_NEXT_FIELD_IS_LI_E;
  }
}

//-----------------------------------------------------------------------------
inline void
rlc_um_fill_pdus (struct rlc_um_entity *rlcP, list_up * pdusP, list_up * segmented_sdusP)
{
//-----------------------------------------------------------------------------
  mem_block      *pdu;
  mem_block      *sdu;
  u8_t             *data_sdu = NULL;
  u8_t             *data_pdu;
  struct rlc_um_tx_data_pdu_struct *rlc_header;
  struct rlc_um_tx_pdu_management *pdu_mngt;
  struct rlc_um_tx_sdu_management *sdu_mngt;

  u16_t             pdu_remaining_size;
  u16_t             sdu_available_size;

  pdu = NULL;
  sdu = NULL;

  // dispatch sdus
  while ((pdu = remove_up_head (pdusP))) {

    pdu_mngt = (struct rlc_um_tx_pdu_management *) (pdu->data);
    rlc_header = (struct rlc_um_tx_data_pdu_struct *) (pdu_mngt->first_byte);

    pdu_remaining_size = pdu_mngt->data_size;
    data_pdu = (u8_t *) pdu_mngt->payload;
#ifdef DEBUG_RLC_UM_SEGMENT_FILL_DATA
    msg ("[RLC_UM %p] SEGMENT FILL GET PDU %p REMAINING SIZE=%d\n", rlcP, pdu, pdu_remaining_size);
#endif

    if (pdu_remaining_size > 0) {
      // fill the pdu with data of sdu
      while (pdu) {
        if (sdu == NULL) {
          sdu = remove_up_head (segmented_sdusP);
          sdu_mngt = (struct rlc_um_tx_sdu_management *) (sdu->data);
          data_sdu = &sdu->data[sizeof (struct rlc_um_tx_sdu_management) + sdu_mngt->sdu_segmented_size];
          sdu_available_size = sdu_mngt->sdu_size - sdu_mngt->sdu_remaining_size - sdu_mngt->sdu_segmented_size;
#ifdef DEBUG_RLC_UM_SEGMENT_FILL_DATA
          msg ("[RLC_UM %p] SEGMENT FILL GET SDU %p AVAILABLE_SZ=%d (SZ %d REMAIN_SZ %d SEGMENT_SZ %d)\n", rlcP, sdu, sdu_available_size, sdu_mngt->sdu_size,
               sdu_mngt->sdu_remaining_size, sdu_mngt->sdu_segmented_size);
#endif
        }
        // copy the whole remaining data of the sdu in the remaining area of the pdu
        if (pdu_remaining_size >= sdu_available_size) {
          memcpy (data_pdu, data_sdu, sdu_available_size);
#ifdef DEBUG_RLC_UM_SEGMENT_FILL_DATA
          msg ("[RLC_UM %p] SEGMENT FILL PDU %p WITH SDU %p  %p <- %p %d bytes (SZ %d REMAIN_SZ %d SEGMENT_SZ %d)\n", rlcP, pdu, sdu, data_pdu, data_sdu, sdu_available_size,
               sdu_mngt->sdu_size, sdu_mngt->sdu_remaining_size, sdu_mngt->sdu_segmented_size);
#endif
          pdu_remaining_size -= sdu_available_size;
          sdu_mngt->sdu_segmented_size += sdu_available_size;
          data_pdu = (u8_t *) ((u32_t) data_pdu + sdu_available_size);
          sdu_available_size = 0;

          // dispatch the sdu
          free_mem_block (sdu);
          sdu = NULL;

          // dispatch the pdu
          if (pdu_remaining_size == 0) {
            ((struct mac_tb_req *) (pdu->data))->rlc = NULL;
            ((struct mac_tb_req *) (pdu->data))->data_ptr = &rlc_header->byte1;
            ((struct mac_tb_req *) (pdu->data))->first_bit = 0;
            ((struct mac_tb_req *) (pdu->data))->tb_size_in_bits = rlcP->data_pdu_size_in_bits;
            add_cnt_up_tail (pdu, &rlcP->pdus_to_mac_layer);
            pdu = NULL;
          } else {
          }
          // copy some data of the sdu in the whole remaining area of the pdu
        } else {
          memcpy (data_pdu, data_sdu, pdu_remaining_size);
#ifdef DEBUG_RLC_UM_SEGMENT_FILL_DATA
          msg ("[RLC_UM %p] SEGMENT FILL PDU %p WITH SDU %p  %p <- %p %d bytes (SZ %d REMAIN_SZ %d SEGMENT_SZ %d)\n", rlcP, pdu, sdu, data_pdu, data_sdu, pdu_remaining_size,
               sdu_mngt->sdu_size, sdu_mngt->sdu_remaining_size, sdu_mngt->sdu_segmented_size);
#endif
          sdu_mngt->sdu_segmented_size += pdu_remaining_size;
          sdu_available_size -= pdu_remaining_size;
          data_sdu = (u8_t *) ((u32_t) data_sdu + (u32) pdu_remaining_size);
          // dispatch the pdu
          ((struct mac_tb_req *) (pdu->data))->rlc = NULL;
          ((struct mac_tb_req *) (pdu->data))->data_ptr = &rlc_header->byte1;
          ((struct mac_tb_req *) (pdu->data))->first_bit = 0;
          ((struct mac_tb_req *) (pdu->data))->tb_size_in_bits = rlcP->data_pdu_size_in_bits;
          add_cnt_up_tail (pdu, &rlcP->pdus_to_mac_layer);
          pdu = NULL;
          pdu_remaining_size = 0;
        }
      }
    } else {                    // end if pdu_remaining_size > 0
      if (pdu) {
        ((struct mac_tb_req *) (pdu->data))->rlc = NULL;
        ((struct mac_tb_req *) (pdu->data))->data_ptr = &rlc_header->byte1;
        ((struct mac_tb_req *) (pdu->data))->first_bit = 0;
        ((struct mac_tb_req *) (pdu->data))->tb_size_in_bits = rlcP->data_pdu_size_in_bits;
        add_cnt_up_tail (pdu, &rlcP->pdus_to_mac_layer);
        pdu = NULL;
      }
    }
  }
  free_up (segmented_sdusP);
}

//-----------------------------------------------------------------------------
void
rlc_um_segment_15 (struct rlc_um_entity *rlcP)
{
//-----------------------------------------------------------------------------

  list_up         segmented_sdus;       // the copying of sdu data is done after identification of all LIs to put in pdu
  list_up         pdus;

  struct rlc_um_tx_sdu_management *sdu_mngt;
  struct rlc_um_tx_pdu_management *pdu_mngt;
  struct rlc_um_tx_data_pdu_struct *rlc_header;
  mem_block      *pdu;
  mem_block      *sdu_copy;
  s16_t             pdu_remaining_size;
  u16_t             li[RLC_UM_SEGMENT_NB_MAX_LI_PER_PDU];
  u8_t              discard_go_on = 1;
  u8_t              li_index = 0;
  u8_t              nb_pdu_to_transmit = rlcP->nb_pdu_requested_by_mac;

  pdu = NULL;

  init_up (&segmented_sdus, NULL);      // param string identifying the list is NULL
  init_up (&pdus, NULL);        // param string identifying the list is NULL

  // DISCARD of SDUs : TIMER_BASED_NO_EXPLICIT
  // from 3GPP TS 25.322 V4.2.0
  // Upon expiry of the timer Timer_Discard in the Sender, the Sender shall:
  // -  discard the associated SDU;
  // -  for the first UMD PDU to be transmitted after the discard operation, the Sender shall:
  // -  increment VT(US) so that the "Sequence Number" field in this UMD PDU is incremented with two
  //      compared with the previous UMD PDU;
  // -  fill the first data octet in this UMD PDU with the first octet of an RLC SDU;
  // -  set the first "Length Indicator" in this UMD PDU to indicate that the previous RLC PDU
  //      was exactly filled with the last segment of an RLC SDU (to avoid that the Receiver unnecessarily
  //      discards an extra SDU).
  if ((rlcP->sdu_discard_mode & RLC_SDU_DISCARD_TIMER_BASED_NO_EXPLICIT)) {
    discard_go_on = 1;
    while ((rlcP->input_sdus[rlcP->current_sdu_index]) && discard_go_on) {
      if ((*rlcP->frame_tick_milliseconds - ((struct rlc_um_tx_sdu_management *) (rlcP->input_sdus[rlcP->current_sdu_index]->data))->sdu_creation_time) >= rlcP->timer_discard_init) {
        PRINT_RLC_UM_DISCARD_SDU ("[RLC_UM %p] SDU DISCARDED : TIMED OUT %d ms\n", rlcP,
                                  *rlcP->frame_tick_milliseconds - ((struct rlc_um_tx_sdu_management *) (rlcP->input_sdus[rlcP->current_sdu_index]->data))->sdu_creation_time);
        free_mem_block (rlcP->input_sdus[rlcP->current_sdu_index]);
        rlcP->input_sdus[rlcP->current_sdu_index] = NULL;
#ifdef DEBUG_RLC_UM_VT_US
        msg ("[RLC_UM %p] Inc VT(US) in discarding sdu in rlc_um_segment_15()\n", rlcP);
#endif
        rlcP->vt_us = (rlcP->vt_us + 1) & 0x7F;
        rlcP->li_one_byte_short_to_add_in_next_pdu = 0;
        rlcP->li_exactly_filled_to_add_in_next_pdu = 1;
        rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % rlcP->size_input_sdus_buffer;
      } else {
        discard_go_on = 0;
      }
    }
  }

  while ((rlcP->input_sdus[rlcP->current_sdu_index]) && (nb_pdu_to_transmit > 0)) {

    sdu_mngt = ((struct rlc_um_tx_sdu_management *) (rlcP->input_sdus[rlcP->current_sdu_index]->data));

    // pdu management
    if (!pdu) {
      if (!(pdu = get_free_mem_block (rlcP->data_pdu_size + sizeof (struct rlc_um_tx_data_pdu_management) + GUARD_CRC_LIH_SIZE))) {
        msg ("[RLC_UM %p][SEGMENT] ERROR COULD NOT GET NEW PDU, EXIT\n");
        return;
      }
      li_index = 0;
      pdu_remaining_size = rlcP->data_pdu_size - 2;     // 2= size of header, minimum
      pdu_mngt = (struct rlc_um_tx_pdu_management *) (pdu->data);
      memset (pdu->data, 0, sizeof (struct rlc_um_tx_pdu_management));
      pdu_mngt->first_byte = &pdu->data[sizeof (struct rlc_um_tx_data_pdu_management)];
      rlc_header = (struct rlc_um_tx_data_pdu_struct *) (pdu_mngt->first_byte);

      // management of previous pdu
      // In the case where 15-bit "Length Indicators" are used for the previous PDU and the last segment
      // of an RLC SDU is one octet short of exactly filling the PDU:
      // -      if a 15-bit "Length Indicator" is used for the following PDU:
      //     -  the "Length Indicator" with value "111 1111 1111 1011" shall be placed as the first "Length Indicator" in the following PDU;
      //     -  the remaining one octet in the previous PDU shall be padded by the Sender and ignored at the Receiver
      //        though there is no "Length Indicator" indicating the existence of Padding;
      //     -  in case this SDU was the last one to be transmitted:
      //         -      a RLC PDU consisting of an RLC Header with "Length Indicator" "111 1111 1111 1011" followed by a padding
      //            "Length Indicator" and padding may be transmitted;
      // -      if a 7-bit "Length Indicator" is used for the following PDU:
      //     -  if RLC is configured for UM mode:
      //         -      the "Length Indicator" with value "000 0000" shall be placed as the first "Length indicator"
      //            in the following PDU and its SN shall be incremented by 2 before it is transmitted.
      if ((rlcP->li_one_byte_short_to_add_in_next_pdu)) {
        li[li_index++] = RLC_LI_LAST_PDU_ONE_BYTE_SHORT;
        rlcP->li_one_byte_short_to_add_in_next_pdu = 0;
        pdu_remaining_size -= 2;
        if (sdu_mngt->use_special_li) {
          li[li_index++] = RLC_LI_1ST_BYTE_SDU_IS_1ST_BYTE_PDU;
          pdu_remaining_size -= 2;
        }
      } else if (rlcP->li_exactly_filled_to_add_in_next_pdu) {
        li[li_index++] = RLC_LI_LAST_PDU_EXACTLY_FILLED;
        rlcP->li_exactly_filled_to_add_in_next_pdu = 0;
        pdu_remaining_size -= 2;
        if (sdu_mngt->use_special_li) {
          li[li_index++] = RLC_LI_1ST_BYTE_SDU_IS_1ST_BYTE_PDU;
          pdu_remaining_size -= 2;
        }
      }
    }

    if ((pdu_remaining_size - sdu_mngt->sdu_remaining_size) < 0) {
      pdu_mngt->data_size += pdu_remaining_size;

      rlc_um_encode_pdu_15 (rlcP, rlc_header, pdu_mngt, li, li_index);
      add_up_tail (pdu, &pdus);
      rlcP->li_length_15_was_used_for_previous_pdu = 1;
      pdu = NULL;
      nb_pdu_to_transmit -= 1;
      sdu_mngt->sdu_remaining_size = sdu_mngt->sdu_remaining_size - pdu_remaining_size;
      sdu_mngt->use_special_li = 0;     // this field is not active when the sdu does not start at the begining of a pdu
    } else {

      if ((pdu_remaining_size - sdu_mngt->sdu_remaining_size) >= 2) {

        pdu_remaining_size = pdu_remaining_size - sdu_mngt->sdu_remaining_size - 2;     // size of li length
        pdu_mngt->data_size += sdu_mngt->sdu_remaining_size;
        li[li_index++] = sdu_mngt->sdu_remaining_size << 1;

        sdu_mngt->sdu_remaining_size = 0;
        add_up_tail (rlcP->input_sdus[rlcP->current_sdu_index], &segmented_sdus);
        rlcP->input_sdus[rlcP->current_sdu_index] = NULL;
        rlcP->buffer_occupancy -= sdu_mngt->sdu_size;
        rlcP->nb_sdu -= 1;
        rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % rlcP->size_input_sdus_buffer;

        if (rlcP->nb_sdu == 0) {
          if (pdu_remaining_size >= 2) {
            li[li_index++] = RLC_LI_PDU_PADDING;
            rlc_um_encode_pdu_15 (rlcP, rlc_header, pdu_mngt, li, li_index);
            add_up_tail (pdu, &pdus);
            rlcP->li_length_15_was_used_for_previous_pdu = 1;
            pdu = NULL;
            nb_pdu_to_transmit -= 1;
          } else {
            if (pdu_remaining_size == 0) {
              rlc_um_encode_pdu_15 (rlcP, rlc_header, pdu_mngt, li, li_index);
              add_up_tail (pdu, &pdus);
              pdu = NULL;
              nb_pdu_to_transmit -= 1;

              rlcP->li_exactly_filled_to_add_in_next_pdu = 1;
              rlcP->li_length_15_was_used_for_previous_pdu = 1;

              // In the case where 15-bit "Length Indicators" are used for the previous PDU and the last segment
              // of an RLC SDU is one octet short of exactly filling the PDU:
              // -        if a 15-bit "Length Indicator" is used for the following PDU:
              //   -      the "Length Indicator" with value "111 1111 1111 1011" shall be placed as the first "Length Indicator" in the following PDU;
              //   -      the remaining one octet in the previous PDU shall be padded by the Sender and ignored at the Receiver
              //      though there is no "Length Indicator" indicating the existence of Padding;
              //   -      in case this SDU was the last one to be transmitted:
              //     -    a RLC PDU consisting of an RLC Header with "Length Indicator" "111 1111 1111 1011" followed by a padding
              //       "Length Indicator" and padding may be transmitted;
            } else if (pdu_remaining_size == 1) {       // one byte remaining
              rlc_um_encode_pdu_15 (rlcP, rlc_header, pdu_mngt, li, li_index);
              add_up_tail (pdu, &pdus);
              pdu = NULL;
              nb_pdu_to_transmit -= 1;
              rlcP->li_one_byte_short_to_add_in_next_pdu = 1;
              rlcP->li_length_15_was_used_for_previous_pdu = 1;
            }
          }
        }
        /*
           } else { // else:if (!last_sdu)
           } // the while loop continue with the same pdu  */
      } else if ((pdu_remaining_size - sdu_mngt->sdu_remaining_size) == 0) {
        // from 3GPP TS 25.322 V4.2.0
        //In the case where the end of the last segment of an RLC SDU exactly ends at the end of a PDU and there is
        // no "Length Indicator" that indicates the end of the RLC SDU:
        // -    if 7-bit "Length Indicator" is used:
        // -    a "Length Indicator" with value "000 0000" shall be placed as the first "Length Indicator" in the following PDU;
        // -    if 15-bit "Length Indicator" is used:
        // -    a "Length Indicator" with value "000 0000 0000 0000" shall be placed as the first "Length Indicator" in the following PDU.
        pdu_mngt->data_size += pdu_remaining_size;
        rlc_um_encode_pdu_15 (rlcP, rlc_header, pdu_mngt, li, li_index);
        add_up_tail (pdu, &pdus);
        pdu = NULL;
        nb_pdu_to_transmit -= 1;

        sdu_mngt->sdu_remaining_size = 0;
        add_up_tail (rlcP->input_sdus[rlcP->current_sdu_index], &segmented_sdus);
        rlcP->input_sdus[rlcP->current_sdu_index] = NULL;
        rlcP->buffer_occupancy -= sdu_mngt->sdu_size;
        rlcP->nb_sdu -= 1;
        rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % rlcP->size_input_sdus_buffer;

        rlcP->li_exactly_filled_to_add_in_next_pdu = 1;
        rlcP->li_length_15_was_used_for_previous_pdu = 1;
      } else if ((pdu_remaining_size - sdu_mngt->sdu_remaining_size) == 1) {    // one byte remaining
        // from 3GPP TS 25.322 V4.2.0
        // In the case where a PDU contains a 15-bit "Length Indicator" indicating that an RLC SDU ends with
        // one octet left in the PDU, the last octet of this PDU shall:
        // -    be padded by the Sender and ignored by the Receiver though there is no "Length Indicator"
        // indicating the existence of Padding; and
        // -    not be filled with the first octet of the next RLC SDU data.

        // In the case where 15-bit "Length Indicators" are used for the previous PDU and the last segment
        // of an RLC SDU is one octet short of exactly filling the PDU:
        // -    if a 15-bit "Length Indicator" is used for the following PDU:
        //   -  the "Length Indicator" with value "111 1111 1111 1011" shall be placed as the first "Length Indicator" in the following PDU;
        //   -  the remaining one octet in the previous PDU shall be padded by the Sender and ignored at the Receiver
        //      though there is no "Length Indicator" indicating the existence of Padding;
        //   -  in case this SDU was the last one to be transmitted:
        //     -        a RLC PDU consisting of an RLC Header with "Length Indicator" "111 1111 1111 1011" followed by a padding
        //       "Length Indicator" and padding may be transmitted;
        pdu_mngt->data_size += sdu_mngt->sdu_remaining_size;
        rlc_um_encode_pdu_15 (rlcP, rlc_header, pdu_mngt, li, li_index);
        add_up_tail (pdu, &pdus);
        pdu = NULL;
        nb_pdu_to_transmit -= 1;

        sdu_mngt->sdu_remaining_size = 0;
        add_up_tail (rlcP->input_sdus[rlcP->current_sdu_index], &segmented_sdus);
        rlcP->input_sdus[rlcP->current_sdu_index] = NULL;
        rlcP->buffer_occupancy -= sdu_mngt->sdu_size;
        rlcP->nb_sdu -= 1;
        rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % rlcP->size_input_sdus_buffer;

        rlcP->li_one_byte_short_to_add_in_next_pdu = 1;
        rlcP->li_length_15_was_used_for_previous_pdu = 1;
      }
    }
  }

  if (nb_pdu_to_transmit > 0) {
    if ((rlcP->li_one_byte_short_to_add_in_next_pdu)) {
      pdu = rlc_um_build_pdu_with_only_2li (rlcP, RLC_LI_LAST_PDU_ONE_BYTE_SHORT, RLC_LI_PDU_PADDING);
      add_up_tail (pdu, &pdus);
      rlcP->li_one_byte_short_to_add_in_next_pdu = 0;
    } else if (rlcP->li_exactly_filled_to_add_in_next_pdu) {
      pdu = rlc_um_build_pdu_with_only_2li (rlcP, RLC_LI_LAST_PDU_EXACTLY_FILLED, RLC_LI_PDU_PADDING);
      add_up_tail (pdu, &pdus);
      rlcP->li_exactly_filled_to_add_in_next_pdu = 0;
    }
    if (!(sdu_mngt)) {
      rlc_um_fill_pdus (rlcP, &pdus, &segmented_sdus);
    }
  }
  if ((sdu_mngt)) {
    if (sdu_mngt->sdu_remaining_size > 0) {
      sdu_copy = get_free_copy_mem_block ();
      sdu_copy->data = (u8_t *) sdu_mngt;
      add_up_tail (sdu_copy, &segmented_sdus);
    }
    rlc_um_fill_pdus (rlcP, &pdus, &segmented_sdus);
  }
}

//-----------------------------------------------------------------------------
void
rlc_um_segment_7 (struct rlc_um_entity *rlcP)
{
//-----------------------------------------------------------------------------

  list_up         segmented_sdus;       // the copying of sdu data is done after identification of all LIs to put in pdu
  list_up         pdus;

  struct rlc_um_tx_sdu_management *sdu_mngt = NULL;
  struct rlc_um_tx_pdu_management *pdu_mngt;
  struct rlc_um_tx_data_pdu_struct *rlc_header;
  mem_block      *pdu;
  mem_block      *sdu_copy;
  s16_t             pdu_remaining_size;
  u16_t             li[RLC_UM_SEGMENT_NB_MAX_LI_PER_PDU];
  u8_t              discard_go_on = 1;
  u8_t              li_index = 0;
  u8_t              nb_pdu_to_transmit = rlcP->nb_pdu_requested_by_mac;

  pdu = NULL;

  init_up (&segmented_sdus, NULL);      // param string identifying the list is NULL
  init_up (&pdus, NULL);        // param string identifying the list is NULL

  // DISCARD of SDUs : TIMER_BASED_NO_EXPLICIT
  // from 3GPP TS 25.322 V4.2.0
  // Upon expiry of the timer Timer_Discard in the Sender, the Sender shall:
  // -  discard the associated SDU;
  // -  for the first UMD PDU to be transmitted after the discard operation, the Sender shall:
  // -  increment VT(US) so that the "Sequence Number" field in this UMD PDU is incremented with two
  //      compared with the previous UMD PDU;
  // -  fill the first data octet in this UMD PDU with the first octet of an RLC SDU;
  // -  set the first "Length Indicator" in this UMD PDU to indicate that the previous RLC PDU
  //      was exactly filled with the last segment of an RLC SDU (to avoid that the Receiver unnecessarily
  //      discards an extra SDU).
  if ((rlcP->sdu_discard_mode & RLC_SDU_DISCARD_TIMER_BASED_NO_EXPLICIT)) {
    discard_go_on = 1;
    while ((rlcP->input_sdus[rlcP->current_sdu_index]) && discard_go_on) {
      if ((*rlcP->frame_tick_milliseconds - ((struct rlc_um_tx_sdu_management *) (rlcP->input_sdus[rlcP->current_sdu_index]->data))->sdu_creation_time) >= rlcP->timer_discard_init) {
        PRINT_RLC_UM_DISCARD_SDU ("[RLC_UM %p] SDU DISCARDED  TIMED OUT %ld ms ", rlcP,
                                  (*rlcP->frame_tick_milliseconds - ((struct rlc_um_tx_sdu_management *) (rlcP->input_sdus[rlcP->current_sdu_index]->data))->sdu_creation_time));
        PRINT_RLC_UM_DISCARD_SDU ("BO %d, NB SDU %d\n", rlcP->buffer_occupancy, rlcP->nb_sdu);
        rlcP->buffer_occupancy -= ((struct rlc_um_tx_sdu_management *) (rlcP->input_sdus[rlcP->current_sdu_index]->data))->sdu_remaining_size;
        rlcP->nb_sdu -= 1;
        free_mem_block (rlcP->input_sdus[rlcP->current_sdu_index]);
        rlcP->input_sdus[rlcP->current_sdu_index] = NULL;
#ifdef DEBUG_RLC_UM_VT_US
        msg ("[RLC_UM %p] Inc VT(US) in discarding SDU in rlc_um_segment_7()\n", rlcP);
#endif
        rlcP->vt_us = (rlcP->vt_us + 1) & 0x7F;
        rlcP->li_one_byte_short_to_add_in_next_pdu = 0;
        rlcP->li_exactly_filled_to_add_in_next_pdu = 1;
        rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % rlcP->size_input_sdus_buffer;
      } else {
        discard_go_on = 0;
      }
    }
  }

  while ((rlcP->input_sdus[rlcP->current_sdu_index]) && (nb_pdu_to_transmit > 0)) {

    sdu_mngt = ((struct rlc_um_tx_sdu_management *) (rlcP->input_sdus[rlcP->current_sdu_index]->data));
    PRINT_RLC_UM_SEGMENT ("[RLC_UM %p] SEGMENT GET NEW SDU %p AVAILABLE SIZE %d Bytes\n", rlcP, sdu_mngt, sdu_mngt->sdu_remaining_size);

    // pdu management
    if (!pdu) {
      if (!(pdu = get_free_mem_block (rlcP->data_pdu_size + sizeof (struct rlc_um_tx_data_pdu_management) + GUARD_CRC_LIH_SIZE))) {
        msg ("[RLC_UM %p][SEGMENT 7] ERROR COULD NOT GET NEW PDU, EXIT\n");
        return;
      }
      li_index = 0;
      pdu_remaining_size = rlcP->data_pdu_size - 1;     // 1= size of header, minimum
      pdu_mngt = (struct rlc_um_tx_pdu_management *) (pdu->data);
      memset (pdu->data, 0, sizeof (struct rlc_um_tx_pdu_management));
      pdu_mngt->first_byte = &pdu->data[sizeof (struct rlc_um_tx_data_pdu_management)];
      rlc_header = (struct rlc_um_tx_data_pdu_struct *) (pdu_mngt->first_byte);

      PRINT_RLC_UM_SEGMENT ("[RLC_UM %p][SEGMENT 7] CONSTRUCT NEW PDU\n", rlcP);
      // management of previous pdu
      // In the case where 15-bit "Length Indicators" are used for the previous PDU and the last segment
      // of an RLC SDU is one octet short of exactly filling the PDU:
      // -      if a 15-bit "Length Indicator" is used for the following PDU:
      //     -  the "Length Indicator" with value "111 1111 1111 1011" shall be placed as the first "Length Indicator" in the following PDU;
      //     -  the remaining one octet in the previous PDU shall be padded by the Sender and ignored at the Receiver
      //        though there is no "Length Indicator" indicating the existence of Padding;
      //     -  in case this SDU was the last one to be transmitted:
      //         -      a RLC PDU consisting of an RLC Header with "Length Indicator" "111 1111 1111 1011" followed by a padding
      //            "Length Indicator" and padding may be transmitted;
      // -      if a 7-bit "Length Indicator" is used for the following PDU:
      //     -  if RLC is configured for UM mode:
      //         -      the "Length Indicator" with value "000 0000" shall be placed as the first "Length indicator"
      //            in the following PDU and its SN shall be incremented by 2 before it is transmitted.
      if (rlcP->li_exactly_filled_to_add_in_next_pdu) {
        li[li_index++] = RLC_LI_LAST_PDU_EXACTLY_FILLED;
        rlcP->li_exactly_filled_to_add_in_next_pdu = 0;
        pdu_remaining_size -= 1;
        if (sdu_mngt->use_special_li) {
          li[li_index++] = RLC_LI_1ST_BYTE_SDU_IS_1ST_BYTE_PDU;
          pdu_remaining_size -= 1;
          sdu_mngt->use_special_li = 0; // this field is not active when the sdu does not start at the begining of a pdu
        }
      } else if (rlcP->li_one_byte_short_to_add_in_next_pdu) {
        li[li_index++] = RLC_LI_LAST_PDU_ONE_BYTE_SHORT;
        rlcP->li_one_byte_short_to_add_in_next_pdu = 0;
        pdu_remaining_size -= 1;
        if (rlcP->li_length_15_was_used_for_previous_pdu) {
#ifdef DEBUG_RLC_UM_VT_US
          msg ("[RLC_UM %p] Inc VT(US) in rlc_um_segment_7()/li_length_15_was_used_for_previous_pdu\n", rlcP);
#endif
          rlcP->vt_us = (rlcP->vt_us + 1) & 0x7F;
        }
      }
    }

    if (sdu_mngt->use_special_li) {
      PRINT_RLC_UM_SEGMENT ("[RLC_UM %p][SEGMENT 7] ADD SPECIAL LI\n", rlcP);
      li[li_index++] = RLC_LI_1ST_BYTE_SDU_IS_1ST_BYTE_PDU;
      pdu_remaining_size -= 1;
      sdu_mngt->use_special_li = 0;     // this field is not active when the sdu does not start at the begining of a pdu
    }

    if ((pdu_remaining_size - sdu_mngt->sdu_remaining_size) < 0) {
      pdu_mngt->data_size += pdu_remaining_size;

      rlcP->buffer_occupancy -= pdu_remaining_size;
      rlc_um_encode_pdu_7 (rlcP, rlc_header, pdu_mngt, li, li_index);
      add_up_tail (pdu, &pdus);
      rlcP->li_length_15_was_used_for_previous_pdu = 0;
      PRINT_RLC_UM_SEGMENT ("[RLC_UM %p][SEGMENT 7] PDU %p CONSTRUCTED WITH PAYLOAD %d Bytes offset_end_sdu %d @0\n", rlcP, pdu, pdu_mngt->data_size, sdu_mngt->sdu_remaining_size);
      pdu = NULL;
      nb_pdu_to_transmit -= 1;
      sdu_mngt->sdu_remaining_size = sdu_mngt->sdu_remaining_size - pdu_remaining_size;
      sdu_mngt->use_special_li = 0;     // this field is not active when the sdu does not start at the begining of a pdu
    } else {

      if ((pdu_remaining_size - sdu_mngt->sdu_remaining_size) >= 1) {

        pdu_remaining_size = pdu_remaining_size - sdu_mngt->sdu_remaining_size - 1;     // size of li length
        pdu_mngt->data_size += sdu_mngt->sdu_remaining_size;
        li[li_index++] = sdu_mngt->sdu_remaining_size << 1;

        rlcP->buffer_occupancy -= sdu_mngt->sdu_remaining_size;
        sdu_mngt->sdu_remaining_size = 0;
        add_up_tail (rlcP->input_sdus[rlcP->current_sdu_index], &segmented_sdus);
        rlcP->input_sdus[rlcP->current_sdu_index] = NULL;
        rlcP->nb_sdu -= 1;
        rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % rlcP->size_input_sdus_buffer;

        if (rlcP->nb_sdu == 0) {
          if (pdu_remaining_size >= 1) {
            li[li_index++] = RLC_LI_PDU_PADDING;
            rlc_um_encode_pdu_7 (rlcP, rlc_header, pdu_mngt, li, li_index);
            add_up_tail (pdu, &pdus);
            PRINT_RLC_UM_SEGMENT ("[RLC_UM %p][SEGMENT 7] PDU %p CONSTRUCTED WITH PAYLOAD %d Bytes offset_end_sdu %d @1\n", rlcP, pdu, pdu_mngt->data_size, sdu_mngt->sdu_remaining_size);
            rlcP->li_length_15_was_used_for_previous_pdu = 0;
            pdu = NULL;
            nb_pdu_to_transmit -= 1;
          } else {
            if (pdu_remaining_size == 0) {
              rlc_um_encode_pdu_7 (rlcP, rlc_header, pdu_mngt, li, li_index);
              add_up_tail (pdu, &pdus);
              PRINT_RLC_UM_SEGMENT ("[RLC_UM %p][SEGMENT 7]PDU %p CONSTRUCTED WITH PAYLOAD %d Bytes offset_end_sdu %d @2\n", rlcP, pdu, pdu_mngt->data_size, sdu_mngt->sdu_remaining_size);
              pdu = NULL;
              nb_pdu_to_transmit -= 1;

              rlcP->li_exactly_filled_to_add_in_next_pdu = 1;
              rlcP->li_length_15_was_used_for_previous_pdu = 0;
            }
          }
        }
        /*
           } else { // else:if (!last_sdu)
           } // the while loop continue with the same pdu  */
      } else if ((pdu_remaining_size - sdu_mngt->sdu_remaining_size) == 0) {
        // from 3GPP TS 25.322 V4.2.0
        //In the case where the end of the last segment of an RLC SDU exactly ends at the end of a PDU and there is
        // no "Length Indicator" that indicates the end of the RLC SDU:
        // -    if 7-bit "Length Indicator" is used:
        // -    a "Length Indicator" with value "000 0000" shall be placed as the first "Length Indicator" in the following PDU;
        // -    if 15-bit "Length Indicator" is used:
        // -    a "Length Indicator" with value "000 0000 0000 0000" shall be placed as the first "Length Indicator" in the following PDU.
        pdu_mngt->data_size += pdu_remaining_size;
        rlc_um_encode_pdu_7 (rlcP, rlc_header, pdu_mngt, li, li_index);
        add_up_tail (pdu, &pdus);
        PRINT_RLC_UM_SEGMENT ("[RLC_UM %p][SEGMENT 7] PDU %p CONSTRUCTED WITH PAYLOAD %d Bytes offset_end_sdu %d @3\n", rlcP, pdu, pdu_mngt->data_size, sdu_mngt->sdu_remaining_size);
        pdu = NULL;
        nb_pdu_to_transmit -= 1;

        rlcP->buffer_occupancy -= sdu_mngt->sdu_remaining_size;
        sdu_mngt->sdu_remaining_size = 0;

        add_up_tail (rlcP->input_sdus[rlcP->current_sdu_index], &segmented_sdus);
        rlcP->input_sdus[rlcP->current_sdu_index] = NULL;
        rlcP->nb_sdu -= 1;
        rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % rlcP->size_input_sdus_buffer;

        rlcP->li_exactly_filled_to_add_in_next_pdu = 1;
        rlcP->li_length_15_was_used_for_previous_pdu = 0;
      }
    }
  }
  if (nb_pdu_to_transmit > 0) {
    if ((rlcP->li_one_byte_short_to_add_in_next_pdu)) {
      pdu = rlc_um_build_pdu_with_only_2li (rlcP, RLC_LI_LAST_PDU_ONE_BYTE_SHORT, RLC_LI_PDU_PADDING);
      add_up_tail (pdu, &pdus);
      rlcP->li_one_byte_short_to_add_in_next_pdu = 0;
    } else if (rlcP->li_exactly_filled_to_add_in_next_pdu) {
      pdu = rlc_um_build_pdu_with_only_2li (rlcP, RLC_LI_LAST_PDU_EXACTLY_FILLED, RLC_LI_PDU_PADDING);
      add_up_tail (pdu, &pdus);
      rlcP->li_exactly_filled_to_add_in_next_pdu = 0;
    }
    if (!(sdu_mngt)) {
      rlc_um_fill_pdus (rlcP, &pdus, &segmented_sdus);
    }
  }
  if ((sdu_mngt)) {
    if (sdu_mngt->sdu_remaining_size > 0) {
      sdu_copy = get_free_copy_mem_block ();
      sdu_copy->data = (u8_t *) sdu_mngt;
      add_up_tail (sdu_copy, &segmented_sdus);
    }
    rlc_um_fill_pdus (rlcP, &pdus, &segmented_sdus);
  }
}

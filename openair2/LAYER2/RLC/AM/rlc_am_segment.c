/***************************************************************************
                          rlc_am_segment.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr
 ***************************************************************************/
#include "rtos_header.h"
//-----------------------------------------------------------------------------
#include "list.h"
#include "rlc_am_entity.h"
#include "rlc_am_structs.h"
#include "rlc_am_constants.h"
#include "rlc_primitives.h"
#include "mem_block.h"

//#define DEBUG_RLC_AM_SEGMENT
//#define DEBUG_RLC_AM_SEGMENT_FILL_DATA
//#define DEBUG_MEM_MNGT
//-----------------------------------------------------------------------------
mem_block_t      *rlc_am_segment_15 (struct rlc_am_entity *rlcP);
mem_block_t      *rlc_am_segment_7  (struct rlc_am_entity *rlcP);
//-----------------------------------------------------------------------------
inline void
rlc_am_encode_pdu_15 (struct rlc_am_pdu_header *rlc_headerP, struct rlc_am_tx_data_pdu_management *pdu_mngtP, u16_t * li_arrayP, u8_t nb_liP)
{
//-----------------------------------------------------------------------------
  u8_t              li_index;

#ifdef DEBUG_RLC_AM_SEGMENT
  int             index;
  msg ("[RLC_AM] SEGMENT_15 PDU SET LIs: ");
  for (index = 0; index < nb_liP; index++) {
    msg ("%04X ", li_arrayP[index]);
  }
  msg ("\n");
#endif
  rlc_headerP->byte1 = RLC_DC_DATA_PDU;
  rlc_headerP->byte2 = 0;

  nb_liP = nb_liP << 1;
  pdu_mngtP->payload = (u8_t *) ((u32_t) (&rlc_headerP->li_data_7[nb_liP]));

  if (nb_liP) {
    rlc_headerP->byte2 |= RLC_E_NEXT_FIELD_IS_LI_E;
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
rlc_am_encode_pdu_7 (struct rlc_am_pdu_header *rlc_headerP, struct rlc_am_tx_data_pdu_management *pdu_mngtP, u16_t * li_arrayP, u8_t nb_liP)
{
//-----------------------------------------------------------------------------
  u8_t              li_index;

#ifdef DEBUG_RLC_AM_SEGMENT
  int             index;
  msg ("[RLC_AM] SEGMENT_7 PDU %p SET LIs: ", pdu_mngtP);
  for (index = 0; index < nb_liP; index++) {
    msg ("%04X ", li_arrayP[index]);
  }
  msg ("\n");
#endif

  rlc_headerP->byte1 = RLC_DC_DATA_PDU;
  rlc_headerP->byte2 = 0;

  pdu_mngtP->payload = (u8_t *) ((u32_t) (&rlc_headerP->li_data_7[nb_liP]));

  if (nb_liP) {
    rlc_headerP->byte2 |= RLC_E_NEXT_FIELD_IS_LI_E;
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
rlc_am_fill_pdu (mem_block_t * pduP, list_t * segmented_sdusP)
{
//-----------------------------------------------------------------------------
  mem_block_t      *sdu;
  u8_t             *data_sdu;
  u8_t             *data_pdu;
  struct rlc_am_pdu_header *rlc_header;
  struct rlc_am_tx_data_pdu_management *pdu_mngt;
  struct rlc_am_tx_sdu_management *sdu_mngt;
  int             continue_fill;
  u16_t             pdu_remaining_size;
  u16_t             sdu_available_size;


  sdu = NULL;

  pdu_mngt = (struct rlc_am_tx_data_pdu_management *) (pduP->data);
  rlc_header = (struct rlc_am_pdu_header *) (pdu_mngt->first_byte);

  pdu_remaining_size = pdu_mngt->data_size;
  data_pdu = (u8_t *) pdu_mngt->payload;
#ifdef DEBUG_RLC_AM_SEGMENT_FILL_DATA
  msg ("[RLC_AM] SEGMENT FILL GET PDU %p REMAINING SIZE=%d\n", pduP, pdu_remaining_size);
#endif

  continue_fill = 1;
  while ((continue_fill)) {
    // fill the pdu with data of sdu
    if (sdu == NULL) {
      sdu = list_remove_head (segmented_sdusP);
      if (sdu == NULL) {
        return;                 // case pdu only 2 LIs : exactly_filled or one_byte_short + paddind
      }
      sdu_mngt = (struct rlc_am_tx_sdu_management *) (sdu->data);
      data_sdu = &sdu->data[sizeof (struct rlc_am_tx_sdu_management) + sdu_mngt->sdu_segmented_size];
      sdu_available_size = sdu_mngt->sdu_size - sdu_mngt->sdu_remaining_size - sdu_mngt->sdu_segmented_size;
#ifdef DEBUG_RLC_AM_SEGMENT_FILL_DATA
      msg ("[RLC_AM] SEGMENT FILL GET SDU %p AVAILABLE_SZ=%d (SZ %d REMAIN_SZ %d SEGMENT_SZ %d)\n",
           sdu, sdu_available_size, sdu_mngt->sdu_size, sdu_mngt->sdu_remaining_size, sdu_mngt->sdu_segmented_size);
#endif
    }
    // copy the whole remaining data of the sdu in the remaining area of the pdu
    if (pdu_remaining_size >= sdu_available_size) {
      memcpy (data_pdu, data_sdu, sdu_available_size);
#ifdef DEBUG_RLC_AM_SEGMENT_FILL_DATA
      msg ("[RLC_AM] SEGMENT FILL PDU %p WITH SDU %p  %p <- %p %d bytes (SZ %d REMAIN_SZ %d SEGMENT_SZ %d)\n",
           pduP, sdu, data_pdu, data_sdu, sdu_available_size, sdu_mngt->sdu_size, sdu_mngt->sdu_remaining_size, sdu_mngt->sdu_segmented_size);
#endif
      pdu_remaining_size -= sdu_available_size;
      sdu_mngt->sdu_segmented_size += sdu_available_size;
      data_pdu = (u8_t *) ((u32_t) data_pdu + sdu_available_size);
      sdu_available_size = 0;

      // dispatch the sdu
      sdu = NULL;

      // dispatch the pdu
      if (pdu_remaining_size == 0) {
        continue_fill = 0;
      } else {
      }
      // copy some data of the sdu in the whole remaining area of the pdu
    } else {
      memcpy (data_pdu, data_sdu, pdu_remaining_size);
#ifdef DEBUG_RLC_AM_SEGMENT_FILL_DATA
      msg ("[RLC_AM] SEGMENT FILL PDU %p WITH SDU %p  %p <- %p %d bytes (SZ %d REMAIN_SZ %d SEGMENT_SZ %d)\n",
           pduP, sdu, data_pdu, data_sdu, pdu_remaining_size, sdu_mngt->sdu_size, sdu_mngt->sdu_remaining_size, sdu_mngt->sdu_segmented_size);
#endif
      sdu_mngt->sdu_segmented_size += pdu_remaining_size;
      sdu_available_size -= pdu_remaining_size;
      data_sdu = (u8_t *) ((u32_t) data_sdu + (u32) pdu_remaining_size);
      continue_fill = 0;
      pdu_remaining_size = 0;
    }
  }
  list_free (segmented_sdusP);
}

//-----------------------------------------------------------------------------
mem_block_t      *
rlc_am_segment_15 (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------

  list_t         segmented_sdus;       // the copying of sdu data is done after identification of all LIs to put in pdu

  struct rlc_am_tx_sdu_management *sdu_mngt;
  struct rlc_am_tx_data_pdu_management *pdu_mngt;
  struct rlc_am_pdu_header *rlc_header;
  mem_block_t      *pdu;
  int             continue_segment = 1;
  s16_t             pdu_remaining_size;
  u8_t              li_index = 0;

  pdu = NULL;

  list_init (&segmented_sdus, NULL);      // param string identifying the list is NULL

  while ((rlcP->input_sdus[rlcP->current_sdu_index]) && (continue_segment)) {

    sdu_mngt = ((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[rlcP->current_sdu_index]->data));
    if (!(sdu_mngt->segmented)) {
      // pdu management
      if (!(pdu)) {
        if (!(pdu = get_free_mem_block (rlcP->pdu_size + sizeof (struct rlc_am_tx_data_pdu_allocation) + GUARD_CRC_LIH_SIZE))) {
#ifdef DEBUG_MEM_MNGT
          msg ("[RLC_AM][RB %d][SEGMENT] ERROR COULD NOT GET NEW PDU, EXIT\n", rlcP->rb_id);
#endif
          return NULL;
        }
        li_index = 0;
        pdu_remaining_size = rlcP->pdu_size - 2;        // 2= size of header, minimum
        pdu_mngt = (struct rlc_am_tx_data_pdu_management *) (pdu->data);
        memset (pdu->data, 0, sizeof (struct rlc_am_tx_data_pdu_allocation));
        pdu_mngt->rlc_tb_type = RLC_AM_DATA_PDU_TYPE;
        pdu_mngt->first_byte = &pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)];
        rlc_header = (struct rlc_am_pdu_header *) (pdu_mngt->first_byte);
      }
      if (sdu_mngt->sdu_remaining_size > 0) {   // think RLC_LI_LAST_PDU_ONE_BYTE_SHORT, RLC_LI_LAST_PDU_EXACTLY_FILLED

        if ((pdu_remaining_size - sdu_mngt->sdu_remaining_size) < 0) {
          pdu_mngt->data_size += pdu_remaining_size;

          rlc_am_encode_pdu_15 (rlc_header, pdu_mngt, rlcP->li, li_index);
          continue_segment = 0;
          sdu_mngt->sdu_remaining_size = sdu_mngt->sdu_remaining_size - pdu_remaining_size;

          sdu_mngt->nb_pdus += 1;
          pdu_mngt->sdu[pdu_mngt->nb_sdu++] = rlcP->current_sdu_index;
        } else {

          // From 3GPP TS 25.322 V4.2.0 (2001-09) p28
          // If "SDU discard with explicit signalling" is configured:
          // -  an AMD PDU can contain a maximum number of 15 "Length Indicators" indicating the end
          //    of 15 corresponding SDUs; and
          // -  the rest of the AMD PDU space shall be used as padding or as piggybacked STATUS PDU.

          // implementation limitation : we limit the number of li to 15 even  if
          // no "SDU discard with explicit signalling" is configured.
          if (li_index >= 14) {
            rlcP->li[li_index++] = RLC_LI_PDU_PADDING;
            rlc_am_encode_pdu_15 (rlc_header, pdu_mngt, rlcP->li, li_index);
            continue_segment = 0;
            sdu_mngt->no_new_sdu_segmented_in_last_pdu = 1;
          } else {              // nb li have not reached the limit

            if ((pdu_remaining_size - sdu_mngt->sdu_remaining_size) >= 2) {

              pdu_remaining_size = pdu_remaining_size - sdu_mngt->sdu_remaining_size - 2;       // size of li length
              pdu_mngt->data_size += sdu_mngt->sdu_remaining_size;

              sdu_mngt->li_index_for_discard = li_index;
              rlcP->li[li_index++] = sdu_mngt->sdu_remaining_size << 1;

              sdu_mngt->sdu_remaining_size = 0;
              sdu_mngt->segmented = 1;

              sdu_mngt->nb_pdus += 1;
              pdu_mngt->sdu[pdu_mngt->nb_sdu++] = rlcP->current_sdu_index;
              pdu_mngt->last_pdu_of_sdu += 1;

              list_add_tail_eurecom (rlcP->input_sdus[rlcP->current_sdu_index], &segmented_sdus);

              rlcP->buffer_occupancy -= sdu_mngt->sdu_size;
              rlcP->nb_sdu -= 1;
              rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % rlcP->size_input_sdus_buffer;
              //} else the while loop continue with the same pdu
            } else if ((pdu_remaining_size - sdu_mngt->sdu_remaining_size) == 0) {
              // from 3GPP TS 25.322 V4.2.0
              //In the case where the end of the last segment of an RLC SDU exactly ends at the end of
              // a PDU and there is no "Length Indicator" that indicates the end of the RLC SDU:
              // -      if 7-bit "Length Indicator" is used:
              // -      a "Length Indicator" with value "000 0000" shall be placed as the first "Length
              //       Indicator" in the following PDU;
              // -      if 15-bit "Length Indicator" is used:
              // -      a "Length Indicator" with value "000 0000 0000 0000" shall be placed as the first
              //       "Length Indicator" in the following PDU.
              pdu_mngt->data_size += pdu_remaining_size;
              rlc_am_encode_pdu_15 (rlc_header, pdu_mngt, rlcP->li, li_index);
              continue_segment = 0;
              sdu_mngt->sdu_remaining_size = 0;

              sdu_mngt->nb_pdus += 1;
              pdu_mngt->sdu[pdu_mngt->nb_sdu++] = rlcP->current_sdu_index;

              list_add_tail_eurecom (rlcP->input_sdus[rlcP->current_sdu_index], &segmented_sdus);
              rlcP->li_exactly_filled_to_add_in_next_pdu = 1;
            } else if ((pdu_remaining_size - sdu_mngt->sdu_remaining_size) == 1) {      // one byte remaining
              // from 3GPP TS 25.322 V4.2.0
              // In the case where a PDU contains a 15-bit "Length Indicator" indicating that an RLC SDU ends with
              // one octet left in the PDU, the last octet of this PDU shall:
              // -      be padded by the Sender and ignored by the Receiver though there is no "Length Indicator"
              // indicating the existence of Padding; and
              // -      not be filled with the first octet of the next RLC SDU data.

              // In the case where 15-bit "Length Indicators" are used for the previous PDU and
              // the last segment of an RLC SDU is one octet short of exactly filling the PDU:
              // -      if a 15-bit "Length Indicator" is used for the following PDU:
              //   -    the "Length Indicator" with value "111 1111 1111 1011" shall be placed as
              //      the first "Length Indicator" in the following PDU;
              //   -    the remaining one octet in the previous PDU shall be padded by the Sender
              //      and ignored at the Receiver though there is no "Length Indicator" indicating
              //      the existence of Padding;
              //   -    in case this SDU was the last one to be transmitted:
              //     -  a RLC PDU consisting of an RLC Header with "Length Indicator"
              //        "111 1111 1111 1011" followed by a padding "Length Indicator" and padding
              //         may be transmitted;
              pdu_mngt->data_size += pdu_remaining_size;
              rlc_am_encode_pdu_15 (rlc_header, pdu_mngt, rlcP->li, li_index);
              continue_segment = 0;
              sdu_mngt->sdu_remaining_size = 0;

              sdu_mngt->nb_pdus += 1;
              pdu_mngt->sdu[pdu_mngt->nb_sdu++] = rlcP->current_sdu_index;

              list_add_tail_eurecom (rlcP->input_sdus[rlcP->current_sdu_index], &segmented_sdus);
              rlcP->li_one_byte_short_to_add_in_next_pdu = 1;
            }
          }
        }
        // management of previous pdu
        // In the case where 15-bit "Length Indicators" are used for the previous PDU and
        // the last segmentof an RLC SDU is one octet short of exactly filling the PDU:
        // -      if a 15-bit "Length Indicator" is used for the following PDU:
        //     -  the "Length Indicator" with value "111 1111 1111 1011" shall be placed
        //        as the first "Length Indicator" in the following PDU;
        //     -  the remaining one octet in the previous PDU shall be padded by the Sender
        //        and ignored at the Receiver though there is no "Length Indicator" indicating
        //        the existence of Padding;
        //     -  in case this SDU was the last one to be transmitted:
        //         -      a RLC PDU consisting of an RLC Header with "Length Indicator"
        //            "111 1111 1111 1011" followed by a padding "Length Indicator" and padding
        //             may be transmitted;
      } else if ((rlcP->li_one_byte_short_to_add_in_next_pdu)) {
        sdu_mngt->li_index_for_discard = li_index;
        rlcP->li[li_index++] = RLC_LI_LAST_PDU_ONE_BYTE_SHORT;
        rlcP->li_one_byte_short_to_add_in_next_pdu = 0;
        sdu_mngt->segmented = 1;
        pdu_remaining_size -= 2;

        pdu_mngt->last_pdu_of_sdu += 1;
        pdu_mngt->sdu[pdu_mngt->nb_sdu++] = rlcP->current_sdu_index;
        sdu_mngt->nb_pdus += 1;

        rlcP->buffer_occupancy -= sdu_mngt->sdu_size;
        rlcP->nb_sdu -= 1;

        rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % rlcP->size_input_sdus_buffer;
      } else if ((rlcP->li_exactly_filled_to_add_in_next_pdu)) {
        sdu_mngt->li_index_for_discard = li_index;
        rlcP->li[li_index++] = RLC_LI_LAST_PDU_EXACTLY_FILLED;
        rlcP->li_exactly_filled_to_add_in_next_pdu = 0;
        sdu_mngt->segmented = 1;
        pdu_remaining_size -= 2;

        pdu_mngt->last_pdu_of_sdu += 1;
        pdu_mngt->sdu[pdu_mngt->nb_sdu++] = rlcP->current_sdu_index;
        sdu_mngt->nb_pdus += 1;

        rlcP->buffer_occupancy -= sdu_mngt->sdu_size;
        rlcP->nb_sdu -= 1;

        rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % rlcP->size_input_sdus_buffer;
      }
    }


    if (sdu_mngt->sdu_remaining_size > 0) {
      list_add_tail_eurecom (rlcP->input_sdus[rlcP->current_sdu_index], &segmented_sdus);
    }
  }
  if ((pdu)) {
    if ((continue_segment > 0)) {       // means pdu not totaly filled
      if (pdu_remaining_size >= 2) {
        rlcP->li[li_index++] = RLC_LI_PDU_PADDING;
        sdu_mngt->no_new_sdu_segmented_in_last_pdu = 1;
      } else if (pdu_remaining_size == 0) {
        rlcP->li_exactly_filled_to_add_in_next_pdu = 1;
      } else if (pdu_remaining_size == 1) {
        rlcP->li_one_byte_short_to_add_in_next_pdu = 1;
      }

      rlc_am_encode_pdu_15 (rlc_header, pdu_mngt, rlcP->li, li_index);
    }
    rlc_am_fill_pdu (pdu, &segmented_sdus);
  } else {
    // patch for max dat discard : avoid waiting sdu pointed by current_sdu_index <> NULL
    // (this sdu may have been segmented not at all then discarded)
    while ((rlcP->next_sdu_index != rlcP->current_sdu_index) && (rlcP->input_sdus[rlcP->current_sdu_index] == NULL)) {
      rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % rlcP->size_input_sdus_buffer;
      rlcP->nb_sdu -= 1;
    }
  }
  return pdu;
}

//-----------------------------------------------------------------------------
mem_block_t      *
rlc_am_segment_7 (struct rlc_am_entity * rlcP)
{
//-----------------------------------------------------------------------------

  list_t          segmented_sdus;       // the copying of sdu data is done after identification of all LIs to put in pdu

  struct rlc_am_tx_sdu_management *sdu_mngt = NULL;
  struct rlc_am_tx_data_pdu_management *pdu_mngt;
  struct rlc_am_pdu_header *rlc_header;
  mem_block_t      *pdu = NULL;
  int             continue_segment = 1;
  s16_t             pdu_remaining_size;
  u8_t              li_index = 0;

  list_init (&segmented_sdus, NULL);      // param string identifying the list is NULL

  while ((rlcP->input_sdus[rlcP->current_sdu_index]) && (continue_segment)) {
    sdu_mngt = ((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[rlcP->current_sdu_index]->data));
    if (!(sdu_mngt->segmented)) {

#ifdef DEBUG_RLC_AM_SEGMENT
      msg ("[RLC_AM][RB %d] SEGMENT GET NEW SDU %p INDEX %d AVAILABLE SIZE %d Bytes\n", rlcP->rb_id, sdu_mngt, rlcP->current_sdu_index, sdu_mngt->sdu_remaining_size);
#endif

      if (!(pdu)) {
        // pdu management
        if (!(pdu = get_free_mem_block  (rlcP->pdu_size + sizeof (struct rlc_am_tx_data_pdu_allocation) + GUARD_CRC_LIH_SIZE))) {
#ifdef DEBUG_MEM_MNGT
          msg ("[RLC_AM][RB %d][SEGMENT7] ERROR COULD NOT GET NEW PDU, EXIT\n", rlcP->rb_id);
#endif
          return NULL;
        }
        li_index = 0;
        pdu_remaining_size = rlcP->pdu_size - 2;        // 2= size of header, minimum
        pdu_mngt = (struct rlc_am_tx_data_pdu_management *) (pdu->data);
        memset (pdu->data, 0, sizeof (struct rlc_am_tx_data_pdu_allocation));
        pdu_mngt->rlc_tb_type = RLC_AM_DATA_PDU_TYPE;
        pdu_mngt->first_byte = &pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)];
        rlc_header = (struct rlc_am_pdu_header *) (pdu_mngt->first_byte);

        if ((rlcP->li_exactly_filled_to_add_in_next_pdu)) {
          pdu_remaining_size -= 1;
        }
#ifdef DEBUG_RLC_AM_SEGMENT
        msg ("[RLC_AM][RB %d][SEGMENT7] CONSTRUCT NEW PDU %p %p\n", rlcP->rb_id, pdu, pdu_mngt);
#endif
      }

      if (sdu_mngt->sdu_remaining_size > 0) {
	msg("sdu_mngt->sdu_remaining_size %d\n",sdu_mngt->sdu_remaining_size);
        if ((pdu_remaining_size - sdu_mngt->sdu_remaining_size) < 0) {
          pdu_mngt->data_size += pdu_remaining_size;
          rlcP->buffer_occupancy -= pdu_remaining_size;

          sdu_mngt->nb_pdus += 1;
          pdu_mngt->sdu[pdu_mngt->nb_sdu++] = rlcP->current_sdu_index;

          rlc_am_encode_pdu_7 (rlc_header, pdu_mngt, rlcP->li, li_index);
          continue_segment = 0;
          sdu_mngt->sdu_remaining_size = sdu_mngt->sdu_remaining_size - pdu_remaining_size;
          pdu_remaining_size = 0;
#ifdef DEBUG_RLC_AM_SEGMENT
          msg ("[RLC_AM][RB %d][SEGMENT7] PDU %p %p FILLED WITH SDU index %d SDU REMAINING %d\n", rlcP->rb_id, pdu, pdu_mngt, rlcP->current_sdu_index, sdu_mngt->sdu_remaining_size);
#endif
        } else {

          // From 3GPP TS 25.322 V4.2.0 (2001-09) p28
          // If "SDU discard with explicit signalling" is configured:
          // -  an AMD PDU can contain a maximum number of 15 "Length Indicators" indicating the end
          //    of 15 corresponding SDUs; and
          // -  the rest of the AMD PDU space shall be used as padding or as piggybacked STATUS PDU.

          // implementation limitation : we limit the number of li to 15 even  if
          // no "SDU discard with explicit signalling" is configured.
          if (li_index >= 14) {
            rlcP->li[li_index++] = RLC_LI_PDU_PADDING;
            rlc_am_encode_pdu_7 (rlc_header, pdu_mngt, rlcP->li, li_index);
            continue_segment = 0;
            sdu_mngt->no_new_sdu_segmented_in_last_pdu = 1;
#ifdef DEBUG_RLC_AM_SEGMENT
            msg ("[RLC_AM][RB %d][SEGMENT7] NB MAX LI REACHED\n", rlcP->rb_id);
#endif
          } else {              // nb li have not reached the limit
            if ((pdu_remaining_size - sdu_mngt->sdu_remaining_size) >= 1) {

              pdu_remaining_size = pdu_remaining_size - sdu_mngt->sdu_remaining_size - 1;       // size of li length
              pdu_mngt->data_size += sdu_mngt->sdu_remaining_size;
              sdu_mngt->li_index_for_discard = li_index;
              rlcP->li[li_index++] = sdu_mngt->sdu_remaining_size << 1;

              rlcP->buffer_occupancy -= sdu_mngt->sdu_remaining_size;
              sdu_mngt->sdu_remaining_size = 0;
              sdu_mngt->segmented = 1;

              sdu_mngt->nb_pdus += 1;
              pdu_mngt->sdu[pdu_mngt->nb_sdu++] = rlcP->current_sdu_index;
              pdu_mngt->last_pdu_of_sdu += 1;

              list_add_tail_eurecom (rlcP->input_sdus[rlcP->current_sdu_index], &segmented_sdus);

              rlcP->nb_sdu -= 1;
#ifdef DEBUG_RLC_AM_SEGMENT
              msg ("[RLC_AM][RB %d][SEGMENT7] SDU index %d->%d ENDING IN PDU %p %p PDU REMAINING SIZE %d\n", rlcP->rb_id,
                   rlcP->current_sdu_index, (rlcP->current_sdu_index + 1) % rlcP->size_input_sdus_buffer, pdu, pdu_mngt, pdu_remaining_size);
#endif
              rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % rlcP->size_input_sdus_buffer;
              // case where Li length exactly ends the pdu
              if (pdu_remaining_size == 0) {
                continue_segment = 0;
                rlc_am_encode_pdu_7 (rlc_header, pdu_mngt, rlcP->li, li_index);
              }
              /*
                 } else { // else:if (!last_sdu)
                 } // the while loop continue with the same pdu  */
            } else if ((pdu_remaining_size - sdu_mngt->sdu_remaining_size) == 0) {
              // from 3GPP TS 25.322 V4.2.0
              //In the case where the end of the last segment of an RLC SDU exactly ends at the end of
              // a PDU and there is no "Length Indicator" that indicates the end of the RLC SDU:
              // -      if 7-bit "Length Indicator" is used:
              // -      a "Length Indicator" with value "000 0000" shall be placed as the first "Length
              //       Indicator" in the following PDU;
              // -      if 15-bit "Length Indicator" is used:
              // -      a "Length Indicator" with value "000 0000 0000 0000" shall be placed as the first
              //       "Length Indicator" in the following PDU.
              pdu_mngt->data_size += pdu_remaining_size;
              rlc_am_encode_pdu_7 (rlc_header, pdu_mngt, rlcP->li, li_index);
              continue_segment = 0;
              rlcP->buffer_occupancy -= sdu_mngt->sdu_remaining_size;
              sdu_mngt->sdu_remaining_size = 0;
              pdu_remaining_size = 0;

              sdu_mngt->nb_pdus += 1;
              pdu_mngt->sdu[pdu_mngt->nb_sdu++] = rlcP->current_sdu_index;

              list_add_tail_eurecom (rlcP->input_sdus[rlcP->current_sdu_index], &segmented_sdus);

              rlcP->li_exactly_filled_to_add_in_next_pdu = 1;
#ifdef DEBUG_RLC_AM_SEGMENT
              msg ("[RLC_AM][RB %d][SEGMENT7] SDU index %d EXACTLY ENDING IN PDU %p %p\n", rlcP->rb_id, rlcP->current_sdu_index, pdu, pdu_mngt);
#endif
            }
          }
        }
        // management of previous pdu
        // In the case where 15-bit "Length Indicators" are used for the previous PDU and the
        // last segment of an RLC SDU is one octet short of exactly filling the PDU:
        // -      if a 15-bit "Length Indicator" is used for the following PDU:
        //     -  the "Length Indicator" with value "111 1111 1111 1011" shall be placed as
        //             the first "Length Indicator" in the following PDU;
        //     -  the remaining one octet in the previous PDU shall be padded by the Sender and
        //             ignored at the Receiver
        //        though there is no "Length Indicator" indicating the existence of Padding;
        //     -  in case this SDU was the last one to be transmitted:
        //         -      a RLC PDU consisting of an RLC Header with "Length Indicator"
        //             "111 1111 1111 1011" followed by a padding "Length Indicator" and padding
        //              may be transmitted;
        // -      if a 7-bit "Length Indicator" is used for the following PDU:
        //     -  if RLC is configured for UM mode:
        //         -      the "Length Indicator" with value "000 0000" shall be placed as the first
        //             "Length indicator" in the following PDU and its SN shall be incremented by 2
        //              before it is transmitted.
      } else if ((rlcP->li_exactly_filled_to_add_in_next_pdu)) {
        sdu_mngt->li_index_for_discard = li_index;
        rlcP->li[li_index++] = RLC_LI_LAST_PDU_EXACTLY_FILLED;
        rlcP->li_exactly_filled_to_add_in_next_pdu = 0;
        sdu_mngt->segmented = 1;
        //pdu_remaining_size -= 1;

        pdu_mngt->last_pdu_of_sdu += 1;
        pdu_mngt->sdu[pdu_mngt->nb_sdu++] = rlcP->current_sdu_index;
        sdu_mngt->nb_pdus += 1;

        //rlcP->buffer_occupancy      -= sdu_mngt->sdu_size;
        rlcP->nb_sdu -= 1;

#ifdef DEBUG_RLC_AM_SEGMENT
        msg ("[RLC_AM][RB %d][SEGMENT7] RLC_LI_LAST_PDU_EXACTLY_FILLED -> PDU %p, %p, sdu index %d->%d \n", rlcP->rb_id, pdu, pdu_mngt,
             rlcP->current_sdu_index, (rlcP->current_sdu_index + 1) % rlcP->size_input_sdus_buffer);
#endif
        rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % rlcP->size_input_sdus_buffer;
      }
    }
    if (sdu_mngt->sdu_remaining_size > 0) {
      list_add_tail_eurecom (rlcP->input_sdus[rlcP->current_sdu_index], &segmented_sdus);
    }
  }
  if ((pdu)) {
    if ((continue_segment > 0)) {       // means pdu not totaly filled
      if (pdu_remaining_size >= 1) {
        rlcP->li[li_index++] = RLC_LI_PDU_PADDING;
        sdu_mngt->no_new_sdu_segmented_in_last_pdu = 1;
      } else {
        rlcP->li_exactly_filled_to_add_in_next_pdu = 1;
      }
      rlc_am_encode_pdu_7 (rlc_header, pdu_mngt, rlcP->li, li_index);
    }
    rlc_am_fill_pdu (pdu, &segmented_sdus);
  } else {
    // patch for max dat discard : avoid waiting sdu pointed by current_sdu_index <> NULL
    // (this sdu may have been segmented not at all then discarded)
    while ((rlcP->next_sdu_index != rlcP->current_sdu_index) && (rlcP->input_sdus[rlcP->current_sdu_index] == NULL)) {
      rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % rlcP->size_input_sdus_buffer;
      rlcP->nb_sdu -= 1;
    }
  }
  return pdu;
}

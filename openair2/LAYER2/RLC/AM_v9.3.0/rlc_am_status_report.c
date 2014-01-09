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
#define RLC_AM_MODULE
#define RLC_AM_STATUS_REPORT_C
//-----------------------------------------------------------------------------
#include <string.h>
//-----------------------------------------------------------------------------
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "assertions.h"
#include "list.h"
#include "rlc_am.h"
#include "LAYER2/MAC/extern.h"
#include "UTIL/LOG/log.h"
#define TRACE_STATUS_CREATION

static rlc_am_control_pdu_info_t  g_rlc_am_control_pdu_info;
//-----------------------------------------------------------------------------
u16_t rlc_am_read_bit_field(u8_t** dataP, unsigned int* bit_posP, signed int bits_to_readP) {
//-----------------------------------------------------------------------------
    u16_t value = 0;
    unsigned int bits_read;
    do {
           // bits read > bits to read
        if ((8 - *bit_posP) > bits_to_readP) {
           bits_read = 8 - *bit_posP;
           value = (value << bits_to_readP) | ((((u16_t)(**dataP)) & (u16_t)(0x00FF >> *bit_posP)) >> (bits_read -
bits_to_readP));
           *bit_posP = *bit_posP + bits_to_readP;
           return value;
           // bits read == bits to read
        } else if ((8 - *bit_posP) == bits_to_readP) {
           value = (value << bits_to_readP) | (((u16_t)(**dataP)) & (u16_t)(0x00FF >> *bit_posP));
           *bit_posP = 0;
           *dataP = *dataP + 1;
           return value;
           // bits read < bits to read
        } else {
           bits_read = 8 - *bit_posP;
           value = (value << bits_read) | ((((u16_t)(**dataP)) & (u16_t)(0x00FF >> *bit_posP)));
           *bit_posP = 0;
           *dataP = *dataP + 1;
           bits_to_readP = bits_to_readP - bits_read;
        }
    } while (bits_to_readP > 0);
    return value;
}
//-----------------------------------------------------------------------------
void rlc_am_write8_bit_field(u8_t** dataP, unsigned int* bit_posP, signed int bits_to_writeP, u8_t valueP) {
//-----------------------------------------------------------------------------
    unsigned int available_bits;

    do {
        available_bits = 8 - *bit_posP;
        // available_bits > bits to write
        if (available_bits > bits_to_writeP) {
           **dataP = **dataP | (((valueP & (((u8_t)0xFF) >> (available_bits - bits_to_writeP)))) << (available_bits -
bits_to_writeP));
           *bit_posP = *bit_posP + bits_to_writeP;
           return;
           // bits read == bits to read
        } else if (available_bits == bits_to_writeP) {
           **dataP = **dataP | (valueP & (((u8_t)0xFF) >> (8 - bits_to_writeP)));
           *bit_posP = 0;
           *dataP = *dataP + 1;
           return;
           // available_bits < bits to write
        } else {
           **dataP = **dataP  | (valueP >> (bits_to_writeP - available_bits));
           *bit_posP = 0;
           *dataP = *dataP + 1;
           bits_to_writeP = bits_to_writeP - available_bits;
        }
    } while (bits_to_writeP > 0);
}
//-----------------------------------------------------------------------------
void rlc_am_write16_bit_field(u8_t** dataP, unsigned int* bit_posP, signed int bits_to_writeP, u16_t valueP) {
//-----------------------------------------------------------------------------
    assert(bits_to_writeP <= 16);

    if (bits_to_writeP > 8) {
        rlc_am_write8_bit_field(dataP,bit_posP,  bits_to_writeP - 8, (u8_t)(valueP >> 8));
        rlc_am_write8_bit_field(dataP,bit_posP,  8, (u8_t)(valueP & 0x00FF));
    } else {
        rlc_am_write8_bit_field(dataP,bit_posP,  bits_to_writeP, (u8_t)(valueP & 0x00FF));
    }
}
//-----------------------------------------------------------------------------
signed int rlc_am_get_control_pdu_infos(rlc_am_pdu_sn_10_t* headerP, s16_t *total_size_pP, rlc_am_control_pdu_info_t* pdu_infoP)
//-----------------------------------------------------------------------------
{
    memset(pdu_infoP, 0, sizeof (rlc_am_control_pdu_info_t));

    pdu_infoP->d_c = headerP->b1 >> 7;


    if (!pdu_infoP->d_c) {
        pdu_infoP->cpt    = (headerP->b1 >> 4) & 0x07;
        if (pdu_infoP->cpt != 0x00) return -3;
        pdu_infoP->ack_sn = ((headerP->b2 >> 2) & 0x3F) | (((u16_t)(headerP->b1 & 0x0F)) << 6);
        pdu_infoP->e1     = (headerP->b2 >> 1) & 0x01;
        *total_size_pP -= 1;

        if (pdu_infoP->e1) {
            unsigned int nack_to_read  = 1;
            unsigned int bit_pos       = 7; // range from 0 (MSB/left) to 7 (LSB/right)
            u8_t*        byte_pos      = &headerP->b2;

            while (nack_to_read)  {
                pdu_infoP->nack_list[pdu_infoP->num_nack].nack_sn = rlc_am_read_bit_field(&byte_pos, &bit_pos, 10);
                pdu_infoP->nack_list[pdu_infoP->num_nack].e1      = rlc_am_read_bit_field(&byte_pos, &bit_pos, 1);
                pdu_infoP->nack_list[pdu_infoP->num_nack].e2      = rlc_am_read_bit_field(&byte_pos, &bit_pos, 1);
                // READ SOstart, SOend field
                if (pdu_infoP->nack_list[pdu_infoP->num_nack].e2) {
                    pdu_infoP->nack_list[pdu_infoP->num_nack].so_start = rlc_am_read_bit_field(&byte_pos, &bit_pos, 15);
                    pdu_infoP->nack_list[pdu_infoP->num_nack].so_end   = rlc_am_read_bit_field(&byte_pos, &bit_pos, 15);
                } else {
                    pdu_infoP->nack_list[pdu_infoP->num_nack].so_start = 0;
                    // all 15 bits set to 1 (indicate that the missing portion of the AMD PDU includes all bytes
                    // to the last byte of the AMD PDU)
                    pdu_infoP->nack_list[pdu_infoP->num_nack].so_end   = 0x7FFF;
                }
                pdu_infoP->num_nack = pdu_infoP->num_nack + 1;

                if (!pdu_infoP->nack_list[pdu_infoP->num_nack - 1].e1) {
                    nack_to_read = 0;
                    *total_size_pP = *total_size_pP - (s16_t)((uint64_t)byte_pos + (uint64_t)((bit_pos + 7)/8) - (uint64_t)headerP);
                    return 0;
                }

                if (pdu_infoP->num_nack == RLC_AM_MAX_NACK_IN_STATUS_PDU) {
                    *total_size_pP = *total_size_pP - (s16_t)((uint64_t)byte_pos + (uint64_t)((bit_pos + 7)/8) - (uint64_t)headerP);
                    return -2;
                }
            }
            *total_size_pP = *total_size_pP - (s16_t)((uint64_t)byte_pos + (uint64_t)((bit_pos + 7)/8) - (uint64_t)headerP);
        } else {
            *total_size_pP = *total_size_pP - 2;
        }
        return 0;
    } else {
        return -1;
    }
}
//-----------------------------------------------------------------------------
void rlc_am_display_control_pdu_infos(rlc_am_control_pdu_info_t* pdu_infoP)
//-----------------------------------------------------------------------------
{
    int num_nack;

    if (!pdu_infoP->d_c) {
        LOG_T(RLC, "CONTROL PDU ACK SN %04d", pdu_infoP->ack_sn);

        for (num_nack = 0; num_nack < pdu_infoP->num_nack; num_nack++) {
            if (pdu_infoP->nack_list[num_nack].e2) {
                LOG_T(RLC, "\n\tNACK SN %04d SO START %05d SO END %05d",  pdu_infoP->nack_list[num_nack].nack_sn,
                                                      pdu_infoP->nack_list[num_nack].so_start,
                                                      pdu_infoP->nack_list[num_nack].so_end);
            } else {
                LOG_T(RLC, "\n\tNACK SN %04d",  pdu_infoP->nack_list[num_nack].nack_sn);
            }
        }
       LOG_T(RLC, "\n");
    } else {
        LOG_E(RLC, "CAN'T DISPLAY CONTROL INFO: PDU IS DATA PDU\n");
    }
}
//-----------------------------------------------------------------------------
void rlc_am_receive_process_control_pdu(rlc_am_entity_t* rlcP, u32_t frame, mem_block_t*  tbP, u8_t* first_byteP, s16_t *tb_size_in_bytes_pP)
//-----------------------------------------------------------------------------
{
  //rlc_am_control_pdu_info_t* pdu_info  = ((rlc_am_control_pdu_info_t*)(tbP->data));
  rlc_am_pdu_sn_10_t* rlc_am_pdu_sn_10 = (rlc_am_pdu_sn_10_t*)first_byteP;

  if (rlc_am_get_control_pdu_infos(rlc_am_pdu_sn_10, tb_size_in_bytes_pP, &g_rlc_am_control_pdu_info) >= 0) {

    rlc_am_tx_buffer_display(rlcP, frame, " TX BUFFER BEFORE PROCESS OF STATUS PDU");
    LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d] RX CONTROL PDU VT(A) %04d VT(S) %04d POLL_SN %04d ACK_SN %04d\n",
	  frame, rlcP->module_id, rlcP->rb_id, rlcP->vt_a, rlcP->vt_s, rlcP->poll_sn, g_rlc_am_control_pdu_info.ack_sn);
    rlc_am_display_control_pdu_infos(&g_rlc_am_control_pdu_info);

    u16_t        ack_sn    = g_rlc_am_control_pdu_info.ack_sn;
    u16_t        sn_cursor = rlcP->vt_a;
    u16_t        nack_sn;
    unsigned int nack_index;

    // 5.2.1 Retransmission
    //
    // The transmitting side of an AM RLC entity can receive a negative acknowledgement (notification of reception failure
    // by its peer AM RLC entity) for an AMD PDU or a portion of an AMD PDU by the following:
    //     - STATUS PDU from its peer AM RLC entity.
    //
    // When receiving a negative acknowledgement for an AMD PDU or a portion of an AMD PDU by a STATUS PDU from
    // its peer AM RLC entity, the transmitting side of the AM RLC entity shall:
    //     - if the SN of the corresponding AMD PDU falls within the range VT(A) <= SN < VT(S):
    //         - consider the AMD PDU or the portion of the AMD PDU for which a negative acknowledgement was
    //           received for retransmission.

    // 5.2.2.2    Reception of a STATUS report
    // Upon reception of a STATUS report from the receiving RLC AM entity the
    // transmitting side of an AM RLC entity shall:
    // - if the STATUS report comprises a positive or negative
    //     acknowledgement for the RLC data PDU with sequence number equal to
    //     POLL_SN:
    //     - if t-PollRetransmit is running:
    //         - stop and reset t-PollRetransmit.
    assert(ack_sn < RLC_AM_SN_MODULO);
    assert(g_rlc_am_control_pdu_info.num_nack < RLC_AM_MAX_NACK_IN_STATUS_PDU);

    if (rlc_am_in_tx_window(rlcP, ack_sn) > 0) {
      rlcP->num_nack_so = 0;
      rlcP->num_nack_sn = 0;

      if (g_rlc_am_control_pdu_info.num_nack == 0) {
          while (sn_cursor != ack_sn) {
              if (sn_cursor == rlcP->poll_sn) {
                  rlc_am_stop_and_reset_timer_poll_retransmit(rlcP,frame);
              }
              rlc_am_ack_pdu(rlcP, frame, sn_cursor);
              sn_cursor = (sn_cursor + 1)  & RLC_AM_SN_MASK;
          }
      } else {
          nack_index = 0;
          nack_sn   = g_rlc_am_control_pdu_info.nack_list[nack_index].nack_sn;
          while (sn_cursor != ack_sn) {
              if (sn_cursor == rlcP->poll_sn) {
                  rlc_am_stop_and_reset_timer_poll_retransmit(rlcP,frame);
              }
              if (sn_cursor != nack_sn) {
                  rlc_am_ack_pdu(rlcP, frame, sn_cursor);
              } else {
                  rlc_am_nack_pdu (rlcP, frame,
                     sn_cursor,
                     g_rlc_am_control_pdu_info.nack_list[nack_index].so_start,
                     g_rlc_am_control_pdu_info.nack_list[nack_index].so_end);

                  nack_index = nack_index + 1;
                  if (nack_index == g_rlc_am_control_pdu_info.num_nack) {
                      nack_sn = 0xFFFF; // value never reached by sn
                  } else {
                      nack_sn = g_rlc_am_control_pdu_info.nack_list[nack_index].nack_sn;
                  }
              }
              if ((nack_index <  g_rlc_am_control_pdu_info.num_nack) && (nack_index > 0)) {
                  if (g_rlc_am_control_pdu_info.nack_list[nack_index].nack_sn != g_rlc_am_control_pdu_info.nack_list[nack_index-1].nack_sn) {
                      sn_cursor = (sn_cursor + 1)  & RLC_AM_SN_MASK;
                  }
              } else {
                  sn_cursor = (sn_cursor + 1)  & RLC_AM_SN_MASK;
              }
          }
      }
    } else {
      LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d] WARNING CONTROL PDU ACK SN OUT OF WINDOW\n", frame, rlcP->module_id, rlcP->rb_id);
    }
  } else {
    LOG_W(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d] ERROR IN DECODING CONTROL PDU\n", frame, rlcP->module_id, rlcP->rb_id);
  }
  free_mem_block(tbP);
  rlc_am_tx_buffer_display(rlcP, frame, NULL);
}
//-----------------------------------------------------------------------------
int rlc_am_write_status_pdu(u32_t frame, rlc_am_pdu_sn_10_t* rlc_am_pdu_sn_10P, rlc_am_control_pdu_info_t* pdu_infoP)
//-----------------------------------------------------------------------------
{
  unsigned int bit_pos       = 4; // range from 0 (MSB/left) to 7 (LSB/right)
  u8_t*        byte_pos      = &rlc_am_pdu_sn_10P->b1;
  unsigned int index;
  unsigned int num_bytes;

  rlc_am_write16_bit_field(&byte_pos, &bit_pos, 10, pdu_infoP->ack_sn);
  if (pdu_infoP->num_nack > 0) {
      rlc_am_write8_bit_field(&byte_pos, &bit_pos, 1, 1);
  } else {
      rlc_am_write8_bit_field(&byte_pos, &bit_pos, 1, 0);
  }
  for (index = 0; index < pdu_infoP->num_nack ; index++) {
      rlc_am_write16_bit_field(&byte_pos, &bit_pos, 10, pdu_infoP->nack_list[index].nack_sn);
      rlc_am_write8_bit_field(&byte_pos, &bit_pos, 1,  pdu_infoP->nack_list[index].e1);
      rlc_am_write8_bit_field(&byte_pos, &bit_pos, 1,  pdu_infoP->nack_list[index].e2);
      // if SO_START SO_END fields
      if (pdu_infoP->nack_list[index].e2 > 0) {
          rlc_am_write16_bit_field(&byte_pos, &bit_pos, 15, pdu_infoP->nack_list[index].so_start);
          rlc_am_write16_bit_field(&byte_pos, &bit_pos, 15, pdu_infoP->nack_list[index].so_end);
      }
  }
  num_bytes = ((unsigned int)byte_pos) - ((unsigned int)(&rlc_am_pdu_sn_10P->b1));
  if (bit_pos > 0) {
      num_bytes += 1;
  }
#ifdef TRACE_STATUS_CREATION
  LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD XX][RB XX] WROTE STATUS PDU %d BYTES\n",
       frame, num_bytes);
#endif
  return num_bytes;
}
//-----------------------------------------------------------------------------
void rlc_am_send_status_pdu(rlc_am_entity_t* rlcP, u32_t frame)
//-----------------------------------------------------------------------------
{
  // When STATUS reporting has been triggered, the receiving side of an AM RLC entity shall:
  // - if t-StatusProhibit is not running:
  //     - at the first transmission opportunity indicated by lower layer, construct a STATUS PDU and deliver it to lower layer;
  // - else:
  //     - at the first transmission opportunity indicated by lower layer after t-StatusProhibit expires, construct a single
  //       STATUS PDU even if status reporting was triggered several times while t-StatusProhibit was running and
  //       deliver it to lower layer;
  //
  // When a STATUS PDU has been delivered to lower layer, the receiving side of an AM RLC entity shall:
  //     - start t-StatusProhibit.
  //
  // When constructing a STATUS PDU, the AM RLC entity shall:
  //     - for the AMD PDUs with SN such that VR(R) <= SN < VR(MR) that has not been completely received yet, in
  //       increasing SN of PDUs and increasing byte segment order within PDUs, starting with SN = VR(R) up to
  //       the point where the resulting STATUS PDU still fits to the total size of RLC PDU(s) indicated by lower layer:
  //         - for an AMD PDU for which no byte segments have been received yet::
  //             - include in the STATUS PDU a NACK_SN which is set to the SN of the AMD PDU;
  //         - for a continuous sequence of byte segments of a partly received AMD PDU that have not been received yet:
  //             - include in the STATUS PDU a set of NACK_SN, SOstart and SOend
  //     - set the ACK_SN to the SN of the next not received RLC Data PDU which is not indicated as missing in the
  //       resulting STATUS PDU.

  signed int                 nb_bits_to_transmit  = rlcP->nb_bytes_requested_by_mac << 3;
  rlc_am_control_pdu_info_t  control_pdu_info;
  rlc_am_pdu_info_t          *pdu_info_cursor;
  u16_t                      previous_sn_cursor = (rlcP->vr_r - 1) & RLC_AM_SN_MASK;
  u16_t                      sn_cursor;
  mem_block_t                *cursor = rlcP->receiver_buffer.head;
  int                        all_segments_received;
  int                        waited_so = 0;
  mem_block_t                *tb;
  int                        pdu_size;

  memset(&control_pdu_info, 0, sizeof(rlc_am_control_pdu_info_t));
  // header size
  nb_bits_to_transmit = nb_bits_to_transmit - 15;
#ifdef TRACE_STATUS_CREATION
  LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEND-STATUS] nb_bits_to_transmit %d (15 already allocated for header)\n",
       frame,rlcP->module_id, rlcP->rb_id, nb_bits_to_transmit);
  rlc_am_rx_list_display(rlcP, " DISPLAY BEFORE CONSTRUCTION OF STATUS REPORT");
#endif

  if (cursor != NULL) {
      pdu_info_cursor       = &((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info;
      sn_cursor             = pdu_info_cursor->sn;

      while (rlc_am_in_rx_window(rlcP, sn_cursor) == 0) {
          cursor                = cursor->next;
          previous_sn_cursor    = sn_cursor;

          pdu_info_cursor       = &((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info;
          sn_cursor             = pdu_info_cursor->sn;
#ifdef TRACE_STATUS_CREATION
          LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEND-STATUS] LINE %d FIND VR(R) <= SN sn_cursor %04d -> %04d\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, previous_sn_cursor, sn_cursor);
#endif
      }

      // 12 bits = size of NACK_SN field + E1, E2 bits
      // 42 bits = size of NACK_SN field + SO_START, SO_END fields, E1, E2 bits
      while ((cursor != NULL) && (nb_bits_to_transmit >= 12)){
          pdu_info_cursor       = &((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info;
          all_segments_received = ((rlc_am_rx_pdu_management_t*)(cursor->data))->all_segments_received;
          sn_cursor             = pdu_info_cursor->sn;
#ifdef TRACE_STATUS_CREATION
          LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEND-STATUS] LINE %d LOOPING sn_cursor %04d previous sn_cursor %04d \n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, sn_cursor, previous_sn_cursor);
#endif

          // -------------------------------------------------------------------------------
          // case resegmentation : several PDUs related to the same SN queued in list
          // -------------------------------------------------------------------------------
          if (sn_cursor == previous_sn_cursor) {
              do {
                  cursor = cursor->next;
                  if (cursor != NULL) {
                      pdu_info_cursor       = &((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info;
                      all_segments_received = ((rlc_am_rx_pdu_management_t*)(cursor->data))->all_segments_received;
                      sn_cursor             = pdu_info_cursor->sn;
#ifdef TRACE_STATUS_CREATION
                      LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEND-STATUS] LINE %d NOW sn_cursor %04d \n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, sn_cursor);
#endif
                  } else {
                    if (all_segments_received) {
                        control_pdu_info.ack_sn = (sn_cursor + 1) & RLC_AM_SN_MASK;
#ifdef TRACE_STATUS_CREATION
                        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEND-STATUS] LINE %d PREPARE SENDING ACK SN %04d \n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, control_pdu_info.ack_sn);
#endif
                    } else {
                        control_pdu_info.ack_sn = (previous_sn_cursor + 1) & RLC_AM_SN_MASK;
#ifdef TRACE_STATUS_CREATION
                        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEND-STATUS] LINE %d PREPARE SENDING ACK SN %04d (CASE PREVIOUS SN)\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, control_pdu_info.ack_sn);
#endif
                    }
                    goto end_push_nack;
                  }
              } while ((cursor != NULL) && (sn_cursor == previous_sn_cursor));
          }
          // -------------------------------------------------------------------------------
          // simple case, PDU(s) is/are missing
          // -------------------------------------------------------------------------------
          while (((previous_sn_cursor + 1) & RLC_AM_SN_MASK) != sn_cursor) {
              if (nb_bits_to_transmit > 12) {
              previous_sn_cursor = (previous_sn_cursor + 1) & RLC_AM_SN_MASK;
              control_pdu_info.nack_list[control_pdu_info.num_nack].nack_sn   = previous_sn_cursor;
              control_pdu_info.nack_list[control_pdu_info.num_nack].so_start  = 0;
              control_pdu_info.nack_list[control_pdu_info.num_nack].so_end    = 0x7ff;
              control_pdu_info.nack_list[control_pdu_info.num_nack].e1        = 1;
              control_pdu_info.nack_list[control_pdu_info.num_nack].e2        = 0;
              control_pdu_info.num_nack += 1;
              nb_bits_to_transmit = nb_bits_to_transmit - 12;
#ifdef TRACE_STATUS_CREATION
              LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEND-STATUS] LINE %d PREPARE SENDING NACK %04d\n",
                       frame, rlcP->module_id, rlcP->rb_id, __LINE__, previous_sn_cursor);
#endif
              } else {
                  control_pdu_info.ack_sn = (previous_sn_cursor + 1) & RLC_AM_SN_MASK;
#ifdef TRACE_STATUS_CREATION
                  LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEND-STATUS] LINE %d NO MORE BITS FOR SENDING NACK %04d -> ABORT AND SET FINAL ACK %04d\n",
                       frame, rlcP->module_id, rlcP->rb_id, __LINE__, previous_sn_cursor, control_pdu_info.ack_sn);
#endif
                  goto end_push_nack;
              }
          }
          // -------------------------------------------------------------------------------
          // not so simple case, a resegmented PDU(s) is missing
          // -------------------------------------------------------------------------------
          if (all_segments_received == 0) {
              waited_so = 0;
#ifdef TRACE_STATUS_CREATION
              LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEND-STATUS] if (all_segments_received == 0) \n", frame, rlcP->module_id, rlcP->rb_id);
#endif
              do {
                  if (pdu_info_cursor->so > waited_so) {
                      if (nb_bits_to_transmit >= 42) {
                          control_pdu_info.nack_list[control_pdu_info.num_nack].nack_sn   = sn_cursor;
                          control_pdu_info.nack_list[control_pdu_info.num_nack].so_start  = waited_so;
                          control_pdu_info.nack_list[control_pdu_info.num_nack].so_end    = pdu_info_cursor->so - 1;
                          control_pdu_info.nack_list[control_pdu_info.num_nack].e1        = 1;
                          control_pdu_info.nack_list[control_pdu_info.num_nack].e2        = 1;
                          control_pdu_info.num_nack += 1;
                          nb_bits_to_transmit = nb_bits_to_transmit - 42;
#ifdef TRACE_STATUS_CREATION
                          LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEND-STATUS] LINE %d PREPARE SENDING NACK %04d SO START %05d SO END %05d (CASE SO %d > WAITED SO %d)\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, sn_cursor, waited_so, pdu_info_cursor->so - 1, pdu_info_cursor->so, waited_so);
#endif
                          if (pdu_info_cursor->lsf == 1) { // last segment flag
                              //waited_so = 0x7FF;
                              waited_so = 0x7FFF;
#ifdef TRACE_STATUS_CREATION
                              LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEND-STATUS] LINE %d SN %04d SET WAITED SO 0x7FFF)\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, sn_cursor);
#endif
                              //break;
                          } else {
                              waited_so = pdu_info_cursor->so + pdu_info_cursor->payload_size;
#ifdef TRACE_STATUS_CREATION
                              LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEND-STATUS] LINE %d SN %04d SET WAITED SO %d @1\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, sn_cursor, waited_so);
#endif
                          }
                      } else {
                          control_pdu_info.ack_sn = sn_cursor;
                          goto end_push_nack;
                      }
                  } else { //else { // pdu_info_cursor->so <= waited_so
                      waited_so = pdu_info_cursor->so + pdu_info_cursor->payload_size;
                      if (pdu_info_cursor->lsf == 1) { // last segment flag
                          //waited_so = 0x7FF;
                          waited_so = 0x7FFF;
                      }

#ifdef TRACE_STATUS_CREATION
                      LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEND-STATUS] LINE %d SN %04d SET WAITED SO %d @2\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, sn_cursor, waited_so);
#endif
                  }

                  cursor = cursor->next;
                  if (cursor != NULL) {
                      pdu_info_cursor       = &((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info;
                      all_segments_received = ((rlc_am_rx_pdu_management_t*)(cursor->data))->all_segments_received;
                      previous_sn_cursor    = sn_cursor;
                      sn_cursor             = pdu_info_cursor->sn;
                  } else {
                    // LG control_pdu_info.ack_sn = (previous_sn_cursor + 1) & RLC_AM_SN_MASK;
                    control_pdu_info.ack_sn = previous_sn_cursor;
                    goto end_push_nack;
                  }
              } while ((cursor != NULL) && (sn_cursor == previous_sn_cursor));

              // may be last segment of PDU not received
              //if ((sn_cursor != previous_sn_cursor) && (waited_so != 0x7FF)) {
              if ((sn_cursor != previous_sn_cursor) && (waited_so != 0x7FFF)) {
                  if (nb_bits_to_transmit >= 42) {
                      control_pdu_info.nack_list[control_pdu_info.num_nack].nack_sn   = previous_sn_cursor;
                      control_pdu_info.nack_list[control_pdu_info.num_nack].so_start  = waited_so;
                      //control_pdu_info.nack_list[control_pdu_info.num_nack].so_end    = 0x7FF;
                      control_pdu_info.nack_list[control_pdu_info.num_nack].so_end    = 0x7FFF;
                      control_pdu_info.nack_list[control_pdu_info.num_nack].e1        = 1;
                      control_pdu_info.nack_list[control_pdu_info.num_nack].e2        = 1;
                      control_pdu_info.num_nack += 1;
                      nb_bits_to_transmit = nb_bits_to_transmit - 42;
#ifdef TRACE_STATUS_CREATION
                      LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEND-STATUS] LINE %d PREPARE SENDING NACK %04d SO START %05d SO END %05d\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, previous_sn_cursor, waited_so, 0x7FFF);
#endif
                  } else {
                      control_pdu_info.ack_sn = previous_sn_cursor;
                      goto end_push_nack;
                  }
              }
              waited_so = 0;
          } else {
              waited_so = 0;
              cursor = cursor->next;
              previous_sn_cursor = sn_cursor;
          }
      }
      control_pdu_info.ack_sn = (previous_sn_cursor + 1) & RLC_AM_SN_MASK;
  } else {
      control_pdu_info.ack_sn = rlcP->vr_r;
#ifdef TRACE_STATUS_CREATION
      LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEND-STATUS] LINE %d PREPARE SENDING ACK %04d  = VR(R)\n",
        frame, rlcP->module_id, rlcP->rb_id, __LINE__, control_pdu_info.ack_sn);
#endif
  }

end_push_nack:
  if (control_pdu_info.num_nack > 0) {
      control_pdu_info.nack_list[control_pdu_info.num_nack - 1].e1  = 0;
  }
  //msg ("[FRAME %05d][RLC_AM][MOD %02d][RB %02d] nb_bits_to_transmit %d\n",
  //     rlcP->module_id, rlcP->rb_id, frame,nb_bits_to_transmit);

#ifdef TRACE_STATUS_CREATION
  LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEND-STATUS] LINE %d PREPARE SENDING ACK %04d NUM NACK %d\n",
        frame, rlcP->module_id, rlcP->rb_id, __LINE__, control_pdu_info.ack_sn, control_pdu_info.num_nack);
#endif
  // encode the control pdu
  pdu_size = rlcP->nb_bytes_requested_by_mac - ((nb_bits_to_transmit - 7 )>> 3);

#ifdef TRACE_STATUS_CREATION
  LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEND-STATUS] LINE %d forecast pdu_size %d\n",
       frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_size);
#endif
       tb = get_free_mem_block(sizeof(struct mac_tb_req) + pdu_size);
  memset(tb->data, 0, sizeof(struct mac_tb_req) + pdu_size);
  //estimation only ((struct mac_tb_req*)(tb->data))->tb_size_in_bits  = pdu_size << 3;
  ((struct mac_tb_req*)(tb->data))->data_ptr         = (u8_t*)&(tb->data[sizeof(struct mac_tb_req)]);

  // warning reuse of pdu_size
  pdu_size = rlc_am_write_status_pdu(frame,(rlc_am_pdu_sn_10_t*)(((struct mac_tb_req*)(tb->data))->data_ptr), &control_pdu_info);
  ((struct mac_tb_req*)(tb->data))->tb_size_in_bits  = pdu_size << 3;
  //assert((((struct mac_tb_req*)(tb->data))->tb_size_in_bits >> 3) < 3000);

#ifdef TRACE_STATUS_CREATION
  LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEND-STATUS] SEND STATUS PDU SIZE %d, rlcP->nb_bytes_requested_by_mac %d, nb_bits_to_transmit>>3 %d\n", frame, rlcP->module_id, rlcP->rb_id, pdu_size, rlcP->nb_bytes_requested_by_mac, nb_bits_to_transmit >> 3);
#endif
  assert(pdu_size == (rlcP->nb_bytes_requested_by_mac - (nb_bits_to_transmit >> 3)));

  // remaining bytes to transmit for RLC (retrans pdus and new data pdus)
  rlcP->nb_bytes_requested_by_mac = rlcP->nb_bytes_requested_by_mac - pdu_size;
  // put pdu in trans
  list_add_head(tb, &rlcP->control_pdu_list);
  rlcP->stat_tx_control_pdu   += 1;
  rlcP->stat_tx_control_bytes += pdu_size;

}

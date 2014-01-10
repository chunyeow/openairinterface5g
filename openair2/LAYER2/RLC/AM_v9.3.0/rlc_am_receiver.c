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
#define RLC_AM_RECEIVER_C
//#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "assertions.h"
#include "rlc.h"
#include "rlc_am.h"
#include "list.h"
#include "LAYER2/MAC/extern.h"
#include "UTIL/LOG/log.h"

#define TRACE_RLC_AM_RX
//#define DEBUG_RLC_AM_DISPLAY_TB_DATA
//#define RLC_AM_GENERATE_ERRORS
#define DEBUG_DISPLAY_NVIDIA
#define RLC_ENABLE_PDU_CONCATENATION
//-----------------------------------------------------------------------------
signed int rlc_am_get_data_pdu_infos(u32_t frame, rlc_am_pdu_sn_10_t* headerP, s16_t total_sizeP, rlc_am_pdu_info_t* pdu_infoP)
//-----------------------------------------------------------------------------
{
    memset(pdu_infoP, 0, sizeof (rlc_am_pdu_info_t));

    s16_t          sum_li = 0;
    pdu_infoP->d_c = headerP->b1 >> 7;
    pdu_infoP->num_li = 0;


    if (pdu_infoP->d_c) {
        pdu_infoP->rf  = (headerP->b1 >> 6) & 0x01;
        pdu_infoP->p   = (headerP->b1 >> 5) & 0x01;
        pdu_infoP->fi  = (headerP->b1 >> 3) & 0x03;
        pdu_infoP->e   = (headerP->b1 >> 2) & 0x01;
        pdu_infoP->sn  = headerP->b2 +  (((u16_t)(headerP->b1 & 0x03)) << 8);

        pdu_infoP->header_size  = 2;
        if (pdu_infoP->rf) {
            pdu_infoP->lsf = (headerP->data[0] >> 7) & 0x01;
            pdu_infoP->so  = headerP->data[1] +  (((u16_t)(headerP->data[0] & 0x7F)) << 8);
            pdu_infoP->payload = &headerP->data[2];
            pdu_infoP->header_size  += 2;
        } else {
            pdu_infoP->payload = &headerP->data[0];
        }

        if (pdu_infoP->e) {
            rlc_am_e_li_t      *e_li;
            unsigned int li_length_in_bytes  = 1;
            unsigned int li_to_read          = 1;

            if (pdu_infoP->rf) {
                e_li = (rlc_am_e_li_t*)(&headerP->data[2]);
            } else {
                e_li = (rlc_am_e_li_t*)(headerP->data);
            }
            while (li_to_read)  {
                li_length_in_bytes = li_length_in_bytes ^ 3;
                if (li_length_in_bytes  == 2) {
                    pdu_infoP->li_list[pdu_infoP->num_li] = ((u16_t)(e_li->b1 << 4)) & 0x07F0;
                    pdu_infoP->li_list[pdu_infoP->num_li] |= (((u8_t)(e_li->b2 >> 4)) & 0x000F);
                    li_to_read = e_li->b1 & 0x80;
                    pdu_infoP->header_size  += 2;
                } else {
                    pdu_infoP->li_list[pdu_infoP->num_li] = ((u16_t)(e_li->b2 << 8)) & 0x0700;
                    pdu_infoP->li_list[pdu_infoP->num_li] |=  e_li->b3;
                    li_to_read = e_li->b2 & 0x08;
                    e_li++;
                    pdu_infoP->header_size  += 1;
                }
                sum_li += pdu_infoP->li_list[pdu_infoP->num_li];
                pdu_infoP->num_li = pdu_infoP->num_li + 1;
                if (pdu_infoP->num_li > RLC_AM_MAX_SDU_IN_PDU) {
                    LOG_E(RLC, "[FRAME %05d][RLC_AM][MOD XX][RB XX][GET PDU INFO]  SN %04d TOO MANY LIs ", frame, pdu_infoP->sn);
                    return -2;
                }
            }
            if (li_length_in_bytes  == 2) {
                pdu_infoP->payload = &e_li->b3;
            } else {
                pdu_infoP->payload = &e_li->b1;
            }
        }
        pdu_infoP->payload_size = total_sizeP - pdu_infoP->header_size;
        if (pdu_infoP->payload_size > sum_li) {
            pdu_infoP->hidden_size = pdu_infoP->payload_size - sum_li;
        }
        return 0;
    } else {
        LOG_W(RLC, "[FRAME %05d][RLC_AM][MOD XX][RB XX][GET DATA PDU INFO]  SN %04d ERROR CONTROL PDU ", frame,  pdu_infoP->sn);
        return -1;
    }
}
//-----------------------------------------------------------------------------
void rlc_am_display_data_pdu_infos(rlc_am_entity_t *rlcP, u32_t frame, rlc_am_pdu_info_t* pdu_infoP)
//-----------------------------------------------------------------------------
{
    int num_li;

    if (pdu_infoP->d_c) {
        if (pdu_infoP->rf) {
            LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][DISPLAY DATA PDU] RX DATA PDU SN %04d FI %1d SO %05d LSF %01d POLL %1d ", frame, rlcP->module_id, rlcP->rb_id, pdu_infoP->sn, pdu_infoP->fi, pdu_infoP->so, pdu_infoP->lsf, pdu_infoP->p);
        } else {
            LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][DISPLAY DATA PDU] RX DATA PDU SN %04d FI %1d POLL %1d ", frame, rlcP->module_id, rlcP->rb_id, pdu_infoP->sn, pdu_infoP->fi, pdu_infoP->p);
        }
        for (num_li = 0; num_li < pdu_infoP->num_li; num_li++) {
            LOG_D(RLC, "LI %05d ",  pdu_infoP->li_list[num_li]);
        }
        if (pdu_infoP->hidden_size > 0) {
            LOG_D(RLC, "hidden size %05d ",  pdu_infoP->hidden_size);
        }
        LOG_D(RLC, "\n");
    } else {
        LOG_E(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][DISPLAY DATA PDU] ERROR RX CONTROL PDU\n", frame, rlcP->module_id, rlcP->rb_id);
    }
}
// assumed the sn of the tb is equal to VR(MS)
//-----------------------------------------------------------------------------
void rlc_am_rx_update_vr_ms(rlc_am_entity_t *rlcP, u32_t frame, mem_block_t* tbP)
//-----------------------------------------------------------------------------
{
    //rlc_am_pdu_info_t* pdu_info        = &((rlc_am_rx_pdu_management_t*)(tbP->data))->pdu_info;
    rlc_am_pdu_info_t* pdu_info_cursor;
    mem_block_t*       cursor;

    cursor = tbP;
    if (cursor) {
        do {
            pdu_info_cursor = &((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info;
            if (((rlc_am_rx_pdu_management_t*)(cursor->data))->all_segments_received == 0) {
#ifdef TRACE_RLC_AM_RX
               LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][UPDATE VR(MS)] UPDATED VR(MS) %04d -> %04d\n", frame, rlcP->module_id, rlcP->rb_id, rlcP->vr_ms, pdu_info_cursor->sn);
#endif
                rlcP->vr_ms = pdu_info_cursor->sn;
                return;
            }
            cursor = cursor->next;
        } while (cursor != NULL);
#ifdef TRACE_RLC_AM_RX
        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][UPDATE VR(MS)] UPDATED VR(MS) %04d -> %04d\n", frame, rlcP->module_id, rlcP->rb_id, rlcP->vr_ms, (pdu_info_cursor->sn + 1)  & RLC_AM_SN_MASK);
#endif
        rlcP->vr_ms = (pdu_info_cursor->sn + 1)  & RLC_AM_SN_MASK;
    }
}
// assumed the sn of the tb is equal to VR(R)
//-----------------------------------------------------------------------------
void rlc_am_rx_update_vr_r(rlc_am_entity_t *rlcP,u32_t frame,mem_block_t* tbP)
//-----------------------------------------------------------------------------
{
    rlc_am_pdu_info_t* pdu_info_cursor;
    mem_block_t*       cursor;
    cursor = tbP;
    if (cursor) {
        do {
            pdu_info_cursor = &((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info;
            if ((((rlc_am_rx_pdu_management_t*)(cursor->data))->all_segments_received == 0) ||
               (rlcP->vr_r != pdu_info_cursor->sn)) {
                return;
            }
#ifdef TRACE_RLC_AM_RX
            LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][UPDATE VR(R)] UPDATED VR(R) %04d -> %04d\n", frame, rlcP->module_id, rlcP->rb_id, rlcP->vr_r, (pdu_info_cursor->sn + 1) & RLC_AM_SN_MASK);
#endif
            if (((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info.rf == 1) {
                if (((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info.lsf == 1) {
                    rlcP->vr_r = (rlcP->vr_r + 1) & RLC_AM_SN_MASK;
                }
            } else  {
                rlcP->vr_r = (rlcP->vr_r + 1) & RLC_AM_SN_MASK;
            }
            cursor = cursor->next;
        } while (cursor != NULL);
        //rlcP->vr_r = (pdu_info_cursor->sn + 1) & RLC_AM_SN_MASK;
    }
}
//-----------------------------------------------------------------------------
void
rlc_am_receive_routing (rlc_am_entity_t *rlcP, u32_t frame, u8_t eNB_flag, struct mac_data_ind data_indP)
//-----------------------------------------------------------------------------
{
    mem_block_t        *tb;
    u8_t               *first_byte;
    s16_t               tb_size_in_bytes;

    while ((tb = list_remove_head (&data_indP.data))) {
        first_byte = ((struct mac_tb_ind *) (tb->data))->data_ptr;
        tb_size_in_bytes = ((struct mac_tb_ind *) (tb->data))->size;

        if (tb_size_in_bytes > 0) {
            if ((*first_byte & 0x80) == 0x80) {
                rlcP->stat_rx_data_bytes += tb_size_in_bytes;
                rlcP->stat_rx_data_pdu   += 1;
                rlc_am_receive_process_data_pdu (rlcP, frame, eNB_flag, tb, first_byte, tb_size_in_bytes);
            } else {
                rlcP->stat_rx_control_bytes += tb_size_in_bytes;
                rlcP->stat_rx_control_pdu += 1;
                rlc_am_receive_process_control_pdu (rlcP, frame, tb, &first_byte, &tb_size_in_bytes);
                // if data pdu concatenated with control PDU (seen with real hardware LTE dongle integration)
                if (tb_size_in_bytes > 0) {
#if defined(RLC_ENABLE_PDU_CONCATENATION)
                    if ((*first_byte & 0x80) == 0x80) {
                        rlcP->stat_rx_data_bytes += tb_size_in_bytes;
                        rlcP->stat_rx_data_pdu   += 1;
                        rlc_am_receive_process_data_pdu (rlcP, frame, eNB_flag, tb, first_byte, tb_size_in_bytes);
                    } else {
                        AssertFatal( tb_size_in_bytes == 0,
                                            "Not a data PDU concatened to control PDU %ld bytes left",
                                            tb_size_in_bytes);
                    }
#else
                    AssertFatal( tb_size_in_bytes == 0,
                                        "Remaining %d bytes following a control PDU",
                                        tb_size_in_bytes);
#endif
                }
            }
            LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RX ROUTING] VR(R)=%03d VR(MR)=%03d\n", frame, rlcP->module_id, rlcP->rb_id, rlcP->vr_r, rlcP->vr_mr);
        }
    } // end while
}
//-----------------------------------------------------------------------------
void rlc_am_receive_process_data_pdu (rlc_am_entity_t *rlcP, u32_t frame, u8_t eNB_flag, mem_block_t* tbP, u8_t* first_byteP, u16_t tb_size_in_bytesP)
//-----------------------------------------------------------------------------
{
  // 5.1.3.2 Receive operations
  // 5.1.3.2.1 General
  // The receiving side of an AM RLC entity shall maintain a receiving window according to state variables VR(R) and
  // VR(MR) as follows:
  //     - a SN falls within the receiving window if VR(R) <= SN < VR(MR);
  //     - a SN falls outside of the receiving window otherwise.
  //
  // When receiving a RLC data PDU from lower layer, the receiving side of an AM RLC entity shall:
  // - either discard the received RLC data PDU or place it in the reception buffer (see sub clause 5.1.3.2.2);
  // - if the received RLC data PDU was placed in the reception buffer:
  //     - update state variables, reassemble and deliver RLC SDUs to upper layer and start/stop t-Reordering as
  //       needed (see sub clause 5.1.3.2.3).
  // When t-Reordering expires, the receiving side of an AM RLC entity shall:
  //     - update state variables and start t-Reordering as needed (see sub clause 5.1.3.2.4).


  // 5.1.3.2.2 Actions when a RLC data PDU is received from lower layer
  // When a RLC data PDU is received from lower layer, where the RLC data PDU contains byte segment numbers y to z of
  // an AMD PDU with SN = x, the receiving side of an AM RLC entity shall:
  //     - if x falls outside of the receiving window; or
  //     - if byte segment numbers y to z of the AMD PDU with SN = x have been received before:
  //         - discard the received RLC data PDU;
  //     - else:
  //         - place the received RLC data PDU in the reception buffer;
  //         - if some byte segments of the AMD PDU contained in the RLC data PDU have been received before:
  //             - discard the duplicate byte segments.
  rlc_am_pdu_info_t* pdu_info = &((rlc_am_rx_pdu_management_t*)(tbP->data))->pdu_info;
  rlc_am_pdu_sn_10_t* rlc_am_pdu_sn_10 = (rlc_am_pdu_sn_10_t*)first_byteP;

  if (rlc_am_get_data_pdu_infos(frame,rlc_am_pdu_sn_10, tb_size_in_bytesP, pdu_info) >= 0) {


#ifdef TRACE_RLC_AM_RX
      rlc_am_display_data_pdu_infos(rlcP, frame, pdu_info);
#endif
      ((rlc_am_rx_pdu_management_t*)(tbP->data))->all_segments_received = 0;
      if (rlc_am_in_rx_window(rlcP, pdu_info->sn)) {

          if (pdu_info->p) {
              LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][PROCESS RX PDU]  POLL BIT SET, STATUS REQUESTED:\n", frame, rlcP->module_id, rlcP->rb_id);
              rlcP->status_requested = 1;
          }
          LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][PROCESS RX PDU] VR(R) %04d VR(H) %04d VR(MR) %04d VR(MS) %04d VR(X) %04d\n", frame, rlcP->module_id, rlcP->rb_id, rlcP->vr_r, rlcP->vr_h, rlcP->vr_mr, rlcP->vr_ms, rlcP->vr_x);

	      if (rlc_am_rx_list_insert_pdu(rlcP, frame,tbP) < 0) {
	    	  rlcP->stat_rx_data_pdu_dropped     += 1;
	    	  rlcP->stat_rx_data_bytes_dropped   += tb_size_in_bytesP;
		      free_mem_block (tbP);
		      LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][PROCESS RX PDU]  PDU DISCARDED, STATUS REQUESTED:\n", frame, rlcP->module_id, rlcP->rb_id);
              rlcP->status_requested = 1;
          } else {
            // 5.1.3.2.3
            // Actions when a RLC data PDU is placed in the reception buffer
            //
            // When a RLC data PDU with SN = x is placed in the reception buffer, the receiving side of an AM RLC entity shall:
            //     - if x >= VR(H)
            //         - update VR(H) to x+ 1;
            //
            //     - if all byte segments of the AMD PDU with SN = VR(MS) are received:
            //         - update VR(MS) to the SN of the first AMD PDU with SN > current VR(MS) for which not all byte segments
            //           have been received;
            //
            //     - if x = VR(R):
            //         - if all byte segments of the AMD PDU with SN = VR(R) are received:
            //             - update VR(R) to the SN of the first AMD PDU with SN > current VR(R) for which not all byte segments
            //               have been received;
            //             - update VR(MR) to the updated VR(R) + AM_Window_Size;
            //
            //         - reassemble RLC SDUs from any byte segments of AMD PDUs with SN that falls outside of the receiving
            //           window and in-sequence byte segments of the AMD PDU with SN = VR(R), remove RLC headers when
            //           doing so and deliver the reassembled RLC SDUs to upper layer in sequence if not delivered before;
            //
            //     - if t-Reordering is running:
            //         - if VR(X) = VR(R); or
            //         - if VR(X) falls outside of the receiving window and VR(X) is not equal to VR(MR):
            //             - stop and reset t-Reordering;
            //
            //     - if t-Reordering is not running (includes the case t-Reordering is stopped due to actions above):
            //         - if VR (H) > VR(R):
            //             - start t-Reordering;
            //             - set VR(X) to VR(H).


#ifdef TRACE_RLC_AM_RX
              LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][PROCESS RX PDU]  RX LIST AFTER INSERTION:\n", frame, rlcP->module_id, rlcP->rb_id);
              rlc_am_rx_list_display(rlcP, "rlc_am_receive_process_data_pdu AFTER INSERTION ");
#endif
              if (rlc_am_sn_gte_vr_h(rlcP, pdu_info->sn) > 0) {
                  rlcP->vr_h = (pdu_info->sn + 1) & RLC_AM_SN_MASK;
              }
              rlc_am_rx_check_all_byte_segments(rlcP, frame, tbP);
              if ((pdu_info->sn == rlcP->vr_ms) && (((rlc_am_rx_pdu_management_t*)(tbP->data))->all_segments_received)) {
                  rlc_am_rx_update_vr_ms(rlcP, frame, tbP);
              }
              if (pdu_info->sn == rlcP->vr_r) {
                 if (((rlc_am_rx_pdu_management_t*)(tbP->data))->all_segments_received) {
                      rlc_am_rx_update_vr_r(rlcP, frame, tbP);
                      rlcP->vr_mr = (rlcP->vr_r + RLC_AM_WINDOW_SIZE) & RLC_AM_SN_MASK;
                  }
                  rlc_am_rx_list_reassemble_rlc_sdus(rlcP,frame,eNB_flag);
              }
              if (rlcP->t_reordering.running) {
                  if ((rlcP->vr_x == rlcP->vr_r) || ((rlc_am_in_rx_window(rlcP, pdu_info->sn) == 0) && (rlcP->vr_x != rlcP->vr_mr))) {
                      rlc_am_stop_and_reset_timer_reordering(rlcP,frame);
                  }
              }
              if (!(rlcP->t_reordering.running)) {
                  if (rlcP->vr_h != rlcP->vr_r) { // - if VR (H) > VR(R) translated to - if VR (H) != VR(R)
                      rlc_am_start_timer_reordering(rlcP,frame);
                      rlcP->vr_x = rlcP->vr_h;
                  }
              }
          }
          LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][PROCESS RX PDU] VR(R) %04d VR(H) %04d  VR(MS) %04d  VR(MR) %04d\n", frame, rlcP->module_id, rlcP->rb_id, rlcP->vr_r, rlcP->vr_h, rlcP->vr_ms, rlcP->vr_mr);
      } else {
    	  rlcP->stat_rx_data_pdu_out_of_window     += 1;
    	  rlcP->stat_rx_data_bytes_out_of_window   += tb_size_in_bytesP;
          free_mem_block (tbP);
          LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][PROCESS RX PDU]  PDU OUT OF RX WINDOW, DISCARDED, STATUS REQUESTED:\n", frame, rlcP->module_id, rlcP->rb_id);
          rlcP->status_requested = 1;
      }
  } else {
      free_mem_block (tbP);
  }
}

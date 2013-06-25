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
#define RLC_AM_RETRANSMIT_C
//-----------------------------------------------------------------------------
//#include "rtos_header.h"
//-----------------------------------------------------------------------------
#include "rlc_am.h"
#include "rlc.h"
#include "LAYER2/MAC/extern.h"
#include "UTIL/LOG/log.h"
//#define TRACE_RLC_AM_RESEGMENT
//#define TRACE_RLC_AM_FORCE_TRAFFIC
//#define TRACE_RLC_AM_NACK
//#define TRACE_RLC_AM_ACK
//-----------------------------------------------------------------------------
void rlc_am_nack_pdu (rlc_am_entity_t *rlcP, u32_t frame, u16_t snP, u16_t so_startP, u16_t so_endP)
//-----------------------------------------------------------------------------
{
    // 5.2.1 Retransmission
    // ...
    // When an AMD PDU or a portion of an AMD PDU is considered for retransmission, the transmitting side of the AM
    // RLC entity shall:
    //     - if the AMD PDU is considered for retransmission for the first time:
    //         - set the RETX_COUNT associated with the AMD PDU to zero;
    //     - else, if it (the AMD PDU or the portion of the AMD PDU that is considered for retransmission) is not pending
    //            for retransmission already, or a portion of it is not pending for retransmission already:
    //         - increment the RETX_COUNT;
    //     - if RETX_COUNT = maxRetxThreshold:
    //         - indicate to upper layers that max retransmission has been reached.


    mem_block_t* mb         = rlcP->pdu_retrans_buffer[snP].mem_block;
    int          pdu_sdu_index;
    int          sdu_index;

    if (mb != NULL) {
        rlcP->num_nack_sn += 1;
        assert(so_startP <= so_endP);
        //-----------------------------------------
        // allow holes in reports
        // it is assumed that hole reports are done in byte offset
        // increasing order among calls refering to only one status PDU
        //  and among time
        //-----------------------------------------
        if (rlcP->pdu_retrans_buffer[snP].last_nack_time != frame) {
            rlcP->pdu_retrans_buffer[snP].last_nack_time = frame;
            rlc_am_clear_holes(rlcP, snP);
        }
        if (!((so_startP == 0) && (so_endP == 0x7FFF))) {
            rlcP->num_nack_so += 1;
        }
        rlc_am_add_hole(rlcP, frame, snP, so_startP, so_endP);

        if (rlcP->first_retrans_pdu_sn < 0) {
            rlcP->first_retrans_pdu_sn = snP;
        } else if (rlc_am_tx_sn1_gt_sn2(rlcP, rlcP->first_retrans_pdu_sn, snP)){
            rlcP->first_retrans_pdu_sn = snP;
        }
        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][NACK-PDU] NACK PDU SN %04d previous retx_count %d  1ST_RETRANS_PDU %04d\n",
        		frame,
        		rlcP->module_id,
        		rlcP->rb_id,
        		snP,
        		rlcP->pdu_retrans_buffer[snP].retx_count,
        		rlcP->first_retrans_pdu_sn);

        rlcP->pdu_retrans_buffer[snP].flags.retransmit = 1;

        if (rlcP->pdu_retrans_buffer[snP].retx_count == -1) {
            rlcP->pdu_retrans_buffer[snP].retx_count = 0;
            rlcP->retrans_num_bytes_to_retransmit += rlcP->pdu_retrans_buffer[snP].header_and_payload_size;
        } else {
            rlcP->pdu_retrans_buffer[snP].retx_count += 1;
        }
        if (rlcP->pdu_retrans_buffer[snP].retx_count >= rlcP->max_retx_threshold) {
            for (pdu_sdu_index = 0; pdu_sdu_index < rlcP->pdu_retrans_buffer[snP].nb_sdus; pdu_sdu_index++) {
                sdu_index = rlcP->pdu_retrans_buffer[snP].sdus_index[pdu_sdu_index];
                assert(pdu_sdu_index < RLC_AM_MAX_SDU_IN_PDU);
                assert(sdu_index < RLC_AM_SDU_CONTROL_BUFFER_SIZE);
                rlcP->input_sdus[sdu_index].nb_pdus_ack += 1;
                if (rlcP->input_sdus[sdu_index].nb_pdus_ack == rlcP->input_sdus[sdu_index].nb_pdus) {
                    #ifdef TEST_RLC_AM
                    rlc_am_v9_3_0_test_data_conf (rlcP->module_id, rlcP->rb_id, rlcP->input_sdus[sdu_index].mui, RLC_SDU_CONFIRM_NO);
                    #else
                    rlc_data_conf(rlcP->module_id, frame, rlcP->is_enb, rlcP->rb_id, rlcP->input_sdus[sdu_index].mui, RLC_SDU_CONFIRM_NO, rlcP->is_data_plane);
                    #endif
                    rlcP->stat_tx_pdcp_sdu_discarded   += 1;
                    rlcP->stat_tx_pdcp_bytes_discarded += rlcP->input_sdus[sdu_index].sdu_size;
                    rlc_am_free_in_sdu(rlcP, frame, sdu_index);
                }
            }
        }
    } else {
        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][NACK-PDU] ERROR NACK MISSING PDU SN %05d\n", frame, rlcP->module_id, rlcP->rb_id, snP);
        //assert(2==3);
    }
}
//-----------------------------------------------------------------------------
void rlc_am_ack_pdu (rlc_am_entity_t *rlcP, u32_t frame, u16_t snP)
//-----------------------------------------------------------------------------
{
    mem_block_t* mb         = rlcP->pdu_retrans_buffer[snP].mem_block;
    int          pdu_sdu_index;
    int          sdu_index;

    rlcP->pdu_retrans_buffer[snP].flags.retransmit = 0;

    if ((rlcP->pdu_retrans_buffer[snP].flags.ack == 0) && (mb != NULL)) {
    //if (mb != NULL) {
        free_mem_block(mb);
        rlcP->pdu_retrans_buffer[snP].mem_block = NULL;
        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][ACK-PDU] ACK PDU SN %05d previous retx_count %d \n", frame, rlcP->module_id, rlcP->rb_id, snP, rlcP->pdu_retrans_buffer[snP].retx_count);
        rlcP->retrans_num_pdus  -= 1;
        rlcP->retrans_num_bytes -= rlcP->pdu_retrans_buffer[snP].header_and_payload_size;

        if (rlcP->pdu_retrans_buffer[snP].retx_count >= 0) {
            rlcP->retrans_num_bytes_to_retransmit -= rlcP->pdu_retrans_buffer[snP].header_and_payload_size;
        }

        for (pdu_sdu_index = 0; pdu_sdu_index < rlcP->pdu_retrans_buffer[snP].nb_sdus; pdu_sdu_index++) {
            sdu_index = rlcP->pdu_retrans_buffer[snP].sdus_index[pdu_sdu_index];
            assert(sdu_index >= 0);
            assert(sdu_index < RLC_AM_SDU_CONTROL_BUFFER_SIZE);
            rlcP->input_sdus[sdu_index].nb_pdus_ack += 1;
            if ((rlcP->input_sdus[sdu_index].nb_pdus_ack == rlcP->input_sdus[sdu_index].nb_pdus) &&
                (rlcP->input_sdus[sdu_index].sdu_remaining_size == 0)) {
#ifdef TEST_RLC_AM
                rlc_am_v9_3_0_test_data_conf (rlcP->module_id, rlcP->rb_id, rlcP->input_sdus[sdu_index].mui, RLC_SDU_CONFIRM_YES);
#else
                rlc_data_conf(rlcP->module_id, frame, rlcP->is_enb, rlcP->rb_id, rlcP->input_sdus[sdu_index].mui, RLC_SDU_CONFIRM_YES, rlcP->is_data_plane);
#endif
                rlc_am_free_in_sdu(rlcP, frame, sdu_index);
            }
        }
        // 7.1...
        // VT(A) – Acknowledgement state variable
        // This state variable holds the value of the SN of the next AMD PDU for which a positive acknowledgment is to be
        // received in-sequence, and it serves as the lower edge of the transmitting window. It is initially set to 0, and is updated
        // whenever the AM RLC entity receives a positive acknowledgment for an AMD PDU with SN = VT(A).
        rlcP->pdu_retrans_buffer[snP].flags.ack = 1;
        if (snP == rlcP->vt_a) {
            //rlcP->pdu_retrans_buffer[snP].flags.ack = 1;
            do {
                memset(&rlcP->pdu_retrans_buffer[rlcP->vt_a], 0, sizeof(rlc_am_tx_data_pdu_management_t));
                if (rlcP->vt_a == rlcP->first_retrans_pdu_sn) {
                    rlcP->first_retrans_pdu_sn = (rlcP->vt_a  + 1) & RLC_AM_SN_MASK;
                }
                rlcP->vt_a = (rlcP->vt_a  + 1) & RLC_AM_SN_MASK;
            } while ((rlcP->pdu_retrans_buffer[rlcP->vt_a].flags.ack == 1) && (rlcP->vt_a != rlcP->vt_s));


            rlcP->vt_ms   = (rlcP->vt_a + RLC_AM_WINDOW_SIZE) & RLC_AM_SN_MASK;
            LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][ACK-PDU] UPDATED VT(A) %04d VT(MS) %04d  VT(S) %04d\n", frame, rlcP->module_id, rlcP->rb_id, rlcP->vt_a, rlcP->vt_ms, rlcP->vt_s);
        }
        if (snP == rlcP->first_retrans_pdu_sn) {
            do {
                rlcP->first_retrans_pdu_sn = (rlcP->first_retrans_pdu_sn  + 1) & RLC_AM_SN_MASK;
                if (rlcP->pdu_retrans_buffer[rlcP->first_retrans_pdu_sn].retx_count >= 0) {
                    LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][ACK-PDU] UPDATED  first_retrans_pdu_sn -> %04d\n", frame, rlcP->module_id, rlcP->rb_id, rlcP->first_retrans_pdu_sn);
                    break;
                }
            } while (rlcP->first_retrans_pdu_sn != rlcP->vt_s);
            if (rlcP->vt_s == rlcP->first_retrans_pdu_sn) {
                rlcP->first_retrans_pdu_sn = -1;
                LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][ACK-PDU] UPDATED  first_retrans_pdu_sn -> %04d\n", frame, rlcP->module_id, rlcP->rb_id, rlcP->first_retrans_pdu_sn);
            }
        }
    } else {
        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][ACK-PDU] WARNING ACK PDU SN %05d -> NO PDU TO ACK\n", frame, rlcP->module_id, rlcP->rb_id, snP);
        if (mb != NULL) {
            free_mem_block(mb);
            rlcP->pdu_retrans_buffer[snP].mem_block = NULL;
        }
        if (rlcP->pdu_retrans_buffer[snP].flags.ack > 0) {
            if (snP == rlcP->vt_a) {
                //rlcP->pdu_retrans_buffer[snP].flags.ack = 1;
                do {
                    memset(&rlcP->pdu_retrans_buffer[rlcP->vt_a], 0, sizeof(rlc_am_tx_data_pdu_management_t));
                    if (rlcP->vt_a == rlcP->first_retrans_pdu_sn) {
                        rlcP->first_retrans_pdu_sn = (rlcP->vt_a  + 1) & RLC_AM_SN_MASK;
                    }
                    rlcP->vt_a = (rlcP->vt_a  + 1) & RLC_AM_SN_MASK;
                } while ((rlcP->pdu_retrans_buffer[rlcP->vt_a].flags.ack == 1) && (rlcP->vt_a != rlcP->vt_s));

                rlcP->vt_ms   = (rlcP->vt_a + RLC_AM_WINDOW_SIZE) & RLC_AM_SN_MASK;
                LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][ACK-PDU] UPDATED VT(A) %04d VT(MS) %04d  VT(S) %04d\n", frame, rlcP->module_id, rlcP->rb_id, rlcP->vt_a, rlcP->vt_ms, rlcP->vt_s);
            }
        }
    }
}
//-----------------------------------------------------------------------------
mem_block_t* rlc_am_retransmit_get_copy (rlc_am_entity_t *rlcP, u32 frame, u16_t snP)
//-----------------------------------------------------------------------------
{
    mem_block_t* mb_original = rlcP->pdu_retrans_buffer[snP].mem_block;
    if (mb_original != NULL) {

        rlc_am_tx_data_pdu_management_t *pdu_mngt = &rlcP->pdu_retrans_buffer[snP % RLC_AM_PDU_RETRANSMISSION_BUFFER_SIZE];

        int size             = pdu_mngt->header_and_payload_size + sizeof(struct mac_tb_req);
        mem_block_t* mb_copy = get_free_mem_block(size);
        memcpy(mb_copy->data, mb_original->data, size);

        rlc_am_pdu_sn_10_t *pdu                         = (rlc_am_pdu_sn_10_t*) (&mb_copy->data[sizeof(struct mac_tb_req)]);
        ((struct mac_tb_req*)(mb_copy->data))->data_ptr = (u8_t*)pdu;

        pdu_mngt->flags.retransmit = 0;

        rlc_am_pdu_polling(rlcP, frame, pdu, pdu_mngt->payload_size);
        return mb_copy;
    } else {
        return NULL;
    }
}
//-----------------------------------------------------------------------------
mem_block_t* rlc_am_retransmit_get_subsegment(rlc_am_entity_t *rlcP, u32_t frame, u16_t snP, u16_t *sizeP)
//-----------------------------------------------------------------------------
{

  // 5.2 ARQ procedures
  // ARQ procedures are only performed by an AM RLC entity.
  // 5.2.1 Retransmission
  // The transmitting side of an AM RLC entity can receive a negative acknowledgement (notification of reception failure
  // by its peer AM RLC entity) for an AMD PDU or a portion of an AMD PDU by the following:
  // - STATUS PDU from its peer AM RLC entity.
  //
  // When receiving a negative acknowledgement for an AMD PDU or a portion of an AMD PDU by a STATUS PDU from
  // its peer AM RLC entity, the transmitting side of the AM RLC entity shall:
  //     - if the SN of the corresponding AMD PDU falls within the range VT(A) <= SN < VT(S):
  //         - consider the AMD PDU or the portion of the AMD PDU for which a negative acknowledgement was
  //           received for retransmission.
  //
  // When an AMD PDU or a portion of an AMD PDU is considered for retransmission, the transmitting side of the AM
  // RLC entity shall:
  //     - if the AMD PDU is considered for retransmission for the first time:
  //         - set the RETX_COUNT associated with the AMD PDU to zero;
  //     - else, if it (the AMD PDU or the portion of the AMD PDU that is considered for retransmission) is not pending
  //       for retransmission already, or a portion of it is not pending for retransmission already:
  //         - increment the RETX_COUNT;
  //     - if RETX_COUNT = maxRetxThreshold:
  //         - indicate to upper layers that max retransmission has been reached.
  //
  // When retransmitting an AMD PDU, the transmitting side of an AM RLC entity shall:
  //     - if the AMD PDU can entirely fit within the total size of RLC PDU(s) indicated by lower layer at the particular
  //       transmission opportunity:
  //         - deliver the AMD PDU as it is except for the P field (the P field should be set according to sub clause 5.2.2) to
  //           lower layer;
  //     - otherwise:
  //         - segment the AMD PDU, form a new AMD PDU segment which will fit within the total size of RLC PDU(s)
  //           indicated by lower layer at the particular transmission opportunity and deliver the new AMD PDU segment
  //           to lower layer.
  //
  // When retransmitting a portion of an AMD PDU, the transmitting side of an AM RLC entity shall:
  //    - segment the portion of the AMD PDU as necessary, form a new AMD PDU segment which will fit within the
  //    total size of RLC PDU(s) indicated by lower layer at the particular transmission opportunity and deliver the new
  //    AMD PDU segment to lower layer.
  //
  // When forming a new AMD PDU segment, the transmitting side of an AM RLC entity shall:
  //    - only map the Data field of the original AMD PDU to the Data field of the new AMD PDU segment;
  //    - set the header of the new AMD PDU segment in accordance with the description in sub clause 6.;
  //    - set the P field according to sub clause 5.2.2.

    mem_block_t*           mb_original     = rlcP->pdu_retrans_buffer[snP].mem_block;

    if (mb_original != NULL) {
        mem_block_t*           mb_sub_segment  = get_free_mem_block(*sizeP + sizeof(struct mac_tb_req));
        rlc_am_pdu_sn_10_t*    pdu_original    = (rlc_am_pdu_sn_10_t*) (&mb_original->data[sizeof(struct mac_tb_req)]);
        rlc_am_pdu_sn_10_t*    pdu_sub_segment = (rlc_am_pdu_sn_10_t*) (&mb_sub_segment->data[sizeof(struct mac_tb_req)]);
        rlc_am_pdu_info_t      pdu_info;
        int                    max_copy_payload_size;
        //LG avoid WARNING int                    test_max_copy_payload_size;
        int                    test_pdu_copy_size          = 0;

        ((struct mac_tb_req*)(mb_sub_segment->data))->data_ptr         = (u8_t*)&(mb_sub_segment->data[sizeof(struct mac_tb_req)]);

        if (rlc_am_get_data_pdu_infos(frame,pdu_original, rlcP->pdu_retrans_buffer[snP].header_and_payload_size, &pdu_info) >= 0) {
            int li_index = 0;
            int start_offset       = rlcP->pdu_retrans_buffer[snP].nack_so_start;
            int stop_offset        = rlcP->pdu_retrans_buffer[snP].nack_so_stop;

            LOG_D(RLC, "\n[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] ORIGINAL PDU SN %04d:\n",frame, rlcP->module_id, rlcP->rb_id, snP);
            rlc_am_display_data_pdu_infos(rlcP, frame,&pdu_info);

            // all 15 bits set to 1 (indicate that the missing portion of the AMD PDU includes all bytes
            // to the last byte of the AMD PDU)
            if (stop_offset == 0x7FFF) {
                rlcP->pdu_retrans_buffer[snP].nack_so_stop = rlcP->pdu_retrans_buffer[snP].payload_size - 1;
                stop_offset = rlcP->pdu_retrans_buffer[snP].nack_so_stop;
                LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] UPDATED RETRANS PDU SN %04d nack_so_stop FROM 0x7FFF to %05d\n",frame, rlcP->module_id, rlcP->rb_id,  snP, stop_offset);
            }

            // FIXED PART AMD PDU SEGMENT HEADER
            // 6.2.1.5 AMD PDU segment
            // AMD PDU segment consists of a Data field and an AMD PDU segment header.
            //
            // AMD PDU segment header consists of a fixed part (fields that are present for every AMD PDU segment) and an
            // extension part (fields that are present for an AMD PDU segment when necessary). The fixed part of the AMD PDU
            // segment header itself is byte aligned and consists of a D/C, a RF, a P, a FI, an E, a SN, a LSF and a SO. The extension
            // part of the AMD PDU segment header itself is byte aligned and consists of E(s) and LI(s).
            //
            // An AMD PDU segment header consists of an extension part only when more than one Data field elements are present
            // in the AMD PDU segment, in which case an E and a LI are present for every Data field element except the last.
            // Furthermore, when an AMD PDU segment header consists of an odd number of LI(s), four padding bits follow after the
            // last LI.

            pdu_sub_segment->b1 = (pdu_original->b1 & 0x83) | 0x40;
            pdu_sub_segment->b2 = pdu_original->b2;
            pdu_sub_segment->data[0] = ((u8_t)(start_offset >> 8));
            pdu_sub_segment->data[1] = ((u8_t)(start_offset & 0xFF));

            *sizeP = *sizeP - 4;


            int            continue_fill_pdu_with_pdu  = 1;
            int            test_pdu_remaining_size     = *sizeP;
            int            test_num_li                 = 0;
            int            fill_num_li                    ;
            int            test_li_sum                 = 0;
            int            not_test_fi                 = 0; // by default not 1st byte and not last byte af a SDU
            int            test_start_offset;
            unsigned int   test_li_length_in_bytes     = 1;
            s16_t          test_li_list[RLC_AM_MAX_SDU_IN_PDU];
            u8_t*          fill_payload;
            //int            test_fi_last_byte_pdu_is_last_byte_sdu = 0;
            //int            test_fi_first_byte_pdu_is_first_byte_sdu = 0;

            rlc_am_e_li_t* e_li_sub_segment            = (rlc_am_e_li_t*)(&pdu_sub_segment->data[2]);
            //int            first_enter_in_start_offset_lt_li_sum = 1;
            int            not_fi_original                 = ((pdu_original->b1 & 0x18) >> 3) ^ 3;

            //-------------------------------------------------------
            // set MAX payload size that can be copied
            // first constraint : the size of the hole to retransmit
            rlc_am_get_next_hole(rlcP, frame, snP, &start_offset, &stop_offset);
            max_copy_payload_size = stop_offset - start_offset + 1;
            assert(max_copy_payload_size > 0);
            assert(test_pdu_remaining_size > 0);
            LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] HOLE FOUND SO %d -> %d\n",frame, rlcP->module_id, rlcP->rb_id,  start_offset, stop_offset);
            LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] ORIGINAL FI %d\n",frame, rlcP->module_id, rlcP->rb_id,  (pdu_original->b1 & 0x18) >> 3);
            // second constraint the size of the pdu requested by MAC layer
            if (max_copy_payload_size > test_pdu_remaining_size) {
                  LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] CUT max_copy_payload_size with test_pdu_remaining_size %d -> %d\n",frame, rlcP->module_id, rlcP->rb_id,  max_copy_payload_size, test_pdu_remaining_size);
                  max_copy_payload_size = test_pdu_remaining_size;
            }


            if (start_offset == 0) {
                not_test_fi = (not_fi_original & 0x02);
            }
            test_start_offset = start_offset;

            //.find the li corresponding to the nack_so_start (start_offset)
            if (pdu_info.num_li > 0) {
            	LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] ORIGINAL NUM LI %d\n",frame, rlcP->module_id, rlcP->rb_id,  pdu_info.num_li);
                while ((li_index < pdu_info.num_li) && (continue_fill_pdu_with_pdu)) {
                	LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] FIND LI %d\n",frame, rlcP->module_id, rlcP->rb_id, pdu_info.li_list[li_index]);
                    if (max_copy_payload_size > test_pdu_remaining_size) {
                    	LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] CUT max_copy_payload_size with test_pdu_remaining_size %d -> %d\n",frame, rlcP->module_id, rlcP->rb_id,  max_copy_payload_size, test_pdu_remaining_size);
                          max_copy_payload_size = test_pdu_remaining_size;
                    }
                    assert(max_copy_payload_size >= 0);
                    assert(test_pdu_remaining_size >= 0);

                    test_li_sum               += pdu_info.li_list[li_index];

                    //---------------------------------------------------------------
                    if (test_start_offset < test_li_sum) {

                    	LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] test_start_offset < test_li_sum  %d < %d\n",frame, rlcP->module_id, rlcP->rb_id, test_start_offset, test_li_sum);
                        /*if (test_max_copy_payload_size > (test_li_sum - test_start_offset)) {
                            #ifdef TRACE_RLC_AM_RESEGMENT
                            LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] CUT test_max_copy_payload_size with test_li_sum - test_start_offset %d -> %d\n",frame, rlcP->module_id, rlcP->rb_id,  test_max_copy_payload_size, test_li_sum - test_start_offset);
                            #endif
                            test_max_copy_payload_size = test_li_sum - test_start_offset;
                        }*/

                        if ((max_copy_payload_size + test_start_offset) < test_li_sum) {
                        	LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] (max_copy_payload_size %d + test_start_offset %d) < test_li_sum %d\n",frame, rlcP->module_id, rlcP->rb_id, max_copy_payload_size, test_start_offset, test_li_sum);
                            LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] COPY SO %d -> %d  %d BYTES\n",frame, rlcP->module_id, rlcP->rb_id,  test_start_offset , test_start_offset + max_copy_payload_size - 1, max_copy_payload_size );
                            assert(max_copy_payload_size > 0);
                            continue_fill_pdu_with_pdu = 0;
                            test_pdu_copy_size         = test_pdu_copy_size + max_copy_payload_size;
                            test_start_offset          = test_start_offset + max_copy_payload_size;
                            not_test_fi                = not_test_fi & 0x02;  // clear b0, last byte does not correspond to last byte of a SDU
                            max_copy_payload_size      = 0;

                        } else if ((max_copy_payload_size + test_start_offset) == test_li_sum) {
                        	LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] (max_copy_payload_size + test_start_offset) == test_li_sum %d == %d\n",frame, rlcP->module_id, rlcP->rb_id,  (max_copy_payload_size + test_start_offset) , test_li_sum);
                            LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] COPY SO %d -> %d  %d BYTES\n",frame, rlcP->module_id, rlcP->rb_id,  test_start_offset , test_start_offset + max_copy_payload_size - 1, max_copy_payload_size );
                            assert(max_copy_payload_size > 0);
                            continue_fill_pdu_with_pdu = 0;
                            test_pdu_copy_size         = test_pdu_copy_size + max_copy_payload_size;
                            test_start_offset          = test_start_offset + max_copy_payload_size;
                            not_test_fi                = not_test_fi | 0x01;// set b0, last byte does correspond to last byte of a SDU
                            max_copy_payload_size      = 0;

                        } else if ((max_copy_payload_size + test_start_offset - (test_li_length_in_bytes ^ 3)) > test_li_sum) {
                        	LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] (max_copy_payload_size + test_start_offset - (test_li_length_in_bytes ^ 3)) > test_li_sum %d > %d\n SET LI %d\n",frame, rlcP->module_id, rlcP->rb_id,  (max_copy_payload_size + test_start_offset)  + (test_li_length_in_bytes ^ 3), test_li_sum, test_li_sum - test_start_offset);
                            LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] COPY SO %d -> %d  %d BYTES\n",frame, rlcP->module_id, rlcP->rb_id,  test_start_offset , test_li_sum - 1, test_li_sum - test_start_offset );
                            assert((test_li_sum - test_start_offset) > 0);
                            test_li_list[test_num_li++] = test_li_sum - test_start_offset;
                            test_pdu_copy_size          = test_pdu_copy_size + test_li_sum - test_start_offset;
                            test_li_length_in_bytes      = test_li_length_in_bytes ^ 3;

                            test_pdu_remaining_size     = test_pdu_remaining_size - test_li_sum + test_start_offset -
                                                          test_li_length_in_bytes;

                            max_copy_payload_size       = max_copy_payload_size - test_li_sum + test_start_offset - test_li_length_in_bytes;
                            LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] NOW max_copy_payload_size %d BYTES test_start_offset %d\n",frame, rlcP->module_id, rlcP->rb_id,  max_copy_payload_size, test_li_sum);
                            // normally the next while itereation will add bytes to PDU
                            //not_test_fi = not_test_fi | 0x01;  // set b0, last byte does correspond to last byte of a SDU
                            test_start_offset           = test_li_sum;

                        } else {
                        	LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] (test_max_copy_payload_size + test_start_offset ) > test_li_sum %d > %d\n NO REMAINING SIZE FOR LI",frame, rlcP->module_id, rlcP->rb_id,  (max_copy_payload_size + test_start_offset), test_li_sum);
                            LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] COPY SO %d -> %d  %d BYTES\n",frame, rlcP->module_id, rlcP->rb_id,  test_start_offset , test_li_sum - 1, test_li_sum - test_start_offset );
                            assert((test_li_sum - test_start_offset) > 0);
                            continue_fill_pdu_with_pdu = 0;
                            test_pdu_copy_size         = test_pdu_copy_size + test_li_sum - test_start_offset;

                            test_pdu_remaining_size    = test_pdu_remaining_size - test_li_sum + test_start_offset;

                            max_copy_payload_size      = max_copy_payload_size - test_li_sum + test_start_offset;
                            test_start_offset          = test_li_sum;
                            not_test_fi                = not_test_fi | 0x01;  // set b0, last byte does correspond to last byte of a SDU
                        }
                    //---------------------------------------------------------------
                    // start offset start at the begining of a SDU
                    // and it cant be the first data field of the original PDU
                    } else if (test_start_offset == test_li_sum) {
                    	LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] (test_start_offset == test_li_sum) %d == %d\n",frame, rlcP->module_id, rlcP->rb_id,  test_start_offset , test_li_sum);
                        if ((test_num_li == 0) && (test_pdu_copy_size == 0)) {
                            not_test_fi = not_test_fi | 0x02;  // set b1, first byte does correspond to first byte of a SDU
                        }
                    }
                    li_index = li_index + 1;
                }
                if ((continue_fill_pdu_with_pdu > 0) &&
                    (li_index    == pdu_info.num_li) &&
                    (pdu_info.hidden_size       > 0) &&
                    (test_pdu_remaining_size    > 0) &&
                    (max_copy_payload_size      > 0) ){

                    if (max_copy_payload_size > test_pdu_remaining_size) {
                    	LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] TRYING HIDDEN SIZE...CUT max_copy_payload_size with test_pdu_remaining_size %d -> %d\n",frame, rlcP->module_id, rlcP->rb_id,  max_copy_payload_size, test_pdu_remaining_size);
                          max_copy_payload_size = test_pdu_remaining_size;
                    }
                    // remaining bytes to fill, redundant check, but ...
                    if  ((max_copy_payload_size + test_start_offset) >= (pdu_info.hidden_size + test_li_sum)) {
                        test_pdu_copy_size += (pdu_info.hidden_size  + test_li_sum - test_start_offset);
                        LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] COPYING WHOLE REMAINING SIZE %d (max_copy_payload_size %d, test_start_offset %d, pdu_info.hidden_size %d test_li_sum %d test_pdu_copy_size %d)\n",frame, rlcP->module_id, rlcP->rb_id, pdu_info.hidden_size  + test_li_sum - test_start_offset, max_copy_payload_size, test_start_offset, pdu_info.hidden_size, test_li_sum, test_pdu_copy_size);
                        test_start_offset   = pdu_info.hidden_size   + test_li_sum;
                        not_test_fi = (not_test_fi & 0x2) | (not_fi_original & 0x1);  // set b0 idendical to the b0 of the non segmented PDU
                    } else {
                    	LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] COPYING REMAINING SIZE %d (/%d)\n",frame, rlcP->module_id, rlcP->rb_id, max_copy_payload_size, pdu_info.hidden_size);
                        test_pdu_copy_size += max_copy_payload_size;
                        test_start_offset = test_start_offset + max_copy_payload_size;
                        not_test_fi = not_test_fi & 0x2;  // clear b0 because no SDU ending in this PDU
                    }
                }
            } else { // num_li == 0
                LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] (num_li == 0)\n",frame, rlcP->module_id, rlcP->rb_id);
                test_pdu_copy_size = max_copy_payload_size;
                if ((stop_offset ==  (start_offset + max_copy_payload_size - 1)) && (stop_offset == rlcP->pdu_retrans_buffer[snP].payload_size - 1)) {
                    not_test_fi = (not_test_fi & 0x2) | (not_fi_original & 0x1);  // set b0 idendical to the b0 of the non segmented PDU
                } else {
                    not_test_fi = not_test_fi & 0x2;  // clear b0 because no SDU ending in this PDU
                }
            }
            //---------------------------------------------------------------
            /*if (stop_offset == (rlcP->pdu_retrans_buffer[snP].payload_size - 1)) {
                    test_fi = (test_fi & 0x02) | (fi_original & 0x01);
            }*/
            //---------------------------------------------------------------
            // write FI field in header
            //---------------------------------------------------------------
            pdu_sub_segment->b1 = pdu_sub_segment->b1 | (((not_test_fi << 3) ^ 0x18) & 0x18);
            //---------------------------------------------------------------
            // fill the segment pdu with Lis and data
            //---------------------------------------------------------------
            LOG_T(RLC, "[FRAME XXXXX][RLC_AM][MOD XX][RB XX][RE-SEGMENT] fill the segment pdu with Lis and data, test_num_li %d\n",test_num_li);
            if (test_num_li > 0) {
                pdu_sub_segment->b1 = pdu_sub_segment->b1 | 0x04; // set E bit
                test_li_length_in_bytes = 1;
                for (fill_num_li=0; fill_num_li < test_num_li; fill_num_li++) {
                    test_li_length_in_bytes = test_li_length_in_bytes ^ 3;

                    if (test_li_length_in_bytes  == 2) {
                        if (fill_num_li == (test_num_li - 1)) {
                            e_li_sub_segment->b1 = 0;
                        } else {
                            e_li_sub_segment->b1 =  0x80;
                        }
                        e_li_sub_segment->b1 = e_li_sub_segment->b1 | (test_li_list[fill_num_li] >> 4);
                        e_li_sub_segment->b2 = test_li_list[fill_num_li] << 4;
                        fill_payload         = (u8_t*)(&e_li_sub_segment->b3);
                        *sizeP               = *sizeP - 2;
                    } else {
                        if (fill_num_li != (test_num_li - 1)) {
                            e_li_sub_segment->b2  = e_li_sub_segment->b2 | 0x08;
                        }
                        e_li_sub_segment->b2 = e_li_sub_segment->b2 | (test_li_list[fill_num_li] >> 8);
                        e_li_sub_segment->b3 = test_li_list[fill_num_li] & 0xFF;
                        e_li_sub_segment++;
                        fill_payload         = (u8_t*)e_li_sub_segment;
                        *sizeP               = *sizeP - 1;
                    }
                    LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] ADD LI %d\n", frame, rlcP->module_id,rlcP->rb_id, test_li_list[fill_num_li]);
                }
            } else {
                    LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] ADD NO LI\n", frame, rlcP->module_id,rlcP->rb_id);
                fill_payload = (u8_t*)e_li_sub_segment;
            }
            //---------------------------------------------------------------
            // copy payload to retransmit
            //---------------------------------------------------------------
            memcpy(fill_payload,
                   &rlcP->pdu_retrans_buffer[snP].payload[start_offset],
                   test_pdu_copy_size);

            ((struct mac_tb_req*)(mb_sub_segment->data))->tb_size_in_bits  = ((((u32_t)fill_payload)+ test_pdu_copy_size) - ((u32_t)(&pdu_sub_segment->b1))) << 3;

            // set LSF
            if ((test_pdu_copy_size + start_offset) == rlcP->pdu_retrans_buffer[snP].payload_size) {
                pdu_sub_segment->data[0] = pdu_sub_segment->data[0] | 0x80;

                rlcP->pdu_retrans_buffer[snP].flags.retransmit = 0;

                LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] RE-SEND DATA PDU SN %04d SO %d %d BYTES PAYLOAD %d BYTES LSF!\n", frame, rlcP->module_id,rlcP->rb_id, snP, start_offset, ((struct mac_tb_req*)(mb_sub_segment->data))->tb_size_in_bits >> 3, test_pdu_copy_size);
            }
              else {
                LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] RE-SEND DATA PDU SN %04d SO %d %d BYTES PAYLOAD %d BYTES\n", frame, rlcP->module_id,rlcP->rb_id, snP, start_offset, ((struct mac_tb_req*)(mb_sub_segment->data))->tb_size_in_bits >> 3, test_pdu_copy_size);
            }
            LOG_T(RLC, "[FRAME XXXXX][RLC_AM][MOD XX][RB XX][RE-SEGMENT] *sizeP %d = *sizeP %d - test_pdu_copy_size %d\n",*sizeP - test_pdu_copy_size, *sizeP,  test_pdu_copy_size);

            *sizeP = *sizeP - test_pdu_copy_size;
            //---------------------------------------------------------------
            // update nack_so_start
            //---------------------------------------------------------------
            rlc_am_remove_hole(rlcP, frame, snP, start_offset, test_pdu_copy_size+start_offset - 1);
            //rlcP->pdu_retrans_buffer[snP].nack_so_start = rlcP->pdu_retrans_buffer[snP].nack_so_start + test_pdu_copy_size;
            LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] RE-SEND DATA PDU SN %04d NOW nack_so_start %d nack_so_stop %d\n", frame, rlcP->module_id,rlcP->rb_id, snP, rlcP->pdu_retrans_buffer[snP].nack_so_start, rlcP->pdu_retrans_buffer[snP].nack_so_stop);
            /*if (rlcP->pdu_retrans_buffer[snP].nack_so_start == rlcP->pdu_retrans_buffer[snP].nack_so_stop) {
                rlcP->pdu_retrans_buffer[snP].nack_so_start = 0;
                rlcP->pdu_retrans_buffer[snP].nack_so_stop  = 0x7FFF;
            }*/
        } else {
            LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] COULD NOT GET INFO FOR DATA PDU SN %04d -> RETURN NULL\n", frame, rlcP->module_id,rlcP->rb_id, snP);
            return NULL;
        }
        rlc_am_pdu_polling(rlcP, frame, pdu_sub_segment, test_pdu_copy_size);

        return mb_sub_segment;
    } else {
        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][RE-SEGMENT] RE-SEND DATA PDU SN %04d BUT NO PDU AVAILABLE -> RETURN NULL\n", frame, rlcP->module_id,rlcP->rb_id, snP);
        assert(3==4);
        return NULL;
    }
}
//-----------------------------------------------------------------------------
void rlc_am_tx_buffer_display (rlc_am_entity_t* rlcP, u32_t frame, char* messageP)
//-----------------------------------------------------------------------------
{
    u16_t       sn = rlcP->vt_a;
    int         i, loop = 0;

    if (messageP) {
        LOG_D(RLC, "\n[FRAME %05d][RLC_AM][MOD %02d][RB %02d] Retransmission buffer %s VT(A)=%04d VT(S)=%04d:", frame, rlcP->module_id, rlcP->rb_id, messageP, rlcP->vt_a, rlcP->vt_s);
    } else {
        LOG_D(RLC, "\n[FRAME %05d][RLC_AM][MOD %02d][RB %02d] Retransmission buffer VT(A)=%04d VT(S)=%04d:", frame, rlcP->module_id, rlcP->rb_id, rlcP->vt_a, rlcP->vt_s);
    }

    while (rlcP->vt_s != sn) {
        if (rlcP->pdu_retrans_buffer[sn].mem_block) {
            if ((loop % 1) == 0) {
                LOG_D(RLC, "\nTX SN:\t");
            }
            if (rlcP->pdu_retrans_buffer[sn].flags.retransmit) {
                LOG_D(RLC, "%04d %d/%d Bytes (NACK RTX:%02d ",sn, rlcP->pdu_retrans_buffer[sn].header_and_payload_size, rlcP->pdu_retrans_buffer[sn].payload_size, rlcP->pdu_retrans_buffer[sn].retx_count);
            } else {
                LOG_D(RLC, "%04d %d/%d Bytes (RTX:%02d ",sn, rlcP->pdu_retrans_buffer[sn].header_and_payload_size, rlcP->pdu_retrans_buffer[sn].payload_size, rlcP->pdu_retrans_buffer[sn].retx_count);
            }
            if (rlcP->pdu_retrans_buffer[sn].num_holes == 0) {
               LOG_D(RLC, "SO:%04d->%04d)\t", rlcP->pdu_retrans_buffer[sn].nack_so_start, rlcP->pdu_retrans_buffer[sn].nack_so_stop);
            } else {
                for (i=0; i<rlcP->pdu_retrans_buffer[sn].num_holes;i++){
                    assert(i < RLC_AM_MAX_HOLES_REPORT_PER_PDU);
                    LOG_D(RLC, "SO:%04d->%04d)\t", rlcP->pdu_retrans_buffer[sn].hole_so_start[i], rlcP->pdu_retrans_buffer[sn].hole_so_stop[i]);
                }
            }
            loop++;
        }
        sn = (sn + 1) & RLC_AM_SN_MASK;
    }
   LOG_D(RLC, "\n");
}
//-----------------------------------------------------------------------------
void rlc_am_retransmit_any_pdu(rlc_am_entity_t* rlcP,u32_t frame)
//-----------------------------------------------------------------------------
{
    u16_t                sn     = (rlcP->vt_s - 1) & RLC_AM_SN_MASK;
    u16_t                sn_end = (rlcP->vt_a - 1) & RLC_AM_SN_MASK;
    int                  found_pdu = 0;
    u16_t                found_pdu_sn = 0; // avoid warning
    mem_block_t*         pdu;
    rlc_am_pdu_sn_10_t*  pdu_sn_10;

    LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][FORCE-TRAFFIC] rlc_am_retransmit_any_pdu()\n", frame, rlcP->module_id,rlcP->rb_id);
    while (sn != sn_end) {
        if (rlcP->pdu_retrans_buffer[sn].mem_block != NULL) {
            if (!found_pdu) {
                found_pdu = 1;
                found_pdu_sn = sn;
            }
            if (rlcP->pdu_retrans_buffer[sn].header_and_payload_size <= rlcP->nb_bytes_requested_by_mac) {
                LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][FORCE-TRAFFIC] RE-SEND DATA PDU SN %04d\n", frame, rlcP->module_id,rlcP->rb_id, sn);
                rlc_am_nack_pdu (rlcP, frame, sn, 0, 0x7FFF);
                // no need for update rlcP->nb_bytes_requested_by_mac
                pdu = rlc_am_retransmit_get_copy(rlcP, frame, sn);
                pdu_sn_10 = (rlc_am_pdu_sn_10_t*) (&pdu->data[sizeof(struct mac_tb_req)]);
                rlc_am_pdu_polling(rlcP, frame, pdu_sn_10, rlcP->pdu_retrans_buffer[sn].header_and_payload_size);
                pdu_sn_10->b1 = pdu_sn_10->b1 | 0x20;
                rlcP->c_pdu_without_poll     = 0;
                rlcP->c_byte_without_poll    = 0;
                //rlcP->poll_sn = (rlcP->vt_s -1) & RLC_AM_SN_MASK;
                rlc_am_start_timer_poll_retransmit(rlcP,frame);
                rlcP->stat_tx_data_pdu                   += 1;
                rlcP->stat_tx_retransmit_pdu             += 1;
                rlcP->stat_tx_data_bytes                 += (((struct mac_tb_req*)(pdu->data))->tb_size_in_bits >> 3);
                rlcP->stat_tx_retransmit_bytes           += (((struct mac_tb_req*)(pdu->data))->tb_size_in_bits >> 3);
                list_add_tail_eurecom (pdu, &rlcP->pdus_to_mac_layer);
                return;
            }
        }
        sn = (sn - 1) & RLC_AM_SN_MASK;
    }
    // no pdu with correct size has been found
    // so re-segment a pdu if possible
    if (found_pdu) {
        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][FORCE-TRAFFIC] SEND SEGMENT OF DATA PDU SN %04d\n", frame, rlcP->module_id,rlcP->rb_id, found_pdu_sn);
        if (rlcP->nb_bytes_requested_by_mac > 4) {
            rlc_am_nack_pdu (rlcP, frame, found_pdu_sn, 0, 0x7FFF);
            pdu = rlc_am_retransmit_get_subsegment(rlcP, frame, found_pdu_sn, &rlcP->nb_bytes_requested_by_mac);
            pdu_sn_10 = (rlc_am_pdu_sn_10_t*) (&pdu->data[sizeof(struct mac_tb_req)]);
            rlc_am_pdu_polling(rlcP, frame, pdu_sn_10, rlcP->pdu_retrans_buffer[found_pdu_sn].header_and_payload_size);
            pdu_sn_10->b1 = pdu_sn_10->b1 | 0x20;
            rlcP->c_pdu_without_poll     = 0;
            rlcP->c_byte_without_poll    = 0;
            //rlcP->poll_sn = (rlcP->vt_s -1) & RLC_AM_SN_MASK;
            rlc_am_start_timer_poll_retransmit(rlcP,frame);
            rlcP->stat_tx_data_pdu                   += 1;
            rlcP->stat_tx_retransmit_pdu             += 1;
            rlcP->stat_tx_data_bytes                 += (((struct mac_tb_req*)(pdu->data))->tb_size_in_bits >> 3);
            rlcP->stat_tx_retransmit_bytes           += (((struct mac_tb_req*)(pdu->data))->tb_size_in_bits >> 3);
            list_add_tail_eurecom (pdu, &rlcP->pdus_to_mac_layer);
            return;
        } else {
            LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][FORCE-TRAFFIC] ... BUT NOT ENOUGH BYTES ALLOWED BY MAC %0d\n", frame, rlcP->module_id,rlcP->rb_id, rlcP->nb_bytes_requested_by_mac);
        }
    }
}

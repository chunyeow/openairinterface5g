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
#define RLC_AM_SEGMENT_C
//-----------------------------------------------------------------------------
#ifdef USER_MODE
#include <assert.h>
#endif
//-----------------------------------------------------------------------------
//#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "list.h"
#include "rlc_am.h"
#include "LAYER2/MAC/extern.h"
#include "UTIL/LOG/log.h"

//-----------------------------------------------------------------------------
void rlc_am_pdu_polling (rlc_am_entity_t *rlcP, u32_t frame, rlc_am_pdu_sn_10_t *pduP, s16_t payload_sizeP)
//-----------------------------------------------------------------------------
{
    // 5.2.2 Polling
    // An AM RLC entity can poll its peer AM RLC entity in order to trigger STATUS reporting at the peer AM RLC entity.
    // 5.2.2.1 Transmission of a AMD PDU or AMD PDU segment
    // Upon assembly of a new AMD PDU, the transmitting side of an AM RLC entity shall:
    //     - increment PDU_WITHOUT_POLL by one;
    //     - increment BYTE_WITHOUT_POLL by every new byte of Data field element that it maps to the Data field of
    //       the RLC data PDU;
    //     - if PDU_WITHOUT_POLL >= pollPDU; or
    //     - if BYTE_WITHOUT_POLL >= pollByte;
    //         -include a poll in the RLC data PDU as described below.
    // Upon assembly of an AMD PDU or AMD PDU segment, the transmitting side of an AM RLC entity shall:
    //     - if both the transmission buffer and the retransmission buffer becomes empty (excluding transmitted RLC data
    //       PDU awaiting for acknowledgements) after the transmission of the RLC data PDU; or
    //     - if no new RLC data PDU can be transmitted after the transmission of the RLC data PDU (e.g. due to window
    //       stalling);
    //         - include a poll in the RLC data PDU as described below.
    // To include a poll in a RLC data PDU, the transmitting side of an AM RLC entity shall:
    //     - set the P field of the RLC data PDU to "1";
    //     - set PDU_WITHOUT_POLL to 0;
    //     - set BYTE_WITHOUT_POLL to 0;
    // After delivering a RLC data PDU including a poll to lower layer and after incrementing of VT(S) if necessary, the
    // transmitting side of an AM RLC entity shall:
    //     - set POLL_SN to VT(S) – 1;
    //     - if t-PollRetransmit is not running:
    //         - start t-PollRetransmit;
    //     - else:
    //         - restart t-PollRetransmit;
    rlcP->c_pdu_without_poll     += 1;
    rlcP->c_byte_without_poll    += payload_sizeP;

    if (
        (rlcP->c_pdu_without_poll >= rlcP->poll_pdu) ||
        (rlcP->c_byte_without_poll >= rlcP->poll_byte) ||
        ((rlcP->sdu_buffer_occupancy == 0) && (rlcP->retrans_num_bytes_to_retransmit == 0)) ||
        (rlcP->vt_s == rlcP->vt_ms)
        ) {

        if (rlcP->c_pdu_without_poll >= rlcP->poll_pdu) {
            LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][POLL] SET POLL BECAUSE TX NUM PDU THRESHOLD %d  HAS BEEN REACHED\n", frame, rlcP->module_id, rlcP->rb_id, rlcP->poll_pdu);
        }
        if (rlcP->c_pdu_without_poll >= rlcP->poll_pdu) {
            LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][POLL] SET POLL BECAUSE TX NUM BYTES THRESHOLD %d  HAS BEEN REACHED\n", frame, rlcP->module_id, rlcP->rb_id, rlcP->poll_byte);
        }
        if ((rlcP->sdu_buffer_occupancy == 0) && (rlcP->retrans_num_bytes_to_retransmit == 0)) {
            LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][POLL] SET POLL BECAUSE TX BUFFERS ARE EMPTY\n", frame, rlcP->module_id, rlcP->rb_id);
        }
        if (rlcP->vt_s == rlcP->vt_ms) {
            LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][POLL] SET POLL BECAUSE OF WINDOW STALLING\n", frame, rlcP->module_id, rlcP->rb_id);
        }
        pduP->b1 = pduP->b1 | 0x20;
        rlcP->c_pdu_without_poll     = 0;
        rlcP->c_byte_without_poll    = 0;

        rlcP->poll_sn = (rlcP->vt_s -1) & RLC_AM_SN_MASK;
        //optimisation if (!rlcP->t_poll_retransmit.running) {
        rlc_am_start_timer_poll_retransmit(rlcP,frame);
        //optimisation } else {
        //optimisation     rlcP->t_poll_retransmit.frame_time_out = frame + rlcP->t_poll_retransmit.time_out;
        //optimisation }
    } else {
        pduP->b1 = pduP->b1 & 0xDF;
    }
}
//-----------------------------------------------------------------------------
void rlc_am_segment_10 (rlc_am_entity_t *rlcP,u32_t frame)
{
//-----------------------------------------------------------------------------
    list_t              pdus;
    signed int          pdu_remaining_size;
    signed int          test_pdu_remaining_size;

    int                 nb_bytes_to_transmit = rlcP->nb_bytes_requested_by_mac;
    rlc_am_pdu_sn_10_t *pdu;
    struct mac_tb_req  *pdu_tb_req;
    mem_block_t        *pdu_mem;
    unsigned char      *data;
    unsigned char      *data_sdu;
    rlc_am_e_li_t      *e_li;
    rlc_am_tx_sdu_management_t *sdu_mngt;
    rlc_am_tx_data_pdu_management_t *pdu_mngt;

    unsigned int       li_length_in_bytes;
    unsigned int       test_li_length_in_bytes;
    unsigned int       test_remaining_size_to_substract;
    unsigned int       test_remaining_num_li_to_substract;
    unsigned int       continue_fill_pdu_with_sdu;
    unsigned int       num_fill_sdu;
    unsigned int       test_num_li;
    unsigned int       fill_num_li;
    unsigned int       sdu_buffer_index;
    unsigned int       data_pdu_size;

    unsigned int       fi_first_byte_pdu_is_first_byte_sdu;
    unsigned int       fi_last_byte_pdu_is_last_byte_sdu;
    unsigned int       fi;
    unsigned int       max_li_overhead;

    LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEGMENT] rlcP->current_sdu_index %d rlcP->next_sdu_index %d rlcP->input_sdus[rlcP->current_sdu_index].mem_block %p sdu_buffer_occupancy %d\n", frame, rlcP->module_id, rlcP->rb_id, rlcP->current_sdu_index, rlcP->next_sdu_index, rlcP->input_sdus[rlcP->current_sdu_index].mem_block, rlcP->sdu_buffer_occupancy);
    if (rlcP->sdu_buffer_occupancy <= 0) {
        return;
    }

    //msg ("[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEGMENT]\n", rlcP->module_id, rlcP->rb_id, frame);
    list_init (&pdus, NULL);    // param string identifying the list is NULL
    pdu_mem = NULL;


    while ((rlcP->input_sdus[rlcP->current_sdu_index].mem_block) && (nb_bytes_to_transmit > 0) ) {
        LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEGMENT] nb_bytes_to_transmit %d BO %d\n", frame, rlcP->module_id, rlcP->rb_id, nb_bytes_to_transmit, rlcP->sdu_buffer_occupancy);
        // pdu management
        if (!pdu_mem) {
            if (rlcP->nb_sdu_no_segmented <= 1) {
                max_li_overhead = 0;
            } else {
                max_li_overhead = (((rlcP->nb_sdu_no_segmented - 1) * 3) / 2) + ((rlcP->nb_sdu_no_segmented - 1) % 2);
            }
            LOG_T(RLC, "FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEGMENT] max_li_overhead %d\n", frame, rlcP->module_id, rlcP->rb_id, max_li_overhead);
            if  (nb_bytes_to_transmit >= (rlcP->sdu_buffer_occupancy + RLC_AM_HEADER_MIN_SIZE + max_li_overhead)) {
                data_pdu_size = rlcP->sdu_buffer_occupancy + RLC_AM_HEADER_MIN_SIZE + max_li_overhead;
                LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEGMENT] alloc PDU size %d bytes to contain not all bytes requested by MAC but all BO of RLC@1\n", frame, rlcP->module_id, rlcP->rb_id, data_pdu_size);
            } else {
                data_pdu_size = nb_bytes_to_transmit;
               LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEGMENT] alloc PDU size %d bytes to contain all bytes requested by MAC@1\n", frame, rlcP->module_id, rlcP->rb_id, data_pdu_size);
            }
            if (!(pdu_mem = get_free_mem_block (data_pdu_size + sizeof(struct mac_tb_req)))) {
                LOG_C(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEGMENT] ERROR COULD NOT GET NEW PDU, EXIT\n", frame, rlcP->module_id, rlcP->rb_id);
                return;
            }
            LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEGMENT] get new PDU %d bytes\n", frame, rlcP->module_id, rlcP->rb_id, data_pdu_size);
            pdu_remaining_size = data_pdu_size - RLC_AM_HEADER_MIN_SIZE;
            pdu        = (rlc_am_pdu_sn_10_t*) (&pdu_mem->data[sizeof(struct mac_tb_req)]);
            pdu_tb_req = (struct mac_tb_req*) (pdu_mem->data);
            pdu_mngt   = &rlcP->pdu_retrans_buffer[rlcP->vt_s % RLC_AM_PDU_RETRANSMISSION_BUFFER_SIZE];
            memset(pdu_mngt, 0, sizeof (rlc_am_tx_data_pdu_management_t));

            memset (pdu_mem->data, 0, sizeof (rlc_am_pdu_sn_10_t)+sizeof(struct mac_tb_req));
            li_length_in_bytes = 1;
        }
        //----------------------------------------
        // compute how many SDUS can fill the PDU
        //----------------------------------------
        continue_fill_pdu_with_sdu = 1;
        num_fill_sdu               = 0;
        test_num_li                = 0;
        sdu_buffer_index           = rlcP->current_sdu_index;
        test_pdu_remaining_size    = pdu_remaining_size;
        test_li_length_in_bytes    = 1;
        test_remaining_size_to_substract   = 0;
        test_remaining_num_li_to_substract = 0;


        while ((rlcP->input_sdus[sdu_buffer_index].mem_block) && (continue_fill_pdu_with_sdu > 0)) {
            sdu_mngt = &rlcP->input_sdus[sdu_buffer_index];

            if (sdu_mngt->sdu_remaining_size > test_pdu_remaining_size) {
                // no LI
                continue_fill_pdu_with_sdu = 0;
                num_fill_sdu += 1;
                test_pdu_remaining_size = 0;
                test_remaining_size_to_substract = 0;
                test_remaining_num_li_to_substract = 0;
            } else if (sdu_mngt->sdu_remaining_size == test_pdu_remaining_size) {
                // fi will indicate end of PDU is end of SDU, no need for LI
                continue_fill_pdu_with_sdu = 0;
                num_fill_sdu += 1;
                test_pdu_remaining_size = 0;
                test_remaining_size_to_substract = 0;
                test_remaining_num_li_to_substract = 0;
            } else if ((sdu_mngt->sdu_remaining_size + (test_li_length_in_bytes ^ 3)) == test_pdu_remaining_size ) {
                // no LI
                continue_fill_pdu_with_sdu = 0;
                num_fill_sdu += 1;
                test_pdu_remaining_size = 0;
                test_remaining_size_to_substract = 0;
                test_remaining_num_li_to_substract = 0;
                pdu_remaining_size = pdu_remaining_size - (test_li_length_in_bytes ^ 3);
            } else if ((sdu_mngt->sdu_remaining_size + (test_li_length_in_bytes ^ 3)) < test_pdu_remaining_size ) {
                if (pdu_mngt->nb_sdus >= (RLC_AM_MAX_SDU_IN_PDU-1)) {
                    continue_fill_pdu_with_sdu = 0;
                    //num_fill_sdu += 1;
                    test_pdu_remaining_size = 0;
                    test_remaining_size_to_substract = 0;
                    test_remaining_num_li_to_substract = 0;
                    pdu_remaining_size = pdu_remaining_size - 1;
                } else {
                    test_num_li += 1;
                    num_fill_sdu += 1;
                    test_pdu_remaining_size = test_pdu_remaining_size - (sdu_mngt->sdu_remaining_size + (test_li_length_in_bytes ^ 3));
                    test_remaining_size_to_substract = test_li_length_in_bytes ^ 3;
                    test_remaining_num_li_to_substract = 1;
                    test_li_length_in_bytes = test_li_length_in_bytes ^ 3;
                }
            } else {
                LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEGMENT] sdu_mngt->sdu_remaining_size=%d test_pdu_remaining_size=%d test_li_length_in_bytes=%d\n", frame, rlcP->module_id, rlcP->rb_id, sdu_mngt->sdu_remaining_size, test_pdu_remaining_size, test_li_length_in_bytes ^ 3);
                // reduce the size of the PDU
                continue_fill_pdu_with_sdu = 0;
                num_fill_sdu += 1;
                test_pdu_remaining_size = 0;
                test_remaining_size_to_substract = 0;
                test_remaining_num_li_to_substract = 0;
                pdu_remaining_size = pdu_remaining_size - 1;
            }
            pdu_mngt->sdus_index[pdu_mngt->nb_sdus++] = sdu_buffer_index;
            sdu_mngt->pdus_index[sdu_mngt->nb_pdus++] = rlcP->vt_s % RLC_AM_PDU_RETRANSMISSION_BUFFER_SIZE;
            assert(sdu_mngt->nb_pdus < RLC_AM_MAX_SDU_FRAGMENTS);
            sdu_buffer_index = (sdu_buffer_index + 1) % RLC_AM_SDU_CONTROL_BUFFER_SIZE;
        }
        if (test_remaining_num_li_to_substract > 0) {
            // there is a LI that is not necessary
            test_num_li = test_num_li - 1;
            pdu_remaining_size = pdu_remaining_size - test_remaining_size_to_substract;
        }
        //----------------------------------------
        // Do the real filling of the pdu
        //----------------------------------------
        LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d] data shift %d Bytes num_li %d\n", frame, rlcP->module_id, rlcP->rb_id, ((test_num_li*3) +1) >> 1, test_num_li);
        data = ((unsigned char*)(&pdu->data[((test_num_li*3) +1) >> 1]));
        pdu_mngt->payload = data;
        e_li = (rlc_am_e_li_t*)(pdu->data);
        continue_fill_pdu_with_sdu          = 1;
        li_length_in_bytes                  = 1;
        fill_num_li                         = 0;
        fi_first_byte_pdu_is_first_byte_sdu = 0;
        fi_last_byte_pdu_is_last_byte_sdu   = 0;

        if (rlcP->input_sdus[rlcP->current_sdu_index].sdu_remaining_size ==
            rlcP->input_sdus[rlcP->current_sdu_index].sdu_size) {
            fi_first_byte_pdu_is_first_byte_sdu = 1;
        }
        while ((rlcP->input_sdus[rlcP->current_sdu_index].mem_block) && (continue_fill_pdu_with_sdu > 0)) {
            sdu_mngt = &rlcP->input_sdus[rlcP->current_sdu_index];
            if (sdu_mngt->sdu_segmented_size == 0) {
                LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEGMENT] GET NEW SDU %p AVAILABLE SIZE %d Bytes\n", frame, rlcP->module_id, rlcP->rb_id, sdu_mngt, sdu_mngt->sdu_remaining_size);
            } else {
                LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEGMENT] GET AGAIN SDU %p REMAINING AVAILABLE SIZE %d Bytes / %d Bytes LENGTH \n", frame, rlcP->module_id, rlcP->rb_id, sdu_mngt, sdu_mngt->sdu_remaining_size, sdu_mngt->sdu_size);
            }
            data_sdu = &sdu_mngt->first_byte[sdu_mngt->sdu_segmented_size];

            if (sdu_mngt->sdu_remaining_size > pdu_remaining_size) {
                LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEGMENT] Filling all remaining PDU with %d bytes\n", frame, rlcP->module_id, rlcP->rb_id, pdu_remaining_size);
                //msg ("[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEGMENT] pdu_mem %p pdu %p pdu->data %p data %p data_sdu %p pdu_remaining_size %d\n", rlcP->module_id, rlcP->rb_id, frame, pdu_mem, pdu, pdu->data, data, data_sdu,pdu_remaining_size);

                memcpy(data, data_sdu, pdu_remaining_size);
                pdu_mngt->payload_size += pdu_remaining_size;
                sdu_mngt->sdu_remaining_size = sdu_mngt->sdu_remaining_size - pdu_remaining_size;
                sdu_mngt->sdu_segmented_size = sdu_mngt->sdu_segmented_size + pdu_remaining_size;
                fi_last_byte_pdu_is_last_byte_sdu = 0;
                // no LI
                rlcP->sdu_buffer_occupancy -= pdu_remaining_size;
                continue_fill_pdu_with_sdu = 0;
                pdu_remaining_size = 0;
                LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEGMENT] sdu_remaining_size %d bytes sdu_segmented_size %d bytes\n", frame, rlcP->module_id, rlcP->rb_id, sdu_mngt->sdu_remaining_size, sdu_mngt->sdu_segmented_size);
            } else if (sdu_mngt->sdu_remaining_size == pdu_remaining_size) {
                LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEGMENT] Exactly Filling remaining PDU with %d remaining bytes of SDU\n", frame, rlcP->module_id, rlcP->rb_id, pdu_remaining_size);
                memcpy(data, data_sdu, pdu_remaining_size);
                pdu_mngt->payload_size += pdu_remaining_size;

                // free SDU
                rlcP->sdu_buffer_occupancy -= sdu_mngt->sdu_remaining_size;
                rlc_am_free_in_sdu_data(rlcP, rlcP->current_sdu_index);
                //free_mem_block (rlcP->input_sdus[rlcP->current_sdu_index]);
                //rlcP->input_sdus[rlcP->current_sdu_index] = NULL;
                //rlcP->nb_sdu -= 1;
                rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % RLC_AM_SDU_CONTROL_BUFFER_SIZE;

                fi_last_byte_pdu_is_last_byte_sdu = 1;
                // fi will indicate end of PDU is end of SDU, no need for LI
                continue_fill_pdu_with_sdu = 0;
                pdu_remaining_size = 0;
            } else if ((sdu_mngt->sdu_remaining_size + (li_length_in_bytes ^ 3)) < pdu_remaining_size ) {
                if (fill_num_li == (RLC_AM_MAX_SDU_IN_PDU - 1)) {
                    LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEGMENT] [SIZE %d] REACHING RLC_AM_MAX_SDU_IN_PDU LIs -> STOP SEGMENTATION FOR THIS PDU SDU\n", frame, rlcP->module_id, rlcP->rb_id, sdu_mngt->sdu_remaining_size);
                    memcpy(data, data_sdu, sdu_mngt->sdu_remaining_size);
                    pdu_mngt->payload_size += sdu_mngt->sdu_remaining_size;
                    pdu_remaining_size = 0; //Forced to 0 pdu_remaining_size - sdu_mngt->sdu_remaining_size;
                    // free SDU
                    rlcP->sdu_buffer_occupancy -= sdu_mngt->sdu_remaining_size;
                    rlc_am_free_in_sdu_data(rlcP, rlcP->current_sdu_index);
                    //rlcP->input_sdus[rlcP->current_sdu_index] = NULL;
                    //rlcP->nb_sdu -= 1;
                    rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % RLC_AM_SDU_CONTROL_BUFFER_SIZE;

                    // reduce the size of the PDU
                    continue_fill_pdu_with_sdu = 0;
                    fi_last_byte_pdu_is_last_byte_sdu = 1;
                } else {
                    LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEGMENT] Filling  PDU with %d all remaining bytes of SDU\n", frame, rlcP->module_id, rlcP->rb_id, sdu_mngt->sdu_remaining_size);
                    memcpy(data, data_sdu, sdu_mngt->sdu_remaining_size);
                    pdu_mngt->payload_size += sdu_mngt->sdu_remaining_size;
                    data = &data[sdu_mngt->sdu_remaining_size];
                    li_length_in_bytes = li_length_in_bytes ^ 3;
                    fill_num_li += 1;
                    if (li_length_in_bytes  == 2) {
                        if (fill_num_li == test_num_li) {
                            //e_li->e1  = 0;
                            e_li->b1 = 0;
                        } else {
                            //e_li->e1  = 1;
                            e_li->b1 =  0x80;
                        }
                        //e_li->li1 = sdu_mngt->sdu_remaining_size;
                        e_li->b1 = e_li->b1 | (sdu_mngt->sdu_remaining_size >> 4);
                        e_li->b2 = sdu_mngt->sdu_remaining_size << 4;
                        LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEGMENT] set e_li->b1=0x%02X set e_li->b2=0x%02X fill_num_li=%d test_num_li=%d\n", frame, rlcP->module_id, rlcP->rb_id, e_li->b1, e_li->b2, fill_num_li, test_num_li);
                    } else {
                        if (fill_num_li != test_num_li) {
                            //e_li->e2  = 1;
                            e_li->b2  = e_li->b2 | 0x08;
                        }
                        //e_li->li2 = sdu_mngt->sdu_remaining_size;
                        e_li->b2 = e_li->b2 | (sdu_mngt->sdu_remaining_size >> 8);
                        e_li->b3 = sdu_mngt->sdu_remaining_size & 0xFF;
                        LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEGMENT] set e_li->b2=0x%02X set e_li->b3=0x%02X fill_num_li=%d test_num_li=%d\n", frame, rlcP->module_id, rlcP->rb_id, e_li->b2, e_li->b3, fill_num_li, test_num_li);
                        e_li++;
                    }

                    pdu_remaining_size = pdu_remaining_size - (sdu_mngt->sdu_remaining_size + li_length_in_bytes);
                    // free SDU
                    rlcP->sdu_buffer_occupancy  -= sdu_mngt->sdu_remaining_size;
                    sdu_mngt->sdu_remaining_size = 0;

                    rlc_am_free_in_sdu_data(rlcP, rlcP->current_sdu_index);
                    //free_mem_block (rlcP->input_sdus[rlcP->current_sdu_index]);
                    //rlcP->input_sdus[rlcP->current_sdu_index] = NULL;
                    //rlcP->nb_sdu -= 1;
                    rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % RLC_AM_SDU_CONTROL_BUFFER_SIZE;
                }
            } else {
                LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEGMENT] Filling  PDU with %d all remaining bytes of SDU and reduce TB size by %d bytes\n", frame, rlcP->module_id, rlcP->rb_id, sdu_mngt->sdu_remaining_size, pdu_remaining_size - sdu_mngt->sdu_remaining_size);
#ifdef USER_MODE
                assert(1!=1);
#endif
                memcpy(data, data_sdu, sdu_mngt->sdu_remaining_size);
                pdu_mngt->payload_size += sdu_mngt->sdu_remaining_size;
                pdu_remaining_size = pdu_remaining_size - sdu_mngt->sdu_remaining_size;
                // free SDU
                rlcP->sdu_buffer_occupancy -= sdu_mngt->sdu_remaining_size;
                rlc_am_free_in_sdu_data(rlcP, rlcP->current_sdu_index);
                //rlcP->input_sdus[rlcP->current_sdu_index] = NULL;
                //rlcP->nb_sdu -= 1;
                rlcP->current_sdu_index = (rlcP->current_sdu_index + 1) % RLC_AM_SDU_CONTROL_BUFFER_SIZE;

                // reduce the size of the PDU
                continue_fill_pdu_with_sdu = 0;
                fi_last_byte_pdu_is_last_byte_sdu = 1;
            }
        }

        // set framing info
        if (fi_first_byte_pdu_is_first_byte_sdu) {
            fi = 0;
        } else {
            fi = 2;
        }
        if (!fi_last_byte_pdu_is_last_byte_sdu) {
            fi = fi + 1;
        }
        pdu->b1 = pdu->b1 | (fi << 3);

        // set fist e bit
        if (fill_num_li > 0) {
            pdu->b1 = pdu->b1 | 0x04;
        }
        LOG_T(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][SEGMENT] SEND PDU SN %04d  SIZE %d BYTES PAYLOAD SIZE %d BYTES\n",
             frame,
             rlcP->module_id,
             rlcP->rb_id,
             rlcP->vt_s,
             data_pdu_size - pdu_remaining_size,
             pdu_mngt->payload_size);

        rlcP->stat_tx_data_pdu   += 1;
        rlcP->stat_tx_data_bytes += (data_pdu_size - pdu_remaining_size);

        //pdu->sn = rlcP->vt_s;
        pdu->b1 = pdu->b1 | 0x80; // DATA/CONTROL field is DATA PDU
        pdu->b1 = pdu->b1 | (rlcP->vt_s >> 8);
        pdu->b2 = rlcP->vt_s & 0xFF;
        rlcP->vt_s = (rlcP->vt_s+1) & RLC_AM_SN_MASK;

        pdu_tb_req->data_ptr        = (unsigned char*)pdu;
        pdu_tb_req->tb_size         = data_pdu_size - pdu_remaining_size;

        assert(pdu_tb_req->tb_size < 3000);
        rlc_am_pdu_polling(rlcP, frame,pdu, pdu_mngt->payload_size);

        //list_add_tail_eurecom (pdu_mem, &rlcP->segmentation_pdu_list);
        pdu_mngt->mem_block  = pdu_mem;
        pdu_mngt->first_byte = (unsigned char*)pdu;
        pdu_mngt->header_and_payload_size  = data_pdu_size - pdu_remaining_size;
        pdu_mngt->retx_count = -1;

        rlcP->retrans_num_pdus  += 1;
        rlcP->retrans_num_bytes += pdu_mngt->header_and_payload_size;

        pdu = NULL;
        pdu_mem = NULL;

        //nb_bytes_to_transmit = nb_bytes_to_transmit - data_pdu_size;
        nb_bytes_to_transmit = 0; // 1 PDU only

        mem_block_t* copy = rlc_am_retransmit_get_copy (rlcP, frame,(rlcP->vt_s-1) & RLC_AM_SN_MASK);
        list_add_tail_eurecom (copy, &rlcP->segmentation_pdu_list);

    }
}

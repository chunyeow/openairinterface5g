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
#define RLC_UM_MODULE
#define RLC_UM_DAR_C
#include "platform_types.h"
#include "assertions.h"
//-----------------------------------------------------------------------------
#include "rlc.h"
#include "rlc_um.h"
#include "rlc_primitives.h"
#include "mac_primitives.h"
#include "list.h"
#include "MAC_INTERFACE/extern.h"
#include "UTIL/LOG/log.h"

//#define TRACE_RLC_UM_DAR 1
//#define TRACE_RLC_UM_RX  1
//-----------------------------------------------------------------------------
signed int rlc_um_get_pdu_infos(
    const frame_t               frameP,
    rlc_um_pdu_sn_10_t  * const header_pP,
    const sdu_size_t            total_sizeP,
    rlc_um_pdu_info_t   * const pdu_info_pP,
    const uint8_t               sn_lengthP)
//-----------------------------------------------------------------------------
{
    sdu_size_t         sum_li = 0;
    memset(pdu_info_pP, 0, sizeof (rlc_um_pdu_info_t));

    pdu_info_pP->num_li = 0;

    AssertFatal( total_sizeP > 0 , "RLC UM PDU LENGTH %d", total_sizeP);

    if (sn_lengthP == 10) {
        pdu_info_pP->fi           = (header_pP->b1 >> 3) & 0x03;
        pdu_info_pP->e            = (header_pP->b1 >> 2) & 0x01;
        pdu_info_pP->sn           = header_pP->b2 + (((uint16_t)(header_pP->b1 & 0x03)) << 8);
        pdu_info_pP->header_size  = 2;
        pdu_info_pP->payload      = &header_pP->data[0];
    } else if (sn_lengthP == 5) {
        pdu_info_pP->fi           = (header_pP->b1 >> 6) & 0x03;
        pdu_info_pP->e            = (header_pP->b1 >> 5) & 0x01;
        pdu_info_pP->sn           = header_pP->b1 & 0x1F;
        pdu_info_pP->header_size  = 1;
        pdu_info_pP->payload      = &header_pP->b2;
    } else {
        AssertFatal( sn_lengthP == 5 || sn_lengthP == 10, "RLC UM SN LENGTH %d", sn_lengthP);
    }


    if (pdu_info_pP->e) {
        rlc_am_e_li_t      *e_li_p;
        unsigned int li_length_in_bytes  = 1;
        unsigned int li_to_read          = 1;

        e_li_p = (rlc_am_e_li_t*)(pdu_info_pP->payload);

        while (li_to_read)  {
            li_length_in_bytes = li_length_in_bytes ^ 3;
            if (li_length_in_bytes  == 2) {
                AssertFatal( total_sizeP >= ((uint64_t)(&e_li_p->b2) - (uint64_t)header_pP),
                        "DECODING PDU TOO FAR PDU size %d", total_sizeP);
                pdu_info_pP->li_list[pdu_info_pP->num_li] = ((uint16_t)(e_li_p->b1 << 4)) & 0x07F0;
                pdu_info_pP->li_list[pdu_info_pP->num_li] |= (((uint8_t)(e_li_p->b2 >> 4)) & 0x000F);
                li_to_read = e_li_p->b1 & 0x80;
                pdu_info_pP->header_size  += 2;
            } else {
                AssertFatal( total_sizeP >= ((uint64_t)(&e_li_p->b3) - (uint64_t)header_pP),
                        "DECODING PDU TOO FAR PDU size %d", total_sizeP);
                pdu_info_pP->li_list[pdu_info_pP->num_li] = ((uint16_t)(e_li_p->b2 << 8)) & 0x0700;
                pdu_info_pP->li_list[pdu_info_pP->num_li] |=  e_li_p->b3;
                li_to_read = e_li_p->b2 & 0x08;
                e_li_p++;
                pdu_info_pP->header_size  += 1;
            }
            AssertFatal( pdu_info_pP->num_li <= RLC_UM_SEGMENT_NB_MAX_LI_PER_PDU,
                         "[FRAME %05u][RLC_UM][MOD XX][RB XX][GET PDU INFO]  SN %04d TOO MANY LIs ",
                         frameP,
                         pdu_info_pP->sn);

            sum_li += pdu_info_pP->li_list[pdu_info_pP->num_li];
            pdu_info_pP->num_li = pdu_info_pP->num_li + 1;
            if (pdu_info_pP->num_li > RLC_UM_SEGMENT_NB_MAX_LI_PER_PDU) {
                return -2;
            }
        }
        if (li_length_in_bytes  == 2) {
            pdu_info_pP->payload = &e_li_p->b3;
        } else {
            pdu_info_pP->payload = &e_li_p->b1;
        }
    }
    pdu_info_pP->payload_size = total_sizeP - pdu_info_pP->header_size;
    if (pdu_info_pP->payload_size > sum_li) {
        pdu_info_pP->hidden_size = pdu_info_pP->payload_size - sum_li;
    }
    return 0;
}
//-----------------------------------------------------------------------------
int rlc_um_read_length_indicators(unsigned char**data_ppP, rlc_um_e_li_t* e_liP, unsigned int* li_array_pP, unsigned int *num_li_pP, sdu_size_t *data_size_pP) {
//-----------------------------------------------------------------------------
    int          continue_loop = 1;
    unsigned int e1  = 0;
    unsigned int li1 = 0;
    unsigned int e2  = 0;
    unsigned int li2 = 0;
    *num_li_pP = 0;

    while ((continue_loop)) {
        //msg("[RLC_UM] e_liP->b1 = %02X\n", e_liP->b1);
        //msg("[RLC_UM] e_liP->b2 = %02X\n", e_liP->b2);
        e1 = ((unsigned int)e_liP->b1 & 0x00000080) >> 7;
        li1 = (((unsigned int)e_liP->b1 & 0x0000007F) << 4) + (((unsigned int)e_liP->b2 & 0x000000F0) >> 4);
        li_array_pP[*num_li_pP] = li1;
        *data_size_pP = *data_size_pP - li1 - 2;
        *num_li_pP = *num_li_pP +1;
        if ((e1)) {
            e2 = ((unsigned int)e_liP->b2 & 0x00000008) >> 3;
            li2 = (((unsigned int)e_liP->b2 & 0x00000007) << 8) + ((unsigned int)e_liP->b3 & 0x000000FF);
            li_array_pP[*num_li_pP] = li2;
            *data_size_pP = *data_size_pP - li2 - 1;
            *num_li_pP = *num_li_pP +1;
            if (e2 == 0) {
                continue_loop = 0;
            } else {
                e_liP++;
            }
        } else {
            continue_loop = 0;
        }
        if (*num_li_pP >= RLC_UM_SEGMENT_NB_MAX_LI_PER_PDU) {
            return -1;
        }
    }
    *data_ppP = *data_ppP + (((*num_li_pP*3) +1) >> 1);
    return 0;
}
//-----------------------------------------------------------------------------
void rlc_um_try_reassembly(rlc_um_entity_t *rlc_pP, frame_t frameP, eNB_flag_t eNB_flagP, rlc_sn_t start_snP, rlc_sn_t end_snP) {
//-----------------------------------------------------------------------------
    mem_block_t        *pdu_mem_p              = NULL;
    struct mac_tb_ind  *tb_ind_p               = NULL;
    rlc_um_e_li_t      *e_li_p                 = NULL;
    unsigned char      *data_p                 = NULL;
    int                 e                      = 0;
    int                 fi                     = 0;
    sdu_size_t          size                   = 0;
    rlc_sn_t            sn                     = 0;
    unsigned int        continue_reassembly    = 0;
    unsigned int        num_li                 = 0;
    unsigned int        li_array[RLC_UM_SEGMENT_NB_MAX_LI_PER_PDU];
    int                 i                      = 0;
    int                 reassembly_start_index = 0;

    if (end_snP < 0)   end_snP   = end_snP   + rlc_pP->rx_sn_modulo;
    if (start_snP < 0) start_snP = start_snP + rlc_pP->rx_sn_modulo;

#if defined (TRACE_RLC_UM_DAR)
    LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] Line %u TRY REASSEMBLY FROM PDU SN=%03d+1  TO  PDU SN=%03d   SN Length = %d bits\n",
            frameP,
            (rlc_pP->is_enb) ? "eNB" : "UE",
            rlc_pP->enb_module_id,
            rlc_pP->ue_module_id,
            (rlc_pP->is_data_plane) ? "DRB" : "SRB",
            rlc_pP->rb_id,
            __LINE__,
            rlc_pP->last_reassemblied_sn,
            end_snP,
            rlc_pP->rx_sn_length);
#endif
    // nothing to be reassemblied
    if (start_snP == end_snP) {
        return;
    }
    continue_reassembly = 1;
    //sn = (rlc_pP->last_reassemblied_sn + 1) % rlc_pP->rx_sn_modulo;
    sn = start_snP;

    //check_mem_area();

    while (continue_reassembly) {
        if ((pdu_mem_p = rlc_pP->dar_buffer[sn])) {

            if ((rlc_pP->last_reassemblied_sn+1)%rlc_pP->rx_sn_modulo != sn) {
//#if defined (TRACE_RLC_UM_DAR)
                LOG_W(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] Line %u FINDING a HOLE in RLC UM SN: CLEARING OUTPUT SDU BECAUSE NEW SN (%03d) TO REASSEMBLY NOT CONTIGUOUS WITH LAST REASSEMBLIED SN (%03d)\n",
                        frameP,
                        (rlc_pP->is_enb) ? "eNB" : "UE",
                        rlc_pP->enb_module_id,
                        rlc_pP->ue_module_id,
                        (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                        rlc_pP->rb_id,
                        __LINE__,
                        sn,
                        rlc_pP->last_reassemblied_sn);
//#endif
                rlc_um_clear_rx_sdu(rlc_pP);
            }
            rlc_pP->last_reassemblied_sn = sn;
            tb_ind_p = (struct mac_tb_ind *)(pdu_mem_p->data);
            if (rlc_pP->rx_sn_length == 10) {
#if defined (TRACE_RLC_UM_DAR)
                LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] Line %u TRY REASSEMBLY 10 PDU SN=%03d\n",
                        frameP,
                        (rlc_pP->is_enb) ? "eNB" : "UE",
                        rlc_pP->enb_module_id,
                        rlc_pP->ue_module_id,
                        (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                        rlc_pP->rb_id,
                        __LINE__,
                        sn);
#endif
                e  = (((rlc_um_pdu_sn_10_t*)(tb_ind_p->data_ptr))->b1 & 0x04) >> 2;
                fi = (((rlc_um_pdu_sn_10_t*)(tb_ind_p->data_ptr))->b1 & 0x18) >> 3;
                e_li_p = (rlc_um_e_li_t*)((rlc_um_pdu_sn_10_t*)(tb_ind_p->data_ptr))->data;
                size   = tb_ind_p->size - 2;
                data_p = &tb_ind_p->data_ptr[2];
            } else {
#if defined (TRACE_RLC_UM_DAR)
                LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] Line TRY REASSEMBLY 5 PDU SN=%03d Byte 0=%02X\n",
                        frameP,
                        (rlc_pP->is_enb) ? "eNB" : "UE",
                        rlc_pP->enb_module_id,
                        rlc_pP->ue_module_id,
                        (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                        rlc_pP->rb_id,
                        __LINE__,
                        sn,
                        ((rlc_um_pdu_sn_5_t*)(tb_ind_p->data_ptr))->b1);
#endif
                e  = (((rlc_um_pdu_sn_5_t*)(tb_ind_p->data_ptr))->b1 & 0x00000020) >> 5;
                fi = (((rlc_um_pdu_sn_5_t*)(tb_ind_p->data_ptr))->b1 & 0x000000C0) >> 6;
                e_li_p = (rlc_um_e_li_t*)((rlc_um_pdu_sn_5_t*)(tb_ind_p->data_ptr))->data;
                size   = tb_ind_p->size - 1;
                data_p = &tb_ind_p->data_ptr[1];
#if defined (TRACE_RLC_UM_DAR)
                LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] Line %u e=%01X fi=%01X\n",
                        frameP,
                        (rlc_pP->is_enb) ? "eNB" : "UE",
                        rlc_pP->enb_module_id,
                        rlc_pP->ue_module_id,
                        (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                        rlc_pP->rb_id,
                        __LINE__,
                        e,
                        fi);
#endif
            }
            if (e == RLC_E_FIXED_PART_DATA_FIELD_FOLLOW) {
                switch (fi) {
                    case RLC_FI_1ST_BYTE_DATA_IS_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_LAST_BYTE_SDU:
#if defined (TRACE_RLC_UM_DAR)
                        LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] Line %u TRY REASSEMBLY PDU NO E_LI FI=11 (00)\n",
                                frameP,
                                (rlc_pP->is_enb) ? "eNB" : "UE",
                                rlc_pP->enb_module_id,
                                rlc_pP->ue_module_id,
                                (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                                rlc_pP->rb_id,
                                __LINE__);
#endif
                        // one complete SDU
                        //LGrlc_um_send_sdu(rlc_pP,frameP,eNB_flagP); // may be not necessary
                        rlc_um_clear_rx_sdu(rlc_pP);
                        rlc_um_reassembly (data_p, size, rlc_pP,frameP);
                        rlc_um_send_sdu(rlc_pP,frameP,eNB_flagP);
                        rlc_pP->reassembly_missing_sn_detected = 0;

                        break;
                    case RLC_FI_1ST_BYTE_DATA_IS_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_NOT_LAST_BYTE_SDU:
#if defined (TRACE_RLC_UM_DAR)
                        LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] Line %u TRY REASSEMBLY PDU NO E_LI FI=10 (01)\n",
                                frameP,
                                (rlc_pP->is_enb) ? "eNB" : "UE",
                                rlc_pP->enb_module_id,
                                rlc_pP->ue_module_id,
                                (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                                rlc_pP->rb_id,
                                __LINE__);
#endif
                        // one beginning segment of SDU in PDU
                        //LG rlc_um_send_sdu(rlc_pP,frameP,eNB_flagP); // may be not necessary
                        rlc_um_clear_rx_sdu(rlc_pP);
                        rlc_um_reassembly (data_p, size, rlc_pP,frameP);
                        rlc_pP->reassembly_missing_sn_detected = 0;
                        break;
                    case RLC_FI_1ST_BYTE_DATA_IS_NOT_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_LAST_BYTE_SDU:
#if defined (TRACE_RLC_UM_DAR)
                        LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] Line %u TRY REASSEMBLY PDU NO E_LI FI=01 (10)\n",
                                frameP,
                                (rlc_pP->is_enb) ? "eNB" : "UE",
                                rlc_pP->enb_module_id,
                                rlc_pP->ue_module_id,
                                (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                                rlc_pP->rb_id,
                                __LINE__);
#endif
                        // one last segment of SDU
                        if (rlc_pP->reassembly_missing_sn_detected == 0) {
                            rlc_um_reassembly (data_p, size, rlc_pP,frameP);
                            rlc_um_send_sdu(rlc_pP,frameP,eNB_flagP);
                        } else {
                            //clear sdu already done
                            rlc_pP->stat_rx_data_pdu_dropped += 1;
                            rlc_pP->stat_rx_data_bytes_dropped += tb_ind_p->size;
                        }
                        rlc_pP->reassembly_missing_sn_detected = 0;
                        break;
                    case RLC_FI_1ST_BYTE_DATA_IS_NOT_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_NOT_LAST_BYTE_SDU:
#if defined (TRACE_RLC_UM_DAR)
                        LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] Line %u TRY REASSEMBLY PDU NO E_LI FI=00 (11)\n",
                                frameP,
                                (rlc_pP->is_enb) ? "eNB" : "UE",
                                rlc_pP->enb_module_id,
                                rlc_pP->ue_module_id,
                                (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                                rlc_pP->rb_id,
                                __LINE__);
#endif
                        if (rlc_pP->reassembly_missing_sn_detected == 0) {
                            // one whole segment of SDU in PDU
                            rlc_um_reassembly (data_p, size, rlc_pP,frameP);
                        } else {
//#if defined (TRACE_RLC_UM_DAR)
                            LOG_W(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] Line %u TRY REASSEMBLY PDU NO E_LI FI=00 (11) MISSING SN DETECTED\n",
                                    frameP,
                                    (rlc_pP->is_enb) ? "eNB" : "UE",
                                    rlc_pP->enb_module_id,
                                    rlc_pP->ue_module_id,
                                    (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                                    rlc_pP->rb_id,
                                    __LINE__);
//#endif
                            //LOG_D(RLC, "[MSC_NBOX][FRAME %05u][%s][RLC_UM][MOD %u/%u][RB %u][Missing SN detected][RLC_UM][MOD %u/%u][RB %u]\n",
                            //      frameP, rlc_pP->module_id,rlc_pP->rb_id, rlc_pP->module_id,rlc_pP->rb_id);
                            rlc_pP->reassembly_missing_sn_detected = 1; // not necessary but for readability of the code
                            rlc_pP->stat_rx_data_pdu_dropped += 1;
                            rlc_pP->stat_rx_data_bytes_dropped += tb_ind_p->size;
#if defined(RLC_STOP_ON_LOST_PDU)
                            AssertFatal( rlc_pP->reassembly_missing_sn_detected == 1,
                                    "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] MISSING PDU DETECTED\n",
                                    frameP,
                                    (rlc_pP->is_enb) ? "eNB" : "UE",
                                    rlc_pP->enb_module_id,
                                    rlc_pP->ue_module_id,
                                    (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                                    rlc_pP->rb_id);
#endif
                        }

                        break;
                    default:
                        AssertFatal( 0 , "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] TRY REASSEMBLY SHOULD NOT GO HERE\n",
                                frameP,
                                (rlc_pP->is_enb) ? "eNB" : "UE",
                                rlc_pP->enb_module_id,
                                rlc_pP->ue_module_id,
                                (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                                rlc_pP->rb_id);
                }
            } else {
                if (rlc_um_read_length_indicators(&data_p, e_li_p, li_array, &num_li, &size ) >= 0) {
                    switch (fi) {
                        case RLC_FI_1ST_BYTE_DATA_IS_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_LAST_BYTE_SDU:
#if defined (TRACE_RLC_UM_DAR)
                            LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] Line %u TRY REASSEMBLY PDU FI=11 (00) Li=",
                                    frameP,
                                    (rlc_pP->is_enb) ? "eNB" : "UE",
                                    rlc_pP->enb_module_id,
                                    rlc_pP->ue_module_id,
                                    (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                                    rlc_pP->rb_id,
                                    __LINE__);

                            for (i=0; i < num_li; i++) {
                                LOG_D(RLC, "%d ",li_array[i]);
                            }
                            LOG_D(RLC, " remaining size %d\n",size);
#endif
                            // N complete SDUs
                            //LGrlc_um_send_sdu(rlc_pP,frameP,eNB_flagP);
                            rlc_um_clear_rx_sdu(rlc_pP);

                            for (i = 0; i < num_li; i++) {
                                rlc_um_reassembly (data_p, li_array[i], rlc_pP,frameP);
                                rlc_um_send_sdu(rlc_pP,frameP,eNB_flagP);
                                data_p = &data_p[li_array[i]];
                            }
                            if (size > 0) { // normally should always be > 0 but just for help debug
                                // data_p is already ok, done by last loop above
                                rlc_um_reassembly (data_p, size, rlc_pP,frameP);
                                rlc_um_send_sdu(rlc_pP,frameP,eNB_flagP);
                            }
                            rlc_pP->reassembly_missing_sn_detected = 0;
                            break;
                        case RLC_FI_1ST_BYTE_DATA_IS_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_NOT_LAST_BYTE_SDU:
#if defined (TRACE_RLC_UM_DAR)
                            LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] Line %u TRY REASSEMBLY PDU FI=10 (01) Li=",
                                    frameP,
                                    (rlc_pP->is_enb) ? "eNB" : "UE",
                                    rlc_pP->enb_module_id,
                                    rlc_pP->ue_module_id,
                                    (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                                    rlc_pP->rb_id,
                                    __LINE__);
                            for (i=0; i < num_li; i++) {
                                LOG_D(RLC, "%d ",li_array[i]);
                            }
                            LOG_D(RLC, " remaining size %d\n",size);
#endif
                            // N complete SDUs + one segment of SDU in PDU
                            //LG rlc_um_send_sdu(rlc_pP,frameP,eNB_flagP);
                            rlc_um_clear_rx_sdu(rlc_pP);
                            for (i = 0; i < num_li; i++) {
                                rlc_um_reassembly (data_p, li_array[i], rlc_pP,frameP);
                                rlc_um_send_sdu(rlc_pP,frameP,eNB_flagP);
                                data_p = &data_p[li_array[i]];
                            }
                            if (size > 0) { // normally should always be > 0 but just for help debug
                                // data_p is already ok, done by last loop above
                                rlc_um_reassembly (data_p, size, rlc_pP,frameP);
                            }
                            rlc_pP->reassembly_missing_sn_detected = 0;
                            break;
                        case RLC_FI_1ST_BYTE_DATA_IS_NOT_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_LAST_BYTE_SDU:
#if defined (TRACE_RLC_UM_DAR)
                            LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] Line %u TRY REASSEMBLY PDU FI=01 (10) Li=",
                                    frameP,
                                    (rlc_pP->is_enb) ? "eNB" : "UE",
                                    rlc_pP->enb_module_id,
                                    rlc_pP->ue_module_id,
                                    (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                                    rlc_pP->rb_id,
                                    __LINE__);
                            for (i=0; i < num_li; i++) {
                                LOG_D(RLC, "%d ",li_array[i]);
                            }
                            LOG_D(RLC, " remaining size %d\n",size);
#endif
                            if (rlc_pP->reassembly_missing_sn_detected) {
                                reassembly_start_index = 1;
                                data_p = &data_p[li_array[0]];
                                //rlc_pP->stat_rx_data_pdu_dropped += 1;
                                rlc_pP->stat_rx_data_bytes_dropped += li_array[0];
                            } else {
                                reassembly_start_index = 0;
                            }

                            // one last segment of SDU + N complete SDUs in PDU
                            for (i = reassembly_start_index; i < num_li; i++) {
                                rlc_um_reassembly (data_p, li_array[i], rlc_pP,frameP);
                                rlc_um_send_sdu(rlc_pP,frameP,eNB_flagP);
                                data_p = &data_p[li_array[i]];
                            }
                            if (size > 0) { // normally should always be > 0 but just for help debug
                                // data_p is already ok, done by last loop above
                                rlc_um_reassembly (data_p, size, rlc_pP,frameP);
                                rlc_um_send_sdu(rlc_pP,frameP,eNB_flagP);
                            }
                            rlc_pP->reassembly_missing_sn_detected = 0;
                            break;
                        case RLC_FI_1ST_BYTE_DATA_IS_NOT_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_NOT_LAST_BYTE_SDU:
#if defined (TRACE_RLC_UM_DAR)
                            LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] Line %u TRY REASSEMBLY PDU FI=00 (11) Li=",
                                    frameP,
                                    (rlc_pP->is_enb) ? "eNB" : "UE",
                                    rlc_pP->enb_module_id,
                                    rlc_pP->ue_module_id,
                                    (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                                    rlc_pP->rb_id,
                                    __LINE__);
                            for (i=0; i < num_li; i++) {
                                LOG_D(RLC, "%d ",li_array[i]);
                            }
                            LOG_D(RLC, " remaining size %d\n",size);
#endif
                            if (rlc_pP->reassembly_missing_sn_detected) {
//#if defined (TRACE_RLC_UM_DAR)
                                LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] Line %u DISCARD FIRST LI %d",
                                        frameP,
                                        (rlc_pP->is_enb) ? "eNB" : "UE",
                                        rlc_pP->enb_module_id,
                                        rlc_pP->ue_module_id,
                                        (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                                        rlc_pP->rb_id,
                                        __LINE__,
                                        li_array[0]);
//#endif
                                reassembly_start_index = 1;
                                data_p = &data_p[li_array[0]];
                            	//rlc_pP->stat_rx_data_pdu_dropped += 1;
                            	rlc_pP->stat_rx_data_bytes_dropped += li_array[0];
                            } else {
                                reassembly_start_index = 0;
                            }

                            for (i = reassembly_start_index; i < num_li; i++) {
                                rlc_um_reassembly (data_p, li_array[i], rlc_pP,frameP);
                                rlc_um_send_sdu(rlc_pP,frameP,eNB_flagP);
                                data_p = &data_p[li_array[i]];
                            }
                            if (size > 0) { // normally should always be > 0 but just for help debug
                                // data_p is already ok, done by last loop above
                                rlc_um_reassembly (data_p, size, rlc_pP,frameP);
                            } else {
                                AssertFatal( 0 !=0, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] SHOULD NOT GO HERE\n",
                                        frameP,
                                        (rlc_pP->is_enb) ? "eNB" : "UE",
                                        rlc_pP->enb_module_id,
                                        rlc_pP->ue_module_id,
                                        (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                                        rlc_pP->rb_id);
                                //rlc_pP->stat_rx_data_pdu_dropped += 1;
                                rlc_pP->stat_rx_data_bytes_dropped += size;
                            }
                            rlc_pP->reassembly_missing_sn_detected = 0;
                            break;
                        default:
//#if defined (TRACE_RLC_UM_DAR)
                            LOG_W(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] Line %u Missing SN detected\n",
                                    frameP,
                                    (rlc_pP->is_enb) ? "eNB" : "UE",
                                    rlc_pP->enb_module_id,
                                    rlc_pP->ue_module_id,
                                    (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                                    rlc_pP->rb_id,
                                    __LINE__);
//#endif
                            rlc_pP->stat_rx_data_pdu_dropped += 1;
                            rlc_pP->stat_rx_data_bytes_dropped += tb_ind_p->size;

                            rlc_pP->reassembly_missing_sn_detected = 1;
#if defined(RLC_STOP_ON_LOST_PDU)
                            AssertFatal( rlc_pP->reassembly_missing_sn_detected == 1,
                                    "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] MISSING PDU DETECTED\n",
                                    frameP,
                                    (rlc_pP->is_enb) ? "eNB" : "UE",
                                    rlc_pP->enb_module_id,
                                    rlc_pP->ue_module_id,
                                    (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                                    rlc_pP->rb_id);
#endif
                    }
                }
            }
#if defined (TRACE_RLC_UM_DAR)
            LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] Line %u REMOVE PDU FROM DAR BUFFER  SN=%03d\n",
                    frameP,
                    (rlc_pP->is_enb) ? "eNB" : "UE",
                    rlc_pP->enb_module_id,
                    rlc_pP->ue_module_id,
                    (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                    rlc_pP->rb_id,
                    __LINE__,
                    sn);
#endif
            free_mem_block(rlc_pP->dar_buffer[sn]);
            rlc_pP->dar_buffer[sn] = NULL;
        } else {
            rlc_pP->last_reassemblied_missing_sn = sn;
//#if defined (TRACE_RLC_UM_DAR)
            LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] Line %u Missing SN %04d detected, clearing RX SDU\n",
                    frameP,
                    (rlc_pP->is_enb) ? "eNB" : "UE",
                    rlc_pP->enb_module_id,
                    rlc_pP->ue_module_id,
                    (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                    rlc_pP->rb_id,
                    __LINE__,
                    sn);
//#endif
            rlc_pP->reassembly_missing_sn_detected = 1;
            rlc_um_clear_rx_sdu(rlc_pP);
#if defined(RLC_STOP_ON_LOST_PDU)
            AssertFatal( rlc_pP->reassembly_missing_sn_detected == 1,
                    "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] MISSING PDU DETECTED\n",
                    frameP,
                    (rlc_pP->is_enb) ? "eNB" : "UE",
                    rlc_pP->enb_module_id,
                    rlc_pP->ue_module_id,
                    (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                    rlc_pP->rb_id);
#endif
        }
        sn = (sn + 1) % rlc_pP->rx_sn_modulo;
        if ((sn == rlc_pP->vr_uh) || (sn == end_snP)){
            continue_reassembly = 0;
        }
    }
#if defined (TRACE_RLC_UM_DAR)
    LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] Line %u TRIED REASSEMBLY VR(UR)=%03d VR(UX)=%03d VR(UH)=%03d\n",
            frameP,
            (rlc_pP->is_enb) ? "eNB" : "UE",
            rlc_pP->enb_module_id,
            rlc_pP->ue_module_id,
            (rlc_pP->is_data_plane) ? "DRB" : "SRB",
            rlc_pP->rb_id,
            __LINE__,
            rlc_pP->vr_ur,
            rlc_pP->vr_ux,
            rlc_pP->vr_uh);
#endif

}
//-----------------------------------------------------------------------------
void rlc_um_stop_and_reset_timer_reordering(rlc_um_entity_t *rlc_pP,frame_t frameP)
//-----------------------------------------------------------------------------
{
#if defined (TRACE_RLC_UM_DAR)
    LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u][T-REORDERING] STOPPED AND RESET\n",
            frameP,
            (rlc_pP->is_enb) ? "eNB" : "UE",
            rlc_pP->enb_module_id,
            rlc_pP->ue_module_id,
            (rlc_pP->is_data_plane) ? "DRB" : "SRB",
            rlc_pP->rb_id);
#endif
    rlc_pP->t_reordering.running         = 0;
    rlc_pP->t_reordering.frame_time_out  = 0;
    rlc_pP->t_reordering.frame_start     = 0;
    rlc_pP->t_reordering.timed_out       = 0;
}
//-----------------------------------------------------------------------------
void rlc_um_start_timer_reordering(rlc_um_entity_t *rlc_pP,frame_t frameP)
//-----------------------------------------------------------------------------
{
    rlc_pP->t_reordering.running         = 1;
    rlc_pP->t_reordering.frame_time_out  = frameP + rlc_pP->t_reordering.time_out;
    rlc_pP->t_reordering.frame_start     = frameP;
    rlc_pP->t_reordering.timed_out       = 0;
#if defined (TRACE_RLC_UM_DAR)
    LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u][T-REORDERING] STARTED (TIME-OUT = FRAME %05u)\n",
            frameP,
            (rlc_pP->is_enb) ? "eNB" : "UE",
            rlc_pP->enb_module_id,
            rlc_pP->ue_module_id,
            (rlc_pP->is_data_plane) ? "DRB" : "SRB",
            rlc_pP->rb_id,
            rlc_pP->t_reordering.frame_time_out);
#endif
}
//-----------------------------------------------------------------------------
void rlc_um_init_timer_reordering(rlc_um_entity_t *rlc_pP, uint32_t time_outP)
//-----------------------------------------------------------------------------
{
    rlc_pP->t_reordering.running         = 0;
    rlc_pP->t_reordering.frame_time_out  = 0;
    rlc_pP->t_reordering.frame_start     = 0;
    rlc_pP->t_reordering.time_out        = time_outP;
    rlc_pP->t_reordering.timed_out       = 0;
}
//-----------------------------------------------------------------------------
void rlc_um_check_timer_dar_time_out(rlc_um_entity_t *rlc_pP, frame_t frameP, eNB_flag_t eNB_flagP) {
//-----------------------------------------------------------------------------
    signed int     in_window;
    rlc_usn_t      old_vr_ur;
    if ((rlc_pP->t_reordering.running)) {
        if (
        // CASE 1:          start              time out
        //        +-----------+------------------+----------+
        //        |           |******************|          |
        //        +-----------+------------------+----------+
        //FRAME # 0                                     FRAME MAX
        ((rlc_pP->t_reordering.frame_start < rlc_pP->t_reordering.frame_time_out) &&
            ((frameP >= rlc_pP->t_reordering.frame_time_out) ||
             (frameP < rlc_pP->t_reordering.frame_start)))                                   ||
        // CASE 2:        time out            start
        //        +-----------+------------------+----------+
        //        |***********|                  |**********|
        //        +-----------+------------------+----------+
        //FRAME # 0                                     FRAME MAX VALUE
        ((rlc_pP->t_reordering.frame_start > rlc_pP->t_reordering.frame_time_out) &&
           (frameP < rlc_pP->t_reordering.frame_start) && (frameP >= rlc_pP->t_reordering.frame_time_out))
        ) {

        //if ((uint32_t)((uint32_t)rlc_pP->timer_reordering  + (uint32_t)rlc_pP->timer_reordering_init)   <= frameP) {
            // 5.1.2.2.4   Actions when t-Reordering expires
            //  When t-Reordering expires, the receiving UM RLC entity shall:
            //  -update VR(UR) to the SN of the first UMD PDU with SN >= VR(UX) that has not been received;
            //  -reassemble RLC SDUs from any UMD PDUs with SN < updated VR(UR), remove RLC headers when doing so and deliver the reassembled RLC SDUs to upper layer in ascending order of the RLC SN if not delivered before;
            //  -if VR(UH) > VR(UR):
            //      -start t-Reordering;
            //      -set VR(UX) to VR(UH).
        	rlc_pP->stat_timer_reordering_timed_out += 1;
#if defined (TRACE_RLC_UM_DAR)
            LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u]*****************************************************\n",
                    frameP,
                    (rlc_pP->is_enb) ? "eNB" : "UE",
                    rlc_pP->enb_module_id,
                    rlc_pP->ue_module_id,
                    (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                    rlc_pP->rb_id);
            LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u]*    T I M E  -  O U T                              *\n",
                    frameP,
                    (rlc_pP->is_enb) ? "eNB" : "UE",
                    rlc_pP->enb_module_id,
                    rlc_pP->ue_module_id,
                    (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                    rlc_pP->rb_id);
            LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u]*****************************************************\n",
                    frameP,
                    (rlc_pP->is_enb) ? "eNB" : "UE",
                    rlc_pP->enb_module_id,
                    rlc_pP->ue_module_id,
                    (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                    rlc_pP->rb_id);
            LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] TIMER t-Reordering expiration\n",
                    frameP,
                    (rlc_pP->is_enb) ? "eNB" : "UE",
                    rlc_pP->enb_module_id,
                    rlc_pP->ue_module_id,
                    (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                    rlc_pP->rb_id);
            LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] timer_reordering=%d frameP=%d expire frameP %d\n",
                    frameP,
                    (rlc_pP->is_enb) ? "eNB" : "UE",
                    rlc_pP->enb_module_id,
                    rlc_pP->ue_module_id,
                    (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                    rlc_pP->rb_id,
                    rlc_pP->t_reordering.time_out,
                    rlc_pP->t_reordering.frame_time_out,
                    frameP);
            LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] set VR(UR)=%03d to",
                    frameP,
                    (rlc_pP->is_enb) ? "eNB" : "UE",
                    rlc_pP->enb_module_id,
                    rlc_pP->ue_module_id,
                    (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                    rlc_pP->rb_id,
                    rlc_pP->vr_ur);
#endif

            old_vr_ur   = rlc_pP->vr_ur;

            rlc_pP->vr_ur = rlc_pP->vr_ux;
            while (rlc_um_get_pdu_from_dar_buffer(rlc_pP, rlc_pP->vr_ur)) {
                rlc_pP->vr_ur = (rlc_pP->vr_ur+1)%rlc_pP->rx_sn_modulo;
            }
            LOG_D(RLC, " %d", rlc_pP->vr_ur);
            LOG_D(RLC, "\n");

            rlc_um_try_reassembly(rlc_pP,frameP,eNB_flagP,old_vr_ur, rlc_pP->vr_ur);

            in_window = rlc_um_in_window(rlc_pP, frameP, rlc_pP->vr_ur,  rlc_pP->vr_uh,  rlc_pP->vr_uh);
            if (in_window == 2) {
            	rlc_um_start_timer_reordering(rlc_pP, frameP);
                rlc_pP->vr_ux = rlc_pP->vr_uh;
#if defined (TRACE_RLC_UM_DAR)
                LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] restarting t-Reordering set VR(UX) to %d (VR(UH)>VR(UR))\n",
                        frameP,
                        (rlc_pP->is_enb) ? "eNB" : "UE",
                        rlc_pP->enb_module_id,
                        rlc_pP->ue_module_id,
                        (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                        rlc_pP->rb_id,
                        rlc_pP->vr_ux);
#endif
            } else {
#if defined (TRACE_RLC_UM_DAR)
                LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] STOP t-Reordering VR(UX) = %03d\n",
                        frameP,
                        (rlc_pP->is_enb) ? "eNB" : "UE",
                        rlc_pP->enb_module_id,
                        rlc_pP->ue_module_id,
                        (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                        rlc_pP->rb_id,
                        rlc_pP->vr_ux);
#endif
                rlc_um_stop_and_reset_timer_reordering(rlc_pP, frameP);
            }
        }
    }
}
//-----------------------------------------------------------------------------
inline mem_block_t *
rlc_um_remove_pdu_from_dar_buffer(rlc_um_entity_t *rlc_pP, rlc_usn_t snP)
{
//-----------------------------------------------------------------------------
    mem_block_t * pdu_p     = rlc_pP->dar_buffer[snP];
#if defined (TRACE_RLC_UM_DAR)
    LOG_D(RLC, "[FRAME ?????][%s][RLC_UM][MOD %u/%u][%s %u] REMOVE PDU FROM DAR BUFFER  SN=%03d\n",
            (rlc_pP->is_enb) ? "eNB" : "UE",
            rlc_pP->enb_module_id,
            rlc_pP->ue_module_id,
            (rlc_pP->is_data_plane) ? "DRB" : "SRB",
            rlc_pP->rb_id,
            snP);
#endif
    rlc_pP->dar_buffer[snP] = NULL;
    return pdu_p;
}
//-----------------------------------------------------------------------------
inline mem_block_t *
rlc_um_get_pdu_from_dar_buffer(rlc_um_entity_t *rlc_pP, rlc_usn_t snP)
{
//-----------------------------------------------------------------------------
    return rlc_pP->dar_buffer[snP];
}
//-----------------------------------------------------------------------------
inline void
rlc_um_store_pdu_in_dar_buffer(rlc_um_entity_t *rlc_pP, frame_t frameP, mem_block_t *pdu_pP, rlc_usn_t snP)
{
//-----------------------------------------------------------------------------
#if defined (TRACE_RLC_UM_DAR)
    LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] STORE PDU IN DAR BUFFER  SN=%03d  VR(UR)=%03d VR(UX)=%03d VR(UH)=%03d\n",
            frameP,
            (rlc_pP->is_enb) ? "eNB" : "UE",
            rlc_pP->enb_module_id,
            rlc_pP->ue_module_id,
            (rlc_pP->is_data_plane) ? "DRB" : "SRB",
            rlc_pP->rb_id,
            snP,
            rlc_pP->vr_ur,
            rlc_pP->vr_ux,
            rlc_pP->vr_uh);
#endif
    rlc_pP->dar_buffer[snP] = pdu_pP;
}
//-----------------------------------------------------------------------------
// returns -2 if lower_bound  > sn
// returns -1 if higher_bound < sn
// returns  0 if lower_bound  < sn < higher_bound
// returns  1 if lower_bound  == sn
// returns  2 if higher_bound == sn
// returns  3 if higher_bound == sn == lower_bound
inline signed int rlc_um_in_window(rlc_um_entity_t *rlc_pP, frame_t frameP, rlc_sn_t lower_boundP, rlc_sn_t snP, rlc_sn_t higher_boundP) {
//-----------------------------------------------------------------------------

    rlc_sn_t modulus = (rlc_sn_t)rlc_pP->vr_uh - rlc_pP->rx_um_window_size;
#ifdef TRACE_RLC_UM_RX
    rlc_sn_t     lower_bound  = lower_boundP;
    rlc_sn_t     higher_bound = higher_boundP;
    rlc_sn_t     sn           = snP;
#endif
    lower_boundP  = (lower_boundP  - modulus) % rlc_pP->rx_sn_modulo;
    higher_boundP = (higher_boundP - modulus) % rlc_pP->rx_sn_modulo;
    snP           = (snP           - modulus) % rlc_pP->rx_sn_modulo;

    if ( lower_boundP > snP) {
#ifdef TRACE_RLC_UM_RX
        LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] %d not in WINDOW[%03d:%03d] (SN<LOWER BOUND)\n",
                frameP,
                (rlc_pP->is_enb) ? "eNB" : "UE",
                rlc_pP->enb_module_id,
                rlc_pP->ue_module_id,
                (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                rlc_pP->rb_id,
                sn,
                lower_bound,
                higher_bound);
#endif
        return -2;
    }
    if ( higher_boundP < snP) {
#ifdef TRACE_RLC_UM_RX
        LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] %d not in WINDOW[%03d:%03d] (SN>HIGHER BOUND) <=> %d not in WINDOW[%03d:%03d]\n",
                frameP,
                (rlc_pP->is_enb) ? "eNB" : "UE",
                rlc_pP->enb_module_id,
                rlc_pP->ue_module_id,
                (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                rlc_pP->rb_id,
                sn,
                lower_bound,
                higher_bound,
                snP,
                lower_boundP,
                higher_boundP);
#endif
        return -1;
    }
    if ( lower_boundP == snP) {
        if ( higher_boundP == snP) {
#ifdef TRACE_RLC_UM_RX
        LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] %d  in WINDOW[%03d:%03d] (SN=HIGHER BOUND=LOWER BOUND)\n",
                frameP,
                (rlc_pP->is_enb) ? "eNB" : "UE",
                rlc_pP->enb_module_id,
                rlc_pP->ue_module_id,
                (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                rlc_pP->rb_id,
                sn,
                lower_bound,
                higher_bound);
#endif
            return 3;
        }
#ifdef TRACE_RLC_UM_RX
        LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] %d  in WINDOW[%03d:%03d] (SN=LOWER BOUND)\n",
                frameP,
                (rlc_pP->is_enb) ? "eNB" : "UE",
                rlc_pP->enb_module_id,
                rlc_pP->ue_module_id,
                (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                rlc_pP->rb_id,
                sn,
                lower_bound,
                higher_bound);
#endif
        return 1;
    }
    if ( higher_boundP == snP) {
#ifdef TRACE_RLC_UM_RX
        LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] %d  in WINDOW[%03d:%03d] (SN=HIGHER BOUND)\n",
                frameP,
                (rlc_pP->is_enb) ? "eNB" : "UE",
                rlc_pP->enb_module_id,
                rlc_pP->ue_module_id,
                (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                rlc_pP->rb_id,
                sn,
                lower_bound,
                higher_bound);
#endif
        return 2;
    }
    return 0;

}
//-----------------------------------------------------------------------------
inline signed int rlc_um_in_reordering_window(rlc_um_entity_t *rlc_pP, frame_t frameP, rlc_sn_t snP) {
//-----------------------------------------------------------------------------
    rlc_sn_t   modulus = (signed int)rlc_pP->vr_uh - rlc_pP->rx_um_window_size;
    rlc_sn_t   sn = snP;
    snP           = (snP - modulus) % rlc_pP->rx_sn_modulo;

    if ( 0 <= snP) {
        if (snP < rlc_pP->rx_um_window_size) {
#if defined (TRACE_RLC_UM_DAR)
           LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] %d IN REORDERING WINDOW[%03d:%03d[ SN %d IN [%03d:%03d[ VR(UR)=%03d VR(UH)=%03d\n",
                   frameP,
                   (rlc_pP->is_enb) ? "eNB" : "UE",
                   rlc_pP->enb_module_id,
                   rlc_pP->ue_module_id,
                   (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                   rlc_pP->rb_id,
                   snP,
                   0,
                   rlc_pP->rx_um_window_size,
                   sn,
                   (signed int)rlc_pP->vr_uh - rlc_pP->rx_um_window_size,
                   rlc_pP->vr_uh,
                   rlc_pP->vr_ur,
                   rlc_pP->vr_uh);
#endif
            return 0;
        }
    }
#if defined (TRACE_RLC_UM_DAR)
    if (modulus < 0) {
        LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] %d NOT IN REORDERING WINDOW[%03d:%03d[ SN %d NOT IN [%03d:%03d[ VR(UR)=%03d VR(UH)=%03d\n",
                frameP,
                (rlc_pP->is_enb) ? "eNB" : "UE",
                rlc_pP->enb_module_id,
                rlc_pP->ue_module_id,
                (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                rlc_pP->rb_id,
                snP,
                modulus + 1024,
                rlc_pP->rx_um_window_size,
                sn,
                modulus + 1024 ,
                rlc_pP->vr_uh,
                rlc_pP->vr_ur,
                rlc_pP->vr_uh);
    } else {
        LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] %d NOT IN REORDERING WINDOW[%03d:%03d[ SN %d NOT IN [%03d:%03d[ VR(UR)=%03d VR(UH)=%03d\n",
                frameP,
                (rlc_pP->is_enb) ? "eNB" : "UE",
                rlc_pP->enb_module_id,
                rlc_pP->ue_module_id,
                (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                rlc_pP->rb_id,
                snP,
                modulus,
                rlc_pP->rx_um_window_size,
                sn,
                modulus ,
                rlc_pP->vr_uh,
                rlc_pP->vr_ur,
                rlc_pP->vr_uh);
    }
#endif
    return -1;
}
//-----------------------------------------------------------------------------
void
rlc_um_receive_process_dar (rlc_um_entity_t *rlc_pP, frame_t frameP, eNB_flag_t eNB_flagP, mem_block_t *pdu_mem_pP,rlc_um_pdu_sn_10_t *pdu_pP, sdu_size_t tb_sizeP)
{
//-----------------------------------------------------------------------------
    // 36.322v9.3.0 section 5.1.2.2.1:
    // The receiving UM RLC entity shall maintain a reordering window according to state variable VR(UH) as follows:
    //      -a SN falls within the reordering window if (VR(UH) – UM_Window_Size) <= SN < VR(UH);
    //      -a SN falls outside of the reordering window otherwise.
    // When receiving an UMD PDU from lower layer, the receiving UM RLC entity shall:
    //      -either discard the received UMD PDU or place it in the reception buffer (see sub clause 5.1.2.2.2);
    //      -if the received UMD PDU was placed in the reception buffer:
    //          -update state variables, reassemble and deliver RLC SDUs to upper layer and start/stop t-Reordering as needed (see sub clause 5.1.2.2.3);
    // When t-Reordering expires, the receiving UM RLC entity shall:
    // -   update state variables, reassemble and deliver RLC SDUs to upper layer and start t-Reordering as needed (see sub clause 5.1.2.2.4).



    // When an UMD PDU with SN = x is received from lower layer, the receiving UM RLC entity shall:
    // -if VR(UR) < x < VR(UH) and the UMD PDU with SN = x has been received before; or
    // -if (VR(UH) – UM_Window_Size) <= x < VR(UR):
    //      -discard the received UMD PDU;
    // -else:
    //      -place the received UMD PDU in the reception buffer.

    rlc_sn_t sn = -1;
    signed int in_window;

    if (rlc_pP->rx_sn_length == 10) {
        sn = ((pdu_pP->b1 & 0x00000003) << 8) + pdu_pP->b2;
    } else if (rlc_pP->rx_sn_length == 5) {
        sn = pdu_pP->b1 & 0x1F;
    } else {
    	free_mem_block(pdu_mem_pP);
    }
    in_window = rlc_um_in_window(rlc_pP, frameP, rlc_pP->vr_uh - rlc_pP->rx_um_window_size, sn, rlc_pP->vr_ur);

    rlc_util_print_hex_octets(RLC, &pdu_pP->b1, tb_sizeP);

    // rlc_um_in_window() returns -2 if lower_bound  > sn
    // rlc_um_in_window() returns -1 if higher_bound < sn
    // rlc_um_in_window() returns  0 if lower_bound  < sn < higher_bound
    // rlc_um_in_window() returns  1 if lower_bound  == sn
    // rlc_um_in_window() returns  2 if higher_bound == sn
    // rlc_um_in_window() returns  3 if higher_bound == sn == lower_bound
    if ((in_window == 1) || (in_window == 0)){
#if defined (TRACE_RLC_UM_DAR)
        LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] RX PDU  VR(UH) – UM_Window_Size) <= SN %d < VR(UR) -> GARBAGE\n",
                frameP,
                (rlc_pP->is_enb) ? "eNB" : "UE",
                rlc_pP->enb_module_id,
                rlc_pP->ue_module_id,
                (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                rlc_pP->rb_id,
                sn);
#endif
        rlc_pP->stat_rx_data_pdu_out_of_window   += 1;
        rlc_pP->stat_rx_data_bytes_out_of_window += tb_sizeP;
        free_mem_block(pdu_mem_pP);
        pdu_mem_pP = NULL;
        return;
    }
    if ((rlc_um_get_pdu_from_dar_buffer(rlc_pP, sn))) {
        in_window = rlc_um_in_window(rlc_pP, frameP, rlc_pP->vr_ur, sn, rlc_pP->vr_uh);
        if (in_window == 0){
#if defined (TRACE_RLC_UM_DAR)
            LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] RX PDU  VR(UR) < SN %d < VR(UH) and RECEIVED BEFORE-> GARBAGE\n",
                    frameP,
                    (rlc_pP->is_enb) ? "eNB" : "UE",
                    rlc_pP->enb_module_id,
                    rlc_pP->ue_module_id,
                    (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                    rlc_pP->rb_id,
                    sn);
#endif
            //discard the PDU
            rlc_pP->stat_rx_data_pdus_duplicate  += 1;
            rlc_pP->stat_rx_data_bytes_duplicate += tb_sizeP;
            free_mem_block(pdu_mem_pP);
            pdu_mem_pP = NULL;
            return;
        }
        // 2 lines to avoid memory leaks
        rlc_pP->stat_rx_data_pdus_duplicate  += 1;
        rlc_pP->stat_rx_data_bytes_duplicate += tb_sizeP;
#if defined (TRACE_RLC_UM_DAR)
        LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] RX PDU SN %03d REMOVE OLD PDU BEFORE STORING NEW PDU\n",
                frameP,
                (rlc_pP->is_enb) ? "eNB" : "UE",
                rlc_pP->enb_module_id,
                rlc_pP->ue_module_id,
                (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                rlc_pP->rb_id,
                sn);
#endif
        mem_block_t *pdu = rlc_um_remove_pdu_from_dar_buffer(rlc_pP, sn);
        free_mem_block(pdu);
    }
    rlc_um_store_pdu_in_dar_buffer(rlc_pP, frameP, pdu_mem_pP, sn);


    // -if x falls outside of the reordering window:
    //      -update VR(UH) to x + 1;
    //      -reassemble RLC SDUs from any UMD PDUs with SN that falls outside of
    //       the reordering window, remove RLC headers when doing so and deliver
    //       the reassembled RLC SDUs to upper layer in ascending order of the
    //       RLC SN if not delivered before;
    //
    //      -if VR(UR) falls outside of the reordering window:
    //          -set VR(UR) to (VR(UH) – UM_Window_Size);
    if (rlc_um_in_reordering_window(rlc_pP, frameP, sn) < 0) {
#if defined (TRACE_RLC_UM_DAR)
        LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] RX PDU  SN %d OUTSIDE REORDERING WINDOW VR(UH)=%d UM_Window_Size=%d\n",
                frameP,
                (rlc_pP->is_enb) ? "eNB" : "UE",
                rlc_pP->enb_module_id,
                rlc_pP->ue_module_id,
                (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                rlc_pP->rb_id,
                sn,
                rlc_pP->vr_uh,
                rlc_pP->rx_um_window_size);
#endif
        rlc_pP->vr_uh = (sn + 1) % rlc_pP->rx_sn_modulo;

        if (rlc_um_in_reordering_window(rlc_pP, frameP, rlc_pP->vr_ur) != 0) {
            in_window = rlc_pP->vr_uh - rlc_pP->rx_um_window_size;
            if (in_window < 0) {
                in_window = in_window + rlc_pP->rx_sn_modulo;
            }

            rlc_um_try_reassembly(rlc_pP, frameP, eNB_flagP, rlc_pP->vr_ur, in_window);
        }


        if (rlc_um_in_reordering_window(rlc_pP, frameP, rlc_pP->vr_ur) < 0) {
#if defined (TRACE_RLC_UM_DAR)
            LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] VR(UR) %d OUTSIDE REORDERING WINDOW SET TO VR(UH) – UM_Window_Size = %d\n",
                    frameP,
                    (rlc_pP->is_enb) ? "eNB" : "UE",
                    rlc_pP->enb_module_id,
                    rlc_pP->ue_module_id,
                    (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                    rlc_pP->rb_id,
                    rlc_pP->vr_ur,
                    in_window);
#endif
            rlc_pP->vr_ur = in_window;
        }
    }
    // -if the reception buffer contains an UMD PDU with SN = VR(UR):
    //      -update VR(UR) to the SN of the first UMD PDU with SN > current
    //          VR(UR) that has not been received;
    //      -reassemble RLC SDUs from any UMD PDUs with SN < updated VR(UR),
    //          remove RLC headers when doing so and deliver the reassembled RLC
    //          SDUs to upper layer in ascending order of the RLC SN if not
    //          delivered before;
    if ((sn == rlc_pP->vr_ur) && rlc_um_get_pdu_from_dar_buffer(rlc_pP, rlc_pP->vr_ur)) {
        //sn_tmp = rlc_pP->vr_ur;
        do {
            rlc_pP->vr_ur = (rlc_pP->vr_ur+1) % rlc_pP->rx_sn_modulo;
        } while (rlc_um_get_pdu_from_dar_buffer(rlc_pP, rlc_pP->vr_ur) && (rlc_pP->vr_ur != rlc_pP->vr_uh));
        rlc_um_try_reassembly(rlc_pP, frameP, eNB_flagP, sn, rlc_pP->vr_ur);
    }

    // -if t-Reordering is running:
    //      -if VR(UX) <= VR(UR); or
    //      -if VR(UX) falls outside of the reordering window and VR(UX) is not
    //          equal to VR(UH)::
    //          -stop and reset t-Reordering;
    if (rlc_pP->t_reordering.running) {
        if (rlc_pP->vr_uh != rlc_pP->vr_ux) {
            in_window = rlc_um_in_reordering_window(rlc_pP, frameP, rlc_pP->vr_ux);
            if (in_window < 0) {
#if defined (TRACE_RLC_UM_DAR)
                LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] STOP and RESET t-Reordering because VR(UX) falls outside of the reordering window and VR(UX)=%d is not equal to VR(UH)=%d -or- VR(UX) <= VR(UR)\n",
                        frameP,
                        (rlc_pP->is_enb) ? "eNB" : "UE",
                        rlc_pP->enb_module_id,
                        rlc_pP->ue_module_id,
                        (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                        rlc_pP->rb_id,
                        rlc_pP->vr_ux,
                        rlc_pP->vr_uh);
#endif
                rlc_um_stop_and_reset_timer_reordering(rlc_pP, frameP);
            }
        }
    }
    if (rlc_pP->t_reordering.running) {
      in_window = rlc_um_in_window(rlc_pP, frameP, rlc_pP->vr_ur,  rlc_pP->vr_ux,  rlc_pP->vr_uh);
        if ((in_window == -2) || (in_window == 1)) {
#if defined (TRACE_RLC_UM_DAR)
            LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] STOP and RESET t-Reordering because VR(UX) falls outside of the reordering window and VR(UX)=%d is not equal to VR(UH)=%d\n",
                    frameP,
                    (rlc_pP->is_enb) ? "eNB" : "UE",
                    rlc_pP->enb_module_id,
                    rlc_pP->ue_module_id,
                    (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                    rlc_pP->rb_id,
                    rlc_pP->vr_ux,
                    rlc_pP->vr_uh);
#endif
            rlc_um_stop_and_reset_timer_reordering(rlc_pP, frameP);
        }
    }
    // -if t-Reordering is not running (includes the case when t-Reordering is
    //      stopped due to actions above):
    //      -if VR(UH) > VR(UR):
    //          -start t-Reordering;
    //          -set VR(UX) to VR(UH).
    if (rlc_pP->t_reordering.running == 0) {
      in_window = rlc_um_in_window(rlc_pP, frameP, rlc_pP->vr_ur,  rlc_pP->vr_uh,  rlc_pP->vr_uh);
        if (in_window == 2) {
            rlc_um_start_timer_reordering(rlc_pP, frameP);
            rlc_pP->vr_ux = rlc_pP->vr_uh;
#if defined (TRACE_RLC_UM_DAR)
            LOG_D(RLC, "[FRAME %05u][%s][RLC_UM][MOD %u/%u][%s %u] RESTART t-Reordering set VR(UX) to VR(UH) =%d\n",
                    frameP,
                    (rlc_pP->is_enb) ? "eNB" : "UE",
                    rlc_pP->enb_module_id,
                    rlc_pP->ue_module_id,
                    (rlc_pP->is_data_plane) ? "DRB" : "SRB",
                    rlc_pP->rb_id,
                    rlc_pP->vr_ux);
#endif
        }
    }
}

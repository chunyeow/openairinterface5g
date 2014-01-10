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

#define DEBUG_RLC_UM_RX_DECODE
//#define DEBUG_RLC_UM_RX
//#define DEBUG_DISPLAY_NVIDIA
//-----------------------------------------------------------------------------
signed int rlc_um_get_pdu_infos(u32_t frame,rlc_um_pdu_sn_10_t* headerP, s16_t total_sizeP, rlc_um_pdu_info_t* pdu_infoP)
//-----------------------------------------------------------------------------
{
    memset(pdu_infoP, 0, sizeof (rlc_um_pdu_info_t));

    s16_t          sum_li = 0;
    pdu_infoP->num_li = 0;


    pdu_infoP->fi  = (headerP->b1 >> 3) & 0x03;
    pdu_infoP->e   = (headerP->b1 >> 2) & 0x01;
    pdu_infoP->sn  = headerP->b2 +  (((u16_t)(headerP->b1 & 0x03)) << 8);

    pdu_infoP->header_size  = 2;

    pdu_infoP->payload = &headerP->data[0];

    if (pdu_infoP->e) {
        rlc_am_e_li_t      *e_li;
        unsigned int li_length_in_bytes  = 1;
        unsigned int li_to_read          = 1;

        e_li = (rlc_am_e_li_t*)(headerP->data);

        while (li_to_read)  {
            li_length_in_bytes = li_length_in_bytes ^ 3;
            if (li_length_in_bytes  == 2) {
                AssertFatal( total_sizeP >= ((uint64_t)(&e_li->b2) - (uint64_t)headerP),
                        "DECODING PDU TOO FAR PDU size %d", total_sizeP);
                pdu_infoP->li_list[pdu_infoP->num_li] = ((u16_t)(e_li->b1 << 4)) & 0x07F0;
                pdu_infoP->li_list[pdu_infoP->num_li] |= (((u8_t)(e_li->b2 >> 4)) & 0x000F);
                li_to_read = e_li->b1 & 0x80;
                pdu_infoP->header_size  += 2;
            } else {
                AssertFatal( total_sizeP >= ((uint64_t)(&e_li->b3) - (uint64_t)headerP),
                        "DECODING PDU TOO FAR PDU size %d", total_sizeP);
                pdu_infoP->li_list[pdu_infoP->num_li] = ((u16_t)(e_li->b2 << 8)) & 0x0700;
                pdu_infoP->li_list[pdu_infoP->num_li] |=  e_li->b3;
                li_to_read = e_li->b2 & 0x08;
                e_li++;
                pdu_infoP->header_size  += 1;
            }
            AssertFatal( pdu_infoP->num_li >= RLC_AM_MAX_SDU_IN_PDU, "[FRAME %05d][RLC_UM][MOD XX][RB XX][GET PDU INFO]  SN %04d TOO MANY LIs ", frame, pdu_infoP->sn);
            sum_li += pdu_infoP->li_list[pdu_infoP->num_li];
            pdu_infoP->num_li = pdu_infoP->num_li + 1;
            if (pdu_infoP->num_li > RLC_AM_MAX_SDU_IN_PDU) {
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
}
//-----------------------------------------------------------------------------
int rlc_um_read_length_indicators(unsigned char**dataP, rlc_um_e_li_t* e_liP, unsigned int* li_arrayP, unsigned int *num_liP, unsigned int *data_sizeP) {
//-----------------------------------------------------------------------------
    int continue_loop = 1;
    *num_liP = 0;
    unsigned int e1;
    unsigned int li1;
    unsigned int e2;
    unsigned int li2;

    while ((continue_loop)) {
        //msg("[RLC_UM] e_liP->b1 = %02X\n", e_liP->b1);
        //msg("[RLC_UM] e_liP->b2 = %02X\n", e_liP->b2);
        e1 = ((unsigned int)e_liP->b1 & 0x00000080) >> 7;
        li1 = (((unsigned int)e_liP->b1 & 0x0000007F) << 4) + (((unsigned int)e_liP->b2 & 0x000000F0) >> 4);
        li_arrayP[*num_liP] = li1;
        *data_sizeP = *data_sizeP - li1 - 2;
        *num_liP = *num_liP +1;
        if ((e1)) {
            e2 = ((unsigned int)e_liP->b2 & 0x00000008) >> 3;
            li2 = (((unsigned int)e_liP->b2 & 0x00000007) << 8) + ((unsigned int)e_liP->b3 & 0x000000FF);
            li_arrayP[*num_liP] = li2;
            *data_sizeP = *data_sizeP - li2 - 1;
            *num_liP = *num_liP +1;
            if (e2 == 0) {
                continue_loop = 0;
            } else {
                e_liP++;
            }
        } else {
            continue_loop = 0;
        }
        if (*num_liP >= RLC_UM_SEGMENT_NB_MAX_LI_PER_PDU) {
            return -1;
        }
    }
    *dataP = *dataP + (((*num_liP*3) +1) >> 1);
    return 0;
}
//-----------------------------------------------------------------------------
void rlc_um_try_reassembly(rlc_um_entity_t *rlcP, u32_t frame, u8_t eNB_flag, signed int start_snP, signed int end_snP) {
//-----------------------------------------------------------------------------
    mem_block_t        *pdu_mem;
    struct mac_tb_ind  *tb_ind;
    rlc_um_e_li_t      *e_li;
    unsigned char      *data;
    int                 e;
    int                 fi;
    unsigned int        size;
    signed int          sn;
    unsigned int        continue_reassembly;
    unsigned int        num_li;
    unsigned int        li_array[RLC_UM_SEGMENT_NB_MAX_LI_PER_PDU];
    int i;
    int reassembly_start_index;

    if (end_snP < 0)   end_snP   = end_snP   + rlcP->rx_sn_modulo;
    if (start_snP < 0) start_snP = start_snP + rlcP->rx_sn_modulo;

    LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] TRY REASSEMBLY FROM PDU SN=%03d+1  TO  PDU SN=%03d   SN Length = %d bits\n",
          rlcP->module_id,
          rlcP->rb_id,
          frame,
          rlcP->last_reassemblied_sn,
          end_snP,
          rlcP->rx_sn_length);

    // nothing to be reassemblied
    if (start_snP == end_snP) {
        return;
    }
    continue_reassembly = 1;
    //sn = (rlcP->last_reassemblied_sn + 1) % rlcP->rx_sn_modulo;
    sn = start_snP;

    //check_mem_area();

    while (continue_reassembly) {
        if ((pdu_mem = rlcP->dar_buffer[sn])) {

            if ((rlcP->last_reassemblied_sn+1)%rlcP->rx_sn_modulo != sn) {
                LOG_W(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] FINDING a HOLE in RLC UM SN: CLEARING OUTPUT SDU BECAUSE NEW SN (%03d) TO REASSEMBLY NOT CONTIGUOUS WITH LAST REASSEMBLIED SN (%03d)\n",
                      rlcP->module_id, rlcP->rb_id, frame, sn, rlcP->last_reassemblied_sn);
                rlc_um_clear_rx_sdu(rlcP);
            }
            rlcP->last_reassemblied_sn = sn;
            tb_ind = (struct mac_tb_ind *)(pdu_mem->data);
            if (rlcP->rx_sn_length == 10) {
                LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] TRY REASSEMBLY 10 PDU SN=%03d\n", rlcP->module_id, rlcP->rb_id, frame, sn);
                e  = (((rlc_um_pdu_sn_10_t*)(tb_ind->data_ptr))->b1 & 0x04) >> 2;
                fi = (((rlc_um_pdu_sn_10_t*)(tb_ind->data_ptr))->b1 & 0x18) >> 3;
                e_li = (rlc_um_e_li_t*)((rlc_um_pdu_sn_10_t*)(tb_ind->data_ptr))->data;
                size   = tb_ind->size - 2;
                data = &tb_ind->data_ptr[2];
            } else {
                LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] TRY REASSEMBLY 5 PDU SN=%03d Byte 0=%02X\n", rlcP->module_id, rlcP->rb_id, frame, sn, ((rlc_um_pdu_sn_5_t*)(tb_ind->data_ptr))->b1);
                e  = (((rlc_um_pdu_sn_5_t*)(tb_ind->data_ptr))->b1 & 0x00000020) >> 5;
                fi = (((rlc_um_pdu_sn_5_t*)(tb_ind->data_ptr))->b1 & 0x000000C0) >> 6;
                e_li = (rlc_um_e_li_t*)((rlc_um_pdu_sn_5_t*)(tb_ind->data_ptr))->data;
                size   = tb_ind->size - 1;
                data = &tb_ind->data_ptr[1];
                LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] e=%01X fi=%01X\n", rlcP->module_id, rlcP->rb_id, frame, e, fi);
            }
            if (e == RLC_E_FIXED_PART_DATA_FIELD_FOLLOW) {
                switch (fi) {
                    case RLC_FI_1ST_BYTE_DATA_IS_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_LAST_BYTE_SDU:
                        LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] TRY REASSEMBLY PDU NO E_LI FI=11 (00)\n", rlcP->module_id, rlcP->rb_id, frame);
                        // one complete SDU
                        //LGrlc_um_send_sdu(rlcP,frame,eNB_flag); // may be not necessary
                        rlc_um_clear_rx_sdu(rlcP);
                        rlc_um_reassembly (data, size, rlcP,frame);
                        rlc_um_send_sdu(rlcP,frame,eNB_flag);
                        rlcP->reassembly_missing_sn_detected = 0;

                        break;
                    case RLC_FI_1ST_BYTE_DATA_IS_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_NOT_LAST_BYTE_SDU:
                        LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] TRY REASSEMBLY PDU NO E_LI FI=10 (01)\n", rlcP->module_id, rlcP->rb_id, frame);
                        // one beginning segment of SDU in PDU
                        //LG rlc_um_send_sdu(rlcP,frame,eNB_flag); // may be not necessary
                        rlc_um_clear_rx_sdu(rlcP);
                        rlc_um_reassembly (data, size, rlcP,frame);
                        rlcP->reassembly_missing_sn_detected = 0;
                        break;
                    case RLC_FI_1ST_BYTE_DATA_IS_NOT_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_LAST_BYTE_SDU:
                        LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] TRY REASSEMBLY PDU NO E_LI FI=01 (10)\n", rlcP->module_id, rlcP->rb_id, frame);
                        // one last segment of SDU
                        if (rlcP->reassembly_missing_sn_detected == 0) {
                            rlc_um_reassembly (data, size, rlcP,frame);
                            rlc_um_send_sdu(rlcP,frame,eNB_flag);
                        } else {
                            //clear sdu already done
                            rlcP->stat_rx_data_pdu_dropped += 1;
                            rlcP->stat_rx_data_bytes_dropped += tb_ind->size;
                        }
                        rlcP->reassembly_missing_sn_detected = 0;
                        break;
                    case RLC_FI_1ST_BYTE_DATA_IS_NOT_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_NOT_LAST_BYTE_SDU:
                        LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] TRY REASSEMBLY PDU NO E_LI FI=00 (11)\n", rlcP->module_id, rlcP->rb_id, frame);
                        if (rlcP->reassembly_missing_sn_detected == 0) {
                            // one whole segment of SDU in PDU
                            rlc_um_reassembly (data, size, rlcP,frame);
                        } else {
                            LOG_W(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] TRY REASSEMBLY PDU NO E_LI FI=00 (11) MISSING SN DETECTED\n", rlcP->module_id, rlcP->rb_id, frame);
                            //LOG_D(RLC, "[MSC_NBOX][FRAME %05d][RLC_UM][MOD %02d][RB %02d][Missing SN detected][RLC_UM][MOD %02d][RB %02d]\n",
                            //      frame, rlcP->module_id,rlcP->rb_id, rlcP->module_id,rlcP->rb_id);
                            rlcP->reassembly_missing_sn_detected = 1; // not necessary but for readability of the code
                            rlcP->stat_rx_data_pdu_dropped += 1;
                            rlcP->stat_rx_data_bytes_dropped += tb_ind->size;
#if defined(RLC_STOP_ON_LOST_PDU)
                            AssertFatal( rlcP->reassembly_missing_sn_detected == 1,
                                    "[RLC_UM][MOD %d][RB %d][FRAME %05d] MISSING PDU DETECTED\n",
                                    rlcP->module_id, rlcP->rb_id, frame);
#endif
                        }

                        break;
                    default:
                        AssertFatal( 0 !=0, "[RLC_UM][MOD %d][RB %d][FRAME %05d] TRY REASSEMBLY SHOULD NOT GO HERE\n", rlcP->module_id, rlcP->rb_id, frame);
                }
            } else {
                if (rlc_um_read_length_indicators(&data, e_li, li_array, &num_li, &size ) >= 0) {
                    switch (fi) {
                        case RLC_FI_1ST_BYTE_DATA_IS_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_LAST_BYTE_SDU:
                            LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] TRY REASSEMBLY PDU FI=11 (00) Li=", rlcP->module_id, rlcP->rb_id, frame);
                            for (i=0; i < num_li; i++) {
                                LOG_D(RLC, "%d ",li_array[i]);
                            }
                            LOG_D(RLC, " remaining size %d\n",size);
                            // N complete SDUs
                            //LGrlc_um_send_sdu(rlcP,frame,eNB_flag);
                            rlc_um_clear_rx_sdu(rlcP);

                            for (i = 0; i < num_li; i++) {
                                rlc_um_reassembly (data, li_array[i], rlcP,frame);
                                rlc_um_send_sdu(rlcP,frame,eNB_flag);
                                data = &data[li_array[i]];
                            }
                            if (size > 0) { // normally should always be > 0 but just for help debug
                                // data is already ok, done by last loop above
                                rlc_um_reassembly (data, size, rlcP,frame);
                                rlc_um_send_sdu(rlcP,frame,eNB_flag);
                            }
                            rlcP->reassembly_missing_sn_detected = 0;
                            break;
                        case RLC_FI_1ST_BYTE_DATA_IS_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_NOT_LAST_BYTE_SDU:
                            LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] TRY REASSEMBLY PDU FI=10 (01) Li=", rlcP->module_id, rlcP->rb_id, frame);
                            for (i=0; i < num_li; i++) {
                                LOG_D(RLC, "%d ",li_array[i]);
                            }
                            LOG_D(RLC, " remaining size %d\n",size);
                            // N complete SDUs + one segment of SDU in PDU
                            //LG rlc_um_send_sdu(rlcP,frame,eNB_flag);
                            rlc_um_clear_rx_sdu(rlcP);
                            for (i = 0; i < num_li; i++) {
                                rlc_um_reassembly (data, li_array[i], rlcP,frame);
                                rlc_um_send_sdu(rlcP,frame,eNB_flag);
                                data = &data[li_array[i]];
                            }
                            if (size > 0) { // normally should always be > 0 but just for help debug
                                // data is already ok, done by last loop above
                                rlc_um_reassembly (data, size, rlcP,frame);
                            }
                            rlcP->reassembly_missing_sn_detected = 0;
                            break;
                        case RLC_FI_1ST_BYTE_DATA_IS_NOT_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_LAST_BYTE_SDU:
                            LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] TRY REASSEMBLY PDU FI=01 (10) Li=", rlcP->module_id, rlcP->rb_id, frame);
                            for (i=0; i < num_li; i++) {
                                LOG_D(RLC, "%d ",li_array[i]);
                            }
                            LOG_D(RLC, " remaining size %d\n",size);
                            if (rlcP->reassembly_missing_sn_detected) {
                                reassembly_start_index = 1;
                                data = &data[li_array[0]];
                                //rlcP->stat_rx_data_pdu_dropped += 1;
                                rlcP->stat_rx_data_bytes_dropped += li_array[0];
                            } else {
                                reassembly_start_index = 0;
                            }

                            // one last segment of SDU + N complete SDUs in PDU
                            for (i = reassembly_start_index; i < num_li; i++) {
                                rlc_um_reassembly (data, li_array[i], rlcP,frame);
                                rlc_um_send_sdu(rlcP,frame,eNB_flag);
                                data = &data[li_array[i]];
                            }
                            if (size > 0) { // normally should always be > 0 but just for help debug
                                // data is already ok, done by last loop above
                                rlc_um_reassembly (data, size, rlcP,frame);
                                rlc_um_send_sdu(rlcP,frame,eNB_flag);
                            }
                            rlcP->reassembly_missing_sn_detected = 0;
                            break;
                        case RLC_FI_1ST_BYTE_DATA_IS_NOT_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_NOT_LAST_BYTE_SDU:
                            LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] TRY REASSEMBLY PDU FI=00 (11) Li=", rlcP->module_id, rlcP->rb_id, frame);
                            for (i=0; i < num_li; i++) {
                                LOG_D(RLC, "%d ",li_array[i]);
                            }
                            LOG_D(RLC, " remaining size %d\n",size);
                            if (rlcP->reassembly_missing_sn_detected) {
                                LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] DISCARD FIRST LI %d", rlcP->module_id, rlcP->rb_id, frame, li_array[0]);
                                reassembly_start_index = 1;
                                data = &data[li_array[0]];
                            	//rlcP->stat_rx_data_pdu_dropped += 1;
                            	rlcP->stat_rx_data_bytes_dropped += li_array[0];
                            } else {
                                reassembly_start_index = 0;
                            }

                            for (i = reassembly_start_index; i < num_li; i++) {
                                rlc_um_reassembly (data, li_array[i], rlcP,frame);
                                rlc_um_send_sdu(rlcP,frame,eNB_flag);
                                data = &data[li_array[i]];
                            }
                            if (size > 0) { // normally should always be > 0 but just for help debug
                                // data is already ok, done by last loop above
                                rlc_um_reassembly (data, size, rlcP,frame);
                            } else {
                                AssertFatal( 0 !=0, "[RLC_UM][MOD %d][RB %d][FRAME %05d] SHOULD NOT GO HERE\n", rlcP->module_id, rlcP->rb_id, frame);
                            	//rlcP->stat_rx_data_pdu_dropped += 1;
                            	rlcP->stat_rx_data_bytes_dropped += size;
                            }
                            rlcP->reassembly_missing_sn_detected = 0;
                            break;
                        default:
                            LOG_W(RLC, "[MSC_NBOX][FRAME %05d][RLC_UM][MOD %02d][RB %02d][Missing SN detected][RLC_UM][MOD %02d][RB %02d]\n",
                                  frame, rlcP->module_id,rlcP->rb_id, rlcP->module_id,rlcP->rb_id);
                            rlcP->stat_rx_data_pdu_dropped += 1;
                            rlcP->stat_rx_data_bytes_dropped += tb_ind->size;

                            rlcP->reassembly_missing_sn_detected = 1;
#if defined(RLC_STOP_ON_LOST_PDU)
                            AssertFatal( rlcP->reassembly_missing_sn_detected == 1,
                                    "[RLC_UM][MOD %d][RB %d][FRAME %05d] MISSING PDU DETECTED\n", rlcP->module_id, rlcP->rb_id, frame);
#endif
                    }
                }
            }
            LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] REMOVE PDU FROM DAR BUFFER  SN=%03d\n", rlcP->module_id, rlcP->rb_id, frame, sn);
            free_mem_block(rlcP->dar_buffer[sn]);
            rlcP->dar_buffer[sn] = NULL;
        } else {
            rlcP->last_reassemblied_missing_sn = sn;
            LOG_D(RLC, "[MSC_NBOX][FRAME %05d][RLC_UM][MOD %02d][RB %02d][Missing SN %04d detected, clearing RX SDU][RLC_UM][MOD %02d][RB %02d]\n",
		  frame, rlcP->module_id,rlcP->rb_id, sn, rlcP->module_id,rlcP->rb_id);
            rlcP->reassembly_missing_sn_detected = 1;
            rlc_um_clear_rx_sdu(rlcP);
#if defined(RLC_STOP_ON_LOST_PDU)
            AssertFatal( rlcP->reassembly_missing_sn_detected == 1,
                    "[FRAME %05d][RLC_UM][MOD %d][RB %d] MISSING PDU DETECTED\n", frame, rlcP->module_id, rlcP->rb_id);
#endif
        }
        sn = (sn + 1) % rlcP->rx_sn_modulo;
        if ((sn == rlcP->vr_uh) || (sn == end_snP)){
            continue_reassembly = 0;
        }
    }
    LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] TRIED REASSEMBLY VR(UR)=%03d VR(UX)=%03d VR(UH)=%03d\n", rlcP->module_id, rlcP->rb_id, frame, rlcP->vr_ur, rlcP->vr_ux,rlcP->vr_uh);

}
//-----------------------------------------------------------------------------
void rlc_um_stop_and_reset_timer_reordering(rlc_um_entity_t *rlcP,u32_t frame)
//-----------------------------------------------------------------------------
{
    LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][T-REORDERING] STOPPED AND RESET\n", frame,
                        rlcP->module_id, rlcP->rb_id);
    rlcP->t_reordering.running         = 0;
    rlcP->t_reordering.frame_time_out  = 0;
    rlcP->t_reordering.frame_start     = 0;
    rlcP->t_reordering.timed_out       = 0;
}
//-----------------------------------------------------------------------------
void rlc_um_start_timer_reordering(rlc_um_entity_t *rlcP,u32_t frame)
//-----------------------------------------------------------------------------
{
    rlcP->t_reordering.running         = 1;
    rlcP->t_reordering.frame_time_out  = frame + rlcP->t_reordering.time_out;
    rlcP->t_reordering.frame_start     = frame;
    rlcP->t_reordering.timed_out       = 0;
    LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][T-REORDERING] STARTED (TIME-OUT = FRAME %05d)\n",
            frame, rlcP->module_id, rlcP->rb_id, rlcP->t_reordering.frame_time_out);
}
//-----------------------------------------------------------------------------
void rlc_um_init_timer_reordering(rlc_um_entity_t *rlcP, u32_t time_outP)
//-----------------------------------------------------------------------------
{
    rlcP->t_reordering.running         = 0;
    rlcP->t_reordering.frame_time_out  = 0;
    rlcP->t_reordering.frame_start     = 0;
    rlcP->t_reordering.time_out        = time_outP;
    rlcP->t_reordering.timed_out       = 0;
}
//-----------------------------------------------------------------------------
void rlc_um_check_timer_dar_time_out(rlc_um_entity_t *rlcP,u32_t frame,u8_t eNB_flag) {
//-----------------------------------------------------------------------------
    signed int in_window;
    u16_t      old_vr_ur;
    if ((rlcP->t_reordering.running)) {
        if (
        // CASE 1:          start              time out
        //        +-----------+------------------+----------+
        //        |           |******************|          |
        //        +-----------+------------------+----------+
        //FRAME # 0                                     FRAME MAX
        ((rlcP->t_reordering.frame_start < rlcP->t_reordering.frame_time_out) &&
            ((frame >= rlcP->t_reordering.frame_time_out) ||
             (frame < rlcP->t_reordering.frame_start)))                                   ||
        // CASE 2:        time out            start
        //        +-----------+------------------+----------+
        //        |***********|                  |**********|
        //        +-----------+------------------+----------+
        //FRAME # 0                                     FRAME MAX VALUE
        ((rlcP->t_reordering.frame_start > rlcP->t_reordering.frame_time_out) &&
           (frame < rlcP->t_reordering.frame_start) && (frame >= rlcP->t_reordering.frame_time_out))
        ) {

        //if ((u32_t)((u32_t)rlcP->timer_reordering  + (u32_t)rlcP->timer_reordering_init)   <= frame) {
            // 5.1.2.2.4   Actions when t-Reordering expires
            //  When t-Reordering expires, the receiving UM RLC entity shall:
            //  -update VR(UR) to the SN of the first UMD PDU with SN >= VR(UX) that has not been received;
            //  -reassemble RLC SDUs from any UMD PDUs with SN < updated VR(UR), remove RLC headers when doing so and deliver the reassembled RLC SDUs to upper layer in ascending order of the RLC SN if not delivered before;
            //  -if VR(UH) > VR(UR):
            //      -start t-Reordering;
            //      -set VR(UX) to VR(UH).
        	rlcP->stat_timer_reordering_timed_out += 1;
            LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d]*****************************************************\n", rlcP->module_id, rlcP->rb_id, frame);
            LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d]*    T I M E  -  O U T                              *\n", rlcP->module_id, rlcP->rb_id, frame);
            LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d]*****************************************************\n", rlcP->module_id, rlcP->rb_id, frame);
            LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] TIMER t-Reordering expiration\n", rlcP->module_id, rlcP->rb_id, frame);
            LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] timer_reordering=%d frame=%d expire frame %d\n",
            		rlcP->module_id,
            		rlcP->rb_id,
            		frame,
            		rlcP->t_reordering.time_out,
            		rlcP->t_reordering.frame_time_out,
            		frame);
            LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] set VR(UR)=%03d to", rlcP->module_id, rlcP->rb_id, frame, rlcP->vr_ur);

            old_vr_ur   = rlcP->vr_ur;

            rlcP->vr_ur = rlcP->vr_ux;
            while (rlc_um_get_pdu_from_dar_buffer(rlcP, rlcP->vr_ur)) {
                rlcP->vr_ur = (rlcP->vr_ur+1)%rlcP->rx_sn_modulo;
            }
            LOG_D(RLC, " %d", rlcP->vr_ur);
            LOG_D(RLC, "\n");

            rlc_um_try_reassembly(rlcP,frame,eNB_flag,old_vr_ur, rlcP->vr_ur);

            in_window = rlc_um_in_window(rlcP, frame, rlcP->vr_ur,  rlcP->vr_uh,  rlcP->vr_uh);
            if (in_window == 2) {
            	rlc_um_start_timer_reordering(rlcP, frame);
                rlcP->vr_ux = rlcP->vr_uh;
                LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] restarting t-Reordering set VR(UX) to %d (VR(UH)>VR(UR))\n", rlcP->module_id, rlcP->rb_id, frame, rlcP->vr_ux);
                LOG_D(RLC, "[MSC_MSG][FRAME %05d][RLC_UM][MOD %02d][RB %02d][--- t-Reordering Timed-out (restarted) VR(UR)=%03d->%03d VR(UX)=%03d --->][RLC_UM][MOD %02d][RB %02d]\n",
                      frame,
                      rlcP->module_id,
                      rlcP->rb_id,
                      old_vr_ur,
                      rlcP->vr_ur,
                      rlcP->vr_ux,
                      rlcP->module_id,
                      rlcP->rb_id);
            } else {
                LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] STOP t-Reordering VR(UX) = %03d\n", rlcP->module_id, rlcP->rb_id, frame, rlcP->vr_ux);
                rlc_um_stop_and_reset_timer_reordering(rlcP, frame);
                LOG_D(RLC, "[MSC_MSG][FRAME %05d][RLC_UM][MOD %02d][RB %02d][--- t-Reordering Timed-out (stopped) VR(UR)=%03d->%03d VR(UX)=%03d --->][RLC_UM][MOD %02d][RB %02d]\n",
                      frame,
                      rlcP->module_id,
                      rlcP->rb_id,
                      old_vr_ur,
                      rlcP->vr_ur,
                      rlcP->vr_ux,
                      rlcP->module_id,
                      rlcP->rb_id);
            }
        }
    }
}
//-----------------------------------------------------------------------------
inline mem_block_t *
rlc_um_remove_pdu_from_dar_buffer(rlc_um_entity_t *rlcP, u16_t snP)
{
//-----------------------------------------------------------------------------
    mem_block_t * pdu     = rlcP->dar_buffer[snP];
    LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] REMOVE PDU FROM DAR BUFFER  SN=%03d\n", rlcP->module_id, rlcP->rb_id, -1, snP);
    rlcP->dar_buffer[snP] = NULL;
    return pdu;
}
//-----------------------------------------------------------------------------
inline mem_block_t *
rlc_um_get_pdu_from_dar_buffer(rlc_um_entity_t *rlcP, u16_t snP)
{
//-----------------------------------------------------------------------------
    return rlcP->dar_buffer[snP];
}
//-----------------------------------------------------------------------------
inline void
rlc_um_store_pdu_in_dar_buffer(rlc_um_entity_t *rlcP, u32_t frame, mem_block_t *pduP, u16_t snP)
{
//-----------------------------------------------------------------------------
    LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] STORE PDU IN DAR BUFFER  SN=%03d  VR(UR)=%03d VR(UX)=%03d VR(UH)=%03d\n", rlcP->module_id, rlcP->rb_id, frame, snP, rlcP->vr_ur, rlcP->vr_ux, rlcP->vr_uh);
    rlcP->dar_buffer[snP] = pduP;
}
//-----------------------------------------------------------------------------
// returns -2 if lower_bound  > sn
// returns -1 if higher_bound < sn
// returns  0 if lower_bound  < sn < higher_bound
// returns  1 if lower_bound  == sn
// returns  2 if higher_bound == sn
// returns  3 if higher_bound == sn == lower_bound
inline signed int rlc_um_in_window(rlc_um_entity_t *rlcP, u32_t frame, signed int lower_boundP, signed int snP, signed int higher_boundP) {
//-----------------------------------------------------------------------------

    signed int modulus = (signed int)rlcP->vr_uh - rlcP->rx_um_window_size;
#ifdef DEBUG_RLC_UM_RX
    signed int     lower_bound  = lower_boundP;
    signed int     higher_bound = higher_boundP;
    signed int     sn           = snP;
#endif
    lower_boundP  = (lower_boundP  - modulus) % rlcP->rx_sn_modulo;
    higher_boundP = (higher_boundP - modulus) % rlcP->rx_sn_modulo;
    snP           = (snP           - modulus) % rlcP->rx_sn_modulo;

    if ( lower_boundP > snP) {
#ifdef DEBUG_RLC_UM_RX
        LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] %d not in WINDOW[%03d:%03d] (SN<LOWER BOUND)\n", rlcP->module_id, rlcP->rb_id, frame, sn, lower_bound, higher_bound);
#endif
        return -2;
    }
    if ( higher_boundP < snP) {
#ifdef DEBUG_RLC_UM_RX
        LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] %d not in WINDOW[%03d:%03d] (SN>HIGHER BOUND) <=> %d not in WINDOW[%03d:%03d]\n", rlcP->module_id, rlcP->rb_id, frame, sn, lower_bound, higher_bound, snP, lower_boundP, higher_boundP);
#endif
        return -1;
    }
    if ( lower_boundP == snP) {
        if ( higher_boundP == snP) {
#ifdef DEBUG_RLC_UM_RX
        LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] %d  in WINDOW[%03d:%03d] (SN=HIGHER BOUND=LOWER BOUND)\n", rlcP->module_id, rlcP->rb_id, frame, sn, lower_bound, higher_bound);
#endif
            return 3;
        }
#ifdef DEBUG_RLC_UM_RX
        LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] %d  in WINDOW[%03d:%03d] (SN=LOWER BOUND)\n", rlcP->module_id, rlcP->rb_id, frame, sn, lower_bound, higher_bound);
#endif
        return 1;
    }
    if ( higher_boundP == snP) {
#ifdef DEBUG_RLC_UM_RX
        LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] %d  in WINDOW[%03d:%03d] (SN=HIGHER BOUND)\n", rlcP->module_id, rlcP->rb_id, frame, sn, lower_bound, higher_bound);
#endif
        return 2;
    }
    return 0;

}
//-----------------------------------------------------------------------------
inline signed int rlc_um_in_reordering_window(rlc_um_entity_t *rlcP, u32_t frame, signed int snP) {
//-----------------------------------------------------------------------------
    signed int modulus = (signed int)rlcP->vr_uh - rlcP->rx_um_window_size;
    signed int sn = snP;

    snP           = (snP - modulus) % rlcP->rx_sn_modulo;

    if ( 0 <= snP) {
        if (snP < rlcP->rx_um_window_size) {
           LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] %d IN REORDERING WINDOW[%03d:%03d[ SN %d IN [%03d:%03d[ VR(UR)=%03d VR(UH)=%03d\n",
                 rlcP->module_id, rlcP->rb_id, frame, snP, 0, rlcP->rx_um_window_size,
                                                                 sn, (signed int)rlcP->vr_uh - rlcP->rx_um_window_size, rlcP->vr_uh,
                                                                 rlcP->vr_ur, rlcP->vr_uh);
            return 0;
        }
    }
    if (modulus < 0) {
        LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] %d NOT IN REORDERING WINDOW[%03d:%03d[ SN %d NOT IN [%03d:%03d[ VR(UR)=%03d VR(UH)=%03d\n",
             rlcP->module_id, rlcP->rb_id, frame, snP, modulus + 1024, rlcP->rx_um_window_size, sn, modulus + 1024 , rlcP->vr_uh, rlcP->vr_ur, rlcP->vr_uh);
    } else {
        LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] %d NOT IN REORDERING WINDOW[%03d:%03d[ SN %d NOT IN [%03d:%03d[ VR(UR)=%03d VR(UH)=%03d\n",
             rlcP->module_id, rlcP->rb_id, frame, snP, modulus, rlcP->rx_um_window_size, sn, modulus , rlcP->vr_uh, rlcP->vr_ur, rlcP->vr_uh);
    }
    return -1;
}
//-----------------------------------------------------------------------------
void
rlc_um_receive_process_dar (rlc_um_entity_t *rlcP, u32_t frame, u8_t eNB_flag, mem_block_t *pdu_memP,rlc_um_pdu_sn_10_t *pduP, u16_t tb_sizeP)
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

    signed int sn = -1;
    signed int in_window;

    if (rlcP->rx_sn_length == 10) {
        sn = ((pduP->b1 & 0x00000003) << 8) + pduP->b2;
    } else if (rlcP->rx_sn_length == 5) {
        sn = pduP->b1 & 0x1F;
    } else {
    	free_mem_block(pdu_memP);
    }
    in_window = rlc_um_in_window(rlcP, frame, rlcP->vr_uh - rlcP->rx_um_window_size, sn, rlcP->vr_ur);

    rlc_util_print_hex_octets(RLC, &pduP->b1, tb_sizeP);

    // rlc_um_in_window() returns -2 if lower_bound  > sn
    // rlc_um_in_window() returns -1 if higher_bound < sn
    // rlc_um_in_window() returns  0 if lower_bound  < sn < higher_bound
    // rlc_um_in_window() returns  1 if lower_bound  == sn
    // rlc_um_in_window() returns  2 if higher_bound == sn
    // rlc_um_in_window() returns  3 if higher_bound == sn == lower_bound
    if ((in_window == 1) || (in_window == 0)){
        LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] RX PDU  VR(UH) – UM_Window_Size) <= SN %d < VR(UR) -> GARBAGE\n", rlcP->module_id, rlcP->rb_id, frame, sn);
        LOG_D(RLC, "[MSC_NBOX][FRAME %05d][RLC_UM][MOD %02d][RB %02d][RX PDU  (VR(UH) – UM_Window_Size) <= SN %d < VR(UR) -> DROPPED][RLC_UM][MOD %02d][RB %02d]\n",
                                      frame, rlcP->module_id,rlcP->rb_id, sn, rlcP->module_id,rlcP->rb_id);        //discard the PDU
    	rlcP->stat_rx_data_pdu_out_of_window   += 1;
    	rlcP->stat_rx_data_bytes_out_of_window += tb_sizeP;
        free_mem_block(pdu_memP);
        pdu_memP = NULL;
        return;
    }
    if ((rlc_um_get_pdu_from_dar_buffer(rlcP, sn))) {
        in_window = rlc_um_in_window(rlcP, frame, rlcP->vr_ur, sn, rlcP->vr_uh);
        if (in_window == 0){
            LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] RX PDU  VR(UR) < SN %d < VR(UH) and RECEIVED BEFORE-> GARBAGE\n", rlcP->module_id, rlcP->rb_id, frame, sn);
            LOG_D(RLC, "[MSC_NBOX][FRAME %05d][RLC_UM][MOD %02d][RB %02d][RX PDU  VR(UR) < SN %d < VR(UH) and RECEIVED BEFORE-> DROPPED][RLC_UM][MOD %02d][RB %02d]\n",
                                      frame, rlcP->module_id,rlcP->rb_id, sn, rlcP->module_id,rlcP->rb_id);        //discard the PDU
            //discard the PDU
        	rlcP->stat_rx_data_pdus_duplicate  += 1;
        	rlcP->stat_rx_data_bytes_duplicate += tb_sizeP;
            free_mem_block(pdu_memP);
            pdu_memP = NULL;
            return;
        }
        // 2 lines to avoid memory leaks
    	rlcP->stat_rx_data_pdus_duplicate  += 1;
    	rlcP->stat_rx_data_bytes_duplicate += tb_sizeP;
        LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] RX PDU SN %03d REMOVE OLD PDU BEFORE STORING NEW PDU\n", rlcP->module_id, rlcP->rb_id, frame, sn);
        mem_block_t *pdu = rlc_um_remove_pdu_from_dar_buffer(rlcP, sn);
        free_mem_block(pdu);
    }
    rlc_um_store_pdu_in_dar_buffer(rlcP, frame, pdu_memP, sn);


    // -if x falls outside of the reordering window:
    //      -update VR(UH) to x + 1;
    //      -reassemble RLC SDUs from any UMD PDUs with SN that falls outside of
    //       the reordering window, remove RLC headers when doing so and deliver
    //       the reassembled RLC SDUs to upper layer in ascending order of the
    //       RLC SN if not delivered before;
    //
    //      -if VR(UR) falls outside of the reordering window:
    //          -set VR(UR) to (VR(UH) – UM_Window_Size);
    if (rlc_um_in_reordering_window(rlcP, frame, sn) < 0) {
        LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] RX PDU  SN %d OUTSIDE REORDERING WINDOW VR(UH)=%d UM_Window_Size=%d\n", rlcP->module_id, rlcP->rb_id, frame, sn, rlcP->vr_uh, rlcP->rx_um_window_size);
        rlcP->vr_uh = (sn + 1) % rlcP->rx_sn_modulo;

        if (rlc_um_in_reordering_window(rlcP, frame, rlcP->vr_ur) != 0) {
            in_window = rlcP->vr_uh - rlcP->rx_um_window_size;
            if (in_window < 0) {
                in_window = in_window + rlcP->rx_sn_modulo;
            }

            rlc_um_try_reassembly(rlcP, frame, eNB_flag, rlcP->vr_ur, in_window);
        }


        if (rlc_um_in_reordering_window(rlcP, frame, rlcP->vr_ur) < 0) {
            LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] VR(UR) %d OUTSIDE REORDERING WINDOW SET TO VR(UH) – UM_Window_Size = %d\n", rlcP->module_id, rlcP->rb_id, frame, rlcP->vr_ur, in_window);
            rlcP->vr_ur = in_window;
        }
    }
    // -if the reception buffer contains an UMD PDU with SN = VR(UR):
    //      -update VR(UR) to the SN of the first UMD PDU with SN > current
    //          VR(UR) that has not been received;
    //      -reassemble RLC SDUs from any UMD PDUs with SN < updated VR(UR),
    //          remove RLC headers when doing so and deliver the reassembled RLC
    //          SDUs to upper layer in ascending order of the RLC SN if not
    //          delivered before;
    if ((sn == rlcP->vr_ur) && rlc_um_get_pdu_from_dar_buffer(rlcP, rlcP->vr_ur)) {
        //sn_tmp = rlcP->vr_ur;
        do {
            rlcP->vr_ur = (rlcP->vr_ur+1) % rlcP->rx_sn_modulo;
        } while (rlc_um_get_pdu_from_dar_buffer(rlcP, rlcP->vr_ur) && (rlcP->vr_ur != rlcP->vr_uh));
        rlc_um_try_reassembly(rlcP, frame, eNB_flag, sn, rlcP->vr_ur);
    }

    // -if t-Reordering is running:
    //      -if VR(UX) <= VR(UR); or
    //      -if VR(UX) falls outside of the reordering window and VR(UX) is not
    //          equal to VR(UH)::
    //          -stop and reset t-Reordering;
    if (rlcP->t_reordering.running) {
        if (rlcP->vr_uh != rlcP->vr_ux) {
            in_window = rlc_um_in_reordering_window(rlcP, frame, rlcP->vr_ux);
            if (in_window < 0) {
                LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] STOP and RESET t-Reordering because VR(UX) falls outside of the reordering window and VR(UX)=%d is not equal to VR(UH)=%d -or- VR(UX) <= VR(UR)\n", rlcP->module_id, rlcP->rb_id, frame,rlcP->vr_ux,rlcP->vr_uh);
                rlc_um_stop_and_reset_timer_reordering(rlcP, frame);
            }
        }
    }
    if (rlcP->t_reordering.running) {
      in_window = rlc_um_in_window(rlcP, frame, rlcP->vr_ur,  rlcP->vr_ux,  rlcP->vr_uh);
        if ((in_window == -2) || (in_window == 1)) {
            LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] STOP and RESET t-Reordering because VR(UX) falls outside of the reordering window and VR(UX)=%d is not equal to VR(UH)=%d\n", rlcP->module_id, rlcP->rb_id, frame,rlcP->vr_ux,rlcP->vr_uh);
            rlc_um_stop_and_reset_timer_reordering(rlcP, frame);
        }
    }
    // -if t-Reordering is not running (includes the case when t-Reordering is
    //      stopped due to actions above):
    //      -if VR(UH) > VR(UR):
    //          -start t-Reordering;
    //          -set VR(UX) to VR(UH).
    if (rlcP->t_reordering.running == 0) {
      in_window = rlc_um_in_window(rlcP, frame, rlcP->vr_ur,  rlcP->vr_uh,  rlcP->vr_uh);
        if (in_window == 2) {
            rlc_um_start_timer_reordering(rlcP, frame);
            rlcP->vr_ux = rlcP->vr_uh;
            LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] RESTART t-Reordering set VR(UX) to VR(UH) =%d\n", rlcP->module_id, rlcP->rb_id, frame,rlcP->vr_ux);
        }
    }
}

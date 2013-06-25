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
#define RLC_AM_RX_LIST_C
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



// returns 0 if success
// returns neg value if failure
//-----------------------------------------------------------------------------
signed int rlc_am_rx_list_insert_pdu(rlc_am_entity_t* rlcP, u32 frame, mem_block_t* tbP)
//-----------------------------------------------------------------------------
{
    rlc_am_pdu_info_t* pdu_info        = &((rlc_am_rx_pdu_management_t*)(tbP->data))->pdu_info;
    rlc_am_pdu_info_t* pdu_info_cursor;
    rlc_am_pdu_info_t* pdu_info_previous_cursor;
    mem_block_t*       cursor;
    mem_block_t*       previous_cursor = NULL;

    cursor = rlcP->receiver_buffer.head;
    // it is assumed this pdu is in rx window

    if (cursor) {
        if (rlcP->vr_mr < rlcP->vr_r) {
            if (pdu_info->sn >= rlcP->vr_r) {
                pdu_info_cursor = &((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info;
                while ((cursor != NULL)  && (pdu_info_cursor->sn >= rlcP->vr_r)){ // LG added =
                    if (pdu_info->sn < pdu_info_cursor->sn) {
                        if (previous_cursor != NULL) {
                            pdu_info_previous_cursor = &((rlc_am_rx_pdu_management_t*)(previous_cursor->data))->pdu_info;
                            if ((pdu_info_previous_cursor->sn == pdu_info->sn)) {
                                if (pdu_info->rf != pdu_info_previous_cursor->rf) {
                                    LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d WRONG RF -> DROPPED (vr(mr) < vr(r) and sn >= vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                                    return -2;
                                } else if (pdu_info->rf == 1) {
                                    if ((pdu_info_previous_cursor->so + pdu_info_previous_cursor->payload_size - 1) >= pdu_info->so) {
                                        LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d SO OVERLAP -> DROPPED (vr(mr) < vr(r) and sn >= vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                                        return -2;
                                    }
                                }
                            }
                        }
                        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d (vr(mr) > vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                        list2_insert_before_element(tbP, cursor, &rlcP->receiver_buffer);
                        return 0;

                    } else if (pdu_info->sn == pdu_info_cursor->sn) {
                        if (pdu_info_cursor->rf == 0) {
                            LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d DUPLICATE -> DROPPED\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                            return -2;
                        } else if (pdu_info->rf == 1) {
                            if ((pdu_info->so + pdu_info->payload_size - 1) < pdu_info_cursor->so) {

                                if (previous_cursor != NULL) {
                                    pdu_info_previous_cursor = &((rlc_am_rx_pdu_management_t*)(previous_cursor->data))->pdu_info;
                                    if ((pdu_info_previous_cursor->sn == pdu_info_cursor->sn)) {
                                        if ((pdu_info_previous_cursor->so + pdu_info_previous_cursor->payload_size - 1) < pdu_info->so) {

                                            LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d SEGMENT OFFSET %05d (vr(mr) < vr(r) and sn >= vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn, pdu_info->so);
                                            list2_insert_before_element(tbP, cursor, &rlcP->receiver_buffer);
                                            return 0;
                                        } else {
                                            LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d OVERLAP PREVIOUS SO DUPLICATE -> DROPPED\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                                            return -2;
                                        }
                                    }
                                }
                                LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d SEGMENT OFFSET %05d (vr(mr) < vr(r) and sn >= vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn, pdu_info->so);
                                list2_insert_before_element(tbP, cursor, &rlcP->receiver_buffer);
                                return 0;

                            } else if (pdu_info->so <= pdu_info_cursor->so) {
                                LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d OVERLAP SO DUPLICATE -> DROPPED\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                                return -2;
                            }
                        } else {
                            LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d DROPPED\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                            return -2;
                        }
                    }
                    previous_cursor = cursor;
                    cursor = cursor->next;
                    if (cursor != NULL) {
                        pdu_info_cursor = &((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info;
                    }
                }
                if (cursor != NULL) {
                    LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d (vr(mr) < vr(r) and sn >= vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                    list2_insert_before_element(tbP, cursor, &rlcP->receiver_buffer);
                    return 0;
                } else {
                    if (pdu_info_cursor->rf == 0) {
                        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d (vr(mr) < vr(r) and vr(h) > vr(r) and sn >= vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                        list2_add_tail(tbP, &rlcP->receiver_buffer);
                        return 0;
                    } else if ((pdu_info->rf == 1) && (pdu_info_cursor->rf == 1) && (pdu_info->sn == pdu_info_cursor->sn)) {
                        if ((pdu_info_cursor->so + pdu_info_cursor->payload_size - 1) < pdu_info->so) {
                            LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d (vr(mr) < vr(r) and vr(h) > vr(r) and sn >= vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                            list2_add_tail(tbP, &rlcP->receiver_buffer);
                            return 0;
                        } else {
                            LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d DROPPED\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                            return -2;
                        }
                    } else if (pdu_info->sn != pdu_info_cursor->sn) {
                            LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d (vr(mr) < vr(r) and vr(h) > vr(r) and sn >= vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                            list2_add_tail(tbP, &rlcP->receiver_buffer);
                            return 0;
                    }
                }
                LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d DROPPED\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                return -2;
            } else { // (pdu_info->sn < rlcP->vr_r)
                cursor = rlcP->receiver_buffer.tail;
                pdu_info_cursor = &((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info;
                while ((cursor != NULL) && (pdu_info_cursor->sn < rlcP->vr_r)){
                    //pdu_info_cursor = &((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info;
                    if (pdu_info->sn > pdu_info_cursor->sn) {
                        if (previous_cursor != NULL) {
                            pdu_info_previous_cursor = &((rlc_am_rx_pdu_management_t*)(previous_cursor->data))->pdu_info;
                            if ((pdu_info_previous_cursor->sn == pdu_info_cursor->sn)) {
                                if (pdu_info->rf != pdu_info_previous_cursor->rf) {
                                    LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d WRONG RF -> DROPPED (vr(mr) < vr(r) and sn >= vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                                    return -2;
                                } else if (pdu_info->rf == 1) {
                                    if ((pdu_info->so + pdu_info->payload_size - 1) >= pdu_info_previous_cursor->so) {
                                        LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d SO OVERLAP -> DROPPED (vr(mr) < vr(r) and sn >= vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                                        return -2;
                                    }
                                }
                            }
                        }
                        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d (vr(mr) < vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                        list2_insert_after_element(tbP, cursor, &rlcP->receiver_buffer);
                        return 0;
                    } else if (pdu_info->sn == pdu_info_cursor->sn) {
                        if (pdu_info_cursor->rf == 0) {
                            LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d DUPLICATE -> DROPPED\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                            return -2;
                        } else if (pdu_info->rf == 1) {
                            if ((pdu_info_cursor->so + pdu_info_cursor->payload_size - 1) < pdu_info->so) {

                                if (previous_cursor != NULL) {
                                    pdu_info_previous_cursor = &((rlc_am_rx_pdu_management_t*)(previous_cursor->data))->pdu_info;
                                    if ((pdu_info_previous_cursor->sn == pdu_info_cursor->sn)) {
                                        if ((pdu_info->so + pdu_info->payload_size - 1) < pdu_info_previous_cursor->so) {

                                            LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d SEGMENT OFFSET %05d (vr(mr) < vr(r) and sn < vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn, pdu_info->so);
                                            list2_insert_after_element(tbP, cursor, &rlcP->receiver_buffer);
                                            return 0;
                                        } else {
                                            LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d OVERLAP PREVIOUS SO DUPLICATE -> DROPPED\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                                            return -2;
                                        }
                                    }
                                }
                                LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d SEGMENT OFFSET %05d (vr(mr) < vr(r) and sn < vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn, pdu_info->so);
                                list2_insert_after_element(tbP, cursor, &rlcP->receiver_buffer);
                                return 0;

                            } else if (pdu_info_cursor->so <= pdu_info->so) {
                                LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d OVERLAP SO DUPLICATE -> DROPPED\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                                return -2;
                            }
                        } else {
                            LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d DROPPED\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                            return -2;
                        }
                    }
                    previous_cursor = cursor;
                    cursor = cursor->previous;
                    if (cursor != NULL) {
                        pdu_info_cursor = &((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info;
                    }
                }
                if (cursor != NULL) {
                    LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d (vr(mr) < vr(r) and sn < vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                    list2_insert_after_element(tbP, cursor, &rlcP->receiver_buffer);
                    return 0;
                } else {
                    if (pdu_info_cursor->rf == 0) {
                        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d (vr(mr) < vr(r) and vr(h) > vr(r) and sn >= vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                        list2_add_tail(tbP, &rlcP->receiver_buffer);
                        return 0;
                    } else if ((pdu_info->rf == 1) && (pdu_info_cursor->rf == 1) && (pdu_info->sn == pdu_info_cursor->sn)) {
                        if ((pdu_info_cursor->so + pdu_info_cursor->payload_size - 1) < pdu_info->so) {
                            LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d (vr(mr) < vr(r) and vr(h) > vr(r) and sn >= vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                            list2_add_tail(tbP, &rlcP->receiver_buffer);
                            return 0;
                        } else {
                            LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d DROPPED\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                            return -2;
                        }
                    } else if (pdu_info->sn != pdu_info_cursor->sn) {
                            LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d (vr(mr) < vr(r) and vr(h) > vr(r) and sn >= vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                            list2_add_tail(tbP, &rlcP->receiver_buffer);
                            return 0;
                    }
                }
                LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d DROPPED\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                return -2;
            }
        } else { // (pdu_info->vr_mr > rlcP->vr_r), > and not >=
            // FAR MORE SIMPLE CASE
            while (cursor != NULL){
                //msg ("[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d cursor %p\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, cursor);
                pdu_info_cursor = &((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info;
                if (pdu_info->sn < pdu_info_cursor->sn) {

                    if (previous_cursor != NULL) {
                        pdu_info_previous_cursor = &((rlc_am_rx_pdu_management_t*)(previous_cursor->data))->pdu_info;
                        if ((pdu_info_previous_cursor->sn == pdu_info->sn)) {
                            if (pdu_info->rf != pdu_info_previous_cursor->rf) {
                                LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d WRONG RF -> DROPPED (vr(mr) > vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                                return -2;
                            } else if (pdu_info->rf == 1) {
                                if ((pdu_info_previous_cursor->so + pdu_info_previous_cursor->payload_size - 1) >= pdu_info->so) {
                                    LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d SO OVERLAP -> DROPPED (vr(mr) > vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                                    return -2;
                                }
                            }
                        }
                    }
                    LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d (vr(mr) > vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                    list2_insert_before_element(tbP, cursor, &rlcP->receiver_buffer);
                    return 0;

                } else if (pdu_info->sn == pdu_info_cursor->sn) {
                    if (pdu_info_cursor->rf == 0) {
                        LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d WRONG RF -> DROPPED (vr(mr) > vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                        return -2;
                    } else if (pdu_info->rf == 1) {

                        if ((pdu_info->so + pdu_info->payload_size - 1) < pdu_info_cursor->so) {

                            if (previous_cursor != NULL) {
                                pdu_info_previous_cursor = &((rlc_am_rx_pdu_management_t*)(previous_cursor->data))->pdu_info;
                                if ((pdu_info_previous_cursor->sn == pdu_info_cursor->sn)) {
                                    if ((pdu_info_previous_cursor->so + pdu_info_previous_cursor->payload_size - 1) < pdu_info->so) {

                                        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d SEGMENT OFFSET %05d (vr(mr) > vr(r) and sn >= vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn, pdu_info->so);
                                        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] PREVIOUS SO %d PAYLOAD SIZE %d\n", frame, rlcP->module_id, rlcP->rb_id, pdu_info_previous_cursor->so, pdu_info_previous_cursor->payload_size);
                                        list2_insert_before_element(tbP, cursor, &rlcP->receiver_buffer);
                                        return 0;
                                    } else {
                                        LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d OVERLAP PREVIOUS SO DUPLICATE -> DROPPED\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                                        return -2;
                                    }
                                }
                            }
                            LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d SEGMENT OFFSET %05d (vr(mr) > vr(r) and sn >= vr(r))\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn, pdu_info->so);
                            list2_insert_before_element(tbP, cursor, &rlcP->receiver_buffer);
                            return 0;
                        } else if (pdu_info->so <= pdu_info_cursor->so) {
                            LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d OVERLAP SO DUPLICATE -> DROPPED\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                            return -2;
                        }
                    } else {
                        LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d DROPPED\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                        return -2;
                    }
                }
                previous_cursor = cursor;
                cursor = cursor->next;
            }
            LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d (vr(mr) > vr(r))(last inserted)\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
            // pdu_info_cursor can not be NULL here
            if  (pdu_info->sn == pdu_info_cursor->sn) {
                if ((pdu_info_cursor->so + pdu_info_cursor->payload_size - 1) < pdu_info->so) {
                    list2_add_tail(tbP, &rlcP->receiver_buffer);
                    return 0;
                } else {
                    LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d OVERLAP SO DUPLICATE -> DROPPED\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
                    return -2;
                }
            } else {
                list2_add_tail(tbP, &rlcP->receiver_buffer);
                return 0;
            }
        }
    } else {
        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d (only inserted)\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
        list2_add_head(tbP, &rlcP->receiver_buffer);
        return 0;
    }
    LOG_N(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][INSERT PDU] LINE %d RX PDU SN %04d DROPPED @4\n", frame, rlcP->module_id, rlcP->rb_id, __LINE__, pdu_info->sn);
    return -1;
}
//-----------------------------------------------------------------------------
void rlc_am_rx_check_all_byte_segments(rlc_am_entity_t* rlcP, u32_t frame, mem_block_t* tbP)
//-----------------------------------------------------------------------------
{
    rlc_am_pdu_info_t* pdu_info        = &((rlc_am_rx_pdu_management_t*)(tbP->data))->pdu_info;
    mem_block_t*       cursor;
    mem_block_t*       first_cursor;
    u16_t              sn = pdu_info->sn;
    //list2_t            list;
    u16_t              next_waited_so;
    u16_t              last_end_so;

    //msg("rlc_am_rx_check_all_byte_segments(%d) @0\n",sn);
    if (pdu_info->rf == 0) {
        ((rlc_am_rx_pdu_management_t*)(tbP->data))->all_segments_received = 1;
        return;
    }
    // for re-segmented AMD PDUS
    cursor = tbP;
    //list2_init(&list, NULL);
    //list2_add_head(cursor, &list);
    //msg("rlc_am_rx_check_all_byte_segments(%d) @1\n",sn);

    // get all previous PDU with same SN
    while (cursor->previous != NULL) {
        if (((rlc_am_rx_pdu_management_t*)(cursor->previous->data))->pdu_info.sn == sn) {
            //list2_add_head(cursor->previous, &list);
            cursor = cursor->previous;
            //msg("rlc_am_rx_check_all_byte_segments(%d) @2\n",sn);
        } else {
            break;
        }
    }
    // in case all first segments up to tbP are in list
    // the so field of the first PDU should be 0
    //cursor = list.head;
    pdu_info = &((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info;
    if (pdu_info->so != 0) {
        return;
    }
    //msg("rlc_am_rx_check_all_byte_segments(%d) @3\n",sn);
    next_waited_so = pdu_info->payload_size;
    first_cursor = cursor;
    // then check if all segments are contiguous
    last_end_so = pdu_info->payload_size;
    while (cursor->next != NULL) {
    //msg("rlc_am_rx_check_all_byte_segments(%d) @4\n",sn);
        cursor = cursor->next;
        pdu_info = &((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info;

        if (pdu_info->sn == sn) {
            // extra check normally not necessary
            if (
                !(pdu_info->rf == 1) ||
                !(pdu_info->so <= last_end_so)
                ) {
    //msg("rlc_am_rx_check_all_byte_segments(%d) @5 pdu_info->rf %d pdu_info->so %d\n",sn, pdu_info->rf, pdu_info->so);
                    return;
            } else {
                if (pdu_info->so == next_waited_so) {
                    next_waited_so = next_waited_so + pdu_info->payload_size;
    //msg("rlc_am_rx_check_all_byte_segments(%d) @6\n",sn);
                } else { // assumed pdu_info->so + pdu_info->payload_size > next_waited_so
                    next_waited_so = (next_waited_so + pdu_info->payload_size) - (next_waited_so - pdu_info->so);
    //msg("rlc_am_rx_check_all_byte_segments(%d) @7\n",sn);
                }
                if (pdu_info->lsf > 0) {
    //msg("rlc_am_rx_check_all_byte_segments(%d) @8\n",sn);
		  rlc_am_rx_mark_all_segments_received(rlcP, frame, first_cursor);
                }
            }
            last_end_so = pdu_info->so + pdu_info->payload_size;
        } else {
    //msg("rlc_am_rx_check_all_byte_segments(%d) @9\n",sn);
            return;
        }
    }
}
//-----------------------------------------------------------------------------
void rlc_am_rx_mark_all_segments_received(rlc_am_entity_t* rlcP, u32_t frame, mem_block_t* fisrt_segment_tbP)
//-----------------------------------------------------------------------------
{
    rlc_am_pdu_info_t* pdu_info        = &((rlc_am_rx_pdu_management_t*)(fisrt_segment_tbP->data))->pdu_info;
    rlc_am_pdu_info_t* pdu_info_cursor;
    mem_block_t*       cursor;
    u16_t              sn = pdu_info->sn;

    cursor = fisrt_segment_tbP;
    if (cursor) {
        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][PROCESS RX PDU] ALL SEGMENTS RECEIVED SN %04d:\n", frame, rlcP->module_id, rlcP->rb_id, sn);
        do {
            pdu_info_cursor = &((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info;
            if (pdu_info_cursor->sn == sn) {
                ((rlc_am_rx_pdu_management_t*)(cursor->data))->all_segments_received = 1;
            } else {
                return;
            }
            cursor = cursor->next;
        } while (cursor != NULL);
    }
}
//-----------------------------------------------------------------------------
void rlc_am_rx_list_reassemble_rlc_sdus(rlc_am_entity_t* rlcP,u32_t frame, u8_t eNB_flag)
//-----------------------------------------------------------------------------
{
    mem_block_t*                cursor;
    rlc_am_rx_pdu_management_t* rlc_am_rx_old_pdu_management;

    cursor = list2_get_head(&rlcP->receiver_buffer);
    if (cursor == NULL) {
        return;
    }

    rlc_am_rx_pdu_management_t* rlc_am_rx_pdu_management = ((rlc_am_rx_pdu_management_t*)(cursor->data));

    do {
        if (rlc_am_rx_pdu_management->all_segments_received > 0) {
            cursor = list2_remove_head(&rlcP->receiver_buffer);
            rlc_am_reassemble_pdu(rlcP, frame, eNB_flag, cursor);
            rlc_am_rx_old_pdu_management = rlc_am_rx_pdu_management;
            cursor = list2_get_head(&rlcP->receiver_buffer);
            if (cursor == NULL) {
                return;
            }
            rlc_am_rx_pdu_management = ((rlc_am_rx_pdu_management_t*)(cursor->data));
        } else {
            return;
        }

    } while ((rlc_am_rx_pdu_management->pdu_info.sn == ((rlc_am_rx_old_pdu_management->pdu_info.sn + 1) & RLC_AM_SN_MASK)) || ((rlc_am_rx_pdu_management->pdu_info.sn == rlc_am_rx_old_pdu_management->pdu_info.sn) && (rlc_am_rx_pdu_management->all_segments_received > 0)));
}
//-----------------------------------------------------------------------------
mem_block_t *
list2_insert_before_element (mem_block_t * element_to_insertP, mem_block_t * elementP, list2_t * listP)
//-----------------------------------------------------------------------------
{
    //msg ("list2_insert_before_element\n");
    //check_mem_area(NULL);
    if ((element_to_insertP != NULL) && (elementP != NULL)) {
        listP->nb_elements = listP->nb_elements + 1;
        mem_block_t *previous = elementP->previous;
        element_to_insertP->previous = previous;
        element_to_insertP->next     = elementP;
        elementP->previous           = element_to_insertP;
        if (previous != NULL) {
            previous->next = element_to_insertP;
        } else if (listP->head == elementP) {
            listP->head = element_to_insertP;
        }
        //check_mem_area(NULL);
        return element_to_insertP;
    } else {
        assert(2==1);
        return NULL;
    }
}
//-----------------------------------------------------------------------------
mem_block_t *
list2_insert_after_element (mem_block_t * element_to_insertP, mem_block_t * elementP, list2_t * listP)
//-----------------------------------------------------------------------------
{
    //msg ("list2_insert_after_element\n");
    //check_mem_area(NULL);

    if ((element_to_insertP != NULL) && (elementP != NULL)) {
        listP->nb_elements = listP->nb_elements + 1;
        mem_block_t *next = elementP->next;
        element_to_insertP->previous = elementP;
        element_to_insertP->next     = next;
        elementP->next               = element_to_insertP;
        if (next != NULL) {
            next->previous = element_to_insertP;
        } else if (listP->tail == elementP) {
            listP->tail = element_to_insertP;
        }
        //check_mem_area(NULL);
        return element_to_insertP;
    } else {
        assert(2==1);
        return NULL;
    }
}
//-----------------------------------------------------------------------------
void rlc_am_rx_list_display (rlc_am_entity_t* rlcP, char* messageP)
//-----------------------------------------------------------------------------
{
    mem_block_t      *cursor;
    unsigned int     loop = 0;

    cursor = rlcP->receiver_buffer.head;
    if (messageP) {
        LOG_T(RLC, "Display list %s %s VR(R)=%04d:\n", rlcP->receiver_buffer.name, messageP, rlcP->vr_r);
    } else {
        LOG_T(RLC, "Display list %s VR(R)=%04d:\n", rlcP->receiver_buffer.name, rlcP->vr_r);
    }
    if (cursor) {
    // almost one element
        while (cursor != NULL) {
            //if (((loop % 16) == 0) && (loop > 0)) {
            if ((loop % 4) == 0) {
                LOG_T(RLC, "\nRX SN:\t");
            }
            if (((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info.rf) {
                if (((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info.lsf) {
                    LOG_T(RLC, "%04d (%04d->%04d LSF)\t",
                        ((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info.sn,
                        ((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info.so,
                        ((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info.so + ((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info.payload_size - 1);
                } else {
                    LOG_T(RLC, "%04d (%04d->%04d)\t",
                        ((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info.sn,
                        ((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info.so,
                        ((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info.so + ((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info.payload_size - 1);
                }
            } else {
                LOG_T(RLC, "%04d (%04d NOSEG)\t",
                    ((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info.sn,
                    ((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info.payload_size);
            }
            //if (cursor == cursor->next) {
            //    rlc_am_v9_3_0_test_print_trace();
            //}
            assert(cursor != cursor->next);
            cursor = cursor->next;
            loop++;
        }
        LOG_T(RLC, "\n");
    } else {
        LOG_T(RLC, "\nNO ELEMENTS\n");
    }
}

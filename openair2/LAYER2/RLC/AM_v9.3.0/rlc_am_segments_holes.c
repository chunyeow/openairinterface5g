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
#define RLC_AM_SEGMENT_HOLES_C
//#include "rtos_header.h"
//-----------------------------------------------------------------------------
#include "rlc_am.h"
#include "LAYER2/MAC/extern.h"
#include "UTIL/LOG/log.h"
#define TRACE_RLC_AM_HOLE

//-----------------------------------------------------------------------------
void rlc_am_clear_holes (rlc_am_entity_t *rlcP, u16_t snP)
//-----------------------------------------------------------------------------
{
    rlcP->pdu_retrans_buffer[snP].num_holes         = 0;
}
//-----------------------------------------------------------------------------
void rlc_am_shift_down_holes (rlc_am_entity_t *rlcP, u16_t snP, int indexP)
//-----------------------------------------------------------------------------
{
    int i;
    for (i=indexP; i < rlcP->pdu_retrans_buffer[snP].num_holes - 1; i++) {
        rlcP->pdu_retrans_buffer[snP].hole_so_start[i]   = rlcP->pdu_retrans_buffer[snP].hole_so_start[i+1];
        rlcP->pdu_retrans_buffer[snP].hole_so_stop[i]    = rlcP->pdu_retrans_buffer[snP].hole_so_stop[i+1];
    }
    rlcP->pdu_retrans_buffer[snP].num_holes =  rlcP->pdu_retrans_buffer[snP].num_holes - 1;
}
//-----------------------------------------------------------------------------
void rlc_am_shift_up_holes (rlc_am_entity_t *rlcP, u16_t snP, int indexP)
//-----------------------------------------------------------------------------
{
    // shift include indexP
    int i;
    for (i=rlcP->pdu_retrans_buffer[snP].num_holes; i > indexP; i--) {
        rlcP->pdu_retrans_buffer[snP].hole_so_start[i]   = rlcP->pdu_retrans_buffer[snP].hole_so_start[i-1];
        rlcP->pdu_retrans_buffer[snP].hole_so_stop[i]    = rlcP->pdu_retrans_buffer[snP].hole_so_stop[i-1];
    }
    rlcP->pdu_retrans_buffer[snP].num_holes =  rlcP->pdu_retrans_buffer[snP].num_holes + 1;
    assert(rlcP->pdu_retrans_buffer[snP].num_holes < RLC_AM_MAX_HOLES_REPORT_PER_PDU);
}
//-----------------------------------------------------------------------------
void rlc_am_remove_hole (rlc_am_entity_t *rlcP, u32_t frame, u16_t snP, u16_t so_startP, u16_t so_stopP)
//-----------------------------------------------------------------------------
{
    int i;
#ifdef TRACE_RLC_AM_HOLE
    LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][HOLE] REMOVE HOLE SN %04d  so_startP %05d so_stopP %05d rlcP->pdu_retrans_buffer[snP].nack_so_start %05d rlcP->pdu_retrans_buffer[snP].nack_so_stop %05d\n",
             frame,rlcP->module_id, rlcP->rb_id, snP, so_startP, so_stopP, rlcP->pdu_retrans_buffer[snP].nack_so_start, rlcP->pdu_retrans_buffer[snP].nack_so_stop);
#endif
    assert(so_startP <= so_stopP);

    if (rlcP->pdu_retrans_buffer[snP].num_holes == 0) {
        assert(so_startP == rlcP->pdu_retrans_buffer[snP].nack_so_start);
        assert(so_stopP  <= rlcP->pdu_retrans_buffer[snP].nack_so_stop);
#ifdef TRACE_RLC_AM_HOLE
        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][HOLE] REMOVE HOLE SN %04d  MODIFIED nack_so_start %05d->%05d\n",
             frame,rlcP->module_id, rlcP->rb_id, snP, rlcP->pdu_retrans_buffer[snP].nack_so_start, so_stopP+1);
#endif
        rlcP->pdu_retrans_buffer[snP].nack_so_start = so_stopP+1;
        if (rlcP->pdu_retrans_buffer[snP].nack_so_start >= rlcP->pdu_retrans_buffer[snP].nack_so_stop) {
            rlcP->pdu_retrans_buffer[snP].nack_so_start = 0;
            rlcP->pdu_retrans_buffer[snP].nack_so_stop  = 0x7FFF;
        }
    } else {
        // normally should be removed in increasing order...
        for (i = 0; i < rlcP->pdu_retrans_buffer[snP].num_holes; i++) {
            if (so_startP <= rlcP->pdu_retrans_buffer[snP].hole_so_start[i]) {
                if (so_stopP >= rlcP->pdu_retrans_buffer[snP].hole_so_stop[i]) {
                    rlc_am_shift_down_holes(rlcP, snP, i);
                    i = i - 1;
                } else {
                    rlcP->pdu_retrans_buffer[snP].hole_so_start[i] = so_stopP;
                    if (rlcP->pdu_retrans_buffer[snP].num_holes == 0) {
                        rlcP->pdu_retrans_buffer[snP].nack_so_start = 0;
                        rlcP->pdu_retrans_buffer[snP].nack_so_stop  = 0x7FFF;
                    } else {
                        rlcP->pdu_retrans_buffer[snP].nack_so_start = rlcP->pdu_retrans_buffer[snP].hole_so_start[0];
                        rlcP->pdu_retrans_buffer[snP].nack_so_stop  = rlcP->pdu_retrans_buffer[snP].hole_so_stop[rlcP->pdu_retrans_buffer[snP].num_holes - 1];
                    }
#ifdef TRACE_RLC_AM_HOLE
                    LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][HOLE] REMOVE HOLE SN %04d  NOW nack_so_start %05d nack_so_stop %05d num holes %d\n", frame,rlcP->module_id, rlcP->rb_id, snP, rlcP->pdu_retrans_buffer[snP].nack_so_start, rlcP->pdu_retrans_buffer[snP].nack_so_stop, rlcP->pdu_retrans_buffer[snP].num_holes);
#endif
                    return;
                }
            } else if (so_startP > rlcP->pdu_retrans_buffer[snP].hole_so_start[i]) {
                if (so_startP <= rlcP->pdu_retrans_buffer[snP].hole_so_stop[i]) {
                    if (so_stopP < rlcP->pdu_retrans_buffer[snP].hole_so_stop[i]) {
                        // BAD CASE: 1 HOLE IS SPLITTED IN 2 HOLES
                        rlc_am_shift_up_holes(rlcP, snP, i+1);
                        rlcP->pdu_retrans_buffer[snP].hole_so_start[i+1] = so_startP+1;
                        rlcP->pdu_retrans_buffer[snP].hole_so_stop[i+1] = rlcP->pdu_retrans_buffer[snP].hole_so_stop[i];

                        rlcP->pdu_retrans_buffer[snP].hole_so_stop[i] = so_startP - 1;
                    } else {
                        rlcP->pdu_retrans_buffer[snP].hole_so_stop[i] = so_startP;
                    }
                }
            }
        }
        if (rlcP->pdu_retrans_buffer[snP].num_holes == 0) {
            rlcP->pdu_retrans_buffer[snP].nack_so_start = 0;
            rlcP->pdu_retrans_buffer[snP].nack_so_stop  = 0x7FFF;
        } else {
            rlcP->pdu_retrans_buffer[snP].nack_so_start = rlcP->pdu_retrans_buffer[snP].hole_so_start[0];
            rlcP->pdu_retrans_buffer[snP].nack_so_stop  = rlcP->pdu_retrans_buffer[snP].hole_so_stop[rlcP->pdu_retrans_buffer[snP].num_holes - 1];
        }
    }
#ifdef TRACE_RLC_AM_HOLE
    LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][HOLE] REMOVE HOLE SN %04d  NOW nack_so_start %05d nack_so_stop %05d num holes %d\n", frame,rlcP->module_id, rlcP->rb_id, snP, rlcP->pdu_retrans_buffer[snP].nack_so_start, rlcP->pdu_retrans_buffer[snP].nack_so_stop, rlcP->pdu_retrans_buffer[snP].num_holes);
#endif
    assert(rlcP->pdu_retrans_buffer[snP].nack_so_start < rlcP->pdu_retrans_buffer[snP].payload_size);
}
//-----------------------------------------------------------------------------
void rlc_am_get_next_hole (rlc_am_entity_t *rlcP, u32_t frame, u16_t snP, int* so_startP, int* so_stopP)
//-----------------------------------------------------------------------------
{
    if (rlcP->pdu_retrans_buffer[snP].num_holes == 0) {
        *so_startP = rlcP->pdu_retrans_buffer[snP].nack_so_start;
        *so_stopP  = rlcP->pdu_retrans_buffer[snP].nack_so_stop;
#ifdef TRACE_RLC_AM_HOLE
        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][HOLE] rlc_am_get_next_hole(SN %04d) %05d->%05d (NUM HOLES == 0)\n",
             frame,rlcP->module_id, rlcP->rb_id, snP, *so_startP, *so_stopP);
#endif
    } else {
        *so_startP = rlcP->pdu_retrans_buffer[snP].hole_so_start[0];
        *so_stopP  = rlcP->pdu_retrans_buffer[snP].hole_so_stop[0];
#ifdef TRACE_RLC_AM_HOLE
        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][HOLE] rlc_am_get_next_hole(SN %04d) %05d->%05d (NUM HOLES == %d)\n",
             frame,rlcP->module_id, rlcP->rb_id, snP, *so_startP, *so_stopP, rlcP->pdu_retrans_buffer[snP].num_holes);
#endif
    }
}
//-----------------------------------------------------------------------------
void rlc_am_add_hole (rlc_am_entity_t *rlcP, u32_t frame, u16_t snP, u16_t so_startP, u16_t so_stopP)
//-----------------------------------------------------------------------------
{
    int i, hole_index;

    assert(so_startP <= so_stopP);
    assert(so_startP < 0x7FFF);
    assert(so_stopP  <= 0x7FFF);


    // if global NACK
    if ((so_startP == 0) && ((so_stopP == 0x7FFF) || (so_stopP == rlcP->pdu_retrans_buffer[snP].payload_size - 1))) {
        rlcP->pdu_retrans_buffer[snP].num_holes         = 0;
        rlcP->pdu_retrans_buffer[snP].nack_so_start = so_startP;
        rlcP->pdu_retrans_buffer[snP].nack_so_stop  = so_stopP;
#ifdef TRACE_RLC_AM_HOLE
        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][HOLE] SN %04d GLOBAL NACK 0->%05d\n",
             frame,rlcP->module_id, rlcP->rb_id, snP, so_stopP);
#endif
        assert(rlcP->pdu_retrans_buffer[snP].nack_so_start < rlcP->pdu_retrans_buffer[snP].payload_size);
        return;
    }

    if (so_stopP == 0x7FFF) {
        so_stopP = rlcP->pdu_retrans_buffer[snP].payload_size - 1;
    }
    // first hole
    if (rlcP->pdu_retrans_buffer[snP].num_holes == 0) {
        rlcP->pdu_retrans_buffer[snP].hole_so_start[0] = so_startP;
        rlcP->pdu_retrans_buffer[snP].hole_so_stop [0] = so_stopP;

        rlcP->pdu_retrans_buffer[snP].num_holes         = 1;

        rlcP->pdu_retrans_buffer[snP].nack_so_start = so_startP;
        rlcP->pdu_retrans_buffer[snP].nack_so_stop  = so_stopP;
#ifdef TRACE_RLC_AM_HOLE
        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][HOLE] FIRST HOLE SN %04d GLOBAL NACK %05d->%05d\n",
             frame,rlcP->module_id, rlcP->rb_id, snP, so_startP, so_stopP);
#endif
        assert(rlcP->pdu_retrans_buffer[snP].nack_so_start < rlcP->pdu_retrans_buffer[snP].payload_size);
        return;
    }

    hole_index = 0;
    while (hole_index < rlcP->pdu_retrans_buffer[snP].num_holes) {
        if (so_stopP < rlcP->pdu_retrans_buffer[snP].hole_so_start[hole_index]) {
            assert(rlcP->pdu_retrans_buffer[snP].num_holes < RLC_AM_MAX_HOLES_REPORT_PER_PDU);
            if (hole_index > 0) {
                assert(so_startP > rlcP->pdu_retrans_buffer[snP].hole_so_stop[hole_index-1]);
            }
            for (i=rlcP->pdu_retrans_buffer[snP].num_holes; i >= hole_index; i--) {
                rlcP->pdu_retrans_buffer[snP].hole_so_start[i]   = rlcP->pdu_retrans_buffer[snP].hole_so_start[i-1];
                rlcP->pdu_retrans_buffer[snP].hole_so_stop[i]    = rlcP->pdu_retrans_buffer[snP].hole_so_stop[i-1];
            }
            rlcP->pdu_retrans_buffer[snP].hole_so_start[hole_index]   = so_startP;
            rlcP->pdu_retrans_buffer[snP].hole_so_stop[hole_index]    = so_stopP;

            // update nack "window" vars nack_so_start, nack_so_stop
            if (hole_index == 0) {
                rlcP->pdu_retrans_buffer[snP].nack_so_start = so_startP;
            }
            rlcP->pdu_retrans_buffer[snP].num_holes += 1;
#ifdef TRACE_RLC_AM_HOLE
        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][HOLE] INSERT %d th HOLE SN %04d GLOBAL NACK %05d->%05d\n",
             frame,rlcP->module_id, rlcP->rb_id, rlcP->pdu_retrans_buffer[snP].num_holes, snP, so_startP, so_stopP);
#endif
            assert(rlcP->pdu_retrans_buffer[snP].nack_so_start < rlcP->pdu_retrans_buffer[snP].payload_size);
            assert(rlcP->pdu_retrans_buffer[snP].num_holes < RLC_AM_MAX_HOLES_REPORT_PER_PDU);
            return;
        }
        hole_index = hole_index + 1;
    }

    // if here insert to the "tail"
    if (so_startP > rlcP->pdu_retrans_buffer[snP].hole_so_stop[hole_index - 1]) {
        assert(rlcP->pdu_retrans_buffer[snP].num_holes < RLC_AM_MAX_HOLES_REPORT_PER_PDU);
        rlcP->pdu_retrans_buffer[snP].hole_so_start[hole_index]   = so_startP;
        rlcP->pdu_retrans_buffer[snP].hole_so_stop[hole_index]    = so_stopP;
        rlcP->pdu_retrans_buffer[snP].num_holes += 1;
        // update nack "window" vars nack_so_start, nack_so_stop
        rlcP->pdu_retrans_buffer[snP].nack_so_stop = so_stopP;
#ifdef TRACE_RLC_AM_HOLE
        LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][HOLE] INSERT THE %d th LAST HOLE SN %04d GLOBAL NACK %05d->%05d\n",
             frame,rlcP->module_id, rlcP->rb_id, rlcP->pdu_retrans_buffer[snP].num_holes, snP, so_startP, so_stopP);
#endif
    } else {
        assert(1==2);
    }
    assert(rlcP->pdu_retrans_buffer[snP].num_holes < RLC_AM_MAX_HOLES_REPORT_PER_PDU);
    assert(rlcP->pdu_retrans_buffer[snP].nack_so_start < rlcP->pdu_retrans_buffer[snP].payload_size);
}

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
#define RLC_AM_MODULE
#define RLC_AM_SEGMENT_HOLES_C
//-----------------------------------------------------------------------------
#include "rlc_am.h"
#include "LAYER2/MAC/extern.h"
#include "UTIL/LOG/log.h"
#define TRACE_RLC_AM_HOLE

//-----------------------------------------------------------------------------
void rlc_am_clear_holes (
        rlc_am_entity_t * const rlc_pP,
        const rlc_sn_t snP)
//-----------------------------------------------------------------------------
{
    rlc_pP->pdu_retrans_buffer[snP].num_holes         = 0;
}
//-----------------------------------------------------------------------------
void rlc_am_shift_down_holes (
        rlc_am_entity_t *const rlc_pP,
        const rlc_sn_t snP,
        const int indexP)
//-----------------------------------------------------------------------------
{
    int i;
    for (i=indexP; i < rlc_pP->pdu_retrans_buffer[snP].num_holes - 1; i++) {
        rlc_pP->pdu_retrans_buffer[snP].hole_so_start[i]   = rlc_pP->pdu_retrans_buffer[snP].hole_so_start[i+1];
        rlc_pP->pdu_retrans_buffer[snP].hole_so_stop[i]    = rlc_pP->pdu_retrans_buffer[snP].hole_so_stop[i+1];
    }
    rlc_pP->pdu_retrans_buffer[snP].num_holes =  rlc_pP->pdu_retrans_buffer[snP].num_holes - 1;
}
//-----------------------------------------------------------------------------
void rlc_am_shift_up_holes (
        rlc_am_entity_t *const rlc_pP,
        const rlc_sn_t snP,
        const int indexP)
//-----------------------------------------------------------------------------
{
    // shift include indexP
    int i;
    for (i=rlc_pP->pdu_retrans_buffer[snP].num_holes; i > indexP; i--) {
        rlc_pP->pdu_retrans_buffer[snP].hole_so_start[i]   = rlc_pP->pdu_retrans_buffer[snP].hole_so_start[i-1];
        rlc_pP->pdu_retrans_buffer[snP].hole_so_stop[i]    = rlc_pP->pdu_retrans_buffer[snP].hole_so_stop[i-1];
    }
    rlc_pP->pdu_retrans_buffer[snP].num_holes =  rlc_pP->pdu_retrans_buffer[snP].num_holes + 1;
    assert(rlc_pP->pdu_retrans_buffer[snP].num_holes < RLC_AM_MAX_HOLES_REPORT_PER_PDU);
}
//-----------------------------------------------------------------------------
void rlc_am_remove_hole (
        rlc_am_entity_t *const rlc_pP,
        const frame_t frameP,
        const rlc_sn_t snP,
        const sdu_size_t so_startP,
        const sdu_size_t so_stopP)
//-----------------------------------------------------------------------------
{
    int i;
#ifdef TRACE_RLC_AM_HOLE
    LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u][HOLE] REMOVE HOLE SN %04d  so_startP %05d so_stopP %05d rlc_pP->pdu_retrans_buffer[snP].nack_so_start %05d rlc_pP->pdu_retrans_buffer[snP].nack_so_stop %05d\n",
             frameP,
             (rlc_pP->is_enb) ? "eNB" : "UE",
             rlc_pP->enb_module_id,
             rlc_pP->ue_module_id,
             rlc_pP->rb_id,
             snP,
             so_startP,
             so_stopP,
             rlc_pP->pdu_retrans_buffer[snP].nack_so_start,
             rlc_pP->pdu_retrans_buffer[snP].nack_so_stop);
#endif
    assert(so_startP <= so_stopP);

    if (rlc_pP->pdu_retrans_buffer[snP].num_holes == 0) {
        assert(so_startP == rlc_pP->pdu_retrans_buffer[snP].nack_so_start);
        assert(so_stopP  <= rlc_pP->pdu_retrans_buffer[snP].nack_so_stop);
#ifdef TRACE_RLC_AM_HOLE
        LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u][HOLE] REMOVE HOLE SN %04d  MODIFIED nack_so_start %05d->%05d\n",
             frameP,
             (rlc_pP->is_enb) ? "eNB" : "UE",
             rlc_pP->enb_module_id,
             rlc_pP->ue_module_id,
             rlc_pP->rb_id,
             snP,
             rlc_pP->pdu_retrans_buffer[snP].nack_so_start,
             so_stopP+1);
#endif
        rlc_pP->pdu_retrans_buffer[snP].nack_so_start = so_stopP+1;
        if (rlc_pP->pdu_retrans_buffer[snP].nack_so_start >= rlc_pP->pdu_retrans_buffer[snP].nack_so_stop) {
            rlc_pP->pdu_retrans_buffer[snP].nack_so_start = 0;
            rlc_pP->pdu_retrans_buffer[snP].nack_so_stop  = 0x7FFF;
        }
    } else {
        // normally should be removed in increasing order...
        for (i = 0; i < rlc_pP->pdu_retrans_buffer[snP].num_holes; i++) {
            if (so_startP <= rlc_pP->pdu_retrans_buffer[snP].hole_so_start[i]) {
                if (so_stopP >= rlc_pP->pdu_retrans_buffer[snP].hole_so_stop[i]) {
                    rlc_am_shift_down_holes(rlc_pP, snP, i);
                    i = i - 1;
                } else {
                    rlc_pP->pdu_retrans_buffer[snP].hole_so_start[i] = so_stopP;
                    if (rlc_pP->pdu_retrans_buffer[snP].num_holes == 0) {
                        rlc_pP->pdu_retrans_buffer[snP].nack_so_start = 0;
                        rlc_pP->pdu_retrans_buffer[snP].nack_so_stop  = 0x7FFF;
                    } else {
                        rlc_pP->pdu_retrans_buffer[snP].nack_so_start = rlc_pP->pdu_retrans_buffer[snP].hole_so_start[0];
                        rlc_pP->pdu_retrans_buffer[snP].nack_so_stop  = rlc_pP->pdu_retrans_buffer[snP].hole_so_stop[rlc_pP->pdu_retrans_buffer[snP].num_holes - 1];
                    }
#ifdef TRACE_RLC_AM_HOLE
                    LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u][HOLE] REMOVE HOLE SN %04d  NOW nack_so_start %05d nack_so_stop %05d num holes %d\n",
                          frameP,
                          (rlc_pP->is_enb) ? "eNB" : "UE",
                          rlc_pP->enb_module_id,
                          rlc_pP->ue_module_id,
                          rlc_pP->rb_id,
                          snP,
                          rlc_pP->pdu_retrans_buffer[snP].nack_so_start,
                          rlc_pP->pdu_retrans_buffer[snP].nack_so_stop,
                          rlc_pP->pdu_retrans_buffer[snP].num_holes);
#endif
                    return;
                }
            } else if (so_startP > rlc_pP->pdu_retrans_buffer[snP].hole_so_start[i]) {
                if (so_startP <= rlc_pP->pdu_retrans_buffer[snP].hole_so_stop[i]) {
                    if (so_stopP < rlc_pP->pdu_retrans_buffer[snP].hole_so_stop[i]) {
                        // BAD CASE: 1 HOLE IS SPLITTED IN 2 HOLES
                        rlc_am_shift_up_holes(rlc_pP, snP, i+1);
                        rlc_pP->pdu_retrans_buffer[snP].hole_so_start[i+1] = so_startP+1;
                        rlc_pP->pdu_retrans_buffer[snP].hole_so_stop[i+1] = rlc_pP->pdu_retrans_buffer[snP].hole_so_stop[i];

                        rlc_pP->pdu_retrans_buffer[snP].hole_so_stop[i] = so_startP - 1;
                    } else {
                        rlc_pP->pdu_retrans_buffer[snP].hole_so_stop[i] = so_startP;
                    }
                }
            }
        }
        if (rlc_pP->pdu_retrans_buffer[snP].num_holes == 0) {
            rlc_pP->pdu_retrans_buffer[snP].nack_so_start = 0;
            rlc_pP->pdu_retrans_buffer[snP].nack_so_stop  = 0x7FFF;
        } else {
            rlc_pP->pdu_retrans_buffer[snP].nack_so_start = rlc_pP->pdu_retrans_buffer[snP].hole_so_start[0];
            rlc_pP->pdu_retrans_buffer[snP].nack_so_stop  = rlc_pP->pdu_retrans_buffer[snP].hole_so_stop[rlc_pP->pdu_retrans_buffer[snP].num_holes - 1];
        }
    }
#ifdef TRACE_RLC_AM_HOLE
    LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u][HOLE] REMOVE HOLE SN %04d  NOW nack_so_start %05d nack_so_stop %05d num holes %d\n",
          frameP,
          (rlc_pP->is_enb) ? "eNB" : "UE",
          rlc_pP->enb_module_id,
          rlc_pP->ue_module_id,
          rlc_pP->rb_id,
          snP,
          rlc_pP->pdu_retrans_buffer[snP].nack_so_start,
          rlc_pP->pdu_retrans_buffer[snP].nack_so_stop,
          rlc_pP->pdu_retrans_buffer[snP].num_holes);
#endif
    assert(rlc_pP->pdu_retrans_buffer[snP].nack_so_start < rlc_pP->pdu_retrans_buffer[snP].payload_size);
}
//-----------------------------------------------------------------------------
void rlc_am_get_next_hole (
        rlc_am_entity_t *const rlc_pP,
        const frame_t frameP,
        const rlc_sn_t snP,
        sdu_size_t* const so_startP,
        sdu_size_t* const so_stopP)
//-----------------------------------------------------------------------------
{
    if (rlc_pP->pdu_retrans_buffer[snP].num_holes == 0) {
        *so_startP = rlc_pP->pdu_retrans_buffer[snP].nack_so_start;
        *so_stopP  = rlc_pP->pdu_retrans_buffer[snP].nack_so_stop;
#ifdef TRACE_RLC_AM_HOLE
        LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u][HOLE] rlc_am_get_next_hole(SN %04d) %05d->%05d (NUM HOLES == 0)\n",
             frameP,
             (rlc_pP->is_enb) ? "eNB" : "UE",
             rlc_pP->enb_module_id,
             rlc_pP->ue_module_id,
             rlc_pP->rb_id,
             snP,
             *so_startP,
             *so_stopP);
#endif
    } else {
        *so_startP = rlc_pP->pdu_retrans_buffer[snP].hole_so_start[0];
        *so_stopP  = rlc_pP->pdu_retrans_buffer[snP].hole_so_stop[0];
#ifdef TRACE_RLC_AM_HOLE
        LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u][HOLE] rlc_am_get_next_hole(SN %04d) %05d->%05d (NUM HOLES == %d)\n",
             frameP,
             (rlc_pP->is_enb) ? "eNB" : "UE",
             rlc_pP->enb_module_id,
             rlc_pP->ue_module_id,
             rlc_pP->rb_id,
             snP,
             *so_startP,
             *so_stopP,
             rlc_pP->pdu_retrans_buffer[snP].num_holes);
#endif
    }
}
//-----------------------------------------------------------------------------
void rlc_am_add_hole (
        rlc_am_entity_t *const rlc_pP,
        const frame_t frameP,
        const rlc_sn_t snP,
        sdu_size_t so_startP,
        sdu_size_t so_stopP)
//-----------------------------------------------------------------------------
{
    int i, hole_index;

    assert(so_startP <= so_stopP);
    assert(so_startP < 0x7FFF);
    assert(so_stopP  <= 0x7FFF);


    // if global NACK
    if ((so_startP == 0) && ((so_stopP == 0x7FFF) || (so_stopP == rlc_pP->pdu_retrans_buffer[snP].payload_size - 1))) {
        rlc_pP->pdu_retrans_buffer[snP].num_holes         = 0;
        rlc_pP->pdu_retrans_buffer[snP].nack_so_start = so_startP;
        rlc_pP->pdu_retrans_buffer[snP].nack_so_stop  = so_stopP;
#ifdef TRACE_RLC_AM_HOLE
        LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u][HOLE] SN %04d GLOBAL NACK 0->%05d\n",
             frameP,
             (rlc_pP->is_enb) ? "eNB" : "UE",
             rlc_pP->enb_module_id,
             rlc_pP->ue_module_id,
             rlc_pP->rb_id,
             snP,
             so_stopP);
#endif
        assert(rlc_pP->pdu_retrans_buffer[snP].nack_so_start < rlc_pP->pdu_retrans_buffer[snP].payload_size);
        return;
    }

    if (so_stopP == 0x7FFF) {
        so_stopP = rlc_pP->pdu_retrans_buffer[snP].payload_size - 1;
    }
    // first hole
    if (rlc_pP->pdu_retrans_buffer[snP].num_holes == 0) {
        rlc_pP->pdu_retrans_buffer[snP].hole_so_start[0] = so_startP;
        rlc_pP->pdu_retrans_buffer[snP].hole_so_stop [0] = so_stopP;

        rlc_pP->pdu_retrans_buffer[snP].num_holes         = 1;

        rlc_pP->pdu_retrans_buffer[snP].nack_so_start = so_startP;
        rlc_pP->pdu_retrans_buffer[snP].nack_so_stop  = so_stopP;
#ifdef TRACE_RLC_AM_HOLE
        LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u][HOLE] FIRST HOLE SN %04d GLOBAL NACK %05d->%05d\n",
             frameP,
             (rlc_pP->is_enb) ? "eNB" : "UE",
             rlc_pP->enb_module_id,
             rlc_pP->ue_module_id,
             rlc_pP->rb_id,
             snP,
             so_startP,
             so_stopP);
#endif
        assert(rlc_pP->pdu_retrans_buffer[snP].nack_so_start < rlc_pP->pdu_retrans_buffer[snP].payload_size);
        return;
    }

    hole_index = 0;
    while (hole_index < rlc_pP->pdu_retrans_buffer[snP].num_holes) {
        if (so_stopP < rlc_pP->pdu_retrans_buffer[snP].hole_so_start[hole_index]) {
            assert(rlc_pP->pdu_retrans_buffer[snP].num_holes < RLC_AM_MAX_HOLES_REPORT_PER_PDU);
            if (hole_index > 0) {
                assert(so_startP > rlc_pP->pdu_retrans_buffer[snP].hole_so_stop[hole_index-1]);
            }
            for (i=rlc_pP->pdu_retrans_buffer[snP].num_holes; i >= hole_index; i--) {
                rlc_pP->pdu_retrans_buffer[snP].hole_so_start[i]   = rlc_pP->pdu_retrans_buffer[snP].hole_so_start[i-1];
                rlc_pP->pdu_retrans_buffer[snP].hole_so_stop[i]    = rlc_pP->pdu_retrans_buffer[snP].hole_so_stop[i-1];
            }
            rlc_pP->pdu_retrans_buffer[snP].hole_so_start[hole_index]   = so_startP;
            rlc_pP->pdu_retrans_buffer[snP].hole_so_stop[hole_index]    = so_stopP;

            // update nack "window" vars nack_so_start, nack_so_stop
            if (hole_index == 0) {
                rlc_pP->pdu_retrans_buffer[snP].nack_so_start = so_startP;
            }
            rlc_pP->pdu_retrans_buffer[snP].num_holes += 1;
#ifdef TRACE_RLC_AM_HOLE
        LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u][HOLE] INSERT %d th HOLE SN %04d GLOBAL NACK %05d->%05d\n",
             frameP,
             (rlc_pP->is_enb) ? "eNB" : "UE",
             rlc_pP->enb_module_id,
             rlc_pP->ue_module_id,
             rlc_pP->rb_id,
             rlc_pP->pdu_retrans_buffer[snP].num_holes,
             snP,
             so_startP,
             so_stopP);
#endif
            assert(rlc_pP->pdu_retrans_buffer[snP].nack_so_start < rlc_pP->pdu_retrans_buffer[snP].payload_size);
            assert(rlc_pP->pdu_retrans_buffer[snP].num_holes < RLC_AM_MAX_HOLES_REPORT_PER_PDU);
            return;
        }
        hole_index = hole_index + 1;
    }

    // if here insert to the "tail"
    if (so_startP > rlc_pP->pdu_retrans_buffer[snP].hole_so_stop[hole_index - 1]) {
        assert(rlc_pP->pdu_retrans_buffer[snP].num_holes < RLC_AM_MAX_HOLES_REPORT_PER_PDU);
        rlc_pP->pdu_retrans_buffer[snP].hole_so_start[hole_index]   = so_startP;
        rlc_pP->pdu_retrans_buffer[snP].hole_so_stop[hole_index]    = so_stopP;
        rlc_pP->pdu_retrans_buffer[snP].num_holes += 1;
        // update nack "window" vars nack_so_start, nack_so_stop
        rlc_pP->pdu_retrans_buffer[snP].nack_so_stop = so_stopP;
#ifdef TRACE_RLC_AM_HOLE
        LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u][HOLE] INSERT THE %d th LAST HOLE SN %04d GLOBAL NACK %05d->%05d\n",
             frameP,
             (rlc_pP->is_enb) ? "eNB" : "UE",
             rlc_pP->enb_module_id,
             rlc_pP->ue_module_id,
             rlc_pP->rb_id,
             rlc_pP->pdu_retrans_buffer[snP].num_holes,
             snP,
             so_startP,
             so_stopP);
#endif
    } else {
        assert(1==2);
    }
    assert(rlc_pP->pdu_retrans_buffer[snP].num_holes < RLC_AM_MAX_HOLES_REPORT_PER_PDU);
    assert(rlc_pP->pdu_retrans_buffer[snP].nack_so_start < rlc_pP->pdu_retrans_buffer[snP].payload_size);
}

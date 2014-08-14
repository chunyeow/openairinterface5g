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
#define RLC_AM_TIMER_POLL_REORDERING_C
//-----------------------------------------------------------------------------
//#include "rtos_header.h"
#include "platform_types.h"
#include "platform_constants.h"
//-----------------------------------------------------------------------------
#include "rlc_am.h"
# include "LAYER2/MAC/extern.h"
#include "UTIL/LOG/log.h"
//-----------------------------------------------------------------------------
void rlc_am_check_timer_reordering(rlc_am_entity_t *rlc_pP,frame_t frameP)
//-----------------------------------------------------------------------------
{
    return ; // for debug


    if (rlc_pP->t_reordering.running) {
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
        //if (rlc_pP->t_reordering.frame_time_out == frameP) {
            // 5.1.3.2.4 Actions when t-Reordering expires
            // When t-Reordering expires, the receiving side of an AM RLC entity shall:
            //     - update VR(MS) to the SN of the first AMD PDU with SN >= VR(X) for which not all byte segments have been
            //       received;
            //     - if VR(H) > VR(MS):
            //         - start t-Reordering;
            //         - set VR(X) to VR(H).


            rlc_pP->t_reordering.running   = 0;
            rlc_pP->t_reordering.timed_out = 1;
            rlc_pP->stat_timer_reordering_timed_out += 1;

            rlc_am_pdu_info_t* pdu_info;
            mem_block_t*       cursor;
            cursor    =  rlc_pP->receiver_buffer.head;

            if (cursor) {
                do {
                    pdu_info =  &((rlc_am_rx_pdu_management_t*)(cursor->data))->pdu_info;

                    // NOT VERY SURE ABOUT THAT, THINK ABOUT IT
                    rlc_pP->vr_ms = (pdu_info->sn + 1) & RLC_AM_SN_MASK;

                    if (rlc_am_sn_gte_vr_x(rlc_pP, pdu_info->sn)) {
                        if (((rlc_am_rx_pdu_management_t*)(cursor->data))->all_segments_received == 0) {
                            rlc_pP->vr_ms = pdu_info->sn;
                            break;
                        }
                    }
                    cursor = cursor->next;
                } while (cursor != NULL);
                LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u][T-REORDERING] TIME-OUT UPDATED VR(MS) %04d\n",
                      frameP,
                      (rlc_pP->is_enb) ? "eNB" : "UE",
                      rlc_pP->enb_module_id,
                      rlc_pP->ue_module_id,
                      rlc_pP->rb_id,
                      rlc_pP->vr_ms);
            }

            if (rlc_am_sn_gt_vr_ms(rlc_pP, rlc_pP->vr_h)) {
                rlc_pP->vr_x = rlc_pP->vr_h;
                rlc_pP->t_reordering.frame_time_out = frameP + rlc_pP->t_reordering.time_out;
                LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u][T-REORDERING] TIME-OUT, RESTARTED T-REORDERING, UPDATED VR(X) to VR(R) %04d\n",
                      frameP,
                      (rlc_pP->is_enb) ? "eNB" : "UE",
                      rlc_pP->enb_module_id,
                      rlc_pP->ue_module_id,
                      rlc_pP->rb_id,
                      rlc_pP->vr_x);
            }

            rlc_pP->status_requested = 1;
        }
    }
}
//-----------------------------------------------------------------------------
void rlc_am_stop_and_reset_timer_reordering(rlc_am_entity_t *rlc_pP,frame_t frameP)
//-----------------------------------------------------------------------------
{
    LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u][T-REORDERING] STOPPED AND RESET\n",
          frameP,
          (rlc_pP->is_enb) ? "eNB" : "UE",
          rlc_pP->enb_module_id,
          rlc_pP->ue_module_id,
          rlc_pP->rb_id);
    rlc_pP->t_reordering.running         = 0;
    rlc_pP->t_reordering.frame_time_out  = 0;
    rlc_pP->t_reordering.frame_start     = 0;
    rlc_pP->t_reordering.timed_out       = 0;
}
//-----------------------------------------------------------------------------
void rlc_am_start_timer_reordering(rlc_am_entity_t *rlc_pP,frame_t frameP)
//-----------------------------------------------------------------------------
{
    rlc_pP->t_reordering.running         = 1;
    rlc_pP->t_reordering.frame_time_out  = frameP + rlc_pP->t_reordering.time_out;
    rlc_pP->t_reordering.frame_start     = frameP;
    rlc_pP->t_reordering.timed_out       = 0;
    LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u][T-REORDERING] STARTED (TIME-OUT = FRAME %5u)\n",
            frameP,
            (rlc_pP->is_enb) ? "eNB" : "UE",
            rlc_pP->enb_module_id,
            rlc_pP->ue_module_id,
            rlc_pP->rb_id,
            rlc_pP->t_reordering.frame_time_out);
}
//-----------------------------------------------------------------------------
void rlc_am_init_timer_reordering(rlc_am_entity_t *rlc_pP, uint32_t time_outP)
//-----------------------------------------------------------------------------
{
    rlc_pP->t_reordering.running         = 0;
    rlc_pP->t_reordering.frame_time_out  = 0;
    rlc_pP->t_reordering.frame_start     = 0;
    rlc_pP->t_reordering.time_out        = time_outP;
    rlc_pP->t_reordering.timed_out       = 0;
}

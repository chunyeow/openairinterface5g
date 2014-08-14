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
#define RLC_AM_TIMER_STATUS_PROHIBIT_C
//-----------------------------------------------------------------------------
//#include "rtos_header.h"
#include "platform_types.h"
#include "platform_constants.h"
//-----------------------------------------------------------------------------
#include "rlc_am.h"
#include "LAYER2/MAC/extern.h"
#include "UTIL/LOG/log.h"
//-----------------------------------------------------------------------------
void rlc_am_check_timer_status_prohibit(
        rlc_am_entity_t * const rlc_pP,
        const frame_t frameP)
//-----------------------------------------------------------------------------
{
    if (rlc_pP->t_status_prohibit.time_out > 0) {
        if (rlc_pP->t_status_prohibit.running) {
            if (
               // CASE 1:          start              time out
               //        +-----------+------------------+----------+
               //        |           |******************|          |
               //        +-----------+------------------+----------+
               //FRAME # 0                                     FRAME MAX
               ((rlc_pP->t_status_prohibit.frame_start < rlc_pP->t_status_prohibit.frame_time_out) &&
                   ((frameP >= rlc_pP->t_status_prohibit.frame_time_out) ||
                    (frameP < rlc_pP->t_status_prohibit.frame_start)))                                   ||
               // CASE 2:        time out            start
               //        +-----------+------------------+----------+
               //        |***********|                  |**********|
               //        +-----------+------------------+----------+
               //FRAME # 0                                     FRAME MAX VALUE
               ((rlc_pP->t_status_prohibit.frame_start > rlc_pP->t_status_prohibit.frame_time_out) &&
                  (frameP < rlc_pP->t_status_prohibit.frame_start) && (frameP >= rlc_pP->t_status_prohibit.frame_time_out))
               ) {

            //if ((rlc_pP->t_status_prohibit.frame_time_out <= frameP) && (rlc_pP->t_status_prohibit.frame_start)) {
                rlc_pP->t_status_prohibit.running   = 0;
                rlc_pP->t_status_prohibit.timed_out = 1;
                rlc_pP->stat_timer_status_prohibit_timed_out += 1;

                LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u][T-STATUS-PROHIBIT] TIME-OUT\n",
                      frameP,
                      (rlc_pP->is_enb) ? "eNB" : "UE",
                      rlc_pP->enb_module_id,
                      rlc_pP->ue_module_id,
                      rlc_pP->rb_id);
//#warning         TO DO rlc_am_check_timer_status_prohibit
                rlc_am_stop_and_reset_timer_status_prohibit(rlc_pP, frameP);
                //rlc_pP->t_status_prohibit.frame_time_out = frameP + rlc_pP->t_status_prohibit.time_out;
            }
        }
    }
}
//-----------------------------------------------------------------------------
void rlc_am_stop_and_reset_timer_status_prohibit(
        rlc_am_entity_t *const rlc_pP,
        const frame_t frameP)
//-----------------------------------------------------------------------------
{
	if (rlc_pP->t_status_prohibit.time_out > 0) {
        LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u][T-STATUS-PROHIBIT] STOPPED AND RESET\n",
              frameP,
              (rlc_pP->is_enb) ? "eNB" : "UE",
              rlc_pP->enb_module_id,
              rlc_pP->ue_module_id,
              rlc_pP->rb_id);
        rlc_pP->t_status_prohibit.running        = 0;
        rlc_pP->t_status_prohibit.frame_time_out = 0;
        rlc_pP->t_status_prohibit.frame_start    = 0;
        rlc_pP->t_status_prohibit.timed_out      = 0;
	}
}
//-----------------------------------------------------------------------------
void rlc_am_start_timer_status_prohibit(
        rlc_am_entity_t *const rlc_pP,
        const frame_t frameP)
//-----------------------------------------------------------------------------
{
	if (rlc_pP->t_status_prohibit.time_out > 0) {
        rlc_pP->t_status_prohibit.running        = 1;
        rlc_pP->t_status_prohibit.frame_time_out = rlc_pP->t_status_prohibit.time_out + frameP;
        rlc_pP->t_status_prohibit.frame_start    = frameP;
        rlc_pP->t_status_prohibit.timed_out = 0;
        LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u][T-STATUS-PROHIBIT] STARTED (TIME-OUT = FRAME %5u)\n",
              frameP,
              (rlc_pP->is_enb) ? "eNB" : "UE",
              rlc_pP->enb_module_id,
              rlc_pP->ue_module_id,
              rlc_pP->rb_id,
              rlc_pP->t_status_prohibit.frame_time_out);
        LOG_D(RLC, "TIME-OUT = FRAME %5u\n",  rlc_pP->t_status_prohibit.frame_time_out);
	}
}
//-----------------------------------------------------------------------------
void rlc_am_init_timer_status_prohibit(
        rlc_am_entity_t *const rlc_pP,
        const uint32_t time_outP)
//-----------------------------------------------------------------------------
{
    rlc_pP->t_status_prohibit.running        = 0;
    rlc_pP->t_status_prohibit.frame_time_out = 0;
    rlc_pP->t_status_prohibit.frame_start    = 0;
    rlc_pP->t_status_prohibit.time_out       = time_outP;
    rlc_pP->t_status_prohibit.timed_out      = 0;
}

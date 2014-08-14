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
#define RLC_AM_TIMER_POLL_RETRANSMIT_C
//-----------------------------------------------------------------------------
//#include "rtos_header.h"
#include "platform_types.h"
#include "platform_constants.h"
//-----------------------------------------------------------------------------
#include "rlc_am.h"
#include "LAYER2/MAC/extern.h"
#include "UTIL/LOG/log.h"
//-----------------------------------------------------------------------------
void rlc_am_check_timer_poll_retransmit(
        rlc_am_entity_t *const rlc_pP,
        const frame_t frameP)
//-----------------------------------------------------------------------------
{
  // 5.2.2.3 Expiry of t-PollRetransmit
  // Upon expiry of t-PollRetransmit, the transmitting side of an AM RLC entity shall:
  //     - if both the transmission buffer and the retransmission buffer are empty (excluding transmitted RLC data PDU
  //           awaiting for acknowledgements); or
  //     - if no new RLC data PDU can be transmitted (e.g. due to window stalling):
  //         - consider the AMD PDU with SN = VT(S) – 1 for retransmission; or
  //         - consider any AMD PDU which has not been positively acknowledged for retransmission;
  //     - include a poll in a RLC data PDU as described in section 5.2.2.1.

    if (rlc_pP->t_poll_retransmit.running) {
        if (
        // CASE 1:          start              time out
        //        +-----------+------------------+----------+
        //        |           |******************|          |
        //        +-----------+------------------+----------+
        //FRAME # 0                                     FRAME MAX
        ((rlc_pP->t_poll_retransmit.frame_start < rlc_pP->t_poll_retransmit.frame_time_out) &&
            ((frameP >= rlc_pP->t_poll_retransmit.frame_time_out) ||
             (frameP < rlc_pP->t_poll_retransmit.frame_start)))                                   ||
        // CASE 2:        time out            start
        //        +-----------+------------------+----------+
        //        |***********|                  |**********|
        //        +-----------+------------------+----------+
        //FRAME # 0                                     FRAME MAX VALUE
        ((rlc_pP->t_poll_retransmit.frame_start > rlc_pP->t_poll_retransmit.frame_time_out) &&
           (frameP < rlc_pP->t_poll_retransmit.frame_start) && (frameP >= rlc_pP->t_poll_retransmit.frame_time_out))
        ) {
        //if (rlc_pP->t_poll_retransmit.frame_time_out <= frameP) {
            rlc_pP->t_poll_retransmit.running   = 0;
            rlc_pP->t_poll_retransmit.timed_out = 1;
            rlc_pP->stat_timer_poll_retransmit_timed_out += 1;

            LOG_D(RLC, "[FRAME %05d][%s][RLC_AM][MOD %u/%u][RB %u][T_POLL_RETRANSMIT] TIME-OUT\n",
                  frameP,
                  (rlc_pP->is_enb) ? "eNB" : "UE",
                  rlc_pP->enb_module_id,
                  rlc_pP->ue_module_id,
                  rlc_pP->rb_id);

//#warning         TO DO rlc_am_check_timer_poll_retransmit
            rlc_pP->t_poll_retransmit.frame_time_out = frameP + rlc_pP->t_poll_retransmit.time_out;
        }
    }
}
//-----------------------------------------------------------------------------
int rlc_am_is_timer_poll_retransmit_timed_out(rlc_am_entity_t *const rlc_pP)
//-----------------------------------------------------------------------------
{
    return rlc_pP->t_poll_retransmit.timed_out;
}
//-----------------------------------------------------------------------------
void rlc_am_stop_and_reset_timer_poll_retransmit(
        rlc_am_entity_t *const rlc_pP,
        const frame_t frameP)
//-----------------------------------------------------------------------------
{
   LOG_D(RLC, "[FRAME %05d][%s][RLC_AM][MOD %u/%u][RB %u][T_POLL_RETRANSMIT] STOPPED AND RESET\n",
         frameP,
         (rlc_pP->is_enb) ? "eNB" : "UE",
         rlc_pP->enb_module_id,
         rlc_pP->ue_module_id,
         rlc_pP->rb_id);
    rlc_pP->t_poll_retransmit.running         = 0;
    rlc_pP->t_poll_retransmit.frame_time_out  = 0;
    rlc_pP->t_poll_retransmit.frame_start  = 0;
    rlc_pP->t_poll_retransmit.timed_out       = 0;
}
//-----------------------------------------------------------------------------
void rlc_am_start_timer_poll_retransmit(
        rlc_am_entity_t *const rlc_pP,
        const frame_t frameP)
//-----------------------------------------------------------------------------
{
    rlc_pP->t_poll_retransmit.running         = 1;
    rlc_pP->t_poll_retransmit.frame_time_out  = frameP + rlc_pP->t_poll_retransmit.time_out;
    rlc_pP->t_poll_retransmit.frame_start     = frameP;
    rlc_pP->t_poll_retransmit.timed_out       = 0;
    LOG_D(RLC, "[FRAME %05d][%s][RLC_AM][MOD %u/%u][RB %u][T_POLL_RETRANSMIT] STARTED (TIME-OUT = FRAME %05d)\n",
          frameP,
          (rlc_pP->is_enb) ? "eNB" : "UE",
          rlc_pP->enb_module_id,
          rlc_pP->ue_module_id,
          rlc_pP->rb_id,
          rlc_pP->t_poll_retransmit.frame_time_out);
}
//-----------------------------------------------------------------------------
void rlc_am_init_timer_poll_retransmit(
        rlc_am_entity_t *const rlc_pP,
        const uint32_t time_outP)
//-----------------------------------------------------------------------------
{
    rlc_pP->t_poll_retransmit.running         = 0;
    rlc_pP->t_poll_retransmit.frame_time_out  = 0;
    rlc_pP->t_poll_retransmit.frame_start     = 0;
    rlc_pP->t_poll_retransmit.time_out        = time_outP;
    rlc_pP->t_poll_retransmit.timed_out       = 0;
}

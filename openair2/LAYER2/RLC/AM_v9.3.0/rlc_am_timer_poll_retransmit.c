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
void rlc_am_check_timer_poll_retransmit(rlc_am_entity_t *rlcP,u32_t frame)
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

    if (rlcP->t_poll_retransmit.running) {
        if (
        // CASE 1:          start              time out
        //        +-----------+------------------+----------+
        //        |           |******************|          |
        //        +-----------+------------------+----------+
        //FRAME # 0                                     FRAME MAX
        ((rlcP->t_poll_retransmit.frame_start < rlcP->t_poll_retransmit.frame_time_out) &&
            ((frame >= rlcP->t_poll_retransmit.frame_time_out) ||
             (frame < rlcP->t_poll_retransmit.frame_start)))                                   ||
        // CASE 2:        time out            start
        //        +-----------+------------------+----------+
        //        |***********|                  |**********|
        //        +-----------+------------------+----------+
        //FRAME # 0                                     FRAME MAX VALUE
        ((rlcP->t_poll_retransmit.frame_start > rlcP->t_poll_retransmit.frame_time_out) &&
           (frame < rlcP->t_poll_retransmit.frame_start) && (frame >= rlcP->t_poll_retransmit.frame_time_out))
        ) {
        //if (rlcP->t_poll_retransmit.frame_time_out <= frame) {
            rlcP->t_poll_retransmit.running   = 0;
            rlcP->t_poll_retransmit.timed_out = 1;
            rlcP->stat_timer_poll_retransmit_timed_out += 1;

            LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][T_POLL_RETRANSMIT] TIME-OUT\n", frame,
                        rlcP->module_id, rlcP->rb_id);
            LOG_D(RLC, "[MSC_MSG][FRAME %05d][RLC_AM][MOD %02d][RB %02d][--- t-PollRetransmit Timed-out --->][RLC_AM][MOD %02d][RB %02d]\n",
                frame,
                rlcP->module_id,
                rlcP->rb_id,
                rlcP->module_id,
                rlcP->rb_id);

#warning         TO DO rlc_am_check_timer_poll_retransmit
            rlcP->t_poll_retransmit.frame_time_out = frame + rlcP->t_poll_retransmit.time_out;
        }
    }
}
//-----------------------------------------------------------------------------
int rlc_am_is_timer_poll_retransmit_timed_out(rlc_am_entity_t *rlcP)
//-----------------------------------------------------------------------------
{
    return rlcP->t_poll_retransmit.timed_out;
}
//-----------------------------------------------------------------------------
void rlc_am_stop_and_reset_timer_poll_retransmit(rlc_am_entity_t *rlcP,u32_t frame)
//-----------------------------------------------------------------------------
{
   LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][T_POLL_RETRANSMIT] STOPPED AND RESET\n", frame,
                        rlcP->module_id, rlcP->rb_id);
    rlcP->t_poll_retransmit.running         = 0;
    rlcP->t_poll_retransmit.frame_time_out  = 0;
    rlcP->t_poll_retransmit.frame_start  = 0;
    rlcP->t_poll_retransmit.timed_out       = 0;
}
//-----------------------------------------------------------------------------
void rlc_am_start_timer_poll_retransmit(rlc_am_entity_t *rlcP,u32_t frame)
//-----------------------------------------------------------------------------
{
    rlcP->t_poll_retransmit.running         = 1;
    rlcP->t_poll_retransmit.frame_time_out  = frame + rlcP->t_poll_retransmit.time_out;
    rlcP->t_poll_retransmit.frame_start     = frame;
    rlcP->t_poll_retransmit.timed_out       = 0;
    LOG_D(RLC, "[FRAME %05d][RLC_AM][MOD %02d][RB %02d][T_POLL_RETRANSMIT] STARTED (TIME-OUT = FRAME %05d)\n", frame, rlcP->module_id, rlcP->rb_id, rlcP->t_poll_retransmit.frame_time_out);
}
//-----------------------------------------------------------------------------
void rlc_am_init_timer_poll_retransmit(rlc_am_entity_t *rlcP, u32_t time_outP)
//-----------------------------------------------------------------------------
{
    rlcP->t_poll_retransmit.running         = 0;
    rlcP->t_poll_retransmit.frame_time_out  = 0;
    rlcP->t_poll_retransmit.frame_start     = 0;
    rlcP->t_poll_retransmit.time_out        = time_outP;
    rlcP->t_poll_retransmit.timed_out       = 0;
}

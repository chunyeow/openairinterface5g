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
/***************************************************************************
                          rlc_am_mac_status.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "rlc_am_entity.h"
#include "rlc_am_timers_proto_extern.h"
#include "rlc_am_discard_notif_proto_extern.h"
#include "rlc_am_reset_proto_extern.h"
#include "rlc_am_fsm_proto_extern.h"
#include "umts_timer_proto_extern.h"
//-----------------------------------------------------------------------------
void
rlc_am_status_report_from_mac (void *rlcP, uint16_t eventP)
{
//-----------------------------------------------------------------------------

  struct rlc_am_entity *rlc = (struct rlc_am_entity *) rlcP;

  //----------------------------------------
  // STATUS
  //----------------------------------------
  if ((eventP & RLC_AM_FIRST_STATUS_PDU_TYPE) == RLC_AM_FIRST_STATUS_PDU_TYPE) {
#ifdef DEBUG_RLC_AM_MAC_STATUS
    msg ("[RLC_AM %p][MAC_STATUS]  EVENT RLC_AM_FIRST_STATUS_PDU_TYPE\n", rlcP);
#endif
    return;
  }
  //----------------------------------------
  // DISCARD
  //----------------------------------------
  if ((eventP & RLC_AM_MRW_STATUS_PDU_TYPE)) {
#ifdef DEBUG_RLC_AM_MAC_STATUS
    msg ("[RLC_AM %p][MAC_STATUS]  EVENT RLC_AM_MRW_STATUS_PDU_TYPE\n", rlcP);
#endif
    // rearm the timer
    if (!(rlc->timer_mrw) && (rlc->discard_procedures.head)) {
      rlc->timer_mrw = umts_add_timer_list_up (&rlc->rlc_am_timer_list, rlc_am_discard_notify_mrw_ack_time_out, rlc,
                                               rlc->discard_procedures.head, (uint32_t) rlc->timer_mrw_init, *rlc->frame_tick_milliseconds);
    }
    return;
  }
  //----------------------------------------
  // RESET
  //----------------------------------------
  if ((eventP & RLC_AM_RESET_PDU_TYPE)) {
#ifdef DEBUG_RESET
    msg ("[RLC_AM %p][MAC_STATUS]  EVENT RLC_AM_RESET_PDU_TYPE SENT ARMING RESET TIMER %d frames frame %d\n", rlcP, (uint32_t) rlc->timer_rst_init, *rlc->frame_tick_milliseconds);
#endif

    rlc->timer_rst = umts_add_timer_list_up (&rlc->rlc_am_timer_list, rlc_am_reset_time_out, rlcP, NULL, (uint32_t) rlc->timer_rst_init, *rlc->frame_tick_milliseconds);

    return;
  }
  //----------------------------------------
  // RESET ACK
  //----------------------------------------
  if ((eventP & RLC_AM_RESET_ACK_PDU_TYPE)) {
#ifdef DEBUG_RLC_AM_MAC_STATUS
    msg ("[RLC_AM %p][MAC_STATUS]  EVENT RLC_AM_RESET_ACK_PDU_TYPE\n", rlcP);
#endif
    rlc_am_fsm_notify_event (rlc, RLC_AM_TRANSMIT_RESET_ACK_EVENT);
    return;
  }

}

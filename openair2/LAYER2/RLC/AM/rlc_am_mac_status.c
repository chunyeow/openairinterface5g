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
rlc_am_status_report_from_mac (void *rlcP, u16_t eventP)
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
                                               rlc->discard_procedures.head, (u32_t) rlc->timer_mrw_init, *rlc->frame_tick_milliseconds);
    }
    return;
  }
  //----------------------------------------
  // RESET
  //----------------------------------------
  if ((eventP & RLC_AM_RESET_PDU_TYPE)) {
#ifdef DEBUG_RESET
    msg ("[RLC_AM %p][MAC_STATUS]  EVENT RLC_AM_RESET_PDU_TYPE SENT ARMING RESET TIMER %d frames frame %d\n", rlcP, (u32_t) rlc->timer_rst_init, *rlc->frame_tick_milliseconds);
#endif

    rlc->timer_rst = umts_add_timer_list_up (&rlc->rlc_am_timer_list, rlc_am_reset_time_out, rlcP, NULL, (u32_t) rlc->timer_rst_init, *rlc->frame_tick_milliseconds);

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

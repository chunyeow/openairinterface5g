/***************************************************************************
                          rlc_am_timers.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "rlc_am_entity.h"
#include "rlc_am_constants.h"
#include "rlc_am_structs.h"
#include "rlc_am_util_proto_extern.h"
#include "umts_timer_proto_extern.h"
/*
void            rlc_am_timer_epc_notify_time_out (struct rlc_am_entity *rlcP, void *arg_not_usedP);
void            rlc_am_timer_epc_fsm (struct rlc_am_entity *rlcP, u8_t eventP);
//-----------------------------------------------------------------------------
void
rlc_am_timer_epc_notify_time_out (struct rlc_am_entity *rlcP, void *arg_not_usedP)
{
//-----------------------------------------------------------------------------
  rlc_am_timer_epc_fsm (rlcP, TIMER_EPC_TIMER_TIMED_OUT_EVENT);
}

//-----------------------------------------------------------------------------
void
rlc_am_timer_epc_fsm (struct rlc_am_entity *rlcP, u8_t eventP)
{
//-----------------------------------------------------------------------------
  // from 25.322 V4.3.0
  // The Estimated PDU Counter (EPC) is only applicable for RLC entities operating
  // in acknowledged mode. The EPC is a mechanism configured by upper layers used
  // for scheduling the retransmission of status reports in the Receiver. With this
  // mechanism, the Receiver will send a new status report in which it requests for
  // AMD PDUs not yet received. The time between two subsequent status report
  // retransmissions is not fixed, but it is controlled by both the timer Timer_EPC
  // and the state variable VR(EP), which adapt this time to the round trip delay
  // and the current bit rate, indicated in the TFI, in order to minimise the delay
  // of the status report retransmission.
  //
  // When a status report is triggered by some mechanisms and it is submitted to lower
  // layer (in UTRAN) or the successful or unsuccessful transmission of it is indicated
  // by lower layer (in UE) <<SEE CODE "Ref1">> to request for retransmitting one or more missing AMD PDUs,
  // the variable VR(EP) is set equal to the number of requested AMD PDUs. At least
  // one requested AMD PDU is needed to activate the EPC mechanism. The variable VR(EP)
  // is a counter, which is decremented every transmission time interval with the
  // estimated number of AMD PDUs that should have been received during that
  // transmission time interval on the corresponding logical channel.
  //
  // The timer Timer_EPC controls the maximum time that the variable VR(EP) needs to
  // wait before it will start counting down. This timer starts immediately after a
  // transmission of a retransmission request from the Receiver (when the first
  // STATUS PDU of the status report is submitted to lower layer (in UTRAN) or the
  // successful or unsuccessful transmission of it is indicated by lower layer(in UE)).
  // The initial value of the timer Timer_EPC is configured by upper layers. It typically
  // depends on the roundtrip delay, which consists of the propagation delay, processing
  // time in the transmitter and Receiver and the frame structure. This timer can also
  // be implemented as a counter, which counts the number of 10 ms radio frames that
  // could be expected to elapse before the first requested AMD PDU is received.
  //
  // If not all of these requested AMD PDUs have been received correctly when VR(EP)
  // is equal to zero, a new status report will be transmitted and the EPC mechanism
  // will be reset accordingly. The timer Timer_EPC will be started once more when the
  // first STATUS PDU of the status report is submitted to lower layer (in UTRAN) or the
  // successful or unsuccessful transmission of it is indicated by lower layer (in UE).
  // If all of the requested AMD PDUs have been received correctly, the EPC mechanism ends.

  switch (rlcP->epc_state) {

        //--------------------------------------------
      case TIMER_EPC_STATE_IDLE:
        //--------------------------------------------
        rlcP->epc_counting_down = 0;
        switch (eventP) {
              // Ref1
            case TIMER_EPC_PDU_STATUS_SUBMITTED_LOWER_LAYER_EVENT:     // for RG
            case TIMER_EPC_PDU_STATUS_TRANSMITED_EVENT:        // for UE
              rlcP->vr_ep = rlcP->epc_nb_missing_pdus;
              rlcP->epc_old_vr_h = rlcP->vr_h;
              rlcP->epc_old_vr_r = rlcP->vr_r;

              rlcP->epc_state = TIMER_EPC_STATE_TIMER_ARMED;
              rlcP->timer_epc = umts_add_timer_list_up (&rlcP->rlc_am_timer_list, rlc_am_timer_epc_notify_time_out, rlcP, rlcP->discard_procedures.head, NULL, *rlcP->frame_tick_milliseconds);
#ifdef DEBUG_RLC_AM_TIMER_EPC_FSM
              msg ("[RLC_AM %p][TIMER_EPC_FSM]  TIMER_EPC_STATE_IDLE -> TIMER_EPC_STATE_TIMER_ARMED\n", rlcP);
#endif

              return;
              break;

            default:
#ifdef DEBUG_RLC_AM_TIMER_EPC_FSM
              msg ("[RLC_AM %p][TIMER_EPC_FSM]  TIMER_EPC_STATE_IDLE EVENT %02X hex NOT EXPECTED\n", rlcP, eventP);
#endif
              return;
        }
        break;


        //--------------------------------------------
      case TIMER_EPC_STATE_TIMER_ARMED:
        //--------------------------------------------
        switch (eventP) {
              // Ref1
            case TIMER_EPC_PDU_STATUS_TRANSMITED_EVENT:
              return;
              break;

            case TIMER_EPC_TIMER_TIMED_OUT_EVENT:
              rlcP->epc_counting_down = 1;
              rlcP->timer_epc = NULL;
              rlcP->epc_state = TIMER_EPC_STATE_TIMED_OUT;

#ifdef DEBUG_RLC_AM_TIMER_EPC_FSM
              msg ("[RLC_AM %p][TIMER_EPC_FSM]  TIMER_EPC_STATE_TIMER_ARMED -> TIMER_EPC_STATE_TIMED_OUT\n", rlcP);
#endif
              return;
              break;

            default:
#ifdef DEBUG_RLC_AM_TIMER_EPC_FSM
              msg ("[RLC_AM %p][TIMER_EPC_FSM]  TIMER_EPC_STATE_TIMER_ARMED EVENT %02X hex NOT EXPECTED\n", rlcP, eventP);
#endif
              return;
        }
        break;

        //--------------------------------------------
      case TIMER_EPC_STATE_TIMED_OUT:
        //--------------------------------------------
        switch (eventP) {

            case TIMER_EPC_VR_EP_EQUAL_ZERO_EVENT:
              rlcP->epc_counting_down = 0;
              rlcP->epc_state = TIMER_EPC_STATE_IDLE;

              // return 1 if sn1 > sn2
              // return 0 if sn1 = sn2
              // return -1 if sn1 < sn2
              if (rlc_am_comp_sn (rlcP, rlcP->epc_old_vr_r, rlcP->epc_old_vr_h, rlcP->vr_r) > 0) {
                // not all missing pdus have been received
                // so generate a new status report
                rlcP->send_status_pdu_requested = 1;
#ifdef DEBUG_RLC_AM_TIMER_EPC_FSM
                msg ("[RLC_AM %p][TIMER_EPC_FSM]  TIMER_EPC_STATE_TIMED_OUT -> TIMER_EPC_STATE_IDLE GENERATION OF STATUS REQUESTED\n", rlcP);
#endif
              }
#ifdef DEBUG_RLC_AM_TIMER_EPC_FSM
              else {
                msg ("[RLC_AM %p][TIMER_EPC_FSM]  TIMER_EPC_STATE_TIMED_OUT -> TIMER_EPC_STATE_IDLE\n", rlcP);
              }
#endif
              return;
              break;

            default:
#ifdef DEBUG_RLC_AM_TIMER_EPC_FSM
              msg ("[RLC_AM %p][TIMER_EPC_FSM]  TIMER_EPC_STATE_TIMED_OUT EVENT %02X hex NOT EXPECTED\n", rlcP, eventP);
#endif
              return;
        }
        break;

      default:;
  }
}
*/

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
#define RLC_UM_MODULE
#define RLC_UM_FSM_C
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "rlc_um.h"
#include "LAYER2/MAC/extern.h"
#include "UTIL/LOG/log.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
int
rlc_um_fsm_notify_event (struct rlc_um_entity *rlcP, u8_t eventP)
{
//-----------------------------------------------------------------------------

  switch (rlcP->protocol_state) {
        //-------------------------------
        // RLC_NULL_STATE
        //-------------------------------
      case RLC_NULL_STATE:
        switch (eventP) {
            case RLC_UM_RECEIVE_CRLC_CONFIG_REQ_ENTER_DATA_TRANSFER_READY_STATE_EVENT:
              LOG_D(RLC, "[RLC_UM][MOD %d][RB %d] FSM RLC_NULL_STATE -> RLC_DATA_TRANSFER_READY_STATE\n", rlcP->module_id, rlcP->rb_id);
              rlcP->protocol_state = RLC_DATA_TRANSFER_READY_STATE;
              return 1;
              break;

            default:
              LOG_E(RLC, "[RLC_UM][MOD %d][RB %d] FSM WARNING PROTOCOL ERROR - EVENT %02X hex NOT EXPECTED FROM NULL_STATE\n", rlcP->module_id, rlcP->rb_id, eventP);
	      mac_xface->macphy_exit("");
              return 0;
        }
        break;
        //-------------------------------
        // RLC_DATA_TRANSFER_READY_STATE
        //-------------------------------
      case RLC_DATA_TRANSFER_READY_STATE:
        switch (eventP) {
            case RLC_UM_RECEIVE_CRLC_CONFIG_REQ_ENTER_NULL_STATE_EVENT:
              LOG_D(RLC, "[RLC_UM][MOD %d][RB %d] FSM RLC_DATA_TRANSFER_READY_STATE -> RLC_NULL_STATE\n", rlcP->module_id, rlcP->rb_id);
              rlcP->protocol_state = RLC_NULL_STATE;
              return 1;
              break;

            case RLC_UM_RECEIVE_CRLC_SUSPEND_REQ_EVENT:
            case RLC_UM_TRANSMIT_CRLC_SUSPEND_CNF_EVENT:
              LOG_N(RLC, "[RLC_UM][MOD %d][RB %d] FSM RLC_DATA_TRANSFER_READY_STATE -> RLC_LOCAL_SUSPEND_STATE\n", rlcP->module_id, rlcP->rb_id);
              rlcP->protocol_state = RLC_LOCAL_SUSPEND_STATE;
              return 1;
              break;

            default:
              LOG_E(RLC, "[RLC_UM][MOD %d][RB %d] FSM WARNING PROTOCOL ERROR - EVENT %02X hex NOT EXPECTED FROM DATA_TRANSFER_READY_STATE\n", rlcP->module_id, rlcP->rb_id, eventP);
              return 0;
        }
        break;
        //-------------------------------
        // RLC_LOCAL_SUSPEND_STATE
        //-------------------------------
      case RLC_LOCAL_SUSPEND_STATE:
        switch (eventP) {
            case RLC_UM_RECEIVE_CRLC_CONFIG_REQ_ENTER_NULL_STATE_EVENT:
              LOG_N(RLC, "[RLC_UM][MOD %d][RB %d] FSM RLC_LOCAL_SUSPEND_STATE -> RLC_NULL_STATE\n", rlcP->module_id, rlcP->rb_id);
              rlcP->protocol_state = RLC_NULL_STATE;
              return 1;
              break;

            case RLC_UM_RECEIVE_CRLC_RESUME_REQ_EVENT:
              LOG_N(RLC, "[RLC_UM][MOD %d][RB %d] FSM RLC_LOCAL_SUSPEND_STATE -> RLC_DATA_TRANSFER_READY_STATE\n", rlcP->module_id, rlcP->rb_id);
              rlcP->protocol_state = RLC_DATA_TRANSFER_READY_STATE;
              return 1;
              break;

            default:
              LOG_E(RLC, "[RLC_UM][MOD %d][RB %d] FSM  WARNING PROTOCOL ERROR - EVENT %02X hex NOT EXPECTED FROM RLC_LOCAL_SUSPEND_STATE\n", rlcP->module_id, rlcP->rb_id, eventP);
              return 0;
        }
        break;

      default:
        LOG_E(RLC, "[RLC_UM][MOD %d][RB %d] FSM ERROR UNKNOWN STATE %d\n", rlcP->module_id, rlcP->rb_id, rlcP->protocol_state);
        return 0;
  }
}

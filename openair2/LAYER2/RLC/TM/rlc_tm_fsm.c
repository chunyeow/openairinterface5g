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
/***************************************************************************
                          rlc_tm_fsm.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr
 ***************************************************************************/
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "rlc_tm_entity.h"
#include "rlc_tm_constants.h"
#include "rlc_def.h"
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int
rlc_tm_fsm_notify_event (struct rlc_tm_entity *rlcP, u8_t eventP)
{
//-----------------------------------------------------------------------------

  switch (rlcP->protocol_state) {
        //-------------------------------
        // RLC_NULL_STATE
        //-------------------------------
      case RLC_NULL_STATE:
        switch (eventP) {
            case RLC_TM_RECEIVE_CRLC_CONFIG_REQ_ENTER_DATA_TRANSFER_READY_STATE_EVENT:
              #ifdef DEBUG_RLC_TM_FSM
              msg ("[RLC_TM %p][FSM] RLC_NULL_STATE -> RLC_DATA_TRANSFER_READY_STATE\n", rlcP);
              #endif
              rlcP->protocol_state = RLC_DATA_TRANSFER_READY_STATE;
              return 1;
              break;

            default:
              msg ("[RLC_TM %p][FSM] WARNING PROTOCOL ERROR - EVENT %02X hex NOT EXPECTED FROM NULL_STATE\n", rlcP, eventP);
              return 0;
        }
        break;
        //-------------------------------
        // RLC_DATA_TRANSFER_READY_STATE
        //-------------------------------
      case RLC_DATA_TRANSFER_READY_STATE:
        switch (eventP) {
            case RLC_TM_RECEIVE_CRLC_CONFIG_REQ_ENTER_NULL_STATE_EVENT:
              #ifdef DEBUG_RLC_TM_FSM
              msg ("[RLC_TM %p][FSM] RLC_DATA_TRANSFER_READY_STATE -> RLC_NULL_STATE\n", rlcP);
              #endif
              rlcP->protocol_state = RLC_NULL_STATE;
              return 1;
              break;
        }
        break;

      default:
        msg ("[RLC_TM %p][FSM] ERROR UNKNOWN STATE %d\n", rlcP, rlcP->protocol_state);
        return 0;
  }
  return 0;
}

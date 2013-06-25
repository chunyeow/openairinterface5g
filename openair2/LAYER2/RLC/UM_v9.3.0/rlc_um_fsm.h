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
/*! \file rlc_um_fsm.h
* \brief This file defines the prototypes of the functions dealing with the finite state machine of the RLC UM protocol instance.
* \author GAUTHIER Lionel
* \date 2010-2011
* \version
* \note
* \bug
* \warning
*/
/** @defgroup _rlc_um_fsm_impl_ RLC UM FSM Implementation
* @ingroup _rlc_um_impl_
* @{
*/
#    ifndef __RLC_UM_FSM_PROTO_EXTERN_H__
#        define __RLC_UM_FSM_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        ifdef RLC_UM_FSM_C
#            define private_rlc_um_fsm(x)    x
#            define protected_rlc_um_fsm(x)  x
#            define public_rlc_um_fsm(x)     x
#        else
#            ifdef RLC_UM_MODULE
#                define private_rlc_um_fsm(x)
#                define protected_rlc_um_fsm(x)  extern x
#                define public_rlc_um_fsm(x)     extern x
#            else
#                define private_rlc_um_fsm(x)
#                define protected_rlc_um_fsm(x)
#                define public_rlc_um_fsm(x)     extern x
#            endif
#        endif
#        include "platform_types.h"
#        include "rlc_um_entity.h"
//-----------------------------------------------------------------------------
/*! \fn int rlc_um_fsm_notify_event (struct rlc_um_entity *rlcP, u8_t eventP)
* \brief    Send an event to the RLC UM finite state machine.
* \param[in]  rlcP        RLC UM protocol instance pointer.
* \param[in]  eventP      Event (#RLC_UM_RECEIVE_CRLC_CONFIG_REQ_ENTER_NULL_STATE_EVENT,
*                         #RLC_UM_RECEIVE_CRLC_CONFIG_REQ_ENTER_DATA_TRANSFER_READY_STATE_EVENT,
*                         #RLC_UM_RECEIVE_CRLC_SUSPEND_REQ_EVENT,
*                         #RLC_UM_TRANSMIT_CRLC_SUSPEND_CNF_EVENT,
*                         #RLC_UM_RECEIVE_CRLC_RESUME_REQ_EVENT).
* \return     1 if no error was encountered, 0 if the event was not processed.
* \Note       This FSM is not LTE 9.3.0 compliant, it has to be modified or removed.
*/
protected_rlc_um_fsm(int      rlc_um_fsm_notify_event (struct rlc_um_entity *rlcP, u8_t eventP));
/** @} */
#    endif

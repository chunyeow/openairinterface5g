/***************************************************************************
                         lteRALue_mih_execute.h  -  description
 ***************************************************************************
  Eurecom OpenAirInterface 3
  Copyright(c) 1999 - 2013 Eurecom

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
  Address      : Eurecom, 450 route des Chappes, 06410 Biot Sophia Antipolis, France
*******************************************************************************/
/*! \file lteRALue_mih_execute.h
 * \brief This file defines the prototypes of the functions for lteRALue_mih_execute.c
 * \author WETTERWALD Michelle, GAUTHIER Lionel, MAUREL Frederic
 * \date 2013
 * \company EURECOM
 * \email: michelle.wetterwald@eurecom.fr, lionel.gauthier@eurecom.fr, frederic.maurel@eurecom.fr
 */
/*******************************************************************************/
#ifndef __MRALLTE_MIH_EXECUTE_H__
#    define __MRALLTE_MIH_EXECUTE_H__
//-----------------------------------------------------------------------------
#        ifdef MRALLTE_MIH_EXECUTE_C
#            define private_mRALlte_mih_execute(x)    x
#            define protected_mRALlte_mih_execute(x)  x
#            define public_mRALlte_mih_execute(x)     x
#        else
#            ifdef MRAL_MODULE
#                define private_mRALlte_mih_execute(x)
#                define protected_mRALlte_mih_execute(x)  extern x
#                define public_mRALlte_mih_execute(x)     extern x
#            else
#                define private_mRALlte_mih_execute(x)
#                define protected_mRALlte_mih_execute(x)
#                define public_mRALlte_mih_execute(x)     extern x
#            endif
#        endif
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
//-----------------------------------------------------------------------------
#include "MIH_C.h"
#include "lteRALue_constants.h"
#include "lteRALue_mih_msg.h"
//-----------------------------------------------------------------------------
protected_mRALlte_mih_execute(MIH_C_LINK_ACTION_T g_link_action;)
//-----------------------------------------------------------------------------
protected_mRALlte_mih_execute(void mRALlte_action_request(MIH_C_Message_Link_Action_request_t* messageP);)
//-----------------------------------------------------------------------------
protected_mRALlte_mih_execute(void mRALlte_get_parameters_request(MIH_C_Message_Link_Get_Parameters_request_t* messageP);)
//-----------------------------------------------------------------------------
protected_mRALlte_mih_execute(void mRALlte_subscribe_request  (MIH_C_Message_Link_Event_Subscribe_request_t*   messageP);)
protected_mRALlte_mih_execute(void mRALlte_unsubscribe_request(MIH_C_Message_Link_Event_Unsubscribe_request_t* messageP);)
//-----------------------------------------------------------------------------
protected_mRALlte_mih_execute(LIST(MIH_C_LINK_CFG_PARAM, g_link_cfg_param_thresholds);)
//-----------------------------------------------------------------------------
protected_mRALlte_mih_execute(void mRALlte_configure_thresholds_request(MIH_C_Message_Link_Configure_Thresholds_request_t* messageP);)
public_mRALlte_mih_execute(void mRALlte_check_thresholds_signal_strength(MIH_C_THRESHOLD_VAL_T new_valP, MIH_C_THRESHOLD_VAL_T old_valP);)
#endif

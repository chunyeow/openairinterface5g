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
#            define private_mRAL_mih_execute(x)    x
#            define protected_mRAL_mih_execute(x)  x
#            define public_mRAL_mih_execute(x)     x
#        else
#            ifdef LTE_RAL_UE
#                define private_mRAL_mih_execute(x)
#                define protected_mRAL_mih_execute(x)  extern x
#                define public_mRAL_mih_execute(x)     extern x
#            else
#                define private_mRAL_mih_execute(x)
#                define protected_mRAL_mih_execute(x)
#                define public_mRAL_mih_execute(x)     extern x
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
#include "lteRALue.h"

//-----------------------------------------------------------------------------
protected_mRAL_mih_execute(MIH_C_LINK_ACTION_T g_link_action;)
//-----------------------------------------------------------------------------
protected_mRAL_mih_execute(void mRAL_action_request(MIH_C_Message_Link_Action_request_t* messageP);)
//-----------------------------------------------------------------------------
protected_mRAL_mih_execute(void mRAL_get_parameters_request(MIH_C_Message_Link_Get_Parameters_request_t* messageP);)
//-----------------------------------------------------------------------------
protected_mRAL_mih_execute(void mRAL_subscribe_request  (MIH_C_Message_Link_Event_Subscribe_request_t*   messageP);)
protected_mRAL_mih_execute(void mRAL_unsubscribe_request(MIH_C_Message_Link_Event_Unsubscribe_request_t* messageP);)
//-----------------------------------------------------------------------------
protected_mRAL_mih_execute(LIST(MIH_C_LINK_CFG_PARAM, g_link_cfg_param_thresholds);)
//-----------------------------------------------------------------------------
protected_mRAL_mih_execute(void mRAL_configure_thresholds_request(MIH_C_Message_Link_Configure_Thresholds_request_t* messageP);)
public_mRAL_mih_execute(void mRAL_check_thresholds_signal_strength(MIH_C_THRESHOLD_VAL_T new_valP, MIH_C_THRESHOLD_VAL_T old_valP);)
#endif

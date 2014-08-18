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
/*! \file lteRALenb_action.h
 * \brief
 * \author GAUTHIER Lionel, MAUREL Frederic, WETTERWALD Michelle
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */

#ifndef __LTE_RAL_ENB_ACTION_H__
#define __LTE_RAL_ENB_ACTION_H__
//-----------------------------------------------------------------------------
#        ifdef LTE_RAL_ENB_ACTION_C
#            define private_lteralenb_action(x)    x
#            define protected_lteralenb_action(x)  x
#            define public_lteralenb_action(x)     x
#        else
#            ifdef LTE_RAL_ENB
#                define private_lteralenb_action(x)
#                define protected_lteralenb_action(x)  extern x
#                define public_lteralenb_action(x)     extern x
#            else
#                define private_lteralenb_action(x)
#                define protected_lteralenb_action(x)
#                define public_lteralenb_action(x)     extern x
#            endif
#        endif
//-----------------------------------------------------------------------------
#include "lteRALenb.h"

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/
#ifdef MIH_C_MEDIEVAL_EXTENSIONS

/*
 * ---------------------------------------------------------------------------
 * Flow identifier management:
 * Radio Bearer data flows are identified by a source address, a destination
 * address and a port number for a particular IP transport protocol (UDP,
 * TCP). A private data structure is used to map upper-layer flow identifiers
 * to lower-layer RB channel identifiers. It is handled by private functions.
 * ---------------------------------------------------------------------------
 */
/* Structure of the destination data flow */
typedef struct Data_flow {
    unsigned char addr[16]; // IP address
    unsigned int l2id[2]; // L2 identifier
    unsigned int  port;  // IP port identifier
    int proto;   // IP protocol
    int cnxid;   // Data flow identifier
}data_flow_t;

#define ACTION_MAX_FLOW  ((RAL_MAX_MT)*(RAL_MAX_RB))

typedef struct eRAL_action_DataFlowList {
    int          n_flows;
    data_flow_t  flow [ACTION_MAX_FLOW];
    int          flow_id [ACTION_MAX_FLOW];  //added TEMP MW 23/05/13
} eRAL_action_DataFlowList_t;

private_lteralenb_action(eRAL_action_DataFlowList_t g_flows = {};)

#endif // MIH_C_MEDIEVAL_EXTENSIONS
/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

protected_lteralenb_action(void eRAL_action_request        (ral_enb_instance_t instanceP, MIH_C_Message_Link_Action_request_t* msgP);)
protected_lteralenb_action(int  eRAL_action_save_flow_id   (ral_enb_instance_t instanceP, MIH_C_FLOW_ID_T* flowId, int cnxid);)
private_lteralenb_action(  int  eRAL_action_set_channel_id (ral_enb_instance_t instanceP, MIH_C_FLOW_ID_T* flowId, int cnxid);)
private_lteralenb_action(  int  eRAL_action_get_channel_id (ral_enb_instance_t instanceP, MIH_C_FLOW_ID_T* flowId, int* cnxid);)
private_lteralenb_action(  int  eRAL_action_del_channel_id (ral_enb_instance_t instanceP, int fix);)
private_lteralenb_action(  int  eRAL_action_is_in_progress (ral_enb_instance_t instanceP, MIH_C_STATUS_T* status, MIH_C_LINK_AC_RESULT_T* ac_status, MIH_C_LINK_AC_TYPE_T action);)
#ifdef MIH_C_MEDIEVAL_EXTENSIONS
private_lteralenb_action(  MIH_C_LINK_AC_RESULT_T eRAL_action_link_flow_attr(ral_enb_instance_t instanceP);)
private_lteralenb_action(  MIH_C_LINK_AC_RESULT_T eRAL_action_link_activate_resources(ral_enb_instance_t instanceP);)
private_lteralenb_action(  MIH_C_LINK_AC_RESULT_T eRAL_action_link_deactivate_resources(ral_enb_instance_t instanceP);)
#endif // MIH_C_MEDIEVAL_EXTENSIONS

#endif

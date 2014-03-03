/***************************************************************************
                         ltmRALue_subscribe.c  -  description
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
/*! \file ltmRALue_subscribe.c
 * \brief
 * \author WETTERWALD Michelle, GAUTHIER Lionel, MAUREL Frederic
 * \date 2013
 * \company EURECOM
 * \email: michelle.wetterwald@eurecom.fr, lionel.gauthier@eurecom.fr, frederic.maurel@eurecom.fr
 */
/*******************************************************************************/
#define LTE_RAL_UE
#define LTERALUE_SUBSCRIBE_C
//-----------------------------------------------------------------------------
#include "lteRALue.h"
#include "LAYER2/MAC/extern.h"
/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:  mRAL_subscribe_request()                                        **
 **                                                                        **
 ** Description: Subscribes the MIH-F to receive specified link event      **
 **   indications and sends Link Event Subscribe confirmation              **
 **   to the MIH-F.                                                        **
 **                                                                        **
 ** Inputs:  msg_pP:  Pointer to the received MIH message                    **
 **     Others: None                                                       **
 **                                                                        **
 ** Outputs:  None                                                         **
 **   Return: None                                                         **
 **     Others: ralpriv                                                    **
 **                                                                        **
 ***************************************************************************/
void mRAL_subscribe_request(ral_ue_instance_t                             instanceP,
                            MIH_C_Message_Link_Event_Subscribe_request_t *msg_pP)
{
    module_id_t    mod_id = instanceP - NB_eNB_INST;
    MIH_C_STATUS_T status = MIH_C_STATUS_REJECTED;
    /* Check whether the action request is supported */
    if (g_ue_ral_obj[mod_id].mih_supported_link_command_list & MIH_C_BIT_LINK_EVENT_SUBSCRIBE)
    {
        MIH_C_LINK_EVENT_LIST_T mih_subscribed_req_event_list;

        g_ue_ral_obj[mod_id].mih_subscribe_req_event_list |= (msg_pP->primitive.RequestedLinkEventList & g_ue_ral_obj[mod_id].mih_supported_link_event_list);

        mih_subscribed_req_event_list = g_ue_ral_obj[mod_id].mih_subscribe_req_event_list & msg_pP->primitive.RequestedLinkEventList;

        status = MIH_C_STATUS_SUCCESS;

        mRAL_send_event_subscribe_confirm(instanceP, &msg_pP->header.transaction_id,
                &status,
                &mih_subscribed_req_event_list);
    }
    else
    {
        mRAL_send_event_subscribe_confirm(instanceP, &msg_pP->header.transaction_id,
                &status,
                NULL);
    }
}

/****************************************************************************
 **                                                                        **
 ** Name:  mRAL_unsubscribe_request()                                      **
 **                                                                        **
 ** Description: Unsubscribes the MIH-F to receive specified link event    **
 **   indications and sends Link Event Unsubscribe confirmation            **
 **   to the MIH-F.                                                        **
 **                                                                        **
 ** Inputs:  msg_pP:  Pointer to the received MIH message                    **
 **     Others: None                                                       **
 **                                                                        **
 ** Outputs:  None                                                         **
 **   Return: None                                                         **
 **     Others: ralpriv                                                    **
 **                                                                        **
 ***************************************************************************/
void mRAL_unsubscribe_request(ral_ue_instance_t                               instanceP,
                              MIH_C_Message_Link_Event_Unsubscribe_request_t *msg_pP)
{
    MIH_C_STATUS_T status = MIH_C_STATUS_REJECTED;
    module_id_t    mod_id = instanceP - NB_eNB_INST;

    /* Check whether the action request is supported */
    if (g_ue_ral_obj[mod_id].mih_supported_link_command_list & MIH_C_BIT_LINK_EVENT_UNSUBSCRIBE)
    {
        MIH_C_LINK_EVENT_LIST_T mih_unsubscribed_req_event_list;
        MIH_C_LINK_EVENT_LIST_T saved_req_event_list;

        saved_req_event_list = g_ue_ral_obj[mod_id].mih_subscribe_req_event_list;

        g_ue_ral_obj[mod_id].mih_subscribe_req_event_list &= ~(msg_pP->primitive.RequestedLinkEventList & g_ue_ral_obj[mod_id].mih_supported_link_event_list);
        mih_unsubscribed_req_event_list = g_ue_ral_obj[mod_id].mih_subscribe_req_event_list ^ saved_req_event_list;

        status = MIH_C_STATUS_SUCCESS;

        mRAL_send_event_unsubscribe_confirm(instanceP, &msg_pP->header.transaction_id,
                &status,
                &mih_unsubscribed_req_event_list);
    }
    else
    {
        mRAL_send_event_unsubscribe_confirm(instanceP, &msg_pP->header.transaction_id,
                &status,
                NULL);
    }
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/



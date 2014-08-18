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

#include "eRALlte_subscribe.h"

#include "eRALlte_mih_msg.h"
#include "eRALlte_variables.h"

/****************************************************************************/
/*******************  G L O C A L    D E F I N I T I O N S  *****************/
/****************************************************************************/

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:	 eRALlte_subscribe_request()                               **
 **                                                                        **
 ** Description: Subscribes the MIH-F to receive specified link event      **
 **		 indications and sends Link Event Subscribe confirmation   **
 **		 to the MIH-F.                                             **
 **                                                                        **
 ** Inputs:	 msgP:		Pointer to the received MIH message        **
 ** 	 	 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	None                                       **
 ** 	 	 Others:	ralpriv                                    **
 **                                                                        **
 ***************************************************************************/
void eRALlte_subscribe_request(MIH_C_Message_Link_Event_Subscribe_request_t* msgP)
{
    MIH_C_STATUS_T status = MIH_C_STATUS_REJECTED;
    /* Check whether the action request is supported */
    if (ralpriv->mih_supported_link_command_list & MIH_C_BIT_LINK_EVENT_SUBSCRIBE)
    {
	MIH_C_LINK_EVENT_LIST_T mih_subscribed_req_event_list;

	ralpriv->mih_subscribe_req_event_list |= (msgP->primitive.RequestedLinkEventList & ralpriv->mih_supported_link_event_list);

	mih_subscribed_req_event_list = ralpriv->mih_subscribe_req_event_list & msgP->primitive.RequestedLinkEventList;

	status = MIH_C_STATUS_SUCCESS;

	eRALlte_send_event_subscribe_confirm(&msgP->header.transaction_id,
					     &status,
					     &mih_subscribed_req_event_list);
    }
    else
    {
	eRALlte_send_event_subscribe_confirm(&msgP->header.transaction_id,
					     &status,
					     NULL);	
    }
}

/****************************************************************************
 **                                                                        **
 ** Name:	 eRALlte_unsubscribe_request()                             **
 **                                                                        **
 ** Description: Unsubscribes the MIH-F to receive specified link event    **
 **		 indications and sends Link Event Unsubscribe confirmation **
 **		 to the MIH-F.                                             **
 **                                                                        **
 ** Inputs:	 msgP:		Pointer to the received MIH message        **
 ** 	 	 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	None                                       **
 ** 	 	 Others:	ralpriv                                    **
 **                                                                        **
 ***************************************************************************/
void eRALlte_unsubscribe_request(MIH_C_Message_Link_Event_Unsubscribe_request_t* msgP)
{
    MIH_C_STATUS_T status = MIH_C_STATUS_REJECTED;
    /* Check whether the action request is supported */
    if (ralpriv->mih_supported_link_command_list & MIH_C_BIT_LINK_EVENT_UNSUBSCRIBE)
    {
	MIH_C_LINK_EVENT_LIST_T mih_unsubscribed_req_event_list;
	MIH_C_LINK_EVENT_LIST_T saved_req_event_list;

	saved_req_event_list = ralpriv->mih_subscribe_req_event_list;

	ralpriv->mih_subscribe_req_event_list &= ~(msgP->primitive.RequestedLinkEventList & ralpriv->mih_supported_link_event_list);
	mih_unsubscribed_req_event_list = ralpriv->mih_subscribe_req_event_list ^ saved_req_event_list;

	status = MIH_C_STATUS_SUCCESS;

	eRALlte_send_event_unsubscribe_confirm(&msgP->header.transaction_id,
					       &status,
					       &mih_unsubscribed_req_event_list);
    }
    else
    {
	eRALlte_send_event_unsubscribe_confirm(&msgP->header.transaction_id,
					       &status,
					       NULL);
    }
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/


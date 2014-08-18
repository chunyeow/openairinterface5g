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
#define MRAL_MODULE
#define MRALLTE_SUBSCRIBE_C
#include <assert.h>
#include "mRALlte_subscribe.h"
#include "mRALlte_variables.h"

//-----------------------------------------------------------------------------
void mRALlte_subscribe_request(MIH_C_Message_Link_Event_Subscribe_request_t* messageP) {
//-----------------------------------------------------------------------------
    MIH_C_STATUS_T                      status;
    MIH_C_LINK_EVENT_LIST_T             mih_subscribed_req_event_list;

    ralpriv->mih_subscribe_req_event_list |= (messageP->primitive.RequestedLinkEventList & ralpriv->mih_supported_link_event_list);

    mih_subscribed_req_event_list = ralpriv->mih_subscribe_req_event_list & messageP->primitive.RequestedLinkEventList;

    status = MIH_C_STATUS_SUCCESS;

    mRALte_send_event_subscribe_confirm(&messageP->header.transaction_id,
                                        &status,
                                        &mih_subscribed_req_event_list);

}
//-----------------------------------------------------------------------------
void mRALlte_unsubscribe_request(MIH_C_Message_Link_Event_Unsubscribe_request_t* messageP) {
//-----------------------------------------------------------------------------
    MIH_C_STATUS_T                      status;
    MIH_C_LINK_EVENT_LIST_T             mih_unsubscribed_req_event_list;
    MIH_C_LINK_EVENT_LIST_T             saved_req_event_list;

    saved_req_event_list           = ralpriv->mih_subscribe_req_event_list;

    ralpriv->mih_subscribe_req_event_list &= ((messageP->primitive.RequestedLinkEventList & ralpriv->mih_supported_link_event_list) ^
                                                messageP->primitive.RequestedLinkEventList);

    mih_unsubscribed_req_event_list = ralpriv->mih_subscribe_req_event_list ^ saved_req_event_list;

    status = MIH_C_STATUS_SUCCESS;

    mRALte_send_event_unsubscribe_confirm(&messageP->header.transaction_id,
                                        &status,
                                        &mih_unsubscribed_req_event_list);
}



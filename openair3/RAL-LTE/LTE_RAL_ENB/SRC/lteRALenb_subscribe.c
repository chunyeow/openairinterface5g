/*****************************************************************************
 *   Eurecom OpenAirInterface 3
 *    Copyright(c) 2012 Eurecom
 *
 * Source eRALlte_subscribe.c
 *
 * Version 0.1
 *
 * Date  07/02/2012
 *
 * Product MIH RAL LTE
 *
 * Subsystem 
 *
 * Authors Michelle Wetterwald, Lionel Gauthier, Frederic Maurel
 *
 * Description 
 *
 *****************************************************************************/

#include "lteRALenb_subscribe.h"

#include "lteRALenb_mih_msg.h"
#include "lteRALenb_variables.h"

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:  eRALlte_subscribe_request()                               **
 **                                                                        **
 ** Description: Subscribes the MIH-F to receive specified link event      **
 **   indications and sends Link Event Subscribe confirmation   **
 **   to the MIH-F.                                             **
 **                                                                        **
 ** Inputs:  msgP:  Pointer to the received MIH message        **
 **     Others: None                                       **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: None                                       **
 **     Others: ralpriv                                    **
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
 ** Name:  eRALlte_unsubscribe_request()                             **
 **                                                                        **
 ** Description: Unsubscribes the MIH-F to receive specified link event    **
 **   indications and sends Link Event Unsubscribe confirmation **
 **   to the MIH-F.                                             **
 **                                                                        **
 ** Inputs:  msgP:  Pointer to the received MIH message        **
 **     Others: None                                       **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: None                                       **
 **     Others: ralpriv                                    **
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


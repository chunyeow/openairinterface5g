/*****************************************************************************
 *   Eurecom OpenAirInterface 3
 *    Copyright(c) 2012 Eurecom
 *
 * Source lteRALenb_subscribe.c
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
#define LTE_RAL_ENB
#define LTE_RAL_ENB_SUBSCRIBE_C
#include "lteRALenb.h"

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_subscribe_request()                                        **
 **                                                                        **
 ** Description: Subscribes the MIH-F to receive specified link event      **
 **   indications and sends Link Event Subscribe confirmation              **
 **   to the MIH-F.                                                        **
 **                                                                        **
 ** Inputs:  msgP:  Pointer to the received MIH message                    **
 **     Others: None                                                       **
 **                                                                        **
 ** Outputs:  None                                                         **
 **   Return: None                                                         **
 **     Others: ralpriv                                                    **
 **                                                                        **
 ***************************************************************************/
void eRAL_subscribe_request(ral_enb_instance_t instanceP, MIH_C_Message_Link_Event_Subscribe_request_t* msgP)
{
    MIH_C_STATUS_T status = MIH_C_STATUS_REJECTED;
    /* Check whether the action request is supported */
    if (g_enb_ral_obj[instanceP].mih_supported_link_command_list & MIH_C_BIT_LINK_EVENT_SUBSCRIBE)
    {
        MIH_C_LINK_EVENT_LIST_T mih_subscribed_req_event_list;

        g_enb_ral_obj[instanceP].mih_subscribe_req_event_list |= (msgP->primitive.RequestedLinkEventList & g_enb_ral_obj[instanceP].mih_supported_link_event_list);

        mih_subscribed_req_event_list = g_enb_ral_obj[instanceP].mih_subscribe_req_event_list & msgP->primitive.RequestedLinkEventList;

        status = MIH_C_STATUS_SUCCESS;

        eRAL_send_event_subscribe_confirm(instanceP, &msgP->header.transaction_id,
                &status,
                &mih_subscribed_req_event_list);
    }
    else
    {
        eRAL_send_event_subscribe_confirm(instanceP, &msgP->header.transaction_id,
                &status,
                NULL);
    }
}

/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_unsubscribe_request()                                      **
 **                                                                        **
 ** Description: Unsubscribes the MIH-F to receive specified link event    **
 **   indications and sends Link Event Unsubscribe confirmation            **
 **   to the MIH-F.                                                        **
 **                                                                        **
 ** Inputs:  msgP:  Pointer to the received MIH message                    **
 **     Others: None                                                       **
 **                                                                        **
 ** Outputs:  None                                                         **
 **   Return: None                                                         **
 **     Others: ralpriv                                                    **
 **                                                                        **
 ***************************************************************************/
void eRAL_unsubscribe_request(ral_enb_instance_t instanceP, MIH_C_Message_Link_Event_Unsubscribe_request_t* msgP)
{
    MIH_C_STATUS_T status = MIH_C_STATUS_REJECTED;
    /* Check whether the action request is supported */
    if (g_enb_ral_obj[instanceP].mih_supported_link_command_list & MIH_C_BIT_LINK_EVENT_UNSUBSCRIBE)
    {
        MIH_C_LINK_EVENT_LIST_T mih_unsubscribed_req_event_list;
        MIH_C_LINK_EVENT_LIST_T saved_req_event_list;

        saved_req_event_list = g_enb_ral_obj[instanceP].mih_subscribe_req_event_list;

        g_enb_ral_obj[instanceP].mih_subscribe_req_event_list &= ~(msgP->primitive.RequestedLinkEventList & g_enb_ral_obj[instanceP].mih_supported_link_event_list);
        mih_unsubscribed_req_event_list = g_enb_ral_obj[instanceP].mih_subscribe_req_event_list ^ saved_req_event_list;

        status = MIH_C_STATUS_SUCCESS;

        eRAL_send_event_unsubscribe_confirm(instanceP, &msgP->header.transaction_id,
                &status,
                &mih_unsubscribed_req_event_list);
    }
    else
    {
        eRAL_send_event_unsubscribe_confirm(instanceP, &msgP->header.transaction_id,
                &status,
                NULL);
    }
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/


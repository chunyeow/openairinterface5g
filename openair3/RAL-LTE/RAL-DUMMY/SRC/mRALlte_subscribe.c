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



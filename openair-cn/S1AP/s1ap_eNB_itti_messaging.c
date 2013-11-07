#include "intertask_interface.h"

#include "s1ap_eNB_itti_messaging.h"

void s1ap_eNB_itti_send_sctp_data_req(int32_t assoc_id, uint8_t *buffer,
                                      uint32_t buffer_length, uint16_t stream)
{
    MessageDef      *message_p;
    sctp_data_req_t *sctp_data_req;

    message_p = itti_alloc_new_message(TASK_S1AP, SCTP_DATA_REQ);

    sctp_data_req = &message_p->msg.sctp_data_req;

    sctp_data_req->assoc_id      = assoc_id;
    sctp_data_req->buffer        = buffer;
    sctp_data_req->buffer_length = buffer_length;
    sctp_data_req->stream        = stream;

    itti_send_msg_to_task(TASK_SCTP, INSTANCE_DEFAULT, message_p);
}

void s1ap_eNB_itti_send_nas_downlink_ind(uint8_t mod_id, uint8_t *nas_pdu,
                                         uint32_t nas_pdu_length)
{
    MessageDef          *message_p;
    s1ap_downlink_nas_t *s1ap_downlink_nas;

    message_p = itti_alloc_new_message(TASK_S1AP, S1AP_DOWNLINK_NAS);

    s1ap_downlink_nas = &message_p->msg.s1ap_downlink_nas;

    s1ap_downlink_nas->mod_id         = mod_id;
    s1ap_downlink_nas->nas_pdu.buffer = nas_pdu;
    s1ap_downlink_nas->nas_pdu.length = nas_pdu_length;

    itti_send_msg_to_task(TASK_RRC_ENB, INSTANCE_DEFAULT, message_p);
}

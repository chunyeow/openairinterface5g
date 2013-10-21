#include <string.h>

#include "intertask_interface.h"

#include "sctp_itti_messaging.h"

int sctp_itti_send_new_association(uint32_t assoc_id, uint16_t instreams,
                                   uint16_t outstreams)
{
    MessageDef      *message_p;
    sctp_new_peer_t *sctp_new_peer_p;

    message_p = alloc_new_message(TASK_SCTP, SCTP_NEW_ASSOCIATION);

    sctp_new_peer_p = &message_p->msg.sctp_new_peer;

    sctp_new_peer_p->assoc_id   = assoc_id;
    sctp_new_peer_p->instreams  = instreams;
    sctp_new_peer_p->outstreams = outstreams;

    return send_msg_to_task(TASK_S1AP, INSTANCE_DEFAULT, message_p);
}

int sctp_itti_send_new_message_ind(int n, uint8_t *buffer, uint32_t assoc_id,
                                   uint16_t stream,
                                   uint16_t instreams, uint16_t outstreams)
{
    MessageDef              *message_p;
    s1ap_sctp_new_msg_ind_t *sctp_new_msg_ind_p;

    message_p = alloc_new_message(TASK_SCTP, S1AP_SCTP_NEW_MESSAGE_IND);

    sctp_new_msg_ind_p = &message_p->msg.s1ap_sctp_new_msg_ind;

    sctp_new_msg_ind_p->buffer = malloc(sizeof(uint8_t) * n);

    memcpy((void *)sctp_new_msg_ind_p->buffer, (void *)buffer, n);

    sctp_new_msg_ind_p->stream = stream;
    sctp_new_msg_ind_p->buf_length = n;
    sctp_new_msg_ind_p->assoc_id = assoc_id;
    sctp_new_msg_ind_p->instreams = instreams;
    sctp_new_msg_ind_p->outstreams = outstreams;

    return send_msg_to_task(TASK_S1AP, INSTANCE_DEFAULT, message_p);
}

int sctp_itti_send_com_down_ind(uint32_t assoc_id)
{
    MessageDef *message_p;
    sctp_close_association_t *sctp_close_association_p;

    message_p = alloc_new_message(TASK_SCTP, SCTP_CLOSE_ASSOCIATION);

    sctp_close_association_p = &message_p->msg.sctp_close_association;

    sctp_close_association_p->assoc_id = assoc_id;

    return send_msg_to_task(TASK_S1AP, INSTANCE_DEFAULT, message_p);
}

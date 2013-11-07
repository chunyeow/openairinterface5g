#include "intertask_interface.h"

#include "sctp_common.h"
#include "sctp_eNB_itti_messaging.h"

int sctp_itti_send_new_message_ind(task_id_t task_id, uint32_t assoc_id, uint8_t *buffer,
                                   uint32_t buffer_length, uint16_t stream)
{
    MessageDef      *message_p;
    sctp_data_ind_t *sctp_data_ind_p;

    message_p = itti_alloc_new_message(TASK_SCTP, SCTP_DATA_IND);

    sctp_data_ind_p = &message_p->msg.sctp_data_ind;

    sctp_data_ind_p->buffer = malloc(sizeof(uint8_t) * buffer_length);

    /* Copy the buffer */
    memcpy((void *)sctp_data_ind_p->buffer, (void *)buffer, buffer_length);

    sctp_data_ind_p->stream        = stream;
    sctp_data_ind_p->buffer_length = buffer_length;
    sctp_data_ind_p->assoc_id      = assoc_id;

    return itti_send_msg_to_task(task_id, INSTANCE_DEFAULT, message_p);
}

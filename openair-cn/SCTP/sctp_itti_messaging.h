#ifndef SCTP_ITTI_MESSAGING_H_
#define SCTP_ITTI_MESSAGING_H_

int sctp_itti_send_new_association(uint32_t assoc_id, uint16_t instreams,
                                   uint16_t outstreams);

int sctp_itti_send_new_message_ind(int n, uint8_t *buffer, uint32_t assoc_id,
                                   uint16_t stream,
                                   uint16_t instreams, uint16_t outstreams);

int sctp_itti_send_com_down_ind(uint32_t assoc_id);

#endif /* SCTP_ITTI_MESSAGING_H_ */

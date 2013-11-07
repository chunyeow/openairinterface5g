#ifndef S1AP_ENB_ITTI_MESSAGING_H_
#define S1AP_ENB_ITTI_MESSAGING_H_

void s1ap_eNB_itti_send_sctp_data_req(int32_t assoc_id, uint8_t *buffer,
                                      uint32_t buffer_length, uint16_t stream);

#endif /* S1AP_ENB_ITTI_MESSAGING_H_ */

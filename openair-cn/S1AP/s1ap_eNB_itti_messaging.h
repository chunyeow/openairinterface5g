#ifndef S1AP_ENB_ITTI_MESSAGING_H_
#define S1AP_ENB_ITTI_MESSAGING_H_

void s1ap_eNB_itti_send_sctp_data_req(int32_t assoc_id, uint8_t *buffer,
                                      uint32_t buffer_length, uint16_t stream);

void s1ap_eNB_itti_send_nas_downlink_ind(uint8_t mod_id, uint8_t *nas_pdu,
                                         uint32_t nas_pdu_length);

#endif /* S1AP_ENB_ITTI_MESSAGING_H_ */

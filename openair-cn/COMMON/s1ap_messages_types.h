#ifndef S1AP_MESSAGES_TYPES_H_
#define S1AP_MESSAGES_TYPES_H_

typedef struct {
    uint8_t  *buffer;           ///< SCTP buffer
    uint32_t  buf_length;       ///< SCTP buffer length
    int32_t   assoc_id;         ///< SCTP physical association ID
    uint8_t   stream;           ///< Stream number on which data had been received
    uint16_t  instreams;        ///< Number of input streams for the SCTP connection between peers
    uint16_t  outstreams;       ///< Number of output streams for the SCTP connection between peers
} s1ap_sctp_new_msg_ind_t;

typedef struct s1ap_initial_ue_message_s {
    unsigned eNB_ue_s1ap_id:24;
    uint32_t mme_ue_s1ap_id;
    cgi_t    e_utran_cgi;
} s1ap_initial_ue_message_t;

typedef struct s1ap_initial_ctxt_setup_req_s {
    unsigned eNB_ue_s1ap_id:24;
    uint32_t mme_ue_s1ap_id;

    /* Key eNB */
    uint8_t keNB[32];

    ambr_t   ambr;
    ambr_t   apn_ambr;

    /* EPS bearer ID */
    unsigned ebi:4;

    /* QoS */
    qci_t                   qci;
    priority_level_t        prio_level;
    pre_emp_vulnerability_t pre_emp_vulnerability;
    pre_emp_capability_t    pre_emp_capability;

    /* S-GW TEID for user-plane */
    uint32_t teid;
    /* S-GW IP address for User-Plane */
    ip_address_t s_gw_address;
} s1ap_initial_ctxt_setup_req_t;

typedef struct s1ap_ue_cap_ind_s {
    unsigned eNB_ue_s1ap_id:24;
    uint32_t mme_ue_s1ap_id;
    uint8_t  radio_capabilities[100];
    uint32_t radio_capabilities_length;
} s1ap_ue_cap_ind_t;

#endif /* S1AP_MESSAGES_TYPES_H_ */

#ifndef S1AP_MESSAGES_TYPES_H_
#define S1AP_MESSAGES_TYPES_H_

#define S1AP_ENB_DEREGISTERED_IND(mSGpTR) (mSGpTR)->ittiMsg.s1ap_eNB_deregistered_ind
#define S1AP_DEREGISTER_UE_REQ(mSGpTR)    (mSGpTR)->ittiMsg.s1ap_deregister_ue_req

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

#define S1AP_ITTI_UE_PER_DEREGISTER_MESSAGE 20
typedef struct s1ap_eNB_deregistered_ind_s {
    uint8_t  nb_ue_to_deregister;
    uint32_t mme_ue_s1ap_id[S1AP_ITTI_UE_PER_DEREGISTER_MESSAGE];
} s1ap_eNB_deregistered_ind_t;

typedef struct s1ap_deregister_ue_req_s {
    uint32_t mme_ue_s1ap_id;
} s1ap_deregister_ue_req_t;

#endif /* S1AP_MESSAGES_TYPES_H_ */

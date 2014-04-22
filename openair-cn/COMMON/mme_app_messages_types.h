#ifndef MME_APP_MESSAGES_TYPES_H_
#define MME_APP_MESSAGES_TYPES_H_


#define MME_APP_CONNECTION_ESTABLISHMENT_IND(mSGpTR)     (mSGpTR)->ittiMsg.mme_app_connection_establishment_ind
#define MME_APP_CONNECTION_ESTABLISHMENT_CNF(mSGpTR)     (mSGpTR)->ittiMsg.mme_app_connection_establishment_cnf
#define MME_APP_INITIAL_CONTEXT_SETUP_RSP(mSGpTR)        (mSGpTR)->ittiMsg.mme_app_initial_context_setup_rsp

typedef struct mme_app_connection_establishment_ind_s {
    uint32_t            mme_ue_s1ap_id;
    nas_establish_ind_t nas;

    /* Transparent message from s1ap to be forwarded to MME_APP or
     * to S1AP if connection establishment is rejected by NAS.
     */
    s1ap_initial_ue_message_t transparent;
} mme_app_connection_establishment_ind_t;

typedef struct mme_app_connection_establishment_cnf_s {

    ebi_t                   eps_bearer_id;
    FTeid_t                 bearer_s1u_sgw_fteid;
    qci_t                   bearer_qos_qci;
    priority_level_t        bearer_qos_prio_level;
    pre_emp_vulnerability_t bearer_qos_pre_emp_vulnerability;
    pre_emp_capability_t    bearer_qos_pre_emp_capability;
    ambr_t                  ambr;

    nas_conn_est_cnf_t       nas_conn_est_cnf;
} mme_app_connection_establishment_cnf_t;

typedef struct mme_app_initial_context_setup_rsp_s {
    uint32_t                mme_ue_s1ap_id;
    ebi_t                   eps_bearer_id;
    FTeid_t                 bearer_s1u_enb_fteid;
} mme_app_initial_context_setup_rsp_t;


#endif /* MME_APP_MESSAGES_TYPES_H_ */

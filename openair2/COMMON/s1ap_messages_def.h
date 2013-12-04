/* eNB application layer -> S1AP messages */
MESSAGE_DEF(S1AP_REGISTER_ENB_REQ          , MESSAGE_PRIORITY_MED, s1ap_register_enb_req_t          , s1ap_register_enb_req)

/* S1AP -> eNB application layer messages */
MESSAGE_DEF(S1AP_REGISTER_ENB_CNF          , MESSAGE_PRIORITY_MED, s1ap_register_enb_cnf_t          , s1ap_register_enb_cnf)

/* RRC -> S1AP messages */
MESSAGE_DEF(S1AP_NAS_FIRST_REQ             , MESSAGE_PRIORITY_MED, s1ap_nas_first_req_t             , s1ap_nas_first_req)
MESSAGE_DEF(S1AP_UPLINK_NAS                , MESSAGE_PRIORITY_MED, s1ap_uplink_nas_t                , s1ap_uplink_nas)
MESSAGE_DEF(S1AP_UE_CAPABILITIES_IND       , MESSAGE_PRIORITY_MED, s1ap_ue_cap_info_ind_t           , s1ap_ue_cap_info_ind)
MESSAGE_DEF(S1AP_INITIAL_CONTEXT_SETUP_RESP, MESSAGE_PRIORITY_MED, s1ap_initial_context_setup_resp_t, s1ap_initial_context_setup_resp)
MESSAGE_DEF(S1AP_INITIAL_CONTEXT_SETUP_FAIL, MESSAGE_PRIORITY_MED, s1ap_initial_context_setup_fail_t, s1ap_initial_context_setup_fail)
MESSAGE_DEF(S1AP_NAS_NON_DELIVERY_IND      , MESSAGE_PRIORITY_MED, s1ap_nas_non_delivery_ind_t      , s1ap_nas_non_delivery_ind)
MESSAGE_DEF(S1AP_UE_CONTEXT_RELEASE_RESP   , MESSAGE_PRIORITY_MED, s1ap_ue_release_resp_t           , s1ap_ue_release_resp)
MESSAGE_DEF(S1AP_UE_CTXT_MODIFICATION_RESP , MESSAGE_PRIORITY_MED, s1ap_ue_ctxt_modification_resp_t , s1ap_ue_ctxt_modification_resp)
MESSAGE_DEF(S1AP_UE_CTXT_MODIFICATION_FAIL , MESSAGE_PRIORITY_MED, s1ap_ue_ctxt_modification_fail_t , s1ap_ue_ctxt_modification_fail)

/* S1AP -> RRC messages */
MESSAGE_DEF(S1AP_DOWNLINK_NAS              , MESSAGE_PRIORITY_MED, s1ap_downlink_nas_t              , s1ap_downlink_nas )
MESSAGE_DEF(S1AP_INITIAL_CONTEXT_SETUP_REQ , MESSAGE_PRIORITY_MED, s1ap_initial_context_setup_req_t , s1ap_initial_context_setup_req )
MESSAGE_DEF(S1AP_UE_CTXT_MODIFICATION_REQ  , MESSAGE_PRIORITY_MED, s1ap_ue_ctxt_modification_req_t  , s1ap_ue_ctxt_modification_req)
MESSAGE_DEF(S1AP_PAGING_IND                , MESSAGE_PRIORITY_MED, s1ap_paging_ind_t                , s1ap_paging_ind )

/* S1AP <-> RRC messages (can be initiated either by MME or eNB) */
MESSAGE_DEF(S1AP_UE_CONTEXT_RELEASE_REQ    , MESSAGE_PRIORITY_MED, s1ap_ue_release_req_t            , s1ap_ue_release_req)

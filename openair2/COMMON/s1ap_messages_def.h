/* eNB application layer -> S1AP messages */
MESSAGE_DEF(S1AP_REGISTER_ENB              , MESSAGE_PRIORITY_MED, s1ap_register_eNB_t              , s1ap_register_eNB )

/* RRC -> S1AP messages */
MESSAGE_DEF(S1AP_NAS_FIRST_REQ             , MESSAGE_PRIORITY_MED, s1ap_nas_first_req_t             , s1ap_nas_first_req)
MESSAGE_DEF(S1AP_UPLINK_NAS                , MESSAGE_PRIORITY_MED, s1ap_uplink_nas_t                , s1ap_uplink_nas   )
MESSAGE_DEF(S1AP_UE_CAPABILITIES_IND       , MESSAGE_PRIORITY_MED, s1ap_ue_cap_info_ind_t           , s1ap_ue_cap_info_ind)
MESSAGE_DEF(S1AP_INITIAL_CONTEXT_SETUP_RESP, MESSAGE_PRIORITY_MED, s1ap_initial_context_setup_resp_t, s1ap_initial_context_setup_resp)
MESSAGE_DEF(S1AP_INITIAL_CONTEXT_SETUP_FAIL, MESSAGE_PRIORITY_MED, s1ap_initial_context_setup_fail_t, s1ap_initial_context_setup_fail)

/* S1AP -> RRC messages */
MESSAGE_DEF(S1AP_DOWNLINK_NAS              , MESSAGE_PRIORITY_MED, s1ap_downlink_nas_t              , s1ap_downlink_nas )
MESSAGE_DEF(S1AP_INITIAL_CONTEXT_SETUP_REQ , MESSAGE_PRIORITY_MED, s1ap_initial_context_setup_req_t , s1ap_initial_context_setup_req )
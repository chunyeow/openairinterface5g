#include "as_message.h"

#ifndef NAS_MESSAGES_TYPES_H_
#define NAS_MESSAGES_TYPES_H_

#define NAS_UL_DATA_IND(mSGpTR)                 (mSGpTR)->ittiMsg.nas_ul_data_ind
#define NAS_DL_DATA_REQ(mSGpTR)                 (mSGpTR)->ittiMsg.nas_dl_data_req
#define NAS_DL_DATA_CNF(mSGpTR)                 (mSGpTR)->ittiMsg.nas_dl_data_cnf
#define NAS_CONN_EST_IND(mSGpTR)                (mSGpTR)->ittiMsg.nas_conn_est_ind
#define NAS_CONN_EST_CNF(mSGpTR)                (mSGpTR)->ittiMsg.nas_conn_est_cnf
#define NAS_BEARER_PARAM(mSGpTR)                (mSGpTR)->ittiMsg.nas_bearer_param
#define NAS_AUTHENTICATION_REQ(mSGpTR)          (mSGpTR)->ittiMsg.nas_auth_req
#define NAS_AUTHENTICATION_PARAM_REQ(mSGpTR)    (mSGpTR)->ittiMsg.nas_auth_param_req

typedef struct {
    
} nas_paging_ind_t;

typedef struct {
    nas_establish_ind_t nas;

    /* Transparent message from s1ap to be forwarded to MME_APP or
     * to S1AP if connection establishment is rejected by NAS.
     */
    s1ap_initial_ue_message_t transparent;
} nas_conn_est_ind_t;

typedef struct nas_conn_est_cnf_s {
    uint32_t ue_id;

    nas_establish_cnf_t nas_establish_cnf;
    /* Transparent message from MME_APP to S1AP */
    s1ap_initial_ctxt_setup_req_t transparent;
} nas_conn_est_cnf_t;

typedef struct nas_bearer_param_s {
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
} nas_bearer_param_t;

typedef struct {
    
} nas_conn_rel_ind_t;

typedef ul_info_transfer_ind_t nas_ul_data_ind_t;

typedef dl_info_transfer_req_t nas_dl_data_req_t;
typedef dl_info_transfer_cnf_t nas_dl_data_cnf_t;

typedef struct {
    
} nas_non_del_ind_t;

typedef struct {
    
} nas_rab_est_req_t;

typedef struct {
    
} nas_rab_est_rsp_t;

typedef struct {
    
} nas_rab_rel_req_t;

typedef struct {
    /* TODO: Set the correct size */
    char apn[100];
    char imsi[16];
#define INITIAL_REQUEST (0x1)
    unsigned initial:1;
    s1ap_initial_ue_message_t transparent;
} nas_attach_req_t;

typedef struct {
    /* UE imsi */
    char imsi[16];

#define NAS_FAILURE_OK  0x0
#define NAS_FAILURE_IND 0x1
    unsigned failure:1;
    int cause;
} nas_auth_req_t;

typedef struct {
    char imsi[16];
} nas_auth_resp_t;

typedef struct nas_auth_param_req_s {
    uint8_t imsi_length;
    char    imsi[15];

    uint8_t initial_req:1;
} nas_auth_param_req_t;

typedef struct nas_attach_accept_s {
    
} nas_attach_accept_t;

#endif /* NAS_MESSAGES_TYPES_H_ */

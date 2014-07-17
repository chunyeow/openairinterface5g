/*******************************************************************************
    OpenAirInterface
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is
   included in this distribution in the file called "COPYING". If not,
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr

  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.

 *******************************************************************************/
//WARNING: Do not include this header directly. Use intertask_interface.h instead.

// Messages for NAS logging
MESSAGE_DEF(NAS_DL_EMM_RAW_MSG,                 MESSAGE_PRIORITY_MED,   nas_raw_msg_t,              nas_dl_emm_raw_msg)
MESSAGE_DEF(NAS_UL_EMM_RAW_MSG,                 MESSAGE_PRIORITY_MED,   nas_raw_msg_t,              nas_ul_emm_raw_msg)

MESSAGE_DEF(NAS_DL_EMM_PLAIN_MSG,               MESSAGE_PRIORITY_MED,   nas_emm_plain_msg_t,        nas_dl_emm_plain_msg)
MESSAGE_DEF(NAS_UL_EMM_PLAIN_MSG,               MESSAGE_PRIORITY_MED,   nas_emm_plain_msg_t,        nas_ul_emm_plain_msg)
MESSAGE_DEF(NAS_DL_EMM_PROTECTED_MSG,           MESSAGE_PRIORITY_MED,   nas_emm_protected_msg_t,    nas_dl_emm_protected_msg)
MESSAGE_DEF(NAS_UL_EMM_PROTECTED_MSG,           MESSAGE_PRIORITY_MED,   nas_emm_protected_msg_t,    nas_ul_emm_protected_msg)

MESSAGE_DEF(NAS_DL_ESM_RAW_MSG,                 MESSAGE_PRIORITY_MED,   nas_raw_msg_t,              nas_dl_esm_raw_msg)
MESSAGE_DEF(NAS_UL_ESM_RAW_MSG,                 MESSAGE_PRIORITY_MED,   nas_raw_msg_t,              nas_ul_esm_raw_msg)

MESSAGE_DEF(NAS_DL_ESM_PLAIN_MSG,               MESSAGE_PRIORITY_MED,   nas_esm_plain_msg_t,        nas_dl_esm_plain_msg)
MESSAGE_DEF(NAS_UL_ESM_PLAIN_MSG,               MESSAGE_PRIORITY_MED,   nas_esm_plain_msg_t,        nas_ul_esm_plain_msg)
MESSAGE_DEF(NAS_DL_ESM_PROTECTED_MSG,           MESSAGE_PRIORITY_MED,   nas_esm_protected_msg_t,    nas_dl_esm_protected_msg)
MESSAGE_DEF(NAS_UL_ESM_PROTECTED_MSG,           MESSAGE_PRIORITY_MED,   nas_esm_protected_msg_t,    nas_ul_esm_protected_msg)

/* */
MESSAGE_DEF(NAS_PAGING_IND,                     MESSAGE_PRIORITY_MED,   nas_paging_ind_t,           nas_paging_ind)
MESSAGE_DEF(NAS_PDN_CONNECTIVITY_REQ,           MESSAGE_PRIORITY_MED,   nas_pdn_connectivity_req_t, nas_pdn_connectivity_req)
MESSAGE_DEF(NAS_CONNECTION_ESTABLISHMENT_IND,   MESSAGE_PRIORITY_MED,   nas_conn_est_ind_t,         nas_conn_est_ind)
MESSAGE_DEF(NAS_CONNECTION_ESTABLISHMENT_CNF,   MESSAGE_PRIORITY_MED,   nas_conn_est_cnf_t,         nas_conn_est_cnf)
MESSAGE_DEF(NAS_CONNECTION_RELEASE_IND,         MESSAGE_PRIORITY_MED,   nas_conn_rel_ind_t,         nas_conn_rel_ind)
MESSAGE_DEF(NAS_UPLINK_DATA_IND,                MESSAGE_PRIORITY_MED,   nas_ul_data_ind_t,          nas_ul_data_ind)
MESSAGE_DEF(NAS_DOWNLINK_DATA_REQ,              MESSAGE_PRIORITY_MED,   nas_dl_data_req_t,          nas_dl_data_req)
MESSAGE_DEF(NAS_DOWNLINK_DATA_CNF,              MESSAGE_PRIORITY_MED,   nas_dl_data_cnf_t,          nas_dl_data_cnf)
MESSAGE_DEF(NAS_NON_DELIVERY_IND,               MESSAGE_PRIORITY_MED,   nas_non_del_ind_t,          nas_non_del_ind)
MESSAGE_DEF(NAS_RAB_ESTABLISHMENT_REQ,          MESSAGE_PRIORITY_MED,   nas_rab_est_req_t,          nas_rab_est_req)
MESSAGE_DEF(NAS_RAB_ESTABLISHMENT_RESP,         MESSAGE_PRIORITY_MED,   nas_rab_est_rsp_t,          nas_rab_est_rsp)
MESSAGE_DEF(NAS_RAB_RELEASE_REQ,                MESSAGE_PRIORITY_MED,   nas_rab_rel_req_t,          nas_rab_rel_req)

/* NAS layer -> MME app messages */
MESSAGE_DEF(NAS_AUTHENTICATION_PARAM_REQ,       MESSAGE_PRIORITY_MED,   nas_auth_param_req_t,       nas_auth_param_req)

/* MME app -> NAS layer messages */
MESSAGE_DEF(NAS_PDN_CONNECTIVITY_RSP,           MESSAGE_PRIORITY_MED,   nas_pdn_connectivity_rsp_t,  nas_pdn_connectivity_rsp)
MESSAGE_DEF(NAS_PDN_CONNECTIVITY_FAIL,          MESSAGE_PRIORITY_MED,   nas_pdn_connectivity_fail_t, nas_pdn_connectivity_fail)
MESSAGE_DEF(NAS_AUTHENTICATION_PARAM_RSP,       MESSAGE_PRIORITY_MED,   nas_auth_param_rsp_t,        nas_auth_param_rsp)
MESSAGE_DEF(NAS_AUTHENTICATION_PARAM_FAIL,      MESSAGE_PRIORITY_MED,   nas_auth_param_fail_t,       nas_auth_param_fail)

#if defined(DISABLE_USE_NAS)
MESSAGE_DEF(NAS_ATTACH_REQ,                     MESSAGE_PRIORITY_MED,   nas_attach_req_t,           nas_attach_req)
MESSAGE_DEF(NAS_ATTACH_ACCEPT,                  MESSAGE_PRIORITY_MED,   nas_attach_accept_t,        nas_attach_accept)
MESSAGE_DEF(NAS_AUTHENTICATION_RESP,            MESSAGE_PRIORITY_MED,   nas_auth_resp_t,            nas_auth_resp)
MESSAGE_DEF(NAS_AUTHENTICATION_REQ,             MESSAGE_PRIORITY_MED,   nas_auth_req_t,             nas_auth_req)
#endif

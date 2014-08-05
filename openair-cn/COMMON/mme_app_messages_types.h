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

    /* Key eNB */
    uint8_t                 keNB[32];
    uint16_t                security_capabilities_encryption_algorithms;
    uint16_t                security_capabilities_integrity_algorithms;

    nas_conn_est_cnf_t       nas_conn_est_cnf;
} mme_app_connection_establishment_cnf_t;

typedef struct mme_app_initial_context_setup_rsp_s {
    uint32_t                mme_ue_s1ap_id;
    ebi_t                   eps_bearer_id;
    FTeid_t                 bearer_s1u_enb_fteid;
} mme_app_initial_context_setup_rsp_t;


#endif /* MME_APP_MESSAGES_TYPES_H_ */

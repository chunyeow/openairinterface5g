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


/* This file contains definitions related to mme applicative layer and should
 * not be included within other layers.
 * Use mme_app_extern.h to expose mme applicative layer procedures/data.
 */

#include "intertask_interface.h"
#include "mme_app_ue_context.h"

#ifndef MME_APP_DEFS_H_
#define MME_APP_DEFS_H_

#ifndef MME_APP_DEBUG
# define MME_APP_DEBUG(x, args...) do { fprintf(stdout, "[MMEA][D]"x, ##args); } while(0)
#endif
#ifndef MME_APP_ERROR
# define MME_APP_ERROR(x, args...) do { fprintf(stdout, "[MMEA][E]"x, ##args); } while(0)
#endif

typedef struct {
    /* UE contexts + some statistics variables */
    mme_ue_context_t mme_ue_contexts;

    long statistic_timer_id;
    uint32_t statistic_timer_period;
} mme_app_desc_t;

extern mme_app_desc_t mme_app_desc;


#if defined(DISABLE_USE_NAS)
int mme_app_handle_attach_req(nas_attach_req_t *attach_req_p);
#endif

int mme_app_handle_s1ap_ue_capabilities_ind  (const s1ap_ue_cap_ind_t const * s1ap_ue_cap_ind_pP);

int mme_app_send_s11_create_session_req      (struct ue_context_s * const ue_context_pP);

int mme_app_send_s6a_update_location_req     (struct ue_context_s * const ue_context_pP);

int mme_app_handle_s6a_update_location_ans   (const s6a_update_location_ans_t * const ula_pP);

int mme_app_handle_nas_pdn_connectivity_req  ( nas_pdn_connectivity_req_t * const nas_pdn_connectivity_req_p);

void mme_app_handle_conn_est_cnf             (const nas_conn_est_cnf_t * const nas_conn_est_cnf_pP);

void mme_app_handle_conn_est_ind             (const mme_app_connection_establishment_ind_t * const conn_est_ind_pP);

int mme_app_handle_create_sess_resp          (const SgwCreateSessionResponse * const create_sess_resp_pP);

int mme_app_handle_establish_ind             (const nas_establish_ind_t * const nas_establish_ind_pP);

int mme_app_handle_authentication_info_answer(const s6a_auth_info_ans_t * const s6a_auth_info_ans_pP);

int mme_app_handle_nas_auth_resp             (const nas_auth_resp_t * const nas_auth_resp_pP);

nas_cause_t s6a_error_2_nas_cause            (const uint32_t s6a_errorP, const int experimentalP);

void mme_app_handle_nas_auth_param_req       (const nas_auth_param_req_t * const nas_auth_param_req_pP);

void mme_app_handle_initial_context_setup_rsp(const mme_app_initial_context_setup_rsp_t * const initial_ctxt_setup_rsp_pP);

#endif /* MME_APP_DEFS_H_ */

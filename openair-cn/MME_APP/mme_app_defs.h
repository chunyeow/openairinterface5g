/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2013 Eurecom

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information
  Openair Admin: openair_admin@eurecom.fr
  Openair Tech : openair_tech@eurecom.fr
  Forums       : http://forums.eurecom.fr/openairinterface
  Address      : EURECOM, Campus SophiaTech, 450 Route des Chappes
                 06410 Biot FRANCE

*******************************************************************************/

/* This file contains definitions related to mme applicative layer and should
 * not be included within other layers.
 * Use mme_app_extern.h to expose mme applicative layer procedures/data.
 */

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

int mme_app_create_bearer(s6a_update_location_ans_t *ula_p);

#if defined(DISABLE_USE_NAS)
int mme_app_handle_attach_req(nas_attach_req_t *attach_req_p);
#endif

int mme_app_handle_create_sess_resp(SgwCreateSessionResponse *create_sess_resp_p);

int mme_app_handle_establish_ind(nas_establish_ind_t *nas_establish_ind_p);

int mme_app_handle_authentication_info_answer(s6a_auth_info_ans_t *s6a_auth_info_ans_p);

int mme_app_handle_nas_auth_resp(nas_auth_resp_t *nas_auth_resp_p);

int s6a_error_2_nas_cause(uint32_t s6a_error, int experimental);

#endif /* MME_APP_DEFS_H_ */

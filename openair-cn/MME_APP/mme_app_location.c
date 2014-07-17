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

/*! \file mme_app_location.c
 * \brief
 * \author Sebastien ROUX, Lionel GAUTHIER
 * \version 1.0
 * \company Eurecom
 * \email: lionel.gauthier@eurecom.fr
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "intertask_interface.h"
#include "mme_config.h"

#include "mme_app_extern.h"
#include "mme_app_ue_context.h"
#include "mme_app_defs.h"

#include "secu_defs.h"

#include "assertions.h"
#include "common_types.h"


int
mme_app_send_s6a_update_location_req(
    struct ue_context_s * const ue_context_pP)
{
  struct ue_context_s       *ue_context_p = NULL;
  uint64_t                   imsi         = 0;
  MessageDef                *message_p    = NULL;
  s6a_update_location_req_t *s6a_ulr_p    = NULL;


  MME_APP_STRING_TO_IMSI((char *)ue_context_pP->pending_pdn_connectivity_req_imsi, &imsi);

  MME_APP_DEBUG("Handling imsi %"IMSI_FORMAT"\n", imsi);

  if ((ue_context_p = mme_ue_context_exists_imsi(&mme_app_desc.mme_ue_contexts,
                    imsi)) == NULL) {
      MME_APP_ERROR("That's embarrassing as we don't know this IMSI\n");
      return -1;
  }

  message_p = itti_alloc_new_message(TASK_MME_APP, S6A_UPDATE_LOCATION_REQ);
  if (message_p == NULL) {
      return -1;
  }

  s6a_ulr_p = &message_p->ittiMsg.s6a_update_location_req;
  memset((void*)s6a_ulr_p, 0, sizeof(s6a_update_location_req_t));

  MME_APP_IMSI_TO_STRING(imsi, s6a_ulr_p->imsi);
  s6a_ulr_p->imsi_length         = strlen(s6a_ulr_p->imsi);
  s6a_ulr_p->initial_attach      = INITIAL_ATTACH;
  memcpy(&s6a_ulr_p->visited_plmn, &ue_context_p->guti.gummei.plmn, sizeof(plmn_t));
  s6a_ulr_p->rat_type            = RAT_EUTRAN;
  /* Check if we already have UE data */
  s6a_ulr_p->skip_subscriber_data = 0;

  return itti_send_msg_to_task(TASK_S6A, INSTANCE_DEFAULT, message_p);
}



int
mme_app_handle_s6a_update_location_ans(
        const s6a_update_location_ans_t * const ula_pP)
{
    uint64_t                    imsi              = 0;
    struct ue_context_s        *ue_context_p      = NULL;

    DevAssert(ula_pP != NULL);

    if (ula_pP->result.present == S6A_RESULT_BASE) {
        if (ula_pP->result.choice.base != DIAMETER_SUCCESS) {
            /* The update location procedure has failed. Notify the NAS layer
             * and don't initiate the bearer creation on S-GW side.
             */
        	MME_APP_DEBUG("ULR/ULA procedure returned non success (ULA.result.choice.base=%d)\n", ula_pP->result.choice.base);
        	DevMessage("ULR/ULA procedure returned non success\n");
        }
    } else {
        /* The update location procedure has failed. Notify the NAS layer
         * and don't initiate the bearer creation on S-GW side.
         */
    	MME_APP_DEBUG("ULR/ULA procedure returned non success (ULA.result.present=%d)\n", ula_pP->result.present);
        DevMessage("ULR/ULA procedure returned non success\n");
    }

    MME_APP_STRING_TO_IMSI((char *)ula_pP->imsi, &imsi);

    MME_APP_DEBUG("%s Handling imsi %"IMSI_FORMAT"\n", __FUNCTION__, imsi);

    if ((ue_context_p = mme_ue_context_exists_imsi(&mme_app_desc.mme_ue_contexts, imsi)) == NULL) {
        MME_APP_ERROR("That's embarrassing as we don't know this IMSI\n");
        return -1;
    }

    ue_context_p->subscription_known = SUBSCRIPTION_KNOWN;

    ue_context_p->sub_status = ula_pP->subscription_data.subscriber_status;
    ue_context_p->access_restriction_data = ula_pP->subscription_data.access_restriction;

    /* Copy the subscribed ambr to the sgw create session request message */
    memcpy(&ue_context_p->subscribed_ambr, &ula_pP->subscription_data.subscribed_ambr,
           sizeof(ambr_t));
    memcpy(
            ue_context_p->msisdn,
            ula_pP->subscription_data.msisdn,
            ula_pP->subscription_data.msisdn_length);
    ue_context_p->msisdn_length = ula_pP->subscription_data.msisdn_length;
    AssertFatal(ula_pP->subscription_data.msisdn_length != 0, "MSISDN LENGTH IS 0");
    AssertFatal(ula_pP->subscription_data.msisdn_length <= MSISDN_LENGTH, "MSISDN LENGTH is too high %u", MSISDN_LENGTH);
    ue_context_p->msisdn[ue_context_p->msisdn_length] = '\0';

    ue_context_p->rau_tau_timer = ula_pP->subscription_data.rau_tau_timer;
    ue_context_p->access_mode   = ula_pP->subscription_data.access_mode;

    memcpy(&ue_context_p->apn_profile, &ula_pP->subscription_data.apn_config_profile,
           sizeof(apn_config_profile_t));

    return mme_app_send_s11_create_session_req(ue_context_p);
}


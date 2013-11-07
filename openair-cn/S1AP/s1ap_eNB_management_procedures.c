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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "intertask_interface.h"

#include "assertions.h"
#include "conversions.h"

#include "s1ap_common.h"
#include "s1ap_eNB_defs.h"
#include "s1ap_eNB.h"

s1ap_eNB_internal_data_t s1ap_eNB_internal_data;

static int s1ap_eNB_generate_s1_setup_request(
    s1ap_eNB_instance_t *instance_p, s1ap_eNB_mme_data_t *s1ap_mme_data_p);

RB_GENERATE(s1ap_mme_map, s1ap_eNB_mme_data_s, entry, s1ap_eNB_compare_assoc_id);

inline int s1ap_eNB_compare_assoc_id(
    struct s1ap_eNB_mme_data_s *p1, struct s1ap_eNB_mme_data_s *p2)
{
    if (p1->assoc_id == -1) {
        if (p1->cnx_id < p2->cnx_id) {
            return -1;
        }
        if (p1->cnx_id > p2->cnx_id) {
            return 1;
        }
    } else {
        if (p1->assoc_id < p2->assoc_id) {
            return -1;
        }
        if (p1->assoc_id > p2->assoc_id) {
            return 1;
        }
    }

    /* Matching reference */
    return 0;
}

uint16_t s1ap_eNB_fetch_add_global_cnx_id(void)
{
    return ++s1ap_eNB_internal_data.global_cnx_id;
}

void s1ap_eNB_prepare_internal_data(void)
{
    memset(&s1ap_eNB_internal_data, 0, sizeof(s1ap_eNB_internal_data));
    STAILQ_INIT(&s1ap_eNB_internal_data.s1ap_eNB_instances_head);
}

void s1ap_eNB_insert_new_instance(s1ap_eNB_instance_t *new_instance_p)
{
    DevAssert(new_instance_p != NULL);

    STAILQ_INSERT_TAIL(&s1ap_eNB_internal_data.s1ap_eNB_instances_head,
                       new_instance_p, s1ap_eNB_entries);
}

inline struct s1ap_eNB_mme_data_s *s1ap_eNB_get_MME(
    s1ap_eNB_instance_t *instance_p,
    int32_t assoc_id, uint16_t cnx_id)
{
    struct s1ap_eNB_mme_data_s  temp;
    struct s1ap_eNB_mme_data_s *found;

    memset(&temp, 0, sizeof(struct s1ap_eNB_mme_data_s));

    temp.assoc_id = assoc_id;
    temp.cnx_id   = cnx_id;

    if (instance_p == NULL) {
        STAILQ_FOREACH(instance_p, &s1ap_eNB_internal_data.s1ap_eNB_instances_head,
                       s1ap_eNB_entries)
        {
            found = RB_FIND(s1ap_mme_map, &instance_p->s1ap_mme_head, &temp);
            if (found != NULL) {
                return found;
            }
        }
    } else {
        return RB_FIND(s1ap_mme_map, &instance_p->s1ap_mme_head, &temp);
    }

    return NULL;
}

s1ap_eNB_instance_t *s1ap_eNB_get_instance(uint8_t mod_id)
{
    s1ap_eNB_instance_t *temp = NULL;

    STAILQ_FOREACH(temp, &s1ap_eNB_internal_data.s1ap_eNB_instances_head,
                   s1ap_eNB_entries)
    {
        if (temp->mod_id == mod_id) {
            /* Matching occurence */
            return temp;
        }
    }

    return NULL;
}

// int s1ap_eNB_ue_capabilities(eNB_mme_desc_t         *eNB_desc_p,
//                              s1ap_ue_cap_info_ind_t *ue_cap_info_ind_p)
// {
//     struct s1ap_eNB_ue_context_s *ue_context_p;
//     UECapabilityInfoIndicationIEs_t *ue_cap_info_ind_ies_p;
// 
//     s1ap_message  message;
// 
//     uint8_t  *buffer;
//     uint32_t length;
//     int      ret = -1;
// 
//     DevAssert(ue_cap_info_ind_p != NULL);
//     DevAssert(eNB_desc_p != NULL);
// 
//     if ((ue_context_p = s1ap_eNB_get_ue_context(eNB_desc_p, ue_cap_info_ind_p->eNB_ue_s1ap_id)) == NULL)
//     {
//         /* The context for this eNB ue s1ap id doesn't exist in the map of eNB UEs */
//         S1AP_WARN("Failed to find ue context associated with eNB ue s1ap id: %u\n",
//                   ue_cap_info_ind_p->eNB_ue_s1ap_id);
//         return -1;
//     }
// 
//     /* UE capabilities message can occur either during an s1ap connected state
//      * or during initial attach (for example: NAS authentication).
//      */
//     if (!(ue_context_p->ue_state == S1AP_UE_CONNECTED ||
//         ue_context_p->ue_state == S1AP_UE_WAITING_CSR))
//     {
//         S1AP_WARN("You are attempting to send NAS data over non-connected "
//         "eNB ue s1ap id: %u, current state: %d\n",
//         ue_cap_info_ind_p->eNB_ue_s1ap_id, ue_context_p->ue_state);
//         return -1;
//     }
// 
//     /* Prepare the S1AP message to encode */
//     memset(&message, 0, sizeof(s1ap_message));
// 
//     message.direction = S1AP_PDU_PR_initiatingMessage;
//     message.procedureCode = ProcedureCode_id_UECapabilityInfoIndication;
// 
//     ue_cap_info_ind_ies_p = &message.msg.ueCapabilityInfoIndicationIEs;
// 
//     ue_cap_info_ind_ies_p->ueRadioCapability.buf = ue_cap_info_ind_p->ue_radio_cap.buffer;
//     ue_cap_info_ind_ies_p->ueRadioCapability.size = ue_cap_info_ind_p->ue_radio_cap.length;
// 
//     ue_cap_info_ind_ies_p->eNB_UE_S1AP_ID = ue_cap_info_ind_p->eNB_ue_s1ap_id;
//     ue_cap_info_ind_ies_p->mme_ue_s1ap_id = ue_context_p->mme_ue_s1ap_id;
// 
//     if (s1ap_eNB_encode_pdu(&message, &buffer, &length) < 0) {
//         /* Encode procedure has failed... */
//         S1AP_ERROR("Failed to encode UE capabilities indication\n");
//         return -1;
//     }
// 
//     /* UE associated signalling -> use the allocated stream */
//     if ((ret = sctp_send_msg(&ue_context_p->mme_ref->sctp_data, S1AP_SCTP_PPID,
//         ue_context_p->stream, buffer, length)) < 0)
//     {
//         S1AP_ERROR("[SCTP %d] Failed to send message over SCTP: %d\n",
//                    ue_context_p->mme_ref->sctp_data.assoc_id, ret);
//     }
// 
//     free(buffer);
//     return ret;
// }

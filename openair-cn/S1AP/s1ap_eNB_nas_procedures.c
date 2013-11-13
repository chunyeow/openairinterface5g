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

#include "assertions.h"
#include "conversions.h"

#include "intertask_interface.h"

#include "s1ap_common.h"
#include "s1ap_eNB_defs.h"

#include "s1ap_eNB_itti_messaging.h"

#include "s1ap_ies_defs.h"
#include "s1ap_eNB_encoder.h"
#include "s1ap_eNB_nnsf.h"
#include "s1ap_eNB_ue_context.h"
#include "s1ap_eNB_nas_procedures.h"
#include "s1ap_eNB_management_procedures.h"

int s1ap_eNB_handle_nas_first_req(
    instance_t instance, s1ap_nas_first_req_t *s1ap_nas_first_req_p)
{
    s1ap_eNB_instance_t          *instance_p;
    struct s1ap_eNB_mme_data_s   *mme_desc_p;
    struct s1ap_eNB_ue_context_s *ue_desc_p;

    s1ap_message message;

    S1ap_InitialUEMessageIEs_t *initial_ue_message_p;

    uint8_t  *buffer;
    uint32_t  length;

    DevAssert(s1ap_nas_first_req_p != NULL);

    /* Retrieve the S1AP eNB instance associated with Mod_id */
    instance_p = s1ap_eNB_get_instance(instance);
    DevAssert(instance_p != NULL);

    memset(&message, 0, sizeof(s1ap_message));

    message.direction     = S1AP_PDU_PR_initiatingMessage;
    message.procedureCode = S1ap_ProcedureCode_id_initialUEMessage;

    initial_ue_message_p = &message.msg.s1ap_InitialUEMessageIEs;

    /* Select the MME corresponding to the provided GUMMEI.
     * If no MME corresponds to the GUMMEI, the function selects the MME with the
     * highest capacity.
     * In case eNB has no MME associated, the eNB should inform RRC and discard
     * this request.
     */
    if (s1ap_nas_first_req_p->ue_identity.present == GUMMEI_PROVIDED) {
        mme_desc_p = s1ap_eNB_nnsf_select_mme_by_gummei(
            instance_p,
            s1ap_nas_first_req_p->establishment_cause,
            s1ap_nas_first_req_p->ue_identity.identity.gummei);
    } else {
        mme_desc_p = s1ap_eNB_nnsf_select_mme_by_mme_code(
            instance_p,
            s1ap_nas_first_req_p->establishment_cause,
            s1ap_nas_first_req_p->ue_identity.identity.s_tmsi.mme_code);
    }
    if (mme_desc_p == NULL) {
        S1AP_WARN("No MME is associated to the eNB\n");
        // TODO: Inform RRC
        return -1;
    }

    /* The eNB should allocate a unique eNB UE S1AP ID for this UE. The value
     * will be used for the duration of the connectivity.
     */
    ue_desc_p = s1ap_eNB_allocate_new_UE_context();
    DevAssert(ue_desc_p != NULL);

    /* Keep a reference to the selected MME */
    ue_desc_p->mme_ref = mme_desc_p;
    ue_desc_p->rnti    = s1ap_nas_first_req_p->rnti;

    do {
        struct s1ap_eNB_ue_context_s *collision_p;

        /* Peek a random value for the eNB_ue_s1ap_id */
        ue_desc_p->eNB_ue_s1ap_id = (random() + random()) & 0x00ffffff;
        if ((collision_p = RB_INSERT(s1ap_ue_map, &instance_p->s1ap_ue_head, ue_desc_p))
                == NULL)
        {
            S1AP_DEBUG("Found usable eNB_ue_s1ap_id: 0x%06x\n", ue_desc_p->eNB_ue_s1ap_id);
            /* Break the loop as the id is not already used by another UE */
            break;
        }
    } while(1);

    initial_ue_message_p->eNB_UE_S1AP_ID = ue_desc_p->eNB_ue_s1ap_id;
    /* Prepare the NAS PDU */
    initial_ue_message_p->nas_pdu.buf  = s1ap_nas_first_req_p->nas_pdu.buffer;
    initial_ue_message_p->nas_pdu.size = s1ap_nas_first_req_p->nas_pdu.length;

    /* Set the establishment cause according to those provided by RRC */
    DevCheck(s1ap_nas_first_req_p->establishment_cause < RRC_CAUSE_LAST,
             s1ap_nas_first_req_p->establishment_cause, RRC_CAUSE_LAST, 0);
    initial_ue_message_p->rrC_Establishment_Cause = s1ap_nas_first_req_p->establishment_cause;

    if (s1ap_nas_first_req_p->ue_identity.present == S_TMSI_PROVIDED) {
        initial_ue_message_p->presenceMask |= S1AP_INITIALUEMESSAGEIES_S_TMSI_PRESENT;

        MME_CODE_TO_OCTET_STRING(s1ap_nas_first_req_p->ue_identity.identity.s_tmsi.mme_code,
                                 &initial_ue_message_p->s_tmsi.mMEC);
        M_TMSI_TO_OCTET_STRING(s1ap_nas_first_req_p->ue_identity.identity.s_tmsi.m_tmsi,
                               &initial_ue_message_p->s_tmsi.m_TMSI);
    } else {
        initial_ue_message_p->presenceMask |= S1AP_INITIALUEMESSAGEIES_GUMMEI_ID_PRESENT;

        MCC_MNC_TO_PLMNID(s1ap_nas_first_req_p->ue_identity.identity.gummei.mcc,
                          s1ap_nas_first_req_p->ue_identity.identity.gummei.mnc,
                          &initial_ue_message_p->gummei_id.pLMN_Identity);
        MME_GID_TO_OCTET_STRING(s1ap_nas_first_req_p->ue_identity.identity.gummei.mme_group_id,
                                &initial_ue_message_p->gummei_id.mME_Group_ID);
        MME_CODE_TO_OCTET_STRING(s1ap_nas_first_req_p->ue_identity.identity.gummei.mme_code,
                                 &initial_ue_message_p->gummei_id.mME_Code);
    }

    /* Assuming TAI is the TAI from the cell */
    INT16_TO_OCTET_STRING(instance_p->tac, &initial_ue_message_p->tai.tAC);
    MCC_MNC_TO_PLMNID(instance_p->mcc, instance_p->mnc,
                      &initial_ue_message_p->tai.pLMNidentity);

    /* Set the EUTRAN CGI
     * The cell identity is defined on 28 bits but as we use macro enb id,
     * we have to pad.
     */
    MACRO_ENB_ID_TO_CELL_IDENTITY(instance_p->eNB_id,
                                  &initial_ue_message_p->eutran_cgi.cell_ID);
    MCC_MNC_TO_TBCD(instance_p->mcc, instance_p->mnc,
                    &initial_ue_message_p->eutran_cgi.pLMNidentity);

    if (s1ap_eNB_encode_pdu(&message, &buffer, &length) < 0) {
        /* Failed to encode message */
        DevMessage("Failed to encode initial UE message\n");
    }

    /* Update the current S1AP UE state */
    ue_desc_p->ue_state = S1AP_UE_WAITING_CSR;

    /* Assign a stream for this UE */
    mme_desc_p->nextstream %= mme_desc_p->out_streams;
    ue_desc_p->stream = ++mme_desc_p->nextstream;

    /* Send encoded message over sctp */
    s1ap_eNB_itti_send_sctp_data_req(mme_desc_p->assoc_id, buffer, length, ue_desc_p->stream);

    return 0;
}

int s1ap_eNB_handle_nas_downlink(uint32_t               assoc_id,
                                 uint32_t               stream,
                                 struct s1ap_message_s *message_p)
{
    S1ap_DownlinkNASTransportIEs_t *downlink_NAS_transport_p;

    s1ap_eNB_mme_data_t   *mme_desc_p;
    s1ap_eNB_ue_context_t *ue_desc_p;
    s1ap_eNB_instance_t   *s1ap_eNB_instance;

    DevAssert(message_p != NULL);

    downlink_NAS_transport_p = &message_p->msg.s1ap_DownlinkNASTransportIEs;

    /* UE-related procedure -> stream != 0 */
    if (stream == 0) {
        S1AP_ERROR("[SCTP %d] Received UE-related procedure on stream == 0\n",
                   assoc_id);
        return -1;
    }

    if ((mme_desc_p = s1ap_eNB_get_MME(NULL, assoc_id, 0)) == NULL) {
        S1AP_ERROR("[SCTP %d] Received initial context setup request for non "
                   "existing MME context\n", assoc_id);
        return -1;
    }

    s1ap_eNB_instance = mme_desc_p->s1ap_eNB_instance;

    if ((ue_desc_p = s1ap_eNB_get_ue_context(s1ap_eNB_instance,
         downlink_NAS_transport_p->eNB_UE_S1AP_ID)) == NULL)
    {
        S1AP_ERROR("[SCTP %d] Received initial context setup request for non "
                   "existing UE context\n", assoc_id);
        return -1;
    }

    /* Is it the first outcome of the MME for this UE ? If so store the mme
     * UE s1ap id.
     */
    if (ue_desc_p->mme_ue_s1ap_id == 0) {
        ue_desc_p->mme_ue_s1ap_id = downlink_NAS_transport_p->mme_ue_s1ap_id;
    } else {
        /* We already have a mme ue s1ap id check the received is the same */
        if (ue_desc_p->mme_ue_s1ap_id != downlink_NAS_transport_p->mme_ue_s1ap_id) {
            S1AP_ERROR("[SCTP %d] Mismatch is MME UE S1AP ID (0x%08x != 0x%08x)\n",
                       downlink_NAS_transport_p->mme_ue_s1ap_id,
                       ue_desc_p->mme_ue_s1ap_id,
                       assoc_id);
        }
    }

    /* Forward the NAS PDU to RRC */
    s1ap_eNB_itti_send_nas_downlink_ind(s1ap_eNB_instance->instance,
                                        downlink_NAS_transport_p->nas_pdu.buf,
                                        downlink_NAS_transport_p->nas_pdu.size);

    return 0;
}

int s1ap_eNB_nas_uplink(instance_t instance, s1ap_uplink_nas_t *s1ap_uplink_nas_p)
{
    struct s1ap_eNB_ue_context_s *ue_context_p;
    s1ap_eNB_instance_t          *s1ap_eNB_instance_p;
    S1ap_UplinkNASTransportIEs_t *uplink_NAS_transport_p;

    s1ap_message  message;

    uint8_t  *buffer;
    uint32_t length;

    DevAssert(s1ap_uplink_nas_p != NULL);

    /* Retrieve the S1AP eNB instance associated with Mod_id */
    s1ap_eNB_instance_p = s1ap_eNB_get_instance(instance);
    DevAssert(s1ap_eNB_instance_p != NULL);

    if ((ue_context_p = s1ap_eNB_get_ue_context(s1ap_eNB_instance_p, s1ap_uplink_nas_p->eNB_ue_s1ap_id)) == NULL)
    {
        /* The context for this eNB ue s1ap id doesn't exist in the map of eNB UEs */
        S1AP_WARN("Failed to find ue context associated with eNB ue s1ap id: %u\n",
                  s1ap_uplink_nas_p->eNB_ue_s1ap_id);
        return -1;
    }

    /* Uplink NAS transport can occur either during an s1ap connected state
     * or during initial attach (for example: NAS authentication).
     */
    if (!(ue_context_p->ue_state == S1AP_UE_CONNECTED ||
        ue_context_p->ue_state == S1AP_UE_WAITING_CSR))
    {
        S1AP_WARN("You are attempting to send NAS data over non-connected "
                  "eNB ue s1ap id: %u, current state: %d\n",
                  s1ap_uplink_nas_p->eNB_ue_s1ap_id, ue_context_p->ue_state);
        return -1;
    }

    /* Prepare the S1AP message to encode */
    memset(&message, 0, sizeof(s1ap_message));

    message.direction     = S1AP_PDU_PR_initiatingMessage;
    message.procedureCode = S1ap_ProcedureCode_id_uplinkNASTransport;

    uplink_NAS_transport_p = &message.msg.s1ap_UplinkNASTransportIEs;

    uplink_NAS_transport_p->mme_ue_s1ap_id = ue_context_p->mme_ue_s1ap_id;
    uplink_NAS_transport_p->eNB_UE_S1AP_ID = ue_context_p->eNB_ue_s1ap_id;

    uplink_NAS_transport_p->nas_pdu.buf  = s1ap_uplink_nas_p->nas_pdu.buffer;
    uplink_NAS_transport_p->nas_pdu.size = s1ap_uplink_nas_p->nas_pdu.length;

    MCC_MNC_TO_PLMNID(s1ap_eNB_instance_p->mcc, s1ap_eNB_instance_p->mnc,
                      &uplink_NAS_transport_p->eutran_cgi.pLMNidentity);
    MACRO_ENB_ID_TO_CELL_IDENTITY(s1ap_eNB_instance_p->eNB_id,
                                  &uplink_NAS_transport_p->eutran_cgi.cell_ID);

    /* MCC/MNC should be repeated in TAI and EUTRAN CGI */
    MCC_MNC_TO_PLMNID(s1ap_eNB_instance_p->mcc, s1ap_eNB_instance_p->mnc,
                      &uplink_NAS_transport_p->tai.pLMNidentity);
    TAC_TO_ASN1(s1ap_eNB_instance_p->tac, &uplink_NAS_transport_p->tai.tAC);

    if (s1ap_eNB_encode_pdu(&message, &buffer, &length) < 0) {
        S1AP_ERROR("Failed to encode uplink NAS transport\n");
        /* Encode procedure has failed... */
        return -1;
    }

    /* UE associated signalling -> use the allocated stream */
    s1ap_eNB_itti_send_sctp_data_req(ue_context_p->mme_ref->assoc_id, buffer,
                                     length, ue_context_p->stream);

    return 0;
}

int s1ap_eNB_initial_ctxt_resp(
    instance_t instance, s1ap_initial_context_setup_resp_t *initial_ctxt_resp_p)
{
    s1ap_eNB_instance_t          *s1ap_eNB_instance_p;
    struct s1ap_eNB_ue_context_s *ue_context_p;

    S1ap_InitialContextSetupResponseIEs_t *initial_ies_p;

    s1ap_message  message;

    uint8_t  *buffer;
    uint32_t length;
    int      ret = -1;
    int      i;

    DevAssert(initial_ctxt_resp_p != NULL);
    DevAssert(s1ap_eNB_instance_p != NULL);

    if ((ue_context_p = s1ap_eNB_get_ue_context(s1ap_eNB_instance_p,
        initial_ctxt_resp_p->eNB_ue_s1ap_id)) == NULL)
    {
        /* The context for this eNB ue s1ap id doesn't exist in the map of eNB UEs */
        S1AP_WARN("Failed to find ue context associated with eNB ue s1ap id: %u\n",
                  initial_ctxt_resp_p->eNB_ue_s1ap_id);
        return -1;
    }

    /* Uplink NAS transport can occur either during an s1ap connected state
     * or during initial attach (for example: NAS authentication).
     */
    if (!(ue_context_p->ue_state == S1AP_UE_CONNECTED ||
        ue_context_p->ue_state == S1AP_UE_WAITING_CSR))
    {
        S1AP_WARN("You are attempting to send NAS data over non-connected "
                  "eNB ue s1ap id: %u, current state: %d\n",
                  initial_ctxt_resp_p->eNB_ue_s1ap_id, ue_context_p->ue_state);
        return -1;
    }

    /* Prepare the S1AP message to encode */
    memset(&message, 0, sizeof(s1ap_message));

    message.direction     = S1AP_PDU_PR_successfulOutcome;
    message.procedureCode = S1ap_ProcedureCode_id_InitialContextSetup;

    initial_ies_p = &message.msg.s1ap_InitialContextSetupResponseIEs;

    initial_ies_p->eNB_UE_S1AP_ID = initial_ctxt_resp_p->eNB_ue_s1ap_id;
    initial_ies_p->mme_ue_s1ap_id = ue_context_p->mme_ue_s1ap_id;

    for (i = 0; i < initial_ctxt_resp_p->nb_of_e_rabs; i++)
    {
        S1ap_E_RABSetupItemCtxtSURes_t *new_item;

        new_item = calloc(1, sizeof(S1ap_E_RABSetupItemCtxtSURes_t));

        new_item->e_RAB_ID = initial_ctxt_resp_p->e_rabs[i].e_rab_id;
        GTP_TEID_TO_ASN1(initial_ctxt_resp_p->e_rabs[i].gtp_teid, &new_item->gTP_TEID);
        new_item->transportLayerAddress.buf = initial_ctxt_resp_p->e_rabs[i].eNB_addr.buffer;
        new_item->transportLayerAddress.size = initial_ctxt_resp_p->e_rabs[i].eNB_addr.length;
        new_item->transportLayerAddress.bits_unused = 0;

        ASN_SEQUENCE_ADD(&initial_ies_p->e_RABSetupListCtxtSURes.s1ap_E_RABSetupItemCtxtSURes,
                         new_item);
    }

    if (s1ap_eNB_encode_pdu(&message, &buffer, &length) < 0) {
        S1AP_ERROR("Failed to encode uplink NAS transport\n");
        /* Encode procedure has failed... */
        return -1;
    }

    /* UE associated signalling -> use the allocated stream */
    s1ap_eNB_itti_send_sctp_data_req(ue_context_p->mme_ref->assoc_id, buffer,
                                     length, ue_context_p->stream);

    return ret;
}

int s1ap_eNB_ue_capabilities(instance_t instance,
                             s1ap_ue_cap_info_ind_t *ue_cap_info_ind_p)
{
    s1ap_eNB_instance_t          *s1ap_eNB_instance_p;
    struct s1ap_eNB_ue_context_s *ue_context_p;

    S1ap_UECapabilityInfoIndicationIEs_t *ue_cap_info_ind_ies_p;

    s1ap_message  message;

    uint8_t  *buffer;
    uint32_t length;
    int      ret = -1;

    DevAssert(ue_cap_info_ind_p != NULL);
    DevAssert(s1ap_eNB_instance_p != NULL);

    if ((ue_context_p = s1ap_eNB_get_ue_context(s1ap_eNB_instance_p,
        ue_cap_info_ind_p->eNB_ue_s1ap_id)) == NULL)
    {
        /* The context for this eNB ue s1ap id doesn't exist in the map of eNB UEs */
        S1AP_WARN("Failed to find ue context associated with eNB ue s1ap id: %u\n",
                  ue_cap_info_ind_p->eNB_ue_s1ap_id);
        return -1;
    }

    /* UE capabilities message can occur either during an s1ap connected state
     * or during initial attach (for example: NAS authentication).
     */
    if (!(ue_context_p->ue_state == S1AP_UE_CONNECTED ||
        ue_context_p->ue_state == S1AP_UE_WAITING_CSR))
    {
        S1AP_WARN("You are attempting to send NAS data over non-connected "
        "eNB ue s1ap id: %u, current state: %d\n",
        ue_cap_info_ind_p->eNB_ue_s1ap_id, ue_context_p->ue_state);
        return -1;
    }

    /* Prepare the S1AP message to encode */
    memset(&message, 0, sizeof(s1ap_message));

    message.direction     = S1AP_PDU_PR_initiatingMessage;
    message.procedureCode = S1ap_ProcedureCode_id_UECapabilityInfoIndication;

    ue_cap_info_ind_ies_p = &message.msg.s1ap_UECapabilityInfoIndicationIEs;

    ue_cap_info_ind_ies_p->ueRadioCapability.buf = ue_cap_info_ind_p->ue_radio_cap.buffer;
    ue_cap_info_ind_ies_p->ueRadioCapability.size = ue_cap_info_ind_p->ue_radio_cap.length;

    ue_cap_info_ind_ies_p->eNB_UE_S1AP_ID = ue_cap_info_ind_p->eNB_ue_s1ap_id;
    ue_cap_info_ind_ies_p->mme_ue_s1ap_id = ue_context_p->mme_ue_s1ap_id;

    if (s1ap_eNB_encode_pdu(&message, &buffer, &length) < 0) {
        /* Encode procedure has failed... */
        S1AP_ERROR("Failed to encode UE capabilities indication\n");
        return -1;
    }

    /* UE associated signalling -> use the allocated stream */
    s1ap_eNB_itti_send_sctp_data_req(ue_context_p->mme_ref->assoc_id, buffer,
                                     length, ue_context_p->stream);

    return ret;
}

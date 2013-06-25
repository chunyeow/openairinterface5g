/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2012 Eurecom

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

#include <stdint.h>

#include "s1ap_common.h"
#include "s1ap_ies_defs.h"
#include "s1ap_eNB.h"
#include "s1ap_eNB_encoder.h"

#include "sctp_primitives_client.h"

int s1ap_eNB_encode_initial_ue_message(InitialUEMessageIEs_t  *initialUEmessageIEs_p,
                                       uint8_t             **buffer,
                                       uint32_t             *length) {
    InitialUEMessage_t  initialUEMessage;
    InitialUEMessage_t *initialUEMessage_p = &initialUEMessage;

    memset((void *)initialUEMessage_p, 0, sizeof(InitialUEMessage_t));

    if (s1ap_encode_initialuemessageies(initialUEMessage_p, initialUEmessageIEs_p) < 0)
        return -1;

    return s1ap_generate_initiating_message(buffer,
        length,
        ProcedureCode_id_initialUEMessage,
        Criticality_reject,
        &asn_DEF_InitialUEMessage,
        initialUEMessage_p);
}

int s1ap_eNB_encode_s1_setup_request(S1SetupRequestIEs_t  *s1SetupRequestIEs,
                                     uint8_t           **buffer,
                                     uint32_t           *length) {
    S1SetupRequest_t  s1SetupRequest;
    S1SetupRequest_t *s1SetupRequest_p = &s1SetupRequest;

    memset((void *)s1SetupRequest_p, 0, sizeof(S1SetupRequest_t));

    if (s1ap_encode_s1setuprequesties(s1SetupRequest_p, s1SetupRequestIEs) < 0)
        return -1;

    return s1ap_generate_initiating_message(buffer,
        length,
        ProcedureCode_id_S1Setup,
        Criticality_reject,
        &asn_DEF_S1SetupRequest,
        s1SetupRequest_p);
}

int s1ap_eNB_generate_initial_ue_message(struct s1ap_eNB_UE_description_s *ue_ref,
                                         uint8_t                          *nas_pdu,
                                         uint32_t                          nas_len) {
    InitialUEMessageIEs_t  initialUEmessageIEs;
    InitialUEMessageIEs_t *initialUEmessageIEs_p = &initialUEmessageIEs;
    uint8_t  *buffer;
    uint32_t  len;

    char tac[] = { 0x00, 0x01 };
    uint8_t id[] = { 0x03, 0x56, 0xf0, 0xd8 };
    char identity[] = { 0x02, 0x08, 0x34 };

    memset((void *)initialUEmessageIEs_p, 0, sizeof(InitialUEMessageIEs_t));

    initialUEmessageIEs.eNB_UE_S1AP_ID = ue_ref->eNB_UE_s1ap_id;
    initialUEmessageIEs.nas_pdu.buf = nas_pdu;
    initialUEmessageIEs.nas_pdu.size = nas_len;

    initialUEmessageIEs.tai.tAC.buf = (uint8_t*)tac;
    initialUEmessageIEs.tai.tAC.size = 2;
    initialUEmessageIEs.tai.pLMNidentity.buf = (uint8_t*)identity;
    initialUEmessageIEs.tai.pLMNidentity.size = 3;
    initialUEmessageIEs.eutran_cgi.pLMNidentity.buf = (uint8_t*)identity;
    initialUEmessageIEs.eutran_cgi.pLMNidentity.size = 3;
    initialUEmessageIEs.eutran_cgi.cell_ID.buf = (uint8_t*)id;
    initialUEmessageIEs.eutran_cgi.cell_ID.size = 4;
    initialUEmessageIEs.eutran_cgi.cell_ID.bits_unused = 4;

    initialUEmessageIEs.rrC_Establishment_Cause = RRC_Establishment_Cause_mo_Data;

    if (s1ap_eNB_encode_initial_ue_message(initialUEmessageIEs_p, &buffer, &len) < 0) {
        if (buffer != NULL) free(buffer);
        return -1;
    }
    /* Send encoded message over sctp */
    return sctp_send_msg(ue_ref->eNB->assocId, ue_ref->stream_send, buffer, len);
}

int s1ap_eNB_encode_initial_context_setup_response(InitialContextSetupResponseIEs_t  *sptr,
                                                   uint8_t             **buffer,
                                                   uint32_t             *length) {
    InitialContextSetupResponse_t  initial;
    InitialContextSetupResponse_t *initial_p = &initial;

    memset((void *)initial_p, 0, sizeof(InitialContextSetupResponse_t));

    if (s1ap_encode_initialcontextsetupresponseies(initial_p, sptr) < 0)
        return -1;

    return s1ap_generate_successfull_outcome(buffer,
        length,
        ProcedureCode_id_InitialContextSetup,
        Criticality_reject,
        &asn_DEF_InitialContextSetupResponse,
        initial_p);
}

int s1ap_eNB_generate_uplink_nas_transport(struct s1ap_eNB_UE_description_s *ue_ref,
                                           uint8_t                          *nas_pdu,
                                           uint32_t                          nas_len) {

    return 0;
}

int s1ap_eNB_generate_s1_setup_request(struct s1ap_eNB_description_s* eNB_ref) {

    SupportedTAs_Item_t ta;
    PLMNidentity_t plmnIdentity;
    S1SetupRequestIEs_t s1SetupRequest;

    char tac[] = { 0x00, 0x01 };
    char identity[] = { 0x02, 0x08, 0x34 };
    char eNBname[150];
    int eNBnameLength = 0;
    uint8_t *buffer;
    uint32_t len;

    if (eNB_ref == NULL) return -1;

    memset((void *)&s1SetupRequest, 0, sizeof(S1SetupRequestIEs_t));
    s1SetupRequest.global_ENB_ID.eNB_ID.present = ENB_ID_PR_macroENB_ID;
    s1SetupRequest.global_ENB_ID.eNB_ID.choice.macroENB_ID.buf = calloc(1,3);
    s1SetupRequest.global_ENB_ID.eNB_ID.choice.macroENB_ID.buf[0] = eNB_ref->eNB_id;
    s1SetupRequest.global_ENB_ID.eNB_ID.choice.macroENB_ID.size = 3;
    s1SetupRequest.global_ENB_ID.eNB_ID.choice.macroENB_ID.bits_unused = 4;
    OCTET_STRING_fromBuf(&s1SetupRequest.global_ENB_ID.pLMNidentity, identity, 3);

    eNBnameLength = sprintf(eNBname, "eNB %d Eurecom", eNB_ref->eNB_id);
    if (eNBnameLength > 0) {
        s1SetupRequest.presenceMask |= S1SETUPREQUESTIES_ENBNAME_PRESENT;
        OCTET_STRING_fromBuf(&s1SetupRequest.eNBname, eNBname, eNBnameLength);
    }

    memset((void *)&ta, 0, sizeof(SupportedTAs_Item_t));
    memset((void *)&plmnIdentity, 0, sizeof(PLMNidentity_t));
    OCTET_STRING_fromBuf(&ta.tAC, tac, 2);
    OCTET_STRING_fromBuf(&plmnIdentity, identity, 3);
    ASN_SEQUENCE_ADD(&ta.broadcastPLMNs, &plmnIdentity);
    ASN_SEQUENCE_ADD(&s1SetupRequest.supportedTAs, &ta);

    s1SetupRequest.defaultPagingDRX = PagingDRX_v64;

    if (s1ap_eNB_encode_s1_setup_request(&s1SetupRequest, &buffer, &len) < 0)
        return -1;
    eNB_ref->state |= S1AP_ENB_STATE_WAITING;
    return sctp_send_msg(eNB_ref->assocId, 0, buffer, len);
}

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

/*! \file s1ap_mme_decoder.c
 * \brief s1ap decode procedures for MME
 * \author Sebastien ROUX <sebastien.roux@eurecom.fr>
 * \date 2012
 * \version 0.1
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#include "s1ap_common.h"
#include "s1ap_ies_defs.h"
#include "s1ap_mme_decoder.h"
#include "s1ap_mme_handlers.h"

#if !defined(MME_CLIENT_TEST)
# include "intertask_interface.h"
#endif

static int s1ap_mme_decode_initiating(s1ap_message *message, InitiatingMessage_t *initiating_p) {

    assert(initiating_p != NULL);
    message->procedureCode = initiating_p->procedureCode;
    switch(initiating_p->procedureCode) {
        case ProcedureCode_id_uplinkNASTransport:
            return s1ap_decode_uplinknastransporties(&message->msg.uplinkNASTransportIEs, &initiating_p->value);
        case ProcedureCode_id_S1Setup:
            return s1ap_decode_s1setuprequesties(&message->msg.s1SetupRequestIEs, &initiating_p->value);
        case ProcedureCode_id_initialUEMessage:
            return s1ap_decode_initialuemessageies(&message->msg.initialUEMessageIEs, &initiating_p->value);
        case ProcedureCode_id_UEContextReleaseRequest:
            return s1ap_decode_uecontextreleaserequesties(&message->msg.ueContextReleaseRequestIEs, &initiating_p->value);
        default:
            S1AP_DEBUG("Unknown procedure ID (%d) for initiating message", (int)initiating_p->procedureCode);
            break;
    }
    return -1;
}

static int s1ap_mme_decode_successfull_outcome(s1ap_message *message, SuccessfulOutcome_t *successfullOutcome_p) {

    assert(successfullOutcome_p != NULL);
    message->procedureCode = successfullOutcome_p->procedureCode;
    switch(successfullOutcome_p->procedureCode) {
        case ProcedureCode_id_InitialContextSetup:
            return s1ap_decode_initialcontextsetupresponseies(&message->msg.initialContextSetupResponseIEs, &successfullOutcome_p->value);
        case ProcedureCode_id_UEContextRelease:
            return s1ap_decode_uecontextreleasecompleteies(&message->msg.ueContextReleaseCompleteIEs, &successfullOutcome_p->value);
        default:
            S1AP_DEBUG("Unknown procedure ID (%d) for successfull outcome message", (int)successfullOutcome_p->procedureCode);
            break;
    }
    return -1;
}

static int s1ap_mme_decode_unsuccessfull_outcome(s1ap_message *message, UnsuccessfulOutcome_t *unSuccessfulOutcome_p) {

    assert(unSuccessfulOutcome_p != NULL);
    message->procedureCode = unSuccessfulOutcome_p->procedureCode;
    switch(unSuccessfulOutcome_p->procedureCode) {
        case ProcedureCode_id_InitialContextSetup:
            return s1ap_decode_initialcontextsetupfailureies(&message->msg.initialContextSetupFailureIEs, &unSuccessfulOutcome_p->value);
        default:
            S1AP_DEBUG("Unknown procedure ID (%d) for unsuccessfull outcome message", (int)unSuccessfulOutcome_p->procedureCode);
            break;
    }
    return -1;
}

int s1ap_mme_decode_pdu(s1ap_message *message, uint8_t *buffer, uint32_t len) {
    S1AP_PDU_t  pdu;
    S1AP_PDU_t *pdu_p = &pdu;
    asn_dec_rval_t dec_ret;

    assert(buffer != NULL);

    memset((void *)pdu_p, 0, sizeof(S1AP_PDU_t));

    dec_ret = aper_decode(NULL,
                          &asn_DEF_S1AP_PDU,
                          (void**)&pdu_p,
                          buffer,
                          len,
                          0,
                          0);

    if (dec_ret.code != RC_OK)
        return -1;

    message->direction = pdu_p->present;

    switch(pdu_p->present) {
        case S1AP_PDU_PR_initiatingMessage:
            return s1ap_mme_decode_initiating(message, &pdu_p->choice.initiatingMessage);
        case S1AP_PDU_PR_successfulOutcome:
            return s1ap_mme_decode_successfull_outcome(message, &pdu_p->choice.successfulOutcome);
        case S1AP_PDU_PR_unsuccessfulOutcome:
            return s1ap_mme_decode_unsuccessfull_outcome(message, &pdu_p->choice.unsuccessfulOutcome);
        default:
            S1AP_DEBUG("Unknown message outcome (%d) or not implemented", (int)pdu_p->present);
            break;
    }
    return -1;
}

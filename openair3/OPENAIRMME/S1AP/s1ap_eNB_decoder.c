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

#include <stdio.h>

#include "s1ap_common.h"
#include "s1ap_ies_defs.h"
#include "s1ap_eNB_decoder.h"

static int s1ap_eNB_decode_initiating_message(s1ap_message *message, InitiatingMessage_t *initiating_p) {
    assert(initiating_p != NULL);
    message->procedureCode = initiating_p->procedureCode;
    switch(initiating_p->procedureCode) {
        case ProcedureCode_id_downlinkNASTransport:
            return s1ap_decode_downlinknastransporties(&message->msg.downlinkNASTransportIEs, &initiating_p->value);
        case ProcedureCode_id_InitialContextSetup:
            return s1ap_decode_initialcontextsetuprequesties(&message->msg.initialContextSetupRequestIEs, &initiating_p->value);
        default:
            S1AP_DEBUG("Unknown procedure ID (%d) for initiating message\n", (int)initiating_p->procedureCode);
            break;
    }
    return -1;
}

static int s1ap_eNB_decode_successful_outcome(s1ap_message *message, SuccessfulOutcome_t *successfullOutcome_p) {
    assert(successfullOutcome_p != NULL);
    message->procedureCode = successfullOutcome_p->procedureCode;
    switch(successfullOutcome_p->procedureCode) {
        case ProcedureCode_id_S1Setup:
            return s1ap_decode_s1setupresponseies(&message->msg.s1SetupResponseIEs, &successfullOutcome_p->value);
        default:
            S1AP_DEBUG("Unknown procedure ID (%d) for successfull outcome message\n", (int)successfullOutcome_p->procedureCode);
            break;
    }
    return -1;
}

static int s1ap_eNB_decode_unsuccessful_outcome(s1ap_message *message, UnsuccessfulOutcome_t *unSuccessfullOutcome_p) {
    assert(unSuccessfullOutcome_p != NULL);
    message->procedureCode = unSuccessfullOutcome_p->procedureCode;
    switch(unSuccessfullOutcome_p->procedureCode) {
//         case ProcedureCode_id_S1Setup:
//             return decodeS1SetupFailure(message, &unSuccessfullOutcome_p->value);
        default:
            S1AP_DEBUG("Unknown procedure ID (%d) for unsuccessfull outcome message\n", (int)unSuccessfullOutcome_p->procedureCode);
            break;
    }
    return -1;
}

int s1ap_eNB_decode_pdu(s1ap_message *message, uint8_t *buffer, uint32_t length) {
    S1AP_PDU_t  pdu;
    S1AP_PDU_t *pdu_p = &pdu;
    asn_dec_rval_t dec_ret;

    assert(buffer != NULL);

    memset((void *)pdu_p, 0, sizeof(S1AP_PDU_t));

    dec_ret = aper_decode(NULL,
                          &asn_DEF_S1AP_PDU,
                          (void**)&pdu_p,
                          buffer,
                          length,
                          0,
                          0);

    if (dec_ret.code != RC_OK)
        return -1;

    message->direction = pdu_p->present;

    switch(pdu_p->present) {
        case S1AP_PDU_PR_initiatingMessage:
            return s1ap_eNB_decode_initiating_message(message, &pdu_p->choice.initiatingMessage);
        case S1AP_PDU_PR_successfulOutcome:
            return s1ap_eNB_decode_successful_outcome(message, &pdu_p->choice.successfulOutcome);
        case S1AP_PDU_PR_unsuccessfulOutcome:
            return s1ap_eNB_decode_unsuccessful_outcome(message, &pdu_p->choice.unsuccessfulOutcome);
        default:
            S1AP_DEBUG("Unknown presence (%d) or not implemented\n", (int)pdu_p->present);
            break;
    }
    return -1;
}

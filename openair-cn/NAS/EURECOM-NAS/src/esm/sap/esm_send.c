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
/*****************************************************************************
Source      esm_send.c

Version     0.1

Date        2013/02/11

Product     NAS stack

Subsystem   EPS Session Management

Author      Frederic Maurel

Description Defines functions executed at the ESM Service Access
        Point to send EPS Session Management messages to the
        EPS Mobility Management sublayer.

*****************************************************************************/

#include "esm_send.h"
#include "commonDef.h"
#include "nas_log.h"

#ifdef NAS_UE
#include "networkDef.h"
#endif

#include "esm_msgDef.h"
#include "esm_cause.h"

#include <string.h> // strlen

/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/*
 * --------------------------------------------------------------------------
 * Functions executed by both the UE and the MME to send ESM messages
 * --------------------------------------------------------------------------
 */
/****************************************************************************
 **                                                                        **
 ** Name:    esm_send_status()                                         **
 **                                                                        **
 ** Description: Builds ESM status message                                 **
 **                                                                        **
 **      The ESM status message is sent by the network or the UE   **
 **      to pass information on the status of the indicated EPS    **
 **      bearer context and report certain error conditions.       **
 **                                                                        **
 ** Inputs:  pti:       Procedure transaction identity             **
 **      ebi:       EPS bearer identity                        **
 **      esm_cause: ESM cause code                             **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     msg:       The ESM message to be sent                 **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int esm_send_status(int pti, int ebi, esm_status_msg *msg, int esm_cause)
{
    LOG_FUNC_IN;

    /* Mandatory - ESM message header */
    msg->protocoldiscriminator = EPS_SESSION_MANAGEMENT_MESSAGE;
    msg->epsbeareridentity = ebi;
    msg->messagetype = ESM_STATUS;
    msg->proceduretransactionidentity = pti;

    /* Mandatory - ESM cause code */
    msg->esmcause = esm_cause;

    LOG_TRACE(WARNING, "ESM-SAP   - Send ESM Status message (pti=%d, ebi=%d)",
              msg->proceduretransactionidentity, msg->epsbeareridentity);

    LOG_FUNC_RETURN(RETURNok);
}

/*
 * --------------------------------------------------------------------------
 * Functions executed by the UE to send ESM message to the network
 * --------------------------------------------------------------------------
 */
#ifdef NAS_UE
/****************************************************************************
 **                                                                        **
 ** Name:    esm_send_pdn_connectivity_request()                       **
 **                                                                        **
 ** Description: Builds PDN Connectivity Request message                   **
 **                                                                        **
 **      The PDN connectivity request message is sent by the UE to **
 **      the network to initiate establishment of a PDN connection.**
 **                                                                        **
 ** Inputs:  pti:       Procedure transaction identity assigned to **
 **             the PDN connection                         **
 **      is_emergency:  Indicates whether the PDN connectivity is  **
 **             requested for emergency bearer services    **
 **      pdn_type:  PDN address type                           **
 **      apn:       Access Point Name of the PDN to connect to **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     msg:       The ESM message to be sent                 **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int esm_send_pdn_connectivity_request(int pti, int is_emergency, int pdn_type,
                                      const char *apn,
                                      pdn_connectivity_request_msg *msg)
{
    LOG_FUNC_IN;

    /* Mandatory - ESM message header */
    msg->protocoldiscriminator = EPS_SESSION_MANAGEMENT_MESSAGE;
    msg->epsbeareridentity = EPS_BEARER_IDENTITY_UNASSIGNED;
    msg->messagetype = PDN_CONNECTIVITY_REQUEST;
    msg->proceduretransactionidentity = pti;

    /* Mandatory - PDN connectivity request type */
    if (is_emergency) {
        msg->requesttype = REQUEST_TYPE_EMERGENCY;
    } else {
        msg->requesttype = REQUEST_TYPE_INITIAL_REQUEST;
    }

    /* Mandatory - PDN type */
    if (pdn_type == NET_PDN_TYPE_IPV4) {
        msg->pdntype = PDN_TYPE_IPV4;
    } else if (pdn_type == NET_PDN_TYPE_IPV6) {
        msg->pdntype = PDN_TYPE_IPV6;
    } else if (pdn_type == NET_PDN_TYPE_IPV4V6) {
        msg->pdntype = PDN_TYPE_IPV4V6;
    }

    /* Optional - Access Point Name */
    msg->presencemask = 0;
    if (apn) {
        size_t len = strlen(apn);
        if (len > 0) {
            msg->presencemask |=
                PDN_CONNECTIVITY_REQUEST_ACCESS_POINT_NAME_PRESENT;
            msg->accesspointname.accesspointnamevalue.length = len;
            msg->accesspointname.accesspointnamevalue.value = (uint8_t *)apn;
        }
    }

    LOG_TRACE(INFO, "ESM-SAP   - Send PDN Connectivity Request message "
              "(pti=%d, ebi=%d)",
              msg->proceduretransactionidentity, msg->epsbeareridentity);

    LOG_FUNC_RETURN(RETURNok);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_send_pdn_disconnect_request()                         **
 **                                                                        **
 ** Description: Builds PDN Disconnect Request message                     **
 **                                                                        **
 **      The PDN disconnect request message is sent by the UE to   **
 **      the network to initiate release of a PDN connection.      **
 **                                                                        **
 ** Inputs:  pti:       Procedure transaction identity assigned to **
 **             the PDN connection                         **
 **      ebi:       EPS bearer identity of the default bearer  **
 **             associated with the PDN to disconnect from **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     msg:       The ESM message to be sent                 **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int esm_send_pdn_disconnect_request(int pti, int ebi,
                                    pdn_disconnect_request_msg *msg)
{
    LOG_FUNC_IN;

    /* Mandatory - ESM message header */
    msg->protocoldiscriminator = EPS_SESSION_MANAGEMENT_MESSAGE;
    msg->epsbeareridentity = EPS_BEARER_IDENTITY_UNASSIGNED;
    msg->messagetype = PDN_DISCONNECT_REQUEST;
    msg->proceduretransactionidentity = pti;

    /* Mandatory - Linked EPS bearer identity */
    msg->linkedepsbeareridentity = ebi;
    /* Optional IEs  */
    msg->presencemask = 0;

    LOG_TRACE(INFO, "ESM-SAP   - Send PDN Disconnect Request message "
              "(pti=%d, ebi=%d)",
              msg->proceduretransactionidentity, msg->epsbeareridentity);

    LOG_FUNC_RETURN(RETURNok);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_send_activate_default_eps_bearer_context_accept()     **
 **                                                                        **
 ** Description: Builds Activate Default EPS Bearer Context Accept message **
 **                                                                        **
 **      The activate default EPS bearer context accept message is **
 **      sent by the UE to the network to acknowledge activation   **
 **      of a default EPS bearer context.                          **
 **                                                                        **
 ** Inputs:  ebi:       EPS bearer identity                        **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     msg:       The ESM message to be sent                 **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int esm_send_activate_default_eps_bearer_context_accept(int ebi,
        activate_default_eps_bearer_context_accept_msg *msg)
{
    LOG_FUNC_IN;

    /* Mandatory - ESM message header */
    msg->protocoldiscriminator = EPS_SESSION_MANAGEMENT_MESSAGE;
    msg->epsbeareridentity = ebi;
    msg->messagetype = ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT;
    msg->proceduretransactionidentity = PROCEDURE_TRANSACTION_IDENTITY_UNASSIGNED;
    /* Mandatory IEs */
    /* Optional IEs  */
    msg->presencemask = 0;

    LOG_TRACE(INFO, "ESM-SAP   - Send Activate Default EPS Bearer Context "
              "Accept message (pti=%d, ebi=%d)",
              msg->proceduretransactionidentity, msg->epsbeareridentity);

    LOG_FUNC_RETURN(RETURNok);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_send_activate_default_eps_bearer_context_reject()     **
 **                                                                        **
 ** Description: Builds Activate Default EPS Bearer Context Reject message **
 **                                                                        **
 **      The activate default EPS bearer context reject message is **
 **      sent by UE to the network to reject activation of a de-   **
 **      fault EPS bearer context.                                 **
 **                                                                        **
 ** Inputs:  ebi:       EPS bearer identity                        **
 **      esm_cause: ESM cause code                             **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     msg:       The ESM message to be sent                 **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int esm_send_activate_default_eps_bearer_context_reject(int ebi,
        activate_default_eps_bearer_context_reject_msg *msg, int esm_cause)
{
    LOG_FUNC_IN;

    /* Mandatory - ESM message header */
    msg->protocoldiscriminator = EPS_SESSION_MANAGEMENT_MESSAGE;
    msg->epsbeareridentity = ebi;
    msg->messagetype = ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT;
    msg->proceduretransactionidentity = PROCEDURE_TRANSACTION_IDENTITY_UNASSIGNED;
    /* Mandatory - ESM cause code */
    msg->esmcause = esm_cause;
    /* Optional IEs  */
    msg->presencemask = 0;

    LOG_TRACE(INFO, "ESM-SAP   - Send Activate Default EPS Bearer Context "
              "Reject message (pti=%d, ebi=%d)",
              msg->proceduretransactionidentity, msg->epsbeareridentity);

    LOG_FUNC_RETURN(RETURNok);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_send_activate_dedicated_eps_bearer_context_accept()   **
 **                                                                        **
 ** Description: Builds Activate Dedicated EPS Bearer Context Accept       **
 **      message                                                   **
 **                                                                        **
 **      The activate dedicated EPS bearer context accept message  **
 **      is sent by the UE to the network to acknowledge activa-   **
 **      tion of a dedicated EPS bearer context associated with    **
 **      the same PDN address(es) and APN as an already active EPS **
 **      bearer context.                                           **
 **                                                                        **
 ** Inputs:  ebi:       EPS bearer identity                        **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     msg:       The ESM message to be sent                 **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int esm_send_activate_dedicated_eps_bearer_context_accept(int ebi,
        activate_dedicated_eps_bearer_context_accept_msg *msg)
{
    LOG_FUNC_IN;

    /* Mandatory - ESM message header */
    msg->protocoldiscriminator = EPS_SESSION_MANAGEMENT_MESSAGE;
    msg->epsbeareridentity = ebi;
    msg->messagetype = ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_ACCEPT;
    msg->proceduretransactionidentity = PROCEDURE_TRANSACTION_IDENTITY_UNASSIGNED;
    /* Mandatory IEs */
    /* Optional IEs  */
    msg->presencemask = 0;

    LOG_TRACE(INFO, "ESM-SAP   - Send Activate Dedicated EPS Bearer Context "
              "Accept message (pti=%d, ebi=%d)",
              msg->proceduretransactionidentity, msg->epsbeareridentity);

    LOG_FUNC_RETURN(RETURNok);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_send_activate_dedicated_eps_bearer_context_reject()   **
 **                                                                        **
 ** Description: Builds Activate Dedicated EPS Bearer Context Reject       **
 **       message                                                  **
 **                                                                        **
 **      The activate dedicated EPS bearer context reject message  **
 **      is sent by UE to the network to reject activation of a    **
 **      dedicated EPS bearer context.                             **
 **                                                                        **
 ** Inputs:  ebi:       EPS bearer identity                        **
 **      esm_cause: ESM cause code                             **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     msg:       The ESM message to be sent                 **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int esm_send_activate_dedicated_eps_bearer_context_reject(int ebi,
        activate_dedicated_eps_bearer_context_reject_msg *msg, int esm_cause)
{
    LOG_FUNC_IN;

    /* Mandatory - ESM message header */
    msg->protocoldiscriminator = EPS_SESSION_MANAGEMENT_MESSAGE;
    msg->epsbeareridentity = ebi;
    msg->messagetype = ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REJECT;
    msg->proceduretransactionidentity = PROCEDURE_TRANSACTION_IDENTITY_UNASSIGNED;
    /* Mandatory - ESM cause code */
    msg->esmcause = esm_cause;
    /* Optional IEs  */
    msg->presencemask = 0;

    LOG_TRACE(INFO, "ESM-SAP   - Send Activate Dedicated EPS Bearer Context "
              "Reject message (pti=%d, ebi=%d)",
              msg->proceduretransactionidentity, msg->epsbeareridentity);

    LOG_FUNC_RETURN(RETURNok);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_send_deactivate_eps_bearer_context_accept()           **
 **                                                                        **
 ** Description: Builds Deactivate EPS Bearer Context Accept message       **
 **                                                                        **
 **      The deactivate EPS bearer context accept message is sent  **
 **      by the UE to acknowledge deactivation of an EPS bearer    **
 **      context.                                                  **
 **                                                                        **
 ** Inputs:  ebi:       EPS bearer identity                        **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     msg:       The ESM message to be sent                 **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int esm_send_deactivate_eps_bearer_context_accept(int ebi,
        deactivate_eps_bearer_context_accept_msg *msg)
{
    LOG_FUNC_IN;

    /* Mandatory - ESM message header */
    msg->protocoldiscriminator = EPS_SESSION_MANAGEMENT_MESSAGE;
    msg->epsbeareridentity = ebi;
    msg->messagetype = DEACTIVATE_EPS_BEARER_CONTEXT_ACCEPT;
    msg->proceduretransactionidentity = PROCEDURE_TRANSACTION_IDENTITY_UNASSIGNED;
    /* Mandatory IEs */
    /* Optional IEs  */
    msg->presencemask = 0;

    LOG_TRACE(INFO, "ESM-SAP   - Send Deactivate EPS Bearer Context Accept"
              " message (pti=%d, ebi=%d)",
              msg->proceduretransactionidentity, msg->epsbeareridentity);

    LOG_FUNC_RETURN(RETURNok);
}
#endif // NAS_UE

/*
 * --------------------------------------------------------------------------
 * Functions executed by the MME to send ESM message to the UE
 * --------------------------------------------------------------------------
 */
#ifdef NAS_MME
/****************************************************************************
 **                                                                        **
 ** Name:    esm_send_pdn_connectivity_reject()                        **
 **                                                                        **
 ** Description: Builds PDN Connectivity Reject message                    **
 **                                                                        **
 **      The PDN connectivity reject message is sent by the net-   **
 **      work to the UE to reject establishment of a PDN connec-   **
 **      tion.                                                     **
 **                                                                        **
 ** Inputs:  pti:       Procedure transaction identity             **
 **      esm_cause: ESM cause code                             **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     msg:       The ESM message to be sent                 **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int esm_send_pdn_connectivity_reject(int pti, pdn_connectivity_reject_msg *msg,
                                     int esm_cause)
{
    LOG_FUNC_IN;

    /* Mandatory - ESM message header */
    msg->protocoldiscriminator = EPS_SESSION_MANAGEMENT_MESSAGE;
    msg->epsbeareridentity = EPS_BEARER_IDENTITY_UNASSIGNED;
    msg->messagetype = PDN_CONNECTIVITY_REJECT;
    msg->proceduretransactionidentity = pti;

    /* Mandatory - ESM cause code */
    msg->esmcause = esm_cause;
    /* Optional IEs  */
    msg->presencemask = 0;

    LOG_TRACE(INFO, "ESM-SAP   - Send PDN Connectivity Reject message "
              "(pti=%d, ebi=%d)",
              msg->proceduretransactionidentity, msg->epsbeareridentity);

    LOG_FUNC_RETURN(RETURNok);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_send_pdn_disconnect_reject()                          **
 **                                                                        **
 ** Description: Builds PDN Disconnect Reject message                      **
 **                                                                        **
 **      The PDN disconnect reject message is sent by the network  **
 **      to the UE to reject release of a PDN connection.          **
 **                                                                        **
 ** Inputs:  pti:       Procedure transaction identity             **
 **      esm_cause: ESM cause code                             **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     msg:       The ESM message to be sent                 **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int esm_send_pdn_disconnect_reject(int pti, pdn_disconnect_reject_msg *msg,
                                   int esm_cause)
{
    LOG_FUNC_IN;

    /* Mandatory - ESM message header */
    msg->protocoldiscriminator = EPS_SESSION_MANAGEMENT_MESSAGE;
    msg->epsbeareridentity = EPS_BEARER_IDENTITY_UNASSIGNED;
    msg->messagetype = PDN_DISCONNECT_REJECT;
    msg->proceduretransactionidentity = pti;

    /* Mandatory - ESM cause code */
    msg->esmcause = esm_cause;
    /* Optional IEs  */
    msg->presencemask = 0;

    LOG_TRACE(INFO, "ESM-SAP   - Send PDN Disconnect Reject message "
              "(pti=%d, ebi=%d)",
              msg->proceduretransactionidentity, msg->epsbeareridentity);

    LOG_FUNC_RETURN(RETURNok);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_send_activate_default_eps_bearer_context_request()    **
 **                                                                        **
 ** Description: Builds Activate Default EPS Bearer Context Request        **
 **      message                                                   **
 **                                                                        **
 **      The activate default EPS bearer context request message   **
 **      is sent by the network to the UE to request activation of **
 **      a default EPS bearer context.                             **
 **                                                                        **
 ** Inputs:  pti:       Procedure transaction identity             **
 **      ebi:       EPS bearer identity                        **
 **      qos:       Subscribed EPS quality of service          **
 **      apn:       Access Point Name in used                  **
 **      pdn_addr:  PDN IPv4 address and/or IPv6 suffix        **
 **      esm_cause: ESM cause code                             **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     msg:       The ESM message to be sent                 **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int esm_send_activate_default_eps_bearer_context_request(int pti,
        int ebi,
        activate_default_eps_bearer_context_request_msg *msg,
        const OctetString *apn,
        int pdn_type,
        const OctetString *pdn_addr,
        const EpsQualityOfService *qos,
        int esm_cause)
{
    LOG_FUNC_IN;

    /* Mandatory - ESM message header */
    msg->protocoldiscriminator = EPS_SESSION_MANAGEMENT_MESSAGE;
    msg->epsbeareridentity = ebi;
    msg->messagetype = ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST;
    msg->proceduretransactionidentity = pti;

    /* Mandatory - EPS QoS */
    msg->epsqos = *qos;
    LOG_TRACE(INFO, "ESM-SAP   - epsqos  qci:  %u", qos->qci);
    if (qos->bitRatesPresent) {
        LOG_TRACE(INFO, "ESM-SAP   - epsqos  maxBitRateForUL:  %u", qos->bitRates.maxBitRateForUL);
        LOG_TRACE(INFO, "ESM-SAP   - epsqos  maxBitRateForDL:  %u", qos->bitRates.maxBitRateForDL);
        LOG_TRACE(INFO, "ESM-SAP   - epsqos  guarBitRateForUL: %u", qos->bitRates.guarBitRateForUL);
        LOG_TRACE(INFO, "ESM-SAP   - epsqos  guarBitRateForDL: %u", qos->bitRates.guarBitRateForDL);
    } else {
        LOG_TRACE(INFO, "ESM-SAP   - epsqos  no bit rates defined");
    }
    if (qos->bitRatesExtPresent) {
        LOG_TRACE(INFO, "ESM-SAP   - epsqos  maxBitRateForUL  Ext: %u", qos->bitRatesExt.maxBitRateForUL);
        LOG_TRACE(INFO, "ESM-SAP   - epsqos  maxBitRateForDL  Ext: %u", qos->bitRatesExt.maxBitRateForDL);
        LOG_TRACE(INFO, "ESM-SAP   - epsqos  guarBitRateForUL Ext: %u", qos->bitRatesExt.guarBitRateForUL);
        LOG_TRACE(INFO, "ESM-SAP   - epsqos  guarBitRateForDL Ext: %u", qos->bitRatesExt.guarBitRateForDL);
    } else {
        LOG_TRACE(INFO, "ESM-SAP   - epsqos  no bit rates ext defined");
    }


    if ((apn == NULL) || ((apn  != NULL) && (apn->value == NULL))) {
        LOG_TRACE(WARNING, "ESM-SAP   - apn is NULL!");
    }

    LOG_TRACE(INFO, "ESM-SAP   - apn is %s", apn->value);

    /* Mandatory - Access Point Name */
    msg->accesspointname.accesspointnamevalue = *apn;

    /* Mandatory - PDN address */
    LOG_TRACE(INFO, "ESM-SAP   - pdn_type is %u", pdn_type);
    msg->pdnaddress.pdntypevalue = pdn_type;
    LOG_TRACE(INFO, "ESM-SAP   - pdn_addr is %u", dump_octet_string(pdn_addr));
    msg->pdnaddress.pdnaddressinformation = *pdn_addr;

    /* Optional - ESM cause code */
    msg->presencemask = 0;
    if (esm_cause != ESM_CAUSE_SUCCESS) {
        msg->presencemask |=
            ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_ESM_CAUSE_PRESENT;
        msg->esmcause = esm_cause;
    }

    LOG_TRACE(INFO, "ESM-SAP   - Send Activate Default EPS Bearer Context "
              "Request message (pti=%d, ebi=%d)",
              msg->proceduretransactionidentity, msg->epsbeareridentity);

    LOG_FUNC_RETURN(RETURNok);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_send_activate_dedicated_eps_bearer_context_request()  **
 **                                                                        **
 ** Description: Builds Activate Dedicated EPS Bearer Context Request      **
 **      message                                                   **
 **                                                                        **
 **      The activate dedicated EPS bearer context request message **
 **      is sent by the network to the UE to request activation of **
 **      a dedicated EPS bearer context associated with the same   **
 **      PDN address(es) and APN as an already active default EPS  **
 **      bearer context.                                           **
 **                                                                        **
 ** Inputs:  pti:       Procedure transaction identity             **
 **      ebi:       EPS bearer identity                        **
 **      linked_ebi:    EPS bearer identity of the default bearer  **
 **             associated with the EPS dedicated bearer   **
 **             to be activated                            **
 **      qos:       EPS quality of service                     **
 **      tft:       Traffic flow template                      **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     msg:       The ESM message to be sent                 **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int esm_send_activate_dedicated_eps_bearer_context_request(int pti, int ebi,
        activate_dedicated_eps_bearer_context_request_msg *msg,
        int linked_ebi, const EpsQualityOfService *qos,
        PacketFilters *pkfs, int n_pkfs)
{
    int i;
    LOG_FUNC_IN;

    /* Mandatory - ESM message header */
    msg->protocoldiscriminator = EPS_SESSION_MANAGEMENT_MESSAGE;
    msg->epsbeareridentity = ebi;
    msg->messagetype = ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST;
    msg->proceduretransactionidentity = pti;

    /* Mandatory - EPS QoS */
    msg->epsqos = *qos;

    /* Mandatory - traffic flow template */
    msg->tft.tftoperationcode = TRAFFIC_FLOW_TEMPLATE_OPCODE_CREATE;
    msg->tft.ebit = TRAFFIC_FLOW_TEMPLATE_PARAMETER_LIST_IS_NOT_INCLUDED;
    msg->tft.numberofpacketfilters = n_pkfs;
    for (i = 0; i < msg->tft.numberofpacketfilters; i++) {
        msg->tft.packetfilterlist.createtft[i] = (*pkfs)[i];
    }

    /* Optional */
    msg->presencemask = 0;

    LOG_TRACE(INFO, "ESM-SAP   - Send Activate Dedicated EPS Bearer Context "
              "Request message (pti=%d, ebi=%d)",
              msg->proceduretransactionidentity, msg->epsbeareridentity);

    LOG_FUNC_RETURN(RETURNok);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_send_deactivate_eps_bearer_context_request()          **
 **                                                                        **
 ** Description: Builds Deactivate EPS Bearer Context Request message      **
 **                                                                        **
 **      The deactivate EPS bearer context request message is sent **
 **      by the network to request deactivation of an active EPS   **
 **      bearer context.                                           **
 **                                                                        **
 ** Inputs:  pti:       Procedure transaction identity             **
 **      ebi:       EPS bearer identity                        **
 **      esm_cause: ESM cause code                             **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     msg:       The ESM message to be sent                 **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int esm_send_deactivate_eps_bearer_context_request(int pti, int ebi,
        deactivate_eps_bearer_context_request_msg *msg,
        int esm_cause)
{
    LOG_FUNC_IN;

    /* Mandatory - ESM message header */
    msg->protocoldiscriminator = EPS_SESSION_MANAGEMENT_MESSAGE;
    msg->epsbeareridentity = ebi;
    msg->messagetype = DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST;
    msg->proceduretransactionidentity = pti;

    /* Mandatory - ESM cause code */
    msg->esmcause = esm_cause;
    /* Optional IEs  */
    msg->presencemask = 0;

    LOG_TRACE(INFO, "ESM-SAP   - Send Deactivate EPS Bearer Context Request "
              "message (pti=%d, ebi=%d)",
              msg->proceduretransactionidentity, msg->epsbeareridentity);

    LOG_FUNC_RETURN(RETURNok);
}
#endif // NAS_MME

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/


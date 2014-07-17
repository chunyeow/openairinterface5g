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
Source      Detach.c

Version     0.1

Date        2013/05/07

Product     NAS stack

Subsystem   EPS Mobility Management

Author      Frederic Maurel

Description Defines the detach related EMM procedure executed by the
        Non-Access Stratum.

        The detach procedure is used by the UE to detach for EPS servi-
        ces, to disconnect from the last PDN it is connected to; by the
        network to inform the UE that it is detached for EPS services
        or non-EPS services or both, to disconnect the UE from the last
        PDN to which it is connected and to inform the UE to re-attach
        to the network and re-establish all PDN connections.

*****************************************************************************/

#include "emm_proc.h"
#include "nas_log.h"
#include "nas_timer.h"

#include "emmData.h"

#include "emm_sap.h"
#include "esm_sap.h"

#include <stdlib.h> // free

/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

/* String representation of the detach type */
static const char *_emm_detach_type_str[] = {
    "EPS", "IMSI", "EPS/IMSI",
    "RE-ATTACH REQUIRED", "RE-ATTACH NOT REQUIRED", "RESERVED"
};

/*
 * --------------------------------------------------------------------------
 *      Internal data handled by the detach procedure in the UE
 * --------------------------------------------------------------------------
 */
#ifdef NAS_UE
/*
 * Timer handlers
 */
void *_emm_detach_t3421_handler(void *);

/*
 * Abnormal case detach procedures
 */
static int _emm_detach_abort(emm_proc_detach_type_t type);

/*
 * Internal data used for detach procedure
 */
static struct {
#define EMM_DETACH_COUNTER_MAX  5
    unsigned int count;      /* Counter used to limit the number of
                  * subsequently detach attempts    */
    int switch_off;      /* UE switch-off indicator     */
    emm_proc_detach_type_t type; /* Type of the detach procedure
                  * currently in progress       */
} _emm_detach_data = {0, FALSE, EMM_DETACH_TYPE_RESERVED};
#endif // NAS_UE

/*
 * --------------------------------------------------------------------------
 *      Internal data handled by the detach procedure in the MME
 * --------------------------------------------------------------------------
 */
#ifdef NAS_MME
#endif // NAS_MME

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/*
 * --------------------------------------------------------------------------
 *          Detach procedure executed by the UE
 * --------------------------------------------------------------------------
 */
#ifdef NAS_UE
/****************************************************************************
 **                                                                        **
 ** Name:    emm_proc_detach()                                         **
 **                                                                        **
 ** Description: Initiates the detach procedure in order for the UE to de- **
 **      tach for EPS services.                                    **
 **                                                                        **
 **              3GPP TS 24.301, section 5.5.2.2.1                         **
 **      In state EMM-REGISTERED or EMM-REGISTERED-INITIATED, the  **
 **      UE initiates the detach procedure by sending a DETACH RE- **
 **      QUEST message to the network, starting timer T3421 and    **
 **      entering state EMM-DEREGISTERED-INITIATED.                **
 **                                                                        **
 ** Inputs:  type:      Type of the requested detach               **
 **      switch_off:    Indicates whether the detach is required   **
 **             because the UE is switched off or not      **
 **      Others:    _emm_data                                  **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    _emm_detach_data                           **
 **                                                                        **
 ***************************************************************************/
int emm_proc_detach(emm_proc_detach_type_t type, int switch_off)
{
    LOG_FUNC_IN;

    emm_sap_t emm_sap;
    emm_as_data_t *emm_as = &emm_sap.u.emm_as.u.data;
    int rc;

    LOG_TRACE(INFO, "EMM-PROC  - Initiate EPS detach type = %s (%d)",
              _emm_detach_type_str[type], type);

    /* Initialize the detach procedure internal data */
    _emm_detach_data.count = 0;
    _emm_detach_data.switch_off = switch_off;
    _emm_detach_data.type = type;

    /* Setup EMM procedure handler to be executed upon receiving
     * lower layer notification */
    rc = emm_proc_lowerlayer_initialize(emm_proc_detach_request,
                                        emm_proc_detach_failure,
                                        emm_proc_detach_release, NULL);
    if (rc != RETURNok) {
        LOG_TRACE(WARNING, "Failed to initialize EMM procedure handler");
        LOG_FUNC_RETURN (RETURNerror);
    }

    /* Setup NAS information message to transfer */
    emm_as->NASinfo = EMM_AS_NAS_INFO_DETACH;
    emm_as->NASmsg.length = 0;
    emm_as->NASmsg.value = NULL;
    /* Set the detach type */
    emm_as->type = type;
    /* Set the switch-off indicator */
    emm_as->switch_off = switch_off;
    /* Set the EPS mobile identity */
    emm_as->guti = _emm_data.guti;
    emm_as->ueid = 0;
    /* Setup EPS NAS security data */
    emm_as_set_security_data(&emm_as->sctx, _emm_data.security, FALSE, TRUE);

    /*
     * Notify EMM-AS SAP that Detach Request message has to
     * be sent to the network
     */
    emm_sap.primitive = EMMAS_DATA_REQ;
    rc = emm_sap_send(&emm_sap);

    LOG_FUNC_RETURN(rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_proc_detach_request()                                 **
 **                                                                        **
 ** Description: Performs the detach procedure upon receipt of indication  **
 **      from lower layers that Detach Request message has been    **
 **      successfully delivered to the network.                    **
 **                                                                        **
 ** Inputs:  args:      Not used                                   **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    T3421                                      **
 **                                                                        **
 ***************************************************************************/
int emm_proc_detach_request(void *args)
{
    LOG_FUNC_IN;

    emm_sap_t emm_sap;
    int rc;

    if ( !_emm_detach_data.switch_off ) {
        /* Start T3421 timer */
        T3421.id = nas_timer_start(T3421.sec, _emm_detach_t3421_handler, NULL);
        LOG_TRACE(INFO, "EMM-PROC  - Timer T3421 (%d) expires in %ld seconds",
                  T3421.id, T3421.sec);
    }

    /*
     * Notify EMM that Detach Request has been sent to the network
     */
    emm_sap.primitive = EMMREG_DETACH_REQ;
    rc = emm_sap_send(&emm_sap);

    LOG_FUNC_RETURN(rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_proc_detach_accept()                                  **
 **                                                                        **
 ** Description: Performs the UE initiated detach procedure for EPS servi- **
 **      ces only When the DETACH ACCEPT message is received from  **
 **      the network.                                              **
 **                                                                        **
 **              3GPP TS 24.301, section 5.5.2.2.2                         **
 **              Upon receiving the DETACH ACCEPT message, the UE shall    **
 **      stop timer T3421, locally deactivate all EPS bearer con-  **
 **      texts without peer-to-peer signalling and enter state EMM-**
 **      DEREGISTERED.                                             **
 **                                                                        **
 ** Inputs:  None                                                      **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    T3421                                      **
 **                                                                        **
 ***************************************************************************/
int emm_proc_detach_accept(void)
{
    LOG_FUNC_IN;

    int rc;

    LOG_TRACE(INFO, "EMM-PROC  - UE initiated detach procedure completion");

    /* Reset EMM procedure handler */
    (void) emm_proc_lowerlayer_initialize(NULL, NULL, NULL, NULL);

    /* Stop timer T3421 */
    T3421.id = nas_timer_stop(T3421.id);

    /*
     * Notify ESM that all EPS bearer contexts have to be locally deactivated
     */
    esm_sap_t esm_sap;
    esm_sap.primitive = ESM_EPS_BEARER_CONTEXT_DEACTIVATE_REQ;
    esm_sap.data.eps_bearer_context_deactivate.ebi = ESM_SAP_ALL_EBI;
    rc = esm_sap_send(&esm_sap);

    /*
     * XXX - Upon receiving notification from ESM that all EPS bearer
     * contexts are locally deactivated, the UE is considered as
     * detached from the network and is entered state EMM-DEREGISTERED
     */
    LOG_FUNC_RETURN(rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_proc_detach_failure()                                 **
 **                                                                        **
 ** Description: Performs the detach procedure abnormal case upon receipt  **
 **          of transmission failure of Detach Request message.        **
 **                                                                        **
 **              3GPP TS 24.301, section 5.5.2.2.4, case h                 **
 **      The UE shall restart the detach procedure.                **
 **                                                                        **
 ** Inputs:  is_initial:    Not used                                   **
 **          args:      Not used                                   **
 **      Others:    _emm_detach_data                           **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int emm_proc_detach_failure(int is_initial, void *args)
{
    LOG_FUNC_IN;

    emm_sap_t emm_sap;
    int rc;

    LOG_TRACE(WARNING, "EMM-PROC  - Network detach failure");

    /* Reset EMM procedure handler */
    (void) emm_proc_lowerlayer_initialize(NULL, NULL, NULL, NULL);

    /* Stop timer T3421 */
    T3421.id = nas_timer_stop(T3421.id);

    /*
     * Notify EMM that detach procedure has to be restarted
     */
    emm_sap.primitive = EMMREG_DETACH_INIT;
    emm_sap.u.emm_reg.u.detach.switch_off = _emm_detach_data.switch_off;
    rc = emm_sap_send(&emm_sap);

    LOG_FUNC_RETURN(rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_proc_detach_release()                                 **
 **                                                                        **
 ** Description: Performs the detach procedure abnormal case upon receipt  **
 **          of NAS signalling connection release indication before    **
 **      reception of Detach Accept message.                       **
 **                                                                        **
 **              3GPP TS 24.301, section 5.5.2.2.4, case b                 **
 **      The  detach procedure shall be aborted.                   **
 **                                                                        **
 ** Inputs:  args:      not used                                   **
 **      Others:    _emm_detach_data                           **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int emm_proc_detach_release(void *args)
{
    LOG_FUNC_IN;

    LOG_TRACE(WARNING, "EMM-PROC  - NAS signalling connection released");

    /* Abort the detach procedure */
    int rc = _emm_detach_abort(_emm_detach_data.type);

    LOG_FUNC_RETURN(rc);
}
#endif // NAS_UE

/*
 * --------------------------------------------------------------------------
 *          Detach procedure executed by the MME
 * --------------------------------------------------------------------------
 */
#ifdef NAS_MME
/****************************************************************************
 **                                                                        **
 ** Name:    emm_proc_detach()                                         **
 **                                                                        **
 ** Description: Initiate the detach procedure to inform the UE that it is **
 **      detached for EPS services, or to re-attach to the network **
 **      and re-establish all PDN connections.                     **
 **                                                                        **
 **              3GPP TS 24.301, section 5.5.2.3.1                         **
 **      In state EMM-REGISTERED the network initiates the detach  **
 **      procedure by sending a DETACH REQUEST message to the UE,  **
 **      starting timer T3422 and entering state EMM-DEREGISTERED- **
 **      INITIATED.                                                **
 **                                                                        **
 ** Inputs:  ueid:      UE lower layer identifier                  **
 **      type:      Type of the requested detach               **
 **      Others:    _emm_detach_type_str                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    T3422                                      **
 **                                                                        **
 ***************************************************************************/
int emm_proc_detach(unsigned int ueid, emm_proc_detach_type_t type)
{
    LOG_FUNC_IN;

    int rc = RETURNerror;

    LOG_TRACE(INFO, "EMM-PROC  - Initiate detach type = %s (%d)",
              _emm_detach_type_str[type], type);

    /* TODO */

    LOG_FUNC_RETURN(rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_proc_detach_request()                                 **
 **                                                                        **
 ** Description: Performs the UE initiated detach procedure for EPS servi- **
 **      ces only When the DETACH REQUEST message is received by   **
 **      the network.                                              **
 **                                                                        **
 **              3GPP TS 24.301, section 5.5.2.2.2                         **
 **      Upon receiving the DETACH REQUEST message the network     **
 **      shall send a DETACH ACCEPT message to the UE and store    **
 **      the current EPS security context, if the detach type IE   **
 **      does not indicate "switch off". Otherwise, the procedure  **
 **      is completed when the network receives the DETACH REQUEST **
 **      message.                                                  **
 **      The network shall deactivate the EPS bearer context(s)    **
 **      for this UE locally without peer-to-peer signalling and   **
 **      shall enter state EMM-DEREGISTERED.                       **
 **                                                                        **
 ** Inputs:  ueid:      UE lower layer identifier                  **
 **      type:      Type of the requested detach               **
 **      switch_off:    Indicates whether the detach is required   **
 **             because the UE is switched off or not      **
 **      native_ksi:    TRUE if the security context is of type    **
 **             native                                     **
 **      ksi:       The NAS ket sey identifier                 **
 **      guti:      The GUTI if provided by the UE             **
 **      imsi:      The IMSI if provided by the UE             **
 **      imei:      The IMEI if provided by the UE             **
 **      Others:    _emm_data                                  **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int emm_proc_detach_request(unsigned int ueid, emm_proc_detach_type_t type,
                            int switch_off, int native_ksi, int ksi,
                            GUTI_t *guti, imsi_t *imsi, imei_t *imei)
{
    LOG_FUNC_IN;

    int rc;
    emm_data_context_t *emm_ctx = NULL;

    LOG_TRACE(INFO, "EMM-PROC  - Detach type = %s (%d) requested (ueid=%u)",
              _emm_detach_type_str[type], type, ueid);

    /* Get the UE context */
#if defined(EPC_BUILD)
    if (ueid > 0) {
        emm_ctx = emm_data_context_get(&_emm_data, ueid);
    }
#else
    if (ueid < EMM_DATA_NB_UE_MAX) {
        emm_ctx = _emm_data.ctx[ueid];
    }
#endif

    if (emm_ctx == NULL) {
        LOG_TRACE(WARNING, "No EMM context exists for the UE (ueid=%u)", ueid);
        LOG_FUNC_RETURN(RETURNok);
    }

    if (switch_off) {
        /* The UE is switched off */
        if (emm_ctx->guti) {
            free(emm_ctx->guti);
        }
        if (emm_ctx->imsi) {
            free(emm_ctx->imsi);
        }
        if (emm_ctx->imei) {
            free(emm_ctx->imei);
        }
        if (emm_ctx->esm_msg.length > 0) {
            free(emm_ctx->esm_msg.value);
        }
        /* Release NAS security context */
        if (emm_ctx->security) {
            emm_security_context_t *security = emm_ctx->security;
            if (security->kasme.value) {
                free(security->kasme.value);
            }
            if (security->knas_enc.value) {
                free(security->knas_enc.value);
            }
            if (security->knas_int.value) {
                free(security->knas_int.value);
            }
            free(emm_ctx->security);
        }
        /* Release the EMM context */
#if defined(EPC_BUILD)
        emm_data_context_remove(&_emm_data, emm_ctx);
        free(emm_ctx);
#else
        free(_emm_data.ctx[ueid]);
        _emm_data.ctx[ueid] = NULL;
#endif
        rc = RETURNok;
    } else {
        /* Normal detach without UE switch-off */
        emm_sap_t emm_sap;
        emm_as_data_t *emm_as = &emm_sap.u.emm_as.u.data;

        /* Setup NAS information message to transfer */
        emm_as->NASinfo = EMM_AS_NAS_INFO_DETACH;
        emm_as->NASmsg.length = 0;
        emm_as->NASmsg.value = NULL;
        /* Set the UE identifier */
        emm_as->guti = NULL;
        emm_as->ueid = ueid;
        /* Setup EPS NAS security data */
        emm_as_set_security_data(&emm_as->sctx, emm_ctx->security, FALSE, TRUE);
        /*
         * Notify EMM-AS SAP that Detach Accept message has to
         * be sent to the network
         */
        emm_sap.primitive = EMMAS_DATA_REQ;
        rc = emm_sap_send(&emm_sap);
    }

    if (rc != RETURNerror) {
        /*
         * Notify ESM that all EPS bearer contexts allocated for this UE have
         * to be locally deactivated
         */
        esm_sap_t esm_sap;
        esm_sap.primitive = ESM_EPS_BEARER_CONTEXT_DEACTIVATE_REQ;
        esm_sap.ueid = ueid;
        esm_sap.ctx  = emm_ctx;
        esm_sap.data.eps_bearer_context_deactivate.ebi = ESM_SAP_ALL_EBI;
        rc = esm_sap_send(&esm_sap);

        if (rc != RETURNerror) {
            emm_sap_t emm_sap;
            /*
             * Notify EMM that the UE has been implicitly detached
             */
            emm_sap.primitive = EMMREG_DETACH_REQ;
            emm_sap.u.emm_reg.ueid = ueid;
            emm_sap.u.emm_reg.ctx  = emm_ctx;
            rc = emm_sap_send(&emm_sap);
        }
    }

    LOG_FUNC_RETURN(rc);
}
#endif // NAS_MME

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/

#ifdef NAS_UE
/*
 * --------------------------------------------------------------------------
 *              Timer handlers
 * --------------------------------------------------------------------------
 */

/****************************************************************************
 **                                                                        **
 ** Name:    _emm_detach_t3421_handler()                               **
 **                                                                        **
 ** Description: T3421 timeout handler                                     **
 **                                                                        **
 **              3GPP TS 24.301, section 5.5.2.2.4 case c                  **
 **      On the first four expiries of the timer, the UE shall re- **
 **      transmit the DETACH REQUEST message and shall reset and   **
 **      restart timer T3421. On the fifth expiry of timer T3421,  **
 **      the detach procedure shall be aborted.                    **
 **                                                                        **
 ** Inputs:  args:      handler parameters                         **
 **      Others:    _emm_detach_data                           **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    None                                       **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
void *_emm_detach_t3421_handler(void *args)
{
    LOG_FUNC_IN;

    int rc;

    /* Increment the retransmission counter */
    _emm_detach_data.count += 1;

    LOG_TRACE(WARNING, "EMM-PROC  - T3421 timer expired, "
              "retransmission counter = %d", _emm_detach_data.count);

    if (_emm_detach_data.count < EMM_DETACH_COUNTER_MAX) {
        /* Retransmit the Detach Request message */
        emm_sap_t emm_sap;
        emm_as_data_t *emm_as = &emm_sap.u.emm_as.u.data;

        /* Stop timer T3421 */
        T3421.id = nas_timer_stop(T3421.id);

        /* Setup NAS information message to transfer */
        emm_as->NASinfo = EMM_AS_NAS_INFO_DETACH;
        emm_as->NASmsg.length = 0;
        emm_as->NASmsg.value = NULL;
        /* Set the detach type */
        emm_as->type = _emm_detach_data.type;
        /* Set the switch-off indicator */
        emm_as->switch_off = _emm_detach_data.switch_off;
        /* Set the EPS mobile identity */
        emm_as->guti = _emm_data.guti;
        emm_as->ueid = 0;
        /* Setup EPS NAS security data */
        emm_as_set_security_data(&emm_as->sctx, _emm_data.security,
                                 FALSE, TRUE);

        /*
         * Notify EMM-AS SAP that Detach Request message has to
         * be sent to the network
         */
        emm_sap.primitive = EMMAS_DATA_REQ;
        rc = emm_sap_send(&emm_sap);

        if (rc != RETURNerror) {
            /* Start T3421 timer */
            T3421.id = nas_timer_start(T3421.sec, _emm_detach_t3421_handler, NULL);
            LOG_TRACE(INFO, "EMM-PROC  - Timer T3421 (%d) expires in %ld "
                      "seconds", T3421.id, T3421.sec);
        }
    } else {
        /* Abort the detach procedure */
        rc = _emm_detach_abort(_emm_detach_data.type);
    }

    LOG_FUNC_RETURN(NULL);
}

/*
 * --------------------------------------------------------------------------
 *              Abnormal cases in the UE
 * --------------------------------------------------------------------------
 */

/****************************************************************************
 **                                                                        **
 ** Name:    _emm_detach_abort()                                       **
 **                                                                        **
 ** Description: Aborts the detach procedure                               **
 **                                                                        **
 ** Inputs:  type:      not used                                   **
 **      Others:    _emm_detach_data                           **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    T3421                                      **
 **                                                                        **
 ***************************************************************************/
static int _emm_detach_abort(emm_proc_detach_type_t type)
{
    LOG_FUNC_IN;

    emm_sap_t emm_sap;
    int rc ;

    LOG_TRACE(WARNING, "EMM-PROC  - Abort the detach procedure");

    /* Reset EMM procedure handler */
    (void) emm_proc_lowerlayer_initialize(NULL, NULL, NULL, NULL);

    /* Stop timer T3421 */
    T3421.id = nas_timer_stop(T3421.id);

    /*
     * Notify EMM that detach procedure failed
     */
    emm_sap.primitive = EMMREG_DETACH_FAILED;
    emm_sap.u.emm_reg.u.detach.type = type;
    rc = emm_sap_send(&emm_sap);

    LOG_FUNC_RETURN (rc);
}
#endif // NAS_UE

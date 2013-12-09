/*****************************************************************************
            Eurecom OpenAirInterface 3
            Copyright(c) 2012 Eurecom

Source      emm_cn.c

Version     0.1

Date        2013/12/05

Product     NAS stack

Subsystem   EPS Core Network

Author      Sebastien Roux

Description

*****************************************************************************/

#include <string.h>

#if defined(EPC_BUILD)

#include "nas_log.h"
#include "commonDef.h"

#include "emm_cn.h"
#include "emm_sap.h"
#include "emm_proc.h"
#include "emm_cause.h"

/*
 * String representation of EMMCN-SAP primitives
 */
static const char *_emm_cn_primitive_str[] = {
    "EMM_CN_AUTHENTICATION_PARAM_RES",
    "EMM_CN_AUTHENTICATION_PARAM_FAIL",
    "EMM_CN_DEREGISTER_UE",
};

static int _emm_cn_authentication_res(const emm_cn_auth_res_t *msg)
{
    emm_data_context_t *emm_ctx = NULL;
    int rc = RETURNerror;
    OctetString loc_rand;
    OctetString autn;

    /* We received security vector from HSS. Try to setup security with UE */

    LOG_FUNC_IN;

    emm_ctx = emm_data_context_get(&_emm_data, msg->ue_id);
    if (emm_ctx == NULL) {
        LOG_TRACE(ERROR, "EMM-PROC  - "
                  "Failed to find UE associated to id %u...", msg->ue_id);
        LOG_FUNC_RETURN (rc);
    }

    /* Copy provided vector to user context */
    memcpy(emm_ctx->vector.kasme, msg->vector.kasme,     AUTH_KASME_SIZE);
    memcpy(emm_ctx->vector.autn,  msg->vector.autn,      AUTH_AUTN_SIZE);
    memcpy(emm_ctx->vector.rand,  msg->vector.rand,      AUTH_RAND_SIZE);
    memcpy(emm_ctx->vector.xres,  msg->vector.xres.data, msg->vector.xres.size);

    loc_rand.value  = emm_ctx->vector.rand;
    loc_rand.length = AUTH_RAND_SIZE;

    autn.value  = emm_ctx->vector.autn;
    autn.length = AUTH_AUTN_SIZE;

    emm_ctx->vector.xres_size = msg->vector.xres.size;

    /* 3GPP TS 24.401, Figure 5.3.2.1-1, point 5a
     * No EMM context exists for the UE in the network; authentication
     * and NAS security setup to activate integrity protection and NAS
     * ciphering are mandatory.
     */
    rc = emm_proc_authentication(emm_ctx, emm_ctx->ueid, 0, // TODO: eksi != 0
                                 &loc_rand, &autn,
                                 emm_attach_security,
                                 NULL,
                                 NULL);
    if (rc != RETURNok) {
        /* Failed to initiate the authentication procedure */
        LOG_TRACE(WARNING, "EMM-PROC  - "
        "Failed to initiate authentication procedure");
        emm_ctx->emm_cause = EMM_CAUSE_ILLEGAL_UE;
    }

    LOG_FUNC_RETURN (rc);
}

static int _emm_cn_authentication_fail(const emm_cn_auth_fail_t *msg)
{
    int rc = RETURNerror;

    LOG_FUNC_IN;

    rc = emm_proc_attach_reject(msg->ue_id, msg->cause);

    LOG_FUNC_RETURN (rc);
}

static int _emm_cn_deregister_ue(const UInt32_t ue_id)
{
    int rc = RETURNok;

    LOG_FUNC_IN;

    LOG_FUNC_RETURN (rc);
}

int emm_cn_send(const emm_cn_t *msg)
{
    int rc = RETURNerror;
    emm_cn_primitive_t primitive = msg->primitive;

    LOG_FUNC_IN;

    LOG_TRACE(INFO, "EMMCN-SAP - Received primitive %s (%d)",
              _emm_cn_primitive_str[primitive - _EMMCN_START - 1], primitive);

    switch (primitive) {
        case _EMMCN_AUTHENTICATION_PARAM_RES:
            rc = _emm_cn_authentication_res(msg->u.auth_res);
            break;

        case _EMMCN_AUTHENTICATION_PARAM_FAIL:
            rc = _emm_cn_authentication_fail(msg->u.auth_fail);
            break;

        case EMMCN_DEREGISTER_UE:
            rc = _emm_cn_deregister_ue(msg->u.deregister.UEid);
            break;

        default:
            /* Other primitives are forwarded to the Access Stratum */
            rc = RETURNerror;
            break;
    }

    if (rc != RETURNok) {
        LOG_TRACE(ERROR, "EMMCN-SAP - Failed to process primitive %s (%d)",
                  _emm_cn_primitive_str[primitive - _EMMCN_START - 1],
                  primitive);
    }

    LOG_FUNC_RETURN (rc);
}
#endif

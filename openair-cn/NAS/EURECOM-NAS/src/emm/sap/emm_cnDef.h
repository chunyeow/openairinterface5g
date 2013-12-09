/*****************************************************************************
            Eurecom OpenAirInterface 3
            Copyright(c) 2012 Eurecom

Source      emm_cnDef.h

Version     0.1

Date        2013/12/05

Product     NAS stack

Subsystem   EPS Core Network

Author      Sebastien Roux

Description

*****************************************************************************/

#if defined(EPC_BUILD)
#include "intertask_interface.h"
#endif

#ifndef __EMM_CNDEF_H__
#define __EMM_CNDEF_H__

typedef enum emmcn_primitive_s {
    _EMMCN_START = 400,
#if defined(EPC_BUILD)
    _EMMCN_AUTHENTICATION_PARAM_RES,
    _EMMCN_AUTHENTICATION_PARAM_FAIL,
    _EMMCN_DEREGISTER_UE,
#endif
    _EMMCN_END
} emm_cn_primitive_t;

#if defined(EPC_BUILD)
typedef nas_auth_param_rsp_t  emm_cn_auth_res_t;
typedef nas_auth_param_fail_t emm_cn_auth_fail_t;

typedef struct emm_cn_deregister_ue_s {
    UInt32_t UEid;
} emm_cn_deregister_ue_t;

typedef struct emm_mme_ul_s {
    emm_cn_primitive_t primitive;
    union {
        emm_cn_auth_res_t       *auth_res;
        emm_cn_auth_fail_t      *auth_fail;
        emm_cn_deregister_ue_t   deregister;
    } u;
} emm_cn_t;
#endif

#endif /* __EMM_CNDEF_H__ */

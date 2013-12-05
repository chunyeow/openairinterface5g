/*****************************************************************************
            Eurecom OpenAirInterface 3
            Copyright(c) 2013 Eurecom

Source      emm_cn.h

Version     0.1

Date        2013/12/05

Product     NAS stack

Subsystem   EPS Core Network

Author      Sebastien Roux

Description

*****************************************************************************/

#include "emm_cnDef.h"

#ifndef _EMM_CN_H_
#define _EMM_CN_H_

#if defined(EPC_BUILD)
int emm_cn_send(const emm_cn_t *msg);
#endif

#endif /* _EMM_CN_H_ */

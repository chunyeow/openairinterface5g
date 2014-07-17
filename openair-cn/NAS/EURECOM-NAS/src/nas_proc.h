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
Source      nas_proc.h

Version     0.1

Date        2012/09/20

Product     NAS stack

Subsystem   NAS main process

Author      Frederic Maurel

Description NAS procedure call manager

*****************************************************************************/
#ifndef __NAS_PROC_H__
#define __NAS_PROC_H__

#if defined(EPC_BUILD)
# include "mme_config.h"
# include "emm_cnDef.h"
#endif

#include "commonDef.h"
#include "networkDef.h"

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

#ifdef NAS_UE
void nas_proc_initialize(emm_indication_callback_t emm_cb,
                         esm_indication_callback_t esm_cb, const char *imei);
#endif
#ifdef NAS_MME
# if defined(EPC_BUILD)
void nas_proc_initialize(mme_config_t *mme_config_p);
# else
void nas_proc_initialize(void);
# endif
#endif

void nas_proc_cleanup(void);

/*
 * --------------------------------------------------------------------------
 *          NAS procedures triggered by the user
 * --------------------------------------------------------------------------
 */

#ifdef NAS_UE
int nas_proc_enable_s1_mode(void);
int nas_proc_disable_s1_mode(void);
int nas_proc_get_eps(int *stat);

int nas_proc_get_imsi(char *imsi_str);
int nas_proc_get_msisdn(char *msisdn_str, int *ton_npi);

int nas_proc_get_signal_quality(int *rsrq, int *rsrp);

int nas_proc_register(int mode, int format, const network_plmn_t *oper, int AcT);
int nas_proc_deregister(void);
int nas_proc_get_reg_data(int *mode, int *selected, int format,
                          network_plmn_t *oper, int *AcT);
int nas_proc_get_oper_list(const char **oper_list);

int nas_proc_get_reg_status(int *stat);
int nas_proc_get_loc_info(char *tac, char *ci, int *AcT);

int nas_proc_detach(int switch_off);
int nas_proc_attach(void);
int nas_proc_get_attach_status(void);

int nas_proc_reset_pdn(int cid);
int nas_proc_set_pdn(int cid, int type, const char *apn, int ipv4_addr,
                     int emergency, int p_cscf, int im_cn_signal);
int nas_proc_get_pdn_range(void);
int nas_proc_get_pdn_status(int *cids, int *states, int n_pdn_max);
int nas_proc_get_pdn_param(int *cids, int *types, const char **apns,
                           int n_pdn_max);
int nas_proc_get_pdn_addr(int cid, int *cids, const char **addr1,
                          const char **addr2, int n_addr_max);
int nas_proc_deactivate_pdn(int cid);
int nas_proc_activate_pdn(int cid);
#endif

/*
 * --------------------------------------------------------------------------
 *      NAS procedures triggered by the network
 * --------------------------------------------------------------------------
 */

#ifdef NAS_UE
int nas_proc_cell_info(int found, tac_t tac, ci_t ci, AcT_t rat, UInt8_t rsrp,
                       UInt8_t rsrq);

int nas_proc_establish_cnf(const Byte_t *data, UInt32_t len);
int nas_proc_establish_rej(void);

int nas_proc_release_ind(int cause);

int nas_proc_ul_transfer_cnf(void);
int nas_proc_ul_transfer_rej(void);
int nas_proc_dl_transfer_ind(const Byte_t *data, UInt32_t len);
#endif

#ifdef NAS_MME
int nas_proc_establish_ind(UInt32_t ueid, tac_t tac, const Byte_t *data,
                           UInt32_t len);

int nas_proc_dl_transfer_cnf(UInt32_t ueid);
int nas_proc_dl_transfer_rej(UInt32_t ueid);
int nas_proc_ul_transfer_ind(UInt32_t ueid, const Byte_t *data, UInt32_t len);
#endif

/*
 * --------------------------------------------------------------------------
 *      NAS procedures triggered by the mme applicative layer
 * --------------------------------------------------------------------------
 */
#if defined(NAS_MME) && defined(EPC_BUILD)
int nas_proc_auth_param_res(emm_cn_auth_res_t *emm_cn_auth_res);
int nas_proc_auth_param_fail(emm_cn_auth_fail_t *emm_cn_auth_fail);
int nas_proc_deregister_ue(UInt32_t ue_id);
int nas_proc_pdn_connectivity_res(nas_pdn_connectivity_rsp_t *nas_pdn_connectivity_rsp);
int nas_proc_pdn_connectivity_fail(nas_pdn_connectivity_fail_t *nas_pdn_connectivity_fail);
#endif

#endif /* __NAS_PROC_H__*/

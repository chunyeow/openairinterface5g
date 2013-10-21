/*****************************************************************************
			Eurecom OpenAirInterface 3
			Copyright(c) 2012 Eurecom

Source		emm_proc.h

Version		0.1

Date		2012/10/16

Product		NAS stack

Subsystem	EPS Mobility Management

Author		Frederic Maurel

Description	Defines the EPS Mobility Management procedures executed at
		the EMM Service Access Points.

*****************************************************************************/
#ifndef __EMM_PROC_H__
#define __EMM_PROC_H__

#include "commonDef.h"
#include "OctetString.h"

#ifdef NAS_MME
#include "EmmCommon.h"
#include "emmData.h"
#endif

#ifdef NAS_UE
#include "LowerLayer.h"
#endif

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

/* Type of network attachment */
typedef enum {
    EMM_ATTACH_TYPE_EPS = 0,
    EMM_ATTACH_TYPE_IMSI,
    EMM_ATTACH_TYPE_EMERGENCY,
    EMM_ATTACH_TYPE_RESERVED,
} emm_proc_attach_type_t;

/* Type of network detach */
typedef enum {
    EMM_DETACH_TYPE_EPS = 0,
    EMM_DETACH_TYPE_IMSI,
    EMM_DETACH_TYPE_EPS_IMSI,
    EMM_DETACH_TYPE_REATTACH,
    EMM_DETACH_TYPE_NOT_REATTACH,
    EMM_DETACH_TYPE_RESERVED,
} emm_proc_detach_type_t;

/* Type of requested identity */
typedef enum {
    EMM_IDENT_TYPE_NOT_AVAILABLE = 0,
    EMM_IDENT_TYPE_IMSI,
    EMM_IDENT_TYPE_IMEI,
    EMM_IDENT_TYPE_IMEISV,
    EMM_IDENT_TYPE_TMSI
} emm_proc_identity_type_t;

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/*
 *---------------------------------------------------------------------------
 *				EMM status procedure
 *---------------------------------------------------------------------------
 */
int emm_proc_status_ind(unsigned int ueid, int emm_cause);
int emm_proc_status(unsigned int ueid, int emm_cause);

/*
 *---------------------------------------------------------------------------
 *				Lower layer procedure
 *---------------------------------------------------------------------------
 */
#ifdef NAS_UE
int emm_proc_lowerlayer_initialize(lowerlayer_success_callback_t success,
				   lowerlayer_failure_callback_t failure,
				   lowerlayer_release_callback_t release,
				   void* args);
int emm_proc_lowerlayer_success(void);
int emm_proc_lowerlayer_failure(int is_initial);
int emm_proc_lowerlayer_release(void);
#endif

/*
 *---------------------------------------------------------------------------
 *			    UE's Idle mode procedure
 *---------------------------------------------------------------------------
 */
#ifdef NAS_UE
int emm_proc_initialize(void);
int emm_proc_plmn_selection(int index);
int emm_proc_plmn_selection_end(int found, tac_t tac, ci_t ci, AcT_t rat);
#endif

/*
 * --------------------------------------------------------------------------
 * 				Attach procedure
 * --------------------------------------------------------------------------
 */
#ifdef NAS_UE
int emm_proc_attach(emm_proc_attach_type_t type);
int emm_proc_attach_request(void* args);
int emm_proc_attach_accept(long T3412, long T3402, long T3423, int n_tais, tai_t* tai, GUTI_t* guti, int n_eplmns, plmn_t* eplmn, const OctetString* esm_msg);
int emm_proc_attach_reject(int emm_cause, const OctetString* esm_msg);
int emm_proc_attach_complete(void* args);
int emm_proc_attach_failure(int is_initial, void* args);
int emm_proc_attach_release(void* args);
int emm_proc_attach_restart(void);

int emm_proc_attach_set_emergency(void);
int emm_proc_attach_set_detach(void);
#endif

#ifdef NAS_MME
int emm_proc_attach_request(unsigned int ueid, emm_proc_attach_type_t type, int native_ksi, int ksi, int native_guti, GUTI_t* guti, imsi_t* imsi, imei_t* imei, tai_t* tai, int eea, int eia, const OctetString* esm_msg);
int emm_proc_attach_reject(unsigned int ueid, int emm_cause);
int emm_proc_attach_complete(unsigned int ueid, const OctetString* esm_msg);
#endif

/*
 * --------------------------------------------------------------------------
 * 				Detach procedure
 * --------------------------------------------------------------------------
 */
#ifdef NAS_UE
int emm_proc_detach(emm_proc_detach_type_t type, int switch_off);
int emm_proc_detach_request(void* args);
int emm_proc_detach_accept(void);
int emm_proc_detach_failure(int is_initial, void* args);
int emm_proc_detach_release(void* args);
#endif

#ifdef NAS_MME
int emm_proc_detach(unsigned int ueid, emm_proc_detach_type_t type);
int emm_proc_detach_request(unsigned int ueid, emm_proc_detach_type_t type, int switch_off, int native_ksi, int ksi, GUTI_t* guti, imsi_t* imsi, imei_t* imei);
#endif

/*
 * --------------------------------------------------------------------------
 * 			    Identification procedure
 * --------------------------------------------------------------------------
 */
#ifdef NAS_UE
int emm_proc_identification_request(emm_proc_identity_type_t type);
#endif

#ifdef NAS_MME
int emm_proc_identification(unsigned int                   ueid,
                            emm_data_context_t            *emm_ctx,
                            emm_proc_identity_type_t       type,
                            emm_common_success_callback_t  success,
                            emm_common_reject_callback_t   reject,
                            emm_common_failure_callback_t  failure);
int emm_proc_identification_complete(unsigned int ueid, const imsi_t* imsi, const imei_t* imei, UInt32_t* tmsi);
#endif

/*
 * --------------------------------------------------------------------------
 * 			    Authentication procedure
 * --------------------------------------------------------------------------
 */
#ifdef NAS_UE
int emm_proc_authentication_request(int native_ksi, int ksi, const OctetString* rand, const OctetString* autn);
int emm_proc_authentication_reject(void);
int emm_proc_authentication_delete(void);
#endif

#ifdef NAS_MME
int emm_proc_authentication(unsigned int ueid, int ksi,
			    const OctetString* rand, const OctetString*  autn,
			    emm_common_success_callback_t success,
			    emm_common_reject_callback_t reject,
			    emm_common_failure_callback_t failure);
int emm_proc_authentication_complete(unsigned int ueid, int emm_cause, const OctetString* res);
#endif

/*
 * --------------------------------------------------------------------------
 * 			Security mode control procedure
 * --------------------------------------------------------------------------
 */
#ifdef NAS_UE
int emm_proc_security_mode_command(int native_ksi, int ksi, int seea, int seia, int reea, int reia);
#endif

#ifdef NAS_MME
int emm_proc_security_mode_control(unsigned int ueid, int ksi, int eea, int eia,
			    emm_common_success_callback_t success,
			    emm_common_reject_callback_t reject,
			    emm_common_failure_callback_t failure);
int emm_proc_security_mode_complete(unsigned int ueid);
int emm_proc_security_mode_reject(unsigned int ueid);
#endif

/*
 *---------------------------------------------------------------------------
 *			   Network indication handlers
 *---------------------------------------------------------------------------
 */
#ifdef NAS_UE
int emm_proc_registration_notify(Stat_t status);
int emm_proc_location_notify(tac_t tac, ci_t ci, AcT_t rat);
int emm_proc_network_notify(int index);
#endif

#endif /* __EMM_PROC_H__*/

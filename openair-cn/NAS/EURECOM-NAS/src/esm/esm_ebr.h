/*****************************************************************************
			Eurecom OpenAirInterface 3
			Copyright(c) 2012 Eurecom

Source		esm_ebr.h

Version		0.1

Date		2013/01/29

Product		NAS stack

Subsystem	EPS Session Management

Author		Frederic Maurel

Description	Defines functions used to handle state of EPS bearer contexts
		and manage ESM messages re-transmission.

*****************************************************************************/
#ifndef __ESM_EBR_H__
#define __ESM_EBR_H__

#include "OctetString.h"

#ifdef NAS_UE
#include "networkDef.h"
#endif

#include "nas_timer.h"
#include "EpsBearerIdentity.h"

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

/* Unassigned EPS bearer identity value */
#define ESM_EBI_UNASSIGNED	(EPS_BEARER_IDENTITY_UNASSIGNED)

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/

/* EPS bearer context states */
typedef enum {
    ESM_EBR_INACTIVE,		/* No EPS bearer context exists		*/
    ESM_EBR_ACTIVE,		/* The EPS bearer context is active,
				 * in the UE, in the network		*/
#ifdef NAS_MME
    ESM_EBR_INACTIVE_PENDING,	/* The network has initiated an EPS bearer
				 * context deactivation towards the UE	*/
    ESM_EBR_MODIFY_PENDING,	/* The network has initiated an EPS bearer
				 * context modification towards the UE	*/
    ESM_EBR_ACTIVE_PENDING,	/* The network has initiated an EPS bearer
				 * context activation towards the UE	*/
#endif
    ESM_EBR_STATE_MAX
} esm_ebr_state;

#ifdef NAS_MME
/* ESM message timer retransmission data */
typedef struct {
    unsigned int ueid;		/* Lower layers UE identifier		*/
    unsigned int ebi;		/* EPS bearer identity			*/
    unsigned int count;		/* Retransmission counter		*/
    OctetString msg;		/* Encoded ESM message to re-transmit	*/
} esm_ebr_timer_data_t;
#endif

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

int esm_ebr_is_reserved(int ebi);

#ifdef NAS_UE
void esm_ebr_initialize(esm_indication_callback_t cb);
int esm_ebr_assign(int ebi, int cid, int default_ebr);
int esm_ebr_release(int ebi);

int esm_ebr_set_status(int ebi, esm_ebr_state status, int ue_requested);
esm_ebr_state esm_ebr_get_status(int ebi);

int esm_ebr_is_not_in_use(int ebi);
#endif

#ifdef NAS_MME
void esm_ebr_initialize(void);
int esm_ebr_assign(unsigned int ueid, int ebi);
int esm_ebr_release(unsigned int ueid, int ebi);

int esm_ebr_start_timer(unsigned int ueid, int ebi, const OctetString* msg, long sec, nas_timer_callback_t cb);
int esm_ebr_stop_timer(unsigned int ueid, int ebi);

int esm_ebr_get_pending_ebi(unsigned int ueid, esm_ebr_state status);

int esm_ebr_set_status(unsigned int ueid, int ebi, esm_ebr_state status, int ue_requested);
esm_ebr_state esm_ebr_get_status(unsigned int ueid, int ebi);

int esm_ebr_is_not_in_use(unsigned int ueid, int ebi);
#endif

#endif /* __ESM_EBR_H__*/

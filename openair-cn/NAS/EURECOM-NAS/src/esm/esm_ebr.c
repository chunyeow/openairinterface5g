/*****************************************************************************
            Eurecom OpenAirInterface 3
            Copyright(c) 2012 Eurecom

Source      esm_ebr.c

Version     0.1

Date        2013/01/29

Product     NAS stack

Subsystem   EPS Session Management

Author      Frederic Maurel

Description Defines functions used to handle state of EPS bearer contexts
        and manage ESM messages re-transmission.

*****************************************************************************/

#include "esm_ebr.h"
#include "commonDef.h"
#include "nas_log.h"

#include "mme_api.h"

#include <stdlib.h> // malloc, free
#include <string.h> // memcpy

/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/

/*
 * Minimal and maximal value of an EPS bearer identity:
 * The EPS Bearer Identity (EBI) identifies a message flow
 */
#define ESM_EBI_MIN     (EPS_BEARER_IDENTITY_FIRST)
#define ESM_EBI_MAX     (EPS_BEARER_IDENTITY_LAST)

#ifdef NAS_UE
#define ESM_EBR_NB_UE_MAX   1
#endif
#ifdef NAS_MME
#define ESM_EBR_NB_UE_MAX   (MME_API_NB_UE_MAX + 1)
#endif

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

/* String representation of EPS bearer context status */
static const char *_esm_ebr_state_str[ESM_EBR_STATE_MAX] = {
    "BEARER CONTEXT INACTIVE",
    "BEARER CONTEXT ACTIVE",
#ifdef NAS_MME
    "BEARER CONTEXT INACTIVE PENDING",
    "BEARER CONTEXT MODIFY PENDING",
    "BEARER CONTEXT ACTIVE PENDING"
#endif
};

/*
 * -----------------------
 * EPS bearer context data
 * -----------------------
 */
typedef struct {
    unsigned char ebi;      /* EPS bearer identity          */
    esm_ebr_state status;   /* EPS bearer context status        */
#ifdef NAS_UE
    int is_default_ebr;     /* TRUE if the bearer context is associated
                 * to a default EPS bearer      */
    char cid;           /* Identifier of the PDN context the EPS
                 * bearer context has been assigned to  */
#endif
#ifdef NAS_MME
    struct nas_timer_t timer;   /* Retransmission timer         */
    esm_ebr_timer_data_t *args; /* Retransmission timer parameters data */
#endif
} esm_ebr_context_t;

/*
 * ----------------------------------
 * List of EPS bearer contexts per UE
 * ----------------------------------
 */
static struct {
    unsigned char index;    /* Index of the next EPS bearer context
                 * identity to be used */
#define ESM_EBR_DATA_SIZE (ESM_EBI_MAX - ESM_EBI_MIN + 1)
    esm_ebr_context_t *context[ESM_EBR_DATA_SIZE + 1];
} _esm_ebr_data[ESM_EBR_NB_UE_MAX];

/*
 * ----------------------
 * User notification data
 * ----------------------
 */
#ifdef NAS_UE
/* User notification callback executed whenever an EPS bearer context becomes
 * active or inactive */
static esm_indication_callback_t _esm_ebr_callback;
/* PDN connection and EPS bearer status [NW/UE][Dedicated/Default][status] */
static const network_pdn_state_t _esm_ebr_pdn_state[2][2][2] = {
    /* Status modification triggerer by the network */
    {
        /* Dedicated EPS bearer */
        {NET_PDN_NW_DEDICATED_DEACT, NET_PDN_NW_DEDICATED_ACT},
        /* Default EPS bearer */
        {NET_PDN_NW_DEFAULT_DEACT, 0}
    },
    /* Status modification triggered by the UE */
    {
        /* Dedicated EPS bearer */
        {NET_PDN_MT_DEDICATED_DEACT, NET_PDN_MT_DEDICATED_ACT},
        /* Default EPS bearer */
        {NET_PDN_MT_DEFAULT_DEACT, NET_PDN_MT_DEFAULT_ACT}
    }
};
#endif

/* Returns the index of the next available entry in the list of EPS bearer
 * context data */
static int _esm_ebr_get_available_entry(unsigned int ueid);

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:    esm_ebr_initialize()                                      **
 **                                                                        **
 ** Description: Initialize EPS bearer context data                        **
 **                                                                        **
 ** Inputs:  cb:        User notification callback                 **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    None                                       **
 **      Others:    _esm_ebr_data                              **
 **                                                                        **
 ***************************************************************************/
void esm_ebr_initialize(
#ifdef NAS_UE
    esm_indication_callback_t cb
#endif
)
{
    int ueid, i;
    LOG_FUNC_IN;

    for (ueid = 0; ueid < ESM_EBR_NB_UE_MAX; ueid++) {
        _esm_ebr_data[ueid].index = 0;
        /* Initialize EPS bearer context data */
        for (i = 0; i < ESM_EBR_DATA_SIZE + 1; i++) {
            _esm_ebr_data[ueid].context[i] = NULL;
        }
    }
#ifdef NAS_UE
    /* Initialize the user notification callback */
    _esm_ebr_callback = *cb;
#endif

    LOG_FUNC_OUT;
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_ebr_assign()                                          **
 **                                                                        **
 ** Description: Assigns a new EPS bearer context                          **
 **                                                                        **
 ** Inputs:  ueid:      Lower layers UE identifier                 **
 **      ebi:       Identity of the new EPS bearer context     **
 **      cid:       Identifier of the PDN context the EPS bea- **
 **             rer context is associated to               **
 **      default_ebr    TRUE if the new bearer context is associa- **
 **             ted to a default EPS bearer                **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    The identity of the new EPS bearer context **
 **             if successfully assigned;                  **
 **             the not assigned EBI (0) otherwise.        **
 **      Others:    _esm_ebr_data                              **
 **                                                                        **
 ***************************************************************************/
#ifdef NAS_UE
int esm_ebr_assign(int ebi, int cid, int default_ebr)
#endif
#ifdef NAS_MME
int esm_ebr_assign(unsigned int ueid, int ebi)
#endif
{
    LOG_FUNC_IN;

#ifdef NAS_UE
    unsigned int ueid = 0;
#endif

    int i;

    if (ueid >= ESM_EBR_NB_UE_MAX) {
        LOG_FUNC_RETURN (ESM_EBI_UNASSIGNED);
    }

    if (ebi != ESM_EBI_UNASSIGNED) {
        if ( (ebi < ESM_EBI_MIN) || (ebi > ESM_EBI_MAX) ) {
            LOG_FUNC_RETURN (ESM_EBI_UNASSIGNED);
        } else if (_esm_ebr_data[ueid].context[ebi - ESM_EBI_MIN] != NULL) {
            LOG_TRACE(WARNING, "ESM-FSM   - EPS bearer context already "
                      "assigned (ebi=%d)", ebi);
            LOG_FUNC_RETURN (ESM_EBI_UNASSIGNED);
        }
        /* The specified EPS bearer context is available */
        i = ebi - ESM_EBI_MIN;
    } else {
        /* Search for an available EPS bearer identity */
        i = _esm_ebr_get_available_entry(ueid);
        if (i < 0) {
            LOG_FUNC_RETURN(ESM_EBI_UNASSIGNED);
        }
        /* An available EPS bearer context is found */
        ebi = i + ESM_EBI_MIN;
    }

    /* Assign new EPS bearer context */
    _esm_ebr_data[ueid].context[i] =
        (esm_ebr_context_t *)malloc(sizeof(esm_ebr_context_t));
    if (_esm_ebr_data[ueid].context[i] == NULL) {
        LOG_FUNC_RETURN(ESM_EBI_UNASSIGNED);
    }
    /* Store the index of the next available EPS bearer identity */
    _esm_ebr_data[ueid].index = i + 1;

    /* Set the EPS bearer identity */
    _esm_ebr_data[ueid].context[i]->ebi = ebi;
    /* Set the EPS bearer context status to INACTIVE */
    _esm_ebr_data[ueid].context[i]->status = ESM_EBR_INACTIVE;
#ifdef NAS_UE
    /* Set the default EPS bearer indicator */
    _esm_ebr_data[ueid].context[i]->is_default_ebr = default_ebr;
    /* Set the identifier of the PDN context the EPS bearer is assigned to */
    _esm_ebr_data[ueid].context[i]->cid = cid;
#endif
#ifdef NAS_MME
    /* Disable the retransmission timer */
    _esm_ebr_data[ueid].context[i]->timer.id = NAS_TIMER_INACTIVE_ID;
    /* Setup retransmission timer parameters */
    _esm_ebr_data[ueid].context[i]->args = NULL;
#endif

    LOG_TRACE(INFO, "ESM-FSM   - EPS bearer context %d assigned", ebi);
    LOG_FUNC_RETURN(_esm_ebr_data[ueid].context[i]->ebi);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_ebr_release()                                         **
 **                                                                        **
 ** Description: Release the given EPS bearer identity                     **
 **                                                                        **
 ** Inputs:  ueid:      Lower layers UE identifier                 **
 **      ebi:       The identity of the EPS bearer context to  **
 **             be released                                **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok if the EPS bearer context has     **
 **             been successfully released;                **
 **             RETURNerror otherwise.                     **
 **      Others:    _esm_ebr_data                              **
 **                                                                        **
 ***************************************************************************/
int esm_ebr_release(
#ifdef NAS_MME
    unsigned int ueid,
#endif
    int ebi)
{
    LOG_FUNC_IN;

#ifdef NAS_UE
    unsigned int ueid = 0;
#endif

    if (ueid >= ESM_EBR_NB_UE_MAX) {
        LOG_FUNC_RETURN (RETURNerror);
    }
    if ( (ebi < ESM_EBI_MIN) || (ebi > ESM_EBI_MAX) ) {
        LOG_FUNC_RETURN (RETURNerror);
    }

    /* Get EPS bearer context data */
    esm_ebr_context_t *ctx = _esm_ebr_data[ueid].context[ebi - ESM_EBI_MIN];
    if ( (ctx == NULL) || (ctx->ebi != ebi) ) {
        /* EPS bearer context not assigned */
        LOG_FUNC_RETURN (RETURNerror);
    }
    /* Do not release active EPS bearer context */
    if (ctx->status != ESM_EBR_INACTIVE) {
        LOG_TRACE(ERROR, "ESM-FSM   - EPS bearer context is not INACTIVE");
        LOG_FUNC_RETURN (RETURNerror);
    }

#ifdef NAS_MME
    /* Stop the retransmission timer if still running */
    if (ctx->timer.id != NAS_TIMER_INACTIVE_ID) {
        LOG_TRACE(INFO, "ESM-FSM   - Stop retransmission timer %d",
                  ctx->timer.id);
        ctx->timer.id = nas_timer_stop(ctx->timer.id);
    }
    /* Release the retransmisison timer parameters */
    if (ctx->args) {
        if (ctx->args->msg.length > 0) {
            free(ctx->args->msg.value);
        }
        free(ctx->args);
        ctx->args = NULL;
    }
#endif

    /* Release EPS bearer context data */
    free(_esm_ebr_data[ueid].context[ebi - ESM_EBI_MIN]);
    _esm_ebr_data[ueid].context[ebi - ESM_EBI_MIN] = NULL;

    LOG_TRACE(INFO, "ESM-FSM   - EPS bearer context %d released", ebi);
    LOG_FUNC_RETURN (RETURNok);
}

#ifdef NAS_MME
/****************************************************************************
 **                                                                        **
 ** Name:    esm_ebr_start_timer()                                     **
 **                                                                        **
 ** Description: Start the timer of the specified EPS bearer context to    **
 **      expire after a given time interval. Timer expiration will **
 **      schedule execution of the callback function where stored  **
 **      ESM message should be re-transmit.                        **
 **                                                                        **
 ** Inputs:  ueid:      Lower layers UE identifier                 **
 **      ebi:       The identity of the EPS bearer             **
 **      msg:       The encoded ESM message to be stored       **
 **      sec:       The value of the time interval in seconds  **
 **      cb:        Function executed upon timer expiration    **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    _esm_ebr_data                              **
 **                                                                        **
 ***************************************************************************/
int esm_ebr_start_timer( unsigned int ueid, int ebi, const OctetString *msg,
                         long sec, nas_timer_callback_t cb)
{
    LOG_FUNC_IN;

    if (ueid >= ESM_EBR_NB_UE_MAX) {
        LOG_FUNC_RETURN (RETURNerror);
    }
    if ( (ebi < ESM_EBI_MIN) || (ebi > ESM_EBI_MAX) ) {
        LOG_FUNC_RETURN (RETURNerror);
    }

    /* Get EPS bearer context data */
    esm_ebr_context_t *ctx = _esm_ebr_data[ueid].context[ebi - ESM_EBI_MIN];
    if ( (ctx == NULL) || (ctx->ebi != ebi) ) {
        /* EPS bearer context not assigned */
        LOG_FUNC_RETURN (RETURNerror);
    }

    if (ctx->timer.id != NAS_TIMER_INACTIVE_ID) {
        if (ctx->args) {
            /* Re-start the retransmission timer */
            ctx->timer.id = nas_timer_restart(ctx->timer.id);
        }
    } else {
        /* Setup the retransmission timer parameters */
        ctx->args = (esm_ebr_timer_data_t *)malloc(sizeof(esm_ebr_timer_data_t));
        if (ctx->args) {
            /* Set the UE identifier */
            ctx->args->ueid = ueid;
            /* Set the EPS bearer identity */
            ctx->args->ebi = ebi;
            /* Reset the retransmission counter */
            ctx->args->count = 0;
            /* Set the ESM message to be re-transmited */
            ctx->args->msg.value = (uint8_t *)malloc(msg->length);
            ctx->args->msg.length = 0;
            if (ctx->args->msg.value) {
                memcpy(ctx->args->msg.value, msg->value, msg->length);
                ctx->args->msg.length = msg->length;
            }
            /* Setup the retransmission timer to expire at the given
             * time interval */
            ctx->timer.id = nas_timer_start(sec, cb, ctx->args);
            ctx->timer.sec = sec;
        }
    }

    if ( (ctx->args != NULL) && (ctx->timer.id != NAS_TIMER_INACTIVE_ID) ) {
        LOG_TRACE(INFO, "ESM-FSM   - Retransmission timer %d expires in "
                  "%ld seconds", ctx->timer.id, ctx->timer.sec);
        LOG_FUNC_RETURN (RETURNok);
    }
    LOG_FUNC_RETURN (RETURNerror);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_ebr_stop_timer()                                      **
 **                                                                        **
 ** Description: Stop the timer previously started for the given EPS bea-  **
 **      rer context                                               **
 **                                                                        **
 ** Inputs:  ueid:      Lower layers UE identifier                 **
 **      ebi:       The identity of the EPS bearer             **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    _esm_ebr_data                              **
 **                                                                        **
 ***************************************************************************/
int esm_ebr_stop_timer( unsigned int ueid, int ebi)
{
    LOG_FUNC_IN;

    if (ueid >= ESM_EBR_NB_UE_MAX) {
        LOG_FUNC_RETURN (RETURNerror);
    }
    if ( (ebi < ESM_EBI_MIN) || (ebi > ESM_EBI_MAX) ) {
        LOG_FUNC_RETURN (RETURNerror);
    }

    /* Get EPS bearer context data */
    esm_ebr_context_t *ctx = _esm_ebr_data[ueid].context[ebi - ESM_EBI_MIN];
    if ( (ctx == NULL) || (ctx->ebi != ebi) ) {
        /* EPS bearer context not assigned */
        LOG_FUNC_RETURN (RETURNerror);
    }

    /* Stop the retransmission timer if still running */
    if (ctx->timer.id != NAS_TIMER_INACTIVE_ID) {
        LOG_TRACE(INFO, "ESM-FSM   - Stop retransmission timer %d",
                  ctx->timer.id);
        ctx->timer.id = nas_timer_stop(ctx->timer.id);
    }

    /* Release the retransmisison timer parameters */
    if (ctx->args) {
        if (ctx->args->msg.length > 0) {
            free(ctx->args->msg.value);
        }
        free(ctx->args);
        ctx->args = NULL;
    }

    LOG_FUNC_RETURN (RETURNok);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_ebr_get_pending_ebi()                                 **
 **                                                                        **
 ** Description: Returns the EPS bearer identity assigned to the first EPS **
 **      bearer context entry which is pending in the given state  **
 **                                                                        **
 ** Inputs:  ueid:      Lower layers UE identifier                 **
 **      status:    The EPS bearer context status              **
 **      Others:    _esm_ebr_data                              **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    The EPS bearer identity of the EPS bearer  **
 **             context entry if it exists;                **
 **             the not assigned EBI (0) otherwise.        **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int esm_ebr_get_pending_ebi(unsigned int ueid, esm_ebr_state status)
{
    LOG_FUNC_IN;

    int i;
    for (i = 0; i < ESM_EBR_DATA_SIZE; i++) {
        if (_esm_ebr_data[ueid].context[i] == NULL) {
            continue;
        }
        if (_esm_ebr_data[ueid].context[i]->status != status) {
            continue;
        }
        /* EPS bearer context entry found */
        break;
    }

    if (i < ESM_EBR_DATA_SIZE) {
        LOG_FUNC_RETURN (_esm_ebr_data[ueid].context[i]->ebi);
    }
    /* EPS bearer context entry not found */
    LOG_FUNC_RETURN (ESM_EBI_UNASSIGNED);
}
#endif

/****************************************************************************
 **                                                                        **
 ** Name:    esm_ebr_set_status()                                      **
 **                                                                        **
 ** Description: Set the status of the specified EPS bearer context to the **
 **      given state                                               **
 **                                                                        **
 ** Inputs:  ueid:      Lower layers UE identifier                 **
 **      ebi:       The identity of the EPS bearer             **
 **      status:    The new EPS bearer context status          **
 **      ue_requested:  TRUE/FALSE if the modification of the EPS  **
 **             bearer context status was requested by the **
 **             UE/network                                 **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    _esm_ebr_data                              **
 **                                                                        **
 ***************************************************************************/
int esm_ebr_set_status(
#ifdef NAS_MME
    unsigned int ueid,
#endif
    int ebi, esm_ebr_state status, int ue_requested)
{
    LOG_FUNC_IN;

    esm_ebr_state old_status;

#ifdef NAS_UE
    unsigned int ueid = 0;
#endif

    if (ueid >= ESM_EBR_NB_UE_MAX) {
        LOG_FUNC_RETURN (RETURNerror);
    }
    if ( (ebi < ESM_EBI_MIN) || (ebi > ESM_EBI_MAX) ) {
        LOG_FUNC_RETURN (RETURNerror);
    }

    /* Get EPS bearer context data */
    esm_ebr_context_t *ctx = _esm_ebr_data[ueid].context[ebi - ESM_EBI_MIN];
    if ( (ctx == NULL) || (ctx->ebi != ebi) ) {
        /* EPS bearer context not assigned */
        LOG_TRACE(ERROR, "ESM-FSM   - EPS bearer context not assigned "
                  "(ebi=%d)", ebi);
        LOG_FUNC_RETURN (RETURNerror);
    }

    old_status = ctx->status;
    if (status < ESM_EBR_STATE_MAX) {
        LOG_TRACE(INFO, "ESM-FSM   - Status of EPS bearer context %d changed:"
                  " %s ===> %s", ebi,
                  _esm_ebr_state_str[old_status], _esm_ebr_state_str[status]);
        if (status != old_status) {
            ctx->status = status;
#ifdef NAS_UE
            /*
             * Notify the user that the state of the EPS bearer has changed
             */
            (*_esm_ebr_callback)(ctx->cid,
                                 _esm_ebr_pdn_state[ue_requested][ctx->is_default_ebr][status]);
#endif
            LOG_FUNC_RETURN (RETURNok);
        }
    }

    LOG_FUNC_RETURN (RETURNerror);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_ebr_get_status()                                      **
 **                                                                        **
 ** Description: Get the current status value of the specified EPS bearer  **
 **      context                                                   **
 **                                                                        **
 ** Inputs:  ueid:      Lower layers UE identifier                 **
 **      ebi:       The identity of the EPS bearer             **
 **      Others:    _esm_ebr_data                              **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    The current value of the EPS bearer con-   **
 **             text status                                **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
esm_ebr_state esm_ebr_get_status(
#ifdef NAS_MME
    unsigned int ueid,
#endif
    int ebi)
{
#ifdef NAS_UE
    unsigned int ueid = 0;
#endif

    if ( (ebi < ESM_EBI_MIN) || (ebi > ESM_EBI_MAX) ) {
        return (ESM_EBR_INACTIVE);
    }
    if (_esm_ebr_data[ueid].context[ebi - ESM_EBI_MIN] == NULL) {
        /* EPS bearer context not allocated */
        return (ESM_EBR_INACTIVE);
    }
    if (_esm_ebr_data[ueid].context[ebi - ESM_EBI_MIN]->ebi != ebi) {
        /* EPS bearer context not assigned */
        return (ESM_EBR_INACTIVE);
    }
    return (_esm_ebr_data[ueid].context[ebi - ESM_EBI_MIN]->status);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_ebr_is_reserved()                                     **
 **                                                                        **
 ** Description: Check whether the given EPS bearer identity is a reserved **
 **      value                                                     **
 **                                                                        **
 ** Inputs:  ebi:       The identity of the EPS bearer             **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    TRUE, FALSE                                **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int esm_ebr_is_reserved(int ebi)
{
    return ( (ebi != ESM_EBI_UNASSIGNED) && (ebi < ESM_EBI_MIN) );
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_ebr_is_not_in_use()                                   **
 **                                                                        **
 ** Description: Check whether the given EPS bearer identity does not      **
 **      match an assigned EBI value currently in use              **
 **                                                                        **
 ** Inputs:  ueid:      Lower layers UE identifier                 **
 **      ebi:       The identity of the EPS bearer             **
 **      Others:    _esm_ebr_data                              **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    TRUE, FALSE                                **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int esm_ebr_is_not_in_use(
#ifdef NAS_MME
    unsigned int ueid,
#endif
    int ebi)
{
#ifdef NAS_UE
    unsigned int ueid = 0;
#endif

    return ( (ebi == ESM_EBI_UNASSIGNED) ||
             (_esm_ebr_data[ueid].context[ebi - ESM_EBI_MIN] == NULL) ||
             (_esm_ebr_data[ueid].context[ebi - ESM_EBI_MIN]->ebi) != ebi);
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:    _esm_ebr_get_available_entry()                            **
 **                                                                        **
 ** Description: Returns the index of the next available entry in the list **
 **      of EPS bearer context data                                **
 **                                                                        **
 ** Inputs:  ueid:      Lower layers UE identifier                 **
 **      Others:    _esm_ebr_data                              **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    The index of the next available EPS bearer **
 **             context data entry; -1 if no any entry is  **
 **             available.                                 **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static int _esm_ebr_get_available_entry(unsigned int ueid)
{
    int i;
    for (i = _esm_ebr_data[ueid].index; i < ESM_EBR_DATA_SIZE; i++) {
        if (_esm_ebr_data[ueid].context[i] != NULL) {
            continue;
        }
        return i;
    }
    for (i = 0; i < _esm_ebr_data[ueid].index; i++) {
        if (_esm_ebr_data[ueid].context[i] != NULL) {
            continue;
        }
        return i;
    }
    /* No available EBI entry found */
    return (-1);
}

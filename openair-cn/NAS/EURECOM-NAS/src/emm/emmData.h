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
Source      emmData.h

Version     0.1

Date        2012/10/18

Product     NAS stack

Subsystem   EPS Mobility Management

Author      Frederic Maurel

Description Defines internal private data handled by EPS Mobility
        Management sublayer.

*****************************************************************************/
#ifndef __EMMDATA_H__
#define __EMMDATA_H__

#include "commonDef.h"
#include "networkDef.h"
#include "securityDef.h"

#include "OctetString.h"
#include "nas_timer.h"

#include "esmData.h"

#ifdef NAS_MME
#include "emm_fsm.h"
#include "mme_api.h"
# if defined(EPC_BUILD)
#   include "tree.h"
# endif
#endif

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

#ifdef NAS_UE
/*
 * The name of the file used as non-volatile memory device to store
 * persistent EMM data when the UE is switched off
 */
#define EMM_NVRAM_FILENAME  ".ue_emm.nvram"

/*
 * The name of the environment variable which defines the directory
 * where the EMM data file is located
 */
#define EMM_NVRAM_DIRNAME   "NVRAM_DIR"

/* Network selection modes of operation */
#define EMM_DATA_PLMN_AUTO  NET_PLMN_AUTO
#define EMM_DATA_PLMN_MANUAL    NET_PLMN_MANUAL

#endif // NAS_UE

/* Checks Mobile Country Code equality */
#define MCCS_ARE_EQUAL(n1, n2)  (((n1).MCCdigit1 == (n2).MCCdigit1) && \
                                 ((n1).MCCdigit2 == (n2).MCCdigit2) && \
                                 ((n1).MCCdigit3 == (n2).MCCdigit3))

/* Checks Mobile Network Code equality */
#define MNCS_ARE_EQUAL(n1, n2)  (((n1).MNCdigit1 == (n2).MNCdigit1) &&  \
                                 ((n1).MNCdigit2 == (n2).MNCdigit2) &&  \
                                 ((n1).MNCdigit3 == (n2).MNCdigit3))

/* Checks PLMNs equality */
#define PLMNS_ARE_EQUAL(p1, p2) ((MCCS_ARE_EQUAL((p1),(p2))) && \
                                 (MNCS_ARE_EQUAL((p1),(p2))))

/* Checks TAIs equality */
#define TAIS_ARE_EQUAL(t1, t2)  ((PLMNS_ARE_EQUAL((t1).plmn,(t2).plmn)) && \
                                 ((t1).tac == (t2).tac))

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/

/*
 * --------------------------------------------------------------------------
 * EPS NAS security context handled by EPS Mobility Management sublayer in
 * the UE and in the MME
 * --------------------------------------------------------------------------
 */
/* Type of security context */
typedef enum {
    EMM_KSI_NOT_AVAILABLE = 0,
    EMM_KSI_NATIVE,
    EMM_KSI_MAPPED
} emm_ksi_t;

/* EPS NAS security context structure */
typedef struct {
    emm_ksi_t type;     /* Type of security context        */
    int eksi;           /* NAS key set identifier for E-UTRAN      */
    OctetString kasme;      /* ASME security key (native context)      */
    //OctetString ksgsn;    /* SGSN security key (mapped context)      */
    OctetString knas_enc;   /* NAS cyphering key               */
    OctetString knas_int;   /* NAS integrity key               */
    struct {
        UInt32_t spare:8;
        UInt32_t overflow:16;
        UInt32_t seq_num:8;
    } dl_count, ul_count;   /* Downlink and uplink count parameters    */
    struct {
        UInt8_t eps_encryption;   /* algorithm used for ciphering            */
        UInt8_t eps_integrity;    /* algorithm used for integrity protection */
        UInt8_t umts_encryption;  /* algorithm used for ciphering            */
        UInt8_t umts_integrity;   /* algorithm used for integrity protection */
        UInt8_t gprs_encryption;  /* algorithm used for ciphering            */
        UInt8_t umts_present:1;
        UInt8_t gprs_present:1;
    } capability;       /* UE network capability           */
    struct {
        UInt8_t encryption:4;   /* algorithm used for ciphering           */
        UInt8_t integrity:4;    /* algorithm used for integrity protection */
    } selected_algorithms;       /* MME selected algorithms                */
} emm_security_context_t;

/*
 * --------------------------------------------------------------------------
 *  EMM internal data handled by EPS Mobility Management sublayer in the UE
 * --------------------------------------------------------------------------
 */
#ifdef NAS_UE
/*
 * EPS update status
 * -----------------
 * The EPS update status value is changed only after the execution of an attach
 * or combined attach, network initiated detach, authentication, tracking area
 * update or combined tracking area update, service request or paging for EPS
 * services using IMSI procedure.
 *
 * EU1: The last attach or tracking area updating attempt was successful.
 * EU2: The last attach, service request or tracking area updating attempt
 *  failed procedurally, i.e. no response or reject message was received
 *  from the MME.
 * EU3: The last attach, service request or tracking area updating attempt
 *  was correctly performed, but the answer from the MME was negative
 *  (because of roaming or subscription restrictions).
 */
typedef enum {
    EU1_UPDATED,
    EU2_NOT_UPDATED,
    EU3_ROAMING_NOT_ALLOWED
} emm_eps_update_t;

/*
 * EPS Connection Management states
 * --------------------------------
 * ECM-IDLE:      No NAS signalling connection between UE and network exists.
 *        No UE context exists in the network.
 * ECM-CONNECTED: The signalling connection is established between the UE
 *        and the MME (RRC connection and S1_MME connection).
 */
typedef enum {
    ECM_IDLE,
    ECM_CONNECTED
} emm_ecm_state_t;

/*
 * Structure of the EMM parameters stored in the UE's non-volatile memory
 * ----------------------------------------------------------------------
 * These EMM parameters can only be used if the IMSI from the USIM matches
 * the IMSI stored in the non-volatile memory; else the UE shall delete the
 * EMM parameters.
 *
 * - EPLMN: List of equivalent PLMNs - Updated at the end of each attach or
 *   combined attach or tracking area updating or combined tracking area
 *   updating procedure. Deleted if the USIM is removed or when the UE attached
 *   for emergency bearer services enters the state EMM-DEREGISTERED.
 *   These PLMNs shall be regarded by the UE as equivalent to each other
 *   for PLMN selection, cell selection/re-selection and handover.
 *   The maximum number of possible entries in the stored list is 16.
 */
typedef struct {
    imsi_t imsi;
    plmn_t rplmn;   /* The registered PLMN  */
    /* List of equivalent PLMNs         */
#define EMM_DATA_EPLMN_MAX  16
    PLMN_LIST_T(EMM_DATA_EPLMN_MAX) eplmn;
} emm_nvdata_t;

/*
 * Structure of the EMM data
 * -------------------------
 */
typedef struct {
    int                 usim_is_valid;  /* Indication of USIM data validity   */

    imei_t             *imei;   /* IMEI read from the UE's non-volatile memory*/
    const imsi_t       *imsi; /* The valid IMSI read from the USIM            */
    GUTI_t             *guti;   /* The valid GUTI read from the USIM          */
    tai_t              *tai;    /* Last visited registered Tracking Area Id   */

    emm_eps_update_t    status;    /* The current EPS update status           */
    emm_ecm_state_t     ecm_status; /* The EPS Connection Management status   */
    int                 is_attached;    /* Network attachment indicator       */
    int                 is_emergency;   /* Emergency bearer services indicator*/

    /* Tracking Areas list the UE is registered to
     * Contains the list of TAIs that identify the tracking areas that
     * the UE can enter without performing a tracking area updating
     * procedure. The TAIs in a TAI list assigned by an MME to a UE
     * pertain to the same MME area.
     */
#define EMM_DATA_TAI_MAX        16
    TAI_LIST_T(EMM_DATA_TAI_MAX) ltai;

    int                 plmn_mode;  /* Network selection operating mode       */
    int                 plmn_index; /* Manually selected PLMN                 */
    int                 plmn_rat; /* Manually selected Radio Access Technology*/

    plmn_t              splmn;      /* The currently selected PLMN            */
    int                 is_rplmn;   /* splmn is the registered PLMN           */
    int                 is_eplmn; /* splmn is in the list of equivalent PLMNs */
    Stat_t              stat;    /* Current network registration status       */
    tac_t               tac;   /* Tracking area code                          */
    ci_t                ci;    /* GERAN/UTRAN/E-UTRAN serving cell identifier */
    AcT_t               rat; /* Radio Access Technology of the serving cell   */

    /* An octet string representation of operators present in the network */
    struct {
#define EMM_DATA_BUFFER_SIZE    2048
        char buffer[EMM_DATA_BUFFER_SIZE+1];
    } plist;

    /*
     * Data used for PLMN selection procedure
     * --------------------------------------
     */
    plmn_t hplmn;       /* The Home PLMN derived from the IMSI  */
    /* List of Forbidden PLMNs
    * Contains the list of PLMN identities for which a Location
    * Registration has been rejected with EMM cause code #11 (PLMN
    * not allowed). A PLMN is removed from this list if, after a
    * subsequent manual selection of that PLMN, there is a successful
    * Location Request.                */
#define EMM_DATA_FPLMN_MAX  4
    PLMN_LIST_T(EMM_DATA_FPLMN_MAX) fplmn;
    /* List of Forbidden PLMNs for GPRS service
    * Contains the list of PLMN identities for which an Attach Request
    * has been rejected with EMM cause code #14 (GPRS/EPS services not
    * allowed in this PLMN). A PLMN is removed from this list if, after
    * a subsequent manual selection of that PLMN, there is a successful
    * GPRS attach or EPS attach.           */
#define EMM_DATA_FPLMN_GPRS_MAX 4
    PLMN_LIST_T(EMM_DATA_FPLMN_GPRS_MAX) fplmn_gprs;
    /* List of Equivalent HPLMNs            */
#define EMM_DATA_EHPLMN_MAX 4
    PLMN_LIST_T(EMM_DATA_EHPLMN_MAX) ehplmn;
    /* List of user controlled PLMNs        */
#define EMM_DATA_PLMN_MAX   4
    PLMN_LIST_T(EMM_DATA_PLMN_MAX) plmn;
    UInt16_t userAcT[EMM_DATA_PLMN_MAX];
    /* List of operator controlled PLMNs        */
#define EMM_DATA_OPLMN_MAX  4
    PLMN_LIST_T(EMM_DATA_OPLMN_MAX) oplmn;
    UInt16_t operAcT[EMM_DATA_OPLMN_MAX];
    /* List of operator network name records    */
#define EMM_DATA_OPNN_MAX   16
    UInt8_t n_opnns;
    struct {
        const plmn_t *plmn;
        const char *fullname;
        const char *shortname;
    } opnn[EMM_DATA_OPNN_MAX];

    /*
     * Data used for roaming service
     * -----------------------------
     */
    /* List of Forbidden Tracking Areas
    * Contains the list of TAIs for which an attach request has been
    * rejected with EMM cause code #12 (tracking area not allowed).
    */
#define EMM_DATA_FTAI_MAX       40
    TAI_LIST_T(EMM_DATA_FTAI_MAX) ftai;
    /* List of Forbidden Tracking Areas for roaming
    * Contains the list of TAIs for which an attach request has been
    * rejected with EMM cause code #13 (roaming not allowed in this
    * tracking area).
    */
#define EMM_DATA_FTAI_ROAMING_MAX   40
    TAI_LIST_T(EMM_DATA_FTAI_ROAMING_MAX) ftai_roaming;

    /*
     * NAS configuration parameters
     * ----------------------------
     * see "Non Access Stratum Configuration" in USIM API header file
     */
    Byte_t NAS_SignallingPriority;
    Byte_t NMO_I_Behaviour;
    Byte_t AttachWithImsi;
    Byte_t MinimumPeriodicSearchTimer;
    Byte_t ExtendedAccessBarring;
    Byte_t Timer_T3245_Behaviour;

    /*
     * EMM data stored in the UE's memory
     * ----------------------------------
     */
    emm_nvdata_t nvdata;

    /*
     * EPS NAS security context
     * ------------------------
     */
    emm_security_context_t *security;    /* current security context     */
    emm_security_context_t *non_current; /* non-current security context */

} emm_data_t;

#endif // NAS_UE

/*
 * --------------------------------------------------------------------------
 *  EMM internal data handled by EPS Mobility Management sublayer in the MME
 * --------------------------------------------------------------------------
 */
#ifdef NAS_MME
/*
 * Structure of the EMM context established by the network for a particular UE
 * ---------------------------------------------------------------------------
 */
typedef struct emm_data_context_s {
#if defined(EPC_BUILD)
    RB_ENTRY(emm_data_context_s) entries;
#endif

    unsigned int ueid;        /* UE identifier                                   */
    int          is_dynamic;  /* Dynamically allocated context indicator         */
    int          is_attached; /* Attachment indicator                            */
    int          is_emergency;/* Emergency bearer services indicator             */

    imsi_t      *imsi;        /* The IMSI provided by the UE or the MME          */
    imei_t      *imei;        /* The IMEI provided by the UE                     */
    int          guti_is_new; /* New GUTI indicator                              */
    GUTI_t      *guti;        /* The GUTI assigned to the UE                     */
    GUTI_t      *old_guti;    /* The old GUTI                                    */
    int          n_tacs;      /* Number of consecutive tracking areas the UE is
                               * registered to                                   */
    tac_t       tac;          /* Code of the first tracking area the UE is
                               * registered to                                   */

    int         ksi;          /* Security key set identifier provided by the UE  */
    int         eea;          /* EPS encryption algorithms supported by the UE   */
    int         eia;          /* EPS integrity algorithms supported by the UE    */
    int         ucs2;         /* UCS2 Alphabet*/
    int         uea;          /* UMTS encryption algorithms supported by the UE  */
    int         uia;          /* UMTS integrity algorithms supported by the UE   */
    int         gea;          /* GPRS encryption algorithms supported by the UE  */
    int         umts_present; /* For encoding ue network capabilities (variable size)*/
    int         gprs_present; /* For encoding ue network capabilities (variable size)*/

    auth_vector_t vector;/* EPS authentication vector                            */
    emm_security_context_t *security;    /* Current EPS NAS security context     */
    OctetString esm_msg;      /* ESM message contained within the initial request*/
    int         emm_cause;    /* EMM failure cause code                          */

    emm_fsm_state_t    _emm_fsm_status;

    esm_data_context_t esm_data_ctx;
} emm_data_context_t;

/*
 * Structure of the EMM data
 * -------------------------
 */
typedef struct {
    /*
     * MME configuration
     * -----------------
     */
    mme_api_emm_config_t conf;
    /*
     * EMM contexts
     * ------------
     */
# if defined(EPC_BUILD)
    /* Use a tree for ue data context within MME */
    RB_HEAD(emm_data_context_map, emm_data_context_s) ctx_map;
# else
#   define EMM_DATA_NB_UE_MAX   (MME_API_NB_UE_MAX + 1)
    emm_data_context_t *ctx [EMM_DATA_NB_UE_MAX];
# endif
} emm_data_t;

struct emm_data_context_s *emm_data_context_get(
    emm_data_t *_emm_data, unsigned int _ueid);

struct emm_data_context_s *emm_data_context_remove(
    emm_data_t *_emm_data, struct emm_data_context_s *elm);

void emm_data_context_add(emm_data_t *emm_data, struct emm_data_context_s *elm);

#endif // NAS_MME

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

/*
 * --------------------------------------------------------------------------
 *      EPS mobility management data (used within EMM only)
 * --------------------------------------------------------------------------
 */
emm_data_t _emm_data;

/*
 * --------------------------------------------------------------------------
 *      EPS mobility management timers – UE side
 * --------------------------------------------------------------------------
 */
#ifdef NAS_UE
#define T3402_DEFAULT_VALUE 720 /* 12 minutes   */
#define T3410_DEFAULT_VALUE 15  /* 15 seconds   */
#define T3411_DEFAULT_VALUE 10  /* 10 seconds   */
#define T3412_DEFAULT_VALUE 3240    /* 54 minutes   */
#define T3416_DEFAULT_VALUE 30  /* 30 seconds   */
#define T3417_DEFAULT_VALUE 5   /* 5 seconds    */
#define T3417ext_DEFAULT_VALUE  10  /* 10 seconds   */
#define T3418_DEFAULT_VALUE 20  /* 20 seconds   */
#define T3420_DEFAULT_VALUE 15  /* 15 seconds   */
#define T3421_DEFAULT_VALUE 15  /* 15 seconds   */
#define T3423_DEFAULT_VALUE T3412_DEFAULT_VALUE
#define T3430_DEFAULT_VALUE 15  /* 15 seconds   */
#define T3440_DEFAULT_VALUE 10  /* 10 seconds   */

struct nas_timer_t T3402;   /* attach failure timer         */
struct nas_timer_t T3410;   /* attach timer             */
struct nas_timer_t T3411;   /* attach restart timer         */
struct nas_timer_t T3412;   /* periodic tracking area update timer  */
struct nas_timer_t T3416;   /* EPS authentication challenge timer   */
struct nas_timer_t T3417;   /* Service request timer        */
struct nas_timer_t T3418;   /* MAC authentication failure timer */
struct nas_timer_t T3420;   /* Synch authentication failure timer   */
struct nas_timer_t T3421;   /* Detach timer             */
struct nas_timer_t T3430;   /* tracking area update timer       */

struct nas_timer_t T3423;   /* E-UTRAN deactivate ISR timer     */
#endif

/*
 * --------------------------------------------------------------------------
 *      EPS mobility management timers – Network side
 * --------------------------------------------------------------------------
 */
#ifdef NAS_MME
#define T3450_DEFAULT_VALUE 6   /* 6 seconds    */
#define T3460_DEFAULT_VALUE 6   /* 6 seconds    */
#define T3470_DEFAULT_VALUE 6   /* 6 seconds    */

struct nas_timer_t T3450;   /* EMM message retransmission timer */
struct nas_timer_t T3460;   /* Authentication timer         */
struct nas_timer_t T3470;   /* Identification timer         */

/*
 * mobile reachable timer
 * ----------------------
 * The network supervises the periodic tracking area updating procedure
 * of the UE by means of the mobile reachable timer.
 * If the UE is not attached for emergency bearer services, the mobile
 * reachable timer is 4 minutes greater than T3412.
 * If the UE is attached for emergency bearer services, the MME shall
 * set the mobile reachable timer with a value equal to T3412. When
 * the mobile reachable timer expires, the MME shall locally detach the UE.
 *
 * The mobile reachable timer shall be reset and started, when the MME
 * releases the NAS signalling connection for the UE. The mobile reachable
 * timer shall be stopped when a NAS signalling connection is established
 * for the UE.
 */

/*
 * implicit detach timer
 * ---------------------
 * If ISR is activated, the default value of the implicit detach timer is
 * 4 minutes greater than T3423.
 * If the implicit detach timer expires before the UE contacts the network,
 * the network shall implicitly detach the UE.
 * If the MME includes timer T3346 in the TRACKING AREA UPDATE REJECT message
 * or the SERVICE REJECT message and T3346 is greater than T3412, the MME
 * sets the mobile reachable timer and the implicit detach timer such that
 * the sum of the timer values is greater than T3346.
 *
 * Upon expiry of the mobile reachable timer the network shall start the
 * implicit detach timer. The implicit detach timer shall be stopped when
 * a NAS signalling connection is established for the UE.
 */
#endif

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

#endif /* __EMMDATA_H__*/

#include "../SGW-LITE/sgw_lite_ie_defs.h"

#ifndef SGW_LITE_MESSAGES_TYPES_H_
#define SGW_LITE_MESSAGES_TYPES_H_

typedef struct {
    Teid_t           teid;               ///< Tunnel Endpoint Identifier
    Imsi_t           imsi;
    Msisdn_t         msisdn;
    Mei_t            mei;
    Uli_t            uli;
    ServingNetwork_t serving_network;
    RatType_t        rat_type;
    FTeid_t          sender_fteid_for_cp; ///< Sender F-TEID for control plane
    FTeid_t          pgw_address_for_cp;  ///< PGW S5/S8 address for control plane or PMIP
    APN_t            apn;                 ///< Access Point Name
    SelectionMode_t  selection_mode;      ///< Selection Mode
    PDNType_t        pdn_type;            ///< PDN Type
    PAA_t            paa;                 ///< PDN Address Allocation
    /* Shall include APN Restriction but not used in our case */
    AMBR_t           ambr;                ///< Aggregate Maximum Bit Rate
    /* TODO: add Protocol Configuration otions */
    FQ_CSID_t        mme_fq_csid;
    UETimeZone_t     ue_time_zone;
    UCI_t            uci;                 ///< User CSG Information
    struct {
        unsigned         eps_bearer_id:4;   ///< EPS Bearer ID
        BearerQOS_t      bearer_level_qos;
    } bearer_to_create;                   ///< Bearer Contexts to be created
} SgwCreateSessionRequest;

typedef struct {
    Teid_t           teid;               ///< Tunnel Endpoint Identifier
    PAA_t            paa;                 ///< PDN Address Allocation
    APNRestriction_t apn_restriction;
    struct {
        unsigned         eps_bearer_id:4;   ///< EPS Bearer ID
        SGWCause_t       cause;
        FTeid_t          s1u_sgw_fteid;   ///< Sender F-TEID for control plane
    } bearer_context_created;
    FQ_CSID_t        pgw_fq_csid;
    FQ_CSID_t        sgw_fq_csid;
} SgwCreateSessionResponse;

typedef struct {
    Teid_t           teid;               ///< Tunnel Endpoint Identifier
    /* Delay Value in integer multiples of 50 millisecs, or zero */
    DelayValue_t     delay_dl_packet_notif_req;
    struct {
        unsigned         eps_bearer_id:4;   ///< EPS Bearer ID
        FTeid_t          s1_eNB_fteid;      ///< S1 eNodeB F-TEID
    } bearer_context_to_modify;
    FQ_CSID_t        mme_fq_csid;
} SgwModifyBearerRequest;

typedef struct {
    Teid_t           teid;               ///< Tunnel Endpoint Identifier
    
} SgwModifyBearerResponse;

typedef struct {
    uint8_t  status;    ///< Status of S1U endpoint creation (Failed = 0xFF or Success = 0x0)
    uint32_t teid;
} SgwS1UEndpointCreated;

typedef struct {
    Teid_t      teid;                   ///< Tunnel Endpoint Identifier
    EBI_t       lbi;                    ///< Linked EPS Bearer ID
    FTeid_t     sender_fteid_for_cp;    ///< Sender F-TEID for control plane
} SgwDeleteSessionRequest;

typedef struct {
    
} SgwDeleteBearerRequest;

#endif

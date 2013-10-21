#ifndef SGI_FORWARD_MESSAGES_TYPES_H_
#define SGI_FORWARD_MESSAGES_TYPES_H_

typedef enum SGIStatus_e {
    SGI_STATUS_OK               = 0,
    SGI_STATUS_ERROR_CONTEXT_ALREADY_EXIST       = 50,
    SGI_STATUS_ERROR_CONTEXT_NOT_FOUND           = 51,
    SGI_STATUS_ERROR_INVALID_MESSAGE_FORMAT      = 52,
    SGI_STATUS_ERROR_SERVICE_NOT_SUPPORTED       = 53,
    SGI_STATUS_ERROR_SYSTEM_FAILURE              = 54,
    SGI_STATUS_ERROR_NO_RESOURCES_AVAILABLE      = 55,
    SGI_STATUS_ERROR_NO_MEMORY_AVAILABLE         = 56,
    SGI_STATUS_MAX,
} SGIStatus_t;


typedef struct {
    Teid_t           context_teid;        ///< Tunnel Endpoint Identifier S11
    Teid_t           sgw_S1u_teid;        ///< Tunnel Endpoint Identifier S1-U
    ebi_t            eps_bearer_id;       ///< EPS bearer identifier
    pdn_type_t       pdn_type;            ///< PDN Type
    PAA_t            paa;                 ///< PDN Address Allocation
} SGICreateEndpointReq;

typedef struct {
	SGIStatus_t      status;              ///< Status of  endpoint creation (Failed = 0xFF or Success = 0x0)
    Teid_t           context_teid;        ///< Tunnel Endpoint Identifier S11
    Teid_t           sgw_S1u_teid;        ///< Tunnel Endpoint Identifier S1-U
    ebi_t            eps_bearer_id;       ///< EPS bearer identifier
    pdn_type_t       pdn_type;            ///< PDN Type
    PAA_t            paa;                 ///< PDN Address Allocation
} SGICreateEndpointResp;

typedef struct {
    Teid_t           context_teid;        ///< Tunnel Endpoint Identifier S11
    Teid_t           sgw_S1u_teid;        ///< Tunnel Endpoint Identifier S1-U
    Teid_t           enb_S1u_teid;        ///< Tunnel Endpoint Identifier S1-U
    ebi_t            eps_bearer_id;       ///< EPS bearer identifier
} SGIUpdateEndpointReq;

typedef struct {
	SGIStatus_t      status;              ///< Status of  endpoint creation (Failed = 0xFF or Success = 0x0)
    Teid_t           context_teid;        ///< Tunnel Endpoint Identifier S11
    Teid_t           sgw_S1u_teid;        ///< Tunnel Endpoint Identifier S1-U
    Teid_t           enb_S1u_teid;        ///< Tunnel Endpoint Identifier S1-U
    ebi_t            eps_bearer_id;       ///< EPS bearer identifier
} SGIUpdateEndpointResp;


typedef struct {
    Teid_t           context_teid;        ///< Tunnel Endpoint Identifier S11
    Teid_t           sgw_S1u_teid;        ///< Tunnel Endpoint Identifier S1-U
    ebi_t            eps_bearer_id;       ///< EPS bearer identifier
    pdn_type_t       pdn_type;            ///< PDN Type
    PAA_t            paa;                 ///< PDN Address Allocation
} SGIDeleteEndpointReq;

typedef struct {
	SGIStatus_t      status;              ///< Status of  endpoint deletion (Failed = 0xFF or Success = 0x0)
    Teid_t           context_teid;        ///< Tunnel Endpoint Identifier S11
    Teid_t           sgw_S1u_teid;        ///< Tunnel Endpoint Identifier S1-U
    ebi_t            eps_bearer_id;       ///< EPS bearer identifier
    pdn_type_t       pdn_type;            ///< PDN Type
    PAA_t            paa;                 ///< PDN Address Allocation
} SGIDeleteEndpointResp;

#endif /* SGI_FORWARD_MESSAGES_TYPES_H_ */

#ifndef S1AP_MESSAGES_TYPES_H_
#define S1AP_MESSAGES_TYPES_H_

//-------------------------------------------------------------------------------------------//
// Defines to access message fields.

#define S1AP_NAS_FIRST_REQ(mSGpTR)              (mSGpTR)->msg.s1ap_nas_first_req
#define S1AP_UPLINK_NAS(mSGpTR)                 (mSGpTR)->msg.s1ap_uplink_nas
#define S1AP_UE_CAPABILITIES_IND(mSGpTR)        (mSGpTR)->msg.s1ap_ue_cap_info_ind
#define S1AP_INITIAL_CONTEXT_SETUP_RESP(mSGpTR) (mSGpTR)->msg.s1ap_initial_context_setup_resp

#define S1AP_DOWNLINK_NAS(mSGpTR)               (mSGpTR)->msg.s1ap_downlink_nas
#define S1AP_INITIAL_CONTEXT_SETUP_REQ(mSGpTR)  (mSGpTR)->msg.s1ap_initial_context_setup_req

//-------------------------------------------------------------------------------------------//

typedef enum cell_type_e {
    CELL_MACRO_ENB,
    CELL_HOME_ENB
} cell_type_t;

typedef enum paging_drx_e {
    PAGING_DRX_32  = 0x0,
    PAGING_DRX_64  = 0x1,
    PAGING_DRX_128 = 0x2,
    PAGING_DRX_256 = 0x3,
} paging_drx_t;

typedef struct net_ip_address_s {
    unsigned ipv4:1;
    unsigned ipv6:1;
    char ipv4_address[16];
    char ipv6_address[40];
} net_ip_address_t;

typedef uint64_t bitrate_t;

typedef struct {
    bitrate_t br_ul;
    bitrate_t br_dl;
} ambr_t;

typedef enum priority_level_s {
    PRIORITY_LEVEL_SPARE       = 0,
    PRIORITY_LEVEL_HIGHEST     = 1,
    PRIORITY_LEVEL_LOWEST      = 14,
    PRIORITY_LEVEL_NO_PRIORITY = 15,
} priority_level_t;

typedef enum {
    PRE_EMPTION_CAPABILITY_ENABLED  = 0,
    PRE_EMPTION_CAPABILITY_DISABLED = 1,
    PRE_EMPTION_CAPABILITY_MAX,
} pre_emp_capability_t;

typedef enum {
    PRE_EMPTION_VULNERABILITY_ENABLED  = 0,
    PRE_EMPTION_VULNERABILITY_DISABLED = 1,
    PRE_EMPTION_VULNERABILITY_MAX,
} pre_emp_vulnerability_t;

typedef struct {
    priority_level_t        priority_level;
    pre_emp_capability_t    pre_emp_capability;
    pre_emp_vulnerability_t pre_emp_vulnerability;
} allocation_retention_priority_t;

typedef struct security_capabilities_s {
    uint16_t encryption_algorithms;
    uint16_t integrity_algorithms;
} security_capabilities_t;

/* Maximum number of e-rabs to be setup/deleted in a single message.
 * Even if only one bearer will be modified by message.
 */
#define S1AP_MAX_E_RAB  11

/* Length of the transport layer address string
 * 160 bits / 8 bits by char.
 */
#define S1AP_TRANSPORT_LAYER_ADDRESS_SIZE (160 / 8)

#define S1AP_MAX_NB_MME_IP_ADDRESS 10

/* Security key length used within eNB
 * Even if only 16 bytes will be effectively used,
 * the key length is 32 bytes (256 bits)
 */
#define SECURITY_KEY_LENGTH 32

/* Provides the establishment cause for the RRC connection request as provided
 * by the upper layers. W.r.t. the cause value names: highPriorityAccess
 * concerns AC11..AC15, ‘mt’ stands for ‘Mobile Terminating’ and ‘mo’ for
 * 'Mobile Originating'. Defined in TS 36.331.
 */
typedef enum rrc_establishment_cause_e {
    RRC_CAUSE_EMERGENCY        = 0x0,
    RRC_CAUSE_HIGH_PRIO_ACCESS = 0x1,
    RRC_CAUSE_MT_ACCESS        = 0x2,
    RRC_CAUSE_MO_SIGNALLING    = 0x3,
    RRC_CAUSE_MO_DATA          = 0x4,
    RRC_CAUSE_LAST
} rrc_establishment_cause_t;

typedef struct s1ap_gummei_s {
    uint16_t mcc;
    uint16_t mnc;
    uint8_t  mme_code;
    uint16_t mme_group_id;
} s1ap_gummei_t;

typedef struct s_tmsi_s {
    uint8_t  mme_code;
    uint32_t m_tmsi;
} s_tmsi_t;

typedef enum ue_identities_presenceMask_e {
  UE_IDENTITIES_NONE = 0,
  UE_IDENTITIES_s_tmsi = 1 << 1,
  UE_IDENTITIES_gummei = 1 << 2,
} ue_identities_presenceMask_t;

typedef struct ue_identity_s {
    ue_identities_presenceMask_t presenceMask;
    s_tmsi_t s_tmsi;
    s1ap_gummei_t gummei;
} ue_identity_t;

typedef struct nas_pdu_s {
    /* Octet string data */
    uint8_t  *buffer;
    /* Length of the octet string */
    uint32_t  length;
} nas_pdu_t, ue_radio_cap_t;

typedef struct transport_layer_addr_s {
    /* Length of the transport layer address buffer in bits. S1AP layer received a
     * bit string<1..160> containing one of the following addresses: ipv4,
     * ipv6, or ipv4 and ipv6. The layer doesn't interpret the buffer but
     * silently forward it to S1-U.
     */
    uint8_t length;
    uint8_t buffer[20];
} transport_layer_addr_t;

typedef struct e_rab_level_qos_parameter_s {
    uint8_t qci;

    allocation_retention_priority_t allocation_retention_priority;
} e_rab_level_qos_parameter_t;

typedef struct e_rab_s {
    /* Unique e_rab_id for the UE. */
    uint8_t                     e_rab_id;
    /* Quality of service for this e_rab */
    e_rab_level_qos_parameter_t qos;
    /* The NAS PDU should be forwarded by the RRC layer to the NAS layer */
    nas_pdu_t                   nas_pdu;
    /* The transport layer address for the IP packets */
    transport_layer_addr_t      sgw_addr;
    /* S-GW Tunnel endpoint identifier */
    uint32_t                    gtp_teid;
} e_rab_t;

typedef struct e_rab_setup_s {
    /* Unique e_rab_id for the UE. */
    uint8_t e_rab_id;

    /* The transport layer address for the IP packets */
    transport_layer_addr_t eNB_addr;

    /* S-GW Tunnel endpoint identifier */
    uint32_t gtp_teid;
} e_rab_setup_t;

typedef struct e_rab_failed_s {
    /* Unique e_rab_id for the UE. */
    uint8_t e_rab_id;
    /* Cause of the failure */
    //     cause_t cause;
} e_rab_failed_t;

typedef struct s1ap_register_eNB_s {
    /* Unique eNB_id to identify the eNB within EPC.
     * For macro eNB ids this field should be 20 bits long.
     * For home eNB ids this field should be 28 bits long.
     */
    uint32_t eNB_id;
    /* The type of the cell */
    enum cell_type_e cell_type;

    /* Optional name for the cell
     * NOTE: the name can be NULL (i.e no name) and will be cropped to 150
     * characters.
     */
    char *eNB_name;

    /* Tracking area code */
    uint16_t tac;

    /* Mobile Country Code
     * Mobile Network Code
     */
    uint16_t mcc;
    uint16_t mnc;

    /* Default Paging DRX of the eNB as defined in TS 36.304 */
    paging_drx_t default_drx;

    /* The eNB IP address to bind */
    net_ip_address_t enb_ip_address;

    /* Nb of MME to connect to */
    uint8_t          nb_mme;
    /* List of MME to connect to */
    net_ip_address_t mme_ip_address[S1AP_MAX_NB_MME_IP_ADDRESS];
} s1ap_register_eNB_t;

/* The NAS First Req is the first message exchanged between RRC and S1AP
 * for an UE.
 * The rnti uniquely identifies an UE within a cell. Later the enb_ue_s1ap_id
 * will be the unique identifier used between RRC and S1AP.
 */
typedef struct s1ap_nas_first_req_s {
    /* UE id for initial connection to S1AP */
    uint16_t ue_initial_id;

    /* Establishment cause as sent by UE */
    rrc_establishment_cause_t establishment_cause;

    /* NAS PDU */
    nas_pdu_t nas_pdu;

    /* If this flag is set S1AP layer is expecting the GUMMEI. If = 0,
     * the temporary s-tmsi is used.
     */
    ue_identity_t ue_identity;
} s1ap_nas_first_req_t;

typedef struct s1ap_uplink_nas_s {
    /* Unique UE identifier within an eNB */
    unsigned eNB_ue_s1ap_id:24;

    /* NAS pdu */
    nas_pdu_t nas_pdu;
} s1ap_uplink_nas_t;

typedef struct s1ap_downlink_nas_s {
    /* UE id for initial connection to S1AP */
    uint16_t ue_initial_id;

    /* Unique UE identifier within an eNB */
    unsigned eNB_ue_s1ap_id:24;

    /* NAS pdu */
    nas_pdu_t nas_pdu;
} s1ap_downlink_nas_t;

typedef struct s1ap_initial_context_setup_req_s {
    /* UE id for initial connection to S1AP */
    uint16_t ue_initial_id;

    /* eNB ue s1ap id as initialized by S1AP layer */
    unsigned eNB_ue_s1ap_id:24;

    /* UE aggregate maximum bitrate */
    ambr_t ue_ambr;

    /* Security algorithms */
    security_capabilities_t security_capabilities;

    /* Security key */
    uint8_t security_key[SECURITY_KEY_LENGTH];

    /* Number of e_rab to be setup in the list */
    uint8_t  nb_of_e_rabs;
    /* list of e_rab to be setup by RRC layers */
    e_rab_t  e_rab_param[S1AP_MAX_E_RAB];
} s1ap_initial_context_setup_req_t;

typedef struct s1ap_initial_context_setup_resp_s {
    unsigned  eNB_ue_s1ap_id:24;

    /* Number of e_rab setup-ed in the list */
    uint8_t       nb_of_e_rabs;
    /* list of e_rab setup-ed by RRC layers */
    e_rab_setup_t e_rabs[S1AP_MAX_E_RAB];

    /* Number of e_rab failed to be setup in list */
    uint8_t        nb_of_e_rabs_failed;
    /* list of e_rabs that failed to be setup */
    e_rab_failed_t e_rabs_failed[S1AP_MAX_E_RAB];
} s1ap_initial_context_setup_resp_t;

typedef struct s1ap_initial_context_setup_fail_s {
    unsigned  eNB_ue_s1ap_id:24;

    /* TODO add cause */
} s1ap_initial_context_setup_fail_t;

typedef struct s1ap_ue_cap_info_ind_s {
    unsigned  eNB_ue_s1ap_id:24;
    ue_radio_cap_t ue_radio_cap;
} s1ap_ue_cap_info_ind_t;

#endif /* S1AP_MESSAGES_TYPES_H_ */

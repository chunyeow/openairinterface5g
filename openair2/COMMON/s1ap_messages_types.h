#ifndef S1AP_MESSAGES_TYPES_H_
#define S1AP_MESSAGES_TYPES_H_

enum cell_type_e {
    CELL_MACRO_ENB,
    CELL_HOME_ENB
};

typedef enum {
    PAGING_DRX_32  = 0x0,
    PAGING_DRX_64  = 0x1,
    PAGING_DRX_128 = 0x2,
    PAGING_DRX_256 = 0x3,
} paging_drx_t;

typedef struct {
    unsigned ipv4:1;
    unsigned ipv6:1;
    char ipv4_address[16];
    char ipv6_address[40];
} net_ip_address_t;

/* Maximum number of e-rabs to be setup/deleted in a single message.
 * Even if only one bearer will be modified by message.
 */
#define S1AP_MAX_E_RAB  11

/* Length of the transport layer address string
 * 160 bits / 8 bits by char.
 */
#define S1AP_TRANSPORT_LAYER_ADDRESS_SIZE (160 / 8)

#define S1AP_MAX_NB_MME_IP_ADDRESS 10

/* Provides the establishment cause for the RRC connection request as provided
 * by the upper layers. W.r.t. the cause value names: highPriorityAccess
 * concerns AC11..AC15, ‘mt’ stands for ‘Mobile Terminating’ and ‘mo’ for
 * 'Mobile Originating'. Defined in TS 36.331.
 */
typedef enum {
    RRC_CAUSE_EMERGENCY        = 0x0,
    RRC_CAUSE_HIGH_PRIO_ACCESS = 0x1,
    RRC_CAUSE_MT_ACCESS        = 0x2,
    RRC_CAUSE_MO_SIGNALLING    = 0x3,
    RRC_CAUSE_MO_DATA          = 0x4,
    RRC_CAUSE_LAST
} rrc_establishment_cause_t;

typedef struct {
    uint16_t mcc;
    uint16_t mnc;
    uint8_t  mme_code;
    uint16_t mme_group_id;
} gummei_t;

typedef struct {
    uint8_t  mme_code;
    uint32_t m_tmsi;
} s_tmsi_t;

typedef struct {
#define S_TMSI_PROVIDED 0x0
#define GUMMEI_PROVIDED 0x1
    unsigned     present:1;
    union {
        gummei_t gummei;
        s_tmsi_t s_tmsi;
    } identity;
} ue_identity_t;

typedef struct {
    /* Octet string data */
    uint8_t  *buffer;
    /* Length of the octet string */
    uint32_t  length;
} nas_pdu_t, ue_radio_cap_t;

typedef struct {
    /* Length of the transport layer address buffer. S1AP layer received a
     * bit string<1..160> containing one of the following addresses: ipv4,
     * ipv6, or ipv4 and ipv6. The layer doesn't interpret the buffer but
     * silently forward it to S1-U.
     */
    uint8_t length;
    uint8_t buffer[20];
} transport_layer_addr_t;

typedef struct {
    uint8_t qci;
    
} e_rab_level_qos_parameter_t;

typedef struct {
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

typedef struct {
    /* Unique e_rab_id for the UE. */
    uint8_t e_rab_id;

    /* The transport layer address for the IP packets */
    transport_layer_addr_t eNB_addr;

    /* S-GW Tunnel endpoint identifier */
    uint32_t gtp_teid;
} e_rab_setup_t;

typedef struct {
    /* Unique e_rab_id for the UE. */
    uint8_t e_rab_id;
    /* Cause of the failure */
    //     cause_t cause;
} e_rab_failed_t;

typedef struct {
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
typedef struct {
    /* RNTI of the mobile */
    uint16_t rnti;

    /* Establishment cause as sent by UE */
    rrc_establishment_cause_t establishment_cause;

    /* NAS PDU */
    nas_pdu_t nas_pdu;

    /* If this flag is set S1AP layer is expecting the GUMMEI. If = 0,
     * the temporary s-tmsi is used.
     */
    ue_identity_t ue_identity;
} s1ap_nas_first_req_t;

typedef struct {
    /* Unique UE identifier within an eNB */
    unsigned eNB_ue_s1ap_id:24;

    /* NAS pdu */
    nas_pdu_t nas_pdu;
} s1ap_uplink_nas_t;

typedef s1ap_uplink_nas_t s1ap_downlink_nas_t;

typedef struct {
    unsigned eNB_ue_s1ap_id:24;

    /* Number of e_rab to be setup in the list */
    uint8_t  nb_of_e_rabs;
    /* list of e_rab to be setup by RRC layers */
    e_rab_t  e_rab_param[S1AP_MAX_E_RAB];
} s1ap_initial_context_setup_req_t;

typedef struct {
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

typedef struct {
    unsigned  eNB_ue_s1ap_id:24;
    ue_radio_cap_t ue_radio_cap;
} s1ap_ue_cap_info_ind_t;

#endif /* S1AP_MESSAGES_TYPES_H_ */

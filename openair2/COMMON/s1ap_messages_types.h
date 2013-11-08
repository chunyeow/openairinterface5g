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
    /* For virtual mode, mod_id of the eNB as defined in the rest of the L1/L2 stack */
    uint8_t mod_id;

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
    /* For virtual mode, mod_id of the eNB as defined in the rest of the L1/L2 stack */
    uint8_t mod_id;

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
    /* For virtual mode, mod_id of the eNB as defined in the rest of the L1/L2 stack */
    uint8_t mod_id;

    /* Unique UE identifier within an eNB */
    unsigned eNB_ue_s1ap_id:24;

    /* NAS pdu */
    nas_pdu_t nas_pdu;
} s1ap_uplink_nas_t;

typedef s1ap_uplink_nas_t s1ap_downlink_nas_t;

#endif /* S1AP_MESSAGES_TYPES_H_ */

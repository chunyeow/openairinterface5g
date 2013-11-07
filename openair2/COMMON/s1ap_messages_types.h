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

typedef struct {
    /* For virtual mode, mod_id as defined in the rest of the L1/L2 stack */
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

#endif /* S1AP_MESSAGES_TYPES_H_ */

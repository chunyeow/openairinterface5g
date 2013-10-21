#include "NwGtpv1u.h"
#include "gtpv1u.h"
#include "udp_primitives_client.h"
#include "hashtable.h"

//TEST LG #define GTPU_IN_KERNEL

//#include "tree.h"

#ifndef GTPV1U_ENB_DEFS_H_
#define GTPV1U_ENB_DEFS_H_

#define GTPV1U_UDP_PORT (2152)

#define MAX_BEARERS_PER_UE (11)

typedef enum {
    BEARER_DOWN = 0,
    BEARER_IN_CONFIG,
    BEARER_UP,
    BEARER_DL_HANDOVER,
    BEARER_UL_HANDOVER,
    BEARER_MAX,
} bearer_state_t;

typedef struct gtpv1u_bearer_s {
    /* TEID used in dl and ul */
    uint32_t teid_eNB;         ///< eNB TEID
    uint32_t teid_sgw;         ///< Remote TEID
    uint32_t sgw_ip_addr;
    uint16_t port;
    NwGtpv1uStackSessionHandleT stack_session;
    bearer_state_t state;
} gtpv1u_bearer_t;

typedef struct gtpv1u_ue_data_s {
    /* UE identifier for oaisim stack */
    uint8_t  ue_id;

    /* Unique identifier used between PDCP and GTP-U to distinguish UEs */
    uint32_t instance_id;

    /* Bearer related data.
     * Note that the first LCID available for data is 3 and we fixed the maximum
     * number of e-rab per UE to be 11. The real rb id will 3 + rab_id (0..10).
     */
    gtpv1u_bearer_t bearers[MAX_BEARERS_PER_UE];

    //RB_ENTRY(gtpv1u_ue_data_s) gtpv1u_ue_node;
} gtpv1u_ue_data_t;

typedef struct gtpv1u_data_s{
    /* nwgtpv1u stack internal data */
    NwGtpv1uStackHandleT  gtpv1u_stack;
    /* RB tree of UEs */
	hash_table_t         *ue_mapping;

    //RB_HEAD(gtpv1u_ue_map, gtpv1u_ue_data_s) gtpv1u_ue_map_head;
    /* Local IP address to use */
    char                 *ip_addr;
    /* UDP internal data */
    udp_data_t            udp_data;

    uint16_t              seq_num;
    uint8_t               restart_counter;

#ifdef GTPU_IN_KERNEL
    char                 *interface_name;
    int                   interface_index;

    struct iovec         *malloc_ring;
    void                 *sock_mmap_ring[16];
    int                   sock_desc[16]; // indexed by marking
#endif
} gtpv1u_data_t;

int gtpv1u_new_data_req(gtpv1u_data_t *gtpv1u_data_p,
                        uint8_t ue_id, uint8_t rab_id,
                        uint8_t *buffer, uint32_t buf_len);

int gtpv1u_initial_req(gtpv1u_data_t *gtpv1u_data_p, uint32_t teid,
                       uint16_t port, uint32_t address);

int gtpv1u_eNB_init(gtpv1u_data_t *gtpv1u_data_p);

#endif /* GTPV1U_ENB_DEFS_H_ */

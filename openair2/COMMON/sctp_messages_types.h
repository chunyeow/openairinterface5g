#ifndef SCTP_MESSAGES_TYPES_H_
#define SCTP_MESSAGES_TYPES_H_

enum sctp_state_e {
    SCTP_STATE_CLOSED,
    SCTP_STATE_SHUTDOWN,
    SCTP_STATE_ESTABLISHED,
    SCTP_STATE_UNREACHABLE
};

typedef struct sctp_new_association_req_s {
    /* Upper layer connexion identifier */
    uint16_t         ulp_cnx_id;

    /* The port to connect to */
    uint16_t         port;
    /* Payload Protocol Identifier to use */
    uint32_t         ppid;

    /* Local address to bind to */
    net_ip_address_t local_address;
    /* Remote address to connect to */
    net_ip_address_t remote_address;
} sctp_new_association_req_t;

typedef struct sctp_new_association_ind_s {
    /* Assoc id of the new association */
    int32_t  assoc_id;

    /* The port used by remote host */
    uint16_t port;

    /* Number of streams used for this association */
    uint16_t in_streams;
    uint16_t out_streams;
} sctp_new_association_ind_t;

typedef struct sctp_new_association_resp_s {
    /* Upper layer connexion identifier */
    uint16_t ulp_cnx_id;

    /* SCTP Association ID */
    int32_t  assoc_id;

    /* Input/output streams */
    uint16_t out_streams;
    uint16_t in_streams;

    /* State of the association at SCTP level */
    enum sctp_state_e sctp_state;
} sctp_new_association_resp_t;

typedef struct sctp_data_ind_s {
    /* SCTP Association ID */
    int32_t   assoc_id;

    /* Buffer to send over SCTP */
    uint32_t  buffer_length;
    uint8_t  *buffer;

    /* Streams on which data will be sent/received */
    uint16_t  stream;
} sctp_data_ind_t;

typedef sctp_data_ind_t sctp_data_req_t;

typedef struct sctp_listener_register_upper_layer_s {
    /* Port to listen to */
    uint16_t port;
    /* Payload protocol identifier
     * Any data receveid on PPID != will be discarded
     */
    uint32_t ppid;
} sctp_listener_register_upper_layer_t;

#endif /* SCTP_MESSAGES_TYPES_H_ */

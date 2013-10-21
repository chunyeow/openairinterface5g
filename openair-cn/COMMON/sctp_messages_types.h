#ifndef SCTP_MESSAGES_TYPES_H_
#define SCTP_MESSAGES_TYPES_H_

typedef struct {
    uint8_t  *buffer;
    uint32_t  bufLen;
    uint32_t  assocId;
    uint16_t  stream;
} SctpNewDataReq;

typedef struct {
    /* Request usage of ipv4 */
    unsigned  ipv4:1;
    /* Request usage of ipv6 */
    unsigned  ipv6:1;
    uint8_t   nb_ipv4_addr;
    uint32_t  ipv4_address[10];
    uint8_t   nb_ipv6_addr;
    char     *ipv6_address[10];
    uint16_t  port;
    uint32_t  ppid;
} SctpInit;

typedef struct {
    uint32_t  assoc_id;
} sctp_close_association_t;

typedef struct {
    uint32_t instreams;
    uint32_t outstreams;
    uint32_t assoc_id;
} sctp_new_peer_t;

#endif /* SCTP_MESSAGES_TYPES_H_ */

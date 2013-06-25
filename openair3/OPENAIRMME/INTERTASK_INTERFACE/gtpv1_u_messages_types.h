#ifndef GTPV1_U_MESSAGES_TYPES_H_
#define GTPV1_U_MESSAGES_TYPES_H_

typedef struct {
    uint8_t *buffer;
    uint32_t buffer_length;
    uint16_t port;
    uint32_t peer_addr;
} Gtpv1uDataReq;

typedef struct {
    
} S1UCreateTunnel;

#endif /* GTPV1_U_MESSAGES_TYPES_H_ */

#ifndef UDP_MESSAGES_TYPES_H_
#define UDP_MESSAGES_TYPES_H_

typedef struct {
    uint32_t  port;
    char     *address;
} UdpInit;

typedef struct {
    uint8_t  *buffer;
    uint32_t  buffer_length;
    uint32_t  peer_address;
    uint32_t  peer_port;
} UdpDataReq;

#endif /* UDP_MESSAGES_TYPES_H_ */

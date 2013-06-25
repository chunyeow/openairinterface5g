#ifndef SCTP_MESSAGES_TYPES_H_
#define SCTP_MESSAGES_TYPES_H_

typedef struct {
    uint8_t  *buffer;
    uint32_t  bufLen;
    uint32_t  assocId;
    uint16_t  stream;
} SctpNewDataReq;

typedef struct {
    uint16_t  port;
    char     *address;
    uint32_t  ppid;
} SctpS1APInit;

typedef struct {
    uint32_t  assocId;
} SctpCloseAssociation;

#endif /* SCTP_MESSAGES_TYPES_H_ */

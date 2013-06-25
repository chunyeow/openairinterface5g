#ifndef S1AP_MESSAGES_TYPES_H_
#define S1AP_MESSAGES_TYPES_H_

typedef struct {
    uint8_t  *buffer;           ///< SCTP buffer
    uint32_t  bufLen;           ///< SCTP buffer length
    int32_t   assocId;          ///< SCTP physical association ID
    uint8_t   stream;           ///< Stream number on which data had been received
    uint16_t  instreams;        ///< Number of input streams for the SCTP connection between peers
    uint16_t  outstreams;       ///< Number of output streams for the SCTP connection between peers
} S1apSctpNewMessageInd;

typedef struct {
    uint32_t  assocId;          ///< SCTP Association ID of the closed connection
} S1apSctpAsscociationClosed;

typedef struct {
    uint8_t  *nas_buffer;       ///< NAS buffer
    uint32_t  nas_length;       ///< NAS buffer length
    uint32_t  mme_ue_s1ap_id;   ///< Unique MME UE s1ap id
} S1apNASNewMessageInd;

#endif /* S1AP_MESSAGES_TYPES_H_ */

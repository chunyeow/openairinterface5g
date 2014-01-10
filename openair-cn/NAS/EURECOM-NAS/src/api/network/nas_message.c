/*****************************************************************************
			Eurecom OpenAirInterface 3
			Copyright(c) 2012 Eurecom

Source		nas_message.h

Version		0.1

Date		2012/26/09

Product		NAS stack

Subsystem	Application Programming Interface

Author		Frederic Maurel

Description	Defines the layer 3 messages supported by the NAS sublayer
		protocol and functions used to encode and decode

*****************************************************************************/

#include "nas_message.h"
#include "nas_log.h"

#include "TLVDecoder.h"
#include "TLVEncoder.h"

#include <stdlib.h>	// malloc, free
#include <string.h>	// memcpy

#if (defined(EPC_BUILD) && defined(NAS_MME))
# include "nas_itti_messaging.h"
#endif

/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

/* Functions used to decode layer 3 NAS messages */
static int _nas_message_header_decode(const char* buffer,
			nas_message_security_header_t* header, int length);
static int _nas_message_plain_decode(const char* buffer,
			const nas_message_security_header_t* header,
			nas_message_plain_t* msg, int length);
static int _nas_message_protected_decode(const char* buffer,
			nas_message_security_header_t* header,
			nas_message_plain_t* msg, int length);

/* Functions used to encode layer 3 NAS messages */
static int _nas_message_header_encode(char* buffer,
		const nas_message_security_header_t* header, int length);
static int _nas_message_plain_encode(char* buffer,
		const nas_message_security_header_t* header,
		const nas_message_plain_t* msg, int length);
static int _nas_message_protected_encode(char* buffer,
		const nas_message_security_protected_t* msg, int length);

/* Functions used to decrypt and encrypt layer 3 NAS messages */
static int _nas_message_decrypt(char* dest, const char* src, UInt8_t type,
				UInt32_t code, UInt8_t seq, int length);
static int _nas_message_encrypt(char* dest, const char* src, UInt8_t type,
				UInt32_t code, UInt8_t seq, int length);

/* Functions used for integrity protection of layer 3 NAS messages */
static UInt32_t _nas_message_get_mac(const char* buffer, UInt32_t count,
				     int length);

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:	 nas_message_encrypt()                                     **
 **                                                                        **
 ** Description: Encripts plain NAS message into security protected NAS    **
 **		 message                                                   **
 **                                                                        **
 ** Inputs:	 inbuf:		Input buffer containing plain NAS message  **
 **		 header:	Security protected header to be applied    **
 ** 		 length:	Number of bytes that should be encrypted   **
 **		 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 outbuf:	Output buffer containing security protec-  **
 **				ted message                                **
 ** 		 Return:	The number of bytes in the output buffer   **
 **				if the input buffer has been successfully  **
 **				encrypted; Negative error code otherwise.  **
 **		 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
int nas_message_encrypt(const char* inbuf, char* outbuf,
			const nas_message_security_header_t* header, int length)
{
    LOG_FUNC_IN;

    int bytes = length;

    /* Encode the header */
    int size = _nas_message_header_encode(outbuf, header, length);

    if (size < 0) {
	LOG_FUNC_RETURN (TLV_ENCODE_BUFFER_TOO_SHORT);
    }
    else if (size > 1) {
	/* Encrypt the plain NAS message */
	bytes = _nas_message_encrypt(outbuf + size, inbuf,
				     header->security_header_type,
				     header->message_authentication_code,
				     header->sequence_number,
				     length - size);
	/* Integrity protected the NAS message */
	if (bytes > 0) {
	    /* Compute offset of the sequence number field */
	    int offset = size - sizeof(UInt8_t);
	    /* Compute the NAS message authentication code */
	    UInt32_t mac = _nas_message_get_mac(outbuf + offset,
						0, // TODO !!! ul counter
						length - offset);
	    /* Set the message authentication code of the NAS message */
	    *(UInt32_t*)(outbuf + sizeof(UInt8_t)) = mac;
	}
    }
    else {
	/* The input buffer does not need to be encrypted */
	memcpy(outbuf, inbuf, length);
    }

    if (bytes < 0) {
	LOG_FUNC_RETURN (bytes);
    }
    if (size > 1) {
 	LOG_FUNC_RETURN (size + bytes);
    }
    LOG_FUNC_RETURN (bytes);
}

/****************************************************************************
 **                                                                        **
 ** Name:	 nas_message_decrypt()                                     **
 **                                                                        **
 ** Description: Decripts security protected NAS message into plain NAS    **
 **		 message                                                   **
 **                                                                        **
 ** Inputs:	 inbuf:		Input buffer containing security protected **
 **				NAS message                                **
 ** 		 length:	Number of bytes that should be decrypted   **
 **		 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 outbuf:	Output buffer containing plain NAS message **
 **		 header:	Security protected header applied          **
 ** 		 Return:	The number of bytes in the output buffer   **
 **				if the input buffer has been successfully  **
 **				decrypted; Negative error code otherwise.  **
 **		 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
int nas_message_decrypt(const char* inbuf, char* outbuf,
			nas_message_security_header_t* header, int length)
{
    LOG_FUNC_IN;

    int bytes = length;

    /* Decode the header */
    int size = _nas_message_header_decode(inbuf, header, length);

    if (size < 0) {
	LOG_FUNC_RETURN (TLV_DECODE_BUFFER_TOO_SHORT);
    }
    else if (size > 1) {
	/* Compute offset of the sequence number field */
	int offset = size - sizeof(UInt8_t);
	/* Compute the NAS message authentication code */
	UInt32_t mac = _nas_message_get_mac(inbuf + offset,
					    0, // TODO !!! dl counter
					    length - offset);
	/* Check NAS message integrity */
	if (mac != header->message_authentication_code) {
	    LOG_FUNC_RETURN (TLV_DECODE_MAC_MISMATCH);
	}

	/* Decrypt the security protected NAS message */
	header->protocol_discriminator =
	    _nas_message_decrypt(outbuf, inbuf + size,
				 header->security_header_type,
				 header->message_authentication_code,
				 header->sequence_number,
				 length - size);
	bytes = length - size;
    }
    else {
	/* The input buffer contains a plain NAS message */
	memcpy(outbuf, inbuf, length);
    }

    LOG_FUNC_RETURN (bytes);
}

/****************************************************************************
 **                                                                        **
 ** Name:	 nas_message_decode()                                      **
 **                                                                        **
 ** Description: Decode layer 3 NAS message                                **
 **                                                                        **
 ** Inputs:	 buffer:	Pointer to the buffer containing layer 3   **
 **				NAS message data                           **
 ** 		 length:	Number of bytes that should be decoded     **
 **		 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 msg:		L3 NAS message structure to be filled      **
 ** 		 Return:	The number of bytes in the buffer if the   **
 **				data have been successfully decoded;       **
 **				A negative error code otherwise.           **
 **		 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
int nas_message_decode(const char* const buffer, nas_message_t* msg, int length)
{
    LOG_FUNC_IN;

    int bytes;

    /* Decode the header */
    int size = _nas_message_header_decode(buffer, &msg->header, length);

    if (size < 0) {
        LOG_FUNC_RETURN (TLV_DECODE_BUFFER_TOO_SHORT);
    }
    else if (size > 1) {
        /* Compute offset of the sequence number field */
        int offset = size - sizeof(UInt8_t);
        /* Compute the NAS message authentication code */
        UInt32_t mac = _nas_message_get_mac(buffer + offset,
                                            0, // TODO !!! dl counter
                                            length - offset);
        /* Check NAS message integrity */
        if (mac != msg->header.message_authentication_code) {
            LOG_FUNC_RETURN (TLV_DECODE_MAC_MISMATCH);
        }

        /* Decode security protected NAS message */
        bytes = _nas_message_protected_decode(buffer + size, &msg->header,
                                              &msg->plain, length - size);
#if defined(EPC_BUILD) && defined(NAS_MME)
        /* Message has been decoded and security header removed, handle it has a plain message */
        nas_itti_plain_msg(buffer, msg, length, 0);
#endif
    }
    else {
        /* Decode plain NAS message */
        bytes = _nas_message_plain_decode(buffer, &msg->header,
                                          &msg->plain, length);
#if defined(EPC_BUILD) && defined(NAS_MME)
        nas_itti_plain_msg(buffer, msg, length, 0);
#endif
    }

    if (bytes < 0) {
        LOG_FUNC_RETURN (bytes);
    }
    if (size > 1) {
        LOG_FUNC_RETURN (size + bytes);
    }
    LOG_FUNC_RETURN (bytes);
}

/****************************************************************************
 **                                                                        **
 ** Name:	 nas_message_encode()                                      **
 **                                                                        **
 ** Description: Encode layer 3 NAS message                                **
 **                                                                        **
 ** Inputs	 msg:		L3 NAS message structure to encode         **
 **		 length:	Maximal capacity of the output buffer      **
 **		 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 buffer:	Pointer to the encoded data buffer         **
 ** 		 Return:	The number of bytes in the buffer if the   **
 **				data have been successfully encoded;       **
 **				A negative error code otherwise.           **
 **		 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
int nas_message_encode(char* buffer, const nas_message_t* const msg, int length)
{
    LOG_FUNC_IN;

    int bytes;

    /* Encode the header */
    int size = _nas_message_header_encode(buffer, &msg->header, length);

    if (size < 0) {
        LOG_FUNC_RETURN (TLV_ENCODE_BUFFER_TOO_SHORT);
    }
    else if (size > 1) {
        /* Encode security protected NAS message */
        bytes = _nas_message_protected_encode(buffer + size, &msg->security_protected,
                              length - size);
        /* Integrity protect the NAS message */
        if (bytes > 0) {
            /* Compute offset of the sequence number field */
            int offset = size - sizeof(UInt8_t);
            /* Compute the NAS message authentication code */
            UInt32_t mac = _nas_message_get_mac(buffer + offset,
                            0, // TODO !!! ul counter
                            length - offset);
            /* Set the message authentication code of the NAS message */
            *(UInt32_t*)(buffer + sizeof(UInt8_t)) = mac;
        }
#if defined(EPC_BUILD) && defined(NAS_MME)
        nas_itti_protected_msg(buffer, msg, length, 1);
#endif
    }
    else {
        /* Encode plain NAS message */
        bytes = _nas_message_plain_encode(buffer, &msg->header,
                          &msg->plain, length);
#if defined(EPC_BUILD) && defined(NAS_MME)
        nas_itti_plain_msg(buffer, msg, length, 1);
#endif
    }

    if (bytes < 0) {
        LOG_FUNC_RETURN (bytes);
    }
    if (size > 1) {
        LOG_FUNC_RETURN (size + bytes);
    }
    LOG_FUNC_RETURN (bytes);
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/

/*
 * -----------------------------------------------------------------------------
 *		Functions used to decode layer 3 NAS messages
 * -----------------------------------------------------------------------------
 */

/****************************************************************************
 **                                                                        **
 ** Name:	 _nas_message_header_decode()                              **
 **                                                                        **
 ** Description: Decode header of a security protected NAS message         **
 **                                                                        **
 ** Inputs:	 buffer:	Pointer to the buffer containing layer 3   **
 **				message data                               **
 ** 		 length:	Number of bytes that should be decoded     **
 **		 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 header:	Security header structure to be filled     **
 ** 		 Return:	The size in bytes of the security header   **
 **				if data have been successfully decoded;    **
 **				1, if the header is not a security header  **
 **				(header of plain NAS message);             **
 **				-1 otherwise.                              **
 **		 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
static int _nas_message_header_decode(const char* buffer,
                                      nas_message_security_header_t* header,
                                      int length)
{
    LOG_FUNC_IN;

    int size = 0;

    /* Decode the first octet of the header (security header type or EPS bearer
     * identity, and protocol discriminator) */
    DECODE_U8(buffer, *(UInt8_t*)(header), size);

    if (header->protocol_discriminator == EPS_MOBILITY_MANAGEMENT_MESSAGE) {
        if (header->security_header_type != SECURITY_HEADER_TYPE_NOT_PROTECTED)
        {
            if (length < NAS_MESSAGE_SECURITY_HEADER_SIZE) {
            /* The buffer is not big enough to contain security header */
            LOG_TRACE(WARNING, "NET-API   - The size of the header (%u) "
                  "exceeds the buffer length (%u)",
                  NAS_MESSAGE_SECURITY_HEADER_SIZE, length);
            LOG_FUNC_RETURN (-1);
            }
            /* Decode the message authentication code */
            DECODE_U32(buffer+size, header->message_authentication_code, size);
            /* Decode the sequence number */
            DECODE_U8(buffer+size, header->sequence_number, size);
        }
    }

    LOG_FUNC_RETURN (size);
}

/****************************************************************************
 **                                                                        **
 ** Name:	 _nas_message_plain_decode()                               **
 **                                                                        **
 ** Description: Decode plain NAS message                                  **
 **                                                                        **
 ** Inputs:	 buffer:	Pointer to the buffer containing plain NAS **
 **				message data                               **
 **		 header:	Header of the plain NAS message            **
 ** 		 length:	Number of bytes that should be decoded     **
 **		 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 msg:		Decoded NAS message                        **
 ** 		 Return:	The number of bytes in the buffer if the   **
 **				data have been successfully decoded;       **
 **				A negative error code otherwise.           **
 **		 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
static int _nas_message_plain_decode(const char* buffer,
				const nas_message_security_header_t* header,
				nas_message_plain_t* msg, int length)
{
    LOG_FUNC_IN;

    int bytes = TLV_DECODE_PROTOCOL_NOT_SUPPORTED;

    if (header->protocol_discriminator == EPS_MOBILITY_MANAGEMENT_MESSAGE) {
	/* Decode EPS Mobility Management L3 message */
	bytes = emm_msg_decode(&msg->emm, (uint8_t *)buffer, length);
    }
    else if (header->protocol_discriminator == EPS_SESSION_MANAGEMENT_MESSAGE) {
	/* Decode EPS Session Management L3 message */
	bytes = esm_msg_decode(&msg->esm, (uint8_t *)buffer, length);
    }
    else {
	/* Discard L3 messages with not supported protocol discriminator */
	LOG_TRACE(WARNING,"NET-API   - Protocol discriminator 0x%x is "
		  "not supported", header->protocol_discriminator);
    }

    LOG_FUNC_RETURN (bytes);
}

/****************************************************************************
 **                                                                        **
 ** Name:	 _nas_message_protected_decode()                           **
 **                                                                        **
 ** Description: Decode security protected NAS message                     **
 **                                                                        **
 ** Inputs:	 buffer:	Pointer to the buffer containing the secu- **
 **				rity protected NAS message data            **
 **		 header:	Header of the security protected NAS mes-  **
 **				sage                                       **
 ** 		 length:	Number of bytes that should be decoded     **
 **		 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 msg:		Decoded NAS message                        **
 ** 		 Return:	The number of bytes in the buffer if the   **
 **				data have been successfully decoded;       **
 **				A negative error code otherwise.           **
 **		 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
static int _nas_message_protected_decode(const char* buffer,
				nas_message_security_header_t* header,
				nas_message_plain_t* msg, int length)
{
    LOG_FUNC_IN;

    int bytes = TLV_DECODE_BUFFER_TOO_SHORT;

    char* plain_msg = (char*)malloc(length);
    if (plain_msg)
    {
	/* Decrypt the security protected NAS message */
	header->protocol_discriminator =
	    _nas_message_decrypt(plain_msg, buffer,
				 header->security_header_type,
				 header->message_authentication_code,
				 header->sequence_number, length);
	/* Decode the decrypted message as plain NAS message */
	bytes = _nas_message_plain_decode(plain_msg, header, msg, length);
	free(plain_msg);
    }

    LOG_FUNC_RETURN (bytes);
}

/*
 * -----------------------------------------------------------------------------
 *		Functions used to encode layer 3 NAS messages
 * -----------------------------------------------------------------------------
 */

/****************************************************************************
 **                                                                        **
 ** Name:	 _nas_message_header_encode()                              **
 **                                                                        **
 ** Description: Encode header of a security protected NAS message         **
 **                                                                        **
 ** Inputs	 header:	Security header structure to encode        **
 **		 length:	Maximal capacity of the output buffer      **
 **		 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 buffer:	Pointer to the encoded data buffer         **
 ** 		 Return:	The number of bytes in the buffer if the   **
 **				data have been successfully encoded;       **
 **				1, if the header is not a security header  **
 **				(header of plain NAS message);             **
 **				-1 otherwise.                              **
 **		 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
static int _nas_message_header_encode(char* buffer,
                                      const nas_message_security_header_t* header, int length)
{
    LOG_FUNC_IN;

    int size = 0;

    /* Encode the first octet of the header (security header type or EPS bearer
     * identity, and protocol discriminator) */
    ENCODE_U8(buffer, *(UInt8_t*)(header), size);

    if (header->protocol_discriminator == EPS_MOBILITY_MANAGEMENT_MESSAGE) {
        if (header->security_header_type != SECURITY_HEADER_TYPE_NOT_PROTECTED)
        {
            //static uint8_t seq = 0;
            if (length < NAS_MESSAGE_SECURITY_HEADER_SIZE) {
            /* The buffer is not big enough to contain security header */
            LOG_TRACE(WARNING, "NET-API   - The size of the header (%u) "
                  "exceeds the buffer length (%u)",
                  NAS_MESSAGE_SECURITY_HEADER_SIZE, length);
            LOG_FUNC_RETURN (-1);
            }
            /* Encode the message authentication code */
            ENCODE_U32(buffer+size, header->message_authentication_code, size);
            /* Encode the sequence number */
            ENCODE_U8(buffer+size, header->sequence_number, size);
            //ENCODE_U8(buffer+size, seq, size);
            //seq++;
        }
    }

    LOG_FUNC_RETURN (size);
}

/****************************************************************************
 **                                                                        **
 ** Name:	 _nas_message_plain_encode()                               **
 **                                                                        **
 ** Description: Encode plain NAS message                                  **
 **                                                                        **
 ** Inputs:	 pd:		Protocol discriminator of the NAS message  **
 **				to encode                                  **
 ** 		 msg:		Plain NAS message structure to encode      **
 **		 length:	Maximal capacity of the output buffer      **
 **		 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 buffer:	Pointer to the encoded data buffer         **
 ** 		 Return:	The number of bytes in the buffer if the   **
 **				data have been successfully encoded;       **
 **				A negative error code otherwise.           **
 **		 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
static int _nas_message_plain_encode(char* buffer,
				const nas_message_security_header_t* header,
				const nas_message_plain_t* msg, int length)
{
    LOG_FUNC_IN;

    int bytes = TLV_ENCODE_PROTOCOL_NOT_SUPPORTED;

    if (header->protocol_discriminator == EPS_MOBILITY_MANAGEMENT_MESSAGE) {
	/* Encode EPS Mobility Management L3 message */
	bytes = emm_msg_encode((EMM_msg*)(&msg->emm), (uint8_t*)buffer, length);
    }
    else if (header->protocol_discriminator == EPS_SESSION_MANAGEMENT_MESSAGE) {
	/* Encode EPS Session Management L3 message */
	bytes = esm_msg_encode((ESM_msg*)(&msg->esm), (uint8_t*)buffer, length);
    }
    else {
	/* Discard L3 messages with not supported protocol discriminator */
	LOG_TRACE(WARNING,"NET-API   - Protocol discriminator 0x%x is "
		  "not supported", header->protocol_discriminator);
    }

    LOG_FUNC_RETURN (bytes);
}

/****************************************************************************
 **                                                                        **
 ** Name:	 _nas_message_protected_encode()                           **
 **                                                                        **
 ** Description: Encode security protected NAS message                     **
 **                                                                        **
 ** Inputs	 msg:		Security protected NAS message structure   **
 **				to encode                                  **
 **		 length:	Maximal capacity of the output buffer      **
 **		 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 buffer:	Pointer to the encoded data buffer         **
 ** 		 Return:	The number of bytes in the buffer if the   **
 **				data have been successfully encoded;       **
 **				A negative error code otherwise.           **
 **		 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
static int _nas_message_protected_encode(char* buffer,
			const nas_message_security_protected_t* msg, int length)
{
    LOG_FUNC_IN;

    int bytes = TLV_ENCODE_BUFFER_TOO_SHORT;

    char* plain_msg = (char*)malloc(length);
    if (plain_msg) {
	/* Encode the security protected NAS message as plain NAS message */
	int size = _nas_message_plain_encode(plain_msg, &msg->header,
					     &msg->plain, length);
	if (size > 0) {
	    //static uint8_t seq = 0;
	    /* Encrypt the encoded plain NAS message */
	    bytes = _nas_message_encrypt(buffer, plain_msg,
					msg->header.security_header_type,
					msg->header.message_authentication_code,
					msg->header.sequence_number, size);
					//seq, size);
	    //seq ++;
	}
	free(plain_msg);
    }

    LOG_FUNC_RETURN (bytes);
}

/*
 * -----------------------------------------------------------------------------
 *	    Functions used to decrypt and encrypt layer 3 NAS messages
 * -----------------------------------------------------------------------------
 */

/****************************************************************************
 **                                                                        **
 ** Name:	 _nas_message_decrypt()                                    **
 **                                                                        **
 ** Description: Decrypt security protected NAS message                    **
 **                                                                        **
 ** Inputs	 src:		Pointer to the encrypted data buffer       **
 **		 type:		The security header type                   **
 **		 code:		The message authentication code            **
 **		 seq:		The sequence number                        **
 **		 length:	Maximal capacity of the output buffer      **
 **		 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 dest:		Pointer to the decrypted data buffer       **
 ** 		 Return:	The protocol discriminator of the message  **
 **				that has been decrypted;                   **
 **		 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
static int _nas_message_decrypt(char* dest, const char* src, UInt8_t type,
				UInt32_t code, UInt8_t seq, int length)
{
    LOG_FUNC_IN;

    int size = 0;
    nas_message_security_header_t header;

    /* TODO: run the uncyphering algorithm */
    memcpy(dest, src, length);

    /* Decode the first octet (security header type or EPS bearer identity,
     * and protocol discriminator) */
    DECODE_U8(src, *(UInt8_t*)(&header), size);

    LOG_FUNC_RETURN (header.protocol_discriminator);
}

/****************************************************************************
 **                                                                        **
 ** Name:	 _nas_message_encrypt()                                    **
 **                                                                        **
 ** Description: Encrypt plain NAS message                                 **
 **                                                                        **
 ** Inputs	 src:		Pointer to the decrypted data buffer       **
 **		 type:		The security header type                   **
 **		 code:		The message authentication code            **
 **		 seq:		The sequence number                        **
 **		 length:	Maximal capacity of the output buffer      **
 **		 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 dest:		Pointer to the encrypted data buffer       **
 ** 		 Return:	The number of bytes in the output buffer   **
 **				if data have been successfully encrypted;  **
 **				RETURNerror otherwise.                     **
 **		 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
static int _nas_message_encrypt(char* dest, const char* src, UInt8_t type,
				UInt32_t code, UInt8_t seq, int length)
{
    LOG_FUNC_IN;

    int size = length;

    /* TODO: run the cyphering algorithm */
    memcpy(dest, src, length);

    LOG_FUNC_RETURN (size);
}

/*
 * -----------------------------------------------------------------------------
 *	Functions used for integrity protection of layer 3 NAS messages
 * -----------------------------------------------------------------------------
 */

/****************************************************************************
 **                                                                        **
 ** Name:	 _nas_message_get_mac()                                    **
 **                                                                        **
 ** Description: Run integrity algorithm onto cyphered or uncyphered NAS   **
 **		 message encoded in the input buffer and return the compu- **
 **		 ted message authentication code                           **
 **                                                                        **
 ** Inputs	 buffer:	Pointer to the integrity protected data    **
 **				buffer                                     **
 **		 count:		Value of the uplink NAS counter            **
 **		 length:	Length of the input buffer                 **
 **		 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 None                                                      **
 ** 		 Return:	The message authentication code            **
 **		 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
static UInt32_t _nas_message_get_mac(const char* buffer, UInt32_t count,
				     int length)
{
    LOG_FUNC_IN;
    /* TODO: run integrity protection algorithm */
    /* TODO: Return the message authentication code */
#if defined(EPC_BUILD)
    LOG_FUNC_RETURN (0);
#else
    LOG_FUNC_RETURN (0xabababab);
#endif
}



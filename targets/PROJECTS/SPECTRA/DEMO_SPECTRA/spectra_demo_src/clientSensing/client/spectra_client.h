/*
 * spectra_client.h
 *
 * Generic functions and types for the spectra demonstration
 * control system. These functions are used by the client side.
 *
 *  Created on: Dec 11, 2013
 *  Author: camara
 *  Copyright (c) 2008-2014 Institut Mines-Telecom / Telecom ParisTech
 */

#ifndef SPECTRA_H_
#define SPECTRA_H_

/**
 * The IP address of the Embb server (e.g the Embb board)
 */
//#define SERVER_ADDRESS "192.168.0.2"
#define SERVER_ADDRESS "127.0.0.1"

/**
 * Connection port
 */
#define CONNECTION_PORT 4546

/**
 * Alinges packet, internally, into 32 bits, or 4 bytes
 */
#define PACKET_ALIGNEMENT_BYTES 4

/**
 * Maximum packet size, dependent on the size of the
 * number of elements field on the header, at the present 8 bits
 * i.e. (256 * 4 bytes + the header that is 4 bytes)
 */
#define MAX_PACKET_SIZE 1028

/**
 * Which is the type of the message, if it is, for example, an start sensing,
 * or an answer to a previously requested score
 */
typedef enum {
	Sense,       //!< Sense
	Classifying, //!< Classifying
	ScoreResponse, //!< ScoreResponse
    EndProcessing
} messageType;

/**
 * Which is the functions that should be performed by the embb
 */
//! An enum.
/*! More detailed enum description. */
typedef enum {
	EnergyDetection,   //!< EnergyDetection
	WelchPeriodograms  //!< WelchPeriodograms
} functionToBePerformed;

/**
 * Definition of the spectra control packets
 */
typedef struct {
	uint16_t messageID;                //!<  Operation ID, used to identify an request and its answer
	messageType type :4;                //!<  16 different types of message possible
	functionToBePerformed function :4;  //!<  Embb function to be performed 16 possible different functions
	unsigned char numberOfparameters;  //!<  up to 255 parameters per message
	uint32_t * parameters;             //!<  variable number of parameters, depending
									   //!<  on what we are doing and the default values
}__attribute__((packed)) SpectraMessage;

/**
 * \typedef SpectraMsgHelper
 * Helper structure to handle control packets
 */
typedef struct {
	uint16_t messageID;                //!<  Operation ID
	unsigned char type_function;       //!<  Assembled type and function for easier treatment
	unsigned char numberOfparameters;  //!<  Number of parameters
//	uint32_t parameter;          //!<  Parameters in sequence
	uint32_t parameters[256];          //!<  Parameters in sequence
}__attribute__((packed)) SpectraMsgHelper;

/**
 * Helper union structure to make easier to pack and unpack messages
 */
union uSpectraPackets {
	SpectraMessage spectraMsg;                       //!<  spectraMsg
	SpectraMsgHelper spectraMsgHelper;               //!<  spectraMsgHelper
	unsigned char spectraMsgStream[MAX_PACKET_SIZE]; //!<  spectraMsgStream
};

/**
 * Verifies if the machine architecture is big endian
 *
 * @return true if the machine architecture is big endian
 */
int isBigEndian(void)
{
    union {
        uint32_t i;
        char c[4];
    } e = { 0x01000000 };

    return e.c[0];
}

/**
 * Network order, byte swap function for 64bits size numbers
 *
 * @param value - the 64 bits number to swap, depending on the architecture
 * @return the same value in the network order
 */
uint64_t htonll(uint64_t value)
{
    // Check the endianess
    if (!isBigEndian())
    {
        uint32_t high_part = htonl((uint32_t)(value >> 32));
        uint32_t low_part = htonl((uint32_t)(value & 0xFFFFFFFFULL));
        return (uint64_t)(((uint64_t)low_part) << 32) | high_part;
   } else
    {
        return value;
    }
}

/**
 * Host order, byte swap function for 64bits size numbers
 *
 * @param value - the 64 bits number to swap, depending on the architecture
 * @return the same value in the host order
 */
uint64_t ntohll(uint64_t value)
{

    // Check the endianess
    if (!isBigEndian())
    {
        uint32_t high_part = ntohl((uint32_t)(value >> 32));
        uint32_t low_part = ntohl((uint32_t)(value & 0xFFFFFFFFULL));

        return (uint64_t)(((uint64_t)low_part) << 32) | high_part;
    } else
    {
        return value;
    }
}


/**
 * Serialize and send packet over the indicated socket
 *
 * @param sock - in use socket descriptor
 * @param msg  - message to be transmitted
 * @return error status 0 if OK, 1 if something went wrong
 */
int sendClientSpectraPacket(int sock, SpectraMessage msg) {
	int i;
	union uSpectraPackets toTransmitPacket;
	toTransmitPacket.spectraMsg = msg;

	// Puts the values bigger than one byte on Internet format
	toTransmitPacket.spectraMsgHelper.messageID = htons(msg.messageID);

	toTransmitPacket.spectraMsgHelper.type_function = (msg.type << 4) | msg.function;
	for (i = 0; i < msg.numberOfparameters; i++) {
		toTransmitPacket.spectraMsgHelper.parameters[i] = htonl(msg.parameters[i]);
        printf("Value %d=%d\n", i, htonl(msg.parameters[i]));
	}

    // sends the package
	if ( (i = send(sock, toTransmitPacket.spectraMsgStream, PACKET_ALIGNEMENT_BYTES + (msg.numberOfparameters * sizeof(uint32_t)), 0)) < 0) {
		printf("Send failed (%i)\n", i);
		// to avoid memory leaking
 	    free(msg.parameters);
		return -1;
	}

    int k = (int)(PACKET_ALIGNEMENT_BYTES + (msg.numberOfparameters * sizeof(uint32_t)));
	printf("Sent message size: %d\n >", k);
        
	for (i=0; i<k;i++){
	   printf("%x ", (toTransmitPacket.spectraMsgStream[i] & 0xff));	
    }
	printf("<\n");
	// to avoid memory leaking
    free(msg.parameters);
	return 0;
}

/**
 *  Assemble the packet from the raw input packet
 *  @param var - void pointer to the memory area the message is stored
 *  @return  a formated Spectra message
 */
SpectraMessage assembleMessage(void * var) {
	int i;
	SpectraMessage returnMessage;
	char nParameters = (*(SpectraMessage*) var).numberOfparameters;

	// Alocates enough memory area for the variable parameters field
	returnMessage.parameters = malloc(nParameters * sizeof(uint32_t));

	returnMessage.messageID = ntohs((*(SpectraMessage*) var).messageID);
	returnMessage.type = ((*(SpectraMsgHelper*) var).type_function) >>4;
	returnMessage.function = (((*(SpectraMsgHelper*) var).type_function << 4)>>4) & 0xF;
	returnMessage.numberOfparameters = nParameters;

	for (i = 0; i < nParameters; i++) {
		returnMessage.parameters[i] = (*(SpectraMsgHelper*) var).parameters[i];
	}

	return returnMessage;
}

/**
 *  Prints the test message, only for debugging purposes
 *
 *  @param msg - message to print the components
 */
void printReturnValueMessage(SpectraMessage msg) {
// 	int i;

	printf("msg.messageID: %d\n", msg.messageID);
	printf("msg.type: %d\n", msg.type);
	printf("msg.function: %d\n", msg.function);
//	printf("msg.numberOfparameters: %d\n", msg.numberOfparameters);
//	for (i = 0; i < msg.numberOfparameters; i++) {
//		printf("[%d] parameters: %d\n", i, msg.parameters[i]);
//	}

	// To transform the 2x32bits in on 64bits value
	uint64_t realValue = (uint64_t)(((uint64_t)msg.parameters[0]) << 32) | msg.parameters[1];

	printf("packet value: 0x%016llx\n", (long long int)realValue);
	printf("Network to host value: 0x%016llx\n", (long long int)ntohll(realValue));
	printf("SCORE: %lld\n", (long long int)ntohll(realValue));
}

#endif /* SPECTRA_H_ */

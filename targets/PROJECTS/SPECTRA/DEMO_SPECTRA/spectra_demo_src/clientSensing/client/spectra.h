/*
 * spectra.h
 *
 * Helper functions for the test server program. The functions are as
 * close as possible to the ones used in the real server implementation
 * that is supposed to be running in the board.
 *
 *  Created on: Jun 26, 2014
 *  Author: camara
 *  Copyright (c) 2008-2014 Institut Mines-Telecom / Telecom ParisTech
 */

#ifndef SPECTRA_H_
#define SPECTRA_H_
#include <errno.h>

#define IP_ADDRESS "10.42.0.104"
#define IP_MASK    "255.255.255.0"
#define PORT 4546

/**
 * Maximum packet size, dependent on the size of the
 * number of elements field on the header, at the present 8 bits
 * i.e. (256 * 4 bytes + the header that is 4 bytes)
 */
#define MAX_PACKET_SIZE 1028

/**
 * Aligns packet, internally, into 32 bits, or 4 bytes
 */
#define PACKET_ALIGNEMENT_BYTES 4

//#define DEBUG
#ifdef DEBUG
#define PRINTD printf("-> %s:%d: ", __FUNCTION__, __LINE__); printf
#else
#define PRINTD(format, args...) ((void)0)
#endif

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
}__attribute__((packed)) SpectraMessage;

/**
 * \typedef SpectraMsgHelper
 * Helper structure to handle control packets
 */
typedef struct {
	uint16_t messageID;                //!<  Operation ID
	unsigned char type_function;       //!<  Assembled type and function for easier treatment
	unsigned char numberOfparameters;  //!<  Number of parameters
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

union FuncPtr {
	void (*fp)();  // function pointer
	unsigned char c[1]; // address of the pointer as a vector of chars
};


/**
 * Serialize and send packet over the indicated socket
 *
 * @param sock - in use socket descriptor
 * @param msg  - message to be transmitted
 * @return error status 0 if OK, 1 if something went wrong
 */
int returnSpectraPacket(SpectraMsgHelper msg,
		int		desc,
		struct sockaddr	remote) {

	int i;
	// returns the response value to the OpenAirInterface
	if ((i = sendto(desc,&msg, PACKET_ALIGNEMENT_BYTES + (msg.numberOfparameters * sizeof(uint32_t)),
			0, &remote, sizeof(remote)))<0) {
	  printf("Send failed (%i - %s)\n", i, strerror(errno));
		return -1;
	}

//	// to avoid memory leaking
//	free(msg.parameters);
	return 0;
}

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
    // Check the endianness
    if (!isBigEndian())
    {
        uint32_t high_part = htonl((uint32_t)(value >> 32));
        uint32_t low_part = htonl((uint32_t)(value & 0xFFFFFFFFULL));
        value = low_part & 0xffffffffULL;
	value = (value<<32) | high_part;
	value = (uint64_t)(((uint64_t)low_part) << 32) | high_part;
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

    // Check the endianness
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

#endif /* SPECTRA_H_ */

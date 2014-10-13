/*
 * spectra_deamon.c
 *
 * Server part of the spectra demo control system.
 *
 * The real code is supposed to run on the board waiting from the
 * OpenAirInterface configuration messages. This is just a "stub"
 * version of the real server.  It emulates the behavior of the
 * server for tests purposes only. It has a list of scores that
 * it plays over and over.
 *
 *  Created on: Dec 11, 2013
 *  Author: camara
 *  Copyright (c) 2008-2014 Institut Mines-Telecom / Telecom ParisTech
 */

#include <stdio.h>
#include <string.h>      //strlen
#include <stdlib.h>      //malloc
#include <sys/socket.h>
#include <arpa/inet.h>   //inet_addr
#include <unistd.h>      //write
#include <pthread.h>     //for threading , link with lpthread
#include "spectra.h"     // The types and generic functions of spectra

/**
 * List of fake scores that will be sent to the application
 */
uint64_t scoresList[] = {
		81886716,
		94073410,
		147421016,
		117364836,
		104805562,
		142811370,
		107986264,
		100032514
   };

/**
 * Index of the score to send
 */
int scoreIndex = 0;

/**
 * Creates the return packet
 *
 *  Fills the packet that will be returned to the OpenAir interface with the
 *  score or with the end of connection
 *
 * @param request - The original request message
 * @param score - The score value
 * @param request - The type of this message
 *
 * @return - A new spectra message filled with the right fields
 */
SpectraMsgHelper createReturnPacket(SpectraMessage* request, uint64_t score,
		messageType type) {
	SpectraMsgHelper returnMessage;

	returnMessage.messageID = htons(request->messageID); // copy the request id
	returnMessage.type_function = (type << 4) | request->function;
	returnMessage.numberOfparameters = 2; // it is a 64bits number (2 x uint32_t)
	score = htonll(score);
	returnMessage.parameters[0] = score >> 32; // the score number to send back but it is a 64 bit
	returnMessage.parameters[1] = score & 0xFFFFFFFFULL;

	return returnMessage;
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
	returnMessage.type = ((*(SpectraMsgHelper*) var).type_function) >> 4;
	returnMessage.function = (((*(SpectraMsgHelper*) var).type_function << 4)>> 4) & 0xF;
	returnMessage.numberOfparameters = nParameters;

	for (i = 0; i < nParameters; i++) {
		returnMessage.parameters[i] = (*(SpectraMsgHelper*) var).parameters[i];
	}

	return returnMessage;
}

/**
 * Responsible for handling the client connections
 *
 * @param sock - socket descriptor
 */
void *connection_handler(int fd) {
	SpectraMessage request; // received message in the spectra format
	SpectraMsgHelper returnMessage; // message in the format we want to send
	char rawPacket[MAX_PACKET_SIZE]; // raw packet
	struct sockaddr remaddr; // remote address
	socklen_t addrlen = sizeof(remaddr); // length of addresses
	int recvlen; // # bytes received
	int i=0;

	/* now loop, receiving data and printing what we received */
	for (;;) {
		printf("---\n Waiting on port %d\n", PORT);
		recvlen = recvfrom(fd, rawPacket, MAX_PACKET_SIZE, 0,
				(struct sockaddr *) &remaddr, &addrlen);
		printf("Received %d bytes  > ", recvlen);

		for (i=0; i<recvlen;i++){
		   printf("%x ", (rawPacket[i] & 0xff));
	    }
		printf("<\n");


		// prints the packet
		if (recvlen <= 0) {
			perror("Receive failed!!!!\n");
		} else {

			request = assembleMessage(&rawPacket);
			printf(" > Received spectra packet %i of type %i for function %i with %i parameters\n",
					request.messageID,
					(uint32_t) request.type, (uint32_t) request.function,
					(uint32_t) request.numberOfparameters);

			// Process the requested operation
			if (request.type == EndProcessing) {

				returnMessage = createReturnPacket(&request, 0, EndProcessing);
				printf("End processing!!!\n");
				fflush(stdout);
			} else {
				if (request.function == EnergyDetection) {

					// circular loop over the score list
					scoreIndex = (scoreIndex < (sizeof(scoresList)/ sizeof(scoresList[0]))) ?
									scoreIndex : 0;

					printf("Score value: %16llx (%lld)\n",
							(long long int)scoresList[scoreIndex],
							(long long int)scoresList[scoreIndex]);
					printf("Score host to network value: 0x%016llx (%lld)\n",
							(long long int)htonll(scoresList[scoreIndex]),
							(long long int)htonll(scoresList[scoreIndex]));

					returnMessage = createReturnPacket(&request,
							scoresList[scoreIndex++], ScoreResponse);
				}
			}

			// Sends the answer
			int retError = returnSpectraPacket(returnMessage, fd, remaddr);
			if (retError == -1) {
				perror("Send failed!!!\n");
			}

			// If the command was an end processing exits!!
			if (request.type == EndProcessing) {
				break;
			}
			free(request.parameters);
		}

	}
	//Free the socket pointer
	close(fd);
	return 0;
}

/**
 * Creates the socket and binds it to the defined port
 *
 * @return the socket descriptor
 */
int socketBind() {

	struct sockaddr_in myaddr; /* our address */
// 	struct sockaddr_in remaddr; /* remote address */
	int fd; /* our socket */

	/* create a UDP socket */

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket\n");
		return 0;
	}

	/* bind the socket to any valid IP address and a specific port */
	memset((char *) &myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(PORT);

	if (bind(fd, (struct sockaddr *) &myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return 0;
	}
	puts("bind done");
	return fd;
}

/**
 * The main function for the server
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]) {
// 	int client_sock;
	int socket_desc;

	socket_desc = socketBind();
	connection_handler(socket_desc);

	return 0;
}

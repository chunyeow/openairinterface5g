/*
 * spectra_client.c
 *
 * Client part of the spectra demo control system. It is
 * supposed to run on the OpenAirInterface machine that
 * will send commands to configure Embb and collect the
 * answers sent by the Embb "server", running on the
 * board.
 *
 * Considering this compiled with gcc -o spectraClient spectra_client.c.
 *
 * Use: spectraClient [server IP] [port] [nSamples]
 * e.g: spectraClient 127.0.0.1 4546 10
 *         - This will tell the client to try to find the server in the
 *         address 127.0.0.1 at the port 4546 and that the score should be
 *         calculated over 10 samples.
 *
 *  Created on: Dec 11, 2013
 *  Author: camara
 *  Copyright (c) 2008-2014 Institut Mines-Telecom / Telecom ParisTech
 */
#include <stdio.h>         // printf
#include <string.h>        // strlen
#include <unistd.h>        // close
#include <sys/socket.h>    // socket
#include <arpa/inet.h>     // inet_addr
#include <stdlib.h>        // malloc
#include "spectra_client.h"       // The types and generic functions of spectra

/**
 * Creates the test message
 *
 * @param id messsage identificatior
 * @return example message
 */
SpectraMessage createTestMessageClient(int id, int type, int nSamples) {
	SpectraMessage msg;

	msg.type = type;
	msg.function = EnergyDetection;
	msg.messageID = id;
	msg.numberOfparameters = 1;

	msg.parameters = malloc(msg.numberOfparameters * sizeof(uint32_t));
	msg.parameters[0] = nSamples;

	return msg;
}

/**
 * Creates the socket and connects to the server
 *
 * @return - the socket descriptor
 */
int createsSocket(int argc, char *argv[]) {
	int sock;
	struct sockaddr_in server;
	//Create socket

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == -1) {
		printf("Could not create socket");
		return -1;
	}

	puts("Socket created");

	// treats the parameters
	if (argc >= 2)
		server.sin_addr.s_addr = inet_addr(argv[1]);
	else
		server.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

	server.sin_family = AF_INET;

	if (argc >= 3)
		server.sin_port = htons(atoi(argv[2]));
	else
		server.sin_port = htons(CONNECTION_PORT);

	printf("Target address: %s,  port %d : addr\n", inet_ntoa(server.sin_addr),
			server.sin_port);

	//Connect to remote server
	if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("connect failed. Error");
		return -2;
	}

	puts("Connected\n");
	return sock;
}

/**
 * Main function for the client
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]) {
	char server_reply[MAX_PACKET_SIZE];
	int nSamples = 10;

	int sock = createsSocket(argc, argv), id;

	if (argc >= 4)
		nSamples = atoi(argv[3]);

	if (sock < 0)
		return sock;

	int type = Classifying;
	// Just for test purposes send 2 packets to the server, changing only the packet id
	for (id = 33; id < 36; ++id) {
		printf("-->\nSending message ID : %d\n", id);

		if (id == 35)
			type = EndProcessing;

		SpectraMessage message = createTestMessageClient(id, type, nSamples);

		//Send some data
		if (sendClientSpectraPacket(sock, message) != 0) {
			return 1;
		}

		//Receive a reply from the server
		if (recv(sock, server_reply, MAX_PACKET_SIZE, 0) < 0) {
			puts("recv failed");
			break;
		}

		printf("<--\nServer reply :\n");
		printReturnValueMessage(assembleMessage(&server_reply));
	}

	close(sock);
	return 0;
}

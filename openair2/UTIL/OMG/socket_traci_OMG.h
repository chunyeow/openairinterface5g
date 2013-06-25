/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2011 Eurecom

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information
  Openair Admin: openair_admin@eurecom.fr
  Openair Tech : openair_tech@eurecom.fr
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/

/*! \file socket_traci_OMG.h
* \brief The socket interface of TraCI to connect OAI to SUMO. A 'C' reimplementation of the TraCI version of simITS (F. Hrizi, fatma.hrizi@eurecom.fr)
* \author  S. Uppoor
* \date 2012
* \version 0.1
* \company INRIA
* \email: sandesh.uppor@inria.fr
* \note
* \warning
*/

#ifndef SOCKET_TRACI_OMG_H
#define SOCKET_TRACI_OMG_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include "omg.h"
#include "storage_traci_OMG.h"

int sock, portno, msgLength;
struct hostent *host;
struct sockaddr_in server_addr;  

/**
 * Global parameters defined in storage_traci_OMG.h
 */
extern storage *tracker;
extern storage *head;
extern storage *storageStart;


/**
 * \fn connection_(char *,int)
 * \brief Talks to SUMO by establishing connection
 * \param Accepts host name and port number
 */
int connection_(char *, int);

/**
 * \fn sendExact(int);
 * \brief Pack the data from storage to buf and write to socket
 * \param Accepts command length 
 */
void sendExact(int);

/**
 * \fn  recieveExact();
 * \brief Pack the data to storage from buf after reading from socket
 * Returns storage pointer
 */     
storage* receiveExact();


/**
 * \fn  close_connection();
 * \brief close socket connection 
 */       
void close_connection();

#endif 

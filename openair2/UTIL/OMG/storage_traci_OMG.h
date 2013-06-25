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

/*! \file storage_traci_OMG.h
* \brief The data storage object carrying data from/to SUMO. 'C' reimplementation of the TraCI version of simITS (F. Hrizi, fatma.hrizi@eurecom.fr)
* \author  S. Uppoor
* \date 2012
* \version 0.1
* \company INRIA
* \email: sandesh.uppor@inria.fr
* \note
* \warning
*/

#ifndef STORAGE_TRACI_OMG_H
#define STORAGE_TRACI_OMG_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "omg.h"

// sortation of bytes forwards or backwards?------------------

extern bool bigEndian;

union n{
       short   s;
       char    c[sizeof(short)];
     } un ;


void check_endianness();

//----------------STORAGE------------------------------------
struct Storage {

        unsigned char item;
        struct Storage *next;

} ;
typedef struct Storage storage;
// pointer which always points to next entry to be read
// updated in readChar function in storage_traci_OMG

storage *tracker;
storage *head;
storage *storageStart;
int descLen;

extern int msgLength;

	void reset();
        int storageLength(storage *);
        
        void rearange();
        unsigned char readChar(); 
	void writeChar(unsigned char); 

	int readByte() ;
	void writeByte(int) ;


	int readUnsignedByte(); 
	void writeUnsignedByte(int); 

	char * readString() ;
	void writeString(char *);

        String_list readStringList(String_list) ;
	void writeStringList(String_list); 

	int readShort() ;
	void writeShort(int); 

	int readInt() ;
	void writeInt(int); 

	float readFloat() ;
	void writeFloat( float ); 

        double readDouble() ;
        void writeDouble( double ); 

	storage* writePacket(unsigned char*, int);

	//void writeStorage(storage & );
        void freeStorage(storage *);

#endif


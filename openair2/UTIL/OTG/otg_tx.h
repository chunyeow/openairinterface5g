/*******************************************************************************
    OpenAirInterface
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is
    included in this distribution in the file called "COPYING". If not,
    see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr

  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

*******************************************************************************/

/*! \file otg_tx.h
* \brief Data structure and functions for OTG 
* \author N. Nikaein and A. Hafsaoui
* \date 2011
* \version 1.0
* \company Eurecom
* \email: navid.nikaein@eurecom.fr
* \note
* \warning
*/

#ifndef __OTG_TX_H__
#	define __OTG_TX_H__



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>


#include "otg.h"
//#include "COMMON/platform_constants.h"


/*! \fn int time_dist(int src, int dst, int state)
* \brief compute Inter Departure Time, in ms
* \param[in] Source, destination, state
* \param[out] Inter Departure Time 
* \note 
* @ingroup  _otg
*/
int time_dist(int src, int dst, int application, int state);

/*! \fn int size_dist(int src, int dst, int state)
* \brief compute the payload size, in bytes 
* \param[in] Source, node_dst, state
* \param[out] size of the payload, in bytes
* \note 
* @ingroup  _otg
*/
int size_dist(int src, int dst, int application, int state);

/*! \fn char *random_string(int size, ALPHABET_GEN mode, ALPHABET_TYPE data_type);
* \brief return a random string[size]
* \param[in] size  of the string to generate, 
* \param[in] ALPHABET_GEN  : static or random string
* \param[in] ALPHABET_TYPE : numeric or letters + numeric
* \param[out] string of a random char
* \note 
* @ingroup  _otg
*/
char * random_string(int size, ALPHABET_GEN mode, ALPHABET_TYPE data_type);
//char *random_string(int size, ALPHABET data_type, char *data_string);

/*! \fn int packet_gen(int src, int dst, int state, int ctime)
* \brief return int= 1 if the packet is generated: OTG header + header + payload, else 0
* \param[in] src source identity 
* \param[in] dst destination id 
* \param[in] application id that might generate the packet
* \param[out] final packet size 
* \param[out] packet_t: the generated packet: otg_header + header + payload
* \note 
* @ingroup  _otg
*/
unsigned char *packet_gen(int src, int dst, int app, int ctime, unsigned int *pkt_size);

/*! \fn int packet_gen(int src, int dst, int state, int ctime)
* \brief return int= 1 if the packet is generated: OTG header + header + payload, else 0
* \param[in] src source identity
* \param[in] dst destination id
* \param[in] application id that might generate the packet
* \param[out] final packet size
* \param[out] packet_t: the generated packet: otg_header + header + payload
* \note
* @ingroup  _otg
*/
unsigned char *packet_gen_multicast(int src, int dst, int ctime, unsigned int * pkt_size);

/*! \fn char *header_gen(int  hdr_size);
* \brief generate IP (v4/v6) + transport header(TCP/UDP) 
* \param[in] int : size 
* \param[out] the payload corresponding to ip version and transport protocol
* \note 
* @ingroup  _otg
*/
unsigned char *header_gen(int hdr_size);

/*! \fn char *payload_pkts(int payload_size);
* \brief generate the payload
* \param[in] int : payload size  
* \param[out] char * payload
* \note 
* @ingroup  _otg
*/
unsigned char *payload_pkts(int payload_size);


/*! \fn 
char * serialize_buffer(char* header, char* payload, unsigned int buffer_size, int flag, int flow_id, int ctime, int seq_num, int hdr_type, int state)
* \brief serilize the packet and add otg control information
* \param[in] char* header pointer to the header
* \param[in] char* payload pointer to the payload
* \param[in] buffer_size size of the serilized tx buffer
* \param[in] flag for regular and background traffic 
* \param[in] flow_id 
* \param[in] ctime time of the transmission 
* \param[in] seq_num sequence number of the packet used to calculate the loss rate
* \param[in] hdr_type indicatting the ip protocol version and transport protocol
* \param[in] state indicate in which state (PU, ED, PE) the packet is generated when generating M2M traffic 
flow id, simulation time, , sequence number, header type (to know the transport/ip version in the RX)  
* \param[out] tx_buffer the serilized packet
* @ingroup  _otg
*/

unsigned char * serialize_buffer(char* header, char* payload, unsigned int buffer_size, unsigned int traffic_type, int flag, int flow_id, int ctime, int seq_num, int hdr_type, int state, unsigned int aggregation_level);


/*! \fn int adjust_size(int size);
* \brief adjuste the generated packet size when size<min or size>max   
* \param[in]  size  
* \param[out] modified size in case
* \note 
* @ingroup  _otg
*/
int adjust_size(int size);


/*! \fn int header_size_genint src();
* \brief return the header size corresponding to ip version and transport protocol  
* \param[in]  the sender (src)
* \param[out] size of packet header 
* \note 
* @ingroup  _otg
*/
void header_size_gen(int src, int dst, int application);

void init_predef_multicast_traffic(void);

/*! \fn void init_predef_traffic();
* \brief initialise OTG with predifined value for pre-configured traffic: cbr, openarena,etc. 
* \param[in]  
* \param[out] 
* \note 
* @ingroup  _otg
*/
void init_predef_traffic(unsigned char nb_ue_local, unsigned char nb_enb_local);

/*! \fn int background_gen(int src, int dst, int ctime);
* \brief manage idt and packet size for the backgrounf traffic. 
* \param[in] src
* \param[in] dst
* \param[in] ctime
* \param[out] 
* \note 
* @ingroup  _otg
*/
int background_gen(int src, int dst, int ctime);

int header_size_gen_background(int src, int dst);

void state_management(int src, int dst,int application, int ctime);

void voip_traffic(int src, int dst, int application, int ctime);

int otg_hdr_size(int src, int dst);

void init_packet_gen(int src, int dst, int ctime);

int check_data_transmit(int src,int dst, int app, int ctime); 

unsigned int get_application_state(int src, int dst, int application, int ctime);

void check_ctime(int ctime);

#endif

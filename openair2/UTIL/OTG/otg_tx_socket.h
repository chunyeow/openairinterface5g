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

/*! \file otg_tx_socket.h
* \brief brief explain how this block is organized, and how it works: OTG TX traffic generation functions with sockets
* \author A. Hafsaoui
* \date 2012
* \version 0.1
* \company Eurecom
* \email: openair_tech@eurecom.fr
* \note
* \warning
*/

#ifndef __OTG_TX_SOCKET_H__
#define __OTG_TX_SOCKET_H__


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include"otg.h"
#include"otg_tx.h"


//-----------------------begin func proto-------------------

/*! \fn socket_packet_send(int, int , int)
* \brief this function allow to run the client, with the appropriate parameters.
* \param[in] src, dst and the state
* \param[out]  void
* \return void 
* \note 
* @ingroup  _otg
*/
void socket_packet_send(int src, int dst, int state,int ctime);

/*! \fn client_socket_tcp_ip4(int, int , int)
* \brief this function allow to run the client, with IPv4 and TCP protocol.
* \param[in] src, dst and the state
* \param[out]  void
* \return void 
* \note 
* @ingroup  _otg
*/
void client_socket_tcp_ip4(int src, int dst, int state,int ctime);

/*! \fn client_socket_tcp_ip6(int, int , int)
* \brief this function allow to run the client, with IPv6 and TCP protocol.
* \param[in] src, dst and the state
* \param[out]  void
* \return void 
* \note 
* @ingroup  _otg
*/
void client_socket_tcp_ip6(int src, int dst, int state,int ctime);

/*! \fn client_socket_udp_ip4(int, int , int)
* \brief this function allow to run the client, with IPv4 and UDP protocol.
* \param[in] src, dst and the state
* \param[out]  void
* \return void 
* \note 
* @ingroup  _otg
*/
void client_socket_udp_ip4(int src, int dst, int state,int ctime);

/*! \fn client_socket_udp_ip6(int, int , int)
* \brief this function allow to run the client, with IPv6 and UDP protocol.
* \param[in] src, dst and the state
* \param[out]  void
* \return void 
* \note 
* @ingroup  _otg
*/
void client_socket_udp_ip6(int src, int dst, int state,int ctime);



/*! \fn int packet_gen(int src, int dst, int state, int ctime)
* \brief return char *  pointer over the payload, else NULL
* \param[in] source, 
* \param[out] packet_t: the generated packet: otg_header + header + payload
* \note 
* @ingroup  _otg
*/
char* packet_gen_socket(int src, int dst, int state, int ctime);




control_hdr_t *otg_info_hdr_gen(int src, int dst, int trans_proto, int ip_v);




void init_control_header();


//-----------------------end func proto-------------------



#endif

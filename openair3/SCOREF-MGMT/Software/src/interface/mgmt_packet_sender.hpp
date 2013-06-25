/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2012 Eurecom

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

/*!
 * \file mgmt_packet_sender.hpp
 * \brief This is an abstract base class implemented by ManagementServer class
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_PACKET_SENDER_H_
#define MGMT_PACKET_SENDER_H_

/**
 * This is an abstract base class implemented by ManagementServer class
 * to provide functionality of status query packet sending for InquiryThread
 */
class IManagementPacketSender {
	public:
		/**
		 * Virtual destructor for this abstract base class
		 */
//		virtual ~IManagementPacketSender() = 0;

	public:
		/**
		 * Send a Wireless State Request packet to GN client
		 *
		 * @param none
		 * @return true if success, false otherwise
		 */
		virtual bool sendWirelessStateRequest() = 0;
};

#endif /* MGMT_PACKET_SENDER_H_ */

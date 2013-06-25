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
 * \file mgmt_fac_packet_comm_profile_selection_request.hpp
 * \brief A container for Communication Profile Selection Request event
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_FAC_PACKET_COMM_PROFILE_SELECTION_REQUEST_HPP_
#define MGMT_FAc_PACKET_COMM_PROFILE_SELECTION_REQUEST_HPP_

#include "mgmt_gn_packet.hpp"

/**
 * Communication Profile Selection Request
 */
struct CommunicationProfileSelectionRequest {
	MessageHeader header;

	u_int8_t latency;
	u_int8_t relevance;
	u_int8_t reliability;
	u_int8_t sequenceNumber;
} __attribute__((packed));

/**
 * A container for Communication Profile Selection Request event
 */
class FacCommunicationProfileSelectionRequestPacket: public GeonetPacket {
	public:
		/**
		 * Constructor for FacCommunicationProfileSelectionRequestPacket class
		 *
		 * @param packetBuffer vector containing packet data
		 * @param logger Logger object reference
		 */
		FacCommunicationProfileSelectionRequestPacket(const vector<unsigned char>& packetBuffer, Logger& logger);
		/**
		 * Destructor for FacCommunicationProfileSelectionRequestPacket class
		 */
		~FacCommunicationProfileSelectionRequestPacket();

	public:
		/**
		 * Returns `latency' field
		 *
		 * @return 8-bit `latency' field
		 */
		u_int8_t getLatency() const;
		/**
		 * Returns `reliability' field
		 *
		 * @return 8-bit `reliability' field
		 */
		u_int8_t getReliability() const;
		/**
		 * Returns `relevance' field
		 *
		 * @return 8-bit `relevance' field
		 */
		u_int8_t getRelevance() const;
		/**
		 * Returns `sequence number' field
		 *
		 * @return 8-bit sequence number field
		 */
		u_int8_t getSequenceNumber() const;
		/**
		 * Parses the packet buffer and fills in private member
		 *
		 * @param packetBuffer Vector containing packet data
		 * @return true on success, false otherwise
		 */
		bool parse(const vector<unsigned char>& packetBuffer);
		/**
		 * Returns std::string representation of packet
		 *
		 * @return String representation of packet
		 */
		string toString() const;

	private:
		CommunicationProfileSelectionRequest packet;
};

#endif /* MGMT_FAC_PACKET_COMM_PROFILE_SELECTION_REQUEST_HPP_ */

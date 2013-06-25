/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2010 Eurecom

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
 * \file mgmt_gn_packet_comm_profile_response.hpp
 * \brief A container for Communication Profile Response event
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_GN_PACKET_COMM_PROFILE_RESPONSE_HPP_
#define MGMT_GN_PACKET_COMM_PROFILE_RESPONSE_HPP_

#include "mgmt_gn_packet.hpp"

/**
 * Communication Profile Response
 */
struct CommunicationProfileResponse {
	MessageHeader header;

	u_int16_t communicationProfileCount;
	u_int8_t sequenceNumber;
	u_int8_t reserved;
	/* CommunicationProfileItem(s) follow(s)... */
} __attribute__((packed));

/**
 * A container for Communication Profile Response event
 */
class GeonetCommunicationProfileResponsePacket : public GeonetPacket {
	public:
		/**
		 * Constructor for GeonetCommunicationProfileResponsePacket class
		 *
		 * @param mib Management Information Base reference
		 * @param communicationProfileRequest Communication Profile Request
		 * @param sequenceNumber 8-bit sequence number
		 * @param logger Logger object reference
		 */
		GeonetCommunicationProfileResponsePacket(ManagementInformationBase& mib, u_int32_t communicationProfileRequest, u_int8_t sequenceNumber, Logger& logger);
		/**
		 * Destructor for GeonetCommunicationProfileResponsePacket class
		 */
		~GeonetCommunicationProfileResponsePacket();

	public:
		/**
		 * Serialises the packet into given buffer
		 *
		 * @param Vector to be used to serialise the packet into
		 * @return true on success, false otherwise
		 */
		bool serialize(vector<unsigned char>& buffer) const;
		/**
		 * Returns std::string representation of packet
		 *
		 * @return String representation of packet
		 */
		string toString() const;

	private:
		/**
		 * Management Information Base reference to fetch
		 * necessary information to build this packet
		 */
		ManagementInformationBase& mib;
		/**
		 * Logger reference
		 */
		Logger& logger;
		/**
		 * Sequence number (the one parsed from corresponding Communication Profile Request packet)
		 */
		u_int8_t sequenceNumber;
		/**
		 * Communication Profile Request flag set to determine
		 * requested options and respond accordingly
		 */
		u_int32_t communicationProfileRequest;
};

#endif /* MGMT_GN_PACKET_COMM_PROFILE_RESPONSE_HPP_ */

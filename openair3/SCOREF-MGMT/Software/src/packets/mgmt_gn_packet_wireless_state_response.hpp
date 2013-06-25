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
 * \file mgmt_gn_packet_wireless_state.hpp
 * \brief A container for Wireless State Event Response packet
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_GN_PACKET_WIRELESS_STATE_RESPONSE_HPP_
#define MGMT_GN_PACKET_WIRELESS_STATE_RESPONSE_HPP_

#include "../mgmt_information_base.hpp"
#include "../util/mgmt_log.hpp"
#include "mgmt_gn_packet.hpp"

/**
 * A container for Wireless State Event Response packet
 */
class GeonetWirelessStateResponseEventPacket : public GeonetPacket {
	public:
		/**
		 * Constructor for GeonetWirelessStateResponseEventPacket class
		 *
		 * @param mib Management Information Base reference to update it
		 * with incoming information
		 * @param packetBuffer Packet data as a vector
		 * @param logger Logger object reference
		 */
		GeonetWirelessStateResponseEventPacket(ManagementInformationBase& mib, const vector<unsigned char>& packetBuffer, Logger& logger);
		/**
		 * Destructor for GeonetWirelessStateEventPacket class
		 */
		~GeonetWirelessStateResponseEventPacket();

	public:
		/**
		 * Returns string representation of this packet
		 *
		 * @return std::string representation of this packet
		 */
		string toString() const;

	private:
		/**
		 * Parses given buffer and updates Geonet::header and Management Information Base
		 *
		 * @param packetBuffer std::vector keeping packet data
		 * @return true on success, false otherwise
		 */
		bool parse(const vector<unsigned char>& packetBuffer);

	private:
		/**
		 * Management Information Base reference to keep it up-to-date with
		 * incoming information
		 */
		ManagementInformationBase& mib;
};

#endif /* MGMT_GN_PACKET_WIRELESS_STATE_RESPONSE_HPP_ */

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
 * \file mgmt_gn_packet_network_state.hpp
 * \brief A container for Network State Event packet
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_GN_PACKET_NETWORK_STATE_HPP_
#define MGMT_GN_PACKET_NETWORK_STATE_HPP_

#include "mgmt_gn_packet.hpp"
#include "../mgmt_information_base.hpp"

/**
 * A container for Network State Event packet
 */
class GeonetNetworkStateEventPacket: public GeonetPacket {
	public:
		/**
		 * Constructor of GeonetNetworkStateEventPacket class
		 *
		 * @param mib Management Information Base reference to keep it up-to-date
		 * with incoming information
		 * @param packetBuffer Buffer containing Network State Event packet (which
		 * is going to be parsed using parse())
		 * @param logger Logger object reference
		 */
		GeonetNetworkStateEventPacket(ManagementInformationBase& mib, vector<unsigned char> packetBuffer, Logger& logger);
		/**
		 * Destructor of GeonetNetworkStateEventPacket class
		 */
		~GeonetNetworkStateEventPacket();

	public:
		/**
		 * Returns string representation of this packet
		 *
		 * @return std::string representation of this packet
		 */
		string toString() const;

	private:
		/**
		 * Parses incoming packet buffer and updates MIB
		 *
		 * @param packetBuffer Buffer containing packet data
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

#endif /* MGMT_GN_PACKET_NETWORK_STATE_HPP_ */

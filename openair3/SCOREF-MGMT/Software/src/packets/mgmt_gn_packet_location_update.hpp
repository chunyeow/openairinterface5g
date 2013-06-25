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
 * \file mgmt_gn_packet_location_update.hpp
 * \brief A container for Location Update Event packet
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_GN_PACKET_LOCATION_UPDATE_HPP_
#define MGMT_GN_PACKET_LOCATION_UPDATE_HPP_

#include "../mgmt_types.hpp"
#include "../util/mgmt_log.hpp"
#include "mgmt_gn_packet.hpp"

/**
 * A container for Location Update Event packet
 */
class GeonetLocationUpdateEventPacket : public GeonetPacket {
	public:
		/**
		 * Constructor for GeonetLocationUpdateEventPacket class
		 *
		 * @param mib Management Information Base reference to fetch necessary information
		 * @param logger Logger object reference
		 */
		GeonetLocationUpdateEventPacket(ManagementInformationBase& mib, Logger& logger);
		/**
		 * Constructor for GeonetLocationUpdateEventPacket class to parse packet buffer
		 *
		 * @param mib Management Information Base reference to keep it up-to-date
		 * with incoming information
		 * @param packetBuffer std::vector containing packet data
		 * @param logger Logger object reference
		 */
		GeonetLocationUpdateEventPacket(ManagementInformationBase& mib, const vector<unsigned char>& packetBuffer, Logger& logger);
		/**
		 * Destructor for GeonetLocationUpdateEventPacket class
		 */
		~GeonetLocationUpdateEventPacket();

	public:
		/**
		 * Serialises packet information into incoming buffer
		 *
		 * @param buffer std::vector that packet information will be serialised into
		 * @return true on success, false otherwise
		 */
		bool serialize(vector<unsigned char>& buffer) const;
		/**
		 * Parses incoming packet buffer and updates MIB with this information
		 *
		 * @param packetBuffer std::vector containing packet data
		 * @return true on success, false otherwise
		 */
		bool parse(const vector<unsigned char>& packetBuffer);

	private:
		/**
		 * ManagementInformationBase object to fetch necessary information
		 */
		ManagementInformationBase& mib;
		/**
		 * Logger object reference
		 */
		Logger& logger;
		/**
		 * Location Update Event packet to keep relevant set of information
		 */
		LocationUpdateMessage packet;
};

#endif /* MGMT_GN_PACKET_LOCATION_UPDATE_HPP_ */

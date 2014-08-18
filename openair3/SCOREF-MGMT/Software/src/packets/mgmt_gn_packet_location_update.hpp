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
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06410 Biot Sophia Antipolis cedex, FRANCE

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

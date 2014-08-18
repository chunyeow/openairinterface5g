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

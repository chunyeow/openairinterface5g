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

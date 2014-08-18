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
 * \file mgmt_gn_packet_get_configuration.hpp
 * \brief A container for Get Configuration Event packet
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_GN_PACKET_GET_CONFIGURATION_HPP_
#define MGMT_GN_PACKET_GET_CONFIGURATION_HPP_

#include <string>
#include <vector>
#include "mgmt_gn_packet.hpp"
using namespace std;

/**
 * A container for Get Configuration Event packet
 */
class GeonetGetConfigurationEventPacket: public GeonetPacket {
	public:
		/**
		 * Constructor of GeonetGetConfigurationEventPacket class
		 *
		 * @param packetBuffer Incoming packet buffer that is going to be parsed
		 * @param logger Logger object reference
		 */
		GeonetGetConfigurationEventPacket(const vector<unsigned char>& packetBuffer, Logger& logger);

	public:
		/**
		 * Returns configuration ID that is requested
		 *
		 * @return Requested configuration ID
		 */
		u_int16_t getConfID() const;
		/**
		 * Returns TX mode that is requested
		 *
		 * @return Requested TX mode (bulk or single)
		 */
		u_int16_t getTxMode() const;
		/**
		 * Returns string representation of this packet
		 *
		 * @return std::string representation of this packet
		 */
		string toString() const;

	private:
		/**
		 * Parses incoming packet data and updates `packet' member
		 *
		 * @param packetBuffer std::vector carrying packet data
		 * @return true on success, false otherwise
		 */
		bool parse(const vector<unsigned char>& packetBuffer);

	private:
		/**
		 * Packet that will hold parsed information
		 */
		ConfigurationRequestMessage packet;
};

#endif /* MGMT_GN_PACKET_GET_CONFIGURATION_HPP_ */

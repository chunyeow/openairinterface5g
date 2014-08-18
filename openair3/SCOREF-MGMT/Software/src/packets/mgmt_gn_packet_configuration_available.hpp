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
 * \file mgmt_gn_packet_configuration_available.hpp
 * \brief A container for Configuration Available Event
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_GN_PACKET_CONFIGURATION_AVAILABLE_HPP_
#define MGMT_GN_PACKET_CONFIGURATION_AVAILABLE_HPP_

#include "../mgmt_information_base.hpp"
#include "../util/mgmt_log.hpp"
#include "mgmt_gn_packet.hpp"
#include <vector>

/**
 * A container for Configuration Available Event
 */
class GeonetConfigurationAvailableEventPacket : public GeonetPacket {
	public:
		/**
		 * Constructor for GeonetConfigurationAvailableEventPacket class
		 *
		 * @param mib Management Information Base reference to fetch necessary
		 * information to build this packet
		 * @param logger Logger object reference
		 */
		GeonetConfigurationAvailableEventPacket(ManagementInformationBase& mib, Logger& logger);
		/**
		 * Destructor for GeonetConfigurationAvailableEventPacket class
		 */
		~GeonetConfigurationAvailableEventPacket();

	public:
		/**
		 * Serialises packet header and payload into given buffer
		 *
		 * @param buffer Vector buffer that the packet will be serialised into
		 * @return true on success, false otherwise
		 */
		bool serialize(vector<unsigned char>& buffer);
		/**
		 * Returns string representation of the packet
		 *
		 * @return std::string representation of the packet
		 */
		string toString() const;

	private:
		/**
		 * Management Information Base reference to fetch necessary
		 * information to build this packet
		 */
		ManagementInformationBase& mib;
};

#endif /* MGMT_GN_PACKET_CONFIGURATION_AVAILABLE_HPP_ */

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
 * \file mgmt_lte_packet_wireless_state_request.hpp
 * \brief A container for Wireless State Event Request packet
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_LTE_PACKET_WIRELESS_STATE_REQUEST_HPP_
#define MGMT_LTE_PACKET_WIRELESS_STATE_REQUEST_HPP_

#include "mgmt_gn_packet.hpp"

/**
 * A container for Wireless State Event Request packet
 */
class LteWirelessStateRequestEventPacket : public GeonetPacket {
	public:
		/**
		 * Constructor for LteWirelessStateRequestEventPacket class
		 *
		 * @param logger Logger object reference
		 */
		LteWirelessStateRequestEventPacket(Logger& logger);
		/**
		 * Destructor for LteWirelessStateRequestEventPacket class
		 */
		virtual ~LteWirelessStateRequestEventPacket();

	public:
		/**
		 * Serialises packet information into incoming buffer
		 *
		 * @param buffer std::vector that packet information will be serialised into
		 * @return true on success, false otherwise
		 */
		bool serialize(vector<unsigned char>& buffer) const;
		/**
		 * Returns string representation of this packet
		 *
		 * @return std::string representation of this packet
		 */
		string toString() const;
};

#endif /* MGMT_GN_PACKET_WIRELESS_STATE_REQUEST_HPP_ */

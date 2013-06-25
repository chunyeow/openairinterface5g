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

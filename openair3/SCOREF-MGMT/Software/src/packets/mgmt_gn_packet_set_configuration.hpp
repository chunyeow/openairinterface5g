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
 * \file mgmt_gn_packet_set_configuration.hpp
 * \brief A container for Set Configuration Event packet
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_GN_PACKET_SET_CONFIGURATION_HPP_
#define MGMT_GN_PACKET_SET_CONFIGURATION_HPP_

#include "../util/mgmt_log.hpp"
#include "mgmt_gn_packet.hpp"

/**
 * A container for Set Configuration Event packet
 */
class GeonetSetConfigurationEventPacket: public GeonetPacket {
	public:
		/**
		 * Constructor for GeonetSetConfigurationEventPacket class
		 *
		 * @param mib Management Information Base reference to fetch necessary
		 * configuration information
		 * @param logger Logger object reference
		 * @param itsKeyId Requested ITS key or key set
		 */
		GeonetSetConfigurationEventPacket(ManagementInformationBase& mib, Logger& logger, ItsKeyID itsKeyID = MGMT_GN_ITSKEY_ALL);
		/**
		 * Destructor of GeonetSetConfigurationEventPacket class
		 */
		virtual ~GeonetSetConfigurationEventPacket();

	public:
		/**
		 * Serialises the packet into given buffer
		 *
		 * @param Vector to be used to serialise the packet into
		 * @return true on success, false otherwise
		 */
		bool serialize(vector<unsigned char>& buffer) const;
		/**
		 * Returns string (of type std::string) representation of packet
		 *
		 * @return String representation of packet
		 */
		string toString() const;

	private:
		/**
		 * Serialises given configuration item into given buffer
		 *
		 * @param buffer Unsigned char array of buffer to serialise configuration into
		 * @param configurationItem Configuration item that'll be serialised into given buffer
		 * @return true on success, false otherwise
		 */
		static bool encodeConfigurationItem(unsigned char* buffer, const ConfigurationItem* configurationItem);
		/**
		 * Creates a ConfigurationItem by fetching necessary information from
		 * Management Information Base
		 */
		ConfigurationItem buildConfigurationItem(ItsKeyID itsKey) const;

	private:
		/**
		 * Boolean for request type: true for bulk request, false for a request
		 * for a single ITS key
		 */
		bool isBulk;
		/**
		 * Requested ITS key
		 */
		ItsKeyID requestedItsKey;
		/**
		 * Requested ITS key type
		 */
		ItsKeyType requestedItsKeyType;
		/**
		 * Management Information Base reference
		 */
		ManagementInformationBase& mib;
};

#endif /* MGMT_GN_PACKET_SET_CONFIGURATION_HPP_ */

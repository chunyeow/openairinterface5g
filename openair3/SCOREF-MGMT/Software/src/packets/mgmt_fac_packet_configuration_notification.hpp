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
 * \file mgmt_fac_packet_configuration_notification.hpp
 * \brief A container for Configuration Notification
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_FAC_PACKET_CONFIGURATION_NOTIFICATION_HPP_
#define MGMT_FAC_PACKET_CONFIGURATION_NOTIFICATION_HPP_

#include "../util/mgmt_log.hpp"
#include "mgmt_gn_packet.hpp"

/**
 * A container for Configuration Notification
 */
class FacConfigurationNotificationPacket : public GeonetPacket {
	public:
		/**
		 * Constructor for FacConfigurationNotificationPacket class
		 *
		 * @param mib Management Information Base reference to update relevant configuration
		 * @param packetBuffer Packet bytes
		 * @param logger Logger object reference
		 */
		FacConfigurationNotificationPacket(ManagementInformationBase& mib, const vector<unsigned char>& packetBuffer, Logger& logger);
		/**
		 * Destructor for FacConfigurationNotificationPacket class
		 */
		virtual ~FacConfigurationNotificationPacket();

	public:
		/**
		 * Parses incoming packet data and updates `packet' member
		 *
		 * @param packetBuffer std::vector carrying packet data
		 * @return true on success, false otherwise
		 */
		bool parse(const vector<unsigned char>& packetBuffer);
		/**
		 * Returns string (of type std::string) representation of packet
		 *
		 * @return String representation of packet
		 */
		string toString() const;

	private:
		/**
		 * Management Information Base reference
		 */
		ManagementInformationBase& mib;
		/**
		 * Configuration Notification packet
		 */
		ConfigurationNotification packet;
		/**
		 * Logger reference
		 */
		Logger& logger;
};

#endif /* MGMT_FAC_PACKET_CONFIGURATION_NOTIFICATION_HPP_ */

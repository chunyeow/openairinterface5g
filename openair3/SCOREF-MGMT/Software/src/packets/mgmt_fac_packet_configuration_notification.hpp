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

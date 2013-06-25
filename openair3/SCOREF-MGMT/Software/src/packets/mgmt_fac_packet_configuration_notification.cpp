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
  Forums       : http://forums.eurecom.fr/openairinterface
  Address      : EURECOM, Campus SophiaTech, 450 Route des Chappes, 06410 Biot FRANCE

*******************************************************************************/

/*!
 * \file mgmt_fac_packet_configuration_notification.cpp
 * \brief A container Configuration Notification
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#include "mgmt_fac_packet_configuration_notification.hpp"
#include <boost/lexical_cast.hpp>
#include <sstream>
using namespace std;

FacConfigurationNotificationPacket::FacConfigurationNotificationPacket(ManagementInformationBase& mib, const vector<unsigned char>& packetBuffer, Logger& logger) :
	GeonetPacket(packetBuffer, logger), mib(mib), logger(logger) {
	/**
	 * Parse the packet...
	 */
	parse(packetBuffer);
	/**
	 * ...and update MIB with extracted information
	 */
	logger.info("Notifying MIB about this ITS key value change...");
	mib.setValue(static_cast<ItsKeyID>(packet.configurationItem.configurationId), packet.configurationItem.configurationBuffer);
}

FacConfigurationNotificationPacket::~FacConfigurationNotificationPacket() {}

bool FacConfigurationNotificationPacket::parse(const vector<unsigned char>& packetBuffer) {
	if (packetBuffer.size() < sizeof(MessageHeader)) {
		logger.warning("Incoming buffer is short of size (size=" + boost::lexical_cast<string>(packetBuffer.size()) + ") to carry a Configuration Notification packet!");
		return false;
	}

	/**
	 * Parse configuration id and size of configuration item
	 */
	u_int8_t payloadIndex = sizeof(MessageHeader);
	packet.configurationItem.configurationId = packetBuffer[payloadIndex];
	packet.configurationItem.configurationId <<= 8;
	packet.configurationItem.configurationId |= packetBuffer[payloadIndex + 1];
	packet.configurationItem.length = packetBuffer[payloadIndex + 2];
	packet.configurationItem.length <<= 8;
	packet.configurationItem.length |= packetBuffer[payloadIndex + 3];

	/**
	 * Verify packet size again having data length this time
	 */
	u_int16_t packetHeaderLength = sizeof(MessageHeader) + sizeof(packet.configurationItem.configurationId) + sizeof(packet.configurationItem.length);
	if (packetBuffer.size() != packetHeaderLength + packet.configurationItem.length) {
		logger.info("Incoming Configuration Notification packet is short of size to carry the payload it asserts it carries!");
		return false;
	}

	/**
	 * Extract payload...
	 */
	packet.configurationItem.configurationBuffer.resize(packet.configurationItem.length);
	copy(packetBuffer.begin() + packetHeaderLength, packetBuffer.end(), packet.configurationItem.configurationBuffer.begin());

	return true;
}

string FacConfigurationNotificationPacket::toString() const {
	stringstream ss;

	ss << "ConfigurationNotification[ConfID:" << hex << showbase << packet.configurationItem.configurationId
	   << resetiosflags(ios_base::hex) << ", Length:" << packet.configurationItem.length << "]";

	return ss.str();
}

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
 * \file mgmt_gn_packet_get_configuration.cpp
 * \brief A container for Get Configuration Event packet
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#include "mgmt_gn_packet_get_configuration.hpp"
#include <iostream>
#include <sstream>
using namespace std;

GeonetGetConfigurationEventPacket::GeonetGetConfigurationEventPacket(const vector<unsigned char>& packetBuffer, Logger& logger) :
	GeonetPacket(packetBuffer, logger) {
	parse(packetBuffer);
	logger.info(toString());
}

u_int16_t GeonetGetConfigurationEventPacket::getConfID() const {
	return packet.configurationId;
}

u_int16_t GeonetGetConfigurationEventPacket::getTxMode() const {
	return packet.transmissionMode;
}

string GeonetGetConfigurationEventPacket::toString() const {
	stringstream ss;

	ss << "[ConfID:" << packet.configurationId << ", txMode:" << packet.transmissionMode << "]";

	return ss.str();
}

bool GeonetGetConfigurationEventPacket::parse(const vector<unsigned char>& packetBuffer) {
	if (packetBuffer.size() < sizeof(ConfigurationRequestMessage))
		return false;

	u_int8_t payloadIndex = sizeof(MessageHeader);

	packet.configurationId = packetBuffer[payloadIndex];
	packet.configurationId <<= 8;
	packet.configurationId |= packetBuffer[payloadIndex + 1];

	packet.transmissionMode = packetBuffer[payloadIndex + 2];
	packet.transmissionMode <<= 8;
	packet.transmissionMode |= packetBuffer[payloadIndex + 3];

	return true;
}

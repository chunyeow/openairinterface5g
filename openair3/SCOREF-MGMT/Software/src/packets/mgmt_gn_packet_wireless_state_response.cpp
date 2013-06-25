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
 * \file mgmt_gn_packet_wireless_state.cpp
 * \brief A container for Wireless State Event Response packet
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#include "mgmt_gn_packet_wireless_state_response.hpp"
#include <boost/lexical_cast.hpp>
#include "../util/mgmt_util.hpp"
#include <sstream>

GeonetWirelessStateResponseEventPacket::GeonetWirelessStateResponseEventPacket(ManagementInformationBase& mib, const vector<unsigned char>& packetBuffer, Logger& logger)
	: GeonetPacket(packetBuffer, logger), mib(mib) {
	if (this->parse(packetBuffer)) {
		logger.info("MIB is updated with incoming wireless state information");
	}
}

GeonetWirelessStateResponseEventPacket::~GeonetWirelessStateResponseEventPacket() {
}

string GeonetWirelessStateResponseEventPacket::toString() const {
	stringstream ss;

	return ss.str();
}

bool GeonetWirelessStateResponseEventPacket::parse(const vector<unsigned char>& packetBuffer) {
	/**
	 * Validate incoming buffer
	 */
	if (packetBuffer.size() < sizeof(WirelessStateResponseMessage))
		return false;

	/**
	 * Parse interface count first
	 */
	u_int8_t interfaceCount = packetBuffer.data()[sizeof(MessageHeader)];
	logger.info("Number of interfaces is " + boost::lexical_cast<string>((int)interfaceCount));

	/**
	 * Then traverse the buffer to get the state for every interface...
	 */
	u_int16_t itemIndex = sizeof(WirelessStateResponseMessage);
	for (; interfaceCount != 0; interfaceCount--) {
		GnWirelessStateResponseItem* item = new GnWirelessStateResponseItem();

		Util::parse2byteInteger(packetBuffer.data() + itemIndex, &item->interfaceId); itemIndex += sizeof(InterfaceID);
		Util::parse2byteInteger(packetBuffer.data() + itemIndex, &item->accessTechnology); itemIndex += 2;
		Util::parse2byteInteger(packetBuffer.data() + itemIndex, &item->channelFrequency); itemIndex += 2;
		Util::parse2byteInteger(packetBuffer.data() + itemIndex, &item->bandwidth); itemIndex += 2;
		item->channelBusyRatio = static_cast<u_int8_t>(packetBuffer.data()[itemIndex]); ++itemIndex;
		item->status = static_cast<u_int8_t>(packetBuffer.data()[itemIndex]); ++itemIndex;
		item->averageTxPower = static_cast<u_int8_t>(packetBuffer.data()[itemIndex]); ++itemIndex;
		item->reserved = static_cast<u_int8_t>(packetBuffer.data()[itemIndex]); ++itemIndex;

		// Update MIB with this record
		mib.updateWirelessState(item->interfaceId, item);

		logger.info("Management Information Base has been updated with following wireless state entry: ");
		logger.info(item->toString());

		// itemIndex shows the next record now, if there's any
	}

	return true;
}

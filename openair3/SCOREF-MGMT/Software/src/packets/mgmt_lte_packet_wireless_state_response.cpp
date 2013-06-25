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

#include "mgmt_lte_packet_wireless_state_response.hpp"
#include <boost/lexical_cast.hpp>
#include "../util/mgmt_util.hpp"
#include <sstream>

LteWirelessStateResponseEventPacket::LteWirelessStateResponseEventPacket(ManagementInformationBase& mib, const vector<unsigned char>& packetBuffer, Logger& logger)
	: GeonetPacket(packetBuffer, logger), mib(mib) {
	if (this->parse(packetBuffer)) {
		logger.info("MIB is updated with incoming wireless state information");
	}
}

LteWirelessStateResponseEventPacket::~LteWirelessStateResponseEventPacket() {
}

string LteWirelessStateResponseEventPacket::toString() const {
	stringstream ss;

	return ss.str();
}

bool LteWirelessStateResponseEventPacket::parse(const vector<unsigned char>& packetBuffer) {
	/**
	 * Validate the size of incoming buffer
	 */
	if (packetBuffer.size() < LTE_WIRELESS_STATE_RESPONSE_SIZE) {
		logger.warning("Incoming buffer is not large enough to contain an LTE Wireless State Response!");
		return false;
	}

	LteWirelessStateResponse* response = new LteWirelessStateResponse();

	/**
	 * Set the index right after the header
	 */
	u_int16_t responseIndex = sizeof(MessageHeader);

	Util::parse2byteInteger(packetBuffer.data() + responseIndex, &response->interfaceId); responseIndex += 2;
	Util::parse2byteInteger(packetBuffer.data() + responseIndex, &response->reservedFirst16bit); responseIndex += 2;
	Util::parse2byteInteger(packetBuffer.data() + responseIndex, &response->referenceSignalReceivedPower); responseIndex += 2;
	Util::parse2byteInteger(packetBuffer.data() + responseIndex, &response->referenceSignalReceivedQuality); responseIndex += 2;
	response->channelQualityIndication = static_cast<u_int8_t>(packetBuffer.data()[responseIndex]); ++responseIndex;
	response->status = static_cast<u_int8_t>(packetBuffer.data()[responseIndex]); ++responseIndex;
	Util::parse2byteInteger(packetBuffer.data() + responseIndex, &response->reservedSecond16bit); responseIndex += 2;
	Util::parse4byteInteger(packetBuffer.data() + responseIndex, &response->packetLossRate); responseIndex += 4;

	// Update MIB with this record
	mib.updateWirelessState(response->interfaceId, response);

	logger.info("Management Information Base has been updated with following wireless state entry: ");
	logger.info(response->toString());

	return true;
}

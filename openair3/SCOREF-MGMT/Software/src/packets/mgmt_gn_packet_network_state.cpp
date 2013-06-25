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
 * \file mgmt_gn_packet_network_state.cpp
 * \brief A container for Network State Event packet
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#include "mgmt_gn_packet_network_state.hpp"
#include "../util/mgmt_util.hpp"
#include <sstream>

GeonetNetworkStateEventPacket::GeonetNetworkStateEventPacket(ManagementInformationBase& mib, vector<unsigned char> packetBuffer, Logger& logger)
	: GeonetPacket(packetBuffer, logger), mib(mib) {
	if (parse(packetBuffer)) {
		logger.info(this->toString());
		logger.info("MIB is updated with incoming network state information");
	}
}

GeonetNetworkStateEventPacket::~GeonetNetworkStateEventPacket() {
}

string GeonetNetworkStateEventPacket::toString() const {
	stringstream ss;

	ss << "NetworkState[ts:" << mib.getNetworkState().timestamp
		<< " TxPackets:" << mib.getNetworkState().rxPackets
		<< " RxBytes:" << mib.getNetworkState().rxBytes
		<< " TxPackets:" << mib.getNetworkState().txPackets
		<< " TxBytes:" << mib.getNetworkState().txBytes
		<< " toUpLayer:" << mib.getNetworkState().toUpperLayerPackets
		<< " disc.:" << mib.getNetworkState().discardedPackets
		<< " dup.:" << mib.getNetworkState().duplicatePackets
		<< " forw.:" << mib.getNetworkState().forwardedPackets
		<< "]";

	return ss.str();
}

bool GeonetNetworkStateEventPacket::parse(const vector<unsigned char>& packetBuffer) {
	/**
	 * Verify that incoming packet's size is sufficient for a Network State packet
	 */
	if (packetBuffer.size() < sizeof(NetworkStateMessage))
		return false;

	unsigned int dataIndex = sizeof(MessageHeader);
	Util::parse4byteInteger(packetBuffer.data() + dataIndex, &mib.getNetworkState().timestamp); dataIndex += 4;
	Util::parse4byteInteger(packetBuffer.data() + dataIndex, &mib.getNetworkState().rxPackets); dataIndex += 4;
	Util::parse4byteInteger(packetBuffer.data() + dataIndex, &mib.getNetworkState().rxBytes); dataIndex += 4;
	Util::parse4byteInteger(packetBuffer.data() + dataIndex, &mib.getNetworkState().txPackets); dataIndex += 4;
	Util::parse4byteInteger(packetBuffer.data() + dataIndex, &mib.getNetworkState().txBytes); dataIndex += 4;
	Util::parse4byteInteger(packetBuffer.data() + dataIndex, &mib.getNetworkState().toUpperLayerPackets); dataIndex += 4;
	Util::parse4byteInteger(packetBuffer.data() + dataIndex, &mib.getNetworkState().discardedPackets); dataIndex += 4;
	Util::parse4byteInteger(packetBuffer.data() + dataIndex, &mib.getNetworkState().duplicatePackets); dataIndex += 4;
	Util::parse4byteInteger(packetBuffer.data() + dataIndex, &mib.getNetworkState().forwardedPackets);

	return true;
}

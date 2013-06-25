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
 * \file mgmt_gn_packet_comm_profile_request.cpp
 * \brief A container for Communication Profile Request event
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#include "mgmt_gn_packet_comm_profile_request.hpp"
#include "../util/mgmt_util.hpp"

#include <sstream>

GeonetCommunicationProfileRequestPacket::GeonetCommunicationProfileRequestPacket(const vector<unsigned char>& packetBuffer, Logger& logger)
	: GeonetPacket(packetBuffer, logger) {
	parse(packetBuffer);
}

GeonetCommunicationProfileRequestPacket::~GeonetCommunicationProfileRequestPacket() {
}

u_int32_t GeonetCommunicationProfileRequestPacket::getCommunicationProfileRequestSet() const {
	return communicationProfileRequestSet;
}

u_int8_t GeonetCommunicationProfileRequestPacket::getSequenceNumber() const {
	return sequenceNumber;
}

#ifdef UNUSED
bool GeonetCommunicationProfileRequestPacket::getTransportBtpA() const {
	return transportBtpA;
}

bool GeonetCommunicationProfileRequestPacket::getTransportBtpB() const {
	return transportUdp;
}

bool GeonetCommunicationProfileRequestPacket::getTransportTcp() const {
	return transportUdp;
}

bool GeonetCommunicationProfileRequestPacket::getTransportUdp() const {
	return transportUdp;
}

bool GeonetCommunicationProfileRequestPacket::getTransportRtp() const {
	return transportUdp;
}

bool GeonetCommunicationProfileRequestPacket::getTransportStcp() const {
	return transportUdp;
}

bool GeonetCommunicationProfileRequestPacket::getNetworkGn() const {
	return networkGn;
}

bool GeonetCommunicationProfileRequestPacket::getNetworkIpv6Gn() const {
	return networkIpv6Gn;
}

bool GeonetCommunicationProfileRequestPacket::getNetworkIpv6() const {
	return networkIpv6;
}

bool GeonetCommunicationProfileRequestPacket::getNetworkIpv4() const {
	return networkIpv4;
}

bool GeonetCommunicationProfileRequestPacket::getNetworkIpv4v6() const {
	return networkIpv4v6;
}

bool GeonetCommunicationProfileRequestPacket::getNetworkDsmiIpv4v6() const {
	return networkDsmIpv4v6;
}

bool GeonetCommunicationProfileRequestPacket::getAccessItsG5() const {
	return accessItsG5;
}

bool GeonetCommunicationProfileRequestPacket::getAccess3g() const {
	return access3G;
}

bool GeonetCommunicationProfileRequestPacket::getAccess11n() const {
	return access11n;
}

bool GeonetCommunicationProfileRequestPacket::getAccessEthernet() const {
	return accessEthernet;
}

bool GeonetCommunicationProfileRequestPacket::getChannelCch() const {
	return channelCch;
}

bool GeonetCommunicationProfileRequestPacket::getChannelSch1() const {
	return channelSch1;
}

bool GeonetCommunicationProfileRequestPacket::getChannelSch2() const {
	return channelSch2;
}

bool GeonetCommunicationProfileRequestPacket::getChannelSch3() const {
	return channelSch3;
}

bool GeonetCommunicationProfileRequestPacket::getChannelSch4() const {
	return channelSch4;
}
#endif

bool GeonetCommunicationProfileRequestPacket::parse(const vector<unsigned char>& packetBuffer) {
	/**
	 * Verify the size of incoming buffer
	 */
	if (packetBuffer.size() < sizeof(MessageHeader))
		return false;

	u_int8_t payloadIndex = sizeof(MessageHeader);
	/*
	 * Parse first octet which is "transport"
	 */
	transportBtpA    = Util::isBitSet(packetBuffer[payloadIndex], 0x01);
	transportBtpB    = Util::isBitSet(packetBuffer[payloadIndex], 0x02);
	transportTcp     = Util::isBitSet(packetBuffer[payloadIndex], 0x03);
	transportUdp     = Util::isBitSet(packetBuffer[payloadIndex], 0x04);
	transportRtp     = Util::isBitSet(packetBuffer[payloadIndex], 0x05);
	transportStcp    = Util::isBitSet(packetBuffer[payloadIndex], 0x06);
	/*
	 * Parse second octet which is "network"
	 */
	networkGn        = Util::isBitSet(packetBuffer[payloadIndex + 1], 0x01);
	networkIpv6Gn    = Util::isBitSet(packetBuffer[payloadIndex + 1], 0x02);
	networkIpv6      = Util::isBitSet(packetBuffer[payloadIndex + 1], 0x03);
	networkIpv4      = Util::isBitSet(packetBuffer[payloadIndex + 1], 0x04);
	networkIpv4v6    = Util::isBitSet(packetBuffer[payloadIndex + 1], 0x05);
	networkDsmIpv4v6 = Util::isBitSet(packetBuffer[payloadIndex + 1], 0x06);
	/*
	 * Parse third octet which is "access"
	 */
	accessItsG5      = Util::isBitSet(packetBuffer[payloadIndex + 2], 0x01);
	access3G         = Util::isBitSet(packetBuffer[payloadIndex + 2], 0x02);
	access11n        = Util::isBitSet(packetBuffer[payloadIndex + 2], 0x03);
	accessEthernet   = Util::isBitSet(packetBuffer[payloadIndex + 2], 0x04);
	/**
	 * Parse fourth octet whih is "channel"
	 */
	channelCch       = Util::isBitSet(packetBuffer[payloadIndex + 3], 0x01);
	channelSch1      = Util::isBitSet(packetBuffer[payloadIndex + 3], 0x02);
	channelSch2      = Util::isBitSet(packetBuffer[payloadIndex + 3], 0x03);
	channelSch3      = Util::isBitSet(packetBuffer[payloadIndex + 3], 0x04);
	channelSch4      = Util::isBitSet(packetBuffer[payloadIndex + 3], 0x05);

	/**
	 * Parse whole set of requested configuration
	 */
	Util::parse4byteInteger(packetBuffer.data() + payloadIndex, &communicationProfileRequestSet); payloadIndex += 4;

	/**
	 * Parse the sequence number and ignore reserved part
	 */
	sequenceNumber = packetBuffer.data()[payloadIndex];

	return true;
}

string GeonetCommunicationProfileRequestPacket::toString() const {
	stringstream ss;

	ss << GeonetPacket::toString() << endl
		<< "Transport [BTPA:" << transportBtpA << ", BTPB:" << transportBtpB
		<< ", TCP:" << transportTcp << ", UDP:" << transportUdp
		<< ", RTP:" << transportRtp << ", STCP:" << transportStcp << "]" << endl
		<< "Network [GN:" << networkGn << ", IPv6GN:" << networkIpv6Gn
		<< ", IPv6:" << networkIpv6 << ", IPv4:" << networkIpv4
		<< ", IPv4v6:" << networkIpv4v6 << ", DSMIPv4v6:" << networkDsmIpv4v6 << "]" << endl
		<< "Access [ITSG5:" << accessItsG5 << ", 3G:" << access3G
		<< ", 11n:" << access11n << ", Ethernet:" << accessEthernet << "]" << endl
		<< "Channel [CCH:" << channelCch << ", SCH1:" << channelSch1
		<< ", SCH2:" << channelSch2 << ", SCH3:" << channelSch3
		<< ", SCH4: " << channelSch4 << "]" << endl
		<< "Sequence number:" << sequenceNumber << endl;

	return ss.str();
}

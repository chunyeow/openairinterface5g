/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2010 Eurecom

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
 * \file mgmt_fac_packet_comm_profile_selection_response.cpp
 * \brief A container for Communication Profile Selection Response event
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#include "mgmt_fac_packet_comm_profile_selection_response.hpp"
#include "../util/mgmt_util.hpp"

FacCommunicationProfileSelectionResponsePacket::FacCommunicationProfileSelectionResponsePacket(ManagementInformationBase& mib,
		u_int8_t latency, u_int8_t relevance, u_int8_t reliability, u_int8_t sequenceNumber, Logger& logger) :
	GeonetPacket(false, true, 0x00, 0x00, MGMT_FAC_EVENT_CONF_COMM_PROFILE_SELECTION_RESPONSE, logger), mib(mib) {
	packet.latency = latency;
	packet.relevance = relevance;
	packet.reliability = reliability;
	packet.sequenceNumber = sequenceNumber;
}

FacCommunicationProfileSelectionResponsePacket::~FacCommunicationProfileSelectionResponsePacket() {}

bool FacCommunicationProfileSelectionResponsePacket::serialize(vector<unsigned char>& buffer) const {
	/**
	 * Verify incoming buffer size
	 */
	if (buffer.size() < sizeof(CommunicationProfileSelectionResponse))
		return false;

	/**
	 * Serialise header first...
	 */
	GeonetPacket::serialize(buffer);
	/**
	 * ..then append communication profile information
	 */
	u_int8_t payloadIndex = sizeof(MessageHeader);
	buffer[payloadIndex++] = packet.latency;
	buffer[payloadIndex++] = packet.relevance;
	buffer[payloadIndex++] = packet.reliability;
	buffer[payloadIndex++] = packet.sequenceNumber;
	/**
	 * Get the most suitable communication profile from CommunicationProfileManager...
	 */
	CommunicationProfileID communicationProfileId = mib.getCommunicationProfileManager().selectProfile(mib.isIpv6Enabled(), packet.latency, packet.relevance, packet.reliability);
	logger.debug(string("Ipv6 is ") + ((mib.isIpv6Enabled()) ? "enabled" : "disabled"));

	/**
	 * ...and encode it into the response packet
	 */
	Util::encode4byteInteger(buffer, payloadIndex, static_cast<int>(communicationProfileId));

	/**
	 * Resize incoming buffer according to packet size
	 */
	buffer.resize(sizeof(CommunicationProfileSelectionResponse));

	return true;
}

string FacCommunicationProfileSelectionResponsePacket::toString() const {
	stringstream ss;

	ss << GeonetPacket::toString() << endl
		<< "Latency: " << hex << (int)packet.latency << endl
		<< "Relevance: " << hex << (int)packet.relevance << endl
		<< "Reliability: " << hex << (int)packet.reliability << endl
		<< "Communication Profile ID: " << oct << (int)packet.communicationProfileId << endl;

	return ss.str();
}

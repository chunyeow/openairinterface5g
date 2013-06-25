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
 * \file mgmt_gn_packet_comm_profile_response.cpp
 * \brief A container for Communication Profile Response event
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#include "mgmt_gn_packet_comm_profile_response.hpp"
#include <boost/lexical_cast.hpp>
#include "../util/mgmt_util.hpp"

GeonetCommunicationProfileResponsePacket::GeonetCommunicationProfileResponsePacket(ManagementInformationBase& mib,
		u_int32_t communicationProfileRequest, u_int8_t sequenceNumber, Logger& logger) :
	GeonetPacket(false, true, 0x00, 0x00, MGMT_GN_EVENT_CONF_COMM_PROFILE_RESPONSE, logger), mib(mib), logger(logger) {
	this->communicationProfileRequest = communicationProfileRequest;
	this->sequenceNumber = sequenceNumber;
}

GeonetCommunicationProfileResponsePacket::~GeonetCommunicationProfileResponsePacket() {
}

bool GeonetCommunicationProfileResponsePacket::serialize(vector<unsigned char>& buffer) const {
	/**
	 * Verify incoming buffer size
	 */
	if (buffer.size() < sizeof(CommunicationProfileResponse))
		return false;

	/**
	 * Serialise header first...
	 */
	GeonetPacket::serialize(buffer);

	/**
	 * Fetch those profiles matching with the incoming bitmap (sent in the COMM_PROFILE_REQ)
	 */
	map<CommunicationProfileID, CommunicationProfileItem> filteredProfileMap = mib.getCommunicationProfileManager().getProfileMapSubset(communicationProfileRequest);

	/**
	 * Append communication profile item count
	 */
	u_int8_t payloadIndex = sizeof(MessageHeader);
	Util::encode2byteInteger(buffer, payloadIndex, filteredProfileMap.size());
	payloadIndex += 2;
	/**
	 * Append sequence number
	 */
	buffer[payloadIndex++] = sequenceNumber;
	/**
	 * ...and then the `reserved' field
	 */
	buffer[payloadIndex++] = 0x00;

	/**
	 * ...and communication profile item(s)
	 */
	map<CommunicationProfileID, CommunicationProfileItem>::const_iterator it = filteredProfileMap.begin();
	while (it != filteredProfileMap.end()) {
		Util::encode4byteInteger(buffer, payloadIndex, it->second.id);
		payloadIndex += 4;

		buffer[payloadIndex++] = it->second.transport;
		buffer[payloadIndex++] = it->second.network;
		buffer[payloadIndex++] = it->second.access;
		buffer[payloadIndex++] = it->second.channel;

		// Now `payloadIndex' points to the next available place
		++it;
	}

	logger.info("A COMM_PROFILE_RESPONSE packet has been generated having " + boost::lexical_cast<string>(filteredProfileMap.size()) + " profile(s) out of " + boost::lexical_cast<string>((int)mib.getCommunicationProfileManager().getProfileCount()));

	/**
	 * Resize buffer to the number of bytes we've written into it
	 */
	buffer.resize(sizeof(CommunicationProfileResponse) + filteredProfileMap.size() * sizeof(CommunicationProfileItem));

	return true;
}

string GeonetCommunicationProfileResponsePacket::toString() const {
	stringstream ss;

	ss << GeonetPacket::toString() << endl
		<< mib.getCommunicationProfileManager().toString() << endl;

	return ss.str();
}

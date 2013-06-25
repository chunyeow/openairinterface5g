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
 * \file mgmt_gn_packet_location_table_response.hpp
 * \brief A container for Location Table Response Event packet
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#include "mgmt_gn_packet_location_table_response.hpp"
#include <boost/lexical_cast.hpp>
#include "../util/mgmt_util.hpp"
#include <sstream>

GeonetLocationTableResponseEventPacket::GeonetLocationTableResponseEventPacket(ManagementInformationBase& mib, const vector<unsigned char>& packetBuffer, Logger& logger)
	: GeonetPacket(packetBuffer, logger), mib(mib) {
	parse(packetBuffer);
}

string GeonetLocationTableResponseEventPacket::toString() const {
	stringstream ss;

	return ss.str();
}

bool GeonetLocationTableResponseEventPacket::parse(const vector<unsigned char>& packetBuffer) {
	u_int16_t lpvCount = 0;
	u_int16_t packetBufferIndex = sizeof(MessageHeader);

	// Parse LPV Count...
	if (Util::parse2byteInteger(packetBuffer.data() + packetBufferIndex, &lpvCount)) {
		logger.info("Location table response has following number of entr{y|ies}: " + boost::lexical_cast<string>(lpvCount));
	} else {
		logger.error("Cannot parse location table entry count");
		return false;
	}
	// ...and Network Flags
	mib.setNetworkFlags(packetBuffer[2]);

	/**
	 * Skip 4-byte part (LPV Count, Network Flags, and Reserved flags) coming immediately after the header
	 */
	u_int16_t itemIndex = packetBufferIndex + sizeof(u_int32_t);

	/**
	 * Traverse location table items...
	 */
	for (; lpvCount != 0; lpvCount--) {
		LocationTableItem* item = new LocationTableItem();

		Util::parse8byteInteger(packetBuffer.data() + itemIndex, &item->gnAddress); itemIndex += sizeof(GnAddress);
		Util::parse4byteInteger(packetBuffer.data() + itemIndex, &item->timestamp); itemIndex += sizeof(u_int32_t);
		Util::parse4byteInteger(packetBuffer.data() + itemIndex, &item->latitude); itemIndex += sizeof(u_int32_t);
		Util::parse4byteInteger(packetBuffer.data() + itemIndex, &item->longitude); itemIndex += sizeof(u_int32_t);
		Util::parse2byteInteger(packetBuffer.data() + itemIndex, &item->speed); itemIndex += sizeof(u_int16_t);
		Util::parse2byteInteger(packetBuffer.data() + itemIndex, &item->heading); itemIndex += sizeof(u_int16_t);
		Util::parse2byteInteger(packetBuffer.data() + itemIndex, &item->altitude); itemIndex += sizeof(u_int16_t);
		Util::parse2byteInteger(packetBuffer.data() + itemIndex, &item->acceleration); itemIndex += sizeof(u_int16_t);
		Util::parse2byteInteger(packetBuffer.data() + itemIndex, &item->sequenceNumber); itemIndex += sizeof(u_int16_t);
		item->lpvFlags = packetBuffer.data()[itemIndex++];
		item->reserved = packetBuffer.data()[itemIndex++];

		// Update MIB with this record
		mib.updateLocationTable(item);

		logger.info("Management Information Base has been updated with following location table entry: ");
		logger.info(item->toString());

		// itemIndex shows the next record now, if there's any
	}

	return true;
}

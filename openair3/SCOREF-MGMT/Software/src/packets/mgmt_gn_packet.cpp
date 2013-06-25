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
 * \file mgmt_gn_packets.cpp
 * \brief Superclass for all Management-Geonet messages
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#include "mgmt_gn_packet.hpp"
#include "../util/mgmt_util.hpp"
#include <sstream>
#include <iostream>
using namespace std;

GeonetPacket::GeonetPacket(bool extendedMessage, bool validity, u_int8_t version, u_int8_t priority,
    u_int16_t eventType, Logger& logger) : logger(logger) {
	Util::resetBuffer(reinterpret_cast<unsigned char*>(&header), sizeof(MessageHeader));

	if (extendedMessage)
		this->header.version |= 0x80;
	if (validity)
		this->header.version |= 0x40;

	this->header.version |= (version & 0x0f);
	this->header.priority = priority;
	this->header.priority <<= 5;
	this->header.eventType = (eventType >> 8);
	this->header.eventSubtype = (eventType & 0xFF);
}

GeonetPacket::GeonetPacket(const vector<unsigned char>& packetBuffer, Logger& logger)
	: logger(logger) {
	parseHeaderBuffer(packetBuffer, &this->header);
	/**
	 * Print the packet information
	 */
	logger.info(toString());
}

GeonetPacket::~GeonetPacket() {
}

bool GeonetPacket::parseHeaderBuffer(const vector<unsigned char>& headerBuffer, MessageHeader* header) {
	if (headerBuffer.size() < sizeof(MessageHeader) || !header)
		return false;

	/**
	 * Parse (E) and (V) fields first
	 */
	extended = Util::isBitSet(headerBuffer[0], 0);
	valid = Util::isBitSet(headerBuffer[0], 1);

	if (!valid)
		logger.warning("Incoming packet's (V) validity flag is not set! Will parse anyway...");

	header->version = headerBuffer[0] & 0x0f;
	header->priority = (headerBuffer[1] >> 5);
	header->eventType = headerBuffer[2];
	header->eventSubtype = headerBuffer[3];

	return true;
}

bool GeonetPacket::serialize(vector<unsigned char>& buffer) const {
	/**
	 * Validate incoming buffer's size
	 */
	if (buffer.size() < sizeof(MessageHeader)) {
		logger.error("Incoming buffer is not sufficient to encode a message header into!");
		return false;
	}

	logger.debug("Serialising header...");

	buffer[0] = header.version;
	buffer[0] |= 0x40; // encode Validity flag as 1
	buffer[1] = header.priority;
	buffer[1] <<= 5;
	buffer[2] = header.eventType;
	buffer[3] = header.eventSubtype;

	return true;
}

bool GeonetPacket::isExtended() const {
	return extended;
}

bool GeonetPacket::isValid() const {
	return valid;
}

bool GeonetPacket::setEventType(const u_int16_t& eventType) {
	this->header.eventType = (eventType >> 8);
	this->header.eventSubtype = (eventType & 0xFF);

	return true;
}

u_int16_t GeonetPacket::getEventType() const {
	u_int16_t eventType = header.eventType;
	eventType <<= 8;
	eventType |= header.eventSubtype;

	return eventType;
}

string GeonetPacket::toString() const {
	stringstream ss;

	ss << "GeonetHeader[extended:" << isExtended() << ", valid:" << isValid()
	   << ", version:" << (int) header.getVersion() << ", priority:" << (int) header.getPriority()
		<< ", eventType:" << hex << showbase << setw(2) << (int) header.eventType
		<< ", eventSubType: " << (int) header.eventSubtype << "]";

	return ss.str();
}

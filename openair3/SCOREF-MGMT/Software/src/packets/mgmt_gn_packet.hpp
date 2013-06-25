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
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/

/*!
 * \file mgmt_gn_packets.hpp
 * \brief Superclass for all Management-Geonet messages
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_GN_PACKET_HPP_
#define MGMT_GN_PACKET_HPP_

#include "../mgmt_information_base.hpp"
#include "../mgmt_types.hpp"
#include "../util/mgmt_log.hpp"
#include <string>
#include <vector>
using namespace std;

/**
 * Superclass for all Management-Geonet messages
 */
class GeonetPacket {
	public:
		/**
		 * Constructor for GeonetPacket class
		 *
		 * @param extendedMessage Indicates if this an extended (vendor specific) message
		 * @param validity Indicates non-existent data
		 * @param version Version number
		 * @param priority Priority
		 * @param eventType 16-bit Event Type and Event Subtype information
		 * @param logger Logger object reference
		 */
		GeonetPacket(bool extendedMessage, bool validity, u_int8_t version, u_int8_t priority, u_int16_t eventType, Logger& logger);
		/**
		 * Buffer-parser constructor for GeonetPacket class
		 *
		 * @param packetBuffer Buffer carrying GeonetPacket
		 * @param logger Logger object reference
		 */
		GeonetPacket(const vector<unsigned char>& packetBuffer, Logger& logger);
		/**
		 * Virtual destructor for GeonetPacket class
		 */
		virtual ~GeonetPacket();

	public:
		/**
		 * Parses buffer and fills in incoming header structure
		 *
		 * @param headerBuffer Buffer carrying message header
		 * @param header Structure to be filled in
		 * @return true on success, false otherwise
		 */
		bool parseHeaderBuffer(const vector<unsigned char>& headerBuffer, MessageHeader* header);
		/**
		 * Serialises header fields onto given buffer
		 * This method is called by every subclass::serialize()
		 *
		 * @param buffer Buffer to serialise buffer on
		 * @return true on success, false otherwise
		 */
		virtual bool serialize(vector<unsigned char>& buffer) const;
		/**
		 * Returns if this packet contains extended/vendor specific data
		 * Those who override this method will be excommunicated, fair warning
		 *
		 * @return boolean
		 */
		bool isExtended() const;
		/**
		 * Returns if this packet contains valid data
		 * Those who override this method will be excommunicated, fair warning
		 *
		 * @return boolean
		 */
		bool isValid() const;
		/**
		 * Sets the Event Type/SubType field with incoming value
		 *
		 * @param eventType 16-bit Event Type/SubType
		 * @return true on success, false otherwise
		 */
		bool setEventType(const u_int16_t& eventType);
		/**
		 * Returns the Event Type/SubType value as a 16-bit value
		 *
		 * @return 16-bit Event Type/SubType value
		 */
		u_int16_t getEventType() const;
		/**
		 * Utility method to get event type of a packet buffer
		 *
		 * @param buffer A packet buffer of type vector<unsigned char> reference
		 * @return Event type
		 */
		static u_int16_t parseEventTypeOfPacketBuffer(const vector<unsigned char>& buffer) {
			const MessageHeader* header = reinterpret_cast<const MessageHeader*>(buffer.data());

			u_int16_t eventType = header->eventType;
			eventType <<= 8;
			eventType |= header->eventSubtype;

			return eventType;
		}
		/**
		 * Returns string representation of relevant GeonetPacket object
		 *
		 * @return std::string representation
		 */
		virtual string toString() const;

	protected:
		/**
		 * Header is kept in superclass for every Geonet* subclass
		 */
		MessageHeader header;
		/**
		 * (E) Extended bit
		 */
		bool extended;
		/**
		 * (V) Validity bit
		 */
		bool valid;
		/**
		 * Logger object reference
		 */
		Logger& logger;
};

#endif /* MGMT_GN_PACKET_HPP_ */

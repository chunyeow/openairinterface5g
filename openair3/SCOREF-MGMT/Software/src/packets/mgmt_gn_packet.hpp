/*******************************************************************************
    OpenAirInterface 
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is 
   included in this distribution in the file called "COPYING". If not, 
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06410 Biot Sophia Antipolis cedex, FRANCE

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

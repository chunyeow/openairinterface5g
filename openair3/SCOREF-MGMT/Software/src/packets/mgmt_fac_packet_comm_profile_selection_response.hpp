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
 * \file mgmt_fac_packet_comm_profile_selection_response.hpp
 * \brief A container for Communication Profile Selection Response event
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_FAC_PACKET_COMM_PROFILE_SELECTION_RESPONSE_HPP_
#define MGMT_FAC_PACKET_COMM_PROFILE_SELECTION_RESPONSE_HPP_

#include "mgmt_gn_packet.hpp"

/**
 * Communication Profile Selection Response
 */
struct CommunicationProfileSelectionResponse {
	MessageHeader header;

	u_int8_t latency;
	u_int8_t relevance;
	u_int8_t reliability;
	u_int8_t sequenceNumber;
	u_int32_t communicationProfileId;
} __attribute__((packed));

/**
 * A container for Communication Profile Selection Response event
 */
class FacCommunicationProfileSelectionResponsePacket : public GeonetPacket {
	public:
		/**
		 * Constructor for FacCommunicationProfileSelectionResponsePacket class
		 *
		 * @param mib Management Information Base reference
		 * @param latency `Latency' requirement parameter
		 * @param relevance `Relevance' requirement parameter
		 * @param reliability `Reliability' requirement parameter
		 * @param sequenceNumber `Sequence Number' of the request (that of response's will match it)
		 * @param logger Logger object reference
		 */
		FacCommunicationProfileSelectionResponsePacket(ManagementInformationBase& mib, u_int8_t latency, u_int8_t relevance, u_int8_t reliability, u_int8_t sequenceNumber, Logger& logger);
		/**
		 * Destructor for FacCommunicationProfileSelectionResponsePacket class
		 */
		~FacCommunicationProfileSelectionResponsePacket();

	public:
		/**
		 * Serialises the packet into given buffer
		 *
		 * @param Vector to be used to serialise the packet into
		 * @return true on success, false otherwise
		 */
		bool serialize(vector<unsigned char>& buffer) const;
		/**
		 * Returns std::string representation of packet
		 *
		 * @return String representation of packet
		 */
		string toString() const;

	private:
		/**
		 * Management Information Base reference to fetch
		 * necessary information to build this packet
		 */
		ManagementInformationBase& mib;
		/**
		 * A copy of the packet information
		 */
		CommunicationProfileSelectionResponse packet;
};

#endif /* MGMT_FAC_PACKET_COMM_PROFILE_SELECTION_RESPONSE_HPP_ */

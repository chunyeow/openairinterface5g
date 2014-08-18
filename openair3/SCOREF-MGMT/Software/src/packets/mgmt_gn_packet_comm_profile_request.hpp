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
 * \file mgmt_gn_packet_comm_profile_request.hpp
 * \brief A container for Communication Profile Request event
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_GN_PACKET_COMM_PROFILE_REQUEST_HPP_
#define MGMT_GN_PACKET_COMM_PROFILE_REQUEST_HPP_

#include "mgmt_gn_packet.hpp"

/**
 * Communication Profile Request
 */
struct CommunicationProfileRequest {
	MessageHeader header;

	u_int8_t transport;
	u_int8_t network;
	u_int8_t access;
	u_int8_t channel;
	u_int8_t sequenceNumber;
	u_int8_t reserved8bit;
	u_int16_t reserved16bit;
} __attribute__((packed));

/**
 * A container for Communication Profile Request event
 */
class GeonetCommunicationProfileRequestPacket: public GeonetPacket {
	public:
		/**
		 * Constructor for GeonetCommunicationProfileRequestPacket class
		 *
		 * @param packetBuffer vector containing packet data
		 * @param logger Logger object reference
		 */
		GeonetCommunicationProfileRequestPacket(const vector<unsigned char>& packetBuffer, Logger& logger);
		/**
		 * Destructor for GeonetCommunicationProfileRequestPacket class
		 */
		~GeonetCommunicationProfileRequestPacket();

	public:
		/**
		 * Returns 32-bit requested flag set that actually is the payload of this packet
		 *
		 * @return 32-bit requested flag set
		 */
		u_int32_t getCommunicationProfileRequestSet() const;
		/**
		 * Returns 8-bit sequence number
		 *
		 * @return 8-bit sequence number
		 */
		u_int8_t getSequenceNumber() const;
#ifdef UNUSED
		/**
		 * Returns Transport::BTPA flag's value
		 *
		 * @return true if set, false otherwise
		 */
		bool getTransportBtpA() const;
		/**
		 * Returns Transport::BTPB flag's value
		 *
		 * @return true if set, false otherwise
		 */
		bool getTransportBtpB() const;
		/**
		 * Returns Transport::TCP flag's value
		 *
		 * @return true if set, false otherwise
		 */
		bool getTransportTcp() const;
		/**
		 * Returns Transport::UDP flag's value
		 *
		 * @return true if set, false otherwise
		 */
		bool getTransportUdp() const;
		/**
		 * Returns Transport::RTP flag's value
		 *
		 * @return true if set, false otherwise
		 */
		bool getTransportRtp() const;
		/**
		 * Returns Transport::STCP flag's value
		 *
		 * @return true if set, false otherwise
		 */
		bool getTransportStcp() const;
		/**
		 * Returns Network::GN flag's value
		 *
		 * @return true if set, false otherwise
		 */
		bool getNetworkGn() const;
		/**
		 * Returns Network::IPv6GN flag's value
		 *
		 * @return true if set, false otherwise
		 */
		bool getNetworkIpv6Gn() const;
		/**
		 * Returns Network::IPv6 flag's value
		 *
		 * @return true if set, false otherwise
		 */
		bool getNetworkIpv6() const;
		/**
		 * Returns Network::IPv4 flag's value
		 *
		 * @return true if set, false otherwise
		 */
		bool getNetworkIpv4() const;
		/**
		 * Returns Network::IPv4v6 flag's value
		 *
		 * @return true if set, false otherwise
		 */
		bool getNetworkIpv4v6() const;
		/**
		 * Returns Network::DSMIIPv4v6 flag's value
		 *
		 * @return true if set, false otherwise
		 */
		bool getNetworkDsmiIpv4v6() const;
		/**
		 * Returns Access::ITSG5 flag's value
		 *
		 * @return true if set, false otherwise
		 */
		bool getAccessItsG5() const;
		/**
		 * Returns Access::3G flag's value
		 *
		 * @return true if set, false otherwise
		 */
		bool getAccess3g() const;
		/**
		 * Returns Access::11n flag's value
		 *
		 * @return true if set, false otherwise
		 */
		bool getAccess11n() const;
		/**
		 * Returns Access::Ethernet flag's value
		 *
		 * @return true if set, false otherwise
		 */
		bool getAccessEthernet() const;
		/**
		 * Returns Channel::CCH flag's value
		 *
		 * @return true if set, false otherwise
		 */
		bool getChannelCch() const;
		/**
		 * Returns Channel::SCH1 flag's value
		 *
		 * @return true if set, false otherwise
		 */
		bool getChannelSch1() const;
		/**
		 * Returns Channel::SCH2 flag's value
		 *
		 * @return true if set, false otherwise
		 */
		bool getChannelSch2() const;
		/**
		 * Returns Channel::SCH3 flag's value
		 *
		 * @return true if set, false otherwise
		 */
		bool getChannelSch3() const;
		/**
		 * Returns Channel::SCH4 flag's value
		 *
		 * @return true if set, false otherwise
		 */
		bool getChannelSch4() const;
#endif
		/**
		 * Parses the packet buffer and fills configuration flags
		 *
		 * @param packetBuffer Vector containing packet data
		 * @return true on success, false otherwise
		 */
		bool parse(const vector<unsigned char>& packetBuffer);
		/**
		 * Returns std::string representation of packet
		 *
		 * @return String representation of packet
		 */
		string toString() const;

	private:
		/**
		 * Transport flags
		 */
		bool transportBtpA, transportBtpB, transportTcp;
		bool transportUdp, transportRtp, transportStcp;
		/**
		 * Network flags
		 */
		bool networkGn, networkIpv6Gn, networkIpv6;
		bool networkIpv4, networkIpv4v6, networkDsmIpv4v6;
		/**
		 * Access flags
		 */
		bool accessItsG5, access3G, access11n, accessEthernet;
		/**
		 * Channel flags
		 */
		bool channelCch, channelSch1, channelSch2;
		bool channelSch3, channelSch4;
		/**
		 * Sequence number
		 */
		u_int8_t sequenceNumber;
		/**
		 * Communication profile request flag set (packet payload)
		 */
		u_int32_t communicationProfileRequestSet;
};

#endif /* MGMT_GN_PACKET_COMM_PROFILE_REQUEST_HPP_ */

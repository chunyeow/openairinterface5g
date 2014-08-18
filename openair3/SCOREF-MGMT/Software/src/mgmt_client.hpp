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
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

/*!
 * \file mgmt_client.hpp
 * \brief A container to hold information about Management clients
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
 */

#ifndef MGMT_CLIENT_HPP_
#define MGMT_CLIENT_HPP_

#include "util/mgmt_log.hpp"
#include <boost/asio.hpp>
#include <string>
#include <map>
using boost::asio::ip::udp;

/**
 * A container to hold information about Management clients, mostly used
 * in Message Handler code
 */
class ManagementClient {
	public:
		/**
		 * Client state
		 */
		enum ManagementClientState {
			/**
			 * Client is not connected or is unreachable
			 */
			OFFLINE = 0,
			/**
			 * A client is connected and alive
			 */
			ONLINE = 1,
		};

		/**
		 * Client type
		 */
		enum ManagementClientType {
			/**
			 * Initial value, this is the value set when a client object
			 * is created but the type has not yet determined
			 */
			UNKNOWN = 0,
			/**
			 * GeoNetworking client
			 */
			GN = 1,
			/**
			 * Facilities client
			 */
			FAC = 2,
			/**
			 * LTE client
			 */
			LTE = 3
		};

	public:
		/**
		 * Constructor for ManagementClient class
		 *
		 * @param mib Management Information Base reference
		 * @param clientEndpoint Client's connection information
		 * @param wirelessStateUpdateInterval Determines how frequent the wireless state update will be performed
		 * @param locationUpdateInterval Determines how frequent the location update will be performed
		 * @logger Logger object reference
		 */
		ManagementClient(ManagementInformationBase& mib, const udp::endpoint& clientEndpoint, u_int8_t wirelessStateUpdateInterval, u_int8_t locationUpdateInterval, Logger& logger);
		/**
		 * Destructor for ManagementClient class
		 */
		~ManagementClient();

	private:
		/**
		 * Copy constructor to prevent the usage of default copy constructor
		 */
		ManagementClient(const ManagementClient& managementClient);

	public:
		/**
		 * Getter for IP address of this client
		 *
		 * @return IP address of this client
		 */
		boost::asio::ip::address getAddress() const;
		/**
		 * Getter for port number of this client
		 *
		 * @return Port number of this client
		 */
		unsigned short int getPort() const;
		/**
		 * Returns a reference to the udp::endpoint of this client
		 *
		 * return A reference to udp::endpoint of this client
		 */
		const udp::endpoint& getEndpoint() const;
		/**
		 * Returns the state of this client
		 *
		 * @return ManagementClientState value for this client
		 */
		ManagementClientState getState() const;
		/**
		 * Sets the state of this client with given state
		 *
		 * @param state New ManagementClientState for this client
		 * @return true on success, false otherwise
		 */
		bool setState(ManagementClientState state);
		/**
		 * Returns the type of this client
		 *
		 * @return ManagementClientType value for this client
		 */
		ManagementClientType getType() const;
		/**
		 * Sets the type of this client with given state
		 *
		 * @param state New ManagementClientType for this client
		 * @return true on success, false otherwise
		 */
		bool setType(ManagementClientType type);
		/**
		 * Overloaded == operator to use ManagementClient type as a std::map key
		 *
		 * @param client Client that is going to be compared with
		 * @return true if clients are the same, false otherwise
		 */
		bool operator==(const ManagementClient& client) const;
		/**
		 * Overloaded < operator to use ManagementClient type as a std::map key
		 *
		 * @param client Client that is going to be compared with
		 * @return true if host object's IP address is smaller, false otherwise
		 */
		bool operator<(const ManagementClient& client) const;
		/**
		 * This is called when we send a packet to this client that
		 * requires a response
		 *
		 * @param none
		 * @return none
		 */
		void waitingForReply();
		/**
		 * This is called when we receive a reply corresponding to the
		 * request packet we sent to this client
		 *
		 * @param none
		 * @return none
		 */
		void replyReceived();
		/**
		 * Returns if this client has replied to the last packet we sent
		 *
		 * @param none
		 * @return true if this client is alive, false otherwise
		 */
		bool isAlive();
		/**
		 * Returns string representation of this client
		 *
		 * @return std::string representation of this client
		 */
		string toString();

	private:
		/**
		 * Management Information Base reference
		 */
		ManagementInformationBase& mib;
		/**
		 * Client's udp::endpoint information
		 */
		udp::endpoint clientEndpoint;
		/**
		 * Client's connection state with Management module
		 */
		ManagementClient::ManagementClientState state;
		/**
		 * Client type
		 */
		ManagementClient::ManagementClientType type;
		/**
		 * Logger object reference
		 */
		Logger& logger;
		/**
		 * String representations for Management Client states
		 */
		map<ManagementClient::ManagementClientState, string> clientStateStringMap;
		/**
		 * String representations for Management Client types
		 */
		map<ManagementClient::ManagementClientType, string> clientTypeStringMap;
		/**
		 * True if this client has replied to the last packet we sent to
		 * it that requires a response, false otherwise
		 */
		bool repliedToTheLastPacket;
		/**
		 * Last heartbeat's receive time
		 */
		time_t lastHeartbeatTime;
};

#endif /* MGMT_CLIENT_HPP_ */

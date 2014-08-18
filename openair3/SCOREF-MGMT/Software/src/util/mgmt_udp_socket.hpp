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
 * \file mgmt_udp_socket.hpp
 * \brief A wrapper container to maintain UDP socket connection
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_UDP_SOCKET_H_
#define MGMT_UDP_SOCKET_H_

#include <vector>
using namespace std;

#include <boost/thread.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
using boost::asio::ip::udp;

#include "../packets/mgmt_gn_packet.hpp"
#include "../util/mgmt_log.hpp"

/**
 * A wrapper container to maintain UDP socket connection
 */
class UdpSocket {
	public:
		/**
		 * Socket type enumeration
		 */
		enum SOCKET_TYPE {
			CLIENT_SOCKET = 0,
			SERVER_SOCKET = 1
		};
		/**
		 * Receive buffer size in bytes
		 */
		static const u_int16_t RX_BUFFER_SIZE = 1024;
		/**
		 * Transmit buffer size in bytes
		 */
		static const u_int16_t TX_BUFFER_SIZE = 1024;

	public:
		/**
		 * Constructor for UdpSocket class used as a server socket
		 *
		 * @param portNumber UDP port number that will be listened/connected to
		 * @param logger Logger object reference for logging purposes
		 */
		UdpSocket(u_int16_t portNumber, Logger& logger);
		/**
		 * Constructor for UdpSocket class used as a client socket
		 *
		 * @param address IP address
		 * @param portNumber UDP port number that will be listened/connected to
		 * @param logger Logger object reference for logging purposes
		 */
		UdpSocket(const string& address, u_int16_t portNumber, Logger& logger);
		/**
		 * Destructor for UdpSocket class
		 */
		~UdpSocket();

	private:
		/**
		 * Copy constructor to prevent the usage of default copy constructor
		 */
		UdpSocket(const UdpSocket& udpSocket);

	public:
		/**
		 * Reads available data from socket into given buffer
		 *
		 * @param rxBuffer RX buffer that read data will be put into
		 * @return Number of bytes read
		 */
		unsigned receive(vector<unsigned char>& rxBuffer);
		/**
		 * Writes given data through socket
		 *
		 * @param txBuffer TX buffer that will be sent
		 * @return true on success, false otherwise
		 */
		bool send(vector<unsigned char>& txBuffer);
		/**
		 * Serialises given Geonet packet and writes onto socket
		 *
		 * @param packet GeonetPacket reference that will be serialised
		 * and sent through socket
		 * @return true on success, false otherwise
		 */
		bool send(const GeonetPacket& packet);
		/**
		 * Returns the reference of udp::endpoint
		 *
		 * @return The reference of udp::endpoint
		 */
		const udp::endpoint& getRecipient() const;
		/**
		 * Returns string representation of this connection
		 *
		 * @return String representation of this class of type std::string
		 */
		string toString() const;

	private:
		/**
		 * Socket type, client or server
		 */
		UdpSocket::SOCKET_TYPE socketType;
		/**
		 * The io_service object that the datagram socket will use to dispatch
		 * handlers for any asynchronous operations performed on the socket
		 */
		boost::asio::io_service ioService;
		/**
		 * Mutexes to coordinate I/O on UDP server socket
		 */
		boost::mutex readMutex;
		boost::mutex writeMutex;
		/**
		 * udp::socket of Boost library
		 */
		udp::socket* socket;
		/**
		 * UDP recipient
		 */
		boost::asio::ip::udp::endpoint recipient;
		/**
		 * Logger object reference for logging purposes
		 */
		Logger& logger;
};

#endif /* MGMT_UDP_SOCKET_H_ */

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
 * \file mgmt_server.hpp
 * \brief This is where everything begins and ends
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_SERVER_H_
#define MGMT_SERVER_H_

#include "interface/mgmt_packet_sender.hpp"
#include "mgmt_information_base.hpp"
#include "mgmt_inquiry_thread.hpp"
#include "mgmt_packet_handler.hpp"
#include "mgmt_client_manager.hpp"
#include "mgmt_configuration.hpp"
#include <boost/thread/mutex.hpp>
#include "util/mgmt_log.hpp"
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <vector>
using boost::asio::ip::udp;

namespace ba = boost::asio;

/**
 * Management server functionality and a wrapper for boost::asio's asynchronous I/O
 */
class ManagementServer : public IManagementPacketSender {
	public:
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
		 * Constructor for ManagementServer class
		 *
		 * @param ioService I/O functionality is passed by the caller
		 * @param configuration A reference to the Configuration object of SCOREF-MGMT
		 * @param mib ManagementInformationBase reference
		 * @param logger Logger object reference for logging purposes
		 */
		ManagementServer(ba::io_service& ioService, const Configuration& configuration, ManagementInformationBase& mib, ManagementClientManager& clientManager, Logger& logger);
		/**
		 * Destructor for ManagementServer class
		 */
		virtual ~ManagementServer();

	public:
		/**
		 * Implementation of IManagementPacketSender functionality
		 *
		 * @param none
		 * @return true on success, false otherwise
		 */
		bool sendWirelessStateRequest();

	public:
		/**
		 * Receive data
		 */
		void readData();
		/**
		 * Callback for Rx, so called when data is read on the socket
		 *
		 * @param errorCode Error code
		 * @param size Amount of data available
		 */
		void handleReceive(const boost::system::error_code& errorCode, size_t size);
		/**
		 * Handles incoming Management data, called by handleReceive()
		 *
		 * @param none
		 * @return none
		 */
		void handleClientData();
		/**
		 * Callback for Tx, so called when data is written onto the socket
		 *
		 * @param errorCode Error code
		 * @param size Amount of data to be sent
		 */
		void handleSend(const boost::system::error_code& errorCode, size_t size);
		/**
		 * Sends given buffer over the socket
		 *
		 * @param buffer Buffer to be sent of type unsigned char vector
		 * @param recipient Destination of type udp::endpoint
		 */
		bool send(const vector<unsigned char>& buffer, const boost::asio::ip::udp::endpoint& recipient);

	private:
		/**
		 * The io_service object that the datagram socket will use to dispatch
		 * handlers for any asynchronous operation performed on the socket
		 */
		ba::io_service& ioService;
		/**
		 * udp::socket object
		 */
		ba::ip::udp::socket socket;
		/**
		 * UDP connection's recipient
		 */
		ba::ip::udp::endpoint recipient;
		/**
		 * RX buffer
		 */
		vector<unsigned char> rxData;
		/**
		 * TX buffer
		 */
		vector<unsigned char> txData;
		/**
		 * Management Information Base reference
		 */
		ManagementInformationBase& mib;
		/**
		 * Configuration information
		 */
		const Configuration& configuration;
		/**
		 * Client manager
		 */
		ManagementClientManager& clientManager;
		/**
		 * InquiryThread object for Wireless State updates
		 */
		InquiryThread* inquiryThreadObject;
		/**
		 * InquiryThread runner for Wireless State updates
		 */
		boost::thread* inquiryThread;
		/**
		 * Logger object reference for logging purposes
		 */
		Logger& logger;
		/**
		 * Response packets are generated using PacketHandler class
		 */
		PacketHandler packetHandler;
		/**
		 * Mutex to control data transmission
		 */
		boost::mutex txMutex;
};

#endif /* MGMT_SERVER_H_ */

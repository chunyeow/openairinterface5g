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
 * \file mgmt_udp_server.cpp
 * \brief A wrapper container to maintain UDP socket connection
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#include <boost/lexical_cast.hpp>
#include "mgmt_udp_socket.hpp"
#include "mgmt_exception.hpp"
#include "mgmt_util.hpp"

#include <iostream>
using namespace std;

UdpSocket::UdpSocket(u_int16_t portNumber, Logger& logger)
	: socketType(UdpSocket::SERVER_SOCKET), logger(logger) {
	logger.debug("Server socket is being created at port " + boost::lexical_cast<string>(portNumber));

	try {
		socket = new udp::socket(ioService, udp::endpoint(udp::v4(), portNumber));
		socket->set_option(boost::asio::socket_base::reuse_address(true));
	} catch (std::exception& e) {
		throw Exception(e.what(), logger);
	}

	logger.info("A UDP socket created for port " + boost::lexical_cast<string>(portNumber));
}

UdpSocket::UdpSocket(const string& address, u_int16_t portNumber, Logger& logger)
	: socketType(UdpSocket::CLIENT_SOCKET), logger(logger) {
	logger.debug("Client socket is being created to " + address + ":" + boost::lexical_cast<string>(portNumber));
	/**
	 * Convert incoming string address
	 */
	boost::asio::ip::address_v4 recipientAddress = boost::asio::ip::address_v4::from_string(address);
	recipient.address(recipientAddress);
	recipient.port(portNumber);
	/**
	 * Create a socket
	 */
	logger.debug("Creating a socket to " + recipient.address().to_string() + ":" + boost::lexical_cast<string>(recipient.port()));
	try {
		socket = new udp::socket(ioService, recipient);
		socket->set_option(boost::asio::socket_base::reuse_address(true));
	} catch (std::exception& e) {
		logger.error(e.what());
		throw Exception(e.what(), logger);
	}
}

UdpSocket::UdpSocket(const UdpSocket& UdpSocket)
	: logger(UdpSocket.logger) {
	throw Exception("Copy constructor is called for an UdpSocket object!", logger);
}

UdpSocket::~UdpSocket() {
	delete socket;
}

unsigned UdpSocket::receive(vector<unsigned char>& rxBuffer) {
	boost::system::error_code error;
	unsigned bytesRead = 0;

	/**
	 * Ensure there's only one I/O method of UdpSocket running at any given moment
	 */
	boost::lock_guard<boost::mutex> lock(readMutex);

	/**
	 * Always reset receive buffer size since it's resized after every read
	 * according to the packet size (and it becomes packet buffer)
	 */
	rxBuffer.resize(UdpSocket::RX_BUFFER_SIZE);

	try {
		logger.info("Reading from socket...");
		bytesRead = socket->receive_from(boost::asio::buffer(rxBuffer), recipient, 0, error);
	} catch (std::exception& e) {
		logger.error(e.what());
		return 0;
	}

	if (error && error != boost::asio::error::message_size)
		throw boost::system::system_error(error);

	rxBuffer.resize(bytesRead);

	logger.info(boost::lexical_cast<string>(bytesRead) + " byte(s) received from " + recipient.address().to_string() + ":" + boost::lexical_cast<string>(recipient.port()));
	Util::printHexRepresentation(rxBuffer.data(), rxBuffer.size(), logger);

	return bytesRead;
}

bool UdpSocket::send(vector<unsigned char>& txBuffer) {
	boost::system::error_code error;

	/**
	 * Ensure there's only one I/O method of UdpSocket running at any given moment
	 */
	boost::lock_guard<boost::mutex> lock(writeMutex);

	try {
		logger.info("Writing...");
		socket->send_to(boost::asio::buffer(txBuffer), recipient, 0, error);
		logger.info(boost::lexical_cast<string>(txBuffer.size()) + " byte(s) sent");
		Util::printHexRepresentation(txBuffer.data(), txBuffer.size(), logger);
	} catch (std::exception& e) {
		logger.error(e.what());
		return false;
	}

	return true;
}

bool UdpSocket::send(const GeonetPacket& packet) {
	vector<unsigned char> txBuffer(TX_BUFFER_SIZE);

	/**
	 * There's already a check for mutex in the overloaded function so
	 * we don't do it here
	 */
	if (packet.serialize(txBuffer))
		return send(txBuffer);

	return false;
}

const udp::endpoint& UdpSocket::getRecipient() const {
	return this->recipient;
}

string UdpSocket::toString() const {
	stringstream ss;

	ss << "[address: " << recipient.address() << ", port:" << recipient.port() << "]";

	return ss.str();
}

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
 * \file mgmt_server.cpp
 * \brief Management functionality provided by boost::asio's asynchronous I/O wrapper
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#include "packets/mgmt_gn_packet_configuration_available.hpp"
#include "packets/mgmt_gn_packet_wireless_state_request.hpp"
#include "packets/mgmt_gn_packet_location_table_request.hpp"
#include "packets/mgmt_gn_packet_location_update.hpp"
#include "util/mgmt_exception.hpp"
#include <boost/lexical_cast.hpp>
#include "mgmt_server.hpp"
#include <boost/bind.hpp>

ManagementServer::ManagementServer(ba::io_service& ioService, const Configuration& configuration, ManagementInformationBase& mib, ManagementClientManager& clientManager, Logger& logger)
	try : ioService(ioService), socket(ioService, ba::ip::udp::endpoint(udp::v4(), configuration.getServerPort())), mib(mib), configuration(configuration), clientManager(clientManager),
	  logger(logger), packetHandler(mib, logger) {
		/**
		 * Immediately start reading data
		 */
		logger.info("Reading data on port " + boost::lexical_cast<string>(configuration.getServerPort()));
		readData();

		/**
		 * Initialise InquiryThread object for Wireless State updates
		 */
		try {
			inquiryThreadObject = new InquiryThread(this, configuration.getWirelessStateUpdateInterval(), logger);
			inquiryThread = new boost::thread(*inquiryThreadObject);
		} catch (std::exception& e) {
			throw Exception(e.what(), logger);
		}
	} catch (Exception& e) {
		e.updateStackTrace("Cannot initialize ManagementServer!");
		throw;
	}

ManagementServer::~ManagementServer() {
	delete inquiryThreadObject;
	delete inquiryThread;

	rxData.clear();
	txData.clear();
}

bool ManagementServer::sendWirelessStateRequest() {
	/**
	 * Check if there's a GN connected
	 */
	if (!clientManager.isGnConnected()) {
		logger.warning("Attempted to send a Wireless Status Request but GN is not connected...");
		return false;
	}

	/**
	 * Fetch and validate client object
	 */
	ManagementClient* geoNetworkingClient = clientManager.getClientByEndpoint(recipient);

	if (!geoNetworkingClient)
		return false;

	/**
	 * Check if we have received a reply for the last packet we sent
	 */
	if (!geoNetworkingClient->isAlive()) {
		logger.error("This client did not reply to the last packet we sent, marking it as OFFLINE");
		geoNetworkingClient->setState(ManagementClient::OFFLINE);

		return false;
	}

	/**
	 * Create the relevant packet and serialize it
	 */
	GeonetWirelessStateRequestEventPacket request(logger);

	txData.resize(TX_BUFFER_SIZE);
	request.serialize(txData);

	/**
	 * Send serialized data thru socket
	 */
	this->send(txData, recipient);

	geoNetworkingClient->waitingForReply();

	/**
	 * Reset TX buffer
	 */
	txData.resize(TX_BUFFER_SIZE);

	return true;
}

void ManagementServer::readData() {
	try {
		/**
		 * Reset buffers
		 */
		rxData.resize(RX_BUFFER_SIZE);

		/**
		 * Register handleReceive() as the call-back of Rx
		 */
		socket.async_receive_from(boost::asio::buffer(rxData, ManagementServer::RX_BUFFER_SIZE), recipient,
				boost::bind(&ManagementServer::handleReceive, this,
						ba::placeholders::error,
						ba::placeholders::bytes_transferred));
	} catch (Exception& e) {
		e.updateStackTrace("Cannot receive data from asynchronous UDP socket!");
		throw;
	}
}

void ManagementServer::handleReceive(const boost::system::error_code& error, size_t size) {
	try {
		/**
		 * Resize RX buffer according to the amount of data received
		 */
		rxData.resize(size);

		if (!error) {
			logger.debug("Following " + boost::lexical_cast<string>(size) + " byte(s) received from " + recipient.address().to_string() + ":" + boost::lexical_cast<string>(recipient.port()));
			Util::printHexRepresentation(rxData.data(), size, logger);

			/**
			 * Utilize PacketHandler class to generate a response, if necessary
			 */
			try {
				handleClientData();
			} catch (Exception& e) {
				e.updateStackTrace("Cannot process Rx data!");
				throw;
			}
		} else {
			logger.warning("Error[code:" + boost::lexical_cast<string>(error.value()) + ", message:" + error.message() + "]");
			logger.info("Discarding incoming data...");
		}

		/**
		 * Read data again...
		 */
		readData();
	} catch (Exception& e) {
		e.updateStackTrace("Cannot handle a receive on asynchronous UDP socket!");
		throw;
	}
}

void ManagementServer::handleClientData() {
	/**
	 * PacketHandler class will return a result set
	 */
	PacketHandlerResult* result = NULL;
	ManagementClientManager::Task task;

	try {
		result = packetHandler.handle(rxData);
	} catch (Exception& e) {
		e.updateStackTrace("Cannot process incoming client data!");
		throw;
	}

	/**
	 * First inform Management Client Manager about this incoming packet (if it's valid)
	 */
	if (!result)
		return;

	else if (result->getResult() == PacketHandlerResult::DISCARD_PACKET
			|| result->getResult() == PacketHandlerResult::DELIVER_PACKET
			|| result->getResult() == PacketHandlerResult::SEND_CONFIGURATION_UPDATE_AVAILABLE
			|| result->getResult() == PacketHandlerResult::RELAY_TO_GN) {
		/**
		 * Inform Client Manager of this sender
		 */
		try {
			task = clientManager.updateClientState(recipient, (EventType)GeonetPacket::parseEventTypeOfPacketBuffer(rxData));
		} catch (Exception& e) {
			e.updateStackTrace("Cannot update Management Client's state according to incoming data!");
			throw;
		}
	}

	/**
	 * Do the necessary told by PacketHandler
	 */
	GeonetConfigurationAvailableEventPacket* configurationAvailablePacket = NULL;
	GeonetLocationUpdateEventPacket* locationUpdatePacket = NULL;
	/**
	 * Find the GN client as we might need to talk to it
	 */
	const ManagementClient* gnClient = clientManager.getClientByType(ManagementClient::GN);

	switch (result->getResult()) {
		case PacketHandlerResult::DISCARD_PACKET:
			delete result;
			break;

		case PacketHandlerResult::INVALID_PACKET:
			logger.error("Incoming packet is not valid, discarding..");
			delete result;
			break;

		case PacketHandlerResult::DELIVER_PACKET:
			/**
			 * Serialize response packet
			 */
			txData.resize(TX_BUFFER_SIZE);
			result->getPacket()->serialize(txData);

			/**
			 * Send serialized data thru socket
			 */
			this->send(txData, recipient);

			/**
			 * Reset TX buffer
			 */
			txData.resize(TX_BUFFER_SIZE);

			delete result;
			break;

		case PacketHandlerResult::SEND_CONFIGURATION_UPDATE_AVAILABLE:
			/**
			 * Update GN client with new configuration information
			 */
			if (!gnClient) {
				logger.info("There is no GN client connected right now, so a CONFIGURATION_AVAILABLE won't be sent");
				break;
			}

			/**
			 * Create a CONFIGURATION_UPDATE_AVAILABLE packet
			 */
			try {
				configurationAvailablePacket = new GeonetConfigurationAvailableEventPacket(mib, logger);
				/**
				 * Serialize...
				 */
				txData.resize(TX_BUFFER_SIZE);
				configurationAvailablePacket->serialize(txData);
			} catch (...) {
				throw Exception("Cannot create a CONFIGURATION_UPDATE_AVAILABLE packet!", logger);
			}

			logger.info("A CONFIGURATION_UPDATE_AVAILABLE packet is prepared, sending...");

			/**
			 * Send serialized data thru socket
			 */
			this->send(txData, udp::endpoint(udp::v4(), gnClient->getPort()));

			/**
			 * Reset TX buffer
			 */
			txData.resize(TX_BUFFER_SIZE);

			delete result;
			break;

		case PacketHandlerResult::RELAY_TO_GN:
			/**
			 * Relay incoming LOCATION_UPDATE (from FAC-CM) to CM-GN
			 */
			if (!gnClient) {
				logger.info("There is no GN client connected right now, so the LOCATION_UPDATE won't be relayed");
				break;
			}

			/**
			 * Create a LOCATION_UPDATE packet
			 */
			try {
				locationUpdatePacket = new GeonetLocationUpdateEventPacket(mib, logger);
				/**
				 * Serialize...
				 */
				txData.resize(TX_BUFFER_SIZE);
				locationUpdatePacket->serialize(txData);
			} catch (...) {
				throw Exception("Cannot create a LOCATION_UPDATE packet!", logger);
			}

			logger.info("A LOCATION_UPDATE packet is prepared, sending...");

			/**
			 * Send thru socket
			 */
			this->send(txData, udp::endpoint(udp::v4(), gnClient->getPort()));

			delete result;
			break;

		default:
			logger.warning("Something nasty happened, there's no such task for PacketHandler!");
			break;
	}

	/**
	 * Do the necessary told by ManagementClientHandler
	 */
	GeonetLocationTableRequestEventPacket* locationTableRequest = NULL;

	switch (task) {
		case ManagementClientManager::NOTHING:
			break;

		case ManagementClientManager::SEND_LOCATION_TABLE_REQUEST:
			/**
			 * Here we should send a LOCATION TABLE REQUEST
			 */
			try {
				locationTableRequest = new GeonetLocationTableRequestEventPacket(0xffffffffffffffff, logger);
				locationTableRequest->serialize(txData);
				delete locationTableRequest;
			} catch (...) {
				throw Exception("Cannot create/serialize a Location Table Request packet!", logger);
			}

			/**
			 * Send serialized data thru socket
			 */
			logger.info("Sending a LOCATION_TABLE_REQUEST packet");
			this->send(txData, recipient);

			break;

		default:
			logger.warning("Invalid task is returned by ManagementClientManager class!");
			break;
	}
}

void ManagementServer::handleSend(const boost::system::error_code& errorCode, size_t size) {
	/**
	 * Just print information about the data that has just been sent
	 */
	logger.debug("Following content (size=" + boost::lexical_cast<string>(size) + ") has been written onto the socket");
	Util::printHexRepresentation(txData.data(), size, logger);
}

bool ManagementServer::send(const vector<unsigned char>& buffer, const boost::asio::ip::udp::endpoint& recipient) {
	/**
	 * Ensure that there's only one entering here at any given time
	 */
	boost::lock_guard<boost::mutex> lock(txMutex);

	socket.async_send_to(ba::buffer(buffer), recipient,
			boost::bind(&ManagementServer::handleSend, this,
					ba::placeholders::error,
					ba::placeholders::bytes_transferred));

	return true;
}


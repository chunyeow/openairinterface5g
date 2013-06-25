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
 * \file mgmt_gn_packet_handler.cpp
 * \brief A container with packet handling functionality, all the packets read on the socket is passed here
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#include "mgmt_packet_handler.hpp"
#include "util/mgmt_exception.hpp"
#include <boost/lexical_cast.hpp>
#include "mgmt_types.hpp"
#include "util/mgmt_util.hpp"
#include <exception>
#include <iostream>
#include <cstring>
using namespace std;

PacketHandler::PacketHandler(ManagementInformationBase& mib, Logger& logger) :
	mib(mib), logger(logger) {
	try {
		this->packetFactory = new ManagementPacketFactory(mib, logger);
	} catch (...) {
		throw Exception("Cannot allocate a Geonet Packet Factory!", logger);
	}
}

PacketHandler::PacketHandler(const PacketHandler& packetHandler) :
	mib(packetHandler.mib), logger(packetHandler.logger) {
	throw Exception("Copy constructor for a PacketHandler object is called!", logger);
}

PacketHandler::~PacketHandler() {
	delete packetFactory;
}

PacketHandlerResult* PacketHandler::handle(const vector<unsigned char>& packetBuffer) {
	if (packetBuffer.size() < sizeof(MessageHeader)) {
		logger.error("Buffer size (" + boost::lexical_cast<string>(packetBuffer.size()) + " byte(s)) is not enough to carry a message!");
		logger.warning("Discarding packet...");
		return new PacketHandlerResult(PacketHandlerResult::INVALID_PACKET, NULL);
	}

	u_int16_t eventType = GeonetPacket::parseEventTypeOfPacketBuffer(packetBuffer);

	switch (eventType) {
		case MGMT_GN_EVENT_CONF_REQUEST:
			logger.info("GET_CONFIGURATION packet of size " + boost::lexical_cast<string>(packetBuffer.size()) + " has been received from GN");
			return handleGetConfigurationEvent(new GeonetGetConfigurationEventPacket(packetBuffer, logger), ManagementClient::GN);
		case MGMT_FAC_EVENT_CONF_REQUEST:
			logger.info("GET_CONFIGURATION packet of size " + boost::lexical_cast<string>(packetBuffer.size()) + " has been received from FAC");
			return handleGetConfigurationEvent(new GeonetGetConfigurationEventPacket(packetBuffer, logger), ManagementClient::FAC);

		case MGMT_GN_EVENT_STATE_NETWORK_STATE:
			logger.info("NETWORK_STATE packet of size " + boost::lexical_cast<string>(packetBuffer.size()) + " has been received");
			return handleNetworkStateEvent(new GeonetNetworkStateEventPacket(mib, packetBuffer, logger));

		case MGMT_GN_EVENT_STATE_WIRELESS_STATE_RESPONSE:
			logger.info("WIRELESS_STATE_RESPONSE packet (GN) of size " + boost::lexical_cast<string>(packetBuffer.size()) + " has been received");
			return handleWirelessStateResponseEvent(new GeonetWirelessStateResponseEventPacket(mib, packetBuffer, logger));

		case MGMT_LTE_EVENT_STATE_WIRELESS_STATE_RESPONSE:
			logger.info("WIRELESS_STATE_RESPONSE packet (LTE) of size " + boost::lexical_cast<string>(packetBuffer.size()) + " has been received");
			return handleWirelessStateResponseEvent(new LteWirelessStateResponseEventPacket(mib, packetBuffer, logger));

		case MGMT_GN_EVENT_CONF_COMM_PROFILE_REQUEST:
			logger.info("COMMUNICATION_PROFILE_REQUEST packet of size " + boost::lexical_cast<string>(packetBuffer.size()) + " has been received from GN");
			return handleCommunicationProfileRequestEvent(new GeonetCommunicationProfileRequestPacket(packetBuffer, logger), ManagementClient::GN);
		case MGMT_FAC_EVENT_CONF_COMM_PROFILE_REQUEST:
			logger.info("COMMUNICATION_PROFILE_REQUEST packet of size " + boost::lexical_cast<string>(packetBuffer.size()) + " has been received from FAC");
			return handleCommunicationProfileRequestEvent(new GeonetCommunicationProfileRequestPacket(packetBuffer, logger), ManagementClient::FAC);

		case MGMT_GN_EVENT_LOCATION_TABLE_RESPONSE:
			logger.info("LOCATION_TABLE_RESPONSE packet of size " + boost::lexical_cast<string>(packetBuffer.size()) + " has been received");
			return handleLocationTableResponse(new GeonetLocationTableResponseEventPacket(mib, packetBuffer, logger));

		case MGMT_FAC_EVENT_CONF_NOTIFICATION:
			logger.info("CONFIGURATION_NOTIFICATION packet of size " + boost::lexical_cast<string>(packetBuffer.size()) + " has been received");
			return handleConfigurationNotification(new FacConfigurationNotificationPacket(mib, packetBuffer, logger));

		case MGMT_FAC_EVENT_LOCATION_UPDATE:
			logger.info("LOCATION_UPDATE packet of size " + boost::lexical_cast<string>(packetBuffer.size()) + " has been received");
			return handleLocationUpdate(new GeonetLocationUpdateEventPacket(mib, packetBuffer, logger));

		case MGMT_FAC_EVENT_CONF_COMM_PROFILE_SELECTION_REQUEST:
			logger.info("COMM_PROF_SELECTION_REQUEST packet of size " + boost::lexical_cast<string>(packetBuffer.size()) + " has been received");
			return handleCommunicationProfileSelectionRequest(new FacCommunicationProfileSelectionRequestPacket(packetBuffer, logger));

		/**
		 * Handle unexpected packets as well
		 */
		case MGMT_GN_EVENT_LOCATION_TABLE_REQUEST:
		case MGMT_GN_EVENT_CONF_UPDATE_AVAILABLE:
		case MGMT_GN_EVENT_CONF_CONT_RESPONSE:
		case MGMT_FAC_EVENT_CONF_CONT_RESPONSE:
		case MGMT_GN_EVENT_CONF_BULK_RESPONSE:
		case MGMT_FAC_EVENT_CONF_BULK_RESPONSE:
		case MGMT_GN_EVENT_CONF_COMM_PROFILE_RESPONSE:
		case MGMT_FAC_EVENT_CONF_COMM_PROFILE_RESPONSE:
		case MGMT_GN_EVENT_STATE_WIRELESS_STATE_REQUEST:
		case MGMT_FAC_EVENT_CONF_COMM_PROFILE_SELECTION_RESPONSE:
			logger.error("Unexpected packet (event: " + boost::lexical_cast<string>(eventType) + ") received, connected client is buggy");
			logger.error("Ignoring...");
			return new PacketHandlerResult(PacketHandlerResult::INVALID_PACKET, NULL);

		case MGMT_EVENT_ANY:
		default:
			logger.error("Unknown message received, ignoring...");
			return new PacketHandlerResult(PacketHandlerResult::INVALID_PACKET, NULL);
	}

	return new PacketHandlerResult(PacketHandlerResult::DISCARD_PACKET, NULL);
}

PacketHandlerResult* PacketHandler::handleGetConfigurationEvent(GeonetGetConfigurationEventPacket* request, ManagementClient::ManagementClientType clientType) {
	if (!request)
		return new PacketHandlerResult(PacketHandlerResult::INVALID_PACKET, NULL);

	/**
	 * Create a response according to the request
	 */
	GeonetPacket* reply = this->packetFactory->createSetConfigurationEventPacket(static_cast<ItsKeyID> (request->getConfID()));

	/**
	 * Set the event type/sub-type according to the client type (no need to change anything
	 * if the client type is GN, it's the default value)
	 */
	if (clientType == ManagementClient::FAC) {
		/**
		 * Mind the type of the Set Configuration packet, it may be BULK or CONTINUOUS
		 */
		if (reply->getEventType() == MGMT_GN_EVENT_CONF_BULK_RESPONSE)
			reply->setEventType(MGMT_FAC_EVENT_CONF_BULK_RESPONSE);
		else
			reply->setEventType(MGMT_FAC_EVENT_CONF_CONT_RESPONSE);
	}

	/**
	 * Clean up
	 */
	delete request;

	logger.info("A SET_CONFIGURATION packet is prepared, will be sent soon...");

	return new PacketHandlerResult(PacketHandlerResult::DELIVER_PACKET, reply);
}

PacketHandlerResult* PacketHandler::handleNetworkStateEvent(GeonetNetworkStateEventPacket* request) {
	delete request;
	/*
	 * Creation of a GeonetNetworkStateEventPacket is enough for processing...
	 */
	return new PacketHandlerResult(PacketHandlerResult::DISCARD_PACKET, NULL);
}

PacketHandlerResult* PacketHandler::handleWirelessStateResponseEvent(GeonetPacket* request) {
	delete request;
	/*
	 * Creation of a {Lte|Gn}WirelessStateEventPacket is enough for processing...
	 */
	return new PacketHandlerResult(PacketHandlerResult::DISCARD_PACKET, NULL);
}

PacketHandlerResult* PacketHandler::handleLocationTableResponse(GeonetLocationTableResponseEventPacket* packet) {
	delete packet;
	/*
	 * Creation of a GeonetLocationTableResponseEventPacket is enough for processing...
	 */
	return new PacketHandlerResult(PacketHandlerResult::DISCARD_PACKET, NULL);
}

PacketHandlerResult* PacketHandler::handleConfigurationNotification(FacConfigurationNotificationPacket* packet) {
	if (!packet) {
		logger.warning("Invalid Configuration Notification packet received!");
		return NULL;
	}

	/**
	 * MIB is already notified about this ITS key value change in the
	 * constructor of FacConfigurationNotificationPacket class
	 */

	/**
	 * Command ManagementServer to notify GN about this update...
	 */
	return new PacketHandlerResult(PacketHandlerResult::SEND_CONFIGURATION_UPDATE_AVAILABLE, NULL);
}

PacketHandlerResult* PacketHandler::handleCommunicationProfileRequestEvent(GeonetCommunicationProfileRequestPacket* request, ManagementClient::ManagementClientType clientType) {
	if (!request)
		return new PacketHandlerResult(PacketHandlerResult::INVALID_PACKET, NULL);

	/**
	 * Create a response according to the request and send to the client right away
	 */
	GeonetPacket* reply = this->packetFactory->createCommunicationProfileResponse(request);

	/**
	 * Set the event type/sub-type according to the client type (no need to change anything
	 * if the client type is GN, it's the default value)
	 */
	if (clientType == ManagementClient::FAC)
		reply->setEventType(MGMT_FAC_EVENT_CONF_COMM_PROFILE_RESPONSE);

	/**
	 * Clean up
	 */
	delete request;

	return new PacketHandlerResult(PacketHandlerResult::DELIVER_PACKET, reply);
}

PacketHandlerResult* PacketHandler::handleCommunicationProfileSelectionRequest(FacCommunicationProfileSelectionRequestPacket* request) {
	if (!request)
		return new PacketHandlerResult(PacketHandlerResult::INVALID_PACKET, NULL);

	/**
	 * Create a response according to the request and send to the client right away
	 */
	GeonetPacket* reply = this->packetFactory->createCommunicationProfileSelectionResponse(request);

	/**
	 * Clean up
	 */
	delete request;

	return new PacketHandlerResult(PacketHandlerResult::DELIVER_PACKET, reply);
}

PacketHandlerResult* PacketHandler::handleLocationUpdate(GeonetLocationUpdateEventPacket* packet) {
	delete packet;
	/*
	 * Creation of a GeonetLocationUpdateEventPacket is enough for processing...
	 */
	return new PacketHandlerResult(PacketHandlerResult::RELAY_TO_GN, NULL);
}

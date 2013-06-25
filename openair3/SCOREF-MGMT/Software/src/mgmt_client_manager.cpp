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
 * \file mgmt_client_manager.cpp
 * \brief A container for a manager for Management clients
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
 */

#include "packets/mgmt_gn_packet_configuration_available.hpp"
#include "mgmt_client_manager.hpp"
#include "util/mgmt_exception.hpp"
#include <boost/lexical_cast.hpp>

ManagementClientManager::ManagementClientManager(ManagementInformationBase& mib, Configuration& configuration, Logger& logger)
	: mib(mib), configuration(configuration), logger(logger) {
}

ManagementClientManager::~ManagementClientManager() {
	clientVector.clear();
}

ManagementClientManager::Task ManagementClientManager::updateClientState(const udp::endpoint& clientEndpoint, EventType eventType) {
	bool clientExists = false;
	ManagementClient* client = NULL;

	/**
	 * Traverse client list and check if we already have this client
	 */
	for (vector<ManagementClient*>::const_iterator it = clientVector.begin(); it != clientVector.end(); ++it) {
		logger.trace("Comparing IP addresses " + (*it)->getAddress().to_string() + " and " + clientEndpoint.address().to_string());
		logger.trace("Comparing UDP ports " + boost::lexical_cast<string>((*it)->getPort()) + " and " + boost::lexical_cast<string>(clientEndpoint.port()));

		if ((*it)->getAddress() == clientEndpoint.address() && (*it)->getPort() == clientEndpoint.port()) {
			logger.debug("A client object for " + clientEndpoint.address().to_string() + ":" + boost::lexical_cast<string>(clientEndpoint.port()) + " is found");
			client = *it;
			clientExists = true;
		}
	}

	/**
	 * Create a new client object if we couldn't find one
	 */
	if (!clientExists) {
		ManagementClient* newClient = NULL;

		try {
			newClient = new ManagementClient(mib, clientEndpoint, configuration.getWirelessStateUpdateInterval(), configuration.getLocationUpdateInterval(), logger);
		} catch (Exception& e) {
			e.updateStackTrace("Cannot create a ManagementClient object!");
			throw;
		} catch (std::exception& e) {
			throw Exception(e.what(), logger);
		}

		clientVector.push_back(newClient);
		logger.info("A client object for " + clientEndpoint.address().to_string() + ":" + boost::lexical_cast<string>(clientEndpoint.port()) + " is created");

		client = newClient;
	}

	/**
	 * Update client's (the one either found or created) state according
	 * to the event type
	 */
	switch (eventType) {
		case MGMT_GN_EVENT_CONF_REQUEST:
		case MGMT_FAC_EVENT_CONF_REQUEST:
		case MGMT_FAC_EVENT_CONF_NOTIFICATION:
		case MGMT_GN_EVENT_STATE_WIRELESS_STATE_RESPONSE:
		case MGMT_GN_EVENT_STATE_NETWORK_STATE:
		case MGMT_GN_EVENT_CONF_COMM_PROFILE_REQUEST:
		case MGMT_FAC_EVENT_CONF_COMM_PROFILE_REQUEST:
		case MGMT_FAC_EVENT_CONF_COMM_PROFILE_SELECTION_REQUEST:
		case MGMT_GN_EVENT_LOCATION_TABLE_RESPONSE:
		case MGMT_FAC_EVENT_LOCATION_UPDATE:
		case MGMT_LTE_EVENT_STATE_WIRELESS_STATE_RESPONSE:
			client->setState(ManagementClient::ONLINE);
			break;

		/**
		 * Any other packet doesn't cause a state change for clients
		 */
		default:
			break;
	}

	/**
	 * Update client's type according to incoming message
	 */
	switch (eventType) {

		case MGMT_GN_EVENT_LOCATION_UPDATE:
		case MGMT_GN_EVENT_LOCATION_TABLE_REQUEST:
		case MGMT_GN_EVENT_LOCATION_TABLE_RESPONSE:
		case MGMT_GN_EVENT_CONF_UPDATE_AVAILABLE:
		case MGMT_GN_EVENT_CONF_REQUEST:
		case MGMT_GN_EVENT_CONF_CONT_RESPONSE:
		case MGMT_GN_EVENT_CONF_BULK_RESPONSE:
		case MGMT_GN_EVENT_CONF_COMM_PROFILE_REQUEST:
		case MGMT_GN_EVENT_CONF_COMM_PROFILE_RESPONSE:
		case MGMT_GN_EVENT_STATE_WIRELESS_STATE_REQUEST:
		case MGMT_GN_EVENT_STATE_WIRELESS_STATE_RESPONSE:
		case MGMT_GN_EVENT_STATE_NETWORK_STATE:
			client->setType(ManagementClient::GN);
			break;

		case MGMT_FAC_EVENT_LOCATION_UPDATE:
		case MGMT_FAC_EVENT_LOCATION_TABLE_REQUEST:
		case MGMT_FAC_EVENT_LOCATION_TABLE_RESPONSE:
		case MGMT_FAC_EVENT_CONF_REQUEST:
		case MGMT_FAC_EVENT_CONF_CONT_RESPONSE:
		case MGMT_FAC_EVENT_CONF_BULK_RESPONSE:
		case MGMT_FAC_EVENT_CONF_NOTIFICATION:
		case MGMT_FAC_EVENT_CONF_COMM_PROFILE_REQUEST:
		case MGMT_FAC_EVENT_CONF_COMM_PROFILE_RESPONSE:
		case MGMT_FAC_EVENT_CONF_COMM_PROFILE_SELECTION_REQUEST:
		case MGMT_FAC_EVENT_CONF_COMM_PROFILE_SELECTION_RESPONSE:
			client->setType(ManagementClient::FAC);
			break;

		case MGMT_LTE_EVENT_STATE_WIRELESS_STATE_RESPONSE:
			client->setType(ManagementClient::LTE);
			break;

		case MGMT_EVENT_ANY:
		default:
			logger.warning("Cannot determine client type by incoming event type/subtype!");
			client->setType(ManagementClient::UNKNOWN);
			break;
	}

	logger.info(toString());

	/**
	 * Return a task according to the client type
	 */
	if (client->getType() == ManagementClient::GN && !clientExists) {
		/**
		 * This is a new GN client so we should ask for Location Table
		 */
		return ManagementClientManager::SEND_LOCATION_TABLE_REQUEST;
	}

	return ManagementClientManager::NOTHING;
}

const ManagementClient* ManagementClientManager::getClientByType(ManagementClient::ManagementClientType clientType) const {
	/**
	 * Traverse client vector and find the specific client of given type
	 */
	for (vector<ManagementClient*>::const_iterator it = clientVector.begin(); it != clientVector.end(); ++it) {
		if ((*it)->getType() == clientType)
			return *it;
	}

	return NULL;
}

ManagementClient* ManagementClientManager::getClientByEndpoint(const udp::endpoint& endPoint) {
	/**
	 * Traverse client vector and find the specific client at given end point
	 */
	for (vector<ManagementClient*>::const_iterator it = clientVector.begin(); it != clientVector.end(); ++it) {
		if ((*it)->getEndpoint() == endPoint)
			return *it;
	}

	return NULL;
}

bool ManagementClientManager::isGnConnected() const {
	return (getClientByType(ManagementClient::GN) == NULL) ? false : true;
}

bool ManagementClientManager::isFacConnected() const {
	return (getClientByType(ManagementClient::FAC) == NULL) ? false : true;
}

string ManagementClientManager::toString() {
	stringstream ss;

	ss << "Client Status[count:" << clientVector.size() << "]" << endl;
	for (vector<ManagementClient*>::iterator it = clientVector.begin(); it != clientVector.end(); ++it)
		ss << (*it)->toString() << endl;

	return ss.str();
}

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
 * \file mgmt_client_manager.hpp
 * \brief A container for a manager for Management clients
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
 */

#ifndef MGMT_CLIENT_MANAGER_HPP_
#define MGMT_CLIENT_MANAGER_HPP_

#include "util/mgmt_udp_socket.hpp"
#include "mgmt_configuration.hpp"
#include "mgmt_types.hpp"
#include "util/mgmt_log.hpp"
#include "mgmt_client.hpp"

#include <vector>
using namespace std;

/**
 * A container for a manager for Management clients
 */
class ManagementClientManager {
	public:
		/**
		 * Following enumeration is used to tell caller of updateManagementClientState()
		 * method what steps are to follow
		 */
		enum Task {
			/**
			 * Nothing is to be done
			 */
			NOTHING = 0,
			/**
			 * Send a Location Table Request (only if the client is GN)
			 */
			SEND_LOCATION_TABLE_REQUEST = 1
		};

	public:
		/**
		 * Constructor for ManagementClientManager class
		 *
		 * @param mib Management Information Base reference
		 * @param configuration Management configuration to pass to/to manage client objects
		 * @param logger Logger object reference
		 */
		ManagementClientManager(ManagementInformationBase& mib, Configuration& configuration, Logger& logger);
		/**
		 * Destructor for ManagementClientManager class
		 */
		~ManagementClientManager();

	public:
		/**
		 * Handles incoming data and updates client vector accordingly, by creating
		 * a new client object if necessary or updating its state if there's one
		 * defined for sender source address
		 *
		 * @param clientEndpoint A udp::endpoint reference
		 * @param eventType Type/subtype of event the packet was sent for
		 * @return ManagementClientManager::Task for the caller
		 */
		ManagementClientManager::Task updateClientState(const udp::endpoint& clientEndpoint, EventType eventType);
		/**
		 * Returns relevant ManagementClient of given type
		 *
		 * @return A const pointer of the ManagementClient object of given type
		 */
		const ManagementClient* getClientByType(ManagementClient::ManagementClientType clientType) const;
		/**
		 * Returns the client at given end
		 *
		 * @param endPoint Connection end point information of type udp::endpoint
		 * @return A pointer of the relevant ManagementClient object
		 */
		ManagementClient* getClientByEndpoint(const udp::endpoint& endPoint);
		/**
		 * Tells if GN is connected
		 *
		 * @param none
		 * @return true if there's a GN connected, false otherwise
		 */
		bool isGnConnected() const;
		/**
		 * Tells if FAC is connected
		 *
		 * @param none
		 * @return true if there's a FAC connected, false otherwise
		 */
		bool isFacConnected() const;
		/**
		 * Returns the string representation of Client Manager and the clients it manages
		 *
		 * @param none
		 * @return std::string representation of this class
		 */
		string toString();

	private:
		/**
		 * Management Information Base reference
		 */
		ManagementInformationBase& mib;
		/**
		 * Client vector holding clients of connected/online (see ManagementClientState) state
		 */
		vector<ManagementClient*> clientVector;
		/**
		 * Configuration object reference to fetch necessary configuration information
		 */
		Configuration& configuration;
		/**
		 * Logger object reference
		 */
		Logger& logger;
};

#endif /* MGMT_CLIENT_MANAGER_HPP_ */

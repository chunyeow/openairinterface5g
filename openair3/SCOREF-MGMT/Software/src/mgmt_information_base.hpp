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
 * \file mgmt_information_base.hpp
 * \brief A container to hold configuration parameters of Management entity
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_INFORMATION_BASE_HPP_
#define MGMT_INFORMATION_BASE_HPP_

#include "mgmt_comm_prof_manager.hpp"
#include "mgmt_its_key_manager.hpp"
#include "mgmt_types.hpp"
#include <sys/types.h>
#include <string>
#include <map>

/**
 * A container to hold configuration parameters of Management entity
 */
class ManagementInformationBase {
	public:
		/**
		 * Constructor for ManagementInformationBase class
		 *
		 * @param Logger object reference
		 */
		ManagementInformationBase(Logger& logger);
		/**
		 * Destructor for ManagementInformationBase class
		 */
		~ManagementInformationBase();

	public:
		/**
		 * Initialises ItsKeyManager member by defining configuration items
		 *
		 * @return true on success, false otherwise
		 */
		bool initialise();
		/**
		 * Returns value of relevant ITS key through ItsKeyManager methods
		 *
		 * @param itsKeyId ITS key ID
		 * @return Value of relevant ITS key
		 */
		ItsKeyValue getItsKeyValue(ItsKeyID itsKeyId);
		/**
		 * Sets value of relevant ITS key through ItsKeyManager methods
		 *
		 * @param itsKeyId ITS key ID
		 * @param value Value to be set
		 * @return true on success, false otherwise
		 */
		bool setValue(ItsKeyID itsKeyId, ItsKeyValue value);
		/**
		 * Sets value of relevant ITS key through ItsKeyManager methods
		 *
		 * @param itsKeyId ITS key ID
		 * @param value Value to be set
		 * @return true on success, false otherwise
		 */
		bool setValue(ItsKeyID itsKeyId, const vector<unsigned char>& value);
		/**
		 * Sets value of relevant ITS key through ItsKeyManager methods
		 *
		 * @param configurationItemName std::string name of configuration item
		 * @param value Value to be set
		 * @return true on success, false otherwise
		 */
		bool setValue(const string& configurationItemName, ItsKeyValue value);
		/**
		 * Returns DWORD length of relevant ITS key
		 *
		 * @param itsKeyId ITS key ID
		 * @return byte length of given key of type std::size_t
		 */
		std::size_t getLength(ItsKeyID itsKeyId);
		/**
		 * Returns ItsKeyManager container's reference
		 *
		 * @return ItsKeyManager reference
		 */
		ItsKeyManager& getItsKeyManager();
		/**
		 * Returns wireless state of relevant interface
		 *
		 * @param interfaceId Interface ID of the interface
		 * @return A reference to WirelessStateResponseItem of relevant interface
		 */
		WirelessStateResponseItem* getWirelessState(InterfaceID interfaceId);
		/**
		 * Adds a new wireless state information for an interface
		 *
		 * @param interfaceId Interface ID of type InterfaceID
		 * @param wirelessState Wireless State information
		 * @return true on success, false otherwise
		 */
		bool updateWirelessState(InterfaceID interfaceId, WirelessStateResponseItem* wirelessState);
		/**
		 * Returns the network state of this MIB
		 *
		 * @return A reference to the NetworkStateMessage object of this MIB
		 */
		NetworkStateMessage& getNetworkState();
		/**
		 * Returns Communication Profile Manager reference
		 *
		 * @return Communication Profile Manager reference
		 */
		CommunicationProfileManager& getCommunicationProfileManager();
		/**
		 * Updates Location Table with given information
		 *
		 * @param locationTableItem A pointer to a location Table Item
		 * @return true on success, false otherwise
		 */
		bool updateLocationTable(LocationTableItem* locationTableItem);
		/**
		 * Returns location information
		 *
		 * @return LocationInformation structure
		 */
		const LocationInformation& getLocationInformation();
		/**
		 * Updates location information with given data
		 *
		 * @param locationUpdate A copy of a LocationInformation structure
		 * @return true on success, false otherwise
		 */
		bool setLocationInformation(const LocationInformation& locationUpdate);
		/**
		 * Sets network flags
		 *
		 * @param networkFlags Network flags of type u_int8_t
		 * @return true on success, false otherwise
		 */
		bool setNetworkFlags(const u_int8_t& networkFlags);
		/**
		 * Sets IPv6 configuration value, if this is not called then the
		 * default value "false" will stay intact
		 */
		void enableIpv6();
		/**
		 * Retuns a boolean indication IPv6 configuration status
		 */
		bool isIpv6Enabled() const;

	private:
		/**
		 * An object of ItsKeyManager class to keep track of ITS keys and their values
		 */
		ItsKeyManager itsKeyManager;
		/**
		 * Wireless and network state messages
		 */
		map<InterfaceID, WirelessStateResponseItem*> wirelessStateMap;
		/**
		 * Network state of this MIB entity
		 */
		NetworkStateMessage networkState;
		/**
		 * Location Table map
		 */
		u_int8_t networkFlags;
		/**
		 * Location table that consists of a map of LocationTableItem objects
		 */
		map<GnAddress, LocationTableItem*> locationTable;
		/**
		 * Communication profile manager
		 */
		CommunicationProfileManager communicationProfileManager;
		/**
		 * Location information
		 */
		LocationInformation location;
		/**
		 * Ipv6 configuration
		 */
		bool ipv6Enabled;
		/**
		 * Logger object reference
		 */
		Logger& logger;
};

#endif /* MGMT_INFORMATION_BASE_HPP_ */

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
 * \file mgmt_comm_prof_manager.hpp
 * \brief Communication Profiles list is kept and maintained by this container
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_COMM_PROF_MANAGER_HPP_
#define MGMT_COMM_PROF_MANAGER_HPP_

#include "util/mgmt_util.hpp"
#include "util/mgmt_log.hpp"
#include "mgmt_types.hpp"
#include <string>
#include <map>
using namespace std;

/**
 * Communication Profile Item
 */
typedef u_int32_t CommunicationProfileID;
struct CommunicationProfileItem {
	CommunicationProfileID id;
	u_int8_t transport;
	u_int8_t network;
	u_int8_t access;
	u_int8_t channel;

	/**
	 * Constructor for CommunicationProfileItem to
	 * initialise variables
	 */
	CommunicationProfileItem() {
		id = 0;
		transport = 0;
		network = 0;
		access = 0;
		channel = 0;
	}

	string toString() const {
		stringstream ss;

		ss << "[id:" << setfill('0') << setw(2) << id
			<< " transport:" << Util::getBinaryRepresentation(transport)
			<< " network:" << Util::getBinaryRepresentation(network)
			<< " access:" << Util::getBinaryRepresentation(access)
			<< " channel:" << Util::getBinaryRepresentation(channel) << "]";

		return ss.str();
	}
} __attribute__((packed));

/**
 * Communication Profiles list is kept and maintained by this container
 */
class CommunicationProfileManager {
	public:
		/**
		 * Constructor for CommunicationProfileManager class
		 *
		 * @param logger Logger object reference
		 */
		CommunicationProfileManager(Logger& logger);
		/**
		 * Destructor for CommunicationProfileManager class
		 */
		~CommunicationProfileManager();

	public:
		/**
		 * Inserts given communication profile information into the table
		 *
		 * @param profileIdString Communication profile ID string
		 * @param profileDefinitionString Communication profile details
		 * @return true on success, false otherwise
		 */
		bool insert(const string& profileIdString, const string& profileDefinitionString);
		/**
		 * Returns the number of profiles present
		 *
		 * @return Number of communication profiles, ie. table size
		 */
		u_int8_t getProfileCount() const;
		/**
		 * Returns communication profile map
		 *
		 * @return std::map of Communication Profile Table
		 */
		map<CommunicationProfileID, CommunicationProfileItem>& getProfileMap();
		/**
		 * Returns communication profile map subset filtered by incoming request map
		 *
		 * @param filter 32-bit filter part of a Communication Profile Request packet
		 * @return Filtered subset of communication profile
		 */
		map<CommunicationProfileID, CommunicationProfileItem> getProfileMapSubset(u_int32_t filter);
		/**
		 * In this method we have the intelligent code that'll pick a proper communication
		 * profile according to the parameters (or in other words the requirements) given
		 * by the client
		 *
		 * @param ipv6Enabled Indicates if IPv6 is enabled
		 * @param latency Latency requirement
		 * @param relevance Relevance requirement
		 * @param reliability Reliability requirement
		 * @return A communication profile ID of type CommunicationProfileID
		 */
		static CommunicationProfileID selectProfile(bool ipv6Enabled, u_int8_t latency, u_int8_t relevance, u_int8_t reliability);
		/**
		 * Returns string representation of Communication Profile Table
		 *
		 * return std::string representation of table
		 */
		string toString() const;

	private:
		/**
		 * Initialises profile item strings
		 */
		void initialise();

		/**
		 * Parses comma-separated Communication Profile string and returns
		 * a CommunicationProfileItem structure filled accordingly
		 *
		 * @param profileIdString Profile ID string (CP1, CP2, etc.)
		 * @param profileDefinitionString Comma-separated profile definition
		 * @return CommunicationProfileItem having parsed information
		 */
		CommunicationProfileItem parse(const string& profileIdString, const string& profileDefinitionString);
		/**
		 * A helper method to set any bits given in particular communication
		 * profile string, e.g. if a string "IPv4/v6:DSMIPv4/v6" is given then
		 * this method will set both 'IPv4/v6' and 'DSMIPv4/v6' flags in given
		 * octet
		 *
		 * @param octet Octet that the found out flags will be set
		 * @return true on success, false otherwise
		 */
		bool setFlags(const string& configuration, u_int8_t& octet);

	private:
		/**
		 * Communication profile map
		 */
		map<CommunicationProfileID, CommunicationProfileItem> communicationProfileMap;
		/**
		 * Communication profile string and bitmap index map
		 */
		map<string, u_int8_t> communicationProfileStringMap;
		/**
		 * Logger object reference
		 */
		Logger& logger;
};

#endif /* MGMT_COMM_PROF_MANAGER_HPP_ */

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
 * \file mgmt_configuration.hpp
 * \brief A container with configuration file parsing capability
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_CONFIGURATION_HPP_
#define MGMT_CONFIGURATION_HPP_

#include "mgmt_information_base.hpp"
#include "util/mgmt_log.hpp"
#include <string>
using namespace std;

/**
 * This is a little interface to pass to Communication Profile Manager without
 * exposing every single configuration detail
 */
class IPv6Configuration {
	public:
		/**
		 * Virtual destructor for IPv6Configuration class
		 */
		virtual ~IPv6Configuration() {}

	public:
		/**
		 * Returns a boolean that indicates IPv6 enable/disable configuration
		 */
		virtual bool isIpv6Enabled() const = 0;

	protected:
		/**
		 * IPv6 is enabled or not
		 */
		bool ipv6Enabled;
};

/**
 * A container with configuration file parsing capability, this class is utilised
 * to update ManagementInformationBase class with configuration file content
 */
class Configuration : public IPv6Configuration {
	public:
		/**
		 * Parameter string for UDP server port
		 */
		static const string CONF_SERVER_PORT_PARAMETER;
		/**
		 * Parameter string for repetition interval (in seconds)
		 * for Wireless State Request message
		 */
		static const string CONF_WIRELESS_STATE_UPDATE_INTERVAL;
		/**
		 * Parameter string for repetition interval (in seconds)
		 * for Location Update message
		 */
		static const string CONF_LOCATION_UPDATE_INTERVAL;
		/**
		 * Parameter string for IPv6 enabling
		 */
		static const string CONF_IPV6_ENABLED;
		/**
		 * Parameter string for log level
		 */
		static const string CONF_LOG_LEVEL;

	public:
		/**
		 * Constructor for Configuration class
		 *
		 * @param configurationFileNameList Configuration files that shall be parsed
		 * @param logger Logger object reference
		 */
		Configuration(const vector<string>& configurationFileNameVector, Logger& logger);
		/**
		 * Destructor for Configuration class
		 */
		~Configuration();

	public:
		/**
		 * Parses given set of configuration files and updates MIB accordingly
		 *
		 * @param mib Management Information Base reference
		 * @return true on success, false otherwise
		 */
		bool parseConfigurationFiles(ManagementInformationBase& mib);
		/**
		 * Returns configuration file name vector
		 *
		 * @return Configuration file name vector
		 */
		const vector<string>& getConfigurationFileVector() const;
		/**
		 * Adds a configuration file name to the vector
		 *
		 * @param configurationFileName New configuration file's name
		 * @return none
		 */
		void addConfigurationFile(const string& configurationFileName);
		/**
		 * Sets the directory name where Configuration class is going to
		 * look for FACilities configuration files
		 *
		 * @param directory Directory name (relative to ./)
		 * @return none
		 */
		void setFacilitiesConfigurationDirectory(const string& directory);
		/**
		 * Returns UDP server port number
		 *
		 * @param none
		 * @return UDP server port number
		 */
		int getServerPort() const;
		/**
		 * Sets UDP server port number
		 *
		 * @param new port number
		 * @return none
		 */
		void setServerPort(int serverPort);
		/**
		 * Returns Wireless State Update interval (in seconds)
		 *
		 * @return Wireless State Update interval in seconds
		 */
		u_int8_t getWirelessStateUpdateInterval() const;
		/**
		 * Sets Wireless State Update interval
		 *
		 * @param interval Wireless State Update interval in seconds
		 * @return none
		 */
		void setWirelessStateUpdateInterval(u_int8_t interval);
		/**
		 * Returns Location Update interval (in seconds)
		 *
		 * @return Location Update interval in seconds
		 */
		u_int8_t getLocationUpdateInterval() const;
		/**
		 * Sets Location Update interval
		 *
		 * @param interval Location Update interval in seconds
		 * @return none
		 */
		void setLocationUpdateInterval(u_int8_t interval);
		/**
		 * Returns a boolean indication IPv6 configuration
		 * @implements bool IPv6Configuration::isIpv6Enabled()
		 */
		bool isIpv6Enabled() const;

	private:
		/**
		 * Parses incoming string of format "parameter = value" and fills in
		 * passed parameter and value variables
		 *
		 * @param line Configuration file line
		 * @param parameter Parameter's name
		 * @param value Parameter's value
		 * @return true on success, false otherwise
		 */
		static bool parseLine(const string& line, string& parameter, string& value);
		/**
		 * Parses IHM configuration IDs of type <configurationItemName|configurationItemID>
		 *
		 * @param param Parameter string (input)
		 * @param parameterString "configurationItemName" part of the parameter string (output)
		 * @param parameterID "configurationItemID" part of the parameter string (output)
		 * @return bool true on success, false otherwise
		 */
		static bool parseParameterId(const string& param, string& parameterString, u_int16_t& parameterId);
		/**
		 * Sets configuration parameter's value with given value
		 *
		 * @param parameter Parameter name
		 * @param value Parameter value
		 * @return true on success, false otherwise
		 */
		bool setValue(const string& parameter, const string& value);

	private:
		/**
		 * Directory where FACilities configuration files reside
		 */
		string facilitiesConfigurationDirectory;
		/**
		 * Configuration files name vector
		 */
		vector<string> configurationFileNameVector;
		/**
		 * UDP server port number
		 */
		int serverPort;
		/**
		 * Wireless State Update interval (in seconds)
		 */
		u_int8_t wirelessStateUpdateInterval;
		/**
		 * Location Update interval (in seconds)
		 */
		u_int8_t locationUpdateInterval;
		/**
		 * Log level configuration
		 */
		Logger::LOG_LEVEL logLevel;
		/**
		 * Logger object reference
		 */
		Logger& logger;
};

#endif /* MGMT_CONFIGURATION_HPP_ */

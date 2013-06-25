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
 * \file mgmt_configuration.cpp
 * \brief A container with configuration file parsing capability
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#include "util/mgmt_exception.hpp"
#include "mgmt_configuration.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <iostream>
#include <fstream>
using namespace boost;
using namespace std;

/**
 * Initialise configuration parameter name strings
 */
const string Configuration::CONF_SERVER_PORT_PARAMETER("CONF_SERVER_PORT");
const string Configuration::CONF_WIRELESS_STATE_UPDATE_INTERVAL("CONF_WIRELESS_STATE_UPDATE_INTERVAL");
const string Configuration::CONF_LOCATION_UPDATE_INTERVAL("CONF_LOCATION_UPDATE_INTERVAL");
const string Configuration::CONF_IPV6_ENABLED("CONF_IPV6_ENABLED");
const string Configuration::CONF_LOG_LEVEL("CONF_LOG_LEVEL");

Configuration::Configuration(const vector<string>& configurationFileNameVector, Logger& logger)
	: configurationFileNameVector(configurationFileNameVector), logger(logger) {

	/**
	 * Write given configuration files' names
	 */
	stringstream ss;
	vector<string>::const_iterator it = configurationFileNameVector.begin();
	while (it != configurationFileNameVector.end())
		/**
		 * Following is a nasty-looking one-liner not to put a comma at the end
		 */
		ss << *it++ << ((it+1 == configurationFileNameVector.end()) ? "" : ", ");

	logger.info("Following " + boost::lexical_cast<string>(configurationFileNameVector.size()) + \
			" file(s) have been found (only those with .conf extension will be parsed): " + ss.str());

	/**
	 * Set default values
	 */
	this->serverPort = 1402;
	this->wirelessStateUpdateInterval = 10;
	this->locationUpdateInterval = 20;
	this->ipv6Enabled = false;
}

Configuration::~Configuration() {
	configurationFileNameVector.clear();
}

bool Configuration::parseConfigurationFiles(ManagementInformationBase& mib) {
	/**
	 * Verify there is at least one configuration file given to be parsed
	 */
	if (configurationFileNameVector.empty())
		throw Exception("No configuration file name is given to be parsed", logger);

	ifstream configurationFileStream;
	string parameter, value;
	string line;
	string netParameterPrefix("MIB_GN_NET");
	string facParameterPrefix("MIB_GN_FAC");
	string ihmParameterPrefix("MIB_MC002");
	string commonParameterPrefix("MIB_GN_ALL");
	string confParameterPrefix("CONF_");
	string communicationProfilePrefix("CP");

	/*
	 * Here we traverse configuration files and let MIB know about
	 * every single parameter/[parameterID/]value pair we find
	 */
	for (vector<string>::const_iterator it = configurationFileNameVector.begin(); it != configurationFileNameVector.end(); ++it) {
		/**
		 * Check file name extension and do not parse if it's not '.conf'
		 */
		string fileExtension = Util::getFileExtension(*it);
		if (fileExtension.compare(".conf") != 0) {
			logger.warning("There is a file named '" + *it + "' without .conf extension in given list");
			continue;
		}

		string configurationFile = *it;
		/**
		 * Add directory name if a FACilities configuration file is being parsed
		 */
		if (configurationFile.compare("MGMT.conf") != 0)
			configurationFile = facilitiesConfigurationDirectory + configurationFile;

		logger.info("Parsing configuration file '" + configurationFile + "'");
		configurationFileStream.open(configurationFile.c_str());

		if (configurationFileStream.is_open()) {
			/**
			 * Traverse and parse configuration file lines
			 */
			while (!configurationFileStream.eof()) {
				getline(configurationFileStream, line);

				if (parseLine(line, parameter, value)) {
					/**
					 * Trim value if it's not a string
					 */
					if (value.find('"') == string::npos)
						value = Util::trim(value, '\0');

					/*
					 * NETwork and FACilities parameters are sent to MIB
					 */
					if (!line.compare(0, netParameterPrefix.length(), netParameterPrefix) ||
							!line.compare(0, facParameterPrefix.length(), facParameterPrefix) ||
							!line.compare(0, commonParameterPrefix.length(), commonParameterPrefix)) {
						try {
							ItsKeyValue valueContainer;
							valueContainer.intValue = atoi(value.c_str());
							if (mib.setValue(parameter, valueContainer))
								logger.info("Parameter '" + parameter + "' has been set to '" + value + "'");
						} catch (Exception& e) {
							e.updateStackTrace("Cannot set MIB ITS key using value given in the configuration file");
							throw;
						}
					/*
					 * General configuration parameters are handled locally in this class
					 */
					} else if (!line.compare(0, confParameterPrefix.size(), confParameterPrefix)) {
						try {
							setValue(parameter, value);
						} catch (Exception& e) {
							e.updateStackTrace("Cannot process given configuration value");
							throw;
						}
					/*
					 * Communication profiles are sent to MIB
					 */
					} else if (!line.compare(0, communicationProfilePrefix.size(), communicationProfilePrefix)) {
						try {
							mib.getCommunicationProfileManager().insert(parameter, value);
						} catch (Exception& e) {
							e.updateStackTrace("Cannot process communication profile string");
							throw;
						}
					/**
					 * Point de Charge Vehicule Electrique (PCVE) IHM Parameters
					 */
					} else if (!line.compare(0, ihmParameterPrefix.size(), ihmParameterPrefix)) {
						/**
						 * Parse parameter of format <configurationParameter|configurationParameterID>
						 */
						string parameterName;
						u_int16_t parameterId;
						parseParameterId(parameter, parameterName, parameterId);

						/**
						 * Update MIB and add new ITS key but before check ITS data type (it
						 * can be integer and string)
						 */
						if (Util::isNumeric(value)) {
							logger.info("Adding IHM integer parameter [name:" + parameterName + ", id:" + boost::lexical_cast<string>(parameterId) + ", value:" + value + "]");
							mib.getItsKeyManager().addKey(static_cast<ItsKeyID>(parameterId), parameterName, ITS_KEY_TYPE_IHM, atoi(value.c_str()));
						} else if (value.find('.') != string::npos) {
							logger.info("Adding IHM float parameter [name:" + parameterName + ", id:" + boost::lexical_cast<string>(parameterId) + ", value:" + value + "]");
							mib.getItsKeyManager().addKey(static_cast<ItsKeyID>(parameterId), parameterName, ITS_KEY_TYPE_IHM, boost::lexical_cast<float>(value));
						} else {
							logger.info("Adding IHM string parameter [name:" + parameterName + ", id:" + boost::lexical_cast<string>(parameterId) + ", value:" + value + "]");
							/**
							 * Parse value trimming '"' characters and whitespace if it's not "NULL"
							 */
							if (value.find("NULL") == string::npos) {
								value = value.substr(value.find('"') + 1, value.length() - value.find('"') - 2);
							} else {
								// Since NULL actually means empty string
								value = "";
							}
							mib.getItsKeyManager().addKey(static_cast<ItsKeyID>(parameterId), parameterName, ITS_KEY_TYPE_IHM, value);
						}
					} else {
						logger.warning("Unrecognized configuration item: '" + parameter + "'");
					}
				}
			}
			logger.debug("Closing file stream for '" + configurationFile + "'");
			configurationFileStream.close();
		} else {
			configurationFileStream.close();
			throw Exception("Cannot open configuration file '" + configurationFile + "'!", logger);
		}
	}

	return true;
}

bool Configuration::parseLine(const string& line, string& parameter, string& value) {
	/**
	 * Get the substring till '#' character if there's one
	 */
	string configurationLine = line;
	if (line.find('#') != string::npos) {
		configurationLine.erase(0, configurationLine.length() - configurationLine.find('#'));
	}

	/**
	 * Ignore this line if it's empty or there's no equal sign in it
	 */
	if (configurationLine.empty() || configurationLine.find('=') == string::npos)
		return false;

	/**
	 * Parse the line according to the place of equal sign
	 */
	parameter = configurationLine.substr(0, configurationLine.find("="));
	value = configurationLine.substr(configurationLine.find("=") + 1, configurationLine.length());

	/**
	 * Trim value string if there's no '"' character
	 */
	remove(parameter.begin(), parameter.end() + 1, ' ');
	if (value.find('"') == string::npos)
		remove(value.begin(), value.end() + 1, ' ');

	return true;
}

bool Configuration::parseParameterId(const string& param, string& parameterString, u_int16_t& parameterId) {
	if (param.find('|') == string::npos)
		return false;

	/**
	 * Parse the string according to the separator '|'
	 */
	parameterString = param.substr(0, param.find('|'));
	stringstream parameterIdStringStream;
	parameterIdStringStream << hex << param.substr(param.find('|') + 1, param.length());
	/**
	 * Cast parameter ID string and update incoming variable reference
	 */
	parameterIdStringStream >> parameterId;

	return true;
}

bool Configuration::setValue(const string& parameter, const string& value) {
	/**
	 * Parse CONF_SERVER_PORT parameter
	 */
	if (!parameter.compare(0, CONF_SERVER_PORT_PARAMETER.length(), CONF_SERVER_PORT_PARAMETER)) {
		setServerPort(atoi(value.c_str()));
	/**
	 * Parse CONF_WIRELESS_STATE_UPDATE_INTERVAL parameter
	 */
	} else if (!parameter.compare(0, CONF_WIRELESS_STATE_UPDATE_INTERVAL.length(), CONF_WIRELESS_STATE_UPDATE_INTERVAL)) {
		setWirelessStateUpdateInterval(atoi(value.c_str()));
	/**
	 * Parse CONF_LOCATION_UPDATE_INTERVAL parameter
	 */
	} else if (!parameter.compare(0, CONF_LOCATION_UPDATE_INTERVAL.length(), CONF_LOCATION_UPDATE_INTERVAL)) {
		/**
		 * This configuration parameter is removed because Location Update is now sent by
		 * a module developed by Laurens from POTI directly to GN
		 *
		 * Add the following configuration parameter into MGMT.conf to get this back
		 *
		 * CONF_LOCATION_UPDATE_INTERVAL = 30
		 */
		setLocationUpdateInterval(atoi(value.c_str()));
	/**
	 * Parse CONF_IPV6_ENABLED parameter
	 */
	} else if (!parameter.compare(0, CONF_IPV6_ENABLED.length(), CONF_IPV6_ENABLED)) {
		if (atoi(value.c_str()) == 1)
			ipv6Enabled = true;
		else if (atoi(value.c_str()) == 0)
			ipv6Enabled = false;
		else
			throw Exception(CONF_IPV6_ENABLED + " can be 0 or 1, invalid value (" + value + ") provided", logger);

		logger.info(string("IPv6 is ") + ((ipv6Enabled) ? "enabled" : "disabled"));
	/**
	 * Parse CONF_LOG_LEVEL parameter
	 */
	} else if (!parameter.compare(0, CONF_LOG_LEVEL.length(), CONF_LOG_LEVEL)) {
		if (atoi(value.c_str()) < 0 || atoi(value.c_str()) > 4)
			throw Exception(CONF_LOG_LEVEL + " can be 0 to 4, invalid value (" + value + ") provided", logger);
		/**
		 * Update the log level
		 */
		logger.setLogLevel(static_cast<Logger::LOG_LEVEL>(atoi(value.c_str())));
		logger.info("Log level is updated to " + logger.getCurrentLogLevelName());
	}

	return true;
}

const vector<string>& Configuration::getConfigurationFileVector() const {
	return configurationFileNameVector;
}

void Configuration::addConfigurationFile(const string& configurationFileName) {
	this->configurationFileNameVector.push_back(configurationFileName);
}

void Configuration::setFacilitiesConfigurationDirectory(const string& directory) {
	facilitiesConfigurationDirectory = directory;
}

int Configuration::getServerPort() const {
	return serverPort;
}

void Configuration::setServerPort(int serverPort) {
	if (serverPort > 0 && serverPort < 65535)
		this->serverPort = serverPort;
	/**
	 * Do nothing and so keep the default value otherwise
	 */
}

u_int8_t Configuration::getWirelessStateUpdateInterval() const {
	return wirelessStateUpdateInterval;
}

void Configuration::setWirelessStateUpdateInterval(u_int8_t interval) {
	/**
	 * Verify incoming value for wireless state update interval
	 * Keep default value if incoming is invalid
	 */
	if (interval >= 10 && interval <= 120) {
		logger.info("Setting Wireless State Update Interval to " + boost::lexical_cast<string>((int)interval) + " seconds");
		wirelessStateUpdateInterval = interval;
	} else
		logger.warning("Parsed value (" + boost::lexical_cast<string>((int)interval) + ") of Wireless State Update Interval is invalid [min=10,max=120], keeping default value (" + boost::lexical_cast<string>((int)wirelessStateUpdateInterval) + ")");
}

u_int8_t Configuration::getLocationUpdateInterval() const {
	return locationUpdateInterval;
}

void Configuration::setLocationUpdateInterval(u_int8_t interval) {
	/**
	 * Verify incoming value for location update interval
	 * Keep default value if incoming is invalid
	 */
	if (interval >= 20 && interval <= 120) {
		logger.info("Setting Location Update Interval to " + boost::lexical_cast<string>((int)interval) + " seconds");
		locationUpdateInterval = interval;
	} else
		logger.warning("Parsed value (" + boost::lexical_cast<string>((int)interval) + ") of Location Update Interval is invalid [min=20,max=120], keeping default value (" + boost::lexical_cast<string>((int)locationUpdateInterval) + ")");
}

bool Configuration::isIpv6Enabled() const {
	return ipv6Enabled;
}

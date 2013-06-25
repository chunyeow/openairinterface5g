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
 * \file main.cpp
 * \brief SCOREF Management module's hometown
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#include <iostream>
#include <vector>
#include <string>
#include <ctime>
using namespace std;

#include <boost/program_options.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
using boost::asio::ip::udp;

#include "mgmt_packet_handler.hpp"
#include "mgmt_client_manager.hpp"
#include "util/mgmt_exception.hpp"
#include "mgmt_configuration.hpp"
#include "util/mgmt_util.hpp"
#include "util/mgmt_log.hpp"
#include "mgmt_server.hpp"
#include "version.hpp"

void printVersion() {
	cerr << "SCORE@F MANAGEMENT Module version " << MGMT_VERSION << endl;
}
void printHelp(const string& binaryName) {
	cerr << binaryName << " <configurationFile> [logFileName]" << endl;
}

#ifdef BOOST_VERSION_1_50
const string CONF_HELP_PARAMETER_STRING = "help";
const string CONF_LOG_LEVEL_PARAMETER_STRING = "loglevel";
#endif

int main(int argc, char** argv) {
	string logFileName, configurationFileName;

	/**
	 * Process command-line parameters
	 * Configuration file name is necessary yet log file name is optional here
	 */
	if (argc > 1 && (!string(argv[1]).compare("-v") || !string(argv[1]).compare("--version"))) {
		printVersion();
		exit(0);
	} else if (argc == 2) {
		logFileName = "SCOREF-MGMT.log";
		configurationFileName = argv[1];
	} else if (argc == 3) {
		configurationFileName = argv[1];
		logFileName = argv[2];
	} else {
		printHelp(argv[0]);
		exit(1);
	}

#ifdef BOOST_VERSION_1_50
	/**
	 * Define and parse command-line parameters
	 */
	po::options_description commandLineOptions("Command-line options");
	desc.add_options()
			(CONF_HELP_PARAMETER_STRING, "Print help message")
			(CONF_LOG_LEVEL_PARAMETER_STRING, po::value<int>(), "Set log level (DEBUG=0, INFO=1, WARNING=2, ERROR=3)");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, commandLineOptions), vm);
	po::notify(vm);

	if (vm.count(CONF_HELP_PARAMETER_STRING)) {
		printHelp(argv[0]);
		return 1;
	}

	if (vm.count(CONF_LOG_LEVEL_PARAMETER_STRING)) {
		logger.info("Log level was set to " + vm[CONF_LOG_LEVEL_PARAMETER_STRING].as<int>());
		logger.setLogLevel(vm[CONF_LOG_LEVEL_PARAMETER_STRING.as<int>]);
	} else {
		logger.info("Compression level was not set, default is DEBUG");
	}
#endif

	/**
	 * Create a Logger object
	 */
	Logger logger(logFileName, Logger::TRACE);

	/**
	 * Prepare the list of FACilities configuration files by traversing
	 * the configration/ directory's content
	 */
	string facilitiesConfigurationFileDirectory = "configuration/";
	vector<string> configurationFileVector = Util::getListOfFiles(facilitiesConfigurationFileDirectory);
	/**
	 * Add MGMT module's configuration file to the list
	 */
	configurationFileVector.push_back(configurationFileName);
	Configuration configuration(configurationFileVector, logger);
	configuration.setFacilitiesConfigurationDirectory(facilitiesConfigurationFileDirectory);

	/**
	 * Create the father
	 */
	ManagementInformationBase mib(logger);

	/**
	 * Parse configuration file and create UDP server socket
	 */
	try {
		configuration.parseConfigurationFiles(mib);
	} catch (Exception& e) {
		e.updateStackTrace("Cannot parse the configuration file");
		e.printStackTrace();
		exit(-1);
	}

	/**
	 * Update IPv6 configuration
	 */
	if (configuration.isIpv6Enabled())
		mib.enableIpv6();

	/**
	 * Initialise MIB
	 */
	try {
		mib.initialise();
	} catch (Exception& e) {
		e.updateStackTrace("Cannot initialise ManagementInformationBase!");
		throw;
	}

	/**
	 * Instantiate a Client Manager and pass it to Management Server
	 */
	boost::asio::io_service ioService;
	ManagementClientManager clientManager(mib, configuration, logger);
	ManagementServer server(ioService, configuration, mib, clientManager, logger);

	try {
		ioService.run();
	} catch (Exception& e) {
		e.updateStackTrace("Cannot run I/O service!");
		e.printStackTrace();
	} catch (std::exception& e) {
		logger.error(e.what());
	}

	logger.info("Exiting...");

	return 0;
}

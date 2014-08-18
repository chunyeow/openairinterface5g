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
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06410 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

#include "../../src/mgmt_configuration.hpp"
#include "test_configuration_util.hpp"
#include <gtest/gtest.h>

#define TEST_CONFIGURATION_FILE_1 "../data/confFiles/testConfFile1.conf"
#define TEST_CONFIGURATION_FILE_2 "../data/confFiles/testConfFile2.conf"
#define TEST_CONFIGURATION_FILE_3 "../data/confFiles/testConfFile3.conf"

void testParseConfigurationFiles(Configuration& configuration, ManagementInformationBase& mib, Logger& logger) {
	logger.info("Testing Configuration::parseConfigurationFiles() method");

	/**
	 * Verify if this method returns FALSE when no configuration file is given
	 */
	EXPECT_FALSE(testParseConfigurationFilesNonexistentFile(configuration, logger));

	/**
	 * Verify if configuration parameters are parsed correctly
	 */
	configuration.addConfigurationFile(TEST_CONFIGURATION_FILE_1);
	configuration.addConfigurationFile(TEST_CONFIGURATION_FILE_2);

	/**
	 * Verify number of configuration files and that they are parsed successfully
	 */
	ASSERT_EQ(2, configuration.getConfigurationFileVector().size());
	ASSERT_TRUE(configuration.parseConfigurationFiles(mib));
}

#ifdef PRIVATE
void testParseLine(Configuration& configuration, Logger& logger) {
	logger.info("Testing Configuration::parseLine() method");

	string line = "parameter = value";
	string parameter, value;

	EXPECT_TRUE(configuration.parseLine(line, parameter, value));
	EXPECT_STREQ("parameter", parameter.c_str());
	EXPECT_STREQ("value", value.c_str());
}

void testParseParameterId(Configuration& configuration, Logger& logger) {
	logger.info("Testing Configuration::parseParameterId() method");

	string testInput = "MIB_MC002_TEST1|0xC100";
	string parameterString;
	u_int16_t parameterId;

	EXPECT_TRUE(configuration.parseParameterId(testInput, parameterString, parameterId));
	EXPECT_STREQ("MIB_MC002_TEST1", parameterString.c_str());
	EXPECT_EQ(0xC100, parameterId);
}

void testSetValue(Configuration& configuration, Logger& logger) {
	logger.info("Testing Configuration::setValue() method");

	/**
	 * Verify that it returns FALSE for a non-existent parameter
	 */
	EXPECT_FALSE(configuration.setValue("CONF_THIS_DOES_NOT_EXISTS", "1234"));

	/**
	 * Verify that it updates a configuration item's value correctly
	 */
	EXPECT_TRUE(configuration.setValue("CONF_SERVER_PORT", "9000"));
	EXPECT_EQ(9000, configuration.getServerPort());
}
#endif

void testGetConfigurationFileVector(Configuration& configuration, Logger& logger) {
	logger.info("Testing Configuration::getConfigurationFileVector() method");

	vector<string> configurationFileNameVector = configuration.getConfigurationFileVector();

	EXPECT_EQ(2, configurationFileNameVector.size());
	EXPECT_STREQ(TEST_CONFIGURATION_FILE_1, configurationFileNameVector[0].c_str());
	EXPECT_STREQ(TEST_CONFIGURATION_FILE_2, configurationFileNameVector[1].c_str());
}

void testAddConfigurationFile(Configuration& configuration, Logger& logger) {
	logger.info("Testing Configuration::addConfigurationFile() method");

	string newConfigurationFile = "../data/confFiles/testConfFile3.conf";
	configuration.addConfigurationFile(newConfigurationFile);

	vector<string> configurationFileNameVector = configuration.getConfigurationFileVector();

	EXPECT_EQ(3, configurationFileNameVector.size());
	EXPECT_STREQ(TEST_CONFIGURATION_FILE_1, configurationFileNameVector[0].c_str());
	EXPECT_STREQ(TEST_CONFIGURATION_FILE_2, configurationFileNameVector[1].c_str());
	EXPECT_STREQ(TEST_CONFIGURATION_FILE_3, configurationFileNameVector[2].c_str());
}

void testGetServerPort(Configuration& configuration, Logger& logger) {
	logger.info("Testing Configuration::getServerPort() method");

	u_int16_t serverPort = configuration.getServerPort();

	EXPECT_EQ(9999, serverPort);
}

void testSetServerPort(Configuration& configuration, Logger& logger) {
	logger.info("Testing Configuration::setServerPort() method");

	configuration.setServerPort(9999);

	EXPECT_EQ(9999, configuration.getServerPort());
}

void testGetWirelessStateUpdateInterval(Configuration& configuration, Logger& logger) {
	logger.info("Testing Configuration::getWirelessStateUpdateInterval() method");

	EXPECT_EQ(100, configuration.getWirelessStateUpdateInterval());
}

void testSetWirelessStateUpdateInterval(Configuration& configuration, Logger& logger) {
	logger.info("Testing Configuration::setWirelessStateUpdateInterval() method");

	/**
	 * Verify that invalid values are not set
	 */
	configuration.setWirelessStateUpdateInterval(9);
	EXPECT_EQ(100, configuration.getWirelessStateUpdateInterval());
	configuration.setWirelessStateUpdateInterval(121);
	EXPECT_EQ(100, configuration.getWirelessStateUpdateInterval());

	/**
	 * Verify that valid values are set
	 */
	configuration.setWirelessStateUpdateInterval(120);
	EXPECT_EQ(120, configuration.getWirelessStateUpdateInterval());
}

void testGetLocationUpdateInterval(Configuration& configuration, Logger& logger) {
	logger.info("Testing Configuration::getLocationUpdateInterval() method");

	EXPECT_EQ(20, configuration.getLocationUpdateInterval());
}

void testSetLocationUpdateInterval(Configuration& configuration, Logger& logger) {
	logger.info("Testing Configuration::setLocationUpdateInterval() method");

	/**
	 * Verify that invalid values are not set
	 */
	configuration.setLocationUpdateInterval(19);
	EXPECT_EQ(20, configuration.getLocationUpdateInterval());
	configuration.setLocationUpdateInterval(121);
	EXPECT_EQ(20, configuration.getLocationUpdateInterval());

	/**
	 * Verify that valid values are set
	 */
	configuration.setLocationUpdateInterval(30);
	EXPECT_EQ(30, configuration.getLocationUpdateInterval());
}

/**
 * This method tests internal configuration data of Configuration class
 */
void testConfiguration(Configuration& configuration, ManagementInformationBase& mib, Logger& logger) {
	/**
	 * Test ItsKeyManager member of Configuration class
	 */
	EXPECT_TRUE(testItsKeyManager(configuration, mib, logger));
	/**
	 * Verify Communication Profile information
	 */
	testCommunicationProfiles(configuration, mib, logger);
}

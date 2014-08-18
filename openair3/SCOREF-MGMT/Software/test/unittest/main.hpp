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

#ifndef MAIN_HPP_
#define MAIN_HPP_

#include <boost/lexical_cast.hpp>
#include <gtest/gtest.h>

#include "../../src/mgmt_configuration.hpp"

#include "test_configuration.hpp"
#include "test_util.hpp"

namespace ScorefTest {
	/*
	 * The fixture for SCOREF-MGMT testing
	 */
	class ScorefManagementTest: public ::testing::Test {
		protected:
			ScorefManagementTest() {
				try {
					logger = new Logger("test.log", Logger::TRACE, Logger::STDOUT);
				} catch (...) {
					cerr << "Cannot create a Logger object!" << endl;
					throw;
				}
				try {
					vector<string> configurationNameVector;
					configuration = new Configuration(configurationNameVector, *logger);
				} catch (...) {
					cerr << "Cannot create a Configuration object!" << endl;
					throw;
				}
				try {
					managementInformationBase = new ManagementInformationBase(*logger);
				} catch (...) {
					cerr << "Cannot create a ManagementInformationBase object!" << endl;
					throw;
				}

			}

			virtual ~ScorefManagementTest() {
				// You can do clean-up work that doesn't throw exceptions here.
			}

			// If the constructor and destructor are not enough for setting up
			// and cleaning up each test, you can define the following methods:

			/**
			 * Initialization that'll be done before each test
			 *
			 * @param none
			 * @return none
			 */
			virtual void SetUp() {
			}

			/**
			 * Finalization that'll be done after each test
			 *
			 * @param none
			 * @return none
			 */
			virtual void TearDown() {
				delete logger;
			}

			// Objects declared here can be used by all tests in the test case for Foo.
			Logger* logger;
			Configuration* configuration;
			ManagementInformationBase* managementInformationBase;
	};

	/**
	 * Util class tests
	 */
	TEST_F(ScorefManagementTest, UtilTest) {
		try {
			testUtilResetBuffer(*logger);
			testUtilCopyBuffer(*logger);
			testUtilGetBinaryRepresentation(*logger);
			testUtilSetBit(*logger);
			testUtilUnsetBit(*logger);
			testUtilIsBitSet(*logger);
			testUtilParse8byteInteger(*logger);
			testUtilParse4byteInteger(*logger);
			testUtilParse2byteInteger(*logger);
			testUtilEncode8ByteInteger(*logger);
			testUtilEncode4ByteInteger(*logger);
			testUtilEncode2ByteInteger(*logger);
			testUtilEncodeBits(*logger);
			testUtilSplit(*logger);
			testUtilTrim(*logger);
			testUtilIsNumeric(*logger);
			testGetListOfFiles(*logger);
			testGetFileExtension(*logger);
		} catch (std::exception& e) {
			cerr << "std::exception.what() = '" << e.what() << "'" << endl;
		}
	}

	/**
	 * Configuration class tests
	 */
	TEST_F(ScorefManagementTest, ConfigurationTest) {
		try {
			testParseConfigurationFiles(*configuration, *managementInformationBase, *logger);
#ifdef PRIVATE
			testParseLine(*configuration, *logger);
			testParseParameterId(*configuration, *logger);
			testSetValue(*configuration, *logger);
#endif
			testGetConfigurationFileVector(*configuration, *logger);
			testAddConfigurationFile(*configuration, *logger);
			testGetServerPort(*configuration, *logger);
			testSetServerPort(*configuration, *logger);
			testGetWirelessStateUpdateInterval(*configuration, *logger);
			testSetWirelessStateUpdateInterval(*configuration, *logger);
			testGetLocationUpdateInterval(*configuration, *logger);
			testSetLocationUpdateInterval(*configuration, *logger);
			testConfiguration(*configuration, *managementInformationBase, *logger);
		} catch (std::exception& e) {
				cerr << "std::exception.what() = '" << e.what() << "'" << endl;
		}
	}
}

#endif /* MAIN_HPP_ */

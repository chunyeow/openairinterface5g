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
#include <gtest/gtest.h>

bool testParseConfigurationFilesNonexistentFile(Configuration& configuration, Logger& logger) {
	ManagementInformationBase mib(logger);

	return configuration.parseConfigurationFiles(mib);
}

bool testItsKeyManager(Configuration& configuration, ManagementInformationBase& mib, Logger& logger) {
	/**
	 * Verify the number of keys which should be 6 (3 for IHM, 3 for MGMT)
	 */
	if (mib.getItsKeyManager().getNumberOfKeys(ITS_KEY_TYPE_ALL) == 3)
		return true;

	return false;
}

void testCommunicationProfiles(Configuration& configuration, ManagementInformationBase& mib, Logger& logger) {
	/**
	 * Verify the number of communication profiles which should be 3
	 */
	EXPECT_EQ(3, mib.getCommunicationProfileManager().getProfileMap().size());
	/**
	 * Verify the contents of communication profiles, starting from 1st profile
	 */
	EXPECT_EQ(1, mib.getCommunicationProfileManager().getProfileMap()[1].id);
	EXPECT_EQ(0xC0, mib.getCommunicationProfileManager().getProfileMap()[1].transport);
	EXPECT_EQ(0x80, mib.getCommunicationProfileManager().getProfileMap()[1].network);
	EXPECT_EQ(0x80, mib.getCommunicationProfileManager().getProfileMap()[1].access);
	EXPECT_EQ(0x80, mib.getCommunicationProfileManager().getProfileMap()[1].channel);
	// Verify 2nd communication profile
	EXPECT_EQ(2, mib.getCommunicationProfileManager().getProfileMap()[2].id);
	EXPECT_EQ(0x20, mib.getCommunicationProfileManager().getProfileMap()[2].transport);
	EXPECT_EQ(0x40, mib.getCommunicationProfileManager().getProfileMap()[2].network);
	EXPECT_EQ(0x80, mib.getCommunicationProfileManager().getProfileMap()[2].access);
	EXPECT_EQ(0x80, mib.getCommunicationProfileManager().getProfileMap()[2].channel);
	// Verify 3rd and last communication profile
	EXPECT_EQ(0x0C, mib.getCommunicationProfileManager().getProfileMap()[3].transport);
	EXPECT_EQ(0x0C, mib.getCommunicationProfileManager().getProfileMap()[3].network);
	EXPECT_EQ(0x40, mib.getCommunicationProfileManager().getProfileMap()[3].access);
}

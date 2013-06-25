/*
 * test_util.cpp
 *
 *  Created on: Oct 3, 2012
 *      Author: demiray
 */

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

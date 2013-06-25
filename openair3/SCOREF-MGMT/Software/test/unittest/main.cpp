/*
 * main.cpp
 *
 *  Created on: Oct 2, 2012
 *      Author: demiray
 */

#include <gtest/gtest.h>

#include "main.hpp"

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

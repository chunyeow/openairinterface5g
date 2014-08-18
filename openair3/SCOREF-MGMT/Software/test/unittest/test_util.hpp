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

#include "../../src/util/mgmt_util.hpp"
#include <gtest/gtest.h>
#include <iostream>
using namespace std;

void testUtilResetBuffer(Logger& logger) {
	logger.info("Testing Util::resetBuffer() method");
	unsigned char array[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	Util::resetBuffer(array, 10);
	for (int i = 0; i < 10; ++i)
		EXPECT_EQ(0, array[i]);
}

void testUtilCopyBuffer(Logger& logger) {
	logger.info("Testing Util::copyBuffer() method");

	unsigned char arrayDestination[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	unsigned char arraySource[5] = { 'a', 'b', 'c', 'd', 'e' };
	unsigned char arrayExpected[10] = { 'a', 'b', 'c', 'd', 'e', '5', '6', '7', '8', '9' };

	/**
	 * Test for NULL arrays
	 */
	EXPECT_FALSE(Util::copyBuffer(arrayDestination, NULL, 5));
	EXPECT_FALSE(Util::copyBuffer(NULL, arraySource, 5));

	/**
	 * Test actual function
	 */
	Util::copyBuffer(arrayDestination, arraySource, 5);
	for (int i = 0; i < 10; ++i)
		EXPECT_EQ(arrayExpected[i], arrayDestination[i]);
}

void testUtilGetBinaryRepresentation(Logger& logger) {
	logger.info("Testing Util::getBinaryRepresentation() method");

	/**
	 * Check for 0xFF which should be '11111111'
	 */
	u_int8_t octet = 0xFF;
	string binaryRepresentation = Util::getBinaryRepresentation(octet);

	for (int i = 0; i < 8; ++i)
		EXPECT_EQ('1', binaryRepresentation.at(i));

	/**
	 * Check for 0xF0 which should be '11110000'
	 */
	octet = 0xF0;
	binaryRepresentation = Util::getBinaryRepresentation(octet);

	for (int i = 0; i < 4; ++i)
		EXPECT_EQ('1', binaryRepresentation.at(i));
	for (int i = 4; i < 8; ++i)
		EXPECT_EQ('0', binaryRepresentation.at(i));
}

void testUtilSetBit(Logger& logger) {
	logger.info("Testing Util::setBit() method");

	/**
	 * Start with 0b00000000 and test every bit
	 */
	u_int8_t octet = 0x00;

	for (int i = 0; i < 8; ++i) {
		Util::setBit(octet, i);
		EXPECT_TRUE(Util::isBitSet(octet, i));
	}
}

void testUtilUnsetBit(Logger& logger) {
	logger.info("Testing Util::unsetBit() method");

	/**
	 * Start with 0b11111111 and test every bit
	 */
	u_int8_t octet = 0xFF;

	for (int i = 0; i < 8; ++i) {
		Util::unsetBit(octet, i);
		EXPECT_FALSE(Util::isBitSet(octet, i));
	}
}

void testUtilIsBitSet(Logger& logger) {
	logger.info("Testing Util::isBitSet() method");

	/**
	 * Test with 0b00000000
	 */
	u_int8_t octet = 0x00;
	for (int i = 0; i < 8; ++i)
		EXPECT_FALSE(Util::isBitSet(octet, i));

	/**
	 * Test with 0b11111111
	 */
	octet = 0xFF;
	for (int i = 0; i < 8; ++i)
		EXPECT_TRUE(Util::isBitSet(octet, i));
}

void testUtilParse8byteInteger(Logger& logger) {
	logger.info("Testing Util::parse8byteInteger() method");

	unsigned char numerical[10] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99 };
	u_int64_t integer = 0x00;
	Util::parse8byteInteger(numerical, &integer);

	EXPECT_EQ(0x0011223344556677, integer);
}

void testUtilParse4byteInteger(Logger& logger) {
	logger.info("Testing Util::parse4byteInteger() method");

	unsigned char numerical[10] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99 };
	u_int32_t integer = 0x00;
	Util::parse4byteInteger(numerical, &integer);

	EXPECT_EQ(0x00112233, integer);
}

void testUtilParse2byteInteger(Logger& logger) {
	logger.info("Testing Util::parse2byteInteger() method");

	unsigned char numerical[10] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99 };
	u_int16_t integer = 0x00;
	Util::parse2byteInteger(numerical, &integer);

	EXPECT_EQ(0x0011, integer);
}

void testUtilEncode8ByteInteger(Logger& logger) {
	logger.info("Testing Util::encode8byteInteger() method");

	vector<unsigned char> numericalDestination(8);

	u_int64_t numericalSource = 0x0011223344556677;
	unsigned char numericalExpected[8] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };

	EXPECT_TRUE(Util::encode8byteInteger(numericalDestination, 0, numericalSource));
	for (int i = 0; i < 8; ++i)
		EXPECT_EQ(numericalExpected[i], numericalDestination.at(i));
}

void testUtilEncode4ByteInteger(Logger& logger) {
	logger.info("Testing Util::encode4byteInteger() method");

	vector<unsigned char> numericalDestination(4);

	u_int32_t numericalSource = 0x00112233;
	unsigned char numericalExpected[4] = { 0x00, 0x11, 0x22, 0x33 };

	EXPECT_TRUE(Util::encode4byteInteger(numericalDestination, 0, numericalSource));
	for (int i = 0; i < 4; ++i)
		EXPECT_EQ(numericalExpected[i], numericalDestination.at(i));
}

void testUtilEncode2ByteInteger(Logger& logger) {
	logger.info("Testing Util::encode2byteInteger() method");

	vector<unsigned char> numericalDestination(2);

	u_int16_t numericalSource = 0x0011;
	unsigned char numericalExpected[2] = { 0x00, 0x11 };

	EXPECT_TRUE(Util::encode2byteInteger(numericalDestination, 0, numericalSource));
	for (int i = 0; i < 2; ++i)
		EXPECT_EQ(numericalExpected[i], numericalDestination.at(i));
}

void testUtilEncodeBits(Logger& logger) {
	logger.info("Testing Util::encodeBits() method");

	/**
	 * Encode '0b1100' (0x0C) into '0b11001100' (0xCC) starting from 3th bit
	 * and see if you get '0b11110000' (0xF0)
	 */
	u_int8_t octetSource = 0x0C;
	u_int8_t octetDestination = 0xCC;
	u_int8_t octetExpected = 0xF0;

	EXPECT_TRUE(Util::encodeBits(octetDestination, 2, octetSource, 4));
	EXPECT_EQ(octetExpected, octetDestination);
}

void testUtilSplit(Logger& logger) {
	logger.info("Testing Util::split() method");

	/**
	 * Check number of tokens and their content
	 */
	string testInput = "this||is|a|test|string";
	vector<string> tokens = Util::split(testInput, '|');

	EXPECT_EQ(6, tokens.size());
	EXPECT_STREQ("this", tokens[0].c_str());
	EXPECT_STREQ("", tokens[1].c_str());
	EXPECT_STREQ("is", tokens[2].c_str());
	EXPECT_STREQ("a", tokens[3].c_str());
	EXPECT_STREQ("test", tokens[4].c_str());
	EXPECT_STREQ("string", tokens[5].c_str());
}

void testUtilTrim(Logger& logger) {
	logger.info("Testing Util::trim() method");

	/**
	 * Check if given characters can be trimmed off
	 */
	string testInput = "|test|";
	string testOutput = Util::trim(testInput, '|');
	EXPECT_STREQ("test", testOutput.c_str());

	testInput = "     test  ";
	testOutput = Util::trim(testInput, ' ');
	EXPECT_STREQ("test", testOutput.c_str());
}

void testUtilIsNumeric(Logger& logger) {
	logger.info("Testing Util::isNumeric() method");

	string numerical = "1234";
	string notNumerical = "ab34";

	EXPECT_TRUE(Util::isNumeric(numerical));
	EXPECT_FALSE(Util::isNumeric(notNumerical));
}

void testGetListOfFiles(Logger& logger) {
	logger.info("Testing Util::getListOfFiles() method");

	vector<string> listOfFiles = Util::getListOfFiles("../data/confFiles/");

	/**
	 * There are currently two configuration files in that directory
	 * and .svn file, this may change and updates will be necessary then,
	 * though
	 *
	 * We need to sort the vector first in order to ensure that the order
	 * of our check is correct
	 */
	std::sort(listOfFiles.begin(), listOfFiles.end());

	EXPECT_EQ(3, listOfFiles.size());
	EXPECT_STREQ(".svn", listOfFiles[0].c_str());
	EXPECT_STREQ("testConfFile1.conf", listOfFiles[1].c_str());
	EXPECT_STREQ("testConfFile2.conf", listOfFiles[2].c_str());
}

void testGetFileExtension(Logger& logger) {
	logger.info("Testing Util::getFileExtension() method");

	vector<string> fileNames;
	fileNames.push_back(".svn");
	fileNames.push_back("testFile.conf");
	fileNames.push_back("testFile.xml.txt");
	fileNames.push_back("testFile");

	EXPECT_STREQ(".svn", Util::getFileExtension(fileNames[0]).c_str());
	EXPECT_STREQ(".conf", Util::getFileExtension(fileNames[1]).c_str());
	EXPECT_STREQ(".txt", Util::getFileExtension(fileNames[2]).c_str());
	EXPECT_STREQ("", Util::getFileExtension(fileNames[3]).c_str());
}

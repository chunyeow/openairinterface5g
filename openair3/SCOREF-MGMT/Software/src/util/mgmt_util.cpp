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
 * \file util.cpp
 * \brief A container for utility methods for bit/byte processing and formatted printing
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/locale/boundary/facets.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
using namespace boost::posix_time;
using namespace boost::filesystem;

#include "mgmt_util.hpp"
#include <iostream>
#include <sstream>
using namespace std;

void Util::resetBuffer(unsigned char* buffer, const size_t bufferSize) {
	memset(buffer, 0x00, bufferSize);
}

bool Util::copyBuffer(unsigned char* destinationBuffer, const unsigned char* sourceBuffer, size_t copySize) {
	if (!destinationBuffer || !sourceBuffer)
		return false;

	memcpy(destinationBuffer, sourceBuffer, copySize);

	return true;
}

bool Util::printHexRepresentation(const unsigned char* buffer, unsigned long bufferSize, Logger& logger) {
	if (!buffer) {
		logger.warning("Incoming buffer is empty, won't write any hex data");
		return false;
	}

	stringstream ss;
	unsigned long octet_index = 0;

	logger.debug("");
	logger.debug("     |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f |");
	logger.debug("-----+-------------------------------------------------|");
	ss << " 000 |";
	for (octet_index = 0; octet_index < bufferSize; ++octet_index) {
		/*
		 * Print every single octet in hexadecimal form
		 */
		ss << " " << setfill('0') << setw(2) << hex << (int) buffer[octet_index];
		/*
		 * Align newline and pipes according to the octets in groups of 2
		 */
		if (octet_index != 0 && (octet_index + 1) % 16 == 0) {
			ss << " |";
			/**
			 * Print this line and reset std::stringstream afterwards
			 */
			logger.debug(ss.str());
			ss.str(string());
			ss.clear();

			ss << " " << setfill('0') << setw(3) << octet_index + 1 << " |";
		}
	}

	/*
	 * Append enough spaces and put final pipe
	 */
	unsigned char index;
	for (index = octet_index; index < 16; ++index)
		ss << "   ";
	ss << " |";

	ss << resetiosflags(ios_base::hex);
	logger.debug(ss.str());

	return true;
}

void Util::printBinaryRepresentation(unsigned char* message, u_int8_t octet, Logger& logger) {
	stringstream ss;

	ss << message << getBinaryRepresentation(octet) << endl;

	logger.debug(ss.str());
}

string Util::getBinaryRepresentation(u_int8_t octet) {
	u_int8_t index = 0;
	u_int8_t mask = 0x80;
	stringstream ss;

	for (index = 0; index < 8; ++index) {
		if (octet & mask)
			ss << "1";
		else
			ss << "0";

		mask /= 2;
	}

	return ss.str();
}

template <class T>
string Util::stringify(T numerical) {
	stringstream ss;
	ss << numerical;
	return ss.str();
}

bool Util::setBit(u_int8_t& octet, u_int8_t index) {
	u_int8_t mask = 0x80;

	/**
	 * Set relevant bit
	 */
	octet |= (mask >>= index);

	return true;
}

bool Util::unsetBit(u_int8_t& octet, u_int8_t index) {
	u_int8_t mask = 0x80;

	/**
	 * Unset relevant bit
	 */
	octet &= ~(mask >>= index);

	return true;
}

bool Util::isBitSet(u_int8_t octet, u_int8_t index) {
	u_int8_t mask = 0x80;

	/*
	 * Check relevant bit
	 */
	return octet &= (mask >>= index);
}

bool Util::parse8byteInteger(const unsigned char* buffer, u_int64_t* integer) {
	if (!buffer || !integer)
		return false;

	*integer = 0x00;
	*integer |= buffer[0] & 0xff; *integer <<= 8;
	*integer |= buffer[1] & 0xff; *integer <<= 8;
	*integer |= buffer[2] & 0xff; *integer <<= 8;
	*integer |= buffer[3] & 0xff; *integer <<= 8;
	*integer |= buffer[4] & 0xff; *integer <<= 8;
	*integer |= buffer[5] & 0xff; *integer <<= 8;
	*integer |= buffer[6] & 0xff; *integer <<= 8;
	*integer |= buffer[7] & 0xff;

	return true;
}

bool Util::parse4byteInteger(const unsigned char* buffer, u_int32_t* integer) {
	if (!buffer || !integer)
		return false;

	*integer = (buffer[0] << 24);
	*integer |= (buffer[1] << 16);
	*integer |= (buffer[2] << 8);
	*integer |= buffer[3] & 0xff;

	return true;
}

bool Util::parse2byteInteger(const unsigned char* buffer, u_int16_t* integer) {
	if (!buffer || !integer)
		return false;

	*integer = (buffer[0] << 8);
	*integer |= (buffer[1] & 0xff);

	return true;
}

float Util::parse4byteFloat(const vector<unsigned char>& floatBuffer) {
	return *((float*)floatBuffer.data());
}

bool Util::encode8byteInteger(vector<unsigned char>& buffer, u_int16_t bufferIndex, u_int64_t data) {
	if (buffer.capacity() < bufferIndex + sizeof(data))
		return false;

	u_int32_t dataHigherPart = ((data >> 32) & 0xffffffff);
	u_int32_t dataLowerPart = (data & 0xffffffff);

	return encode4byteInteger(buffer, bufferIndex, dataHigherPart) && \
		encode4byteInteger(buffer, bufferIndex + sizeof(u_int32_t), dataLowerPart);
}

bool Util::encode4byteInteger(vector<unsigned char>& buffer, u_int16_t bufferIndex, u_int32_t data) {
	if (buffer.capacity() < bufferIndex + sizeof(data))
		return false;

	buffer[bufferIndex] = ((data >> 24) & 0xff);
	buffer[bufferIndex + 1] = ((data >> 16) & 0xff);
	buffer[bufferIndex + 2] = ((data >> 8) & 0xff);
	buffer[bufferIndex + 3] = (data & 0xff);

	return true;
}

bool Util::encode2byteInteger(vector<unsigned char>& buffer, u_int16_t bufferIndex, u_int16_t data) {
	if (buffer.capacity() < bufferIndex + sizeof(data))
		return false;

	buffer[bufferIndex] = ((data >> 8) & 0xff);
	buffer[bufferIndex + 1] = (data & 0xff);

	return true;
}

bool Util::encodeBits(u_int8_t& octet, u_int8_t index, u_int8_t data, u_int8_t dataSize) {
	/**
	 * Do boundary check
	 */
	if (index + dataSize > 8)
		return false;

	/**
	 * Set/unset bits one by one using setBit() and unsetBit()
	 */
	u_int8_t sourceIndex = 7 - dataSize, destinationIndex = index;
	while (sourceIndex++ != 8) {
		if (Util::isBitSet(data, sourceIndex)) {
			setBit(octet, destinationIndex);
		} else {
			unsetBit(octet, destinationIndex);
		}

		destinationIndex++;
	}

	return true;
}

vector<string> Util::split(const string& input, char delimiter) {
	vector<string> elements;
	stringstream inputStream(input);
	string item;

	while (std::getline(inputStream, item, delimiter))
		elements.push_back(item);

	return elements;
}

string Util::trim(const string& str, char character) {
	string trimmed = str;

	trimmed.erase(remove(trimmed.begin(), trimmed.end(), character), trimmed.end());

	return trimmed;
}

bool Util::isNumeric(const string& str) {
	string::const_iterator it = str.begin();

	while (it != str.end() && std::isdigit(*it)) ++it;

	return !str.empty() && it == str.end();
}

string Util::getDateAndTime(bool withDelimiters) {
	stringstream dateAndTime;

	time_facet *facet = NULL;

	try {
		if (withDelimiters)
			facet = new time_facet("%Y/%m/%d-%T");
		else
			facet = new time_facet("%Y%m%d-%H%M%S");
	} catch (...) {
		return string("");
	}

	dateAndTime.imbue(locale(dateAndTime.getloc(), facet));
	dateAndTime << second_clock::local_time();

	return dateAndTime.str();
}

string Util::stringifyDateAndTime(u_int32_t timestamp, bool withDelimiters) {
	stringstream dateAndTime;

	time_facet *facet = NULL;

	try {
		if (withDelimiters)
			facet = new time_facet("%Y/%m/%d-%T");
		else
			facet = new time_facet("%Y%m%d-%H%M%S");
	} catch (...) {
		return string("");
	}

	dateAndTime.imbue(locale(dateAndTime.getloc(), facet));
	dateAndTime << boost::posix_time::from_time_t(timestamp);

	return dateAndTime.str();
}

vector<string> Util::getListOfFiles(const string& directory) {
	boost::filesystem::path directoryPath(directory);
	vector<string> fileList;

	/**
	 * First check if it exists and then if it really is a directory
	 */
	if (!exists(directory) && !is_directory(directoryPath))
		return fileList;

	directory_iterator endIterator;
	for (directory_iterator directoryIterator(directoryPath); directoryIterator != endIterator; ++directoryIterator)
		fileList.push_back(directoryIterator->path().filename().c_str());

	return fileList;
}

string Util::getFileExtension(const string& fileName) {
	/**
	 * If there is no dots then do not let this method to throw an
	 * exception, just return an empty string
	 */
	if (fileName.find('.') == string::npos)
		return "";

	return fileName.substr(fileName.rfind('.'), fileName.length() - fileName.rfind('.'));
}

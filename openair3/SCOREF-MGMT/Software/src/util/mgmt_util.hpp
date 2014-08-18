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

/*!
 * \file util.hpp
 * \brief A container for utility methods for bit/byte processing and formatted printing
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_UTIL_HPP_
#define MGMT_UTIL_HPP_

#include "mgmt_log.hpp"
#include <sys/types.h>
#include <iostream>
#include <cstring>
#include <iomanip>
#include <vector>
using namespace std;

/**
 * A container for utility methods for bit/byte processing and formatted printing
 */
class Util {

	public:
		/**
		 * Fills incoming buffer with 0x00es
		 *
		 * @param buffer Buffer to be reset
		 * @param bufferSize Size of the buffer
		 * @return none
		 */
		static void resetBuffer(unsigned char * buffer, const size_t bufferSize);
		/**
		 * Copies data between given two buffers
		 *
		 * @param destinationBuffer Buffer that data will be copied into
		 * @param sourceBuffer Buffer that data will be copied from
		 * @param copySize Amount of data to be copied
		 * @return true on success, false otherwise
		 */
		static bool copyBuffer(unsigned char* destinationBuffer, const unsigned char* sourceBuffer, size_t copySize);
		/**
		 * Prints hexadecimal representation of given buffer
		 *
		 * @param buffer Buffer that will be printed out
		 * @param bufferSize Size of the buffer
		 * @param logger Logger object reference
		 * @return true on success, false otherwise
		 */
		static bool printHexRepresentation(const unsigned char* buffer, unsigned long bufferSize, Logger& logger);
		/**
		 * Prints binary representation of given octet
		 *
		 * @param message Text message that'll be written before writing octet's content
		 * @param octet Octet to be printed out
		 * @param logger Logger object reference
		 * @return none
		 */
		static void printBinaryRepresentation(unsigned char* message, u_int8_t octet, Logger& logger);
		/**
		 * Returns binary representation of given octet in a string form
		 *
		 * @param octet Octet to be stringified
		 * @return std::string form of binary representation
		 */
		static string getBinaryRepresentation(u_int8_t octet);
		/**
		 * Returns string representation of given numeric value
		 *
		 * @param numeric Numerical value to be 'stringified'
		 * @return std:string representation of given numerical value
		 */
		template <class T>
		static string stringify(T numerical);
		/**
		 * Sets Nth bit of given octet
		 * `index' takes value from 0 to 7
		 *
		 * @param octet Pointer to the octet
		 * @param index Index that'll be set
		 * @return true on success, false otherwise
		 */
		static bool setBit(u_int8_t& octet, u_int8_t index);
		/**
		 * Unsets `index'th bit of given octet
		 * `index' takes value from 0 to 7
		 *
		 * @param octet Pointer to the octet
		 * @param index Index that'll be unset
		 * @return true on success, false otherwise
		 */
		static bool unsetBit(u_int8_t& octet, u_int8_t index);
		/**
		 * Checks if `index'th bit of given octet is set
		 * `index' takes value from 0 to 7
		 *
		 * @param octet Octet
		 * @param index Index that'll be checked
		 * @return true on success, false otherwise
		 */
		static bool isBitSet(u_int8_t octet, u_int8_t index);
		/**
		 * Parses 8-byte integer data from given buffer
		 *
		 * @param buffer Buffer that 8-byte integer will be parsed from
		 * @param integer Integer buffer that parsed data will be copied
		 * @return true on success, false otherwise
		 */
		static bool parse8byteInteger(const unsigned char* buffer, u_int64_t* integer);
		/**
		 * Parses 4-byte integer data from given buffer
		 *
		 * @param buffer Buffer that 4-byte integer will be parsed from
		 * @param integer Integer buffer that parsed data will be copied
		 * @return true on success, false otherwise
		 */
		static bool parse4byteInteger(const unsigned char* buffer, u_int32_t* integer);
		/**
		 * Parses 2-byte integer data from given buffer
		 *
		 * @param buffer Buffer that 2-byte integer will be parsed from
		 * @param integer Integer buffer that parsed data will be copied
		 * @return true on success, false otherwise
		 */
		static bool parse2byteInteger(const unsigned char* buffer, u_int16_t* integer);
		/**
		 * Parses 4-byte floating point data from given buffer
		 *
		 * @param buffer Buffer that 4-byte float will be parsed from
		 * @return Float value
		 */
		static float parse4byteFloat(const vector<unsigned char>& floatBuffer);
		/**
		 * Encodes given 8-byte integer data into buffer at given index
		 *
		 * @param buffer Vector that 8-byte integer will be encoded into
		 * @param bufferIndex Index that 8-byte integer will be encoded at
		 * @param data 8-byte integer data that will be encoded
		 * @return true on success, false otherwise
		 */
		static bool encode8byteInteger(vector<unsigned char>& buffer, u_int16_t bufferIndex, u_int64_t data);
		/**
		 * Encodes given 4-byte integer data into buffer at given index
		 *
		 * @param buffer Vector that 4-byte integer will be encoded into
		 * @param bufferIndex Index that 4-byte integer will be encoded at
		 * @param data 4-byte integer data that will be encoded
		 * @return true on success, false otherwise
		 */
		static bool encode4byteInteger(vector<unsigned char>& buffer, u_int16_t bufferIndex, u_int32_t data);
		/**
		 * Encodes given 2-byte integer data into buffer at given index
		 *
		 * @param buffer Vector that 2-byte integer will be encoded into
		 * @param bufferIndex Index that 2-byte integer will be encoded at
		 * @param data 2-byte integer data that will be encoded
		 * @return true on success, false otherwise
		 */
		static bool encode2byteInteger(vector<unsigned char>& buffer, u_int16_t bufferIndex, u_int16_t data);
		/**
		 * Encodes given bits starting from given index into given octet
		 *
		 * @param octet Octet that given bits will be encoded into
		 * @param index Index that this method starts encoding at (indexes are 0 to 7)
		 * @param data Data to be encoded
		 * @param dataSize Number of data bits to be encoded
		 * @return true on success, false otherwise
		 */
		static bool encodeBits(u_int8_t& octet, u_int8_t index, u_int8_t data, u_int8_t dataSize);
		/**
		 * Splits given string according to given delimiter and returns the
		 * string list as a vector
		 *
		 * @param input Input string that'll be delimited
		 * @param delimiter Delimiter character
		 * @return List of delimited sub-strings as a vector<string>
		 */
		static vector<string> split(const string& input, char delimiter);
		/**
		 * Removes non-printable characters from the end of a string, ie. trims it
		 *
		 * @param str std::string that is going to be trimmed
		 * @param character Character to be trimmed off
		 * @return trimmed string of type std::string
		 */
		static string trim(const string& str, char character);
		/**
		 * Checks if incoming string is numerical or not
		 *
		 * @param str std::string to be checked
		 * @return true if it's numerical, false otherwise
		 */
		static bool isNumeric(const string& str);
		/**
		 * Returns current date/time as string
		 * This is used for log file rotating and in log messages as a prefix
		 *
		 * @param withDelimiters true if asked with delimiters (like YYYY/mm/dd_HH:MM:SS), false otherwise
		 * @return String representation of current date and time information
		 */
		static string getDateAndTime(bool withDelimiters);
		/**
		 * Stringifies given timestamp (in milliseconds)
		 *
		 * @param withDelimiters true if asked with delimiters (like YYYY/mm/dd_HH:MM:SS), false otherwise
		 * @return String representation of current date and time information
		 */
		static string stringifyDateAndTime(u_int32_t timestamp, bool withDelimiters);
		/**
		 * Returns the list of files in given directory
		 *
		 * @param directory Directory that'll be traversed
		 * @return Vector of std::string carrying directory content
		 */
		static vector<string> getListOfFiles(const string& directory);
		/**
		 * Returns the file extension of given file name
		 *
		 * @param fileName File name of type std::string
		 * @return File extension of type std::string
		 */
		static string getFileExtension(const string& fileName);
};

#endif /* MGMT_UTIL_HPP_ */

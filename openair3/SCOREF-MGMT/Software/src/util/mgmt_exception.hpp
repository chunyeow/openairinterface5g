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
 * \file mgmt_exception.hpp
 * \brief A basic exception class for error handling
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_EXCEPTION_HPP_
#define MGMT_EXCEPTION_HPP_

#include "mgmt_log.hpp"
#include <exception>
#include <string>
#include <vector>
using namespace std;

/**
 * A basic exception class for error handling
 */
class Exception : public std::exception {
	public:
		/**
		 * Constructor for Exception class
		 *
		 * @param message Error/warning message
		 * @param logger Logger object reference
		 */
		Exception(const string& message, Logger& logger);
		/**
		 * Destructor for Exception class
		 */
		~Exception() throw();

	public:
		/**
		 * Prints messages from every point the trace has been updated
		 */
		void printStackTrace();
		/**
		 * Adds given message to stack trace to be printed later by printStackTrace()
		 *
		 * @param message A message that stack trace will be updated with
		 */
		void updateStackTrace(const string& message);

	private:
		/**
		 * String vector where stack trace messages are hold
		 */
		vector<string> stackTrace;
		/**
		 * Logger object reference
		 */
		Logger& logger;
};

#endif /* MGMT_EXCEPTION_HPP_ */

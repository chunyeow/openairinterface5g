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
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

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

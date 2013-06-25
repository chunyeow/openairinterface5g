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
 * \file mgmt_log.hpp
 * \brief A container for a basic logging utility
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#include <boost/date_time.hpp>
#include "mgmt_util.hpp"
#include "mgmt_log.hpp"
#include <iostream>
using namespace std;

Logger::Logger(const string& logFileName, Logger::LOG_LEVEL logLevel, Logger::LOG_OUTPUT_CHANNEL logOutputChannel) {
	this->logFileName = logFileName;
	this->logLevel = logLevel;
	this->logOutputChannel = logOutputChannel;
	/**
	 * New name for the log file
	 */
	string newLogFilePath = "";
	/**
	 * Define string representations of log levels
	 */
	logLevelString.insert(logLevelString.end(), std::make_pair(TRACE, "TRACE"));
	logLevelString.insert(logLevelString.end(), std::make_pair(DEBUG, "DEBUG"));
	logLevelString.insert(logLevelString.end(), std::make_pair(INFO, "INFO"));
	logLevelString.insert(logLevelString.end(), std::make_pair(WARNING, "WARNING"));
	logLevelString.insert(logLevelString.end(), std::make_pair(ERROR, "ERROR"));

	/**
	 * Open log file stream, if the file already exists then rename
	 * it appending the date and create a new one
	 */
	if (logOutputChannel == Logger::FILE || logOutputChannel == Logger::BOTH) {
		/**
		 * If we have renamed log file because it exists
		 */
		bool renamed = false;

		logFilePath = boost::filesystem::path(logFileName);

		if (boost::filesystem::exists(logFilePath)) {
			/**
			 * Get the current date/time as string and prepend log file name with it
			 */
			newLogFilePath = logFilePath.string() + Util::getDateAndTime(false);
			boost::filesystem::rename(logFilePath, newLogFilePath);
			renamed = true;
		}

		/**
		 * Open log file stream
		 */
		logFileStream.open(logFileName.c_str(), ios_base::out);
		if (!logFileStream.is_open()){
			cerr << "Cannot open log file!" << endl;
		}

		if (renamed)
			this->info("A log file with the same name exists, created one with name '" + newLogFilePath + "'");
	}
}

Logger::~Logger() {
	logFileStream.flush();
	logFileStream.close();
}

void Logger::trace(const string& message, bool logFormatting) {
	log(message, TRACE, logFormatting);
}

void Logger::debug(const string& message, bool logFormatting) {
	log(message, DEBUG, logFormatting);
}

void Logger::info(const string& message, bool logFormatting) {
	log(message, INFO, logFormatting);
}

void Logger::warning(const string& message, bool logFormatting) {
	log(message, WARNING, logFormatting);
}

void Logger::error(const string& message, bool logFormatting) {
	log(message, ERROR, logFormatting);
}

void Logger::log(const string& message, LOG_LEVEL level, bool logFormatting) {
	/**
	 * Quit if we are asked not to log at all
	 */
	if (logOutputChannel == Logger::NONE)
		return;

	/**
	 * Quit if requested logging level is not allowed in the configuration
	 */
	if (logLevel > level)
		return;
 
	/**
	 * Ensure that there is only one here at any given time
	 */
	boost::lock_guard<boost::mutex> lock(logMutex);

	/**
	 * Write to log file first if we are asked to do
	 */
	if (logOutputChannel == Logger::FILE || logOutputChannel == Logger::BOTH) {
		if (logFormatting)
			logFileStream << setw(15) << Util::getDateAndTime(true) << setw(7) << logLevelString[level] << ": ";
		logFileStream << message;
		if (logFormatting)
			logFileStream << endl;
	}

	/**
	 * And then to standard output
	 */
	if (logOutputChannel == Logger::STDOUT || logOutputChannel == Logger::BOTH) {
		if (logFormatting)
			cout << setw(15) << Util::getDateAndTime(true) << setw(7) << logLevelString[level] << ": ";
		cout << message;
		if (logFormatting)
			cout << endl;
	}
}

void Logger::setLogLevel(Logger::LOG_LEVEL logLevel) {
	this->logLevel = logLevel;
}

string Logger::getCurrentLogLevelName() {
	return logLevelString[logLevel];
}

void Logger::setLogOutputChannel(Logger::LOG_OUTPUT_CHANNEL logOutputChannel) {
	this->logOutputChannel = logOutputChannel;
}

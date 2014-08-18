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

#ifndef MGMT_LOG_HPP_
#define MGMT_LOG_HPP_

#include <boost/thread/mutex.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <string>
#include <map>
using namespace std;

/**
 * A container for a basic logging utility
 */
class Logger {
	public:
		enum LOG_LEVEL {
			TRACE = 0,
			DEBUG = 1,
			INFO = 2,
			WARNING = 3,
			ERROR = 4
		};
		enum LOG_OUTPUT_CHANNEL {
			FILE = 0,
			STDOUT = 1,
			BOTH = 2,
			NONE = 3
		};

	public:
		/**
		 * Constructor for Logger class
		 *
		 * @param logFileName Log file name
		 * @param logLevel Initial log level (default is INFO)
		 */
		Logger(const string& logFileName, Logger::LOG_LEVEL logLevel = Logger::INFO, Logger::LOG_OUTPUT_CHANNEL = Logger::BOTH);
		/**
		 * Destructor for Logger class
		 */
		~Logger();

	public:
		/**
		 * Prints given log message at TRACE level
		 * This is used for finer-grained log messages than DEBUG
		 *
		 * @param message Log message
		 * @param logFormatting Determines if date, log level, etc. will be printed as well
		 */
		void trace(const string& message, bool logFormatting = true);
		/**
		 * Prints given log message at DEBUG level
		 *
		 * @param message Log message
		 * @param logFormatting Determines if date, log level, etc. will be printed as well
		 */
		void debug(const string& message, bool logFormatting = true);
		/**
		 * Prints given log message at INFO level
		 *
		 * @param message Log message
		 * @param logFormatting Determines if date, log level, etc. will be printed as well
		 */
		void info(const string& message, bool logFormatting = true);
		/**
		 * Prints given log message at WARNING level
		 *
		 * @param message Log message
		 * @param logFormatting Determines if date, log level, etc. will be printed as well
		 */
		void warning(const string& message, bool logFormatting = true);
		/**
		 * Prints given log message at ERROR level
		 *
		 * @param message Log message
		 * @param logFormatting Determines if date, log level, etc. will be printed as well
		 */
		void error(const string& message, bool logFormatting = true);
		/**
		 * Prints given log message at given level
		 *
		 * @param message Log message
		 * @param level Log level
		 * @param logFormatting Determines if date, log level, etc. will be printed as well
		 * @param newline Determines if a '\n' character will be appended to log messages
		 */
		void log(const string& message, Logger::LOG_LEVEL level, bool logFormatting = true);
		/**
		 * Updates configured log level with incoming information
		 *
		 * @param logLevel New log level of type Logger::LOG_LEVEL
		 */
		void setLogLevel(Logger::LOG_LEVEL logLevel);
		/**
		 * Returns the string name of current log level integer
		 */
		string getCurrentLogLevelName();
		/**
		 * Updates configured log output channel with given value
		 *
		 * @param logOutputChannel New log output channel of type Logger::LOG_OUTPUT_CHANNEL
		 */
		void setLogOutputChannel(Logger::LOG_OUTPUT_CHANNEL logOutputChannel);

	private:
		/**
		 * Log file name
		 */
		string logFileName;
		/**
		 * Log file path of type boost::filesystem::path
		 */
		boost::filesystem::path logFilePath;
		/**
		 * Mutex to ensure that log() is called by only one at any given time
		 */
		boost::mutex logMutex;
		/**
		 * Log file stream of type ofstream
		 */
		ofstream logFileStream;
		/**
		 * Configured log level
		 */
		Logger::LOG_LEVEL logLevel;
		/**
		 * Configured log output channel
		 */
		Logger::LOG_OUTPUT_CHANNEL logOutputChannel;
		/**
		 * Log level string map
		 */
		map<LOG_LEVEL, string> logLevelString;
};

#endif /* MGMT_LOG_HPP_ */

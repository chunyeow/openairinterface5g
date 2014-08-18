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
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

/*!
 * \file mgmt_inquiry_thread.hpp
 * \brief A thread worker function to ask repetitive questions to relevant modules to update MIB
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_INQUIRY_THREAD_HPP_
#define MGMT_INQUIRY_THREAD_HPP_

#include "interface/mgmt_packet_sender.hpp"
#include "mgmt_information_base.hpp"
#include "util/mgmt_log.hpp"

/**
 * A thread worker function to ask repetitive questions to relevant modules to update MIB
 */
class InquiryThread {
	public:
		/**
		 * Constructor for InquiryThread class
		 *
		 * @param packetSender Packet sending functionality of ManagementServer class
		 * @param wirelessStateUpdateInterval Wireless State Update interval in seconds
		 * @param logger Logger object reference
		 */
		InquiryThread(IManagementPacketSender* packetSender, u_int8_t wirelessStateUpdateInterval, Logger& logger);
		/**
		 * Destructor for InquiryThread class
		 */
		virtual ~InquiryThread();

	public:
		/**
		 * () operator overload to pass this method to boost::thread
		 *
		 * @param none
		 * @return none
		 */
		void operator()();
		/**
		 * Sends request for a Wireless State Response message
		 * Incoming message will be handled and MIB will be updated
		 * accordingly by GeonetMessageHandler class
		 *
		 * @param none
		 * @return true on success, false otherwise
		 */
		bool requestWirelessStateUpdate();

	private:
		/**
		 * IManagementPacketSender reference to use ManagementServer's functionality
		 */
		IManagementPacketSender* packetSender;
		/**
		 * Wireless State Update interval in seconds
		 */
		u_int8_t wirelessStateUpdateInterval;
		/**
		 * Logger object reference
		 */
		Logger& logger;
};

#endif /* MGMT_INQUIRY_THREAD_HPP_ */

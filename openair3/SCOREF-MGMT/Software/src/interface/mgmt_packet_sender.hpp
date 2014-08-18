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
 * \file mgmt_packet_sender.hpp
 * \brief This is an abstract base class implemented by ManagementServer class
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_PACKET_SENDER_H_
#define MGMT_PACKET_SENDER_H_

/**
 * This is an abstract base class implemented by ManagementServer class
 * to provide functionality of status query packet sending for InquiryThread
 */
class IManagementPacketSender {
	public:
		/**
		 * Virtual destructor for this abstract base class
		 */
//		virtual ~IManagementPacketSender() = 0;

	public:
		/**
		 * Send a Wireless State Request packet to GN client
		 *
		 * @param none
		 * @return true if success, false otherwise
		 */
		virtual bool sendWirelessStateRequest() = 0;
};

#endif /* MGMT_PACKET_SENDER_H_ */

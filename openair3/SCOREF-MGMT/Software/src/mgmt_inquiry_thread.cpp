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
 * \file mgmt_inquiry_thread.cpp
 * \brief A thread worker function to ask repetitive questions to relevant modules to update MIB
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#include "mgmt_inquiry_thread.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/date_time.hpp>
#include "util/mgmt_util.hpp"
#include <boost/thread.hpp>
#include <iostream>
using namespace std;

InquiryThread::InquiryThread(IManagementPacketSender* packetSender, u_int8_t wirelessStateUpdateInterval, Logger& logger)
	: packetSender(packetSender), logger(logger) {
	this->wirelessStateUpdateInterval = wirelessStateUpdateInterval;
}

InquiryThread::~InquiryThread() {
}

void InquiryThread::operator()() {
	/**
	 * Send a Wireless State Request every `wirelessStateUpdateInterval' second(s)
	 */
	boost::posix_time::seconds wait(wirelessStateUpdateInterval);

	while (true) {
		logger.info("Will wait for " + boost::lexical_cast<string>((int)wirelessStateUpdateInterval) + " second(s) to send a Wireless State Request");
		boost::this_thread::sleep(wait);

		if (requestWirelessStateUpdate())
			logger.info("A Wireless State Request packet sent to GN");
		else
			logger.warning("Cannot send a Wireless State Request packet to GN!");
	}
}

bool InquiryThread::requestWirelessStateUpdate() {
	/**
	 * Use ManagementServerFunctionality to send a Wireless State Update to GN
	 */
	return packetSender->sendWirelessStateRequest();
}

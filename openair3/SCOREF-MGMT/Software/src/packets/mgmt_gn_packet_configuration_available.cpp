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
 * \file mgmt_gn_packet_configuration_available.cpp
 * \brief A container for Configuration Available Event
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#include "mgmt_gn_packet_configuration_available.hpp"
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sstream>
using namespace std;

GeonetConfigurationAvailableEventPacket::GeonetConfigurationAvailableEventPacket(ManagementInformationBase& mib, Logger& logger)
	: GeonetPacket(false, true, 0x00, 0x00, MGMT_GN_EVENT_CONF_UPDATE_AVAILABLE, logger), mib(mib) {
}

GeonetConfigurationAvailableEventPacket::~GeonetConfigurationAvailableEventPacket() {
}

bool GeonetConfigurationAvailableEventPacket::serialize(vector<unsigned char>& buffer) {
	if (buffer.size() < sizeof(ConfigureAvailableMessage)) {
		logger.error("Incoming buffer' size is not sufficient!");
		return false;
	}

	// Get some help from superclass to place header into given buffer
	if (!GeonetPacket::serialize(buffer)) {
		logger.error("Cannot serialise header into given buffer!");
		return false;
	}

	u_int8_t bodyIndex = sizeof(MessageHeader);
	u_int16_t keyCount = mib.getItsKeyManager().getNumberOfKeys();

	// encode `reserved' field
	buffer[bodyIndex] = 0x00;
	buffer[bodyIndex + 1] = 0x00;
	// encode `key count' field
	buffer[bodyIndex + 2] = ((keyCount >> 8) & 0xff);
	buffer[bodyIndex + 3] = (keyCount & 0xff);

	/**
	 * Resize buffer to the number of bytes we've written into it
	 */
	buffer.resize(sizeof(ConfigureAvailableMessage));

	return true;
}

string GeonetConfigurationAvailableEventPacket::toString() const {
	return string("Key count: " + boost::lexical_cast<string>(mib.getItsKeyManager().getNumberOfKeys()));
}


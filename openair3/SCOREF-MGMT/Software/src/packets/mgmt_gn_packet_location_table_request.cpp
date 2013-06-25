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
 * \file mgmt_gn_packet_location_table_request.cpp
 * \brief A container for Location Table Request Event packet
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#include "mgmt_gn_packet_location_table_request.hpp"
#include "../util/mgmt_util.hpp"
#include <iostream>
#include <sstream>
using namespace std;

GeonetLocationTableRequestEventPacket::GeonetLocationTableRequestEventPacket(GnAddress address, Logger& logger)
	: GeonetPacket(false, true, 0x00, 0x00, MGMT_GN_EVENT_LOCATION_TABLE_REQUEST, logger) {
	this->gnAddress = address;
}

bool GeonetLocationTableRequestEventPacket::serialize(vector<unsigned char>& buffer) const {
	if (buffer.size() < sizeof(LocationTableRequest))
		return false;

	// Encode header first
	if (!GeonetPacket::serialize(buffer)) {
		logger.error("Cannot serialise header into given buffer!");
		return false;
	}

	// Then the GN address follows
	if (!Util::encode8byteInteger(buffer, sizeof(MessageHeader), this->gnAddress)) {
		logger.error("Cannot serialise GN address into given buffer!");
		return false;
	}

	buffer.resize(sizeof(LocationTableRequest));

	return true;
}

string GeonetLocationTableRequestEventPacket::toString() const {
	stringstream ss;

	ss << GeonetPacket::toString();
	ss << "GN Address: " << gnAddress << endl;

	return ss.str();
}

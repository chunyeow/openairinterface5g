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
 * \file mgmt_gn_packet_set_configuration.cpp
 * \brief A container for Set Configuration Event packet
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#include "mgmt_gn_packet_set_configuration.hpp"
#include <boost/lexical_cast.hpp>
#include <sstream>
using namespace std;

GeonetSetConfigurationEventPacket::GeonetSetConfigurationEventPacket(ManagementInformationBase& mib, Logger& logger, ItsKeyID itsKeyID) :
	GeonetPacket(false, true, 0x00, 0x00,
	    ((itsKeyID >= MGMT_GN_ITSKEY_SET_NET) ? MGMT_GN_EVENT_CONF_BULK_RESPONSE : MGMT_GN_EVENT_CONF_CONT_RESPONSE), logger), mib(
	    mib) {
	isBulk = (itsKeyID >= MGMT_GN_ITSKEY_SET_NET) ? true : false;
	requestedItsKey = itsKeyID;

	switch (itsKeyID) {
		case MGMT_GN_ITSKEY_SET_FAC:
			requestedItsKeyType = ITS_KEY_TYPE_FAC;
			break;

		case MGMT_GN_ITSKEY_SET_NET:
			requestedItsKeyType = ITS_KEY_TYPE_NET;
			break;

		case MGMT_GN_ITSKEY_ALL:
		default:
			requestedItsKeyType = ITS_KEY_TYPE_ALL;
			break;
	}
}

GeonetSetConfigurationEventPacket::~GeonetSetConfigurationEventPacket() {
}

bool GeonetSetConfigurationEventPacket::serialize(vector<unsigned char>& buffer) const {
	// Following pointers are used to clarify the code
	unsigned char* packetBody = buffer.data() + sizeof(MessageHeader);
	unsigned char* packetPayload = packetBody + sizeof(u_int32_t); // `reserved' and `key count' fields are 4-byte in size

	// Encode header first...
	GeonetPacket::serialize(buffer);

	// ...and then the packet-specific fields
	if (isBulk) {
		unsigned int configurationItemIndex = 0;
		/*
		 * We may be asked all the configuration items but just those for
		 * FAC or NET layers
		 */
		u_int16_t configurationItemCount = 0;
		/**
		 * Fetch relevant ITS key subset...
		 */
		map<ItsKeyID, ItsKeyValue> keyset = mib.getItsKeyManager().getSubset(requestedItsKeyType);
		map<ItsKeyID, ItsKeyValue>::const_iterator iterator = keyset.begin();

		logger.info(boost::lexical_cast<string>(keyset.size()) + " key(s) found for requested ITS key type");

		while (iterator != keyset.end()) {
			ConfigurationItem confItem = buildConfigurationItem(iterator->first);
			encodeConfigurationItem(packetPayload + configurationItemIndex * sizeof(ConfigurationItem), &confItem);

			++configurationItemCount;
			++configurationItemIndex;
			++iterator;
		}

		// `reserved' field
		packetBody[0] = 0x00;
		packetBody[1] = 0x00;
		// `key count' field
		packetBody[2] = ((configurationItemCount & 0xff00) >> 8);
		packetBody[3] = (configurationItemCount & 0xff);

		buffer.resize(sizeof(BulkConfigurationResponse) + configurationItemCount * sizeof(ConfigurationItem));

		return true;
	} else {
		// `conf id' field
		packetBody[0] = ((requestedItsKey & 0xff00) >> 8);
		packetBody[1] = (requestedItsKey & 0xff);
		// `length' field
		packetBody[2] = ((mib.getLength(requestedItsKey) & 0xff00) >> 8);
		packetBody[3] = (mib.getLength(requestedItsKey) & 0xff);
		// `conf value' field
		u_int32_t configurationValue = mib.getItsKeyValue(requestedItsKey).intValue;
		packetBody[4] = (configurationValue >> 24) & 0xff;
		packetBody[5] = (configurationValue >> 16) & 0xff;
		packetBody[6] = (configurationValue >> 8) & 0xff;
		packetBody[7] = (configurationValue & 0xff);

		buffer.resize(sizeof(ContinuousConfigurationResponse));

		return true;
	}
}

bool GeonetSetConfigurationEventPacket::encodeConfigurationItem(unsigned char* buffer,
    const ConfigurationItem* configurationItem) {
	if (!buffer)
		return false;

	// Encode `configuration id' field
	buffer[0] = (configurationItem->configurationId >> 8);
	buffer[1] = configurationItem->configurationId & 0xff;
	// Encode `length' field
	buffer[2] = (configurationItem->length >> 8);
	buffer[3] = configurationItem->length & 0xff;
	// Encode `configuration value' field
	buffer[4] = (configurationItem->configurationValue >> 24) & 0xff;
	buffer[5] = (configurationItem->configurationValue >> 16) & 0xff;
	buffer[6] = (configurationItem->configurationValue >> 8) & 0xff;
	buffer[7] = configurationItem->configurationValue & 0xff;

	return true;
}

ConfigurationItem GeonetSetConfigurationEventPacket::buildConfigurationItem(ItsKeyID itsKey) const {
	ConfigurationItem confItem;

	confItem.configurationId = itsKey;
	/**
	 * Convert byte length into DWORD length
	 */
	confItem.length = mib.getLength(itsKey) / 4;
	confItem.configurationValue = mib.getItsKeyValue(itsKey).intValue;

	return confItem;
}

string GeonetSetConfigurationEventPacket::toString() const {
	stringstream ss;

	if (isBulk) {
		ss << "Key count: " << ((isBulk) ? mib.getItsKeyManager().getNumberOfKeys(requestedItsKeyType) : 1) << endl;
	} else {
		ss << "Configuration ID: " << requestedItsKey << endl << "Length (in bytes): " << mib.getLength(requestedItsKey) << endl
		    << "Value: " << mib.getItsKeyValue(requestedItsKey).intValue << endl;
	}

	return ss.str();
}

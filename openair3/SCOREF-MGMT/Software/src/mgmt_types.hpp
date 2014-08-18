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
 * \file mgmt_types.h
 * \brief Definitions of common data types used in SCOREF Management Module
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_TYPES_HPP_
#define MGMT_TYPES_HPP_

#include "util/mgmt_util.hpp"
#include <sys/types.h>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

/**
 * Configuration Item IDs
 */
enum ItsKeyID {
	/**
	 * Common ITS keys
	 */
	MGMT_GN_ALL_ITSKEY_ID_STATION_TYPE 						= 0,
	MGMT_GN_ALL_ITSKEY_ID_STATION_SUBTYPE					= 1,
	/**
	 * Network ITS keys
	 */
	MGMT_GN_NET_ITSKEY_ID_GN_LOCAL_ADD_CONF_METHOD			= 1000,
	MGMT_GN_NET_ITSKEY_ID_DEFAULT_HOP_LIMIT					= 1001,
	MGMT_GN_NET_ITSKEY_ID_GN_MAX_PKT_LIFETIME				= 1002,
	MGMT_GN_NET_ITSKEY_ID_GN_MIN_PKT_REPETITION_INTERVAL	= 1003,
	MGMT_GN_NET_ITSKEY_ID_GN_GEO_BCAST_FORWARDING_ALG		= 1010,
	MGMT_GN_NET_ITSKEY_ID_GN_GEO_UCAST_FORWARDING_ALG		= 1011,
	MGMT_GN_NET_ITSKEY_ID_GN_TRAFFIC_CLASS_RELEVANCE		= 1020,
	MGMT_GN_NET_ITSKEY_ID_GN_TRAFFIC_CLASS_RELIABILITY		= 1021,
	MGMT_GN_NET_ITSKEY_ID_GN_TRAFFIC_CLASS_LATENCY			= 1022,
	MGMT_GN_NET_ITSKEY_ID_GN_CBF_MIN_TTS					= 1030,
	MGMT_GN_NET_ITSKEY_ID_GN_CBF_MAX_TTS					= 1031,
	MGMT_GN_NET_ITSKEY_ID_GN_MAX_COMM_RANGE					= 1040,
	MGMT_GN_NET_ITSKEY_ID_GN_DEF_TX_POWER					= 1050,
	MGMT_GN_NET_ITSKEY_ID_GN_DEF_BITRATE					= 1051,
	MGMT_GN_NET_ITSKEY_ID_GN_DEF_CHANNEL					= 1052,
	MGMT_GN_NET_ITSKEY_ID_GN_DEF_PRIORITY					= 1053,
	MGMT_GN_NET_ITSKEY_ID_GN_DEF_CHANNEL_BW					= 1054,
	MGMT_GN_NET_ITSKEY_ID_SEC_ALLOW_UNSECURE				= 2000,
	MGMT_GN_NET_ITSKEY_ID_SEC_END_2_END						= 2001,
	MGMT_GN_NET_ITSKEY_ID_SEC_PSEUDONYM						= 2002,
	/**
	 * FACilities ITS keys
	 */
	MGMT_GN_FAC_ITSKEY_ID_VEHICLE_WIDTH						= 2,
	MGMT_GN_FAC_ITSKEY_ID_VEHICLE_LENGTH					= 3,
	MGMT_GN_FAC_ITSKEY_ID_CAM_BTP_PORT						= 3010,
	MGMT_GN_FAC_ITSKEY_ID_DENM_BTP_PORT						= 3011,
	MGMT_GN_FAC_ITSKEY_ID_LDM_GARBAGE_COLLECTION_INTERVAL	= 3020,
	/**
	 * Configuration set ITS keys
	 */
	MGMT_GN_ITSKEY_SET_NET									= 0xaaaa,
	MGMT_GN_ITSKEY_SET_FAC									= 0xbbbb,
	MGMT_GN_ITSKEY_ALL										= 0xffff
};

/**
 * Message & Event Types
 */
enum EventType {
	/**
	 * Any
	 */
	MGMT_EVENT_ANY = 0x000,
	/**
	 * Location
	 */
	MGMT_GN_EVENT_LOCATION_UPDATE = 0x100,
	MGMT_FAC_EVENT_LOCATION_UPDATE = 0x110,
	MGMT_GN_EVENT_LOCATION_TABLE_REQUEST = 0x101,
	MGMT_FAC_EVENT_LOCATION_TABLE_REQUEST = 0x103,
	MGMT_GN_EVENT_LOCATION_TABLE_RESPONSE = 0x102,
	MGMT_FAC_EVENT_LOCATION_TABLE_RESPONSE = 0x104,
	/**
	 * Configuration
	 */
	MGMT_GN_EVENT_CONF_UPDATE_AVAILABLE = 0x300,
	MGMT_GN_EVENT_CONF_REQUEST = 0x301,
	MGMT_FAC_EVENT_CONF_REQUEST = 0x311,
	MGMT_GN_EVENT_CONF_CONT_RESPONSE = 0x302,
	MGMT_FAC_EVENT_CONF_CONT_RESPONSE = 0x312,
	MGMT_GN_EVENT_CONF_BULK_RESPONSE = 0x303,
	MGMT_FAC_EVENT_CONF_BULK_RESPONSE = 0x313,
	MGMT_FAC_EVENT_CONF_NOTIFICATION = 0x314,
	MGMT_GN_EVENT_CONF_COMM_PROFILE_REQUEST = 0x304,
	MGMT_FAC_EVENT_CONF_COMM_PROFILE_REQUEST = 0x315,
	MGMT_GN_EVENT_CONF_COMM_PROFILE_RESPONSE = 0x305,
	MGMT_FAC_EVENT_CONF_COMM_PROFILE_RESPONSE = 0x316,
	MGMT_FAC_EVENT_CONF_COMM_PROFILE_SELECTION_REQUEST = 0x317,
	MGMT_FAC_EVENT_CONF_COMM_PROFILE_SELECTION_RESPONSE = 0x318,
	/**
	 * State
	 */
	MGMT_GN_EVENT_STATE_WIRELESS_STATE_REQUEST = 0x402,
	MGMT_GN_EVENT_STATE_WIRELESS_STATE_RESPONSE = 0x403,
	MGMT_LTE_EVENT_STATE_WIRELESS_STATE_RESPONSE = 0x423,
	MGMT_GN_EVENT_STATE_NETWORK_STATE = 0x404
};

/**
 * Configuration Message / Transmission Type
 */
enum ConfigurationTransmissionMode {
	C2X_MGMT_GN_CONF_TX_CONT = 0,
	C2X_MGMT_GN_CONF_TX_BULK = 1
};

/**
 * Message Header
 */
struct MessageHeader {
	u_int8_t version;
	u_int8_t priority;
	u_int8_t eventType;
	u_int8_t eventSubtype;

	/**
	 * Returns if this packet contains extended/ventor specific data
	 */
	bool isExtended() const {
		return Util::isBitSet(version, 0);
	}
	/**
	 * Returns if this packet contains valid data
	 */
	bool isValid() const {
		return Util::isBitSet(version, 1);
	}
	/**
	 * Returns the last 4 bits of version field (which is actual version information)
	 */
	u_int8_t getVersion() const {
		return version & 0x0F;
	}
	/**
	 * Returns the first 3 bits of priority field (which is actual priority information)
	 */
	u_int8_t getPriority() const {
		return priority >> 5;
	}
} __attribute__((packed));

/**
 * Location Information
 */
struct LocationInformation {
	u_int32_t timestamp; /* Time in milliseconds */
	u_int32_t latitude;  /* Latitude in 1/10 micro-degree */
	u_int32_t longitude; /* Longitude in 1/10 micro-degree */
	u_int16_t speed;	 /* Speed in signed units of 1 meter */
	u_int16_t heading;
	u_int16_t altitude;
	u_int16_t acceleration; /* TAcc, PodAcc, SAcc, Hacc, AltAcc */

	/**
	 * Initialize everything to zero
	 */
	LocationInformation() {
		timestamp = latitude = longitude = speed = heading = altitude = acceleration = 0;
	}

	string toString() const {
		stringstream ss;

		ss << "LocationInformation[timestamp:" << timestamp << ", latitude:" << latitude
			<< ", longitude:" << longitude << ", speed:" << speed << ", heading:" << heading
			<< ", altitude:" << altitude << ", acceleration:" << acceleration;

		return ss.str();
	}
} __attribute__((packed));

/**
 * Update Location Event
 */
struct LocationUpdateMessage {
	MessageHeader header;

	LocationInformation location;
} __attribute__((packed));

typedef u_int64_t GnAddress;

/**
 * Query Location Table Event Message
 */
struct LocationTableRequest {
	MessageHeader header;

	GnAddress gnAddress;
} __attribute__((packed));

/**
 * Location Table Response Item
 */
struct LocationTableItem {
	GnAddress gnAddress;
	u_int32_t timestamp; /* Time in milliseconds */
	u_int32_t latitude;  /* Latitude in 1/10 micro-degree */
	u_int32_t longitude; /* Longitude in 1/10 micro-degree */
	u_int16_t speed;	 /* Speed in signed units of 1 meter */
	u_int16_t heading;
	u_int16_t altitude;
	u_int16_t acceleration;
	u_int16_t sequenceNumber;
	u_int8_t lpvFlags;
	u_int8_t reserved;

	string toString() const {
		stringstream ss;

		/**
		 * Print GN address in hex (as Andrea told) and the rest in decimal notation
		 */
		ss << "[gnAddr:" << hex << showbase << gnAddress << ", ";
		ss << resetiosflags(ios_base::hex) << resetiosflags(ios_base::showbase);
		ss << "ts:" << timestamp
			<< " lat.:" << latitude
			<< " long.:" << longitude
			<< " speed:" << speed
			<< " heading:" << heading
			<< " alt.:" << altitude
			<< " accel.:" << acceleration
			<< " sn.:" << sequenceNumber
			<< " lpv:" << (int)lpvFlags
			<< " res.:" << (int)reserved << "]";

		return ss.str();
	}
} __attribute__((packed));

/**
 * Reply Location Event
 */
struct LocationTableResponse {
	MessageHeader header;

	u_int16_t lpvCount;
	u_int8_t networkFlags;
	u_int8_t reserved;

	// Location table items will follow
} __attribute__((packed));

/**
 * Wireless State Request Message
 */
struct WirelessStateRequestMessage {
	MessageHeader header;
} __attribute__((packed));

/**
 * Wireless State Response Message
 */
struct WirelessStateResponseMessage {
	MessageHeader header;

	u_int8_t interfaceCount;
	u_int8_t reserved_first8;
	u_int16_t reserved_last16;
} __attribute__((packed));

/**
 * Wireless State of a Certain Interface
 */
typedef u_int16_t InterfaceID;
/**
 * Interface type
 */
enum WirelessInterfaceType {
	WIRELESS_TYPE_LTE = 0,
	WIRELESS_TYPE_GN = 1
};

struct WirelessStateResponseItem {
	virtual ~WirelessStateResponseItem() {}

	WirelessInterfaceType interfaceType;

	/**
	 * This method stringfies the information contained in this struct and
	 * to be implemented by sub-structs
	 */
	virtual string toString() const = 0;
} __attribute__((packed));

/**
 * Wireless State for GN
 */
struct GnWirelessStateResponseItem : public WirelessStateResponseItem {
	InterfaceID interfaceId; /** 16-bit */
	u_int16_t accessTechnology;
	u_int16_t channelFrequency;
	u_int16_t bandwidth;
	u_int8_t channelBusyRatio;
	u_int8_t status;
	u_int8_t averageTxPower;
	u_int8_t reserved;

	string toString() const {
		stringstream ss;

		ss << "GN WirelessState[If ID:" << interfaceId
			<< ", Access Tech:" << accessTechnology
			<< ", Channel Freq:" << channelFrequency
			<< ", Bandwidth:" << bandwidth
			<< ", Busy Ratio:" << (int)channelBusyRatio
			<< ", Status:" << (int)status
			<< ", Average TX Power:" << (int)averageTxPower << "]";

		return ss.str();
	}
} __attribute__((packed));

/**
 * Wireless State for LTE
 */
#define LTE_WIRELESS_STATE_RESPONSE_SIZE 0x14 /** 4-byte header, 16-byte body (including reserved 4 bytes) */
struct LteWirelessStateResponse : public WirelessStateResponseItem {
	InterfaceID interfaceId; /** 16-bit */
	u_int16_t reservedFirst16bit;
	u_int16_t referenceSignalReceivedPower;
	u_int16_t referenceSignalReceivedQuality;
	u_int8_t channelQualityIndication;
	u_int8_t status;
	u_int16_t reservedSecond16bit;
	u_int32_t packetLossRate;

	string toString() const {
		stringstream ss;

		ss << "LTE WirelessState[If ID:" << interfaceId
			<< ", Ref. Sig. Rx Pow.:" << referenceSignalReceivedPower
			<< ", Ref. Sig. Rx Qual.:" << referenceSignalReceivedQuality
			<< ", Channel Qual. Ind.:" << (int)channelQualityIndication
			<< ", Status:" << (int)status
			<< ", Pkt Loss Rate:" << packetLossRate << "]";

		return ss.str();
	}
} __attribute__((packed));

/**
 * Network State Message
 */
struct NetworkStateMessage {
	MessageHeader header;

	u_int32_t timestamp;
	u_int32_t rxPackets;
	u_int32_t rxBytes;
	u_int32_t txPackets;
	u_int32_t txBytes;
	u_int32_t toUpperLayerPackets;
	u_int32_t discardedPackets;
	u_int32_t duplicatePackets;
	u_int32_t forwardedPackets;
} __attribute__((packed));

/**
 * Configuration Available Message
 */
struct ConfigureAvailableMessage {
	MessageHeader header;

	u_int16_t reserved;
	u_int16_t keyCount;
} __attribute__((packed));

/**
 * Configuration Request Message
 */
struct ConfigurationRequestMessage {
	MessageHeader header;

	u_int16_t configurationId;
	u_int16_t transmissionMode;
} __attribute__((packed));

/**
 * Configuration Item
 *
 * This structure holds the common "configuration item" fields of
 * continuous and bulk configuration set messages
 */
struct ConfigurationItem {
	u_int16_t configurationId;
	u_int16_t length; /* # of DWORDs */
	u_int32_t configurationValue;
} __attribute__((packed));

/**
 * Variable-size Configuration Item
 *
 * This structure holds the variable-size "configuration item" field
 * of FAC Configuration Notification
 */
struct VariableSizeConfigurationItem {
	u_int16_t configurationId;
	u_int16_t length; /* # of bytes */
	vector<unsigned char> configurationBuffer;
};

/**
 * Set Configuration Event (Continuous)
 */
struct ContinuousConfigurationResponse {
	MessageHeader header;

	ConfigurationItem configurationItem;
} __attribute__((packed));

/**
 * Set Configuration Event (Bulk)
 *
 * This struct is processed with a std::vector<ConfigurationItem>
 * See GeonetSetConfigurationEventPacket.{hpp|cpp}
 */
struct BulkConfigurationResponse {
	MessageHeader header;

	u_int16_t reserved;
	u_int16_t keyCount;
} __attribute__((packed));

/**
 * FAC Configuration Notification
 */
struct ConfigurationNotification {
	MessageHeader header;

	VariableSizeConfigurationItem configurationItem;
};

#endif /* MGMT_TYPES_HPP_ */

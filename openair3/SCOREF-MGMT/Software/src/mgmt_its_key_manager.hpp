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
 * \file mgmt_its_key_manager.hpp
 * \brief ITS keys and relevant configuration information is maintained in this container
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_ITS_KEY_MANAGER_HPP_
#define MGMT_ITS_KEY_MANAGER_HPP_

#include <map>
#include <string>
#include <climits>
using namespace std;

#include "mgmt_types.hpp"
#include "util/mgmt_log.hpp"
#include <sys/types.h>

/**
 * ITS key types according to module they belong to
 *
 * ITS_KEY_TYPE_COMMON: ITS keys common for NETwork and FACilities,
 * ITS_KEY_TYPE_NET: NETwork ITS keys,
 * ITS_KEY_TYPE_FAC: FACilities ITS keys,
 * ITS_KEY_TYPE_IHM: PCVE IHM keys,
 * ITS_KEY_TYPE_ALL: All ITS keys defined
 */
enum ItsKeyType {
	ITS_KEY_TYPE_COMMON = 0,
	ITS_KEY_TYPE_NET = 1,
	ITS_KEY_TYPE_FAC = 2,
	ITS_KEY_TYPE_IHM = 3,
	ITS_KEY_TYPE_ALL = 4
};
/**
 * ITS data type
 */
enum ItsDataType {
	ITS_DATA_TYPE_INTEGER = 0,
	ITS_DATA_TYPE_FLOAT = 1,
	ITS_DATA_TYPE_STRING = 2
};
/**
 * ITS key value container that can contain
 * integer, float, and string values
 */
struct ItsKeyValue {
	string stringValue;
	float floatValue;
	u_int32_t intValue;
};
/**
 * ITS key structure holding every property of an ITS key
 */
struct ItsKey {
	string name;
	ItsKeyType keyType;
	ItsDataType dataType;
	ItsKeyValue value;
	ItsKeyValue minValue;
	ItsKeyValue maxValue;
};

/**
 * ITS keys and relevant configuration information is maintained in this container
 */
class ItsKeyManager {
	public:
		/**
		 * Constructor for ItsKeyManager class
		 *
		 * @param logger Logger object reference
		 */
		ItsKeyManager(Logger& logger);
		/**
		 * Destructor for ItsKeyManager class
		 */
		~ItsKeyManager();

	public:
		/**
		 * Enqueues given ITS key, name and value to ITS keys map
		 *
		 * @param id ITS key ID of new key to be added
		 * @param name Name of new key to be added
		 * @param keyType ITS key type
		 * @param dataType ITS key's data type
		 * @param value Value of new key to be added
		 * @param minValue Minimum value of new key
		 * @param maxValue Maximum value of new key
		 * @return true on success, false otherwise
		 */
		bool addKey(ItsKeyID id, const string& name, ItsKeyType keyType, ItsDataType dataType, ItsKeyValue value, ItsKeyValue minValue, ItsKeyValue maxValue);
		/**
		 * Enqueues given ITS key, name and value to ITS keys map
		 *
		 * @param id ITS key ID of new key to be added
		 * @param name Name of new key to be added
		 * @param keyType ITS key type
		 * @param value Value of new key to be added
		 * @param minValue Minimum value of new key
		 * @param maxValue Maximum value of new key
		 * @return true on success, false otherwise
		 */
		bool addKey(ItsKeyID id, const string& name, ItsKeyType keyType, u_int32_t value, u_int32_t minValue = 0, u_int32_t maxValue = INT_MAX);
		/**
		 * Enqueues given ITS key, name and value to ITS keys map
		 *
		 * @param id ITS key ID of new key to be added
		 * @param name Name of new key to be added
		 * @param keyType ITS key type
		 * @param value Value of new key to be added
		 * @return true on success, false otherwise
		 */
		bool addKey(ItsKeyID id, const string& name, ItsKeyType keyType, const string& value);
		/**
		 * Returns the value container of the key with given ITS key ID
		 *
		 * @param id ITS key ID of the key
		 * @return Value of ITS key
		 */
		ItsKeyValue& getKeyValue(ItsKeyID id);
		/**
		 * Returns ITS key type
		 *
		 * @param id ITS key ID of the key
		 * @return ITS key type
		 */
		ItsKeyType getKeyType(ItsKeyID id);
		/**
		 * Returns ITS data type (integer or string)
		 *
		 * @param id ITS key ID of the key
		 * @return ITS key type
		 */
		ItsDataType getDataType(ItsKeyID id);
		/**
		 * Returns ITS data type's string name
		 *
		 * @param id ITS key ID of the key
		 * @return std::string name of the data type of the ITS key
		 */
		string getDataTypeName(ItsKeyID id);
		/**
		 * Returns the size of ITS key
		 *
		 * @param id ITS key ID of the key
		 * @return Size of the ITS key's value in bytes
		 */
		std::size_t getDataTypeSize(ItsKeyID id);
		/**
		 * Sets the value of ITS key given its name
		 *
		 * @param name Name of ITS key to be reset
		 * @param value Value to be set as the new value of relevant ITS key
		 * @return true on success, false otherwise
		 */
		bool setKeyValue(const string& name, ItsKeyValue value);
		/**
		 * Sets the value of ITS key given its ITS key ID
		 *
		 * @param id ITS key ID of ITS key to be reset
		 * @param value Value to be set as the new value of relevant ITS key
		 * @return true on success, false otherwise
		 */
		bool setKeyValue(ItsKeyID id, ItsKeyValue value);
		/**
		 * Returns ITS key ID of ITS key given its name
		 *
		 * @param keyName Name of the ITS key being searched
		 * @return ITS key ID of the ITS key if found, 0xDEAD otherwise
		 */
		ItsKeyID findKeyId(const string& keyName) const;
		/**
		 * Returns the number of ITS keys defined in ITS key map
		 *
		 * @param type ITS Key type, may take following values,
		 * ITS_KEY_TYPE_COMMON = Returns ITS keys common for NETwork and FACilities,
		 * ITS_KEY_TYPE_NET = Returns NETwork ITS keys,
		 * ITS_KEY_TYPE_FAC = Returns FACilities ITS keys,
		 * ITS_KEY_TYPE_ALL = Returns all ITS keys defined
		 */
		u_int16_t getNumberOfKeys(ItsKeyType type = ITS_KEY_TYPE_COMMON) const;

	public:
		/**
		 * Returns the subset of ITS key map having ITS keys of given type
		 *
		 * @param keyType ITS key type to filter ITS keys
		 * @return List of type std::map containing ITS keys of asked type
		 */
		map<ItsKeyID, ItsKeyValue> getSubset(ItsKeyType keyType) const;

	private:
		/**
		 * List of type std::map for 'ITS key ID to ITS key value' mapping
		 */
		map<ItsKeyID, ItsKey> itsKeyMap;
		/**
		 * String name map for ITS key data types
		 */
		map<ItsDataType, string> itsKeyDataTypeName;
		/**
		 * Logger object reference
		 */
		Logger& logger;
};

#endif /* MGMT_ITS_KEY_MANAGER_HPP_ */

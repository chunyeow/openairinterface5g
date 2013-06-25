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
 * \file mgmt_its_key_manager.cpp
 * \brief ITS keys and relevant configuration information is maintained in this container
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#include "mgmt_its_key_manager.hpp"
#include "util/mgmt_exception.hpp"
#include <boost/lexical_cast.hpp>
#include "util/mgmt_util.hpp"
#include <sstream>

ItsKeyManager::ItsKeyManager(Logger& logger) : logger(logger) {
	/**
	 * Insert string correspondences for numerical values of ITS key data types
	 */
	itsKeyDataTypeName.insert(itsKeyDataTypeName.end(), std::make_pair(ITS_DATA_TYPE_FLOAT, "FLOAT"));
	itsKeyDataTypeName.insert(itsKeyDataTypeName.end(), std::make_pair(ITS_DATA_TYPE_INTEGER, "INTEGER"));
	itsKeyDataTypeName.insert(itsKeyDataTypeName.end(), std::make_pair(ITS_DATA_TYPE_STRING, "STRING"));
}

ItsKeyManager::~ItsKeyManager() {
	itsKeyMap.clear();
}

ItsKeyID ItsKeyManager::findKeyId(const string& keyName) const {
	map<ItsKeyID, ItsKey>::const_iterator iterator = itsKeyMap.begin();

	while (iterator != this->itsKeyMap.end()) {
		if (!iterator->second.name.compare(keyName))
			return iterator->first;

		++iterator;
	}

	return static_cast<ItsKeyID>(0xDEAD);
}

map<ItsKeyID, ItsKeyValue> ItsKeyManager::getSubset(ItsKeyType keyType) const {
	map<ItsKeyID, ItsKey>::const_iterator iterator = itsKeyMap.begin();
	map<ItsKeyID, ItsKeyValue> subset;

	while (iterator != this->itsKeyMap.end()) {
		// Add every ITS key which is common and is of requested type into the map
		if (iterator->second.keyType == keyType || iterator->second.keyType == ITS_KEY_TYPE_COMMON || keyType == ITS_KEY_TYPE_ALL)
			subset.insert(subset.end(), std::make_pair(iterator->first, iterator->second.value));

		++iterator;
	}

	return subset;
}

bool ItsKeyManager::addKey(ItsKeyID id, const string& name, ItsKeyType keyType, ItsDataType dataType, ItsKeyValue value, ItsKeyValue minValue, ItsKeyValue maxValue) {
	/**
	 * Validate incoming values
	 */
	if (name.empty())
		throw Exception("ITS key name is empty!", logger);
	else if (dataType == ITS_DATA_TYPE_INTEGER && (value.intValue < minValue.intValue || value.intValue > maxValue.intValue))
		throw Exception("ITS key '" + name + "'s value (" + boost::lexical_cast<string>(value.intValue) + ") is out-of-range!", logger);
	else if (dataType == ITS_DATA_TYPE_FLOAT && (value.floatValue < minValue.floatValue || value.floatValue > maxValue.floatValue))
		throw Exception("ITS key '" + name + "'s value (" + boost::lexical_cast<string>(value.floatValue) + ") is out-of-range!", logger);

	ItsKey itsKey;
	itsKey.name = name;
	itsKey.keyType = keyType;
	itsKey.dataType = dataType;
	itsKey.value = value;
	itsKey.minValue = minValue;
	itsKey.maxValue = maxValue;
	itsKeyMap.insert(itsKeyMap.end(), std::make_pair(id, itsKey));

	return true;
}

bool ItsKeyManager::addKey(ItsKeyID id, const string& name, ItsKeyType keyType, u_int32_t value, u_int32_t minValue, u_int32_t maxValue) {
	ItsKeyValue valueContainer, minValueContainer, maxValueContainer;

	valueContainer.intValue = value;
	minValueContainer.intValue = minValue;
	maxValueContainer.intValue = maxValue;

	ItsKey itsKey;
	itsKey.name = name;
	itsKey.keyType = keyType;
	itsKey.dataType = ITS_DATA_TYPE_INTEGER;
	itsKey.value = valueContainer;
	itsKey.minValue = minValueContainer;
	itsKey.maxValue = maxValueContainer;
	itsKeyMap.insert(itsKeyMap.end(), std::make_pair(id, itsKey));

	return true;
}

bool ItsKeyManager::addKey(ItsKeyID id, const string& name, ItsKeyType keyType, const string& value) {
	ItsKeyValue valueContainer;
	valueContainer.stringValue = Util::trim(value, '"');

	ItsKey itsKey;
	itsKey.name = name;
	itsKey.keyType = keyType;
	itsKey.dataType = ITS_DATA_TYPE_STRING;
	itsKey.value = valueContainer;
	itsKeyMap.insert(itsKeyMap.end(), std::make_pair(id, itsKey));

	return true;
}

ItsKeyValue& ItsKeyManager::getKeyValue(ItsKeyID id) {
	return itsKeyMap[id].value;
}

ItsKeyType ItsKeyManager::getKeyType(ItsKeyID id) {
	return itsKeyMap[id].keyType;
}

ItsDataType ItsKeyManager::getDataType(ItsKeyID id) {
	return itsKeyMap[id].dataType;
}

string ItsKeyManager::getDataTypeName(ItsKeyID id) {
	return itsKeyDataTypeName[itsKeyMap[id].dataType];
}

std::size_t ItsKeyManager::getDataTypeSize(ItsKeyID id) {
	switch (itsKeyMap[id].dataType) {
		case ITS_DATA_TYPE_INTEGER:
			return sizeof(int);

		case ITS_DATA_TYPE_FLOAT:
			return sizeof(float);

		case ITS_DATA_TYPE_STRING:
			return itsKeyMap[id].value.stringValue.length();

		default:
			logger.warning("ITS key with ID " + boost::lexical_cast<string>((int)id) + " doesn't have a valid data type!");
			/**
			 * Most of the ITS keys are of type integer so let's return the size of an integer
			 */
			return sizeof(int);
	}
}

bool ItsKeyManager::setKeyValue(const string& name, ItsKeyValue value) {
	map<ItsKeyID, ItsKey>::iterator iterator = itsKeyMap.begin();

	while (iterator != this->itsKeyMap.end()) {
		if (!name.compare(0, iterator->second.name.length(), iterator->second.name)) {
			iterator->second.value = value;
			return true;
		}

		++iterator;
	}

	return false;
}

bool ItsKeyManager::setKeyValue(ItsKeyID id, ItsKeyValue value) {
	itsKeyMap[id].value = value;

	return true;
}

u_int16_t ItsKeyManager::getNumberOfKeys(ItsKeyType type) const {
	/**
	 * If we're asked for all, return the size of the ITS key map
	 */
	if (type == ITS_KEY_TYPE_ALL)
		return itsKeyMap.size();

	map<ItsKeyID, ItsKey>::const_iterator iterator = itsKeyMap.begin();
	u_int16_t numberOfKeys = 0;

	while (iterator != itsKeyMap.end()) {
		/**
		 * Count all `common' keys and those of type `type'
		 */
		if (type == ITS_KEY_TYPE_COMMON || iterator->second.keyType == type)
			++numberOfKeys;

		++iterator;
	}

	return numberOfKeys;
}

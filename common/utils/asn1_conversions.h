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

#ifndef ASN1_CONVERSIONS_H_
#define ASN1_CONVERSIONS_H_

#include "BIT_STRING.h"
#include "assertions.h"

//-----------------------begin func -------------------

/*! \fn uint8_t BIT_STRING_to_uint8(BIT_STRING_t *)
 *\brief  This function extract at most a 8 bits value from a BIT_STRING_t object, the exact bits number depend on the BIT_STRING_t contents.
 *\param[in] pointer to the BIT_STRING_t object.
 *\return the extracted value.
 */
static inline uint8_t BIT_STRING_to_uint8(BIT_STRING_t *asn) {
  DevCheck ((asn->size == 1), asn->size, 0, 0);

  return asn->buf[0] >> asn->bits_unused;
}

/*! \fn uint16_t BIT_STRING_to_uint16(BIT_STRING_t *)
 *\brief  This function extract at most a 16 bits value from a BIT_STRING_t object, the exact bits number depend on the BIT_STRING_t contents.
 *\param[in] pointer to the BIT_STRING_t object.
 *\return the extracted value.
 */
static inline uint16_t BIT_STRING_to_uint16(BIT_STRING_t *asn) {
  uint16_t result = 0;
  int index = 0;

  DevCheck ((asn->size > 0) && (asn->size <= 2), asn->size, 0, 0);

  switch (asn->size) {
    case 2:
      result |= asn->buf[index++] << (8 - asn->bits_unused);

    case 1:
      result |= asn->buf[index] >> asn->bits_unused;
      break;

    default:
      break;
  }

  return result;
}

/*! \fn uint32_t BIT_STRING_to_uint32(BIT_STRING_t *)
 *\brief  This function extract at most a 32 bits value from a BIT_STRING_t object, the exact bits number depend on the BIT_STRING_t contents.
 *\param[in] pointer to the BIT_STRING_t object.
 *\return the extracted value.
 */
static inline uint32_t BIT_STRING_to_uint32(BIT_STRING_t *asn) {
  uint32_t result = 0;
  int index;
  int shift;

  DevCheck ((asn->size > 0) && (asn->size <= 4), asn->size, 0, 0);

  shift = ((asn->size - 1) * 8) - asn->bits_unused;
  for (index = 0; index < (asn->size - 1); index++) {
    result |= asn->buf[index] << shift;
    shift -= 8;
  }

  result |= asn->buf[index] >> asn->bits_unused;

  return result;
}

/*! \fn uint64_t BIT_STRING_to_uint64(BIT_STRING_t *)
 *\brief  This function extract at most a 64 bits value from a BIT_STRING_t object, the exact bits number depend on the BIT_STRING_t contents.
 *\param[in] pointer to the BIT_STRING_t object.
 *\return the extracted value.
 */
static inline uint64_t BIT_STRING_to_uint64(BIT_STRING_t *asn) {
  uint64_t result = 0;
  int index;
  int shift;

  DevCheck ((asn->size > 0) && (asn->size <= 8), asn->size, 0, 0);

  shift = ((asn->size - 1) * 8) - asn->bits_unused;
  for (index = 0; index < (asn->size - 1); index++) {
    result |= asn->buf[index] << shift;
    shift -= 8;
  }

  result |= asn->buf[index] >> asn->bits_unused;

  return result;
}

#endif

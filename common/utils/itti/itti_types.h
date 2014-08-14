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

/** @brief Intertask Interface common types
 * Contains type definitions used for generating and parsing ITTI messages.
 * @author Laurent Winckel <laurent.winckel@eurecom.fr>
 */

#ifndef _ITTI_TYPES_H_
#define _ITTI_TYPES_H_

#include <stdint.h>

#define CHARS_TO_UINT32(c1, c2, c3, c4) (((c4) << 24) | ((c3) << 16) | ((c2) << 8) | (c1))

#define MESSAGE_NUMBER_CHAR_FORMAT      "%11u"

/* Intertask message types */
enum itti_message_types_e
{
    ITTI_DUMP_XML_DEFINITION =        CHARS_TO_UINT32 ('\n', 'I', 'x', 'd'),
    ITTI_DUMP_XML_DEFINITION_END =    CHARS_TO_UINT32 ('i', 'X', 'D', '\n'),

    ITTI_DUMP_MESSAGE_TYPE =          CHARS_TO_UINT32 ('\n', 'I', 'm', 's'),
    ITTI_DUMP_MESSAGE_TYPE_END =      CHARS_TO_UINT32 ('i', 'M', 'S', '\n'),

    ITTI_STATISTIC_MESSAGE_TYPE =     CHARS_TO_UINT32 ('\n', 'I', 's', 't'),
    ITTI_STATISTIC_MESSAGE_TYPE_END = CHARS_TO_UINT32 ('i', 'S', 'T', '\n'),

    /* This signal is not meant to be used by remote analyzer */
    ITTI_DUMP_EXIT_SIGNAL =           CHARS_TO_UINT32 ('e', 'X', 'I', 'T'),
};

typedef uint32_t itti_message_types_t;

/* Message header is the common part that should never change between
 * remote process and this one.
 */
typedef struct {
    /* The size of this structure */
    uint32_t              message_size;
    itti_message_types_t  message_type;
} itti_socket_header_t;

typedef struct {
    char message_number_char[12]; /* 9 chars are needed to store an unsigned 32 bits value in decimal, but must be a multiple of 32 bits to avoid alignment issues */
} itti_signal_header_t;


#define INSTANCE_DEFAULT    (UINT16_MAX - 1)
#define INSTANCE_ALL        (UINT16_MAX)

typedef uint16_t instance_t;

#endif


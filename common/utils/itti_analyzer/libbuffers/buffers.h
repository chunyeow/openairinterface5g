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

#include <stdint.h>

#ifndef BUFFERS_H_
#define BUFFERS_H_

typedef struct buffer_s {
    /* The size in bytes as read from socket */
    uint32_t size_bytes;

    /* Current position */
    uint8_t *buffer_current;

    /* The complete data */
    uint8_t *data;

    /* The message number as read from socket */
    uint32_t message_number;

    uint32_t message_id;
} buffer_t;

uint8_t buffer_get_uint8_t(buffer_t *buffer, uint32_t offset);

uint16_t buffer_get_uint16_t(buffer_t *buffer, uint32_t offset);

uint32_t buffer_get_uint32_t(buffer_t *buffer, uint32_t offset);

uint64_t buffer_get_uint64_t(buffer_t *buffer, uint32_t offset);

int buffer_fetch_bits(buffer_t *buffer, uint32_t offset, int nbits, uint32_t *value);

int buffer_fetch_nbytes(buffer_t *buffer, uint32_t offset, int n_bytes, uint8_t *value);

void buffer_dump(buffer_t *buffer, FILE *to);

int buffer_append_data(buffer_t *buffer, const uint8_t *data, const uint32_t length);

int buffer_new_from_data(buffer_t **buffer, uint8_t *data, const uint32_t length,
                         int data_static);

int buffer_has_enouch_data(buffer_t *buffer, uint32_t offset, uint32_t to_get);

void *buffer_at_offset(buffer_t *buffer, uint32_t offset);

#endif /* BUFFERS_H_ */

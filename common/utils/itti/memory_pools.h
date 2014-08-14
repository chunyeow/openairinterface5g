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

#ifndef MEMORY_POOLS_H_
#define MEMORY_POOLS_H_

#include <stdint.h>

typedef void * memory_pools_handle_t;
typedef void * memory_pool_item_handle_t;

memory_pools_handle_t memory_pools_create (uint32_t pools_number);

char *memory_pools_statistics(memory_pools_handle_t memory_pools_handle);

int memory_pools_add_pool (memory_pools_handle_t memory_pools_handle, uint32_t pool_items_number, uint32_t pool_item_size);

memory_pool_item_handle_t memory_pools_allocate (memory_pools_handle_t memory_pools_handle, uint32_t item_size, uint16_t info_0, uint16_t info_1);

int memory_pools_free (memory_pools_handle_t memory_pools_handle, memory_pool_item_handle_t memory_pool_item_handle, uint16_t info_0);

void memory_pools_set_info (memory_pools_handle_t memory_pools_handle, memory_pool_item_handle_t memory_pool_item_handle, int index, uint16_t info);

#endif /* MEMORY_POOLS_H_ */

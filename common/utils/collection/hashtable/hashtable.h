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


#ifndef _UTILS_COLLECTION_HASH_TABLE_H_
#define _UTILS_COLLECTION_HASH_TABLE_H_
#include<stdlib.h>
#include <stdint.h>
#include <stddef.h>

typedef size_t   hash_size_t;
typedef uint64_t hash_key_t;

#define HASHTABLE_QUESTIONABLE_KEY_VALUE ((uint64_t)-1)

typedef enum hashtable_return_code_e {
    HASH_TABLE_OK                      = 0,
    HASH_TABLE_INSERT_OVERWRITTEN_DATA = 1,
    HASH_TABLE_KEY_NOT_EXISTS          = 2,
    HASH_TABLE_KEY_ALREADY_EXISTS      = 3,
    HASH_TABLE_BAD_PARAMETER_HASHTABLE = 4,
    HASH_TABLE_SYSTEM_ERROR            = 5,
    HASH_TABLE_CODE_MAX
} hashtable_rc_t;


typedef struct hash_node_s {
    hash_key_t          key;
    void               *data;
    struct hash_node_s *next;
} hash_node_t;

typedef struct hash_table_s {
	hash_size_t         size;
	hash_size_t         num_elements;
	struct hash_node_s **nodes;
	hash_size_t       (*hashfunc)(const hash_key_t);
	void              (*freefunc)(void*);
} hash_table_t;

char*           hashtble_rc_code2string(hashtable_rc_t rcP);
void            hash_free_int_func(void* memoryP);
hash_table_t   *hashtable_create (hash_size_t   size, hash_size_t (*hashfunc)(const hash_key_t ), void (*freefunc)(void*));
hashtable_rc_t  hashtable_destroy(hash_table_t *hashtbl);
hashtable_rc_t  hashtable_is_key_exists (hash_table_t *hashtbl, const uint64_t key);
hashtable_rc_t  hashtable_apply_funct_on_elements (hash_table_t *hashtblP, void funct(hash_key_t keyP, void* dataP, void* parameterP), void* parameterP);
hashtable_rc_t  hashtable_insert (hash_table_t *hashtbl, const hash_key_t key, void *data);
hashtable_rc_t  hashtable_remove (hash_table_t *hashtbl, const hash_key_t key);
hashtable_rc_t  hashtable_get    (hash_table_t *hashtbl, const hash_key_t key, void **dataP);
hashtable_rc_t  hashtable_resize (hash_table_t *hashtbl, hash_size_t size);



#endif


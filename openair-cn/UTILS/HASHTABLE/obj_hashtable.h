#ifndef _OBJ_HASH_TABLE_H_
#define _OBJ_HASH_TABLE_H_
#include<stdlib.h>
#include <stdint.h>
#include <stddef.h>

#include "hashtable.h"

typedef size_t hash_size_t;


typedef struct obj_hash_node_s {
	int                 key_size;
	void               *key;
	void               *data;
	struct obj_hash_node_s *next;
} obj_hash_node_t;

typedef struct obj_hash_table_s {
	hash_size_t         size;
	hash_size_t         num_elements;
	struct obj_hash_node_s **nodes;
	hash_size_t       (*hashfunc)(const void*, int);
	void              (*freekeyfunc)(void*);
	void              (*freedatafunc)(void*);
} obj_hash_table_t;

obj_hash_table_t *obj_hashtbl_create (hash_size_t   size, hash_size_t (*hashfunc)(const void*, int ), void (*freekeyfunc)(void*), void (*freedatafunc)(void*));
hashtbl_rc_t      obj_hashtbl_destroy(obj_hash_table_t *hashtblP);
hashtbl_rc_t      obj_hashtbl_is_key_exists (obj_hash_table_t *hashtblP, void* keyP, int key_sizeP);
hashtbl_rc_t      obj_hashtbl_insert (obj_hash_table_t *hashtblP,       void* keyP, int key_sizeP, void *dataP);
hashtbl_rc_t      obj_hashtbl_remove (obj_hash_table_t *hashtblP, const void* keyP, int key_sizeP);
hashtbl_rc_t      obj_hashtbl_get    (obj_hash_table_t *hashtblP, const void* keyP, int key_sizeP, void ** dataP);
hashtbl_rc_t      obj_hashtbl_resize (obj_hash_table_t *hashtblP, hash_size_t sizeP);



#endif


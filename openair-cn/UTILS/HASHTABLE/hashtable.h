#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_
#include<stdlib.h>
#include <stdint.h>
#include <stddef.h>

typedef size_t hash_size_t;

typedef enum hashtbl_return_code_e {
    HASH_TABLE_OK                      = 0,
    HASH_TABLE_INSERT_OVERWRITTEN_DATA = 1,
    HASH_TABLE_KEY_NOT_EXISTS          = 2,
    HASH_TABLE_KEY_ALREADY_EXISTS      = 3,
    HASH_TABLE_BAD_PARAMETER_HASHTABLE = 4,
    HASH_TABLE_CODE_MAX
} hashtbl_rc_t;


typedef struct hash_node_s {
	uint64_t            key;
	void               *data;
	struct hash_node_s *next;
} hash_node_t;

typedef struct hash_table_s {
	hash_size_t         size;
	hash_size_t         num_elements;
	struct hash_node_s **nodes;
	hash_size_t       (*hashfunc)(const uint64_t);
	void              (*freefunc)(void*);
} hash_table_t;

char*         hashtble_rc_code2string(hashtbl_rc_t rcP);
hash_table_t *hashtbl_create (hash_size_t   size, hash_size_t (*hashfunc)(const uint64_t ), void (*freefunc)(void*));
hashtbl_rc_t  hashtbl_destroy(hash_table_t *hashtbl);
hashtbl_rc_t  hashtbl_is_key_exists (hash_table_t *hashtbl, const uint64_t key);
hashtbl_rc_t  hashtbl_apply_funct_on_elements (hash_table_t *hashtblP, void funct(uint64_t keyP, void* dataP, void* parameterP), void* parameterP);
hashtbl_rc_t  hashtbl_insert (hash_table_t *hashtbl, const uint64_t key, void *data);
hashtbl_rc_t  hashtbl_remove (hash_table_t *hashtbl, const uint64_t key);
hashtbl_rc_t  hashtbl_get    (hash_table_t *hashtbl, const uint64_t key, void **dataP);
hashtbl_rc_t  hashtbl_resize (hash_table_t *hashtbl, hash_size_t size);



#endif


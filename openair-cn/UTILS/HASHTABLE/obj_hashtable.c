/* from: http://en.literateprograms.org/Hash_table_%28C%29#chunk%20def:node
 *
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "obj_hashtable.h"
//-------------------------------------------------------------------------------------------------------------------------------
/*
 * Default hash function
 * def_hashfunc() is the default used by hashtbl_create() when the user didn't specify one.
 * This is a simple/naive hash function which adds the key's ASCII char values. It will probably generate lots of collisions on large hash tables.
 */

static hash_size_t def_hashfunc(const void *keyP, int key_sizeP)
{
	hash_size_t hash=0;

	while(key_sizeP) hash^=((unsigned char*)keyP)[key_sizeP --];

	return hash;
}

//-------------------------------------------------------------------------------------------------------------------------------
/*
 * Initialisation
 * hashtbl_create() sets up the initial structure of the hash table. The user specified size will be allocated and initialized to NULL.
 * The user can also specify a hash function. If the hashfunc argument is NULL, a default hash function is used.
 * If an error occurred, NULL is returned. All other values in the returned obj_hash_table_t pointer should be released with hashtbl_destroy().
 */
obj_hash_table_t *obj_hashtbl_create(hash_size_t sizeP, hash_size_t (*hashfuncP)(const void*, int ), void (*freekeyfuncP)(void*), void (*freedatafuncP)(void*))
{
	obj_hash_table_t *hashtbl;

	if(!(hashtbl=malloc(sizeof(obj_hash_table_t)))) return NULL;

	if(!(hashtbl->nodes=calloc(sizeP, sizeof(obj_hash_node_t*)))) {
		free(hashtbl);
		return NULL;
	}

	hashtbl->size=sizeP;

	if(hashfuncP) hashtbl->hashfunc=hashfuncP;
	else hashtbl->hashfunc=def_hashfunc;

	if(freekeyfuncP) hashtbl->freekeyfunc=freekeyfuncP;
	else hashtbl->freekeyfunc=free;

	if(freedatafuncP) hashtbl->freedatafunc=freedatafuncP;
	else hashtbl->freedatafunc=free;

	return hashtbl;
}
//-------------------------------------------------------------------------------------------------------------------------------
/*
 * Cleanup
 * The hashtbl_destroy() walks through the linked lists for each possible hash value, and releases the elements. It also releases the nodes array and the obj_hash_table_t.
 */
hashtbl_rc_t obj_hashtbl_destroy(obj_hash_table_t *hashtblP)
{
	hash_size_t n;
	obj_hash_node_t *node, *oldnode;

	for(n=0; n<hashtblP->size; ++n) {
		node=hashtblP->nodes[n];
		while(node) {
			oldnode=node;
			node=node->next;
			hashtblP->freekeyfunc(oldnode->key);
			hashtblP->freedatafunc(oldnode->data);
			free(oldnode);
		}
	}
	free(hashtblP->nodes);
	free(hashtblP);
	return HASH_TABLE_OK;
}
//-------------------------------------------------------------------------------------------------------------------------------
hashtbl_rc_t obj_hashtbl_is_key_exists (obj_hash_table_t *hashtblP, void* keyP, int key_sizeP)
//-------------------------------------------------------------------------------------------------------------------------------
{
	obj_hash_node_t *node;
	hash_size_t      hash;

	if (hashtblP == NULL) {
		return HASH_TABLE_BAD_PARAMETER_HASHTABLE;
	}
	hash=hashtblP->hashfunc(keyP, key_sizeP)%hashtblP->size;
	node=hashtblP->nodes[hash];
	while(node) {
		if(node->key == keyP) {
			return HASH_TABLE_OK;
		}
		node=node->next;
	}
	return HASH_TABLE_KEY_NOT_EXISTS;
}
//-------------------------------------------------------------------------------------------------------------------------------
/*
 * Adding a new element
 * To make sure the hash value is not bigger than size, the result of the user provided hash function is used modulo size.
 */
hashtbl_rc_t obj_hashtbl_insert(obj_hash_table_t *hashtblP, void* keyP, int key_sizeP, void *dataP)
{
	obj_hash_node_t *node;
	hash_size_t      hash;

	if (hashtblP == NULL) {
		return HASH_TABLE_BAD_PARAMETER_HASHTABLE;
	}
	hash=hashtblP->hashfunc(keyP, key_sizeP)%hashtblP->size;
	node=hashtblP->nodes[hash];
	while(node) {
		if(node->key == keyP) {
			if (node->data) {
				hashtblP->freedatafunc(node->data);
			}
			node->data=dataP;
			// waste of memory here (keyP is lost) we should free it now
			return HASH_TABLE_INSERT_OVERWRITTEN_DATA;
		}
		node=node->next;
	}
	if(!(node=malloc(sizeof(obj_hash_node_t)))) return -1;
	node->key=keyP;
	node->data=dataP;
	if (hashtblP->nodes[hash]) {
	    node->next=hashtblP->nodes[hash];
	} else {
		node->next = NULL;
	}
	hashtblP->nodes[hash]=node;
	return HASH_TABLE_OK;
}
//-------------------------------------------------------------------------------------------------------------------------------
/*
 * To remove an element from the hash table, we just search for it in the linked list for that hash value,
 * and remove it if it is found. If it was not found, it is an error and -1 is returned.
 */
hashtbl_rc_t obj_hashtbl_remove(obj_hash_table_t *hashtblP, const void* keyP, int key_sizeP)
{
	obj_hash_node_t *node, *prevnode=NULL;
	hash_size_t      hash;

	if (hashtblP == NULL) {
		return HASH_TABLE_BAD_PARAMETER_HASHTABLE;
	}

	hash=hashtblP->hashfunc(keyP, key_sizeP)%hashtblP->size;
	node=hashtblP->nodes[hash];
	while(node) {
		if(node->key == keyP) {
			if(prevnode) {
				prevnode->next=node->next;
			} else {
				hashtblP->nodes[hash]=node->next;
			}
			hashtblP->freekeyfunc(node->key);
			hashtblP->freedatafunc(node->data);
			free(node);
			return HASH_TABLE_OK;
		}
		prevnode=node;
		node=node->next;
	}
	return HASH_TABLE_KEY_NOT_EXISTS;
}
//-------------------------------------------------------------------------------------------------------------------------------
/*
 * Searching for an element is easy. We just search through the linked list for the corresponding hash value.
 * NULL is returned if we didn't find it.
 */
hashtbl_rc_t obj_hashtbl_get(obj_hash_table_t *hashtblP, const void* keyP, int key_sizeP, void** dataP)
{
	obj_hash_node_t *node;
	hash_size_t      hash;

	if (hashtblP == NULL) {
		*dataP = NULL;
		return HASH_TABLE_BAD_PARAMETER_HASHTABLE;
	}
	hash=hashtblP->hashfunc(keyP, key_sizeP)%hashtblP->size;
	node=hashtblP->nodes[hash];
	while(node) {
		if(node->key == keyP) {
			*dataP = node->data;
			return HASH_TABLE_OK;
		}
		node=node->next;
	}
	*dataP = NULL;
	return HASH_TABLE_KEY_NOT_EXISTS;
}
//-------------------------------------------------------------------------------------------------------------------------------
/*
 * Resizing
 * The number of elements in a hash table is not always known when creating the table.
 * If the number of elements grows too large, it will seriously reduce the performance of most hash table operations.
 * If the number of elements are reduced, the hash table will waste memory. That is why we provide a function for resizing the table.
 * Resizing a hash table is not as easy as a realloc(). All hash values must be recalculated and each element must be inserted into its new position.
 * We create a temporary obj_hash_table_t object (newtbl) to be used while building the new hashes.
 * This allows us to reuse hashtbl_insert() and hashtbl_remove(), when moving the elements to the new table.
 * After that, we can just free the old table and copy the elements from newtbl to hashtbl.
 */
hashtbl_rc_t obj_hashtbl_resize(obj_hash_table_t *hashtblP, hash_size_t sizeP)
{
	obj_hash_table_t       newtbl;
	hash_size_t        n;
	obj_hash_node_t       *node,*next;

	if (hashtblP == NULL) {
		return HASH_TABLE_BAD_PARAMETER_HASHTABLE;
	}

	newtbl.size     = sizeP;
	newtbl.hashfunc = hashtblP->hashfunc;

	if(!(newtbl.nodes=calloc(sizeP, sizeof(obj_hash_node_t*)))) return -1;

	for(n=0; n<hashtblP->size; ++n) {
		for(node=hashtblP->nodes[n]; node; node=next) {
			next = node->next;
			obj_hashtbl_insert(&newtbl, node->key, node->key_size, node->data);
			//WARNING  Lionel GAUTHIER: BAD CODE TO BE REWRITTEN
			obj_hashtbl_remove(hashtblP, node->key, node->key_size);
		}
	}

	free(hashtblP->nodes);
	hashtblP->size=newtbl.size;
	hashtblP->nodes=newtbl.nodes;

	return HASH_TABLE_OK;
}




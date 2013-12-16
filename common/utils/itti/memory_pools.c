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
 Address      : EURECOM, Campus SophiaTech, 450 Route des Chappes
 06410 Biot FRANCE

 *******************************************************************************/

#include "assertions.h"
#include "memory_pools.h"

/*------------------------------------------------------------------------------*/
#define CHARS_TO_UINT32(c1, c2, c3, c4) (((c1) << 24) | ((c2) << 16) | ((c3) << 8) | (c4))

/*------------------------------------------------------------------------------*/
typedef uint32_t pool_item_start_mark_t;
typedef uint32_t pool_item_end_mark_t;

typedef uint32_t pool_start_mark_t;

typedef uint32_t pools_start_mark_t;

typedef uint8_t pool_id_t;

typedef struct memory_pool_item_start_s {
    pool_item_start_mark_t      start_mark;

    pool_id_t                   pool_id;
    uint32_t                    info;
} memory_pool_item_start_t;

typedef struct memory_pool_item_end_s {
    pool_item_end_mark_t        end_mark;
} memory_pool_item_end_t;

typedef struct memory_pool_item_s {
    memory_pool_item_start_t    start;
    uint32_t                    data[0];
    memory_pool_item_end_t      end;
} memory_pool_item_t;

typedef struct items_group_s {
    volatile int32_t            current;
    volatile int32_t            minimum;
    int32_t                    *indexes;
} items_group_t;

typedef struct memory_pool_s {
    pool_start_mark_t           start_mark;

    pool_id_t                   pool_id;
    uint32_t                    items_number;
    uint32_t                    item_size;
    items_group_t               items_group_free;
    memory_pool_item_t         *items;
} memory_pool_t;

typedef struct memory_pools_s {
    pools_start_mark_t          start_mark;

    uint32_t                    pools_number;
    uint32_t                    pools_defined;
    memory_pool_t              *pools;
} memory_pools_t;

/*------------------------------------------------------------------------------*/
static const uint32_t               MAX_POOLS_NUMBER =      20;
static const uint32_t               MAX_POOL_ITEMS_NUMBER = 1000 * 1000;
static const uint32_t               MAX_POOL_ITEM_SIZE =    100 * 1000;

static const pool_item_start_mark_t POOL_ITEM_START_MARK =  CHARS_TO_UINT32 ('P', 'I', 's', 't');
static const pool_item_end_mark_t   POOL_ITEM_END_MARK =    CHARS_TO_UINT32 ('p', 'i', 'E', 'N');

static const pool_start_mark_t      POOL_START_MARK =       CHARS_TO_UINT32 ('P', '_', 's', 't');

static const pools_start_mark_t     POOLS_START_MARK =      CHARS_TO_UINT32 ('P', 'S', 's', 't');

/*------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
static inline memory_pools_t *memory_pools_from_handler(memory_pools_handle_t *memory_pools_handle)
{
    memory_pools_t *memory_pools;

    /* Recover memory_pools */
    memory_pools = (memory_pools_t *) memory_pools_handle;
    /* Sanity check on passed handle */
    DevAssert (memory_pools->start_mark == POOLS_START_MARK);

    return (memory_pools);
}

static inline memory_pool_item_t *memory_pool_item_from_handler(memory_pool_item_handle_t *memory_pool_item_handle)
{
    void               *address;
    memory_pool_item_t *memory_pool_item;

    /* Recover memory_pools */
    address = ((void *) memory_pool_item_handle) - sizeof (memory_pool_item_start_t);
    memory_pool_item = (memory_pool_item_t *) address;
    /* Sanity check on passed handle */
    DevAssert (memory_pool_item->start.start_mark == POOL_ITEM_START_MARK);

    return (memory_pool_item);
}

static inline int32_t items_group_get_free_item(items_group_t *items_group)
{
    int32_t current;
    int32_t index = -1;

    /* Get current position and decrease it */
    current = __sync_fetch_and_add (&items_group->current, -1);
    if (current < 0)
    {
        /* Current index is not valid, restore previous value */
        __sync_fetch_and_add (&items_group->current, 1);
    }
    else
    {
        /* Updates minimum position if needed */
        while (items_group->minimum > current)
        {
          items_group->minimum = current;
        }

        /* Get index at current position */
        index = items_group->indexes[current];
        DevCheck (index >= 0, current, index, 0);

        /* Clear index at current position */
        items_group->indexes[current] = -1;
    }

    return (index);
}

/*------------------------------------------------------------------------------*/

memory_pools_handle_t *memory_pools_create (uint32_t pools_number)
{
    memory_pools_t *memory_pools;
    pool_id_t       pool;

    DevCheck (pools_number < MAX_POOLS_NUMBER, pools_number, MAX_POOLS_NUMBER, 0); /* Limit to a reasonable number of pools */

    /* Allocate memory_pools */
    memory_pools = malloc (sizeof(memory_pools_t));
    DevAssert (memory_pools != NULL);

    /* Initialize memory_pools */
    {
        memory_pools->start_mark    = POOLS_START_MARK;
        memory_pools->pools_number  = pools_number;
        memory_pools->pools_defined = 0;

        /* Allocate pools */
        memory_pools->pools         = calloc (pools_number, sizeof(memory_pool_t));
        DevAssert (memory_pools->pools != NULL);

        /* Initialize pools */
        for (pool = 0; pool < pools_number; pool++)
        {
            memory_pools->pools[pool].start_mark = POOL_START_MARK;
        }
    }

    return ((memory_pools_handle_t) memory_pools);
}

int memory_pools_add_pool (memory_pools_handle_t *memory_pools_handle, uint32_t pool_items_number, uint32_t pool_item_size)
{
    memory_pools_t *memory_pools;
    memory_pool_t  *memory_pool;
    pool_id_t       pool;
    uint32_t        item;

    DevCheck (pool_items_number < MAX_POOL_ITEMS_NUMBER, pool_items_number, MAX_POOL_ITEMS_NUMBER, 0);  /* Limit to a reasonable number of items */
    DevCheck (pool_item_size < MAX_POOL_ITEM_SIZE, pool_item_size, MAX_POOL_ITEM_SIZE, 0);              /* Limit to a reasonable item size */

    /* Recover memory_pools */
    memory_pools = memory_pools_from_handler (memory_pools_handle);

    /* Check number of already created pools */
    DevAssert (memory_pools->pools_defined < memory_pools->pools_number);

    /* Select pool */
    pool = memory_pools->pools_defined;
    memory_pool = &memory_pools->pools[pool];

    /* Initialize pool */
    {
        memory_pool->pool_id                  = pool;
        memory_pool->items_number             = pool_items_number;
        memory_pool->item_size                = pool_item_size;
        memory_pool->items_group_free.current = pool_items_number - 1;
        memory_pool->items_group_free.minimum = pool_items_number - 1;

        /* Allocate free indexes */
        memory_pool->items_group_free.indexes = malloc(pool_items_number * sizeof(uint32_t));
        DevAssert (memory_pool->items_group_free.indexes != NULL);

        /* Initialize free indexes */
        for (item = 0; item < pool_items_number; item++)
        {
            memory_pool->items_group_free.indexes[item] = item;
        }

        /* Allocate items */
        memory_pool->items = calloc (pool_items_number, (sizeof(memory_pool_item_t) + pool_item_size));
        DevAssert (memory_pool->items != NULL);

        /* Initialize items */
        for (item = 0; item < pool_items_number; item++)
        {
            memory_pool->items[item].start.start_mark                         = POOL_ITEM_START_MARK;
            memory_pool->items[item].start.pool_id                            = pool;
            memory_pool->items[item].data[pool_item_size / sizeof(uint32_t)]  = POOL_ITEM_END_MARK;
        }
    }

    memory_pools->pools_defined ++;

    return (0);
}

memory_pool_item_handle_t *memory_pools_allocate (memory_pools_handle_t *memory_pools_handle, uint32_t item_size)
{
    memory_pools_t     *memory_pools;
    memory_pool_item_t *memory_pool_item = NULL;
    pool_id_t           pool;
    int32_t             item;

    /* Recover memory_pools */
    memory_pools = memory_pools_from_handler (memory_pools_handle);

    for (pool = 0; pool <= memory_pools->pools_defined; pool++)
    {
        if (memory_pools->pools[pool].item_size < item_size)
        {
            /* This memory pool has too small items, skip it */
            continue;
        }

        item = items_group_get_free_item(&memory_pools->pools[pool].items_group_free);
        if (item < 0)
        {
            /* Allocation failed, skip this pool */
            continue;
        }
        else
        {
            /* Allocation succeed, exit searching loop */
            break;
        }
    }

    if (item >= 0)
    {
        /* Convert item index into memory_pool_item address */
        memory_pool_item = &memory_pools->pools[pool].items[item];
    }

    return (memory_pool_item_handle_t *) memory_pool_item->data;
}

void memory_pools_free (memory_pools_handle_t *memory_pools_handle, memory_pool_item_handle_t memory_pool_item_handle)
{
    memory_pools_t     *memory_pools;
    memory_pool_item_t *memory_pool_item = NULL;
    pool_id_t           pool;
    int32_t             item;
    uint32_t            item_size;

    /* Recover memory_pools */
    memory_pools = memory_pools_from_handler (memory_pools_handle);
    /* Recover memory pool item */
    memory_pool_item = memory_pool_item_from_handler (memory_pool_item_handle);
    /* Recover pool index */
    pool = memory_pool_item->start.pool_id;
    DevCheck (pool < memory_pools->pools_defined, pool, memory_pools->pools_defined, 0);

    item_size = memory_pools->pools[pool].item_size;
    item = (((void *) memory_pool_item) - ((void *) memory_pools->pools[pool].items)) / (sizeof(memory_pool_item_t) + item_size);
    /* Sanity check on calculated item index */
    DevCheck (memory_pool_item == &memory_pools->pools[pool].items[item], memory_pool_item, &memory_pools->pools[pool].items[item], pool);
    /* Check if end marker is still present (no write overflow) */
    DevCheck (memory_pool_item->data[item_size / sizeof(uint32_t)] == POOL_ITEM_END_MARK, pool, 0, 0);


}

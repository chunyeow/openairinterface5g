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

#include <stdint.h>

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

    uint32_t                    info;
    pool_id_t                   pool_id;

} memory_pool_item_start_t;

typedef struct memory_pool_item_end_s {
    pool_item_end_mark_t        end_mark;
} memory_pool_item_end_t;

typedef struct memory_pool_item_s {
    memory_pool_item_start_t    start;
    uint32_t                    data[0];
    memory_pool_item_end_t      end;
} memory_pool_item_t;

typedef struct memory_pool_s {
    pool_start_mark_t           start_mark;

    pool_id_t                   pool_id;
    uint32_t                    items_number;
    uint32_t                    item_size;
    uint32_t                    items_free;
    uint32_t                    items_free_min;
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

/*------------------------------------------------------------------------------*/

memory_pools_handle_t *memory_pools_create (uint32_t pools_number)
{
    memory_pools_t *memory_pools;
    uint32_t pool;

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
    memory_pool_t *memory_pool;
    uint32_t pool;
    uint32_t item;

    DevCheck (pool_items_number < MAX_POOL_ITEMS_NUMBER, pool_items_number, MAX_POOL_ITEMS_NUMBER, 0);  /* Limit to a reasonable number of items */
    DevCheck (pool_item_size < MAX_POOL_ITEM_SIZE, pool_item_size, MAX_POOL_ITEM_SIZE, 0);              /* Limit to a reasonable item size */

    /* Recover memory_pools */
    memory_pools = (memory_pools_t *) memory_pools_handle;
    DevAssert (memory_pools->start_mark == POOLS_START_MARK);               /* Sanity check on passed handle */
    DevAssert (memory_pools->pools_defined < memory_pools->pools_number);   /* Check number of already created pools */

    /* Select pool */
    pool = memory_pools->pools_defined;
    memory_pool = &memory_pools->pools[pool];

    /* Initialize pool */
    {
        memory_pool->pool_id           = pool + 1;
        memory_pool->items_number      = pool_items_number;
        memory_pool->item_size         = pool_item_size;
        memory_pool->items_free        = pool_items_number;
        memory_pool->items_free_min    = pool_items_number;

        /* Allocate items */
        memory_pool->items = calloc (pool_items_number, (sizeof(memory_pool_item_t) + pool_item_size));
        DevAssert (memory_pool->items != NULL);

        /* Initialize items */
        for (item = 0; pool < pool_items_number; item++)
        {
            memory_pool->items[item].start.start_mark  = POOL_ITEM_START_MARK;
            memory_pool->items[item].start.pool_id     = memory_pool->pool_id;
            memory_pool->items[item].data[pool_item_size / sizeof(uint32_t)] = POOL_ITEM_END_MARK;
        }
    }

    memory_pools->pools_defined ++;

    return (0);
}


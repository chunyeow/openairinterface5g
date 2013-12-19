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

#ifdef RTAI
# include <rtai_shm.h>
#endif

#include "assertions.h"
#include "memory_pools.h"
#if defined(OAI_EMU) || defined(RTAI)
# include "vcd_signal_dumper.h"
#endif

/*------------------------------------------------------------------------------*/
const static int mp_debug = 0;

#ifdef RTAI
# define MP_DEBUG(x, args...) do { if (mp_debug) rt_printk("[MP][D]"x, ##args); } \
    while(0)
# define MP_ERROR(x, args...) do { rt_printk("[MP][E]"x, ##args); } \
    while(0)
#else
# define MP_DEBUG(x, args...) do { if (mp_debug) fprintf(stdout, "[MP][D]"x, ##args); fflush (stdout); } \
    while(0)
# define MP_ERROR(x, args...) do { fprintf(stdout, "[MP][E]"x, ##args); } \
    while(0)
#endif

#if defined(OAI_EMU) || defined(RTAI)
uint64_t vcd_mp_alloc;
uint64_t vcd_mp_free;
#endif

/*------------------------------------------------------------------------------*/
#define CHARS_TO_UINT32(c1, c2, c3, c4) (((c1) << 24) | ((c2) << 16) | ((c3) << 8) | (c4))

/*------------------------------------------------------------------------------*/
typedef int32_t     items_group_position_t;
typedef int32_t     items_group_index_t;

typedef struct items_group_s {
    items_group_position_t          number;
    volatile items_group_position_t current;
    volatile items_group_position_t minimum;
    volatile items_group_index_t   *indexes;
} items_group_t;

/*------------------------------------------------------------------------------*/
static const items_group_position_t ITEMS_GROUP_POSITION_INVALID    = -1;
static const items_group_index_t    ITEMS_GROUP_INDEX_INVALID       = -1;

/*------------------------------------------------------------------------------*/
typedef uint32_t    pool_item_start_mark_t;
typedef uint32_t    pool_item_end_mark_t;

typedef uint32_t    memory_pool_data_t;

typedef uint32_t    pool_start_mark_t;

typedef uint32_t    pools_start_mark_t;

typedef uint8_t     pool_id_t;
typedef uint8_t     item_status_t;

typedef struct memory_pool_item_start_s {
    pool_item_start_mark_t      start_mark;

    pool_id_t                   pool_id;
    item_status_t               item_status;
    uint16_t                    info[2];
} memory_pool_item_start_t;

typedef struct memory_pool_item_end_s {
    pool_item_end_mark_t        end_mark;
} memory_pool_item_end_t;

typedef struct memory_pool_item_s {
    memory_pool_item_start_t    start;
    memory_pool_data_t          data[0];
    memory_pool_item_end_t      end;
} memory_pool_item_t;

typedef struct memory_pool_s {
    pool_start_mark_t           start_mark;

    pool_id_t                   pool_id;
    uint32_t                    item_data_number;
    uint32_t                    pool_item_size;
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
static const uint32_t               MAX_POOL_ITEMS_NUMBER = 200 * 1000;
static const uint32_t               MAX_POOL_ITEM_SIZE =    100 * 1000;

static const pool_item_start_mark_t POOL_ITEM_START_MARK =  CHARS_TO_UINT32 ('P', 'I', 's', 't');
static const pool_item_end_mark_t   POOL_ITEM_END_MARK =    CHARS_TO_UINT32 ('p', 'i', 'E', 'N');

static const item_status_t          ITEM_STATUS_FREE =      'F';
static const item_status_t          ITEM_STATUS_ALLOCATED = 'a';

static const pool_start_mark_t      POOL_START_MARK =       CHARS_TO_UINT32 ('P', '_', 's', 't');

static const pools_start_mark_t     POOLS_START_MARK =      CHARS_TO_UINT32 ('P', 'S', 's', 't');

/*------------------------------------------------------------------------------*/
static inline items_group_index_t items_group_get_free_item (items_group_t *items_group)
{
    items_group_position_t  current;
    items_group_index_t     index = -1;

    /* Get current position and decrease it */
    current = __sync_fetch_and_add (&items_group->current, -1);
    if (current <= ITEMS_GROUP_POSITION_INVALID)
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
        AssertError (index > ITEMS_GROUP_INDEX_INVALID, "Index (%d) at current position (%d) is not valid!\n", current, index);

        /* Clear index at current position */
        items_group->indexes[current] = ITEMS_GROUP_INDEX_INVALID;
    }

    return (index);
}

static inline void items_group_put_free_item (items_group_t *items_group, items_group_index_t index)
{
    items_group_position_t  next;
    items_group_position_t  current = ITEMS_GROUP_POSITION_INVALID;
    items_group_index_t     index_to_add;
    items_group_index_t     index_previous;

    index_to_add = index - ITEMS_GROUP_INDEX_INVALID;

    do
    {
        /* Calculate next position */
        next = items_group->current + 1;
        /* Checks if next position is free */
        if (items_group->indexes[next] != ITEMS_GROUP_INDEX_INVALID)
        {
            MP_DEBUG(" items_group_put_free_item (items_group->indexes[next] != ITEMS_GROUP_INDEX_INVALID) %d, %d\n", next, index);
        }
        else
        {
            /* Try to write index in next position */
            index_previous = __sync_fetch_and_add (&items_group->indexes[next], index_to_add);
            /* Checks if next position was still free */
            if (index_previous != ITEMS_GROUP_INDEX_INVALID)
            {
                /* Next position was not free anymore, restore its value */
                __sync_fetch_and_add (&items_group->indexes[next], -index_to_add);
                current = ITEMS_GROUP_POSITION_INVALID;

                MP_DEBUG(" items_group_put_free_item (index_previous != ITEMS_GROUP_INDEX_INVALID) %d\n", index);
            }
            else
            {
                /* Checks if next position content is correctly set */
                if (items_group->indexes[next] != index)
                {
                    /* Next position content has been changed, restore its value */
                    __sync_fetch_and_add (&items_group->indexes[next], -index_to_add);
                    current = ITEMS_GROUP_POSITION_INVALID;

                    MP_DEBUG(" items_group_put_free_item (items_group->indexes[next] != index) %d\n", index);
                }
                else
                {
                    /* Increase current position and get it */
                    current = __sync_add_and_fetch (&items_group->current, 1);

                    if (next != current)
                    {
                        /* Current position does not match calculated next position, restore previous values */
                        __sync_fetch_and_add (&items_group->current, -1);
                        __sync_fetch_and_add (&items_group->indexes[next], -index_to_add);

                        MP_DEBUG(" items_group_put_free_item (next != current) %d\n", index);
                    }
                }
            }
        }
    } while (next != current);

    AssertFatal (current < items_group->number, "Current position (%d) is above maximum position (%d)\n", current, items_group->number);
}

/*------------------------------------------------------------------------------*/
static inline memory_pools_t *memory_pools_from_handler (memory_pools_handle_t memory_pools_handle)
{
    memory_pools_t *memory_pools;

    /* Recover memory_pools */
    memory_pools = (memory_pools_t *) memory_pools_handle;
    /* Sanity check on passed handle */
    AssertFatal (memory_pools->start_mark == POOLS_START_MARK, "Handle %p is not a valid memory pools handle, start mark is missing\n", memory_pools_handle);

    return (memory_pools);
}

static inline memory_pool_item_t *memory_pool_item_from_handler (memory_pool_item_handle_t memory_pool_item_handle)
{
    void               *address;
    memory_pool_item_t *memory_pool_item;

    /* Recover memory_pools */
    address = memory_pool_item_handle - sizeof(memory_pool_item_start_t);
    memory_pool_item = (memory_pool_item_t *) address;

    /* Sanity check on passed handle */
    AssertFatal (memory_pool_item->start.start_mark == POOL_ITEM_START_MARK, "Handle %p is not a valid memory pool item handle, start mark is missing\n", memory_pool_item);

    return (memory_pool_item);
}

static inline memory_pool_item_t *memory_pool_item_from_index (memory_pool_t *memory_pool, items_group_index_t index)
{
    void               *address;

    address = (void *) memory_pool->items;
    address += index * memory_pool->pool_item_size;

    return (address);
}

/*------------------------------------------------------------------------------*/
memory_pools_handle_t memory_pools_create (uint32_t pools_number)
{
    memory_pools_t *memory_pools;
    pool_id_t       pool;

    AssertFatal (pools_number <= MAX_POOLS_NUMBER, "Too many memory pools requested (%d/%d)\n", pools_number, MAX_POOLS_NUMBER); /* Limit to a reasonable number of pools */

    /* Allocate memory_pools */
    memory_pools = malloc (sizeof(memory_pools_t));
    AssertFatal (memory_pools != NULL, "Memory pools structure allocation failed\n");

    /* Initialize memory_pools */
    {
        memory_pools->start_mark    = POOLS_START_MARK;
        memory_pools->pools_number  = pools_number;
        memory_pools->pools_defined = 0;

        /* Allocate pools */
        memory_pools->pools         = calloc (pools_number, sizeof(memory_pool_t));
        AssertFatal (memory_pools->pools != NULL, "Memory pools allocation failed\n");

        /* Initialize pools */
        for (pool = 0; pool < pools_number; pool++)
        {
            memory_pools->pools[pool].start_mark = POOL_START_MARK;
        }
    }

    return ((memory_pools_handle_t) memory_pools);
}

char *memory_pools_statistics(memory_pools_handle_t memory_pools_handle)
{
    memory_pools_t     *memory_pools;
    pool_id_t           pool;
    char               *statistics;
    int                 printed_chars;
    uint32_t            allocated_pool_memory;
    uint32_t            allocated_pools_memory = 0;

    /* Recover memory_pools */
    memory_pools = memory_pools_from_handler (memory_pools_handle);

    statistics = malloc(memory_pools->pools_defined * 200);

    printed_chars = sprintf (&statistics[0], "Pool: number,   size, minimum,   free\n");
    for (pool = 0; pool < memory_pools->pools_defined; pool++)
    {
        allocated_pool_memory = memory_pools->pools[pool].items_group_free.number * memory_pools->pools[pool].pool_item_size;
        allocated_pools_memory += allocated_pool_memory;
        printed_chars += sprintf (&statistics[printed_chars], "  %2u: %6u, %6lu,  %6u, %6u, %6u Kbytes\n",
                                  pool,
                                  memory_pools->pools[pool].items_group_free.number,
                                  memory_pools->pools[pool].item_data_number * sizeof(memory_pool_data_t),
                                  memory_pools->pools[pool].items_group_free.minimum + 1,
                                  memory_pools->pools[pool].items_group_free.current + 1,
                                  allocated_pool_memory / (1024));
    }
    printed_chars = sprintf (&statistics[printed_chars], "Pools memory %u Kbytes\n", allocated_pools_memory / (1024));

    return (statistics);
}

int memory_pools_add_pool (memory_pools_handle_t memory_pools_handle, uint32_t pool_items_number, uint32_t pool_item_size)
{
    memory_pools_t     *memory_pools;
    memory_pool_t      *memory_pool;
    pool_id_t           pool;
    items_group_index_t item_index;
    memory_pool_item_t *memory_pool_item;

    AssertFatal (pool_items_number <= MAX_POOL_ITEMS_NUMBER, "Too many items for a memory pool (%u/%d)\n", pool_items_number, MAX_POOL_ITEMS_NUMBER); /* Limit to a reasonable number of items */
    AssertFatal (pool_item_size <= MAX_POOL_ITEM_SIZE, "Item size is too big for memory pool items (%u/%d)\n", pool_item_size, MAX_POOL_ITEM_SIZE);   /* Limit to a reasonable item size */

    /* Recover memory_pools */
    memory_pools = memory_pools_from_handler (memory_pools_handle);

    /* Check number of already created pools */
    AssertFatal (memory_pools->pools_defined < memory_pools->pools_number, "Can not allocate more memory pool (%d)\n", memory_pools->pools_number);

    /* Select pool */
    pool = memory_pools->pools_defined;
    memory_pool = &memory_pools->pools[pool];

    /* Initialize pool */
    {
        memory_pool->pool_id                  = pool;
        /* Item size in memory_pool_data_t items by excess */
        memory_pool->item_data_number         = (pool_item_size + sizeof(memory_pool_data_t) - 1) / sizeof(memory_pool_data_t);
        memory_pool->pool_item_size           = (memory_pool->item_data_number * sizeof(memory_pool_data_t)) + sizeof(memory_pool_item_t);
        memory_pool->items_group_free.number  = pool_items_number;
        memory_pool->items_group_free.current = pool_items_number - 1;
        memory_pool->items_group_free.minimum = pool_items_number - 1;

        /* Allocate free indexes */
        memory_pool->items_group_free.indexes = malloc(pool_items_number * sizeof(items_group_index_t));
        AssertFatal (memory_pool->items_group_free.indexes != NULL, "Memory pool indexes allocation failed\n");

        /* Initialize free indexes */
        for (item_index = 0; item_index < pool_items_number; item_index++)
        {
            memory_pool->items_group_free.indexes[item_index] = item_index;
        }

        /* Allocate items */
        memory_pool->items = calloc (pool_items_number, memory_pool->pool_item_size);
        AssertFatal (memory_pool->items != NULL, "Memory pool items allocation failed\n");

        /* Initialize items */
        for (item_index = 0; item_index < pool_items_number; item_index++)
        {
            memory_pool_item                                      = memory_pool_item_from_index (memory_pool, item_index);
            memory_pool_item->start.start_mark                    = POOL_ITEM_START_MARK;
            memory_pool_item->start.pool_id                       = pool;
            memory_pool_item->start.item_status                   = ITEM_STATUS_FREE;
            memory_pool_item->data[memory_pool->item_data_number] = POOL_ITEM_END_MARK;
        }
    }

    memory_pools->pools_defined ++;

    return (0);
}

memory_pool_item_handle_t memory_pools_allocate (memory_pools_handle_t memory_pools_handle, uint32_t item_size, uint16_t info_0, uint16_t info_1)
{
    memory_pools_t             *memory_pools;
    memory_pool_item_t         *memory_pool_item;
    memory_pool_item_handle_t   memory_pool_item_handle = NULL;
    pool_id_t                   pool;
    items_group_index_t         item_index = ITEMS_GROUP_INDEX_INVALID;

#if defined(OAI_EMU) || defined(RTAI)
    vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLE_MP_ALLOC,
                                            __sync_or_and_fetch (&vcd_mp_alloc, 1L << info_0));
#endif

    /* Recover memory_pools */
    memory_pools = memory_pools_from_handler (memory_pools_handle);

    for (pool = 0; pool < memory_pools->pools_defined; pool++)
    {
        if ((memory_pools->pools[pool].item_data_number * sizeof(memory_pool_data_t)) < item_size)
        {
            /* This memory pool has too small items, skip it */
            continue;
        }

        item_index = items_group_get_free_item(&memory_pools->pools[pool].items_group_free);
        if (item_index <= ITEMS_GROUP_INDEX_INVALID)
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

    if (item_index > ITEMS_GROUP_INDEX_INVALID)
    {
        /* Convert item index into memory_pool_item address */
        memory_pool_item                    = memory_pool_item_from_index (&memory_pools->pools[pool], item_index);
        /* Sanity check on item status, must be free */
        AssertFatal (memory_pool_item->start.item_status == ITEM_STATUS_FREE, "Item status is not set to free (%d) in pool %u, item %d\n",
                     memory_pool_item->start.item_status, pool, item_index);

        memory_pool_item->start.item_status = ITEM_STATUS_ALLOCATED;
        memory_pool_item->start.info[0]     = info_0;
        memory_pool_item->start.info[1]     = info_1;
        memory_pool_item_handle             = memory_pool_item->data;

        MP_DEBUG(" Alloc [%2u][%6d]{%6d}, %3u %3u, %6u, %p, %p, %p\n",
                 pool, item_index,
                 memory_pools->pools[pool].items_group_free.current,
                 info_0, info_1,
                 item_size,
                 memory_pools->pools[pool].items,
                 memory_pool_item,
                 memory_pool_item_handle);
    }
    else
    {
        MP_DEBUG(" Alloc [--][------]{------}, %3u %3u, %6u, failed!\n", info_0, info_1, item_size);
    }

#if defined(OAI_EMU) || defined(RTAI)
    vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLE_MP_ALLOC,
                                            __sync_and_and_fetch (&vcd_mp_alloc, ~(1L << info_0)));
#endif

    return memory_pool_item_handle;
}

void memory_pools_free (memory_pools_handle_t memory_pools_handle, memory_pool_item_handle_t memory_pool_item_handle, uint16_t info_0)
{
    memory_pools_t     *memory_pools;
    memory_pool_item_t *memory_pool_item;
    pool_id_t           pool;
    items_group_index_t item_index;
    uint32_t            item_size;
    uint32_t            pool_item_size;
    uint16_t            info_1;

    /* Recover memory_pools */
    memory_pools = memory_pools_from_handler (memory_pools_handle);
    /* Recover memory pool item */
    memory_pool_item = memory_pool_item_from_handler (memory_pool_item_handle);
    info_1 = memory_pool_item->start.info[1];

#if defined(OAI_EMU) || defined(RTAI)
    vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLE_MP_FREE,
                                            __sync_or_and_fetch (&vcd_mp_free, 1L << info_1));
#endif

    /* Recover pool index */
    pool = memory_pool_item->start.pool_id;
    AssertFatal (pool < memory_pools->pools_defined, "Pool index is invalid (%u/%u)\n", pool, memory_pools->pools_defined);

    item_size = memory_pools->pools[pool].item_data_number;
    pool_item_size = memory_pools->pools[pool].pool_item_size;
    item_index = (((void *) memory_pool_item) - ((void *) memory_pools->pools[pool].items)) / pool_item_size;

    MP_DEBUG(" Free  [%2u][%6d]{%6d}, %3u %3u,         %p, %p, %p, %lu\n",
             pool, item_index, memory_pools->pools[pool].items_group_free.current,
             memory_pool_item->start.info[0], info_1,
             memory_pool_item_handle, memory_pool_item,
             memory_pools->pools[pool].items, item_size * sizeof(memory_pool_data_t));

    /* Sanity check on calculated item index */
    AssertFatal (memory_pool_item == memory_pool_item_from_index(&memory_pools->pools[pool], item_index), "Incorrect memory pool item address (%p, %p) for pool %u, item %d\n",
                 memory_pool_item, memory_pool_item_from_index(&memory_pools->pools[pool], item_index), pool, item_index);
    /* Sanity check on end marker, must still be present (no write overflow) */
    AssertFatal (memory_pool_item->data[item_size] == POOL_ITEM_END_MARK, "Memory pool item is corrupted, end mark is not present for pool %u, item %d\n", pool, item_index);
    /* Sanity check on item status, must be allocated */
    AssertFatal (memory_pool_item->start.item_status == ITEM_STATUS_ALLOCATED, "Trying to free a non allocated (%x) memory pool item (pool %u, item %d)\n",
                 memory_pool_item->start.item_status, pool, item_index);

    memory_pool_item->start.item_status = ITEM_STATUS_FREE;

    items_group_put_free_item(&memory_pools->pools[pool].items_group_free, item_index);

#if defined(OAI_EMU) || defined(RTAI)
    vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLE_MP_FREE,
                                            __sync_and_and_fetch (&vcd_mp_free, ~(1L << info_1)));
#endif
}

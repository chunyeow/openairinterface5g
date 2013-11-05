#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "ui_callbacks.h"
#include "ui_filters.h"
#include "rc.h"

const uint32_t FILTER_ALLOC_NUMBER = 100;

ui_filters_t ui_filters;

static int ui_init_filter(ui_filter_t *filter, int reset, int clear_ids, char *name)
{
    if (filter->items == NULL)
    {
        filter->name = name;

        /* Allocate some filter entries */
        filter->items = malloc (FILTER_ALLOC_NUMBER * sizeof(ui_filter_item_t));
        filter->allocated = FILTER_ALLOC_NUMBER;
    }
    if (reset)
    {
        /* Reset number of used filter entries */
        filter->used = 0;
    }
    else
    {
        if (clear_ids)
        {
            /* Clear entries IDs */
            int item;

            for (item = 0; item < filter->used; item++)
            {
                filter->items[item].id = ~0;
            }
        }
    }

    return (RC_OK);
}

int ui_init_filters(int reset, int clear_ids)
{
    ui_init_filter (&ui_filters.messages, reset, clear_ids, "messages");
    ui_init_filter (&ui_filters.origin_tasks, reset, clear_ids, "origin_tasks");
    ui_init_filter (&ui_filters.destination_tasks, reset, clear_ids, "destination_tasks");

    ui_destroy_filter_menus();

    return (RC_OK);
}

static int ui_search_name(ui_filter_t *filter, char *name)
{
    int item;

    for (item = 0; item < filter->used; item++)
    {
        if (strncmp (name, filter->items[item].name, SIGNAL_NAME_LENGTH) == 0)
        {
            return (item);
        }
    }

    return (item);
}

static int ui_filter_add(ui_filter_t *filter, uint32_t value, char *name)
{
    int item = ui_search_name (filter, name);

    if (item >= filter->allocated)
    {
        /* Increase number of filter entries */
        filter->items = realloc (filter->items, (filter->allocated + FILTER_ALLOC_NUMBER) * sizeof(ui_filter_item_t));
        filter->allocated += FILTER_ALLOC_NUMBER;
    }

    filter->items[item].id = value;
    if (item >= filter->used)
    {
        /* New entry */
        strncpy(filter->items[item].name, name, SIGNAL_NAME_LENGTH);
        filter->items[item].enabled = TRUE;

        filter->used++;
    }

    return (item);
}

void ui_filters_add(ui_filter_e filter, uint32_t value, char *name)
{
    switch (filter)
    {
        case FILTER_MESSAGES:
            ui_filter_add (&ui_filters.messages, value, name);
            break;

        case FILTER_ORIGIN_TASKS:
            ui_filter_add (&ui_filters.origin_tasks, value, name);
            break;

        case FILTER_DESTINATION_TASKS:
            ui_filter_add (&ui_filters.destination_tasks, value, name);
            break;

        default:
            g_warning("unknown filter type %d", filter);
            break;
    }
}

static void write_filter(FILE *filter_file, ui_filter_t *filter)
{
    int item;

    fprintf (filter_file, "  <%s>\n", filter->name);
    for (item = 0; item < filter->used; item++)
    {
        fprintf (filter_file, "    %s=\"%d\"\n", filter->items[item].name, filter->items[item].enabled ? 1 :0);
    }
    fprintf (filter_file, "  </%s>\n", filter->name);
}

int ui_write_filters_file(char *file_name)
{
    FILE *filter_file;

    // types_t *types;

    filter_file = fopen (file_name, "w");
    if (filter_file == NULL)
    {
        g_warning("Failed to open file \"%s\": %s", file_name, g_strerror (errno));
        return RC_FAIL;
    }

    fprintf (filter_file, "<filters>\n");

    write_filter (filter_file, &ui_filters.messages);
    write_filter (filter_file, &ui_filters.origin_tasks);
    write_filter (filter_file, &ui_filters.destination_tasks);

    fprintf (filter_file, "</filters>\n");

    fclose (filter_file);
    return RC_OK;
}

#include <stdlib.h>
#include <stdint.h>

#include <gtk/gtk.h>

#include "rc.h"

#include "buffers.h"

#include "ui_main_screen.h"
#include "ui_tree_view.h"
#include "ui_callbacks.h"
#include "ui_filters.h"

#include "ui_signal_dissect_view.h"

typedef struct
{
    GtkListStore *store;
    GtkTreeModelFilter *filtered;
    uint32_t filtered_last_msg;
    uint32_t filtered_msg_number;
    uint32_t instance_number;
} ui_store_t;

static ui_store_t ui_store;

GdkEventButton *ui_tree_view_last_event;

static gboolean ui_tree_filter_messages(GtkTreeModel *model, GtkTreeIter *iter, ui_store_t *store)
{
    uint32_t msg_number;
    uint32_t message_id;
    uint32_t origin_task_id;
    uint32_t destination_task_id;
    uint32_t instance;

    gboolean enabled = FALSE;

    gtk_tree_model_get (model, iter, COL_MSG_NUM, &msg_number, COL_MESSAGE_ID, &message_id, COL_FROM_TASK_ID, &origin_task_id,
                        COL_TO_TASK_ID, &destination_task_id, COL_INSTANCE, &instance, -1);
    if (msg_number != 0)
    {
        enabled = ui_filters_message_enabled (message_id, origin_task_id, destination_task_id, instance);

        if ((enabled) && (ui_store.filtered_last_msg < msg_number))
        {
            ui_store.filtered_last_msg = msg_number;
            ui_store.filtered_msg_number++;
        }
        g_debug("%p %p %d m:%d o:%d d:%d i:%d %d %d", model, iter, msg_number, message_id, origin_task_id, destination_task_id, instance, enabled, ui_store.filtered_msg_number);
    }

    return enabled;
}

static gboolean onButtonPressed(GtkWidget *treeview, GdkEventButton *event, gpointer userdata)
{
    g_debug("onButtonPressed %p %p %p %d %d", treeview, event, userdata, event->type, event->button);
    ui_tree_view_last_event = event;

    return FALSE;
}

/*
static gboolean onButtonRelease(GtkWidget *treeview, GdkEventButton *event, gpointer userdata)
{
    g_debug("onButtonRelease %p %p %p %d %d", treeview, event, userdata, event->type, event->button);
    // last_event = event;

    return FALSE;
}
*/

static void
ui_tree_view_init_list(GtkWidget *list)
{
    GtkCellRenderer *renderer_left;
    GtkCellRenderer *renderer_right;
    GtkTreeViewColumn *column;

    renderer_left = gtk_cell_renderer_text_new();
    gtk_cell_renderer_set_padding (renderer_left, 5, 0);

    renderer_right = gtk_cell_renderer_text_new();
    gtk_cell_renderer_set_alignment (renderer_right, 1, 0.5);
    gtk_cell_renderer_set_padding (renderer_right, 5, 0);

    column = gtk_tree_view_column_new_with_attributes(
        "MN", renderer_right, "text", COL_MSG_NUM, NULL);
    gtk_tree_view_column_set_resizable (column, TRUE);
    gtk_tree_view_column_set_alignment (column, 0.5);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

    column = gtk_tree_view_column_new_with_attributes(
        "LTE Time", renderer_right, "text", COL_LTE_TIME, NULL);
    gtk_tree_view_column_set_resizable (column, TRUE);
    gtk_tree_view_column_set_alignment (column, 0.5);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

    column = gtk_tree_view_column_new_with_attributes(
        "Signal", renderer_left, "text", COL_MESSAGE, NULL);
    gtk_tree_view_column_set_resizable (column, TRUE);
    gtk_tree_view_column_set_alignment (column, 0.5);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);
    g_signal_connect(G_OBJECT(column), "clicked",
                     G_CALLBACK(ui_callback_on_tree_column_header_click), (gpointer) COL_MESSAGE);

    column = gtk_tree_view_column_new_with_attributes(
        "From", renderer_left, "text", COL_FROM_TASK, NULL);
    gtk_tree_view_column_set_alignment (column, 0.5);
    gtk_tree_view_column_set_resizable (column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);
    g_signal_connect(G_OBJECT(column), "clicked",
                     G_CALLBACK(ui_callback_on_tree_column_header_click), (gpointer) COL_FROM_TASK);

    column = gtk_tree_view_column_new_with_attributes(
        "To", renderer_left, "text", COL_TO_TASK, NULL);
    gtk_tree_view_column_set_alignment (column, 0.5);
    gtk_tree_view_column_set_resizable (column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);
    g_signal_connect(G_OBJECT(column), "clicked",
                     G_CALLBACK(ui_callback_on_tree_column_header_click), (gpointer) COL_TO_TASK);

    column = gtk_tree_view_column_new_with_attributes(
        "Ins", renderer_right, "text", COL_INSTANCE, NULL);
    gtk_tree_view_column_set_alignment (column, 0.5);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);
    g_signal_connect(G_OBJECT(column), "clicked",
                     G_CALLBACK(ui_callback_on_tree_column_header_click), (gpointer) COL_INSTANCE);

    ui_store.store = gtk_list_store_new(NUM_COLS,
                                       G_TYPE_UINT,   // COL_MSG_NUM
                                       G_TYPE_STRING, // COL_LTE_TIME
                                       G_TYPE_STRING, // COL_SIGNAL
                                       G_TYPE_STRING, // COL_FROM_TASK
                                       G_TYPE_STRING, // COL_TO_TASK
                                       G_TYPE_UINT,   // COL_INSTANCE
                                       /* HACK: add id of message fields to speed-up filtering
                                        */
                                       G_TYPE_UINT, // COL_SIGNAL_ID
                                       G_TYPE_UINT, // COL_FROM_TASK_ID
                                       G_TYPE_UINT, // COL_TO_TASK_ID
                                       /* HACK: add a reference to the buffer here
                                        * to avoid maintining multiple lists.
                                        * The reference is not displayed
                                        */
                                       G_TYPE_POINTER);

    if(ui_store.store == NULL)
    {
        g_error("gtk_list_store_new failed");
    }

    ui_store.filtered = GTK_TREE_MODEL_FILTER (gtk_tree_model_filter_new (GTK_TREE_MODEL (ui_store.store), NULL));
    gtk_tree_model_filter_set_visible_func (ui_store.filtered,
                                            (GtkTreeModelFilterVisibleFunc) ui_tree_filter_messages,
                                            &ui_store, NULL);

    gtk_tree_view_set_model(GTK_TREE_VIEW(list), GTK_TREE_MODEL(ui_store.filtered));

    gtk_tree_view_columns_autosize(GTK_TREE_VIEW(list));
}

static void ui_tree_view_add_to_list(GtkWidget *list, const gchar *lte_time, const uint32_t message_number, const uint32_t message_id, const gchar *signal_name,
                                     const uint32_t origin_task_id, const char *origin_task,
                                     const uint32_t destination_task_id, const char *destination_task, uint32_t instance, gpointer buffer)
{
    GtkTreeIter iter;

    gtk_list_store_append(ui_store.store, &iter);
    gtk_list_store_set(ui_store.store, &iter,
                       /* Columns */
                       COL_MSG_NUM      , message_number,
                       COL_LTE_TIME     , lte_time,
                       COL_MESSAGE      , signal_name,
                       COL_FROM_TASK    , origin_task,
                       COL_TO_TASK      , destination_task,
                       COL_INSTANCE     , instance,
                       COL_MESSAGE_ID   , message_id,
                       COL_FROM_TASK_ID , origin_task_id,
                       COL_TO_TASK_ID   , destination_task_id,
                       COL_BUFFER       , buffer,
                       /* End of columns */
                       -1);
}

void ui_tree_view_destroy_list(GtkWidget *list)
{
    g_assert(list != NULL);

    gtk_list_store_clear(ui_store.store);
    ui_store.filtered_last_msg = 0;
    ui_store.filtered_msg_number = 0;
    ui_store.instance_number = 0;

    /* Reset number of messages */
    ui_main_data.nb_message_received = 0;
}

/* Search for the message with its message number equal to the given value or the previous one */
static gboolean ui_tree_view_search (GtkTreeModel *model, gint column, const gchar *key, GtkTreeIter *iter,
        gpointer search_data)
{
    uint32_t msg_number;
    uint32_t key_value = atoi(key);

    gtk_tree_model_get (model, iter, column, &msg_number, -1);

    g_debug ("ui_tree_view_search %d %d", key_value, msg_number);

    if (key_value == msg_number)
    {
        /* Value found, use this message */
        return 0;
    }

    {
        GtkTreeIter iter_next = *iter;

        if (gtk_tree_model_iter_next(model, &iter_next))
        {
            gtk_tree_model_get (model, &iter_next, column, &msg_number, -1);

            if (key_value < msg_number)
            {
                /* Next value will be greater, use this message */
                return 0;
            }
        }
        else
        {
            /* Last value, use this message */
            return 0;
        }
    }
    return 1;
}

int ui_tree_view_create(GtkWidget *window, GtkWidget *vbox)
{
    GtkWidget *hbox;
    GtkTreeSelection *selection;
    GtkWidget *scrolled_window;

    scrolled_window = gtk_scrolled_window_new(NULL, NULL);

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

    ui_main_data.messages_list = gtk_tree_view_new();
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(ui_main_data.messages_list), TRUE);
    gtk_tree_view_set_search_equal_func (GTK_TREE_VIEW(ui_main_data.messages_list), ui_tree_view_search, NULL, NULL);
    gtk_tree_view_set_search_entry (GTK_TREE_VIEW(ui_main_data.messages_list), GTK_ENTRY(ui_main_data.signals_go_to_entry));

    /* Disable multiple selection */
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(ui_main_data.messages_list));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_BROWSE);

    hbox = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);

    gtk_container_add(GTK_CONTAINER(scrolled_window), ui_main_data.messages_list);

    ui_tree_view_init_list(ui_main_data.messages_list);
    gtk_tree_view_set_headers_clickable(GTK_TREE_VIEW(ui_main_data.messages_list), TRUE);

    gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scrolled_window), 620);
    gtk_paned_pack1 (GTK_PANED (hbox), scrolled_window, FALSE, TRUE);
    ui_main_data.text_view = ui_signal_dissect_new(hbox);

    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 5);

    g_signal_connect(G_OBJECT(ui_main_data.messages_list), "button-press-event", G_CALLBACK (onButtonPressed), NULL);
    // g_signal_connect(G_OBJECT(ui_main_data.signalslist), "button-release-event", G_CALLBACK (onButtonRelease), NULL);

    /* Connect callback on row selection */
    gtk_tree_selection_set_select_function(selection, ui_callback_on_select_signal,
                                           ui_main_data.text_view, NULL);

    ui_store.filtered_last_msg = 0;
    ui_store.filtered_msg_number = 0;
    ui_store.instance_number = 0;

    ui_main_data.selection = selection;

    return 0;
}

int ui_tree_view_new_signal_ind(const uint32_t message_number, const gchar *lte_time,
                                const uint32_t message_id, const char *message_name,
                                const uint32_t origin_task_id, const char *origin_task,
                                const uint32_t destination_task_id, const char *destination_task,
                                uint32_t instance, gpointer buffer)
{
    if(ui_store.instance_number < (instance + 1))
    {
        int i;
        char name[10];

        for (i = ui_store.instance_number; i <= instance; i++)
        {
            sprintf (name, "%d", i);
            ui_filters_add (FILTER_INSTANCES, i, name, ENTRY_ENABLED_TRUE);
        }
        ui_store.instance_number = (instance + 1);
        ui_destroy_filter_menu(FILTER_INSTANCES);
    }

    ui_tree_view_add_to_list(ui_main_data.messages_list, lte_time, message_number, message_id, message_name,
                             origin_task_id, origin_task, destination_task_id, destination_task, instance, (buffer_t *)buffer);

    return RC_OK;
}

void ui_tree_view_select_row(gint row)
{
    GtkTreePath *path_row;

    path_row = gtk_tree_path_new_from_indices(row, -1);

    if (path_row != NULL)
    {
        /* Select the message in requested row */
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(ui_main_data.messages_list), path_row, NULL, FALSE);
        /* Center the message in the middle of the list if possible */
        gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(ui_main_data.messages_list), path_row, NULL, TRUE, 0.5, 0.0);
    }
}

void ui_tree_view_refilter(void)
{
    ui_store.filtered_last_msg = 0;
    ui_store.filtered_msg_number = 0;
    if (ui_store.filtered != NULL)
    {
        gtk_tree_model_filter_refilter (ui_store.filtered);
    }
}

guint ui_tree_view_get_filtered_number(void)
{
    return ui_store.filtered_msg_number;
}

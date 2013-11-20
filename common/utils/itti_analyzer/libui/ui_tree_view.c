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

static GValue colors[] =
    { G_VALUE_INIT, G_VALUE_INIT };

GdkEventButton *ui_tree_view_last_event;

static gboolean ui_tree_filter_messages(GtkTreeModel *model, GtkTreeIter *iter, ui_store_t *store)
{
    uint32_t msg_number;
    uint32_t message_id;
    uint32_t origin_task_id;
    uint32_t destination_task_id;
    uint32_t instance;

    gboolean enabled = FALSE;

    gtk_tree_model_get (model, iter, COL_MSG_NUM, &msg_number, COL_MESSAGE_ID, &message_id, COL_FROM_TASK_ID,
                        &origin_task_id, COL_TO_TASK_ID, &destination_task_id, COL_INSTANCE, &instance, -1);
    if (msg_number != 0)
    {
        enabled = ui_filters_message_enabled(message_id, origin_task_id, destination_task_id, instance);

#if  0
        if (store->filtered_last_msg < msg_number)
        {
            GtkTreePath *path = gtk_tree_model_get_path (model, iter);
            GtkTreePath *true_path = gtk_tree_model_filter_convert_path_to_child_path (GTK_TREE_MODEL_FILTER (store->filtered), path);
            g_message ("gtk_tree_model_get: %p %p", path, true_path);
            GtkTreeIter iter_;
            gtk_tree_model_get_iter (GTK_TREE_MODEL(store->store), &iter_, true_path);

            GValue colors = G_VALUE_INIT;
            g_value_init (&colors, G_TYPE_STRING);
            g_value_set_string (&colors, (enabled ? "Black" : "Grey"));

            // gtk_list_store_set_value (ui_store.store, &iter_, COL_FOREGROUND, &gvalue);
            // gtk_list_store_set (ui_store.store, &iter_, COL_FOREGROUND, (enabled ? "Black" : "Grey"), -1);
            // gtk_list_store_set_value (GTK_LIST_STORE(model), iter, COL_FOREGROUND, &colors);
        }
#endif

        enabled |= (!ui_filters.filters_enabled);

        if ((enabled) && (store->filtered_last_msg < msg_number))
        {
            store->filtered_last_msg = msg_number;
            store->filtered_msg_number++;
        }
        g_debug("ui_tree_filter_messages: %p %p %d m:%d o:%d d:%d i:%d %d %d", model, iter, msg_number, message_id, origin_task_id, destination_task_id, instance, enabled, store->filtered_msg_number);
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

static void ui_tree_view_init_list(GtkWidget *list)
{
    GtkCellRenderer *renderer_left;
    GtkCellRenderer *renderer_right;
    GtkTreeViewColumn *column;

    renderer_left = gtk_cell_renderer_text_new ();
    gtk_cell_renderer_set_padding (renderer_left, 5, 0);

    renderer_right = gtk_cell_renderer_text_new ();
    gtk_cell_renderer_set_alignment (renderer_right, 1, 0.5);
    gtk_cell_renderer_set_padding (renderer_right, 5, 0);

    column = gtk_tree_view_column_new_with_attributes ("MN", renderer_right, "text", COL_MSG_NUM, "foreground", COL_FOREGROUND, NULL);
    gtk_tree_view_column_set_resizable (column, TRUE);
    gtk_tree_view_column_set_alignment (column, 0.5);
    gtk_tree_view_append_column (GTK_TREE_VIEW(list), column);

    column = gtk_tree_view_column_new_with_attributes ("LTE Time", renderer_right, "text", COL_LTE_TIME, NULL);
    gtk_tree_view_column_set_resizable (column, TRUE);
    gtk_tree_view_column_set_alignment (column, 0.5);
    gtk_tree_view_append_column (GTK_TREE_VIEW(list), column);

    column = gtk_tree_view_column_new_with_attributes ("Signal", renderer_left, "text", COL_MESSAGE, "foreground", COL_FOREGROUND, NULL);
    gtk_tree_view_column_set_resizable (column, TRUE);
    gtk_tree_view_column_set_alignment (column, 0.5);
    gtk_tree_view_append_column (GTK_TREE_VIEW(list), column);
    g_signal_connect(G_OBJECT(column), "clicked", G_CALLBACK(ui_callback_on_tree_column_header_click),
                     (gpointer) COL_MESSAGE);

    column = gtk_tree_view_column_new_with_attributes ("From", renderer_left, "text", COL_FROM_TASK, NULL);
    gtk_tree_view_column_set_alignment (column, 0.5);
    gtk_tree_view_column_set_resizable (column, TRUE);
    gtk_tree_view_append_column (GTK_TREE_VIEW(list), column);
    g_signal_connect(G_OBJECT(column), "clicked", G_CALLBACK(ui_callback_on_tree_column_header_click),
                     (gpointer) COL_FROM_TASK);

    column = gtk_tree_view_column_new_with_attributes ("To", renderer_left, "text", COL_TO_TASK, NULL);
    gtk_tree_view_column_set_alignment (column, 0.5);
    gtk_tree_view_column_set_resizable (column, TRUE);
    gtk_tree_view_append_column (GTK_TREE_VIEW(list), column);
    g_signal_connect(G_OBJECT(column), "clicked", G_CALLBACK(ui_callback_on_tree_column_header_click),
                     (gpointer) COL_TO_TASK);

    column = gtk_tree_view_column_new_with_attributes ("Ins", renderer_right, "text", COL_INSTANCE, NULL);
    gtk_tree_view_column_set_alignment (column, 0.5);
    gtk_tree_view_append_column (GTK_TREE_VIEW(list), column);
    g_signal_connect(G_OBJECT(column), "clicked", G_CALLBACK(ui_callback_on_tree_column_header_click),
                     (gpointer) COL_INSTANCE);

    ui_store.store = gtk_list_store_new (NUM_COLS, // Number of columns
            // Displayed columns
            G_TYPE_UINT, // COL_MSG_NUM
            G_TYPE_STRING, // COL_LTE_TIME
            G_TYPE_STRING, // COL_MESSAGE
            G_TYPE_STRING, // COL_FROM_TASK
            G_TYPE_STRING, // COL_TO_TASK
            G_TYPE_UINT, // COL_INSTANCE
            // Not displayed columns
            // Id of some message fields to speed-up filtering
            G_TYPE_UINT, // COL_MESSAGE_ID
            G_TYPE_UINT, // COL_FROM_TASK_ID
            G_TYPE_UINT, // COL_TO_TASK_ID
            G_TYPE_STRING, // COL_FOREGROUND
            // Reference to the buffer here to avoid maintaining multiple lists.
            G_TYPE_POINTER);

    if (ui_store.store == NULL)
    {
        g_error("gtk_list_store_new failed");
    }

    ui_store.filtered = GTK_TREE_MODEL_FILTER (gtk_tree_model_filter_new (GTK_TREE_MODEL (ui_store.store), NULL));
    gtk_tree_model_filter_set_visible_func (ui_store.filtered, (GtkTreeModelFilterVisibleFunc) ui_tree_filter_messages,
                                            &ui_store, NULL);

    gtk_tree_view_set_model (GTK_TREE_VIEW(list), GTK_TREE_MODEL(ui_store.filtered));

    gtk_tree_view_columns_autosize (GTK_TREE_VIEW(list));
}

static void ui_tree_view_add_to_list(GtkWidget *list, const gchar *lte_time, const uint32_t message_number,
                                     const uint32_t message_id, const gchar *signal_name, const uint32_t origin_task_id,
                                     const char *origin_task, const uint32_t destination_task_id,
                                     const char *destination_task, uint32_t instance, gpointer buffer)
{
    GtkTreeIter iter;
    gboolean enabled;

    enabled = ui_filters_message_enabled(message_id, origin_task_id, destination_task_id, instance);

    gtk_list_store_append (ui_store.store, &iter);
    gtk_list_store_set (ui_store.store, &iter,
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
    gtk_list_store_set_value (ui_store.store, &iter, COL_FOREGROUND, &colors[enabled ? 1 : 0]);
}

void ui_tree_view_destroy_list(GtkWidget *list)
{
    g_assert(list != NULL);

    gtk_list_store_clear (ui_store.store);
    ui_store.filtered_last_msg = 0;
    ui_store.filtered_msg_number = 0;
    ui_store.instance_number = 0;

    /* Reset number of messages */
    ui_main_data.nb_message_received = 0;
}

/* Search for the message with its message number equal to the given value or the previous one */
static gboolean ui_tree_view_search(GtkTreeModel *model, gint column, const gchar *key, GtkTreeIter *iter,
                                    gpointer search_data)
{
    uint32_t msg_number;
    uint32_t key_value = atoi (key);

    gtk_tree_model_get (model, iter, column, &msg_number, -1);

    g_debug("ui_tree_view_search %d %d", key_value, msg_number);

    if (key_value == msg_number)
    {
        /* Value found, use this message */
        return 0;
    }

    {
        GtkTreeIter iter_next = *iter;

        if (gtk_tree_model_iter_next (model, &iter_next))
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

    g_value_init (&colors[0], G_TYPE_STRING);
    g_value_init (&colors[1], G_TYPE_STRING);
    g_value_set_string (&colors[0], "Grey");
    g_value_set_string (&colors[1], "Black");

    scrolled_window = gtk_scrolled_window_new (NULL, NULL);

    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

    ui_main_data.messages_list = gtk_tree_view_new ();
    gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(ui_main_data.messages_list), TRUE);
    gtk_tree_view_set_search_equal_func (GTK_TREE_VIEW(ui_main_data.messages_list), ui_tree_view_search, NULL, NULL);
    gtk_tree_view_set_search_entry (GTK_TREE_VIEW(ui_main_data.messages_list),
                                    GTK_ENTRY(ui_main_data.signals_go_to_entry));

    /* Disable multiple selection */
    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(ui_main_data.messages_list));
    gtk_tree_selection_set_mode (selection, GTK_SELECTION_BROWSE);

    hbox = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);

    gtk_container_add (GTK_CONTAINER(scrolled_window), ui_main_data.messages_list);

    ui_tree_view_init_list (ui_main_data.messages_list);
    gtk_tree_view_set_headers_clickable (GTK_TREE_VIEW(ui_main_data.messages_list), TRUE);

    gtk_scrolled_window_set_min_content_width (GTK_SCROLLED_WINDOW(scrolled_window), 620);
    gtk_paned_pack1 (GTK_PANED (hbox), scrolled_window, FALSE, TRUE);
    ui_main_data.text_view = ui_signal_dissect_new (hbox);

    gtk_box_pack_start (GTK_BOX(vbox), hbox, TRUE, TRUE, 5);

    g_signal_connect(G_OBJECT(ui_main_data.messages_list), "button-press-event", G_CALLBACK (onButtonPressed), NULL);
    // g_signal_connect(G_OBJECT(ui_main_data.signalslist), "button-release-event", G_CALLBACK (onButtonRelease), NULL);

    /* Connect callback on row selection */
    gtk_tree_selection_set_select_function (selection, ui_callback_on_select_signal, ui_main_data.text_view, NULL);

    ui_store.filtered_last_msg = 0;
    ui_store.filtered_msg_number = 0;
    ui_store.instance_number = 0;

    ui_main_data.selection = selection;

    return 0;
}

int ui_tree_view_new_signal_ind(const uint32_t message_number, const gchar *lte_time, const uint32_t message_id,
                                const char *message_name, const uint32_t origin_task_id, const char *origin_task,
                                const uint32_t destination_task_id, const char *destination_task, uint32_t instance,
                                gpointer buffer)
{
    if (ui_store.instance_number < (instance + 1))
    {
        int i;
        char name[10];

        for (i = ui_store.instance_number; i <= instance; i++)
        {
            sprintf (name, "%d", i);
            ui_filters_add (FILTER_INSTANCES, i, name, ENTRY_ENABLED_TRUE);
        }
        ui_store.instance_number = (instance + 1);
        ui_destroy_filter_menu (FILTER_INSTANCES);
    }

    ui_tree_view_add_to_list (ui_main_data.messages_list, lte_time, message_number, message_id, message_name,
                              origin_task_id, origin_task, destination_task_id, destination_task, instance,
                              (buffer_t *) buffer);

    return RC_OK;
}

void ui_tree_view_select_row(gint row)
{
    GtkTreePath *path_row;

    if (row >= 0)
    {
        path_row = gtk_tree_path_new_from_indices (row, -1);

        if ((ui_main_data.messages_list != NULL) && (path_row != NULL))
        {
            g_debug("Select row %d", row);

            /* Select the message in requested row */
            gtk_tree_view_set_cursor (GTK_TREE_VIEW(ui_main_data.messages_list), path_row, NULL, FALSE);
            /* Center the message in the middle of the list if possible */
            gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW(ui_main_data.messages_list), path_row, NULL, TRUE, 0.5, 0.0);
        }
    }
}

static gboolean updateForegroundColor (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
{
    uint32_t message_id;
    uint32_t origin_task_id;
    uint32_t destination_task_id;
    uint32_t instance;

    gboolean enabled = FALSE;

    gtk_tree_model_get (model, iter, COL_MESSAGE_ID, &message_id, COL_FROM_TASK_ID,
                        &origin_task_id, COL_TO_TASK_ID, &destination_task_id, COL_INSTANCE, &instance, -1);
    enabled = ui_filters_message_enabled(message_id, origin_task_id, destination_task_id, instance);

    gtk_list_store_set_value (GTK_LIST_STORE(model), iter, COL_FOREGROUND, &colors[enabled ? 1 : 0]);

    return FALSE;
}

void ui_tree_view_refilter(void)
{
    ui_store.filtered_last_msg = 0;
    ui_store.filtered_msg_number = 0;

    /* Update foreground color of messages, this will also update filtered model */
    if (ui_store.store != NULL)
    {
        gtk_tree_model_foreach(GTK_TREE_MODEL(ui_store.store), updateForegroundColor, NULL);
    }

    g_debug("ui_tree_view_refilter: last message %d, %d messages displayed", ui_store.filtered_last_msg, ui_store.filtered_msg_number);
}

guint ui_tree_view_get_filtered_number(void)
{
    return ui_store.filtered_msg_number;
}

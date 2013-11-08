#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>
#include <stdint.h>

#include <gtk/gtk.h>

#include "rc.h"

#include "socket.h"

#include "ui_notif_dlg.h"
#include "ui_main_screen.h"
#include "ui_menu_bar.h"
#include "ui_callbacks.h"
#include "ui_interface.h"
#include "ui_notifications.h"
#include "ui_tree_view.h"
#include "ui_signal_dissect_view.h"
#include "ui_filters.h"

#include "types.h"
#include "locate_root.h"
#include "xml_parse.h"

gboolean ui_callback_on_open_messages(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    g_debug("Open messages event occurred");
    CHECK_FCT(ui_messages_open_file_chooser());

    return TRUE;
}

gboolean ui_callback_on_save_messages(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    g_debug("Save messages event occurred");
    // CHECK_FCT(ui_file_chooser());
    return TRUE;
}

gboolean ui_callback_on_open_filters(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    g_debug("Open filters event occurred");
    CHECK_FCT(ui_filters_open_file_chooser());
    return TRUE;
}

gboolean ui_callback_on_save_filters(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    g_debug("Save filters event occurred");
    CHECK_FCT(ui_filters_save_file_chooser());
    return TRUE;
}

gboolean ui_callback_on_about(GtkWidget *widget, GdkEvent *event, gpointer data)
{
#if defined(PACKAGE_STRING)
    ui_notification_dialog (GTK_MESSAGE_INFO, "about", "%s", PACKAGE_STRING);
#else
    ui_notification_dialog (GTK_MESSAGE_INFO, "about", "itti_analyzer");
#endif

    return TRUE;
}

gboolean ui_callback_on_select_signal(GtkTreeSelection *selection, GtkTreeModel *model, GtkTreePath *path,
                                      gboolean path_currently_selected, gpointer user_data)
{
    ui_text_view_t *text_view;
    GtkTreeIter iter;

    text_view = (ui_text_view_t *) user_data;

    g_assert(text_view != NULL);

    if (gtk_tree_model_get_iter (model, &iter, path))
    {
        GValue buffer_store = G_VALUE_INIT;
        gpointer buffer;

        gtk_tree_model_get_value (model, &iter, COL_BUFFER, &buffer_store);

        buffer = g_value_get_pointer (&buffer_store);

        if (!path_currently_selected)
        {
            /* Clear the view */
            CHECK_FCT_DO(ui_signal_dissect_clear_view(text_view), return FALSE);

            /* Dissect the signal */
            CHECK_FCT_DO(dissect_signal((buffer_t*)buffer, ui_signal_set_text, text_view), return FALSE);
        }
    }
    return TRUE;
}

void ui_signal_add_to_list(gpointer data, gpointer user_data)
{
    buffer_t *signal_buffer;
    GtkTreePath *path;
    GtkTreeViewColumn *focus_column;

    gtk_tree_view_get_cursor (GTK_TREE_VIEW(ui_main_data.signalslist), &path, &focus_column);

    signal_buffer = (buffer_t *) data;

    get_message_id (root, signal_buffer, &signal_buffer->message_id);

    ui_tree_view_new_signal_ind (signal_buffer->message_number, message_id_to_string (signal_buffer->message_id),
                                 get_origin_task_id (signal_buffer), get_destination_task_id (signal_buffer), data);

    /* Increment number of messages */
    ui_main_data.nb_message_received++;

    /* Check if no signal was selected in the list or if it was the last signal */
    if ((ui_main_data.path_last == NULL) || (path == NULL) || (gtk_tree_path_compare(ui_main_data.path_last, path) == 0))
    {
        /* Advance to the new last signal */
        ui_callback_signal_go_to_last (NULL, NULL, NULL);
    }
}

static gboolean ui_handle_update_signal_list(gint fd, void *data, size_t data_length)
{
    pipe_new_signals_list_message_t *signal_list_message;

    /* Enable buttons to move in the list of signals */
    ui_set_sensitive_move_buttons (TRUE);

    signal_list_message = (pipe_new_signals_list_message_t *) data;

    g_assert(signal_list_message != NULL);
    g_assert(signal_list_message->signal_list != NULL);

    g_list_foreach (signal_list_message->signal_list, ui_signal_add_to_list, NULL);

    /* Free the list but not user data associated with each element */
    g_list_free (signal_list_message->signal_list);
    /* Free the message */
    free (signal_list_message);

    ui_gtk_flush_events();

    return TRUE;
}

static gboolean ui_handle_socket_connection_failed(gint fd)
{
    GtkWidget *dialogbox;

    dialogbox = gtk_message_dialog_new (GTK_WINDOW(ui_main_data.window), GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                        "Failed to connect to provided host/ip address");

    gtk_dialog_run (GTK_DIALOG(dialogbox));
    gtk_widget_destroy (dialogbox);

    /* Re-enable connect button */
    ui_enable_connect_button ();
    return TRUE;
}

static gboolean ui_handle_socket_connection_lost(gint fd)
{
    GtkWidget *dialogbox;

    dialogbox = gtk_message_dialog_new (GTK_WINDOW(ui_main_data.window), GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                        "Connection with remote host has been lost");

    gtk_dialog_run (GTK_DIALOG(dialogbox));
    gtk_widget_destroy (dialogbox);

    /* Re-enable connect button */
    ui_enable_connect_button ();
    return TRUE;
}

static gboolean ui_handle_socket_xml_definition(gint fd, void *data, size_t data_length)
{
    pipe_xml_definition_message_t *xml_definition_message;

    xml_definition_message = (pipe_xml_definition_message_t *) data;
    g_assert(xml_definition_message != NULL);
    g_assert(data_length == sizeof(pipe_xml_definition_message_t));

    xml_parse_buffer (xml_definition_message->xml_definition, xml_definition_message->xml_definition_length);

    free (data);

    return TRUE;
}

gboolean ui_pipe_callback(gint source, gpointer user_data)
{
    void *input_data = NULL;
    size_t input_data_length = 0;
    pipe_input_header_t input_header;

    /* Read the header */
    if (read (source, &input_header, sizeof(input_header)) < 0)
    {
        g_warning("Failed to read from pipe %d: %s", source, g_strerror(errno));
        return FALSE;
    }

    input_data_length = input_header.message_size - sizeof(input_header);

    /* Checking for non-header part */
    if (input_data_length > 0)
    {
        input_data = malloc (input_data_length);

        if (read (source, input_data, input_data_length) < 0)
        {
            g_warning("Failed to read from pipe %d: %s", source, g_strerror(errno));
            return FALSE;
        }
    }

    switch (input_header.message_type)
    {
        case UI_PIPE_CONNECTION_FAILED:
            return ui_handle_socket_connection_failed (source);
        case UI_PIPE_XML_DEFINITION:
            return ui_handle_socket_xml_definition (source, input_data, input_data_length);
        case UI_PIPE_CONNECTION_LOST:
            return ui_handle_socket_connection_lost (source);
        case UI_PIPE_UPDATE_SIGNAL_LIST:
            return ui_handle_update_signal_list (source, input_data, input_data_length);
        default:
            g_warning("[gui] Unhandled message type %u", input_header.message_type);
            g_assert_not_reached();
    }
    return FALSE;
}

gboolean ui_callback_on_connect(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    /* We have to retrieve the ip address and port of remote host */
    const char *ip;
    uint16_t port;
    int pipe_fd[2];

    g_debug("Connect event occurred");

    port = atoi (gtk_entry_get_text (GTK_ENTRY(ui_main_data.port_entry)));
    ip = gtk_entry_get_text (GTK_ENTRY(ui_main_data.ip_entry));

    if ((ip == NULL) || (port == 0))
    {
        g_warning("NULL parameter given for ip address or port = 0");
        /* TODO: add dialog box here */
        return FALSE;
    }

    ui_pipe_new (pipe_fd, ui_pipe_callback, NULL);

    memcpy (ui_main_data.pipe_fd, pipe_fd, sizeof(int) * 2);

    /* Disable the connect button */
    ui_disable_connect_button ();

    ui_callback_signal_clear_list (widget, event, data);

    if (socket_connect_to_remote_host (ip, port, pipe_fd[1]) != 0)
    {
        ui_enable_connect_button ();
        return FALSE;
    }
    ui_set_title ("%s:%d", ip, port);

    return TRUE;
}

gboolean ui_callback_on_disconnect(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    /* We have to retrieve the ip address and port of remote host */

    g_debug("Disconnect event occurred");

    ui_pipe_write_message (ui_main_data.pipe_fd[0], UI_PIPE_DISCONNECT_EVT, NULL, 0);

    ui_enable_connect_button ();
    return TRUE;
}

gboolean ui_callback_signal_go_to(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    ui_tree_view_select_row (ui_main_data.nb_message_received / 2, NULL);
    return TRUE;
}

gboolean ui_callback_signal_go_to_first(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    ui_tree_view_select_row (0, NULL);
    return TRUE;
}

gboolean ui_callback_signal_go_to_last(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    GtkTreePath *path;

    ui_tree_view_select_row (ui_tree_view_get_filtered_number() - 1, &path);
    ui_main_data.path_last = path;

    return TRUE;
}

gboolean ui_callback_signal_clear_list(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    /* Disable buttons to move in the list of signals */
    ui_set_sensitive_move_buttons (FALSE);
    ui_set_title ("");

    /* Clear list of signals */
    ui_tree_view_destroy_list (ui_main_data.signalslist);
    return TRUE;
}

gboolean ui_callback_on_menu_item_selected(GtkWidget *widget, gpointer data)
{
    ui_filter_item_t *filter_entry = data;
    gboolean enabled;

    enabled = gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM(widget));
    if (filter_entry->enabled != enabled)
    {
        filter_entry->enabled = enabled;
        ui_tree_view_refilter ();
    }
    // g_debug("ui_callback_on_menu_item_selected occurred %x %x %s %d (%d messages to display)", (int) widget, (int) data, filter_entry->name, enabled, ui_tree_view_get_filtered_number());

    return TRUE;
}

gboolean ui_callback_on_tree_column_header_click(GtkWidget *widget, gpointer data)
{
    col_type_e col = (col_type_e) data;

    // g_debug("ui_callback_on_tree_column_header_click %x", col);
    switch (col)
    {
        case COL_SIGNAL:
            ui_show_filter_menu (&ui_main_data.menu_filter_messages, &ui_filters.messages);
            break;

        case COL_FROM_TASK:
            ui_show_filter_menu (&ui_main_data.menu_filter_origin_tasks, &ui_filters.origin_tasks);
            break;

        case COL_TO_TASK:
            ui_show_filter_menu (&ui_main_data.menu_filter_destination_tasks, &ui_filters.destination_tasks);
            break;

        default:
            g_warning("Unknown column filter %d in call to ui_callback_on_tree_column_header_click", col);
            return FALSE;
    }

    return TRUE;
}

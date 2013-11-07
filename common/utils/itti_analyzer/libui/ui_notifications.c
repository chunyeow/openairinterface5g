#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#include <gtk/gtk.h>

#include "itti_types.h"
#include "rc.h"

#include "ui_interface.h"
#include "ui_main_screen.h"
#include "ui_menu_bar.h"
#include "ui_notifications.h"
#include "ui_notif_dlg.h"
#include "ui_callbacks.h"
#include "ui_filters.h"
#include "ui_tree_view.h"

#include "xml_parse.h"

int ui_disable_connect_button(void)
{
    /* Disable Connect button and enable disconnect button */
    gtk_widget_set_sensitive (GTK_WIDGET (ui_main_data.connect), FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET (ui_main_data.disconnect), TRUE);

    return RC_OK;
}

int ui_enable_connect_button(void)
{
    /* Disable Disconnect button and enable connect button */
    gtk_widget_set_sensitive (GTK_WIDGET (ui_main_data.connect), TRUE);
    gtk_widget_set_sensitive (GTK_WIDGET (ui_main_data.disconnect), FALSE);

    return RC_OK;
}

static void ui_change_cursor(gboolean busy)
{
    static GdkWindow *window;

    if (busy)
    {
        GdkDisplay *display;
        GdkCursor *cursor;
        // gint x, y;

        cursor = gdk_cursor_new (GDK_WATCH);
        display = gdk_display_get_default();
        window = gtk_widget_get_window(GTK_WIDGET(ui_main_data.window));
        // window = gdk_display_get_window_at_pointer(display, &x, &y);

        gdk_window_set_cursor (window, cursor);
        gdk_display_sync(display);
        g_object_unref (cursor);
        // gtk_widget_set_sensitive (ui_main_data.window, FALSE);
        ui_gtk_flush_events();
    }
    else
    {
        gdk_window_set_cursor (window, NULL);
        // gtk_widget_set_sensitive (ui_main_data.window, TRUE);
        ui_gtk_flush_events();
    }
}

int ui_messages_read(char *filename)
{
    int result = RC_OK;
    int source;
    int read_data = 0;
    void *input_data = NULL;
    size_t input_data_length = 0;
    int read_messages = 0;

    ui_change_cursor(TRUE);

    source = open (filename, O_RDONLY);
    if (source < 0)
    {
        ui_notification_dialog (GTK_MESSAGE_ERROR, "open messages", "Failed to open file \"%s\": %s", filename, g_strerror (errno));
        result = RC_FAIL;
    }
    else
    {
        itti_socket_header_t message_header;

        ui_callback_signal_clear_list (NULL, NULL, NULL);

        do
        {
            read_data = read (source, &message_header, sizeof(itti_socket_header_t));

            if (read_data == -1)
            {
                ui_notification_dialog (GTK_MESSAGE_ERROR, "open messages", "Failed to read from file \"%s\": %s", filename, g_strerror (errno));
                result = RC_FAIL;
                break;
            }

            if (read_data > 0)
            {
                input_data_length = message_header.message_size - sizeof(itti_socket_header_t);

                // g_debug ("%x, %x ,%x", message_header.message_type, message_header.message_size, input_data_length);

                /* Checking for non-header part */
                if (input_data_length > 0)
                {
                    input_data = malloc (input_data_length);

                    if (read (source, input_data, input_data_length) < 0)
                    {
                        g_warning("Failed to read from file \"%s\": %s", filename, g_strerror (errno));
                        ui_notification_dialog (GTK_MESSAGE_ERROR, "open messages", "Failed to read from file \"%s\": %s", filename, g_strerror (errno));
                        result = RC_FAIL;
                        break;
                    }
                }

                switch (message_header.message_type)
                {
                    case ITTI_DUMP_MESSAGE_TYPE:
                    {
                        itti_signal_header_t *itti_signal_header = input_data;
                        buffer_t *buffer;

                        /* Create the new buffer */
                        if (buffer_new_from_data (&buffer, input_data + sizeof(itti_signal_header_t),
                                                  input_data_length - sizeof(itti_signal_header_t), 0) != RC_OK)
                        {
                            g_error("Failed to create new buffer");
                            g_assert_not_reached ();
                        }

                        buffer->message_number = itti_signal_header->message_number;

                        ui_signal_add_to_list (buffer, NULL);
                        read_messages++;
                        break;
                    }

                    case ITTI_DUMP_XML_DEFINITION:
                        xml_parse_buffer (input_data, input_data_length);
                        break;

                    case ITTI_STATISTIC_MESSAGE_TYPE:
                    default:
                        ui_notification_dialog (GTK_MESSAGE_WARNING, "open messages",
                                                "Unknown (or not implemented) record type %d in file \"%s\"",
                                                message_header.message_type, filename);
                        break;
                }

                free (input_data);
            }
        } while (read_data > 0);

        if (read_messages > 0)
        {
            /* Enable buttons to move in the list of signals */
            ui_set_sensitive_move_buttons (TRUE);
        }

        g_message("Read %d messages (%d to display) from file \"%s\"\n", read_messages, ui_tree_view_get_filtered_number(), filename);

        close (source);
    }

    ui_change_cursor(FALSE);

    return result;
}

int ui_messages_open_file_chooser(void)
{
    int result = RC_OK;
    GtkWidget *filechooser;
    gboolean accept;
    char *filename;

    filechooser = gtk_file_chooser_dialog_new ("Select file", GTK_WINDOW (ui_main_data.window),
                                               GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
                                               GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);

    /* Process the response */
    accept = gtk_dialog_run (GTK_DIALOG (filechooser)) == GTK_RESPONSE_ACCEPT;

    if (accept)
    {
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser));
    }
    gtk_widget_destroy (filechooser);
    if (accept)
    {
        result = ui_messages_read (filename);
        if (result == RC_OK)
        {
            /* Update messages file name for future use */
            if (ui_main_data.messages_file_name != NULL)
            {
                g_free (ui_main_data.messages_file_name);
            }
            ui_main_data.messages_file_name = filename;
        }
        else
        {
            g_free (filename);
        }
    }

    return result;
}

int ui_filters_open_file_chooser(void)
{
    int result = RC_OK;
    GtkWidget *filechooser;
    gboolean accept;
    char *filename;

    filechooser = gtk_file_chooser_dialog_new ("Select file", GTK_WINDOW (ui_main_data.window),
                                               GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
                                               GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);

    /* Process the response */
    accept = gtk_dialog_run (GTK_DIALOG (filechooser)) == GTK_RESPONSE_ACCEPT;

    if (accept)
    {
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser));
    }
    gtk_widget_destroy (filechooser);
    if (accept)
    {
        result = ui_filters_read(filename);
        if (result == RC_OK)
        {
            /* Update filters file name for future use */
            if (ui_main_data.filters_file_name != NULL)
            {
                g_free (ui_main_data.filters_file_name);
            }
            ui_main_data.filters_file_name = filename;
        }
        else
        {
            g_free (filename);
        }
    }

    return result;
}

int ui_filters_save_file_chooser(void)
{
    GtkWidget *filechooser;
    int result = RC_OK;

    filechooser = gtk_file_chooser_dialog_new ("Save file", GTK_WINDOW (ui_main_data.window),
                                               GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                               GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);

    gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (filechooser), TRUE);

    if (ui_main_data.filters_file_name != NULL)
    {
        gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (filechooser), ui_main_data.filters_file_name);
    }
    else
    {
        gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (filechooser), "filters.xml");
    }

    /* Process the response */
    if (gtk_dialog_run (GTK_DIALOG (filechooser)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;

        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser));
        result = ui_filters_file_write (filename);
        if (result == RC_OK)
        {
            /* Update filters file name for future use */
            if (ui_main_data.filters_file_name != NULL)
            {
                g_free (ui_main_data.filters_file_name);
            }
            ui_main_data.filters_file_name = filename;
        }
        else
        {
            g_free (filename);
        }
    }
    gtk_widget_destroy (filechooser);

    return result;
}

int ui_progress_bar_set_fraction(double fraction)
{
//     /* If not exist instantiate */
//     if (!ui_main_data.progressbar) {
//         ui_main_data.progressbar = gtk_progress_bar_new();
//     }
// 
//     gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(ui_main_data.progressbar), fraction);
// 
//     gtk_widget_show(ui_main_data.progressbar);
// 
//     gtk_main_iteration();

    return RC_OK;
}

int ui_progress_bar_terminate(void)
{
//     if (!ui_main_data.progressbar)
//         return RC_FAIL;
// 
//     gtk_widget_destroy(ui_main_data.progressbar);
//     ui_main_data.progressbar = NULL;

    return RC_OK;
}

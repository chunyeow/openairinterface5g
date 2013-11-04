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
#include "ui_callbacks.h"

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

int ui_messages_open_file_chooser(void)
{
    GtkWidget *filechooser;
    int result = RC_OK;

    filechooser = gtk_file_chooser_dialog_new ("Select file", GTK_WINDOW (ui_main_data.window),
                                               GTK_FILE_CHOOSER_ACTION_OPEN,
                                               GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
                                               GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                               NULL);

    /* Process the response */
    if (gtk_dialog_run (GTK_DIALOG (filechooser)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        int source;
        int read_data = 0;
        void *input_data = NULL;
        size_t input_data_length = 0;
        int read_messages = 0;

        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser));

        source = open (filename, O_RDONLY);
        if (source < 0)
        {
            g_warning ("Failed to open file \"%s\": %s", filename, g_strerror (errno));
            result = RC_FAIL;
        }
        else
        {
            itti_socket_header_t message_header;

            ui_callback_signal_clear_list(NULL, NULL, NULL);

            do
            {
                read_data = read (source, &message_header, sizeof(itti_socket_header_t));

                if (read_data == -1)
                {
                    g_warning ("Failed to read from file \"%s\": %s", filename, g_strerror (errno));
                    result = RC_FAIL;
                    break;
                }

                if (read_data > 0)
                {
                    input_data_length = message_header.message_size - sizeof(itti_socket_header_t);

                    // g_debug ("%x, %x ,%x\n", message_header.message_type, message_header.message_size, input_data_length);

                    /* Checking for non-header part */
                    if (input_data_length > 0)
                    {
                        input_data = malloc (input_data_length);

                        if (read (source, input_data, input_data_length) < 0)
                        {
                            g_warning ("Failed to read from file \"%s\": %s", filename, g_strerror (errno));
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
                                g_error ("Failed to create new buffer");
                                g_assert_not_reached ();
                            }

                            buffer->message_number = itti_signal_header->message_number;

                            ui_signal_add_to_list (buffer, NULL);
                            read_messages ++;
                            break;
                        }

                        case ITTI_DUMP_XML_DEFINITION:
                            xml_parse_buffer (input_data, input_data_length);
                            break;

                        case ITTI_STATISTIC_MESSAGE_TYPE:
                        default:
                            g_debug ("Received unknow (or not implemented) message from socket type: %d",
                                     message_header.message_type);
                            break;
                    }

                    free (input_data);
                }
            } while (read_data > 0);

            if (read_messages > 0)
            {
                /* Enable buttons to move in the list of signals */
                ui_set_sensitive_move_buttons(TRUE);
            }

            g_debug ("Read %d messages from file \"%s\"\n", read_messages, filename);

            close (source);
        }

        g_free (filename);
    }

    gtk_widget_destroy (filechooser);

    return result;
}

int ui_filters_open_file_chooser(void)
{
    GtkWidget *filechooser;
    int result = RC_OK;

    filechooser = gtk_file_chooser_dialog_new ("Select file", GTK_WINDOW (ui_main_data.window),
                                               GTK_FILE_CHOOSER_ACTION_OPEN,
                                               GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
                                               GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                               NULL);

    /* Process the response */
    if (gtk_dialog_run (GTK_DIALOG (filechooser)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;

        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser));

        g_free (filename);
    }

    gtk_widget_destroy (filechooser);

    return RC_OK;
}

int ui_filters_save_file_chooser(void)
{
    GtkWidget *filechooser;
    int result = RC_OK;

    filechooser = gtk_file_chooser_dialog_new ("Save file", GTK_WINDOW (ui_main_data.window),
                                               GTK_FILE_CHOOSER_ACTION_SAVE,
                                               GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                               GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                               NULL);

    gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (filechooser), TRUE);

    //gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (filechooser), "filters.xml");
    gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (filechooser), "./filters.xml");

    /* Process the response */
    if (gtk_dialog_run (GTK_DIALOG (filechooser)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;

        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser));

        g_free (filename);
    }

    gtk_widget_destroy (filechooser);

    return RC_OK;
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

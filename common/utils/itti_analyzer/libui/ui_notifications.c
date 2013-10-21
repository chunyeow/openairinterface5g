#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#include <gtk/gtk.h>

#include "rc.h"

#include "ui_interface.h"
#include "ui_main_screen.h"
#include "ui_notifications.h"

int ui_notification_dialog(dialog_type_t type, const char *fmt, ...)
{
    va_list ap;
    GtkWidget *dialogbox;

    va_start(ap, fmt);

    /* In multi-threaded environnements gtk calls should be protected by
     * gdk_threads_enter before calling the GTK function
     * gdk_threads_leave when GTK function has exited
     */
    gdk_threads_enter();

    dialogbox = gtk_message_dialog_new(
        GTK_WINDOW(ui_main_data.window), GTK_DIALOG_MODAL, type,
        GTK_BUTTONS_OK, fmt, ap);

    gtk_dialog_run(GTK_DIALOG (dialogbox));

    gtk_widget_destroy (dialogbox);

    gdk_threads_leave();

    va_end(ap);

    return RC_OK;
}

int ui_disable_connect_button(void)
{
    /* Disable Connect button and enable disconnect button */
    gtk_widget_set_sensitive(GTK_WIDGET(ui_main_data.connect), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(ui_main_data.disconnect), TRUE);

    return RC_OK;
}

int ui_enable_connect_button(void)
{
    /* Disable Disconnect button and enable connect button */
    gtk_widget_set_sensitive(GTK_WIDGET(ui_main_data.connect), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(ui_main_data.disconnect), FALSE);

    return RC_OK;
}

int ui_file_chooser(void)
{
    GtkWidget *filechooser;

    filechooser = gtk_file_chooser_dialog_new(
        "Select file", GTK_WINDOW(ui_main_data.window),
        GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
        GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

    /* Process the response */
    if (gtk_dialog_run (GTK_DIALOG (filechooser)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;

        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser));
        ui_interface.parse_signal_file(filename);
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

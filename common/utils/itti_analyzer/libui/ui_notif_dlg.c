#include "rc.h"

#include "ui_notif_dlg.h"
#include "ui_main_screen.h"

static const char * const title_type[] =
    {"Info", "Warning", "Question", "Error", "Other"};

int ui_notification_dialog(GtkMessageType type, const char *title, const char *fmt, ...)
{
    va_list args;
    GtkWidget *dialogbox;
    char buffer[200];

    va_start(args, fmt);

    vsnprintf (buffer, sizeof(buffer), fmt, args);

    g_warning("%s", buffer);

    dialogbox = gtk_message_dialog_new (GTK_WINDOW(ui_main_data.window), GTK_DIALOG_MODAL, type, GTK_BUTTONS_OK, "%s",
                                        buffer);

    snprintf (buffer, sizeof(buffer), "%s: %s", title_type[type], title);
    gtk_window_set_title (GTK_WINDOW(dialogbox), buffer);

    gtk_dialog_run (GTK_DIALOG (dialogbox));

    gtk_widget_destroy (dialogbox);

    va_end(args);

    return RC_OK;
}

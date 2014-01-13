#define G_LOG_DOMAIN ("UI")

#include "rc.h"

#include "ui_notif_dlg.h"
#include "ui_main_screen.h"

static const char * const title_type[] =
    {"Info", "Warning", "Question", "Error", "Other"};

int ui_notification_dialog(GtkMessageType type, gboolean cancel, const char *title, const char *fmt, ...)
{
    va_list     args;
    GtkWidget  *dialogbox;
    char        buffer[200];
    int         result =  RC_OK;

    va_start(args, fmt);

    vsnprintf (buffer, sizeof(buffer), fmt, args);

    g_warning("%s", buffer);

    dialogbox = gtk_message_dialog_new (GTK_WINDOW(ui_main_data.window), GTK_DIALOG_MODAL, type,
                                        cancel ? GTK_BUTTONS_OK_CANCEL : GTK_BUTTONS_OK, "%s",
                                        buffer);
    gtk_dialog_set_default_response (GTK_DIALOG(dialogbox), GTK_RESPONSE_OK);

    snprintf (buffer, sizeof(buffer), "%s: %s", title_type[type], title);
    gtk_window_set_title (GTK_WINDOW(dialogbox), buffer);

    if (gtk_dialog_run (GTK_DIALOG (dialogbox)) == GTK_RESPONSE_CANCEL)
    {
        result = RC_FAIL;
    }

    gtk_widget_destroy (dialogbox);

    va_end(args);

    return result;
}

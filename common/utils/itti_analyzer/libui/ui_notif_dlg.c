#include "rc.h"

#include "ui_notif_dlg.h"
#include "ui_main_screen.h"

int ui_notification_dialog(dialog_type_t type, const char *fmt, ...)
{
    va_list args;
    GtkWidget *dialogbox;

    va_start(args, fmt);

    dialogbox = gtk_message_dialog_new(GTK_WINDOW(ui_main_data.window),
                                       GTK_DIALOG_MODAL, type,
                                       GTK_BUTTONS_OK, fmt, args);

    gtk_dialog_run(GTK_DIALOG (dialogbox));

    gtk_widget_destroy (dialogbox);

    va_end(args);

    return RC_OK;
}

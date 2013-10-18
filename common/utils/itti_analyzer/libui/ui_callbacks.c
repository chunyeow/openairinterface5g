#include <stdlib.h>
#include <stdint.h>

#include <gtk/gtk.h>

#include "../rc.h"

#include "ui_main_screen.h"
#include "ui_callbacks.h"
#include "ui_interface.h"
#include "ui_notifications.h"
#include "ui_tree_view.h"
#include "ui_signal_dissect_view.h"

gboolean ui_callback_on_open(GtkWidget *widget,
                             GdkEvent  *event,
                             gpointer   data)
{
    g_print ("Open event occurred\n");
    CHECK_FCT(ui_file_chooser());
    return TRUE;
}

gboolean ui_callback_on_about(GtkWidget *widget,
                              GdkEvent  *event,
                              gpointer   data)
{

    return TRUE;
}

gboolean
ui_callback_on_select_signal(GtkTreeSelection *selection,
                             GtkTreeModel     *model,
                             GtkTreePath      *path,
                             gboolean          path_currently_selected,
                             gpointer          userdata)
{
    GtkTreeIter iter;

    if (gtk_tree_model_get_iter(model, &iter, path))
    {
        gchar *name;

        gtk_tree_model_get(model, &iter, 0, &name, -1);

        if (!path_currently_selected)
        {
            if (ui_interface.dissector_ready != 0) {
                uint32_t message_number;

                sscanf(name, "%u", &message_number);
                /* Clear the view */
                CHECK_FCT_DO(ui_signal_dissect_clear_view(), return FALSE);
    //             g_print ("%s is going to be selected.\n", name);
                CHECK_FCT_DO(ui_interface.dissect_signal(message_number), return FALSE);
            } else {
                ui_notification_dialog(DIALOG_ERROR, "No XML signal description"
                " provided\n");
            }
        }
        else
        {
            g_print ("%s is going to be unselected.\n", name);
        }

        g_free(name);
    }
    return TRUE;
}

gboolean ui_callback_on_connect(GtkWidget *widget,
                                GdkEvent  *event,
                                gpointer   data)
{
    /* We have to retrieve the ip address and port of remote host */
    uint16_t port;
    const char *ip;

    g_print ("Connect event occurred\n");

    port = atoi(gtk_entry_get_text(GTK_ENTRY(ui_main_data.portentry)));
    ip = gtk_entry_get_text(GTK_ENTRY(ui_main_data.ipentry));

//     ui_tree_view_destroy_list();
    ui_interface.socket_connect(ip, port);

    return TRUE;
}

gboolean ui_callback_on_disconnect(GtkWidget *widget,
                                   GdkEvent  *event,
                                   gpointer   data)
{
    /* We have to retrieve the ip address and port of remote host */

    g_print ("Disconnect event occurred\n");
    ui_interface.socket_disconnect();
    return TRUE;
}

gboolean ui_callback_on_tree_view_select(GtkWidget *widget,
                                         GdkEvent  *event,
                                         gpointer   data)
{
    /* We have to retrieve the ip address and port of remote host */

    g_print ("List selection event occurred\n");
    return TRUE;
}


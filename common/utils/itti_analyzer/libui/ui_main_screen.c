
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <gtk/gtk.h>

#include "../rc.h"

#include "ui_interface.h"
#include "ui_main_screen.h"
#include "ui_menu_bar.h"
#include "ui_tree_view.h"
#include "ui_notebook.h"
#include "ui_notifications.h"

ui_main_data_t ui_main_data;

int ui_gtk_initialize(int argc, char *argv[])
{
    GtkWidget *vbox;

    memset(&ui_main_data, 0, sizeof(ui_main_data_t));

    if (!g_thread_supported())
        g_thread_init(NULL);

    /* Secure gtk */
    gdk_threads_init();

    /* Obtain gtk's global lock */
    gdk_threads_enter();

    /* Initialize the widget set */
    gtk_init(&argc, &argv);

    /* Create the main window */
    ui_main_data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    gtk_window_set_position(GTK_WINDOW(ui_main_data.window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(ui_main_data.window), 1024, 800);
#if defined(PACKAGE_STRING)
    gtk_window_set_title(GTK_WINDOW(ui_main_data.window), PACKAGE_STRING);
#else
    gtk_window_set_title(GTK_WINDOW(ui_main_data.window), "itti debugger");
#endif
    gtk_window_set_resizable(GTK_WINDOW(ui_main_data.window), TRUE);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    CHECK_FCT(ui_menu_bar_create(vbox));
    CHECK_FCT(ui_toolbar_create(vbox));
//     CHECK_FCT(ui_tree_view_create(ui_main_data.window, vbox));
    CHECK_FCT(ui_notebook_create(vbox));

    gtk_container_add(GTK_CONTAINER(ui_main_data.window), vbox);
    /* Assign the destroy event */
    g_signal_connect(ui_main_data.window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    /* Show the application window */
    gtk_widget_show_all (ui_main_data.window);

    /* Enter the main event loop, and wait for user interaction */
    gtk_main ();

    /* Release gtk's global lock */
    gdk_threads_leave();

    return RC_OK;
}

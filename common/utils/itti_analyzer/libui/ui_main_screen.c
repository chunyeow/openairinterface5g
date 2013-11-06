#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <gtk/gtk.h>

#include "rc.h"

#include "ui_interface.h"
#include "ui_main_screen.h"
#include "ui_menu_bar.h"
#include "ui_tree_view.h"
#include "ui_notebook.h"
#include "ui_notifications.h"
#include "ui_filters.h"

ui_main_data_t ui_main_data;

static void ui_help(void)
{
    printf ("Usage: itti_analyser [options]\n\n"
            "Options:\n"
            "  -f FILTERS   read filters from FILTERS file\n"
            "  -h           display this help and exit\n"
            "  -i IP        set ip address to IP\n"
            "  -m MESSAGES  read messages from MESSAGES file\n"
            "  -p PORT      set port to PORT\n");
}

static void ui_gtk_parse_arg(int argc, char *argv[])
{
    char c;

    while ((c = getopt (argc, argv, "f:hi:m:p:")) != -1)
    {
        switch (c)
        {
            case 'f':
                ui_main_data.filters_file_name = optarg;
                break;

            case 'h':
                ui_help ();
                exit (0);
                break;

            case 'i':
                ui_main_data.ip_entry_init = optarg;
                break;

            case 'm':
                ui_main_data.messages_file_name = optarg;
                break;

            case 'p':
                ui_main_data.port_entry_init = optarg;
                break;

            default:
                ui_help ();
                exit (-1);
                break;
        }
    }
}

static int ui_idle_callback(gpointer data)
{
    g_debug("Entering idle state");

    /* Read filters file */
    if (ui_main_data.filters_file_name != NULL)
    {
        // ui_filters_read(ui_main_data.filters_file_name);
    }

    /* Read messages file */
    if (ui_main_data.messages_file_name != NULL)
    {
        ui_messages_read (ui_main_data.messages_file_name);
    }

    /* One shot execution */
    return FALSE;
}

int ui_gtk_initialize(int argc, char *argv[])
{
    GtkWidget *vbox;

    memset (&ui_main_data, 0, sizeof(ui_main_data_t));

    /* Set some default initialization value */
    ui_main_data.ip_entry_init = "127.0.0.1";
    ui_main_data.port_entry_init = "10007";

    ui_gtk_parse_arg (argc, argv);

    /* Create the main window */
    ui_main_data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    ui_init_filters (TRUE, FALSE);

    gtk_window_set_position (GTK_WINDOW(ui_main_data.window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size (GTK_WINDOW(ui_main_data.window), 1024, 800);
#if defined(PACKAGE_STRING)
    gtk_window_set_title(GTK_WINDOW(ui_main_data.window), PACKAGE_STRING);
#else
    gtk_window_set_title (GTK_WINDOW(ui_main_data.window), "itti debugger");
#endif
    gtk_window_set_resizable (GTK_WINDOW(ui_main_data.window), TRUE);

    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

    CHECK_FCT(ui_menu_bar_create(vbox));
    CHECK_FCT(ui_toolbar_create(vbox));
//     CHECK_FCT(ui_tree_view_create(ui_main_data.window, vbox));
    CHECK_FCT(ui_notebook_create(vbox));

    gtk_container_add (GTK_CONTAINER(ui_main_data.window), vbox);

    /* Assign the destroy event */
    g_signal_connect(ui_main_data.window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    /* Show the application window */
    gtk_widget_show_all (ui_main_data.window);

    g_idle_add (ui_idle_callback, NULL);

    return RC_OK;
}

void ui_gtk_flush_events(void)
{
    while (gtk_events_pending ())
    {
        gtk_main_iteration();
    }
}

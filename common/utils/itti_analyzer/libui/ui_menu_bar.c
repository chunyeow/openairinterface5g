#include <gtk/gtk.h>

#include "rc.h"
#include "ui_main_screen.h"
#include "ui_menu_bar.h"
#include "ui_callbacks.h"

static const guint BUTTON_SPACE = 0;
static const guint LABEL_SPACE = 5;
static const guint SEPARATOR_SPACE = 5;

void ui_set_sensitive_move_buttons(gboolean enable)
{
    // gtk_widget_set_sensitive(GTK_WIDGET(ui_main_data.signals_clear_button), enable);
    // gtk_widget_set_sensitive(GTK_WIDGET(ui_main_data.signals_go_to_button), enable);
    gtk_widget_set_sensitive(GTK_WIDGET(ui_main_data.signals_go_to_last_button), enable);
    gtk_widget_set_sensitive(GTK_WIDGET(ui_main_data.signals_go_to_first_button), enable);
}

int ui_menu_bar_create(GtkWidget *vbox)
{
    GtkWidget *menubar;
    GtkWidget *filemenu, *helpmenu;
    GtkWidget *file;
    GtkWidget *help;
    GtkWidget *open_messages;
    GtkWidget *save_messages;
    GtkWidget *open_filters;
    GtkWidget *save_filters;
    GtkWidget *quit;
    GtkWidget *about;
    GtkWidget *separator1;
    GtkWidget *separator2;

    if (!vbox)
        return RC_BAD_PARAM;

    menubar = gtk_menu_bar_new();

    /* Create the File submenu */
    filemenu = gtk_menu_new();

    file = gtk_menu_item_new_with_label("File");
    open_messages  = gtk_menu_item_new_with_label("Open messages file");
    save_messages  = gtk_menu_item_new_with_label("Save messages file");
    open_filters  = gtk_menu_item_new_with_label("Open filters file");
    save_filters  = gtk_menu_item_new_with_label("Save filters file");
    quit  = gtk_menu_item_new_with_label("Quit");
    separator1 = gtk_menu_item_new();
    separator2 = gtk_menu_item_new();

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);

    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), open_messages);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), save_messages);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), separator1);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), open_filters);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), save_filters);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), separator2);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), quit);

    /* Create the Help submenu */
    helpmenu = gtk_menu_new();

    help = gtk_menu_item_new_with_label("Help");
    about = gtk_menu_item_new_with_label("About");

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), helpmenu);

    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), help);
    gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu), about);

    /* Add the menubar to the vbox */
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 3);

    g_signal_connect(G_OBJECT(open_messages), "activate",
                     G_CALLBACK(ui_callback_on_open_messages), NULL);

    g_signal_connect(G_OBJECT(save_messages), "activate",
                     G_CALLBACK(ui_callback_on_save_messages), NULL);

    g_signal_connect(G_OBJECT(open_filters), "activate",
                     G_CALLBACK(ui_callback_on_open_filters), NULL);

    g_signal_connect(G_OBJECT(save_filters), "activate",
                     G_CALLBACK(ui_callback_on_save_filters), NULL);

    g_signal_connect(G_OBJECT(quit), "activate",
                     G_CALLBACK(gtk_main_quit), NULL);

    g_signal_connect(G_OBJECT(about), "activate",
                     G_CALLBACK(ui_callback_on_about), NULL);

    return RC_OK;
}

int ui_toolbar_create(GtkWidget *vbox)
{
    GtkWidget *hbox;
    GtkWidget *filters_label;
    GtkWidget *messages_label;
    GtkWidget *ip_label;
    GtkWidget *port_label;

    if (!vbox)
        return RC_BAD_PARAM;

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    filters_label = gtk_label_new("Filters");
    messages_label = gtk_label_new("Messages");

    /* Button to open filters file */
    {
        ui_main_data.open_filters_file = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
        gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(ui_main_data.open_filters_file),
                                    "Open filters file");

        g_signal_connect(G_OBJECT(ui_main_data.open_filters_file), "clicked",
                        G_CALLBACK(ui_callback_on_open_filters), NULL);
    }

    /* Button to save filters file */
    {
        ui_main_data.save_filters_file = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
        gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(ui_main_data.save_filters_file),
                                    "Save filters file");

        g_signal_connect(G_OBJECT(ui_main_data.save_filters_file), "clicked",
                        G_CALLBACK(ui_callback_on_save_filters), NULL);
    }

    /* Button to open replay file */
    {
        ui_main_data.open_replay_file = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
        gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(ui_main_data.open_replay_file),
                                    "Open messages file");

        g_signal_connect(G_OBJECT(ui_main_data.open_replay_file), "clicked",
                        G_CALLBACK(ui_callback_on_open_messages), (gpointer) FALSE);
    }

    /* Button to refresh replay file */
    {
        ui_main_data.refresh_replay_file = gtk_tool_button_new_from_stock(GTK_STOCK_REFRESH);
        gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(ui_main_data.refresh_replay_file),
                                    "Reload messages file");

        g_signal_connect(G_OBJECT(ui_main_data.refresh_replay_file), "clicked",
                        G_CALLBACK(ui_callback_on_open_messages), (gpointer) TRUE);
    }

    /* Button to save replay file */
    {
        ui_main_data.save_replay_file = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
        gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(ui_main_data.save_replay_file),
                                    "Save messages file");

        g_signal_connect(G_OBJECT(ui_main_data.save_replay_file), "clicked",
                        G_CALLBACK(ui_callback_on_save_messages), NULL);
    }

#if 0 /* This function is already handled by GTK */
    /* Button to go given signal number */
    {
        ui_main_data.signals_go_to_button = gtk_tool_button_new_from_stock(GTK_STOCK_INDEX);
        /* Set the tooltip text */
        gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(ui_main_data.signals_go_to_button),
                                    "Goto signal");

        gtk_widget_set_sensitive(GTK_WIDGET(ui_main_data.signals_go_to_button), FALSE);

        g_signal_connect(G_OBJECT(ui_main_data.signals_go_to_button), "clicked",
                        G_CALLBACK(ui_callback_signal_go_to), NULL);
    }
#endif

    /* Button to go to first signal in list */
    {
        ui_main_data.signals_go_to_first_button = gtk_tool_button_new_from_stock(GTK_STOCK_GOTO_FIRST);
        /* Set the tooltip text */
        gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(ui_main_data.signals_go_to_first_button),
                                    "Goto first signal");

        gtk_widget_set_sensitive(GTK_WIDGET(ui_main_data.signals_go_to_first_button), FALSE);

        g_signal_connect(G_OBJECT(ui_main_data.signals_go_to_first_button), "clicked",
                        G_CALLBACK(ui_callback_signal_go_to_first), NULL);
    }

    /* Button to go to last signal in list */
    {
        ui_main_data.signals_go_to_last_button = gtk_tool_button_new_from_stock(GTK_STOCK_GOTO_LAST);
        /* Set the tooltip text */
        gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(ui_main_data.signals_go_to_last_button),
                                    "Goto last signal");

        gtk_widget_set_sensitive(GTK_WIDGET(ui_main_data.signals_go_to_last_button), FALSE);

        g_signal_connect(G_OBJECT(ui_main_data.signals_go_to_last_button), "clicked",
                        G_CALLBACK(ui_callback_signal_go_to_last), NULL);
    }

    /* Button to connect to remote */
    {
        ui_main_data.connect = gtk_tool_button_new_from_stock(GTK_STOCK_CONNECT);
        /* Set the tooltip text */
        gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(ui_main_data.connect),
                                    "Connect to remote host");

        g_signal_connect(G_OBJECT(ui_main_data.connect), "clicked",
                        G_CALLBACK(ui_callback_on_connect), NULL);
    }

    /* Button to disconnect from remote */
    {
        ui_main_data.disconnect = gtk_tool_button_new_from_stock(GTK_STOCK_DISCONNECT);
        /* Set the tooltip text */
        gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(ui_main_data.disconnect),
                                    "Disconnect from remote host");

        /* Disabled at startup. Will be activated when a connection is established */
        gtk_widget_set_sensitive(GTK_WIDGET(ui_main_data.disconnect), FALSE);

        g_signal_connect(G_OBJECT(ui_main_data.disconnect), "clicked",
                        G_CALLBACK(ui_callback_on_disconnect), NULL);
    }

    ip_label = gtk_label_new("ip:");
    port_label = gtk_label_new("port:");

    ui_main_data.ip_entry = gtk_entry_new();
    /* Width of 15 characters for port number (ipv4 address) */
    gtk_entry_set_width_chars(GTK_ENTRY(ui_main_data.ip_entry), 15);
    gtk_entry_set_text(GTK_ENTRY(ui_main_data.ip_entry), ui_main_data.ip_entry_init);

    ui_main_data.port_entry = gtk_entry_new();
    /* Width of 5 characters for port number (uint16_t) */
    gtk_entry_set_width_chars(GTK_ENTRY(ui_main_data.port_entry), 5);
    gtk_entry_set_text(GTK_ENTRY(ui_main_data.port_entry), ui_main_data.port_entry_init);

    gtk_box_pack_start(GTK_BOX(hbox), filters_label, FALSE, FALSE, LABEL_SPACE);
    gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(ui_main_data.open_filters_file), FALSE, FALSE, BUTTON_SPACE);
    gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(ui_main_data.save_filters_file), FALSE, FALSE, BUTTON_SPACE);

    gtk_box_pack_start(GTK_BOX(hbox), gtk_separator_new(GTK_ORIENTATION_VERTICAL), FALSE, FALSE, SEPARATOR_SPACE);

    gtk_box_pack_start(GTK_BOX(hbox), messages_label, FALSE, FALSE, LABEL_SPACE);
    gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(ui_main_data.open_replay_file), FALSE, FALSE, BUTTON_SPACE);
    gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(ui_main_data.refresh_replay_file), FALSE, FALSE, BUTTON_SPACE);
    gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(ui_main_data.save_replay_file), FALSE, FALSE, BUTTON_SPACE);

    gtk_box_pack_start(GTK_BOX(hbox), gtk_separator_new(GTK_ORIENTATION_VERTICAL), FALSE, FALSE, SEPARATOR_SPACE);

    gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(ui_main_data.signals_go_to_first_button), FALSE, FALSE, BUTTON_SPACE);
    gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(ui_main_data.signals_go_to_last_button), FALSE, FALSE, BUTTON_SPACE);

    gtk_box_pack_start(GTK_BOX(hbox), gtk_separator_new(GTK_ORIENTATION_VERTICAL), FALSE, FALSE, SEPARATOR_SPACE);

    gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(ui_main_data.connect), FALSE, FALSE, BUTTON_SPACE);
    gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(ui_main_data.disconnect), FALSE, FALSE, BUTTON_SPACE);
    gtk_box_pack_start(GTK_BOX(hbox), ip_label, FALSE, FALSE, LABEL_SPACE);
    gtk_box_pack_start(GTK_BOX(hbox), ui_main_data.ip_entry, FALSE, FALSE, BUTTON_SPACE);
    gtk_box_pack_start(GTK_BOX(hbox), port_label, FALSE, FALSE, LABEL_SPACE);
    gtk_box_pack_start(GTK_BOX(hbox), ui_main_data.port_entry, FALSE, FALSE, BUTTON_SPACE);

    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);

    return RC_OK;
}

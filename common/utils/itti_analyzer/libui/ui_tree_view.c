#include <stdlib.h>
#include <stdint.h>

#include <gtk/gtk.h>

#include "../rc.h"

#include "ui_main_screen.h"
#include "ui_tree_view.h"
#include "ui_callbacks.h"

#include "ui_signal_dissect_view.h"

enum
{
    COL_MSG_NUM = 0,
    COL_SIGNAL,
    NUM_COLS
} ;

static void
init_list(GtkWidget *list)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkListStore *store;

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(
        "Message Number", renderer, "text", COL_MSG_NUM, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

    column = gtk_tree_view_column_new_with_attributes(
        "Signal", renderer, "text", COL_SIGNAL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

    store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING);

    gtk_tree_view_set_model(GTK_TREE_VIEW(list), GTK_TREE_MODEL(store));

    g_object_unref(store);
}

static void
add_to_list(GtkWidget *list, const gchar *message_number, const gchar *signal_name)
{
    GtkListStore *store;
    GtkTreeIter iter;

    store = GTK_LIST_STORE(gtk_tree_view_get_model
        (GTK_TREE_VIEW(list)));

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, COL_MSG_NUM, message_number,
                       COL_SIGNAL, signal_name, -1);
}

void ui_tree_view_destroy_list(GtkWidget *list)
{
    GtkListStore *store;

    g_assert(list != NULL);

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));

    gtk_list_store_clear(store);
}

int ui_tree_view_create(GtkWidget *window, GtkWidget *vbox)
{
    GtkWidget *hbox;
    GtkTreeSelection *selection;
    GtkWidget *scrolled_window;
//     gint width;

    scrolled_window = gtk_scrolled_window_new(NULL, NULL);

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

    //gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 5);

    ui_main_data.signalslist = gtk_tree_view_new();
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(ui_main_data.signalslist), TRUE);

    /* Disable multiple selection */
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(ui_main_data.signalslist));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_BROWSE);

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window),
                                          ui_main_data.signalslist);

    init_list(ui_main_data.signalslist);

//     gtk_widget_get_size_request(GTK_WIDGET(ui_main_data.signalslist), &width, NULL);
    gtk_widget_set_size_request(GTK_WIDGET(scrolled_window), 350, -1);
    gtk_box_pack_start(GTK_BOX(hbox), scrolled_window, FALSE, FALSE, 0);
    CHECK_FCT(ui_signal_dissect_new(hbox));

    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 5);

//     g_signal_connect(G_OBJECT(ui_main_data.signalslist), "cursor-changed",
//                      G_CALLBACK(ui_callback_on_select_signal), NULL);

    /* Connect callback on row selection */
    gtk_tree_selection_set_select_function(selection, ui_callback_on_select_signal, NULL, NULL);

    return 0;
}

int ui_tree_view_new_signal_ind(const uint32_t message_number, const char *signal_name)
{
    gchar message_number_str[11];

    sprintf(message_number_str, "%u", message_number);

    add_to_list(ui_main_data.signalslist, message_number_str, signal_name);

    return RC_OK;
}

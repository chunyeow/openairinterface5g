#include <string.h>

#include <gtk/gtk.h>

#include "rc.h"

#include "ui_main_screen.h"
#include "ui_menu_bar.h"
#include "ui_signal_dissect_view.h"

ui_text_view_t *ui_signal_dissect_new(GtkWidget *hbox)
{
    GtkWidget *scrolled_window;
    ui_text_view_t *new_text_view;

    new_text_view = malloc(sizeof(ui_text_view_t));

    new_text_view->text_view = gtk_text_view_new();
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

    /* Disable editable attribute */
    gtk_text_view_set_editable(GTK_TEXT_VIEW(new_text_view->text_view), FALSE);

    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window),
                                          new_text_view->text_view);

    gtk_box_pack_start(GTK_BOX(hbox), scrolled_window, TRUE, TRUE, 5);

    return new_text_view;
}

int ui_signal_dissect_clear_view(ui_text_view_t *text_view)
{
    GtkTextBuffer *text_buffer;

    g_assert(text_view != NULL);

    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view->text_view));

    /* If a text buffer is present for the textview remove it */
    if (text_buffer != NULL) {
        gtk_text_view_set_buffer(GTK_TEXT_VIEW(text_view->text_view), NULL);
    }

    return RC_OK;
}

gboolean ui_signal_set_text(gpointer user_data, gchar *text, gint length)
{
    GtkTextBuffer  *text_buffer;
    ui_text_view_t *text_view;

    if (length < 0)
        return FALSE;

    text_view = (ui_text_view_t *)user_data;

    g_assert(text != NULL);
    g_assert(text_view != NULL);
    g_assert(text_view->text_view != NULL);

    g_assert(GTK_IS_TEXT_VIEW(GTK_TEXT_VIEW(text_view->text_view)));

    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view->text_view));

    if (text_buffer) {
        /* We already have a text buffer, use it */
        gtk_text_buffer_insert_at_cursor(text_buffer, text, length);
    } else {
        /* No buffer currently in use, create a new one */
        text_buffer = gtk_text_buffer_new(NULL);
        gtk_text_buffer_set_text(text_buffer, text, length);
        gtk_text_view_set_buffer(GTK_TEXT_VIEW(text_view->text_view),
                                 text_buffer);
    }

    return TRUE;
}

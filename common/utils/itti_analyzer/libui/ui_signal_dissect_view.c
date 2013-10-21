#include <string.h>

#include <gtk/gtk.h>

#include "rc.h"

#include "ui_main_screen.h"
#include "ui_menu_bar.h"
#include "ui_signal_dissect_view.h"

int ui_signal_dissect_new(GtkWidget *hbox)
{
    GtkWidget *scrolled_window;

    ui_main_data.textview = gtk_text_view_new();
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

    CHECK_BUFFER(ui_main_data.textview);

    /* Disable editable attribute */
    gtk_text_view_set_editable(GTK_TEXT_VIEW(ui_main_data.textview), FALSE);

    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window),
                                          ui_main_data.textview);

    gtk_box_pack_start(GTK_BOX(hbox), scrolled_window, TRUE, TRUE, 5);

    return 0;
}

int ui_signal_dissect_clear_view(void)
{
    GtkTextBuffer *textbuffer;

    textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui_main_data.textview));

    CHECK_BUFFER(textbuffer);

    /* If a text buffer is present for the textview remove it */
    if (textbuffer) {
        gtk_text_view_set_buffer(GTK_TEXT_VIEW(ui_main_data.textview), NULL);
//         g_object_unref(textbuffer);
    }
    return RC_OK;
}

int ui_signal_set_text(char *text, int length)
{
    GtkTextBuffer *textbuffer;

    if (length < 0)
        return RC_BAD_PARAM;

    CHECK_BUFFER(text);

    // fprintf (stdout, "%*s", length, text);

    textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui_main_data.textview));

    if (textbuffer) {
        /* We already have a text buffer, use it */
        gtk_text_buffer_insert_at_cursor(textbuffer, text, length);
    } else {
        /* No buffer currently in use, create a new one */
        textbuffer = gtk_text_buffer_new(NULL);
        gtk_text_buffer_set_text(textbuffer, text, length);
        gtk_text_view_set_buffer(GTK_TEXT_VIEW(ui_main_data.textview), textbuffer);
    }

    return RC_OK;
}

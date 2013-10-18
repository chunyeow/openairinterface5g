#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdint.h>
#include <gtk/gtk.h>

#include "../rc.h"

#include "ui_notebook.h"
#include "ui_tree_view.h"

int ui_notebook_create(GtkWidget *vbox)
{
    GtkWidget *notebook;
    GtkWidget *vbox_notebook;

    if (!vbox)
        return RC_BAD_PARAM;

    notebook = gtk_notebook_new();

    vbox_notebook = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    ui_tree_view_create(NULL, vbox_notebook);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox_notebook, NULL);

    /* Add the notebook to the vbox of the main window */
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);

    return RC_OK;
}

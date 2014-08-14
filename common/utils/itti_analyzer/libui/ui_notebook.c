/*******************************************************************************
    OpenAirInterface 
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is 
   included in this distribution in the file called "COPYING". If not, 
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdint.h>
#define G_LOG_DOMAIN ("UI")

#include <gtk/gtk.h>

#include "rc.h"

#include "ui_notebook.h"
#include "ui_tree_view.h"
#include "ui_signal_dissect_view.h"

static ui_text_view_t *terminal_view;

void ui_notebook_terminal_clear(void)
{
     ui_signal_dissect_clear_view(terminal_view);
}

void ui_notebook_terminal_append_data(gchar *text, gint length)
{
    ui_signal_set_text(terminal_view, text, length);
}

int ui_notebook_create(GtkWidget *vbox)
{
    GtkWidget *notebook;
    GtkWidget *vbox_notebook, *vbox_terminal;

    if (!vbox)
        return RC_BAD_PARAM;

    notebook = gtk_notebook_new();

    vbox_notebook = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    ui_tree_view_create(NULL, vbox_notebook);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox_notebook, NULL);
    gtk_notebook_set_tab_label_text (GTK_NOTEBOOK(notebook), vbox_notebook, "Messages list");

#if defined (FILTERS_TAB)
    vbox_notebook = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox_notebook, NULL);
    gtk_notebook_set_tab_label_text (GTK_NOTEBOOK(notebook), vbox_notebook, "Filters");
#endif

    vbox_notebook = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox_notebook, NULL);
    gtk_notebook_set_tab_label_text (GTK_NOTEBOOK(notebook), vbox_notebook, "Terminal");

    vbox_terminal = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);

    terminal_view = ui_signal_dissect_new(vbox_terminal);

    gtk_box_pack_start (GTK_BOX(vbox_notebook), vbox_terminal, TRUE, TRUE, 5);

    /* Add the notebook to the vbox of the main window */
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);

    return RC_OK;
}

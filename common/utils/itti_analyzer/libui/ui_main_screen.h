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

#ifndef UI_MAIN_SCREEN_H_
#define UI_MAIN_SCREEN_H_

#include <gtk/gtk.h>

#include "ui_signal_dissect_view.h"

typedef struct {
    GtkWidget *window;
    GtkWidget *ip_entry;
    char *ip_entry_init;
    GtkWidget *port_entry;
    char *port_entry_init;

    GtkWidget      *progressbar_window;
    GtkWidget      *progressbar;
    GtkWidget      *messages_list;
    ui_text_view_t *text_view;

    /* Buttons */
    GtkToolItem *filters_enabled;
    GtkToolItem *open_filters_file;
    GtkToolItem *refresh_filters_file;
    GtkToolItem *save_filters_file;

    GtkToolItem *open_replay_file;
    GtkToolItem *refresh_replay_file;
    GtkToolItem *stop_loading;
    GtkToolItem *save_replay_file;
    GtkToolItem *save_replay_file_filtered;

    GtkToolItem *auto_reconnect;
    GtkToolItem *connect;
    GtkToolItem *disconnect;

    /* Signal list buttons */
    /* Clear signals button */
    GtkWidget *signals_go_to_entry;
    GtkToolItem *signals_go_to_last_button;
    GtkToolItem *signals_go_to_first_button;
    gboolean display_message_header;
    gboolean display_brace;

    GtkTreeSelection *selection;
    gboolean follow_last;

    /* Nb of messages received */
    guint nb_message_received;

    GLogLevelFlags log_flags;
    char *dissect_file_name;
    char *filters_file_name;
    char *messages_file_name;

    GtkWidget *menu_filter_messages;
    GtkWidget *menu_filter_origin_tasks;
    GtkWidget *menu_filter_destination_tasks;
    GtkWidget *menu_filter_instances;

    int pipe_fd[2];
} ui_main_data_t;

extern ui_main_data_t ui_main_data;

void ui_gtk_parse_arg(int argc, char *argv[]);

void ui_set_title(const char *fmt, ...);

void ui_main_window_destroy (void);

int ui_gtk_initialize(int argc, char *argv[]);

void ui_gtk_flush_events(void);

#endif /* UI_MAIN_SCREEN_H_ */

#include "ui_signal_dissect_view.h"

#ifndef UI_MAIN_SCREEN_H_
#define UI_MAIN_SCREEN_H_

typedef struct {
    GtkWidget *window;
    GtkWidget *ipentry;
    GtkWidget *portentry;

    GtkWidget      *progressbar;
    GtkWidget      *signalslist;
    ui_text_view_t *text_view;

    /* Buttons */
    GtkToolItem *open_replay_file;
    GtkToolItem *save_replay_file;
    GtkToolItem *open_filters_file;
    GtkToolItem *save_filters_file;
    GtkToolItem *connect;
    GtkToolItem *disconnect;

    /* Signal list buttons */
    /* Clear signals button */
    GtkToolItem *signals_clear_button;
    GtkToolItem *signals_go_to_button;
    GtkToolItem *signals_go_to_last_button;
    GtkToolItem *signals_go_to_first_button;

    GtkTreeSelection *selection;
    GtkTreePath *path_last;

    /* Nb of messages received */
    guint nb_message_received;

    int pipe_fd[2];
} ui_main_data_t;

extern ui_main_data_t ui_main_data;

int ui_gtk_initialize(int argc, char *argv[]);

#endif /* UI_MAIN_SCREEN_H_ */

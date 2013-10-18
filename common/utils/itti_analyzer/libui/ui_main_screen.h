#ifndef UI_MAIN_SCREEN_H_
#define UI_MAIN_SCREEN_H_

typedef struct {
    GtkWidget *window;
    GtkWidget *ipentry;
    GtkWidget *portentry;

    GtkWidget *progressbar;
    GtkWidget *signalslist;
    GtkWidget *textview;

    /* Buttons */
    GtkToolItem *connect;
    GtkToolItem *disconnect;
} ui_main_data_t;

extern ui_main_data_t ui_main_data;

int ui_gtk_initialize(int argc, char *argv[]);

#endif /* UI_MAIN_SCREEN_H_ */

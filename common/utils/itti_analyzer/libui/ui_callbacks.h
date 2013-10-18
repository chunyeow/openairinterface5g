#ifndef UI_CALLBACKS_H_
#define UI_CALLBACKS_H_

gboolean ui_callback_on_open(GtkWidget *widget,
                             GdkEvent  *event,
                             gpointer   data);

gboolean ui_callback_on_about(GtkWidget *widget,
                              GdkEvent  *event,
                              gpointer   data);

gboolean ui_callback_on_connect(GtkWidget *widget,
                                GdkEvent  *event,
                                gpointer   data);

gboolean ui_callback_on_disconnect(GtkWidget *widget,
                                   GdkEvent  *event,
                                   gpointer   data);

gboolean ui_callback_on_tree_view_select(GtkWidget *widget,
                                         GdkEvent  *event,
                                         gpointer   data);

gboolean
ui_callback_on_select_signal(GtkTreeSelection *selection,
                             GtkTreeModel     *model,
                             GtkTreePath      *path,
                             gboolean          path_currently_selected,
                             gpointer          userdata);

#endif /* UI_CALLBACKS_H_ */

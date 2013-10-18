#ifndef UI_TREE_VIEW_H_
#define UI_TREE_VIEW_H_

int ui_tree_view_create(GtkWidget *window, GtkWidget *vbox);

int ui_tree_view_new_signal_ind(const uint32_t message_number, const char *signal_name);

void ui_tree_view_destroy_list(GtkWidget *list);

#endif /* UI_TREE_VIEW_H_ */

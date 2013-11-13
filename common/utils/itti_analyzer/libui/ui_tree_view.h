#ifndef UI_TREE_VIEW_H_
#define UI_TREE_VIEW_H_

typedef enum
{
    COL_MSG_NUM = 0,
    COL_MESSAGE,
    COL_FROM_TASK,
    COL_TO_TASK,
    COL_INSTANCE,

    COL_MESSAGE_ID,
    COL_FROM_TASK_ID,
    COL_TO_TASK_ID,

    COL_BUFFER,
    NUM_COLS
} col_type_e;

int ui_tree_view_create(GtkWidget *window, GtkWidget *vbox);

int ui_tree_view_new_signal_ind(const uint32_t message_number,
                                const uint32_t message_id, const char *message_name,
                                const uint32_t origin_task_id, const char *origin_task,
                                const uint32_t destination_task_id, const char *to_task,
                                uint32_t instance, gpointer buffer);

void ui_tree_view_destroy_list(GtkWidget *list);

void ui_tree_view_select_row(gint row, GtkTreePath **path);

void ui_tree_view_refilter(void);

guint ui_tree_view_get_filtered_number(void);

#endif /* UI_TREE_VIEW_H_ */

#ifndef UI_NOTIFICATIONS_H_
#define UI_NOTIFICATIONS_H_

int ui_disable_connect_button(void);

int ui_enable_connect_button(void);

int ui_messages_open_file_chooser(void);

int ui_filters_open_file_chooser(void);

int ui_filters_save_file_chooser(void);

int ui_progress_bar_set_fraction(double fraction);

int ui_progress_bar_terminate(void);

#endif /* UI_NOTIFICATIONS_H_ */

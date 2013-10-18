#ifndef UI_NOTIFICATIONS_H_
#define UI_NOTIFICATIONS_H_

int ui_notification_dialog(dialog_type_t type, const char *fmt, ...);

int ui_disable_connect_button(void);

int ui_enable_connect_button(void);

int ui_file_chooser(void);

int ui_progress_bar_set_fraction(double fraction);

int ui_progress_bar_terminate(void);

#endif /* UI_NOTIFICATIONS_H_ */

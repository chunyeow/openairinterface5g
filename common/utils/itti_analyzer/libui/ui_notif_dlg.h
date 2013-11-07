#include <gtk/gtk.h>

#ifndef UI_NOTIF_DLG_H_
#define UI_NOTIF_DLG_H_

extern int ui_notification_dialog(GtkMessageType type, const char *title, const char *fmt, ...);

#endif /* UI_NOTIF_DLG_H_ */

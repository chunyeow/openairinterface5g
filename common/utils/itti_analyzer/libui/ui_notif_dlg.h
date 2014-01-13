#include <gtk/gtk.h>

#ifndef UI_NOTIF_DLG_H_
#define UI_NOTIF_DLG_H_

extern int ui_notification_dialog(GtkMessageType type, gboolean cancel, const char *title, const char *fmt, ...);

#endif /* UI_NOTIF_DLG_H_ */

#include <gtk/gtk.h>

#ifndef UI_NOTIF_DLG_H_
#define UI_NOTIF_DLG_H_

typedef enum dialog_type_e {
    DIALOG_INFO,
    DIALOG_WARNING,
    DIALOG_QUESTION,
    DIALOG_ERROR,
    DIALOG_OTHER,
    DIALOG_MAX
} dialog_type_t;

extern int ui_notification_dialog(dialog_type_t type, const char *fmt, ...);

#endif /* UI_NOTIF_DLG_H_ */

#include <pthread.h>
#include <stdint.h>
#include <unistd.h>

#include <gtk/gtk.h>

#include "ui_interface.h"

#include "ui_tree_view.h"
#include "ui_notifications.h"
#include "ui_signal_dissect_view.h"

#include "socket.h"
#include "xml_parse.h"

ui_interface_t ui_interface = {
    .dissector_ready = 0,

    /** core program -> UI **/
    .ui_notification_dialog       = ui_notification_dialog,
    .ui_disable_connect_button    = ui_disable_connect_button,
    .ui_enable_connect_button     = ui_enable_connect_button,
    .ui_progress_bar_set_fraction = ui_progress_bar_set_fraction,
    .ui_progress_bar_terminate    = ui_progress_bar_terminate,
    .ui_tree_view_new_signal_ind  = ui_tree_view_new_signal_ind,
    .ui_signal_set_text           = ui_signal_set_text,

    /** UI -> core program **/
    .socket_connect = socket_connect_to_remote_host,
    .socket_disconnect = socket_disconnect_from_remote_host,

    .parse_signal_file = xml_parse_file,
    .dissect_signal    = dissect_signal,
};

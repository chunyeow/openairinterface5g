#ifndef UI_INTERFACE_H_
#define UI_INTERFACE_H_

typedef enum dialog_type_e {
    DIALOG_INFO,
    DIALOG_WARNING,
    DIALOG_QUESTION,
    DIALOG_ERROR,
    DIALOG_OTHER,
    DIALOG_MAX
} dialog_type_t;

/**
 * socket_connect_t
 * @param remote_ip Remote ipv4 address
 * @param port Remote port number
 * @return RC_OK on Success, < 0 on failure
 **/
typedef int (*socket_connect_t)(const char *remote_ip, const uint16_t port);
typedef int (*socket_disconnect_t)(void);

typedef int (*parse_signal_file_t)(const char *filename);
typedef int (*dissect_signal_t)(const uint32_t message_number);

/**
 * ui_notification_dialog_t
 * @param type Type for the new dialog box
 * @param fmt String formater
 * @param ... argument list
 * @return RC_OK on Success, < 0 on failure
 **/
typedef int (*ui_notification_dialog_t)(dialog_type_t type, const char *fmt, ...);
typedef int (*ui_disable_connect_button_t)(void);
typedef int (*ui_enable_connect_button_t)(void);
typedef int (*ui_progress_bar_set_fraction_t)(double fraction);
typedef int (*ui_progress_bar_terminate_t)(void);

typedef int (*ui_tree_view_new_signal_ind_t)(const uint32_t message_number, const char *signal_name);
typedef int (*ui_signal_set_text_t)(char *text, int length);

typedef struct {
    /** UI -> core program */
    socket_connect_t    socket_connect;
    socket_disconnect_t socket_disconnect;
    parse_signal_file_t parse_signal_file;
    dissect_signal_t    dissect_signal;

    /** core program -> UI */
    int dissector_ready;
    ui_notification_dialog_t       ui_notification_dialog;
    ui_disable_connect_button_t    ui_disable_connect_button;
    ui_enable_connect_button_t     ui_enable_connect_button;
    ui_progress_bar_set_fraction_t ui_progress_bar_set_fraction;
    ui_progress_bar_terminate_t    ui_progress_bar_terminate;
    ui_tree_view_new_signal_ind_t  ui_tree_view_new_signal_ind;
    ui_signal_set_text_t           ui_signal_set_text;
} ui_interface_t;

extern ui_interface_t ui_interface;

#endif /* UI_INTERFACE_H_ */

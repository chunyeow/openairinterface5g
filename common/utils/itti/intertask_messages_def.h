/* This message asks for task termination */
MESSAGE_DEF(TERMINATE_MESSAGE,  MESSAGE_PRIORITY_MAX, struct {}, terminate_message)

/* Test message used for debug */
MESSAGE_DEF(MESSAGE_TEST,       MESSAGE_PRIORITY_MED, struct {}, message_test)

/* Error message  */
MESSAGE_DEF(ERROR_LOG,          MESSAGE_PRIORITY_MAX, struct {}, error_log)
/* Warning message  */
MESSAGE_DEF(WARNING_LOG,        MESSAGE_PRIORITY_MAX, struct {}, warning_log)

/* Generic log message for text */
MESSAGE_DEF(GENERIC_LOG,        MESSAGE_PRIORITY_MED, struct {}, generic_log)

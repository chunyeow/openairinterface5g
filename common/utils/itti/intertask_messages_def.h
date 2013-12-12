/* This message asks for task initialization */
MESSAGE_DEF(INITIALIZE_MESSAGE, MESSAGE_PRIORITY_MED, struct {}, initialize_message)

/* This message asks for task activation */
MESSAGE_DEF(ACTIVATE_MESSAGE,   MESSAGE_PRIORITY_MED, struct {}, activate_message)

/* This message asks for task deactivation */
MESSAGE_DEF(DEACTIVATE_MESSAGE, MESSAGE_PRIORITY_MED, struct {}, deactivate_message)

/* This message asks for task termination */
MESSAGE_DEF(TERMINATE_MESSAGE,  MESSAGE_PRIORITY_MAX, struct {}, terminate_message)

/* Test message used for debug */
MESSAGE_DEF(MESSAGE_TEST,       MESSAGE_PRIORITY_MED, struct {}, message_test)

/* Error message  */
MESSAGE_DEF(ERROR_LOG,          MESSAGE_PRIORITY_MAX, struct {}, error_log)
/* Warning message  */
MESSAGE_DEF(WARNING_LOG,        MESSAGE_PRIORITY_MAX, struct {}, warning_log)
/* Notice message  */
MESSAGE_DEF(NOTICE_LOG,         MESSAGE_PRIORITY_MED, struct {}, notice_log)
/* Info message  */
MESSAGE_DEF(INFO_LOG,           MESSAGE_PRIORITY_MED, struct {}, info_log)
/* Debug message  */
MESSAGE_DEF(DEBUG_LOG,          MESSAGE_PRIORITY_MED, struct {}, debug_log)

/* Generic log message for text */
MESSAGE_DEF(GENERIC_LOG,        MESSAGE_PRIORITY_MED, struct {}, generic_log)

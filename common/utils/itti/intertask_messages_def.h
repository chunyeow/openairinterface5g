/* This message asks for task termination */
MESSAGE_DEF(TERMINATE_MESSAGE, MESSAGE_PRIORITY_MAX, struct {}, terminate_message)
/* Test message used for debug */
MESSAGE_DEF(MESSAGE_TEST,      MESSAGE_PRIORITY_MAX, struct {}, message_test)
/* Test message used for debug */
MESSAGE_DEF(GENERIC_LOG,       MESSAGE_PRIORITY_MAX, char, generic_log)

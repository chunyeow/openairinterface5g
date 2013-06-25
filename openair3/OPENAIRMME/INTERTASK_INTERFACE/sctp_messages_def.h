//WARNING: Do not include this header directly. Use intertask_interface.h instead.

MESSAGE_DEF(SCTP_S1AP_INIT,         TASK_PRIORITY_MED, SctpS1APInit         sctpS1APInit)
MESSAGE_DEF(SCTP_NEW_DATA_REQ,      TASK_PRIORITY_MED, SctpNewDataReq       sctpNewDataReq)
MESSAGE_DEF(SCTP_CLOSE_ASSOCIATION, TASK_PRIORITY_MAX, SctpCloseAssociation sctpCloseAssociation)
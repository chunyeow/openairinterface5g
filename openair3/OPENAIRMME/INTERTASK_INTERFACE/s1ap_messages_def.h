//WARNING: Do not include this header directly. Use intertask_interface.h instead.

MESSAGE_DEF(S1AP_SCTP_NEW_MESSAGE_IND,    TASK_PRIORITY_MED, S1apSctpNewMessageInd s1apSctpNewMessageInd)
MESSAGE_DEF(S1AP_SCTP_ASSOCIATION_CLOSED, TASK_PRIORITY_MAX, S1apSctpAsscociationClosed  s1apSctpAssociationClosed)
MESSAGE_DEF(S1AP_NAS_NEW_MESSAGE_IND,     TASK_PRIORITY_MED, S1apNASNewMessageInd        s1apNASNewMessageInd)
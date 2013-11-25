// This task is mandatory and must always be placed in first position
TASK_DEF(TASK_TIMER, TASK_PRIORITY_MAX, 10)

// Other possible tasks in the process

/// Layer 2 and Layer 1 task supporting all the synchronous processing
TASK_DEF(TASK_L2L1, TASK_PRIORITY_MAX_LEAST, 10)
//// Layer 2 and Layer 1 sub-tasks
SUB_TASK_DEF(TASK_L2L1, TASK_PDCP_UE, 200)
SUB_TASK_DEF(TASK_L2L1, TASK_PDCP_ENB, 200)

SUB_TASK_DEF(TASK_L2L1, TASK_RLC_UE, 200)
SUB_TASK_DEF(TASK_L2L1, TASK_RLC_ENB, 200)

SUB_TASK_DEF(TASK_L2L1, TASK_MAC_UE, 200)
SUB_TASK_DEF(TASK_L2L1, TASK_MAC_ENB, 200)

SUB_TASK_DEF(TASK_L2L1, TASK_PHY_UE, 200)
SUB_TASK_DEF(TASK_L2L1, TASK_PHY_ENB, 200)

/// Radio Resource Control task for UE
TASK_DEF(TASK_RRC_UE, TASK_PRIORITY_MED, 200)
/// Radio Resource Control task for eNodeB
TASK_DEF(TASK_RRC_ENB, TASK_PRIORITY_MED, 200)

/// Bearers Manager task
TASK_DEF(TASK_BM, TASK_PRIORITY_MED, 200)

/// Non Access Stratum task for UE
TASK_DEF(TASK_NAS_UE, TASK_PRIORITY_MED, 200)

/// S1ap task for eNodeB
TASK_DEF(TASK_S1AP, TASK_PRIORITY_MED, 200)

/// Sctp task for eNodeB (Used by both S1AP and X2AP)
TASK_DEF(TASK_SCTP, TASK_PRIORITY_MED, 200)

/// X2ap task for eNodeB, acts as both source and target
TASK_DEF(TASK_X2AP, TASK_PRIORITY_MED, 200)

/// eNB APP task
TASK_DEF(TASK_ENB_APP, TASK_PRIORITY_MED, 200)

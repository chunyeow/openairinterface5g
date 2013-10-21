// This task is mandatory and must always be placed in first position
TASK_DEF(TASK_TIMER, TASK_PRIORITY_MAX)

// Other possible tasks in the process

/// Layer 2 and Layer 1 task supporting all the synchronous processing
TASK_DEF(TASK_L2L1, TASK_PRIORITY_MAX_LEAST)
//// Layer 2 and Layer 1 sub-tasks
SUB_TASK_DEF(TASK_L2L1, TASK_PDCP)
SUB_TASK_DEF(TASK_L2L1, TASK_RLC)
SUB_TASK_DEF(TASK_L2L1, TASK_MAC)
SUB_TASK_DEF(TASK_L2L1, TASK_PHY)

/// Radio Resource Control task for UE
TASK_DEF(TASK_RRC_UE, TASK_PRIORITY_MED)
/// Radio Resource Control task for eNodeB
TASK_DEF(TASK_RRC_ENB, TASK_PRIORITY_MED)

/// Bearers Manager task
TASK_DEF(TASK_BM, TASK_PRIORITY_MED)

/// Non Access Stratum task for UE
TASK_DEF(TASK_NAS_UE, TASK_PRIORITY_MED)

/// S1ap task for eNodeB
TASK_DEF(TASK_S1AP, TASK_PRIORITY_MED)

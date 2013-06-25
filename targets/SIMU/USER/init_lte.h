#include "PHY/types.h"
#include "PHY/defs.h"

PHY_VARS_eNB* init_lte_eNB(LTE_DL_FRAME_PARMS *frame_parms, 
			   u8 eNB_id,
			   u8 Nid_cell,
			   u8 cooperation_flag,
			   u8 transmission_mode,
			   u8 abstraction_flag);

PHY_VARS_UE* init_lte_UE(LTE_DL_FRAME_PARMS *frame_parms, 
			 u8 UE_id,
			 u8 abstraction_flag,
			 u8 transmission_mode);


void init_lte_vars(LTE_DL_FRAME_PARMS **frame_parms,
 		   u8 frame_type,
		   u8 tdd_config,
		   u8 tdd_config_S,
		   u8 extended_prefix_flag, 
		   u8 N_RB_DL,
		   u16 Nid_cell,
		   u8 cooperation_flag,
		   u8 transmission_mode,
		   u8 abstraction_flag,
		   int nb_antennas_rx);

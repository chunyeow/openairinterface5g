#include "PHY/defs.h"

#ifdef OPENAIR_LTE
int setup_oai_hw(LTE_DL_FRAME_PARMS *frame_parms);
void setup_ue_buffers(PHY_VARS_UE *phy_vars_ue, LTE_DL_FRAME_PARMS *frame_parms, int carrier);
void setup_eNB_buffers(PHY_VARS_eNB *phy_vars_eNB, LTE_DL_FRAME_PARMS *frame_parms);
#endif

#ifdef OPENAIR_ITS
int setup_oai_hw();
void setup_dot11_buffers(s32 **rxdata,s32 **txdata,int antenna_index);


#endif

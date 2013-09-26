/***************************************************************************
                          rrc_ue_init.c
                          -------------------
    copyright            : (C) 2008, 2010 by Eurecom
    created by           : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Initialization of RRC protocol entity for User Equipment
 ***************************************************************************/
/********************
// OpenAir includes
 ********************/
#include "LAYER2/MAC/extern.h"
#include "COMMON/openair_defs.h"
#include "RRC/L2_INTERFACE/openair_rrc_L2_interface.h"
#include "LAYER2/RLC/rlc.h"
#include "COMMON/mac_rrc_primitives.h"

//#include "SIMULATION/simulation_defs.h"
//extern EMULATION_VARS *Emul_vars;
//extern CH_MAC_INST *CH_mac_inst;
//extern UE_MAC_INST *UE_mac_inst;

/********************
// RRC includes
 ********************/
#include "rrc_ue_vars.h"
#include "as_configuration.h"
//#include "rrc_L2_proto.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_int.h"
#include "rrc_proto_fsm.h"
//#include "rrc_proto_intf.h"
#include "rrc_proto_bch.h"
#include "rrc_proto_mbms.h"
//-----------------------------------------------------------------------------

int *pt_nas_ue_irq;
u8  nas_IMEI[14];
#ifndef  USER_MODE
EXPORT_SYMBOL(pt_nas_ue_irq);
EXPORT_SYMBOL(nas_IMEI);
#endif

LCHAN_DESC BCCH_LCHAN_DESC,CCCH_LCHAN_DESC, DCCH_LCHAN_DESC, DTCH_DL_LCHAN_DESC,DTCH_UL_LCHAN_DESC;
rlc_info_t Rlc_info_um;
rlc_info_t Rlc_info_am_config;

//-------------------------------------------------------------------
void rrc_ue_get_mobile_id (void){
//-------------------------------------------------------------------
 // TEMP : This function sets the mobile ID - 1st version, hard coded
 // Later, will be retrieved from configuration to allow sevearl mobile terminals

  rrc_ue_mobileId = 0;
//#ifdef OAI_EMU
#ifdef RRC_OAI_EMU
  rrc_ue_mobileId = rrc_ethernet_id - 1;
  msg ("[RRC CELL]rrc_ue_get_mobile_id - EMULATION - UE_ID initialized to: %d\n", rrc_ue_mobileId);
#endif
#ifndef RRC_OAI_EMU
  msg ("[RRC CELL]rrc_ue_get_mobile_id - UE_ID not initialized. Use default value: %d\n", rrc_ue_mobileId);
#endif
}
//-------------------------------------------------------------------
int rrc_ue_get_initial_id (void){
//-------------------------------------------------------------------
  int UE_Id = 99;

//   #ifdef USER_MODE
//   int uni;
// 
//   UE_Id = rrc_ue_mobileId;
// 
//   uni = uniform ();
//   protocol_ms->rrc.IMEI[0] = (uni & 0x0F000000) >> 24;
//   protocol_ms->rrc.IMEI[1] = (uni & 0x000F0000) >> 16;
//   protocol_ms->rrc.IMEI[2] = (uni & 0x00000F00) >> 8;
//   protocol_ms->rrc.IMEI[3] = uni & 0x0000000F;
//   uni = uniform ();
//   protocol_ms->rrc.IMEI[4] = (uni & 0x0F000000) >> 24;
//   protocol_ms->rrc.IMEI[5] = (uni & 0x000F0000) >> 16;
//   protocol_ms->rrc.IMEI[6] = (uni & 0x00000F00) >> 8;
//   protocol_ms->rrc.IMEI[7] = uni & 0x0000000F;
//   uni = uniform ();
//   protocol_ms->rrc.IMEI[8] = (uni & 0x0F000000) >> 24;
//   protocol_ms->rrc.IMEI[9] = (uni & 0x000F0000) >> 16;
//   protocol_ms->rrc.IMEI[10] = (uni & 0x00000F00) >> 8;
//   protocol_ms->rrc.IMEI[11] = uni & 0x0000000F;
//   uni = uniform ();
//   protocol_ms->rrc.IMEI[12] = (uni & 0x0F000000) >> 24;
//   protocol_ms->rrc.IMEI[13] = (uni & 0x000F0000) >> 16;
//   #else

  protocol_ms->rrc.IMEI[0] = IMEI_D0;
  protocol_ms->rrc.IMEI[1] = IMEI_D1;
  protocol_ms->rrc.IMEI[2] = IMEI_D2;
  protocol_ms->rrc.IMEI[3] = IMEI_D3;
  protocol_ms->rrc.IMEI[4] = IMEI_D4;
  protocol_ms->rrc.IMEI[5] = IMEI_D5;
  protocol_ms->rrc.IMEI[6] = IMEI_D6;
  protocol_ms->rrc.IMEI[7] = IMEI_D7;
  protocol_ms->rrc.IMEI[8] = IMEI_D8;
  protocol_ms->rrc.IMEI[9] = IMEI_D9;
  protocol_ms->rrc.IMEI[10] = IMEI_D10;
  protocol_ms->rrc.IMEI[11] = IMEI_D11;
  protocol_ms->rrc.IMEI[12] = IMEI_D12;
  protocol_ms->rrc.IMEI[13] = rrc_ue_mobileId;
//   protocol_ms->rrc.IMEI[13] = IMEI_D13;
//   #endif
  UE_Id = 32- rrc_ue_mobileId;

  #ifdef DEBUG_RRC_STATE
   msg ("[RRC CELL]rrc_ue_get_initial_id (UE_ID %d) IMEI initialized to:", UE_Id);
   rrc_print_buffer ((char *)protocol_ms->rrc.IMEI, 14);
  #endif
  return UE_Id;
}

//-----------------------------------------------------------------------------
void rrc_ue_init (u8 Mod_id){
//-----------------------------------------------------------------------------
  //  int i;
  #ifdef DEBUG_RRC_STATE
  printk("\n\n***********************************************\n");
  printk("**************[RRC CELL][UE-INIT]**************\n");
  printk("***********************************************\n");
  #endif
  printk("[RRC CELL][UE-INIT] Init UE function start\n");

  // Initialize the control block for variables
  pool_buffer_init();
  protocol_ms = &prot_pool_ms;
  memset ((char *)protocol_ms, 0, sizeof (struct protocol_pool_ms));

  rrm_config = &rrc_as_config;
  memset ((char *)rrm_config, 0, sizeof (RRM_VARS));

  rrc_release_all_ressources = 0;

  protocol_ms->rrc.ue_wait_establish_req = 0;
  protocol_ms->rrc.protocol_state = RRC_UE_IDLE;
  protocol_ms->rrc.u_rnti = 0;
  protocol_ms->rrc.cell_id = 0;
  protocol_ms->rrc.mod_id = Mod_id; //Saved for future use in rrc_ue_L2_frontend.c
  //list2_init (&protocol_ms->rrc.rrc_timers, NULL);

  rrc_ue_bch_init ();
  rrc_rb_ue_init ();

  // initialise NAS global variables
  pt_nas_ue_irq = (int *) &(protocol_ms->rrc.ip_rx_irq);
  rrc_ue_get_mobile_id();  // Read mobile Id from command line parameters
  protocol_ms->rrc.ue_initial_id = rrc_ue_get_initial_id ();
  memcpy (nas_IMEI, (char *)protocol_ms->rrc.IMEI, 14);
  rrc_ue_fsm_init (protocol_ms->rrc.ue_initial_id);
  protocol_ms->rrc.rrc_currently_updating = FALSE;

  #ifdef USER_MODE
   #ifdef RRC_NETLINK
   rrc_ue_netlink_init ();
   #else
   rrc_ue_sap_init (); // init FIFOs towards NAS
   //qos_fifo_open ();
   #endif
  #endif
  //Initialise MBMS
  #ifdef ALLOW_MBMS_PROTOCOL
  rrc_ue_mbms_init();
  #endif

  rrc_init_mac_config();

  //
  printk("\n[RRC CELL][UE-INIT] cell_id %d\n",protocol_ms->rrc.cell_id );
  printk("[RRC CELL][UE-INIT] Init UE function completed\n");
  printk("***********************************************\n\n\n");

}


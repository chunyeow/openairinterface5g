/***************************************************************************
                          rrc_rg_dummies.c - description
                          -------------------
    begin               : April 2009
    copyright           : (C) 2010 by Eurecom
    created by		: Michelle.Wetterwald@eurecom.fr 
 **************************************************************************
      TEMP - This file contains dummy functions to be removed
 **************************************************************************/
//#define RRC_DEBUG_DUMMIES
/********************
//OpenAir definitions
 ********************/
#include "LAYER2/MAC/extern.h"
#include "UTIL/MEM/mem_block.h"

/********************
// RRC definitions
 ********************/
#include "rrc_rg_vars.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_int.h"
#include "rrc_proto_fsm.h"
#include "rrc_proto_intf.h"
#include "rrc_proto_bch.h"
#include "rrc_proto_mbms.h"


//void rc_uncompress_config (char *serialized_configP, int lengthP){
void rrc_rrm_rcve_config (u8 *serialized_configP, int lengthP){
  #ifdef RRC_DEBUG_DUMMIES
  msg ("\n[RRC][RG-DUMMIES] DUMMY CALL to rrc_rrm_rcve_config\n");
  #endif
}


//-----------------------------------------------------------------------------
// Temp reduced version - Keeps only RLC parameters
//-----------------------------------------------------------------------------
void rrc_compress_config (MT_CONFIG * config, char *dest_buffer, int *dest_length){
//-----------------------------------------------------------------------------
  int  i, j, k, el;
  char *outbuf = dest_buffer;

//#ifdef DEBUG_LOAD_CONFIG
  msg ("[RRC][CONFIG][COMPRESS] config size = %d -> %p\n", sizeof (MT_CONFIG), dest_buffer);
//#endif
  *outbuf++ = config->nb_commands;

  memcpy (outbuf, &config->rrm_commands[0], config->nb_commands * sizeof (RRM_COMMAND_MT));

  outbuf += config->nb_commands * sizeof (RRM_COMMAND_MT);

  for (i = 0; i < JRRM_MAX_COMMANDS_PER_TRANSACTION; i++) {
    el = config->rrm_commands[i].rrm_element_index;
    switch (config->rrm_commands[i].rrm_action) {
        case ACTION_ADD:
        case ACTION_MODIFY:
//#ifdef DEBUG_LOAD_CONFIG
          msg ("[RRC][CONFIG][COMPRESS] Command %d : action %d element %d index %d\n", i, config->rrm_commands[i].rrm_action, config->rrm_commands[i].rrm_element, el);
//#endif
          switch (config->rrm_commands[i].rrm_element) {
                //-------------------------------------------
              case SIGNALLING_RADIO_BEARER:
              case RADIO_ACCESS_BEARER:
                //-------------------------------------------
//#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][COMPRESS]  RB (%d)\n", (int) outbuf - (int) dest_buffer);
//#endif
                memcpy (outbuf, (void *) &config->bearer[el], sizeof (RADIOBEARER));
                outbuf += sizeof (RADIOBEARER);

/*                memcpy (outbuf, (void *) &config->bearer_ul[el], sizeof (RADIOBEARER));
                outbuf += sizeof (RADIOBEARER);*/
                break;
              default:
                break;
        }
          break;
          //-------------------------------------------
        default:
          //-------------------------------------------
          break;
    }

  }
  *dest_length = (int) outbuf - (int) dest_buffer;
  msg ("[RRC][CONFIG][COMPRESS] : Compressed config length = %d end @ %p\n", *dest_length, outbuf);
}


//-------------------------------------------------------------------
void rrc_uncompress_config (MT_CONFIG * config, char *source_buffer){
//-------------------------------------------------------------------
  //#ifdef RRC_DEBUG_DUMMIES
  msg ("\n[RRC][RG-DUMMIES] DUMMY CALL to rrc_uncompress_config\n");
  //#endif
}

/*
//-------------------------------------------------------------------
int rb_tx_data_srb_rg (u8 rb_idP, u8 * dataP, u32 length_in_bitsP, u32 muiP, u8 data_confirmP){
  #ifdef RRC_DEBUG_DUMMIES
  msg ("\n[RRC][RG-DUMMIES] DUMMY CALL to rb_tx_data_srb_rg\n");
  #endif
  return 0;
}
*/


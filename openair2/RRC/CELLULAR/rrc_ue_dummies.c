/***************************************************************************
                          rrc_ue_dummies.c - description
                          -------------------
    begin               : April 2009
    copyright           : (C) 2005, 2010 by Eurecom
    created by		: Michelle.Wetterwald@eurecom.fr 
 **************************************************************************
      TEMP - This file contains dummy functions to be removed
 **************************************************************************/
/********************
//OpenAir definitions
 ********************/
#include "LAYER2/MAC/extern.h"
#include "UTIL/MEM/mem_block.h"

/********************
// RRC definitions
 ********************/
#include "rrc_ue_vars.h"

void rrc_compress_config (MT_CONFIG * config, char *dest_buffer, int *dest_length){
  #ifdef RRC_DEBUG_DUMMIES
  msg ("\n[RRC][UE-DUMMIES] CALL to rrc_compress_config\n");
  #endif
}

void rrc_uncompress_config (MT_CONFIG * config, char *source_buffer){
  #ifdef RRC_DEBUG_DUMMIES
  msg ("\n[RRC][UE-DUMMIES] CALL to rrc_uncompress_config\n");
  #endif
}

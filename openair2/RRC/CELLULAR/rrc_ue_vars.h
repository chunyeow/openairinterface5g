/*********************************************************************
                          rrc_ue_vars.h  -  description
                             -------------------
    copyright            : (C) 2005, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 *********************************************************************
  Define control block memory for RRC_UE
 ********************************************************************/
#ifndef __RRC_UE_VARS_H__
#define __RRC_UE_VARS_H__

#include "COMMON/mac_rrc_primitives.h"
#include "rrc_platform_types.h"
#include "mem_pool.h"

#include "rrc_ue_entity.h"
//#include "rrc_L2_asconfig.h"


//-----------------------------------------------------------------------------
struct protocol_pool_ms {
  struct rrc_ue_entity rrc;
};

volatile struct protocol_pool_ms prot_pool_ms;
volatile struct protocol_pool_ms *protocol_ms;
RRC_XFACE *Rrc_xface;

RRM_VARS rrc_as_config;
RRM_VARS *rrm_config;

int rrc_release_all_ressources;
int rrc_ue_mobileId;
int rrc_ethernet_id;

#endif

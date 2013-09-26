/*********************************************************************
                          rrc_rg_vars.h  -  description
                             -------------------
    copyright            : (C) 2005, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 *********************************************************************
  Define control block memory for RRC_UE
 ********************************************************************/
#ifndef __RRC_RG_VARS_H__
#define __RRC_RG_VARS_H__

#include "COMMON/mac_rrc_primitives.h"
#include "rrc_platform_types.h"
#include "mem_pool.h"

#include "rrc_rg_entity.h"
//-----------------------------------------------------------------------------
struct protocol_pool_bs {
  struct rrc_rg_entity rrc;
};

volatile struct protocol_pool_bs prot_pool_bs;
volatile struct protocol_pool_bs *protocol_bs;
RRC_XFACE *Rrc_xface;

RRM_VARS rrc_as_config;
RRM_VARS *rrm_config;

rrc_rg_mbms_variables * volatile p_rg_mbms; /** pointer reference to protocol_bs->rrc.mbms */

int rrc_release_all_ressources;
int rrc_ethernet_id;
#endif

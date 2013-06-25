/***************************************************************************
                          rrc_messages.h  -  description
                             -------------------
    begin                : Jan 11, 2002
    copyright            : (C) 2001, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Define external interface primitive messages
 ***************************************************************************/
#ifndef __RRC_MESSAGES_H__
#define __RRC_MESSAGES_H__


#include "rrc_constant.h"
#include "rrc_nas_primitives.h"

/*****
 * RRC interface
 *****/
// Internal definitions, to work with the Esterel UE FSM
#define NAS_CONN_REQ       1
#define RRC_CONNECT_SETUP  3
#define RRC_CONNECT_REJECT 5
#define UE_RB_SETUP        7
#define UE_RB_RELEASE      9
#define CELLU_CNF          11
#define T300_TO            15
#define WAIT_TO						 17
#define RRC_RLC_SUCCESS    19
#define RRC_RLC_FAILURE    21
#define NAS_REL_REQ        23
#define UE_CRLC_STATUS     25
#define UE_PHY_SETUP       27
#define UE_PHY_FAIL        29
#define UE_CONN_LOSS       31
#define UE_CAP_INFO_CNF    33

// Internal definitions, to work with the Esterel RG FSM
#define RRC_CONNECT_REQ     2
#define RRC_CONNECT_LOSS    4
#define RRC_CONN_SETUP_COMP 6
#define RRC_CONNECT_RELUL   8
#define UE_RB_SU_CMP        10
#define UE_RB_SU_FAIL       12
#define UE_RB_REL_CMP       14
#define UE_RB_REL_FAIL      16
#define UE_CELLU            18
#define RG_RLC_SUCCESS      20
#define RG_RLC_FAILURE      22
#define NAS_CONN_CNF        24
#define NAS_RB_ESTAB        26
#define NAS_RB_RELEASE      28
#define RG_CRLC_STATUS      30
#define RRM_CFG             32
#define RG_PHY_SETUP        34
#define RG_PHY_FAIL         36
#define UE_PHY_OUTSYNCH     38
#define UE_PHY_SYNCH        40
#define UE_CAP_INFO         42
#define RRM_FAILURE         44


/*****
 * NAS interface
 *****/
#define NAS_SIB1  1
#define NAS_SIB18 18

#ifdef NODE_MT
struct nas_ue_if_element {
  u16 prim_length;
  int xmit_fifo;
  mem_block_t  *next;         // to chain before Xmit
  union {
    struct nas_ue_gc_element gc_sap_prim;
    struct nas_ue_dc_element dc_sap_prim;
    struct nas_ue_nt_element nt_sap_prim;
  } nasUePrimitive;
  u8  nasData[NAS_DATA_MAX_LENGTH];
};
#endif

#ifdef NODE_RG
struct nas_rg_if_element {
  u16 prim_length;
  int xmit_fifo;
  mem_block_t *next;         // to chain before Xmit
  union {
    struct nas_rg_gc_element gc_sap_prim;
    struct nas_rg_dc_element dc_sap_prim;
    struct nas_rg_nt_element nt_sap_prim;
  } nasRgPrimitive;
  u8  nasData[NAS_DATA_MAX_LENGTH];
};
#endif

#endif

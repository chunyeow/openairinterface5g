/*
                               rrm_config_structs.h
                             -------------------
  AUTHOR  : Linus GASSER modified by Lionel GAUTHIER Raymond KNOPP
  COMPANY : EURECOM
  EMAIL   : linus.gasser@epfl.ch
  EMAIL   : lionel.gauthier@eurecom.fr
  EMAIL   : raymond.knopp@eurecom.fr



 ***************************************************************************/

#    ifndef __RRM_CONFIG_STRUCTS_H__
#        define __RRM_CONFIG_STRUCTS_H__

#        include "platform_types.h"
#        include "platform_constants.h"
#        include "rrm_constants.h"
#        include "rlc.h"

typedef volatile struct {
  u32             e_r;
  int             timer_poll;
  int             timer_poll_prohibit;
  int             timer_discard;
  int             timer_poll_periodic;
  int             timer_status_prohibit;
  int             timer_status_periodic;
  int             timer_rst;
  int             timer_mrw;
  int             missing_pdu_indicator;
  u32             pdu_size;
//      int                                 in_sequence_delivery; // not implemented
  u8              max_rst;
  u8              max_dat;
  u16             poll_pdu;
  u16             poll_sdu;
  u8              poll_window;
  u32             tx_window_size;
  u32             rx_window_size;
  u8              max_mrw;
  u8              last_transmission_pdu_poll_trigger;
  u8              last_retransmission_pdu_poll_trigger;
  enum RLC_SDU_DISCARD_MODE sdu_discard_mode;
  u32             send_mrw;
} AM;

typedef volatile struct {
  u32             e_r;
  u32             timer_discard;
  u32             sdu_discard_mode;
  u32             segmentation_indication;
  u32             delivery_of_erroneous_sdu;
} TM;

typedef volatile struct {
  u32             e_r;
  u32             timer_discard;
  u32             sdu_discard_mode;
} UM;

typedef volatile struct {
  u8              logch_identity;
  u8              mac_logch_priority;
} MAP_INFO;

typedef volatile struct {
  u32             rlc_mode;
  AM              am;
  TM              tm;
  UM              um;
} RLC_INFO;

typedef volatile struct {
  int             header_compression_algorithm;
} PDCP_INFO;




typedef volatile struct {
  rb_type_t       rb_type;
  RLC_INFO        rlc_info;
  PDCP_INFO       pdcp_info;
  u16             rb_id;
  // Added for OPENAIR MAC
  //LCHAN_DESC      Lchan_desc;

} RADIOBEARER;


typedef volatile struct {
  int             TIMER300;
  int             TIMER302;
  int             TIMER305;
  int             TIMER307;
  int             TIMER308;
  int             TIMER312;
  int             TIMER313;
  int             TIMER314;
  int             TIMER315;
} L3TIMERS_;

typedef volatile struct {
  int             COUNTERN300;
  int             COUNTERN302;
  int             COUNTERN308;
  int             COUNTERN312;
  int             COUNTERN313;
} L3COUNTERS_;


typedef volatile struct {
  int             MaxNumRemote;
  L3TIMERS_       Timers;
  L3COUNTERS_     Counters;
} L3_;


typedef volatile struct {
  u8              rrm_action;   // ACTION_NULL,ADD,REMOVE,MODIFY
  u8              rrm_element;  // rb,trch,cctrch
  u8              rrm_element_index;    // rb/trch/cctrch index
} RRM_COMMAND_MT;

typedef volatile struct {
  u8              rrm_action;   // ACTION_NULL,ADD,REMOVE,MODIFY
  u8              mobile;
  u8              rrm_element;  // rb,trch,cctrch
  u8              rrm_element_index;    // rb/trch/cctrch index
} RRM_COMMAND_RG;


typedef volatile struct {
  u8              nb_commands;
  RRM_COMMAND_RG  rrm_commands[JRRM_MAX_COMMANDS_PER_TRANSACTION];


  RADIOBEARER     bearer[MAX_RB_RG];
  L3_             L3;

} RG_CONFIG;

typedef volatile struct {
  u8              nb_commands;
  RRM_COMMAND_MT  rrm_commands[JRRM_MAX_COMMANDS_PER_TRANSACTION];

  RADIOBEARER     bearer[MAX_RB_MOBILE];
  L3_             L3;

} MT_CONFIG;

#        ifdef NODE_RG
typedef volatile struct {
  RG_CONFIG       rg_config;
  MT_CONFIG       mt_config[MAX_MOBILES_PER_RG];
} RRM_VARS;

#        else
      /* NODE_RG */
typedef volatile struct {
  MT_CONFIG       mt_config[MAX_MANAGED_RG_PER_MOBILE];



} RRM_VARS;
#        endif
       /* NODE_RG */
//typedef   MT_CONFIG MAIN_MOBILE ;
//typedef   RG_CONFIG MAIN_RADIO_GATEWAY;

#    endif


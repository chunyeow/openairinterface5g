/*******************************************************************************
 *
 * Eurecom OpenAirInterface 3
 * Copyright(c) 2012 Eurecom
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 *
 * Contact Information
 * Openair Admin: openair_admin@eurecom.fr
 * Openair Tech : openair_tech@eurecom.fr
 * Forums       : http://forums.eurecom.fsr/openairinterface
 * Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France
 *
 *******************************************************************************/
/*! \file lteRALenb_main.h
 * \brief
 * \author GAUTHIER Lionel, MAUREL Frederic, WETTERWALD Michelle
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */

#ifndef __LTE_RAL_ENB_MAIN_H__
#define __LTE_RAL_ENB_MAIN_H__
//-----------------------------------------------------------------------------
#        ifdef LTE_RAL_ENB_MAIN_C
#            define private_lteralenb_main(x)    x
#            define protected_lteralenb_main(x)  x
#            define public_lteralenb_main(x)     x
#        else
#            ifdef LTE_RAL_ENB
#                define private_lteralenb_main(x)
#                define protected_lteralenb_main(x)  extern x
#                define public_lteralenb_main(x)     extern x
#            else
#                define private_lteralenb_main(x)
#                define protected_lteralenb_main(x)
#                define public_lteralenb_main(x)     extern x
#            endif
#        endif
//-----------------------------------------------------------------------------
#include "lteRALenb.h"
#include "commonDef.h"
#include "collection/hashtable/hashtable.h"
public_lteralenb_main(char*   g_conf_enb_ral_listening_port;)
public_lteralenb_main(char*   g_conf_enb_ral_ip_address;)
public_lteralenb_main(char*   g_conf_enb_ral_link_id;)
public_lteralenb_main(char*   g_conf_enb_ral_link_address;)
public_lteralenb_main(char*   g_conf_enb_mihf_remote_port;)
public_lteralenb_main(char*   g_conf_enb_mihf_ip_address;)
public_lteralenb_main(char*   g_conf_enb_mihf_id;)

/*
 * Radio Bearer data
 */
typedef struct ral_lte_channel {
    u32   cnx_id;
    u8    multicast;
// MIHF parameters  ix=0 UPLINK, ix=1 DOWNLINK
    u16   flowId[2];
    u16   classId[2];
    float resBitrate[2];
    float meanBitrate[2];
    float bktDepth[2];
    float pkBitrate[2];
    float MTU[2];

// #if ! defined(ENABLE_USE_MME)
    //IP driver parameters
    u16   rbId;
    u16   RadioQoSclass;
    u16   dscpUL;
    u16   dscpDL;
    u16   nas_state;
    u16   status;
// #endif
}ral_lte_channel_t;

/*
 * Mobile Terminal data
 */
typedef struct ral_lte_mt_s {
    /* The identifier of the link that is associated with a PoA */
    MIH_C_LINK_TUPLE_ID_T ltid;
    u8  ipv6_addr[16];
    u32 ipv6_l2id[2];
    u32 ue_id;
    struct ral_lte_channel radio_channel[RAL_MAX_RB];
    int num_rbs;
    int num_class;
    int nas_state;
    int mt_state;
}ral_lte_mt_t;


/*
 * Multicast data  // TEMP MW A supprimer!!!!
 */
typedef struct ral_lte_mcast_s {
    /* The identifier of the multicast link that is associated with a PoA */
    MIH_C_LINK_TUPLE_ID_T ltid;
    struct ral_lte_channel radio_channel;
    u8 mc_group_addr[16];
}ral_lte_mcast_t;

/*
 * RAL LTE internal data
 */

typedef struct lte_ral_enb_object {
    //------------------------
    // CONFIG PARAMETERS
    //------------------------
    char*                      ral_listening_port;
    char*                      ral_ip_address;
    char*                      ral_link_address;
    char*                      mihf_remote_port;
    char*                      mihf_ip_address;
    char*                      link_id;
    char*                      mihf_id;
    MIH_C_LINK_MIHCAP_FLAG_T   link_mihcap_flag; // hardcoded parameters
    MIH_C_NET_CAPS_T           net_caps;// hardcoded parameters

    // provided by RRC with RRC_RAL_SYSTEM_CONFIGURATION_IND message
    plmn_t                     plmn_id;
    unsigned int               cell_id:28;


    int num_connected_mts;

    u8 pending_req_flag;
    u8 pending_req_mt_ix;
    u8 pending_req_ch_ix;
    u8 pending_req_multicast;
//    u16 pending_req_transaction_id;
//    u8 pending_req_status;
    MIH_C_FLOW_ID_T pending_req_fid;

    ral_lte_mt_t pending_mt;
    int pending_mt_timer;
    int pending_mt_flag;

    ral_lte_mt_t         mt[RAL_MAX_MT];
    ral_lte_mcast_t      mcast;

    // measurements for MEDIEVAL project
    //MIH_C_TIMER_INTERVAL_T  measurement_timer_interval; // This timer value (ms) is used to set the interval between
                                                        // periodic reports. Valid Range: 0..65535
    //long                    measurement_timer_id;

    //u16                     num_UEs;
    //u32                     rlcBufferOccupancy[RAL_MAX_MT];
    //u32                     scheduledPRB[RAL_MAX_MT];
    //u32                     totalDataVolume[RAL_MAX_MT];
    //u32                     totalNumPRBs;


    //int                     congestion_flag;
    //int                     congestion_threshold;
    //int                     measures_triggered_flag;
    //int                     requested_period;

    // MIH-INTERFACE data
    int                        mih_sock_desc;
    MIH_C_LINK_AC_TYPE_LIST_T  mih_supported_link_action_list;
    MIH_C_LINK_EVENT_LIST_T    mih_supported_link_event_list;
    MIH_C_LINK_CMD_LIST_T      mih_supported_link_command_list;
    MIH_C_LINK_EVENT_LIST_T    mih_subscribe_req_event_list;

    //LIST(MIH_C_LINK_CFG_PARAM, mih_link_cfg_param_thresholds);
    // to tell what are the configured thresholds in mih_link_cfg_param_thresholds_list
    //MIH_C_BOOLEAN_T  active_mih_link_cfg_param_threshold[MIH_C_LINK_CFG_PARAM_LIST_LENGTH];


    MIH_C_LINK_AC_TYPE_T    pending_req_action;
    MIH_C_STATUS_T          pending_req_status;
    MIH_C_LINK_AC_RESULT_T  pending_req_ac_result;
    MIH_C_TRANSACTION_ID_T  pending_req_transaction_id;


    hash_table_t                 *ue_htbl;

    MIH_C_TRANSACTION_ID_T        transaction_id;

    char buffer[800];
} lte_ral_enb_object_t;



/* RAL LTE internal data  */
protected_lteralenb_main(lte_ral_enb_object_t  g_enb_ral_obj[MAX_MODULES];)
private_lteralenb_main(hash_table_t           *g_enb_ral_fd2instance;)

private_lteralenb_main(void  eRAL_get_IPv6_addr           (const char* if_name);)
public_lteralenb_main( void  eRAL_init_default_values     (void);)
private_lteralenb_main(int   eRAL_initialize              (void);)
private_lteralenb_main(void  eRAL_process_file_descriptors(struct epoll_event *events, int nb_events);)
public_lteralenb_main( void* eRAL_task                    (void *args_p);)

#endif

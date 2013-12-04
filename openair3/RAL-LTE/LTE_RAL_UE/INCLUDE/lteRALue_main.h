/***************************************************************************
                         lteRALue_main.h  -  description
 ***************************************************************************
  Eurecom OpenAirInterface 3
  Copyright(c) 1999 - 2013 Eurecom

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information
  Openair Admin: openair_admin@eurecom.fr
  Openair Tech : openair_tech@eurecom.fr
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 450 route des Chappes, 06410 Biot Sophia Antipolis, France
*******************************************************************************/
/*! \file lteRALue_main.h
 * \brief This file defines the prototypes of the functions for lteRALue_main.c
 * \author WETTERWALD Michelle, GAUTHIER Lionel, MAUREL Frederic
 * \date 2013
 * \company EURECOM
 * \email: michelle.wetterwald@eurecom.fr, lionel.gauthier@eurecom.fr, frederic.maurel@eurecom.fr
 */
/*******************************************************************************/
#ifndef __LTERALUE_MAIN_H__
#    define __LTERALUE_MAIN_H__
//-----------------------------------------------------------------------------
#        ifdef LTERALUE_MAIN_C
#            define private_mrallte_main(x)    x
#            define protected_mrallte_main(x)  x
#            define public_mrallte_main(x)     x
#        else
#            ifdef LTE_RAL_UE
#                define private_mrallte_main(x)
#                define protected_mrallte_main(x)  extern x
#                define public_mrallte_main(x)     extern x
#            else
#                define private_mrallte_main(x)
#                define protected_mrallte_main(x)
#                define public_mrallte_main(x)     extern x
#            endif
#        endif
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <libgen.h>
//-----------------------------------------------------------------------------
#include "lteRALue.h"
#include "commonDef.h"
#include "collection/hashtable/hashtable.h"
public_mrallte_main(char*   g_conf_ue_ral_listening_port;)
public_mrallte_main(char*   g_conf_ue_ral_ip_address;)
public_mrallte_main(char*   g_conf_ue_ral_link_id;)
public_mrallte_main(char*   g_conf_ue_ral_link_address;)
public_mrallte_main(char*   g_conf_ue_mihf_remote_port;)
public_mrallte_main(char*   g_conf_ue_mihf_ip_address;)
public_mrallte_main(char*   g_conf_ue_mihf_id;)


typedef int ral_ue_instance_t;

typedef struct lte_ral_ue_object_s {
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


    // only to call ralu_verifyPendingConnection
    u8 pending_req_flag;


 // network parameters
    u16 cell_id;
    u16 nas_state;
    int state;
    u32 curr_signal_level;
    u32 ipv6_l2id[2];
 //measures
    u8  req_num_bs;
    u16 req_cell_id[MAX_NUMBER_BS];
    u32 req_prov_id[MAX_NUMBER_BS];
    u16 req_order_index[MAX_NUMBER_BS];
    int num_measures;
    u16 meas_cell_id[MAX_NUMBER_BS];
    u32 last_meas_level[MAX_NUMBER_BS];
    u32 integrated_meas_level[MAX_NUMBER_BS];
    u32 prev_integrated_meas_level[MAX_NUMBER_BS];
    u32 provider_id[MAX_NUMBER_BS];
 //Radio Bearers
    u16 num_rb;
    int num_class;
    u16 rbId[RAL_MAX_RB];
    u16 QoSclass[RAL_MAX_RB];
    u16 dscp[RAL_MAX_RB];
 // statistics
    //u32 rx_packets;
    //u32 tx_packets;
    //u32 rx_bytes;
    //u32 tx_bytes;
    //u32 rx_errors;
    //u32 tx_errors;
    //u32 rx_dropped;
    //u32 tx_dropped;
    //char buffer[800];  // For ioctl with NAS driver

    // MIH-INTERFACE data
    // MIH-INTERFACE data
    int                        mih_sock_desc;

    // Initialised, then read-only, supported actions
    MIH_C_LINK_AC_TYPE_T       mih_supported_action_list;
    // action currently processed
    MIH_C_LINK_AC_TYPE_T       pending_req_action;
    // actions requested by MIH-H
    MIH_C_LINK_AC_TYPE_T       req_action_list;
    //MIH_C_STATUS_T             pending_req_status;
    MIH_C_LINK_AC_RESULT_T     pending_req_ac_result;
    //MIH_C_TRANSACTION_ID_T     pending_req_transaction_id;
    // set unset bits by MIH_C_Message_Link_Event_Subscribe_request MIH_C_Message_Link_Event_Unsubscribe_request
    MIH_C_LINK_EVENT_LIST_T    mih_subscribe_req_event_list;
    // Initialised, then read-only
    MIH_C_LINK_EVENT_LIST_T    mih_supported_link_event_list;
    // Initialised, then read-only
    MIH_C_LINK_CMD_LIST_T      mih_supported_link_command_list;
    LIST(MIH_C_LINK_CFG_PARAM, mih_link_cfg_param_thresholds);
    // to tell what are the configured thresholds in mih_link_cfg_param_thresholds_list
    MIH_C_BOOLEAN_T            active_mih_link_cfg_param_threshold[MIH_C_LINK_CFG_PARAM_LIST_LENGTH];
    MIH_C_BOOLEAN_T            link_to_be_detected;


    MIH_C_TRANSACTION_ID_T        transaction_id;

}lte_ral_ue_object_t;


/* RAL LTE internal data  */
protected_mrallte_main(lte_ral_ue_object_t   g_ue_ral_obj[MAX_MODULES];)
private_mrallte_main(hash_table_t           *g_ue_ral_fd2instance;);

public_mrallte_main( void  mRAL_init_default_values(void);)
public_mrallte_main( int   mRAL_initialize(void);)
private_mrallte_main(void  mRAL_process_file_descriptors(struct epoll_event *events, int nb_events);)
public_mrallte_main( void* mRAL_task(void *args_p);)

#endif







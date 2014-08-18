/*******************************************************************************
    OpenAirInterface 
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is 
   included in this distribution in the file called "COPYING". If not, 
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

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
#include "hashtable.h"
//#include "collection/hashtable/hashtable.h"

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
    uint8_t pending_req_flag;


 // network parameters
    uint16_t     cell_id;
    plmn_t  plmn_id;
    //uint16_t nas_state;
    int state;
    //uint32_t curr_signal_level;
    uint32_t ipv6_l2id[2];
 //measures
    uint8_t  req_num_bs;
    uint16_t req_cell_id[MAX_NUMBER_BS];
    uint32_t req_prov_id[MAX_NUMBER_BS];
    uint16_t req_order_index[MAX_NUMBER_BS];
    int num_measures;
    uint16_t meas_cell_id[MAX_NUMBER_BS];
    uint32_t last_meas_level[MAX_NUMBER_BS];
    uint32_t integrated_meas_level[MAX_NUMBER_BS];
    uint32_t prev_integrated_meas_level[MAX_NUMBER_BS];
    uint32_t provider_id[MAX_NUMBER_BS];
 //Radio Bearers
    uint16_t num_rb;
    int num_class;
    uint16_t rbId[RAL_MAX_RB];
    uint16_t QoSclass[RAL_MAX_RB];
    uint16_t dscp[RAL_MAX_RB];
 // statistics
    //uint32_t rx_packets;
    //uint32_t tx_packets;
    //uint32_t rx_bytes;
    //uint32_t tx_bytes;
    //uint32_t rx_errors;
    //uint32_t tx_errors;
    //uint32_t rx_dropped;
    //uint32_t tx_dropped;
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







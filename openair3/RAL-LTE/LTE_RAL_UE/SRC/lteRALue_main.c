/***************************************************************************
                         lteRALue_main.c  -  description
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
/*! \file lteRALue_main.c
 * \brief This file contains the main() function for the LTE-RAL-UE
 * \author WETTERWALD Michelle, GAUTHIER Lionel, MAUREL Frederic
 * \date 2013
 * \company EURECOM
 * \email: michelle.wetterwald@eurecom.fr, lionel.gauthier@eurecom.fr, frederic.maurel@eurecom.fr
 */
/*******************************************************************************/
#define LTE_RAL_UE
#define LTERALUE_MAIN_C
//-----------------------------------------------------------------------------
#include <stdio.h>
# include <sys/epoll.h>
#include <sys/select.h>
#include <net/if.h>
#include <getopt.h>
#include <stdlib.h>
#include <time.h>
//-----------------------------------------------------------------------------
#include "assertions.h"
#include "lteRALue.h"
#include "LAYER2/MAC/extern.h"
#include "intertask_interface.h"
#include "OCG.h"
//-----------------------------------------------------------------------------

extern unsigned char NB_eNB_INST;
extern unsigned char NB_UE_INST;
extern OAI_Emulation oai_emulation;


//---------------------------------------------------------------------------
void mRAL_get_IPv6_addr(void) {
//---------------------------------------------------------------------------
#define IPV6_ADDR_LINKLOCAL 0x0020U

#ifdef RAL_DUMMY
    char * eth0_name="eth0";/* interface name  */
#endif
#ifdef RAL_REALTIME
    char * graal0_name="oai0";/* interface name */
#endif

    FILE *f;
    char devname[20];
    int plen, scope, dad_status, if_idx;
    char addr6p[8][5];
    int intf_found = 0;
    char my_addr[16];
    char temp_addr[32];
    int i, j;

    if ((f = fopen("/proc/net/if_inet6", "r")) != NULL) {
        while (fscanf(f, "%4s%4s%4s%4s%4s%4s%4s%4s %02x %02x %02x %02x %20s\n",
                 addr6p[0], addr6p[1], addr6p[2], addr6p[3],
                 addr6p[4], addr6p[5], addr6p[6], addr6p[7],
                 &if_idx, &plen, &scope, &dad_status, devname) != EOF) {
#ifdef RAL_DUMMY
            if (!strcmp(devname, eth0_name)) {
#endif
#ifdef RAL_REALTIME
            if (!strcmp(devname, graal0_name)) {
#endif
                intf_found = 1;
                // retrieve numerical value
                if ((scope ==0)||(scope== IPV6_ADDR_LINKLOCAL)){
                    LOG_D(RAL_UE, " adresse  %s:%s:%s:%s:%s:%s:%s:%s",
                            addr6p[0], addr6p[1], addr6p[2], addr6p[3],
                            addr6p[4], addr6p[5], addr6p[6], addr6p[7]);
                    LOG_D(RAL_UE, " Scope:");
                    switch (scope) {
                        case 0:
                            LOG_D(RAL_UE, "Global");
                            break;
                        case IPV6_ADDR_LINKLOCAL:
                            LOG_D(RAL_UE, "Link");
                            break;
                        default:
                            LOG_D(RAL_UE, "Unknown");
                    }
                    LOG_D(RAL_UE, "\n Numerical value: ");
                    for (i = 0; i < 8; i++) {
                        for (j=0;j<4;j++){
                            addr6p[i][j]= toupper(addr6p[i][j]);
                            if ((addr6p[i][j] >= 'A') && (addr6p[i][j] <= 'F')){
                                temp_addr[(4*i)+j] =(unsigned short int)(addr6p[i][j]-'A')+10;
                            }else if ((addr6p[i][j] >= '0') && (addr6p[i][j] <= '9')){
                                temp_addr[(4*i)+j] =(unsigned short int)(addr6p[i][j]-'0');
                            }
                        }
                        my_addr[2*i]= (16*temp_addr[(4*i)])+temp_addr[(4*i)+1];
                        my_addr[(2*i)+1]= (16*temp_addr[(4*i)+2])+temp_addr[(4*i)+3];

                    }
                    for (i=0;i<16;i++){
                        LOG_D(RAL_UE, "-%hhx-",my_addr[i]);
                    }
                    LOG_D(RAL_UE, "\n\n");
                }
            }
        }
        fclose(f);
        if (!intf_found) {
            LOG_E(RAL_UE, "interface not found\n\n");
        }
    }
}


//---------------------------------------------------------------------------
void mRAL_init_default_values(void)
//---------------------------------------------------------------------------
{
    g_conf_ue_ral_listening_port  = UE_DEFAULT_LOCAL_PORT_RAL;
    g_conf_ue_ral_ip_address      = UE_DEFAULT_IP_ADDRESS_RAL;
    g_conf_ue_ral_link_id         = UE_DEFAULT_LINK_ID_RAL;
    g_conf_ue_ral_link_address    = UE_DEFAULT_LINK_ADDRESS_RAL;
    g_conf_ue_mihf_remote_port    = UE_DEFAULT_REMOTE_PORT_MIHF;
    g_conf_ue_mihf_ip_address     = UE_DEFAULT_IP_ADDRESS_MIHF;
    g_conf_ue_mihf_id             = UE_DEFAULT_MIHF_ID;
}

//---------------------------------------------------------------------------
int mRAL_initialize(void) {
    //---------------------------------------------------------------------------
    ral_ue_instance_t  instance  = 0;
    module_id_t        mod_id    = 0;
    char               *char_tmp = NULL;

    MIH_C_init();

    srand(time(NULL));

    memset(g_ue_ral_obj, 0, sizeof(lte_ral_ue_object_t)*MAX_MODULES);

    g_ue_ral_fd2instance = hashtable_create (32, NULL, hash_free_int_func);

    for (mod_id = oai_emulation.info.first_ue_local; mod_id < oai_emulation.info.first_ue_local+ oai_emulation.info.nb_ue_local; mod_id++) {

        instance = mod_id + NB_eNB_INST;
        char_tmp                                       = calloc(1, strlen(g_conf_ue_ral_listening_port) + 3); // 2 digits + \0 ->99 mod_ids
        sprintf(char_tmp,"%d", atoi(g_conf_ue_ral_listening_port) + mod_id);
        g_ue_ral_obj[mod_id].ral_listening_port      = char_tmp;

        g_ue_ral_obj[mod_id].ral_ip_address          = strdup(g_conf_ue_ral_ip_address);
        g_ue_ral_obj[mod_id].ral_link_address        = strdup(g_conf_ue_ral_link_address);

        char_tmp                                       = calloc(1, strlen(g_conf_ue_mihf_remote_port) + 3); // 2 digits + \0 ->99 mod_ids
        sprintf(char_tmp, "%d", atoi(g_conf_ue_mihf_remote_port) + mod_id);
        g_ue_ral_obj[mod_id].mihf_remote_port        = char_tmp;

        g_ue_ral_obj[mod_id].mihf_ip_address         = strdup(g_conf_ue_mihf_ip_address);

        char_tmp                                       = calloc(1, strlen(g_conf_ue_mihf_id) + 3); // 2 digits + \0 ->99 mod_ids
        sprintf(char_tmp, "%s%02d",g_conf_ue_mihf_id, mod_id);
        g_ue_ral_obj[mod_id].mihf_id                 = char_tmp;

        char_tmp                                       = calloc(1, strlen(g_conf_ue_ral_link_id) + 3); // 2 digits + \0 ->99 mod_ids
        sprintf(char_tmp, "%s%02d",g_conf_ue_ral_link_id, mod_id);
        g_ue_ral_obj[mod_id].link_id                 = char_tmp;

        char_tmp                                       = NULL;

        printf("g_ue_ral_obj[%d].link_id=%s\n", mod_id, g_ue_ral_obj[mod_id].link_id);
        // excluded MIH_C_LINK_AC_TYPE_NONE
        // excluded MIH_C_LINK_AC_TYPE_LINK_LOW_POWER
        g_ue_ral_obj[mod_id].mih_supported_action_list =  MIH_C_LINK_AC_TYPE_LINK_DISCONNECT            |
                                              MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN            |
                                              MIH_C_LINK_AC_TYPE_LINK_POWER_UP              |
                                              MIH_C_LINK_AC_TYPE_LINK_FLOW_ATTR             |
                                              MIH_C_LINK_AC_TYPE_LINK_ACTIVATE_RESOURCES    |
                                              MIH_C_LINK_AC_TYPE_LINK_DEACTIVATE_RESOURCES;



        g_ue_ral_obj[mod_id].mih_supported_link_event_list = MIH_C_BIT_LINK_DETECTED |
                                                  MIH_C_BIT_LINK_UP |
                                                  MIH_C_BIT_LINK_DOWN |
                                                  MIH_C_BIT_LINK_PARAMETERS_REPORT |
                                                  MIH_C_BIT_LINK_GOING_DOWN |
                                                  MIH_C_BIT_LINK_HANDOVER_IMMINENT |
                                                  MIH_C_BIT_LINK_HANDOVER_COMPLETE |
                                                  MIH_C_BIT_LINK_PDU_TRANSMIT_STATUS;

        g_ue_ral_obj[mod_id].mih_supported_link_command_list = MIH_C_BIT_LINK_EVENT_SUBSCRIBE | MIH_C_BIT_LINK_EVENT_UNSUBSCRIBE |
                                                  MIH_C_BIT_LINK_GET_PARAMETERS  | MIH_C_BIT_LINK_CONFIGURE_THRESHOLDS |
                                                  MIH_C_BIT_LINK_ACTION;

        g_ue_ral_obj[mod_id].link_to_be_detected = MIH_C_BOOLEAN_TRUE;




        g_ue_ral_obj[mod_id].link_mihcap_flag = MIH_C_BIT_EVENT_SERVICE_SUPPORTED | MIH_C_BIT_COMMAND_SERVICE_SUPPORTED | MIH_C_BIT_INFORMATION_SERVICE_SUPPORTED;

        g_ue_ral_obj[mod_id].net_caps = MIH_C_BIT_NET_CAPS_QOS_CLASS5 | MIH_C_BIT_NET_CAPS_INTERNET_ACCESS | MIH_C_BIT_NET_CAPS_MIH_CAPABILITY;


        g_ue_ral_obj[mod_id].transaction_id = (MIH_C_TRANSACTION_ID_T)rand();

        LOG_D(RAL_UE, " Connect to the MIH-F for module id instance %d...\n", mod_id,  instance);
        g_ue_ral_obj[mod_id].mih_sock_desc = -1;
        AssertFatal (mRAL_mihf_connect(instance) >= 0, "%s : Could not connect to MIH-F...\n", __FUNCTION__);
        itti_subscribe_event_fd(TASK_RAL_UE, g_ue_ral_obj[mod_id].mih_sock_desc);
        hashtable_insert(g_ue_ral_fd2instance, g_ue_ral_obj[mod_id].mih_sock_desc, (void*)instance);

        mRAL_send_link_register_indication(instance, &g_ue_ral_obj[mod_id].transaction_id);
        g_ue_ral_obj[mod_id].transaction_id += 1;
    }
    return 0;
}

void mRAL_process_file_descriptors(struct epoll_event *events, int nb_events)
{
    int                i;
    ral_ue_instance_t  instance;
    hashtable_rc_t     rc;

    if (events == NULL) {
        return;
    }

    for (i = 0; i < nb_events; i++) {
        rc = hashtable_get(g_ue_ral_fd2instance, events[i].data.fd, (void**)&instance);
        if (rc == HASH_TABLE_OK) {
            mRAL_mih_link_process_message(instance);
        }
    }
}

void* mRAL_task(void *args_p) {
    int                 nb_events;
    struct epoll_event *events;
    MessageDef         *msg_p    = NULL;
    const char         *msg_name = NULL;
    instance_t          instance  = 0;


    mRAL_initialize();

    itti_mark_task_ready (TASK_RAL_UE);

    // Set UE activation state
    for (instance = NB_eNB_INST; instance < (NB_eNB_INST + NB_UE_INST); instance++)
    {
        MessageDef *message_p;

        message_p = itti_alloc_new_message(TASK_RAL_UE, DEACTIVATE_MESSAGE);
        itti_send_msg_to_task(TASK_L2L1, instance, message_p);
    }

    while(1) {
        // Wait for a message
        itti_receive_msg (TASK_RAL_UE, &msg_p);

        if (msg_p != NULL) {

            msg_name = ITTI_MSG_NAME (msg_p);
            instance = ITTI_MSG_INSTANCE (msg_p);

            switch (ITTI_MSG_ID(msg_p)) {
                case TERMINATE_MESSAGE:
                    // TO DO
                    itti_exit_task ();
                    break;

                case TIMER_HAS_EXPIRED:
                    LOG_D(RAL_UE, "Received %s\n", msg_name);
                    break;

                case RRC_RAL_SCAN_CONF:
                    LOG_D(RAL_UE, "Received %s\n", msg_name);
                    mRAL_rx_rrc_ral_scan_confirm(instance, msg_p);
                    break;

                case RRC_RAL_SYSTEM_INFORMATION_IND:
                    LOG_D(RAL_UE, "Received %s\n", msg_name);
                    mRAL_rx_rrc_ral_system_information_indication(instance, msg_p);
                    break;

                case RRC_RAL_CONNECTION_ESTABLISHMENT_IND:
                    LOG_D(RAL_UE, "Received %s\n", msg_name);
                    mRAL_rx_rrc_ral_connection_establishment_indication(instance, msg_p);
                    break;

                case RRC_RAL_CONNECTION_REESTABLISHMENT_IND:
                    LOG_D(RAL_UE, "Received %s\n", msg_name);
                    mRAL_rx_rrc_ral_connection_reestablishment_indication(instance, msg_p);
                    break;

                case RRC_RAL_CONNECTION_RECONFIGURATION_IND:
                    LOG_D(RAL_UE, "Received %s\n", msg_name);
                    mRAL_rx_rrc_ral_connection_reconfiguration_indication(instance, msg_p);
                    break;

                case RRC_RAL_CONNECTION_RECONFIGURATION_HO_IND:
                    LOG_D(RAL_UE, "Received %s\n", msg_name);
                    //mRAL_rx_rrc_ral_connection_reconfiguration_ho_indication(instance, msg_p);
                    break;

                case RRC_RAL_MEASUREMENT_REPORT_IND:
                    LOG_D(RAL_UE, "Received %s\n", msg_name);
                    mRAL_rx_rrc_ral_measurement_report_indication(instance, msg_p);
                    break;

                case RRC_RAL_CONNECTION_RELEASE_IND:
                    LOG_D(RAL_UE, "Received %s\n", msg_name);
                    mRAL_rx_rrc_ral_connection_release_indication(instance, msg_p);
                    break;

                case RRC_RAL_CONFIGURE_THRESHOLD_CONF:
                    LOG_D(RAL_UE, "Received %s\n", msg_name);
                    mRAL_rx_rrc_ral_configure_threshold_conf(instance, msg_p);
                    break;
                default:
                    LOG_E(RAL_UE, "Received unexpected message %s\n", msg_name);
                    break;
            }
            itti_free (ITTI_MSG_ORIGIN_ID(msg_p), msg_p);
            msg_p = NULL;
        }
        nb_events = itti_get_events(TASK_RAL_UE, &events);
        /* Now handle notifications for other sockets */
        if (nb_events > 0) {
            mRAL_process_file_descriptors(events, nb_events);
        }
    }
}

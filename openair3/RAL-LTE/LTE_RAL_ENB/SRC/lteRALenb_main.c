/*****************************************************************************
 *   Eurecom OpenAirInterface 3
 *    Copyright(c) 2012 Eurecom
 *
 * Source lteRALenb_main.c
 * Version 0.1
 * Date  01/17/2014
 * Product MIH RAL LTE
 * Subsystem RAL main process running at the network side
 * Authors Michelle Wetterwald, Lionel Gauthier, Frederic Maurel
 * Description Implements the Radio Access Link process that interface the
 *  Media Independent Handover (MIH) Function to the LTE specific
 *  L2 media-dependent access layer.
 *
 *  The MIH Function provides network information to upper layers
 *  and requests actions from lower layers to optimize handovers
 *  between heterogeneous networks.
 *****************************************************************************/
#define LTE_RAL_ENB
#define LTE_RAL_ENB_MAIN_C
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/select.h>
#include <net/if.h>
#include <getopt.h>
#include <stdlib.h>
#include <time.h>
//-----------------------------------------------------------------------------
#include "assertions.h"
#include "lteRALenb.h"
#include "intertask_interface.h"
#include "OCG.h"
//-----------------------------------------------------------------------------



/****************************************************************************/
/*******************  G L O B A L    D E F I N I T I O N S  *****************/
/****************************************************************************/
extern OAI_Emulation oai_emulation;

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

/****************************************************************************
 ** Name:  get_IPv6_addr()                                                 **
 ** Description: Gets the IPv6 address of the specified network interface. **
 ** Inputs:  if_name Interface name                                        **
 ***************************************************************************/
void eRAL_get_IPv6_addr(const char* if_name){
    //-----------------------------------------------------------------------------
#define IPV6_ADDR_LINKLOCAL 0x0020U

    FILE *f;
    char devname[20];
    int plen, scope, dad_status, if_idx;
    char addr6p[8][5];
    int found = 0;
    char my_addr[16];
    char temp_addr[32];
    int i, j;

    LOG_D(RAL_ENB, " %s : network interface %s\n", __FUNCTION__, if_name);

    if ((f = fopen("/proc/net/if_inet6", "r")) != NULL) {
        while (fscanf(f, "%4s%4s%4s%4s%4s%4s%4s%4s %02x %02x %02x %02x %20s\n",
                addr6p[0], addr6p[1], addr6p[2], addr6p[3],
                addr6p[4], addr6p[5], addr6p[6], addr6p[7],
                &if_idx, &plen, &scope, &dad_status, devname) != EOF) {

            if (!strcmp(devname, if_name)) {
                found = 1;
                // retrieve numerical value
                if ((scope == 0) || (scope == IPV6_ADDR_LINKLOCAL)) {
                    LOG_D(RAL_ENB, " adresse  %s:%s:%s:%s:%s:%s:%s:%s",
                            addr6p[0], addr6p[1], addr6p[2], addr6p[3],
                            addr6p[4], addr6p[5], addr6p[6], addr6p[7]);
                    LOG_D(RAL_ENB, " Scope:");
                    switch (scope) {
                        case 0:
                            LOG_D(RAL_ENB, " Global\n");
                            break;
                        case IPV6_ADDR_LINKLOCAL:
                            LOG_D(RAL_ENB, " Link\n");
                            break;
                        default:
                            LOG_D(RAL_ENB, " Unknown\n");
                            break;
                    }
                    LOG_D(RAL_ENB, " Numerical value: ");
                    for (i = 0; i < 8; i++) {
                        for (j = 0; j < 4; j++) {
                            addr6p[i][j]= toupper(addr6p[i][j]);
                            if ((addr6p[i][j] >= 'A') && (addr6p[i][j] <= 'F')){
                                temp_addr[(4*i)+j] =(unsigned short int)(addr6p[i][j]-'A')+10;
                            } else if ((addr6p[i][j] >= '0') && (addr6p[i][j] <= '9')){
                                temp_addr[(4*i)+j] =(unsigned short int)(addr6p[i][j]-'0');
                            }
                        }
                        my_addr[2*i] = (16*temp_addr[(4*i)])+temp_addr[(4*i)+1];
                        my_addr[(2*i)+1] = (16*temp_addr[(4*i)+2])+temp_addr[(4*i)+3];

                    }
                    for (i = 0; i < 16; i++) {
                        LOG_D(RAL_ENB, "-%hhx-",my_addr[i]);
                    }
                    LOG_D(RAL_ENB, "\n");
                }
            }
        }
        fclose(f);
        if (!found) {
            LOG_E(RAL_ENB, " %s : interface %s not found\n\n", __FUNCTION__, if_name);
        }
    }
}

void eRAL_init_default_values(void) {
    g_conf_enb_ral_listening_port  = ENB_DEFAULT_LOCAL_PORT_RAL;
    g_conf_enb_ral_ip_address      = ENB_DEFAULT_IP_ADDRESS_RAL;
    g_conf_enb_ral_link_id         = ENB_DEFAULT_LINK_ID_RAL;
    g_conf_enb_ral_link_address    = ENB_DEFAULT_LINK_ADDRESS_RAL;
    g_conf_enb_mihf_remote_port    = ENB_DEFAULT_REMOTE_PORT_MIHF;
    g_conf_enb_mihf_ip_address     = ENB_DEFAULT_IP_ADDRESS_MIHF;
    g_conf_enb_mihf_id             = ENB_DEFAULT_MIHF_ID;
}

/****************************************************************************
 ** Name:  eRAL_initialize()                                               **
 **                                                                        **
 ** Description: Performs overall RAL LTE initialisations:                 **
 **                                                                        **
 ** Inputs: None                                                           **
 **                                                                        **
 ***************************************************************************/
int eRAL_initialize(void){
    //-----------------------------------------------------------------------------
    ral_enb_instance_t  instance = 0;
    unsigned int        mod_id   = 0;
    char               *char_tmp = NULL;

    MIH_C_init();

    srand(time(NULL));

    memset(g_enb_ral_obj, 0, sizeof(lte_ral_enb_object_t)*MAX_MODULES);

    g_enb_ral_fd2instance = hashtable_create (32, NULL, hash_free_int_func);

    for (mod_id = 0; mod_id < oai_emulation.info.nb_enb_local; mod_id++) {
        char_tmp                                        = calloc(1, strlen(g_conf_enb_ral_listening_port) + 3); // 2 digits + \0 ->99 instances
        instance = mod_id;
		
        sprintf(char_tmp,"%d", atoi(g_conf_enb_ral_listening_port) + mod_id);
        g_enb_ral_obj[mod_id].ral_listening_port      = char_tmp;

        g_enb_ral_obj[mod_id].ral_ip_address          = strdup(g_conf_enb_ral_ip_address);
        g_enb_ral_obj[mod_id].ral_link_address        = strdup(g_conf_enb_ral_link_address);

        char_tmp                                        = calloc(1, strlen(g_conf_enb_mihf_remote_port) + 3); // 2 digits + \0 ->99 instances
        sprintf(char_tmp, "%d", atoi(g_conf_enb_mihf_remote_port) + instance);
        g_enb_ral_obj[mod_id].mihf_remote_port        = char_tmp;

        g_enb_ral_obj[mod_id].mihf_ip_address         = strdup(g_conf_enb_mihf_ip_address);

        char_tmp                                        = calloc(1, strlen(g_conf_enb_mihf_id) + 3); // 2 digits + \0 ->99 instances
        sprintf(char_tmp, "%s%02d",g_conf_enb_mihf_id, instance);
        g_enb_ral_obj[mod_id].mihf_id                 = char_tmp;

        char_tmp                                        = calloc(1, strlen(g_conf_enb_ral_link_id) + 3); // 2 digits + \0 ->99 instances
        sprintf(char_tmp, "%s%02d",g_conf_enb_ral_link_id, mod_id);
        g_enb_ral_obj[mod_id].link_id                 = char_tmp;
        char_tmp                                        = NULL;

        // excluded MIH_C_LINK_AC_TYPE_NONE
        // excluded MIH_C_LINK_AC_TYPE_LINK_DISCONNECT
        // excluded MIH_C_LINK_AC_TYPE_LINK_LOW_POWER
        // excluded MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN
        // excluded MIH_C_LINK_AC_TYPE_LINK_POWER_UP
        g_enb_ral_obj[mod_id].mih_supported_link_action_list = (1 << MIH_C_LINK_AC_TYPE_LINK_FLOW_ATTR)  |
                (1 << MIH_C_LINK_AC_TYPE_LINK_ACTIVATE_RESOURCES) |
                (1 << MIH_C_LINK_AC_TYPE_LINK_DEACTIVATE_RESOURCES)|
                (1 << MIH_C_LINK_AC_TYPE_LINK_POWER_UP)|
                (1 << MIH_C_LINK_AC_TYPE_LINK_DISCONNECT)|
                (1 << MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN);
        // excluded MIH_C_BIT_LINK_DETECTED
        // excluded MIH_C_BIT_LINK_GOING_DOWN
        // excluded MIH_C_BIT_LINK_HANDOVER_IMMINENT
        // excluded MIH_C_BIT_LINK_HANDOVER_COMPLETE
        // excluded MIH_C_BIT_LINK_PDU_TRANSMIT_STATUS
        g_enb_ral_obj[mod_id].mih_supported_link_event_list = MIH_C_BIT_LINK_UP | MIH_C_BIT_LINK_DOWN | MIH_C_BIT_LINK_PARAMETERS_REPORT;
        // excluded MIH_C_BIT_LINK_GET_PARAMETERS
        // excluded MIH_C_BIT_LINK_CONFIGURE_THRESHOLDS
        g_enb_ral_obj[mod_id].mih_supported_link_command_list = MIH_C_BIT_LINK_EVENT_SUBSCRIBE  | MIH_C_BIT_LINK_CONFIGURE_THRESHOLDS | MIH_C_BIT_LINK_ACTION |
                MIH_C_BIT_LINK_EVENT_UNSUBSCRIBE |
                MIH_C_BIT_LINK_ACTION;

        g_enb_ral_obj[mod_id].link_mihcap_flag = MIH_C_BIT_EVENT_SERVICE_SUPPORTED | MIH_C_BIT_COMMAND_SERVICE_SUPPORTED | MIH_C_BIT_INFORMATION_SERVICE_SUPPORTED;

        g_enb_ral_obj[mod_id].net_caps = MIH_C_BIT_NET_CAPS_QOS_CLASS5 | MIH_C_BIT_NET_CAPS_INTERNET_ACCESS | MIH_C_BIT_NET_CAPS_MIH_CAPABILITY;


        g_enb_ral_obj[mod_id].transaction_id = (MIH_C_TRANSACTION_ID_T)rand();


        g_enb_ral_obj[mod_id].ue_htbl = hashtable_create(32, NULL, NULL);

        LOG_D(RAL_ENB, " Connect to the MIH-F for module id instance %d...\n", mod_id,  instance);
        g_enb_ral_obj[mod_id].mih_sock_desc = -1;
        AssertFatal(eRAL_mihf_connect(instance) >= 0, " %s : Could not connect to MIH-F...\n", __FUNCTION__);
        itti_subscribe_event_fd(TASK_RAL_ENB, g_enb_ral_obj[mod_id].mih_sock_desc);
        hashtable_insert(g_enb_ral_fd2instance, g_enb_ral_obj[mod_id].mih_sock_desc, (void*)instance);
        
        eRAL_send_link_register_indication(instance, &g_enb_ral_obj[mod_id].transaction_id);
        g_enb_ral_obj[mod_id].transaction_id += 1;
    }
    return 0;
}

void eRAL_process_file_descriptors(struct epoll_event *events, int nb_events)
{
    int                i;
    ral_enb_instance_t instance;
    hashtable_rc_t     rc;

    if (events == NULL) {
        return;
    }

    for (i = 0; i < nb_events; i++) {
        rc = hashtable_get(g_enb_ral_fd2instance, events[i].data.fd, (void**)&instance);
        if (rc == HASH_TABLE_OK) {
            eRAL_mih_link_process_message(instance);
        }
    }
}

void* eRAL_task(void *args_p) {
    int                 nb_events;
    struct epoll_event *events;
    MessageDef         *msg_p    = NULL;
    const char         *msg_name = NULL;
    instance_t          instance  = 0;


    eRAL_initialize();
    itti_mark_task_ready (TASK_RAL_ENB);

    while(1) {
        // Wait for a message
        itti_receive_msg (TASK_RAL_ENB, &msg_p);

        if (msg_p != NULL) {

            msg_name = ITTI_MSG_NAME (msg_p);
            instance = ITTI_MSG_INSTANCE (msg_p);

            switch (ITTI_MSG_ID(msg_p)) {
                case TERMINATE_MESSAGE:
                    // TO DO
                    itti_exit_task ();
                    break;

                case TIMER_HAS_EXPIRED:
                    LOG_D(RAL_ENB, "Received %s\n", msg_name);
                    break;

                case RRC_RAL_SYSTEM_CONFIGURATION_IND:
                    LOG_D(RAL_ENB, "Received %s\n", msg_name);
                    eRAL_rx_rrc_ral_system_configuration_indication(instance, msg_p);
                    break;

                case RRC_RAL_CONNECTION_ESTABLISHMENT_IND:
                    LOG_D(RAL_ENB, "Received %s\n", msg_name);
                    eRAL_rx_rrc_ral_connection_establishment_indication(instance, msg_p);
                    break;

                case RRC_RAL_CONNECTION_REESTABLISHMENT_IND:
                    LOG_D(RAL_ENB, "Received %s\n", msg_name);
                    eRAL_rx_rrc_ral_connection_reestablishment_indication(instance, msg_p);
                    break;

                case RRC_RAL_CONNECTION_RECONFIGURATION_IND:
                    LOG_D(RAL_ENB, "Received %s\n", msg_name);
                    eRAL_rx_rrc_ral_connection_reconfiguration_indication(instance, msg_p);
                    break;

                case RRC_RAL_MEASUREMENT_REPORT_IND:
                    LOG_D(RAL_ENB, "Received %s\n", msg_name);
                    eRAL_rx_rrc_ral_measurement_report_indication(instance, msg_p);
                    break;

                case RRC_RAL_CONNECTION_RELEASE_IND:
                    LOG_D(RAL_ENB, "Received %s\n", msg_name);
                    eRAL_rx_rrc_ral_connection_release_indication(instance, msg_p);
                    break;

                case RRC_RAL_CONFIGURE_THRESHOLD_CONF:
                    LOG_D(RAL_ENB, "Received %s\n", msg_name);
                    eRAL_rx_rrc_ral_configure_threshold_conf(instance, msg_p);
                    break;
                default:
                    LOG_E(RAL_ENB, "Received unexpected message %s\n", msg_name);
                    break;
            }
            itti_free (ITTI_MSG_ORIGIN_ID(msg_p), msg_p);
            msg_p = NULL;
        }
        nb_events = itti_get_events(TASK_RAL_ENB, &events);
        /* Now handle notifications for other sockets */
        if (nb_events > 0) {
            eRAL_process_file_descriptors(events, nb_events);
        }
    }
}


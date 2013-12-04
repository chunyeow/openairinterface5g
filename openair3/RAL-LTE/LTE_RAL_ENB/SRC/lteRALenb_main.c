/*****************************************************************************
 *   Eurecom OpenAirInterface 3
 *    Copyright(c) 2012 Eurecom
 *
 * Source eRALlte_main.c
 * Version 0.1
 * Date  06/22/2012
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
# include <sys/epoll.h>
#include <sys/select.h>
#include <net/if.h>
#include <getopt.h>
#include <stdlib.h>
#include <time.h>
//-----------------------------------------------------------------------------
#include "lteRALenb.h"
#include "intertask_interface.h"
#include "OCG.h"
//-----------------------------------------------------------------------------

// LTE AS sub-system
//#include "nas_ue_ioctl.h"

#ifdef RAL_REALTIME
//LG#include "rrc_nas_primitives.h"
//LG#include "nasrg_constant.h"
//LG#include "nasrg_iocontrol.h"
#endif


/****************************************************************************/
/*******************  G L O B A L    D E F I N I T I O N S  *****************/
/****************************************************************************/
extern OAI_Emulation oai_emulation;

//int init_flag = 0;

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

// static struct ral_lte_priv rl_priv;
// //
// static void arg_usage(const char* name);
// static int parse_opts(int argc, char* argv[]);
// static void NAS_Netlink_socket_init(void);
// static void get_IPv6_addr(const char* if_name);
// static int RAL_initialize(int argc, const char *argv[]);

//struct ral_lte_priv rl_priv;

// void arg_usage(const char* name);
// int parse_opts(int argc, char* argv[]);
// void get_IPv6_addr(const char* if_name);
// int RAL_initialize(int argc, const char *argv[]);



/****************************************************************************/
// Next part is used to receive the triggers
/****************************************************************************/
/*
#ifdef MUSER_CONTROL

#define USER_IP_ADDRESS             "127.0.0.1"
#define USER_REMOTE_PORT            "0"
#define NAS_IP_ADDRESS              "127.0.0.1"
#define NAS_LISTENING_PORT_FOR_USER "22222"

int          g_sockd_user;
signed int   g_user_congestion    = 0;
unsigned int g_ratio_modif        = 0;
//---------------------------------------------------------------------------
int lteRALenb_trigger_connect(void){
//---------------------------------------------------------------------------
    struct addrinfo      hints;
    struct addrinfo     *result, *rp;
    int                  s, on;
    struct sockaddr_in  *addr  = NULL;
    struct sockaddr_in6 *addr6 = NULL;
    unsigned char        buf[sizeof(struct sockaddr_in6)];


    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = AF_UNSPEC;    // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_DGRAM;   // Datagram socket
    hints.ai_flags    = 0;
    hints.ai_protocol = 0;            // Any protocol

    s = getaddrinfo(g_mih_user_ip_address, g_mih_user_remote_port, &hints, &result);
    if (s != 0) {
    	LOG_E (RAL_ENB,"getaddrinfo: %s\n", gai_strerror(s));
        return -1;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        g_sockd_user = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (g_sockd_user == -1)
            continue;

        on = 1;
        setsockopt( g_sockd_user, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

        if(rp->ai_family == AF_INET) {
        	LOG_D (RAL_ENB, "Destination address  %s is an ipv4 address\n",g_mih_user_ip_address);
            addr             = (struct sockaddr_in *)(&buf[0]);
            addr->sin_port   = htons(atoi(g_nas_listening_port_for_mih_user));
            addr->sin_family = AF_INET;
            s = inet_pton(AF_INET, g_nas_ip_address, &addr->sin_addr);
            if (s <= 0) {
                if (s == 0) {
                	LOG_E (RAL_ENB, " IP address should be a IPv4 ADDR - But found not in presentation format : %s\n", g_nas_ip_address);
                } else {
                	LOG_E (RAL_ENB, " %s - inet_pton( IPv4 ADDR %s): %s\n", __FUNCTION__, g_nas_ip_address, strerror(s));
                }
                return -1;
            }

            s = bind(g_sockd_user, (const struct sockaddr *)addr, sizeof(struct sockaddr_in));
            if (s == -1) {
            	LOG_D (RAL_ENB, "IPv4 Address Bind: %s\n", strerror(errno));
                return -1;
            }
            // sockd_mihf is of type SOCK_DGRAM, rp->ai_addr is the address to which datagrams are sent by default
            if (connect(g_sockd_user, rp->ai_addr, rp->ai_addrlen) != -1) {
            	LOG_D (RAL_ENB, "  lteRALeNB is now ready to receive triggers\n");
                return 0;
            } else {
                close(g_sockd_user);
            }
        } else if (rp->ai_family == AF_INET6) {
        	LOG_D (RAL_ENB, "Destination address  %s is an ipv6 address\n",g_mih_user_ip_address);
            addr6              = (struct sockaddr_in6 *)(&buf[0]);
            addr6->sin6_port   = htons(atoi(g_nas_listening_port_for_mih_user));
            addr6->sin6_family = AF_INET6;
            s = inet_pton(AF_INET, g_nas_ip_address, &addr6->sin6_addr);
            if (s <= 0) {
                if (s == 0) {
                	LOG_E (RAL_ENB, "IP  address should be a IPv6 ADDR, But found not in presentation format : %s\n", g_nas_ip_address);
                } else {
                	LOG_E (RAL_ENB, "%s - inet_pton( IPv6 ADDR %s): %s\n", __FUNCTION__, g_nas_ip_address, strerror(s));
                }
                return -1;
            }

            s = bind(g_sockd_user, (const struct sockaddr *)addr6, sizeof(struct sockaddr_in));
            if (s == -1) {
            	LOG_D (RAL_ENB, "IPv6 Address Bind: %s\n", strerror(errno));
                return -1;
            }
            if (connect(g_sockd_user, rp->ai_addr, rp->ai_addrlen) != -1) {
            	LOG_D (RAL_ENB, "lteRALeNB is now ready to receive triggers\n");
                return 0;
            } else {
                close(g_sockd_user);
            }
        } else {
        	LOG_E (RAL_ENB, "%s is an unknown address format %d\n",g_mih_user_ip_address,rp->ai_family);
        }
        close(g_sockd_user);
    }

    if (rp == NULL) {   // No address succeeded
    	LOG_E (RAL_ENB, "Could not establish socket to MIH-User\n");
        return -1;
    }
    return -1;
}

//---------------------------------------------------------------------------
int lteRALenb_trigger_receive(int sock){
//---------------------------------------------------------------------------
  unsigned char str[50];
  int  t, done;
    t=recv(sock, str, 50, 0);
    if (t <= 0) {
        if (t < 0) perror("lteRALenb_trigger_receive : recv");
        done = 1;
    }
    printf("\nmessage from USER, length:  %d\n", t);
    switch (str[0]) {
        case 0xff:
            printf("USER ASK FOR STOPPING CONGESTION - not supported yet\n");
            break;
        case 0x01:
            printf("USER ASK FOR TRIGGERING CONGESTION\n");
            g_enb_ral_obj[instanceP].rlcBufferOccupancy[0] = 95;
            RAL_NAS_report_congestion(0);
            break;
        default:
            printf("received %hx\n", str[0]);
            return -1;
    }
    return 0;
}

#endif
 */
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
    char               *char_tmp = NULL;

    MIH_C_init();

    srand(time(NULL));

    memset(g_enb_ral_obj, 0, sizeof(lte_ral_enb_object_t)*MAX_MODULES);

    g_enb_ral_fd2instance = hashtable_create (32, NULL, hash_free_int_func);

    for (instance = 0; instance < oai_emulation.info.nb_enb_local; instance++) {
        char_tmp                                        = calloc(1, strlen(g_conf_enb_ral_listening_port) + 3); // 2 digits + \0 ->99 instances
        sprintf(char_tmp,"%d", atoi(g_conf_enb_ral_listening_port) + instance);
        g_enb_ral_obj[instance].ral_listening_port      = char_tmp;

        g_enb_ral_obj[instance].ral_ip_address          = strdup(g_conf_enb_ral_ip_address);
        g_enb_ral_obj[instance].ral_link_address        = strdup(g_conf_enb_ral_link_address);

        char_tmp                                        = calloc(1, strlen(g_conf_enb_mihf_remote_port) + 3); // 2 digits + \0 ->99 instances
        sprintf(char_tmp, "%d", atoi(g_conf_enb_mihf_remote_port) + instance);
        g_enb_ral_obj[instance].mihf_remote_port        = char_tmp;

        g_enb_ral_obj[instance].mihf_ip_address         = strdup(g_conf_enb_mihf_ip_address);

        char_tmp                                        = calloc(1, strlen(g_conf_enb_mihf_id) + 3); // 2 digits + \0 ->99 instances
        sprintf(char_tmp, "%s%02d",g_conf_enb_mihf_id, instance);
        g_enb_ral_obj[instance].mihf_id                 = char_tmp;

        char_tmp                                        = calloc(1, strlen(g_conf_enb_ral_link_id) + 3); // 2 digits + \0 ->99 instances
        sprintf(char_tmp, "%s%02d",g_conf_enb_ral_link_id, instance);
        g_enb_ral_obj[instance].link_id                 = char_tmp;
        char_tmp                                        = NULL;

        // excluded MIH_C_LINK_AC_TYPE_NONE
        // excluded MIH_C_LINK_AC_TYPE_LINK_DISCONNECT
        // excluded MIH_C_LINK_AC_TYPE_LINK_LOW_POWER
        // excluded MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN
        // excluded MIH_C_LINK_AC_TYPE_LINK_POWER_UP
        g_enb_ral_obj[instance].mih_supported_link_action_list = (1 << MIH_C_LINK_AC_TYPE_LINK_FLOW_ATTR)  |
                (1 << MIH_C_LINK_AC_TYPE_LINK_ACTIVATE_RESOURCES) |
                (1 << MIH_C_LINK_AC_TYPE_LINK_DEACTIVATE_RESOURCES);
        // excluded MIH_C_BIT_LINK_DETECTED
        // excluded MIH_C_BIT_LINK_GOING_DOWN
        // excluded MIH_C_BIT_LINK_HANDOVER_IMMINENT
        // excluded MIH_C_BIT_LINK_HANDOVER_COMPLETE
        // excluded MIH_C_BIT_LINK_PDU_TRANSMIT_STATUS
        g_enb_ral_obj[instance].mih_supported_link_event_list = MIH_C_BIT_LINK_UP | MIH_C_BIT_LINK_DOWN | MIH_C_BIT_LINK_PARAMETERS_REPORT;
        // excluded MIH_C_BIT_LINK_GET_PARAMETERS
        // excluded MIH_C_BIT_LINK_CONFIGURE_THRESHOLDS
        g_enb_ral_obj[instance].mih_supported_link_command_list = MIH_C_BIT_LINK_EVENT_SUBSCRIBE  | MIH_C_BIT_LINK_CONFIGURE_THRESHOLDS |
                MIH_C_BIT_LINK_EVENT_UNSUBSCRIBE |
                MIH_C_BIT_LINK_ACTION;

        g_enb_ral_obj[instance].link_mihcap_flag = MIH_C_BIT_EVENT_SERVICE_SUPPORTED | MIH_C_BIT_COMMAND_SERVICE_SUPPORTED | MIH_C_BIT_INFORMATION_SERVICE_SUPPORTED;

        g_enb_ral_obj[instance].net_caps = MIH_C_BIT_NET_CAPS_QOS_CLASS5 | MIH_C_BIT_NET_CAPS_INTERNET_ACCESS | MIH_C_BIT_NET_CAPS_MIH_CAPABILITY;


        g_enb_ral_obj[instance].transaction_id = (MIH_C_TRANSACTION_ID_T)rand();

        //LOG_N(RAL_ENB, "[MSC_NEW][%s][MIH-F=%s]\n", getTimeStamp4Log(), g_mihf_id);
        //LOG_N(RAL_ENB, "[MSC_NEW][%s][RAL=%s]\n", getTimeStamp4Log(), g_link_id);
        //LOG_N(RAL_ENB, "[MSC_NEW][%s][NAS=%s]\n", getTimeStamp4Log(), "nas");


        g_enb_ral_obj[instance].ue_htbl = hashtable_create(32, NULL, NULL);

        LOG_D(RAL_ENB, " Connect to the MIH-F for instance %d...\n", instance);
        g_enb_ral_obj[instance].mih_sock_desc = -1;
        if (eRAL_mihf_connect(instance) < 0 ) {
            LOG_E(RAL_ENB, " %s : Could not connect to MIH-F...\n", __FUNCTION__);
            // TO DO RETRY LATER
            //exit(-1);
        } else {
            itti_subscribe_event_fd(TASK_RAL_ENB, g_enb_ral_obj[instance].mih_sock_desc);
            hashtable_insert(g_enb_ral_fd2instance, g_enb_ral_obj[instance].mih_sock_desc, (void*)instance);
        }
    }





    /*Initialize the NAS driver communication channel
     */
#ifdef RAL_REALTIME
    //LG IAL_NAS_ioctl_init();
#endif
#ifdef RAL_DUMMY
    //LG NAS_Netlink_socket_init();
    //LG LOG_D(RAL_ENB, " Waiting for a connection from the NAS Driver ...\n");
    //LG t = sizeof(nas_socket);
    //LG if ((g_sockd_nas = accept(netl_s, (struct sockaddr *)&nas_socket, &t)) == -1) {
    //LG     perror("RAL_initialize : g_sockd_nas - accept() failed");
    //LG     exit(1);
    //LG }
#endif
    LOG_D(RAL_ENB, "NAS Driver Connected.\n\n");

    /* Start listening to user commands for triggers*/
#ifdef MUSER_CONTROL
    //LG lteRALenb_trigger_connect();
#endif


    /*Get the interface IPv6 address
     */
#ifdef RAL_DUMMY
    //LG get_IPv6_addr("eth0");
#else
#ifdef RAL_REALTIME
    //LG get_IPv6_addr("oai0");
#endif
#endif

    //  Get list of MTs
    //LG LOG_D(RAL_ENB, "Obtaining list of MTs\n\n");
#ifdef RAL_REALTIME
    //init_flag=1;
    //LG RAL_process_NAS_message(IO_OBJ_CNX, IO_CMD_LIST,0,0);
    //LG RAL_process_NAS_message(IO_OBJ_RB, IO_CMD_LIST,0,0);
    //init_flag=0;
#endif
#ifdef RAL_DUMMY
    //LG eRALlte_NAS_get_MTs_list();
#endif
//    RAL_printInitStatus(0);
//    g_enb_ral_obj[instanceP].pending_req_flag = 0;
//    //
//    g_enb_ral_obj[instanceP].pending_mt_timer = -1;
//    g_enb_ral_obj[instanceP].pending_mt_flag = 0;
//    //
//    LOG_D(RAL_ENB, " List of MTs initialized\n\n");

//    // Initialize measures for demo3
//    g_enb_ral_obj[instanceP].meas_polling_interval = RAL_DEFAULT_MEAS_POLLING_INTERVAL;
//    g_enb_ral_obj[instanceP].meas_polling_counter = 1;

//    g_enb_ral_obj[instanceP].congestion_flag = RAL_FALSE;
//    g_enb_ral_obj[instanceP].measures_triggered_flag = RAL_FALSE;
//    g_enb_ral_obj[instanceP].congestion_threshold = RAL_DEFAULT_CONGESTION_THRESHOLD;
//    transaction_id = (MIH_C_TRANSACTION_ID_T)0;

//    eRALlte_send_link_register_indication(&transaction_id);

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
            free(msg_p);
            msg_p = NULL;
        }
        nb_events = itti_get_events(TASK_RAL_ENB, &events);
        /* Now handle notifications for other sockets */
        if (nb_events > 0) {
            eRAL_process_file_descriptors(events, nb_events);
        }
    }
}

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/
/*int main(int argc, const char *argv[]){
//-----------------------------------------------------------------------------
    int            rc, done;
    fd_set         readfds;
    struct timeval tv;
    int time_counter = 1;

    RAL_initialize(argc, argv);

    g_enb_ral_obj[instanceP].pending_mt_timer = 0;

    done = 0;
    do {
 // Initialize fd_set and wait for input
        FD_ZERO(&readfds);
        FD_SET(g_sockd_mihf, &readfds);
        #ifdef RAL_DUMMY
        FD_SET(g_sockd_nas, &readfds);
        #endif
        #ifdef MUSER_CONTROL
        FD_SET (g_sockd_user, &readfds);
        #endif
        tv.tv_sec  = MIH_C_RADIO_POLLING_INTERVAL_SECONDS;
        tv.tv_usec = MIH_C_RADIO_POLLING_INTERVAL_MICRO_SECONDS;

        rc = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);
        if (rc < 0) {
            perror("main : select() failed");
            done = 1;
        }
        // Something is ready for being read
        else if (rc >= 0){
          // Read data coming from the MIH Function
          if (FD_ISSET(g_sockd_mihf, &readfds)) {
              done = eRALlte_mih_link_process_message();
          }
          #ifdef RAL_DUMMY
          // Read data coming from the NAS driver
            if (FD_ISSET(g_sockd_nas, &readfds)) {
                //printf("Received something from NAS\n");
                done = eRALlte_NAS_process_message();
            }
          #endif
          #ifdef MUSER_CONTROL
          // Get triggers
          if (FD_ISSET(g_sockd_user,&readfds)){
              done = lteRALenb_trigger_receive(g_sockd_user);
          }
          #endif

          // Wait until next pending MT's timer expiration
          if (g_enb_ral_obj[instanceP].pending_mt_timer > 0) {
              g_enb_ral_obj[instanceP].pending_mt_timer --;
              eRALlte_process_verify_pending_mt_status();
          }

          if (time_counter ++ == 11){
             // check if a new MT appeared or disappeared
           #ifdef RAL_REALTIME
             RAL_process_NAS_message(IO_OBJ_CNX, IO_CMD_LIST,0,0);
           #endif
             time_counter = 1;
          }
            //get measures from NAS - timer = 21x100ms  -- impair
          if (g_enb_ral_obj[instanceP].meas_polling_counter ++ == g_enb_ral_obj[instanceP].meas_polling_interval){
              RAL_NAS_measures_polling();
              g_enb_ral_obj[instanceP].meas_polling_counter =1;
          }

        }
    } while (!done);

    close(g_sockd_mihf);
    MIH_C_exit();
    return 0;
}
 */

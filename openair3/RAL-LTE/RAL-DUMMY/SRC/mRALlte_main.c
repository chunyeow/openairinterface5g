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
#define MRAL_MODULE
#define MRALLTE_MAIN_C
//-----------------------------------------------------------------------------
#include "mRALlte_main.h"
#include "mRALlte_constants.h"
#include "mRALlte_variables.h"
#include "mRALlte_proto.h"
#include "mRALlte_mih_msg.h"
#include "nas_ue_netlink.h"
#include "nasUE_config.h"
//-----------------------------------------------------------------------------
#include "MIH_C.h"
//-----------------------------------------------------------------------------

#define NAS_UE_NETL_MAXLEN 500
// TO DO
#ifndef SVN_REV
#define SVN_REV   "0.1"
#endif
// Global variables
int netl_s, s_nas;
struct sockaddr_un ralu_socket;
int wait_start_mihf;
int listen_mih;
struct ral_lte_priv rl_priv;
struct ral_lte_priv *ralpriv;


char message[NAS_UE_NETL_MAXLEN];
static int  g_log_output;
//-----------------------------------------------------------------------------
static void arg_usage(char *exec_nameP) {
//-----------------------------------------------------------------------------
    fprintf(stderr,
            "Usage: %s [options]\nOptions:\n"
            "  -V,          --version             Display version information\n"
            "  -?, -h,      --help                Display this help text\n"
            "  -P <number>, --ral-listening-port  Listening port for incoming MIH-F messages\n"
            "  -I <string>, --ral-ip-address      Binding IP(v4 or v6) address for RAL\n"
            "  -p <number>, --mihf-remote-port    MIH-F remote port\n"
            "  -i <string>, --mihf-ip-address     MIH-F IP(v4 or v6) address\n"
            "  -c,          --output-to-console   All stream outputs are redirected to console\n"
            "  -f,          --output-to-syslog    All stream outputs are redirected to file\n"
            "  -s,          --output-to-syslog    All stream outputs are redirected to syslog\n",
            exec_nameP);
}

//---------------------------------------------------------------------------
int parse_opts(int argc, char *argv[]) {
//---------------------------------------------------------------------------
    static struct option long_opts[] = {
        {"version", 0, 0, 'V'},
        {"help", 0, 0, 'h'},
        {"ral-listening-port", optional_argument, 0, 'P'},
        {"ral-ip-address",     optional_argument, 0, 'I'},
        {"mihf-remote-port",   optional_argument, 0, 'p'},
        {"mihf-ip-address",    optional_argument, 0, 'i'},
        {"link.id",            optional_argument, 0, 'l'},
        {"mihf.id",            optional_argument, 0, 'm'},
        {"output-to-console",  0, 0, 'c'},
        {"output-to-file",     0, 0, 'f'},
        {"output-to-syslog",   0, 0, 's'},
        {0, 0, 0, 0}
    };

    /* parse all other cmd line parameters than -c */
    while (1) {
        int idx, c;
        c = getopt_long(argc, argv, "PIpil:Vh?cfs", long_opts, &idx);
        if (c == -1) break;

        switch (c) {
            case 'V':
                fprintf(stderr, "SVN MODULE VERSION: %s\n", SVN_REV);
                return -1;
            case '?':
            case 'h':
                arg_usage(basename(argv[0]));
                return -1;
            case 'i':
                fprintf(stderr, "Option mihf-ip-address:\t%s\n", optarg);
                g_mihf_ip_address = optarg;
                break;
            case 'p':
                fprintf(stderr, "Option mihf-remote-port:\t%s\n", optarg);
                g_mihf_remote_port = optarg;
                break;
            case 'P':
                fprintf(stderr, "Option ral-listening-port:\t%s\n", optarg);
                g_ral_listening_port_for_mihf = optarg;
                break;
            case 'I':
                fprintf(stderr, "Option ral-ip-address:\t%s\n", optarg);
                g_ral_ip_address = optarg;
                break;
            case 'l':
                fprintf(stderr, "Option link.id:\t%s\n", optarg);
                g_link_id = optarg;
                break;
            case 'm':
                fprintf(stderr, "Option mihf.id:\t%s\n", optarg);
                g_mihf_id = optarg;
                break;
            case 'c':
                fprintf(stderr, "Option output-to-console\n");
                g_log_output = LOG_TO_CONSOLE;
                break;
            case 'f':
                fprintf(stderr, "Option output-to-file\n");
                g_log_output = LOG_TO_FILE;
                break;
            case 's':
                fprintf(stderr, "Option output-to-syslog\n");
                g_log_output = LOG_TO_SYSTEM;
                break;
            default:
                WARNING("UNKNOWN OPTION\n");
                break;
        };
    }
    return 0;
}
//---------------------------------------------------------------------------
void IAL_D_Netlink_socket_init(void){
//---------------------------------------------------------------------------
    int len;
    struct sockaddr_un local;
    if ((netl_s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("IAL_D_Netlink_socket_init : socket command failed, err %d\n");
        exit(1);
    }

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, SOCK_NAS_PATH);
    unlink(local.sun_path);
    len = strlen(local.sun_path) + sizeof(local.sun_family);

    if (bind(netl_s, (struct sockaddr *)&local, len) == -1) {
        perror("IAL_D_Netlink_socket_init : bind command failed, \n");
        exit(1);
    }

    if (listen(netl_s, 1) == -1) {
        perror("IAL_D_Netlink_socket_init : listen command failed, \n");
        exit(1);
    }
}
//---------------------------------------------------------------------------
void mRALlte_get_IPv6_addr(void) {
//---------------------------------------------------------------------------
#define IPV6_ADDR_LINKLOCAL 0x0020U

#ifdef RAL_DUMMY
    char * eth0_name="eth0";/* interface name  */
#endif
#ifdef RAL_REALTIME
    char * graal0_name="graal0";/* interface name */
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
                    DEBUG(" adresse  %s:%s:%s:%s:%s:%s:%s:%s",
                            addr6p[0], addr6p[1], addr6p[2], addr6p[3],
                            addr6p[4], addr6p[5], addr6p[6], addr6p[7]);
                    DEBUG(" Scope:");
                    switch (scope) {
                        case 0:
                            DEBUG("Global");
                            break;
                        case IPV6_ADDR_LINKLOCAL:
                            DEBUG("Link");
                            break;
                        default:
                            DEBUG("Unknown");
                    }
                    DEBUG("\n Numerical value: ");
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
                        DEBUG("-%hhx-",my_addr[i]);
                    }
                    DEBUG("\n\n");
                }
            }
        }
        fclose(f);
        if (!intf_found) {
            ERR("interface not found\n\n");
        }
    }
}
//---------------------------------------------------------------------------
int inits(int argc, char *argv[]) {
    //---------------------------------------------------------------------------
    MIH_C_TRANSACTION_ID_T           transaction_id;
    unsigned int                     t;
    struct sockaddr_un               nas_socket;

    ralpriv = &rl_priv;
    memset(ralpriv, 0, sizeof(struct ral_lte_priv));

    memset(g_msg_codec_recv_buffer, 0, MSG_CODEC_RECV_BUFFER_SIZE);
    // Initialize defaults
    g_ral_ip_address                = DEFAULT_IP_ADDRESS_RAL;
    g_ral_listening_port_for_mihf   = DEFAULT_LOCAL_PORT_RAL;
    g_mihf_remote_port              = DEFAULT_REMOTE_PORT_MIHF;
    g_mihf_ip_address               = DEFAULT_IP_ADDRESS_MIHF;
    g_link_id                       = DEFAULT_LINK_ID;
    g_mihf_id                       = DEFAULT_MIHF_ID;
    g_sockd_mihf                    = -1;
    g_log_output                    = LOG_TO_CONSOLE;

    MIH_C_init(g_log_output);

    if (parse_opts( argc, argv) < 0) {
        exit(0);
    }


    if (mRALlte_mihf_connect() < 0 ) {
        ERR("Could not connect to MIH-F...exiting\n");
        exit(-1);
    }
    DEBUG("MT-MIHF socket initialized.\n\n");

    // excluded MIH_C_LINK_AC_TYPE_NONE
    // excluded MIH_C_LINK_AC_TYPE_LINK_LOW_POWER
    // excluded MIH_C_LINK_AC_TYPE_NONE
    ralpriv->mih_supported_action_list =  MIH_C_LINK_AC_TYPE_LINK_DISCONNECT            |
                                          MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN            |
                                          MIH_C_LINK_AC_TYPE_LINK_POWER_UP              |
                                          MIH_C_LINK_AC_TYPE_LINK_FLOW_ATTR             |
                                          MIH_C_LINK_AC_TYPE_LINK_ACTIVATE_RESOURCES    |
                                          MIH_C_LINK_AC_TYPE_LINK_DEACTIVATE_RESOURCES;



    ralpriv->mih_supported_link_event_list = MIH_C_BIT_LINK_DETECTED |
                                              MIH_C_BIT_LINK_UP |
                                              MIH_C_BIT_LINK_DOWN |
                                              MIH_C_BIT_LINK_PARAMETERS_REPORT |
                                              MIH_C_BIT_LINK_GOING_DOWN |
                                              MIH_C_BIT_LINK_HANDOVER_IMMINENT |
                                              MIH_C_BIT_LINK_HANDOVER_COMPLETE |
                                              MIH_C_BIT_LINK_PDU_TRANSMIT_STATUS;

    ralpriv->mih_supported_link_command_list = MIH_C_BIT_LINK_EVENT_SUBSCRIBE | MIH_C_BIT_LINK_EVENT_UNSUBSCRIBE | \
                                              MIH_C_BIT_LINK_GET_PARAMETERS  | MIH_C_BIT_LINK_CONFIGURE_THRESHOLDS | \
                                              MIH_C_BIT_LINK_ACTION;

    ralpriv->link_to_be_detected = MIH_C_BOOLEAN_TRUE;

    NOTICE("[MSC_NEW][%s][MIH-F=%s]\n", getTimeStamp4Log(), g_mihf_id);
    NOTICE("[MSC_NEW][%s][RAL=%s]\n", getTimeStamp4Log(), g_link_id);
    NOTICE("[MSC_NEW][%s][NAS=%s]\n", getTimeStamp4Log(), "nas");

    IAL_D_Netlink_socket_init();
    DEBUG("Waiting for a connection from NAS Driver ...\n");
    t = sizeof(nas_socket);
    if ((s_nas = accept(netl_s, (struct sockaddr *)&nas_socket, &t)) == -1) {
        perror("main - s_nas - accept ");
        exit(1);
    }
    DEBUG("NAS Driver Connected.\n\n");


    // get interface ipv6 address
    mRALlte_get_IPv6_addr();
    // get L2 identifier
    IAL_process_DNAS_message(IO_OBJ_IMEI, IO_CMD_ADD, 0);

    IAL_decode_NAS_message();

    // Initialize measurements
    IAL_NAS_measures_init();
    ralpriv->state = DISCONNECTED;

    IAL_process_DNAS_message(IO_OBJ_MEAS, IO_CMD_LIST, ralpriv->cell_id);
    IAL_decode_NAS_message();


    transaction_id = (MIH_C_TRANSACTION_ID_T)0;
    mRALlte_send_link_register_indication(&transaction_id);

    return 0;
}

//---------------------------------------------------------------------------
int main(int argc, char *argv[]){
//---------------------------------------------------------------------------
    int            rc, done;
    int            meas_polling_counter;
    fd_set         readfds;
    struct timeval tv;

    inits(argc, argv);

    done                       = 0;
    ralpriv->pending_req_flag = 0;
    meas_polling_counter       = 1;

    do{
        // Create fd_set and wait for input;
        FD_ZERO(&readfds);
        FD_SET(g_sockd_mihf, &readfds);
        FD_SET (s_nas, &readfds);
        tv.tv_sec  = MIH_C_RADIO_POLLING_INTERVAL_SECONDS;
        tv.tv_usec = MIH_C_RADIO_POLLING_INTERVAL_MICRO_SECONDS;

        rc= select(FD_SETSIZE, &readfds, NULL, NULL, &tv);
        if(rc == -1) {
            perror("select");
            done = 1;
        }

        //something received!
        if(rc >= 0){
            if(FD_ISSET(g_sockd_mihf, &readfds)){
                done=mRALlte_mih_link_process_message();
            } /*else { // tick
                mRALlte_mih_fsm(NULL, 0);
            }*/
            if (FD_ISSET(s_nas,&readfds)){
                //printf("\n something received s_nas\n");
                  done = IAL_decode_NAS_message();
            }

            //get measures from NAS - timer = 21x100ms  -- impair
//            if (meas_polling_counter ++ == 51){
            if (meas_polling_counter ++ == 51){
                IAL_NAS_measures_update(meas_polling_counter);
                rallte_NAS_measures_polling();
                meas_polling_counter =1;
            }

            if (ralpriv->pending_req_flag > 0){  //wait until next time
                ralpriv->pending_req_flag ++;
                rallte_verifyPendingConnection();
            }

        }
    }while(!done);

    close(g_sockd_mihf);
    close(netl_s);
    MIH_C_exit();
    return 0;
}

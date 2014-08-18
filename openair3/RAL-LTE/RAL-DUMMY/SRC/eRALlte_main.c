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

#include <sys/select.h>
#include <getopt.h>

#define DEFINE_GLOBAL_CONSTANTS
#include "eRALlte_constants.h"
#include "eRALlte_variables.h"
#include "eRALlte_proto.h"
#include "eRALlte_mih_msg.h"

#include "MIH_C.h"

/****************************************************************************/
/*******************  G L O C A L    D E F I N I T I O N S  *****************/
/****************************************************************************/

/* NAS socket file descritor		*/
int g_sockd_nas;		// referenced in eRALlte_NAS.c

/* RAL LTE internal data		*/
struct ral_lte_priv *ralpriv;

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

static int g_log_output;
static struct ral_lte_priv rl_priv;

static void arg_usage(const char* name);
static int parse_opts(int argc, char* argv[]);
static void NAS_Netlink_socket_init(void);
static void get_IPv6_addr(const char* if_name);

static int RAL_initialize(int argc, const char *argv[]);

static int netl_s;	/* NAS net link socket	*/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

int main(int argc, const char *argv[])
{
    int            rc, done;
    fd_set         readfds;
    struct timeval tv;

    RAL_initialize(argc, argv);

    ralpriv->pending_mt_timer = 0;

    done = 0;
    do {
	/* Initialize fd_set and wait for input */
        FD_ZERO(&readfds);
        FD_SET(g_sockd_mihf, &readfds);
        FD_SET(g_sockd_nas, &readfds);
        tv.tv_sec  = MIH_C_RADIO_POLLING_INTERVAL_SECONDS;
        tv.tv_usec = MIH_C_RADIO_POLLING_INTERVAL_MICRO_SECONDS;

        rc = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);
        if (rc < 0) {
            perror("main : select() failed");
            done = 1;
        }

        /* Something is ready for being read */
        else if (rc >= 0)
	{
	    /* Read data coming from the MIH Function */
            if (FD_ISSET(g_sockd_mihf, &readfds)) {
                done = eRALlte_mih_link_process_message();
            }

	    /* Read data coming from the NAS driver */
            if (FD_ISSET(g_sockd_nas, &readfds)) {
		done = eRALlte_NAS_process_message();
            }

	    /* Wait until next pending MT's timer expiration */
            if (ralpriv->pending_mt_timer > 0) {
                ralpriv->pending_mt_timer --;
		eRALlte_process_verify_pending_mt_status();
            }
	}
    } while (!done);

    free(g_ral_ip_address);
    free(g_ral_listening_port_for_mihf);
    free(g_mihf_remote_port);
    free(g_mihf_ip_address);
    free(g_link_id);
    free(g_mihf_id);

    close(g_sockd_mihf);
    close(netl_s);
    MIH_C_exit();
    return 0;
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:	 arg_usage()                                               **
 **                                                                        **
 ** Description: Displays command line usage                               **
 **                                                                        **
 ** Inputs:	 name:		Name of the running process                **
 ** 	 	 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	None                                       **
 ** 	 	 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
static void arg_usage(const char *name)
{
    fprintf(stderr,
            "Usage: %s [options]\nOptions:\n"
            "  -V,          --version             Display version information\n"
            "  -?, -h,      --help                Display this help text\n"
            "  -P <number>, --ral-listening-port  Listening port for incoming MIH-F messages\n"
            "  -I <string>, --ral-ip-address      Binding IP(v4 or v6) address for RAL\n"
            "  -p <number>, --mihf-remote-port    MIH-F remote port\n"
            "  -i <string>, --mihf-ip-address     MIH-F IP(v4 or v6) address\n"
	    "  -l <number>, --mihf-link-id        MIH-F link identifier\n"
	    "  -m <number>, --mihf-id             MIH-F identifier\n"
            "  -c,          --output-to-console   All stream outputs are redirected to console\n"
            "  -f,          --output-to-file      All stream outputs are redirected to file\n"
            "  -s,          --output-to-syslog    All stream outputs are redirected to syslog\n",
            name);
}

/****************************************************************************
 **                                                                        **
 ** Name:	 parse_opts()                                              **
 **                                                                        **
 ** Description: Parses the command line parameters                        **
 **                                                                        **
 ** Inputs:	 argc:		Number of parameters in the command line   **
 ** 	 	 argv:		Command line parameters                    **
 ** 	 	 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	None                                       **
 ** 	 	 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
static int parse_opts(int argc, char *argv[])
{
    static struct option long_opts[] = {
        {"version", 0, 0, 'V'},
        {"help", 0, 0, 'h'},
        {"help", 0, 0, '?'},
        {"ral-listening-port", optional_argument, 0, 'P'},
        {"ral-ip-address",     optional_argument, 0, 'I'},
        {"mihf-remote-port",   optional_argument, 0, 'p'},
        {"mihf-ip-address",    optional_argument, 0, 'i'},
        {"mihf-link-id",       optional_argument, 0, 'l'},
        {"mihf-id",            optional_argument, 0, 'm'},
        {"output-to-console",  0, 0, 'c'},
        {"output-to-file",     0, 0, 'f'},
        {"output-to-syslog",   0, 0, 's'},
        {0, 0, 0, 0}
    };

    /* parse all other cmd line parameters than -c */
    while (1) {
        int idx, c;
        c = getopt_long(argc, argv, "P:I:p:i:l:m:Vh?cfs", long_opts, &idx);
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
                strncpy(g_mihf_ip_address, optarg, strlen(g_mihf_ip_address));
                break;
            case 'p':
                strncpy(g_mihf_remote_port, optarg, strlen(g_mihf_remote_port));
                break;
            case 'P':
                strncpy(g_ral_listening_port_for_mihf, optarg,
			strlen(g_ral_listening_port_for_mihf));
                break;
            case 'I':
                strncpy(g_ral_ip_address, optarg, strlen(g_ral_ip_address));
                break;
            case 'l':
                strncpy(g_link_id, optarg, strlen(g_link_id));
                break;
            case 'm':
                strncpy(g_mihf_id, optarg, strlen(g_mihf_id));
                break;
            case 'c':
                g_log_output = LOG_TO_CONSOLE;
                break;
            case 'f':
                g_log_output = LOG_TO_FILE;
                break;
            case 's':
               g_log_output = LOG_TO_SYSTEM;
                break;
            default:
                break;
        };
    }
    return 0;
}

/****************************************************************************
 **                                                                        **
 ** Name:	 NAS_Netlink_socket_init()                                 **
 **                                                                        **
 ** Description: Initializes the communication channel with the NAS driver **
 **                                                                        **
 ** Inputs:	 None                                                      **
 ** 	 	 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	None                                       **
 ** 	 	 Others:	netl_s	: the NAS net link socket          **
 **                                                                        **
 ***************************************************************************/
static void NAS_Netlink_socket_init(void)
{
    int len;
    struct sockaddr_un local;
    if ((netl_s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("NAS_Netlink_socket_init : socket() failed");
        exit(1);
    }

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, SOCK_TQAL_NAS_PATH);
    unlink(local.sun_path);
    len = strlen(local.sun_path) + sizeof(local.sun_family);

    if (bind(netl_s, (struct sockaddr *)&local, len) == -1) {
        perror("NAS_Netlink_socket_init : bind() failed");
        exit(1);
    }

    if (listen(netl_s, 1) == -1) {
        perror("NAS_Netlink_socket_init : listen() failed");
        exit(1);
    }
}

/****************************************************************************
 **                                                                        **
 ** Name:	 get_IPv6_addr()                                           **
 **                                                                        **
 ** Description: Gets the IPv6 address of the specified network interface. **
 **                                                                        **
 ** Inputs:	 if_name	Interface name                             **
 ** 	 	 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	None                                       **
 ** 	 	 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
static void get_IPv6_addr(const char* if_name)
{
#define IPV6_ADDR_LINKLOCAL 0x0020U

    FILE *f;
    char devname[20];
    int plen, scope, dad_status, if_idx;
    char addr6p[8][5];
    int found = 0;
    char my_addr[16];
    char temp_addr[32];
    int i, j;

    DEBUG(" %s : network interface %s\n", __FUNCTION__, if_name);

    if ((f = fopen("/proc/net/if_inet6", "r")) != NULL) {
        while (fscanf(f, "%4s%4s%4s%4s%4s%4s%4s%4s %02x %02x %02x %02x %20s\n",
                 addr6p[0], addr6p[1], addr6p[2], addr6p[3],
                 addr6p[4], addr6p[5], addr6p[6], addr6p[7],
                 &if_idx, &plen, &scope, &dad_status, devname) != EOF) {

            if (!strcmp(devname, if_name)) {
                found = 1;
                // retrieve numerical value
                if ((scope == 0) || (scope == IPV6_ADDR_LINKLOCAL)) {
                    DEBUG(" adresse  %s:%s:%s:%s:%s:%s:%s:%s",
                            addr6p[0], addr6p[1], addr6p[2], addr6p[3],
                            addr6p[4], addr6p[5], addr6p[6], addr6p[7]);
                    DEBUG(" Scope:");
                    switch (scope) {
                        case 0:
                            DEBUG(" Global\n");
                            break;
                        case IPV6_ADDR_LINKLOCAL:
                            DEBUG(" Link\n");
                            break;
                        default:
                            DEBUG(" Unknown\n");
			    break;
                    }
                    DEBUG(" Numerical value: ");
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
                        DEBUG("-%hhx-",my_addr[i]);
                    }
                    DEBUG("\n");
                }
            }
        }
        fclose(f);
        if (!found) {
            ERR(" %s : interface %s not found\n\n", __FUNCTION__, if_name);
        }
    }
}

/****************************************************************************
 **                                                                        **
 ** Name:	 RAL_initialize()                                          **
 **                                                                        **
 ** Description: Performs overall RAL LTE initialisations:                 **
 **                  - Default value of global variables                   **
 **                  - Command line parsing                                **
 **                  - List of supported MIH actions                       **
 **                  - List of supported MIH link-events                   **
 **                  - Communication channel with the NAS driver           **
 **                  - MIH link registration                               **
 **                                                                        **
 ** Inputs:	 argc:		Number of parameters in the command line   **
 ** 	 	 argv:		Command line parameters                    **
 ** 	 	 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	None                                       **
 ** 	 	 Others:	g_mihf_ip_address, g_mihf_remote_port      **
 **				g_sockd_mihf, g_ral_ip_address,            **
 **				g_ral_listening_port_for_mihf              **
 **				g_link_id, g_mihf_id, g_log_output         **
 **				g_sockd_nas, ralpriv                       **
 **                                                                        **
 ***************************************************************************/
static int RAL_initialize(int argc, const char *argv[])
{
    MIH_C_TRANSACTION_ID_T           transaction_id;
    unsigned int                     t;
    struct sockaddr_un               nas_socket;

    ralpriv = &rl_priv;
    memset(ralpriv, 0, sizeof(struct ral_lte_priv));

    /*
     * Initialize defaults
     */
    g_ral_ip_address                = strdup(DEFAULT_IP_ADDRESS_RAL);
    g_ral_listening_port_for_mihf   = strdup(DEFAULT_LOCAL_PORT_RAL);
    g_mihf_remote_port              = strdup(DEFAULT_REMOTE_PORT_MIHF);
    g_mihf_ip_address               = strdup(DEFAULT_IP_ADDRESS_MIHF);
    g_sockd_mihf                    = -1;
    g_link_id                       = strdup(DEFAULT_LINK_ID);
    g_mihf_id                       = strdup(DEFAULT_MIHF_ID);
    g_log_output                    = LOG_TO_CONSOLE;

    /*
     * Parse command line parameters
     */
    if (parse_opts(argc, (char**) argv) < 0) {
        exit(0);
    }

    MIH_C_init(g_log_output);

    DEBUG(" %s -I %s -P %s -i %s -p %s -l %s -m %s\n", argv[0],
	  g_ral_ip_address, g_ral_listening_port_for_mihf,
	  g_mihf_ip_address, g_mihf_remote_port,
	  g_link_id, g_mihf_id);

    /*
     * Connect to the MIF Function
     */
    DEBUG(" Connect to the MIH-F ...\n");
    if (eRALlte_mihf_connect() < 0 ) {
        ERR(" %s : Could not connect to MIH-F...exiting\n", __FUNCTION__);
        exit(-1);
    }

    // excluded MIH_C_LINK_AC_TYPE_NONE
    // excluded MIH_C_LINK_AC_TYPE_LINK_DISCONNECT
    // excluded MIH_C_LINK_AC_TYPE_LINK_LOW_POWER
    // excluded MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN
    // excluded MIH_C_LINK_AC_TYPE_LINK_POWER_UP
    ralpriv->mih_supported_link_action_list = 
	(1 << MIH_C_LINK_AC_TYPE_LINK_FLOW_ATTR)		|
	(1 << MIH_C_LINK_AC_TYPE_LINK_ACTIVATE_RESOURCES)	|
	(1 << MIH_C_LINK_AC_TYPE_LINK_DEACTIVATE_RESOURCES);

    // excluded MIH_C_BIT_LINK_DETECTED
    // excluded MIH_C_BIT_LINK_PARAMETERS_REPORT
    // excluded MIH_C_BIT_LINK_GOING_DOWN
    // excluded MIH_C_BIT_LINK_HANDOVER_IMMINENT
    // excluded MIH_C_BIT_LINK_HANDOVER_COMPLETE
    // excluded MIH_C_BIT_LINK_PDU_TRANSMIT_STATUS
    ralpriv->mih_supported_link_event_list =
	MIH_C_BIT_LINK_UP			|
	MIH_C_BIT_LINK_DOWN;

    // excluded MIH_C_BIT_LINK_GET_PARAMETERS
    // excluded MIH_C_BIT_LINK_CONFIGURE_THRESHOLDS
    ralpriv->mih_supported_link_command_list =
	MIH_C_BIT_LINK_EVENT_SUBSCRIBE		|
	MIH_C_BIT_LINK_EVENT_UNSUBSCRIBE	|
	MIH_C_BIT_LINK_ACTION;

    NOTICE("[MSC_NEW][%s][MIH-F=%s]\n", getTimeStamp4Log(), g_mihf_id);
    NOTICE("[MSC_NEW][%s][RAL=%s]\n", getTimeStamp4Log(), g_link_id);
    NOTICE("[MSC_NEW][%s][NAS=%s]\n", getTimeStamp4Log(), "nas");

    /*
     * Initialize the NAS driver communication channel
     */
    NAS_Netlink_socket_init();
    DEBUG(" Waiting for a connection from the NAS Driver ...\n");
    t = sizeof(nas_socket);
    if ((g_sockd_nas = accept(netl_s, (struct sockaddr *)&nas_socket, &t)) == -1) {
        perror("RAL_initialize : g_sockd_nas - accept() failed");
        exit(1);
    }
    DEBUG(" NAS Driver Connected.\n\n");

    /*
     * Get the interface IPv6 address
     */
#ifdef RAL_DUMMY
    get_IPv6_addr("eth0");
#else
#ifdef RAL_REALTIME
    get_IPv6_addr("graal0");
#endif
#endif

    /*
     * Get the list of MTs
     */
    eRALlte_NAS_get_MTs_list();
    DEBUG(" List of MTs initialized\n\n");

    transaction_id = (MIH_C_TRANSACTION_ID_T)0;
    eRALlte_send_link_register_indication(&transaction_id);

    return 0;
}

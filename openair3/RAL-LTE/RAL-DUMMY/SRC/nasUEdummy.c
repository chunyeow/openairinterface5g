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
#include <stdio.h>

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <sys/time.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "nas_ue_netlink.h"

#define CONF_NASUE_DUMMY
#define CONF_UNKNOWN_CELL_ID 0

#include "nasUE_config.h"

int state, cell_id;

#define MEAS_MAX_RSSI 110


#ifdef MIH_USER_CONTROL
char        *g_mih_user_ip_address             = MIH_USER_IP_ADDRESS;
char        *g_mih_user_remote_port            = MIH_USER_REMOTE_PORT;
char        *g_nas_ip_address                  = NAS_IP_ADDRESS;
char        *g_nas_listening_port_for_mih_user = NAS_LISTENING_PORT_FOR_MIH_USER;
int          g_sockd_mih_user;
signed int   g_mih_user_rssi_increment         = 0;
//---------------------------------------------------------------------------
int NAS_mihuser_connect(void){
//---------------------------------------------------------------------------
    struct addrinfo      hints;
    struct addrinfo     *result, *rp;
    int                  s, on;
    struct sockaddr_in  *addr  = NULL;
    struct sockaddr_in6 *addr6 = NULL;
    unsigned char        buf[sizeof(struct sockaddr_in6)];


    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM;   /* Datagram socket */
    hints.ai_flags    = 0;
    hints.ai_protocol = 0;            /* Any protocol */

    s = getaddrinfo(g_mih_user_ip_address, g_mih_user_remote_port, &hints, &result);
    if (s != 0) {
        printf("ERR getaddrinfo: %s\n", gai_strerror(s));
        return -1;
    }

    /* getaddrinfo() returns a list of address structures.
        Try each address until we successfully connect(2).
        If socket(2) (or connect(2)) fails, we (close the socket
        and) try the next address. */

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        g_sockd_mih_user = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (g_sockd_mih_user == -1)
            continue;

        on = 1;
        setsockopt( g_sockd_mih_user, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

        if(rp->ai_family == AF_INET) {
            printf("Destination address  %s is an ipv4 address\n",g_mih_user_ip_address);
            addr             = (struct sockaddr_in *)(&buf[0]);
            addr->sin_port   = htons(atoi(g_nas_listening_port_for_mih_user));
            addr->sin_family = AF_INET;
            s = inet_pton(AF_INET, g_nas_ip_address, &addr->sin_addr);
            if (s <= 0) {
                if (s == 0) {
                    printf("ERR IP NAS address should be a IPv4 ADDR - But found not in presentation format : %s\n", g_nas_ip_address);
                } else {
                    printf("ERR %s - inet_pton(NAS IPv4 ADDR %s): %s\n", __FUNCTION__, g_nas_ip_address, strerror(s));
                }
                return -1;
            }

            s = bind(g_sockd_mih_user, (const struct sockaddr *)addr, sizeof(struct sockaddr_in));
            if (s == -1) {
                printf("ERR NAS IPv4 Address Bind: %s\n", strerror(errno));
                return -1;
            }
            // sockd_mihf is of type SOCK_DGRAM, rp->ai_addr is the address to which datagrams are sent by default
            if (connect(g_sockd_mih_user, rp->ai_addr, rp->ai_addrlen) != -1) {
                printf(" NAS is now UDP-CONNECTED to MIH-F\n");
                return 0;
            } else {
                close(g_sockd_mih_user);
            }
        } else if (rp->ai_family == AF_INET6) {
            printf("Destination address  %s is an ipv6 address\n",g_mih_user_ip_address);
            addr6              = (struct sockaddr_in6 *)(&buf[0]);
            addr6->sin6_port   = htons(atoi(g_nas_listening_port_for_mih_user));
            addr6->sin6_family = AF_INET6;
            s = inet_pton(AF_INET, g_nas_ip_address, &addr6->sin6_addr);
            if (s <= 0) {
                if (s == 0) {
                    printf("ERR IP NAS address should be a IPv6 ADDR, But found not in presentation format : %s\n", g_nas_ip_address);
                } else {
                    printf("ERR %s - inet_pton(NAS IPv6 ADDR %s): %s\n", __FUNCTION__, g_nas_ip_address, strerror(s));
                }
                return -1;
            }

            s = bind(g_sockd_mih_user, (const struct sockaddr *)addr6, sizeof(struct sockaddr_in));
            if (s == -1) {
                printf("ERR NAS IPv6 Address Bind: %s\n", strerror(errno));
                return -1;
            }
            if (connect(g_sockd_mih_user, rp->ai_addr, rp->ai_addrlen) != -1) {
                printf(" NAS is now able to receive UDP control messages from MIH-User\n");
                return 0;
            } else {
                close(g_sockd_mih_user);
            }
        } else {
            printf("ERR %s is an unknown address format %d\n",g_mih_user_ip_address,rp->ai_family);
        }
        close(g_sockd_mih_user);
    }

    if (rp == NULL) {   /* No address succeeded */
        printf("ERR Could not establish socket to MIH-User\n");
        return -1;
    }
    return -1;
}

int NAS_MIHUSERreceive(int sock)
{
  unsigned char str[NAS_UE_NETL_MAXLEN];
  int  t, done;
    t=recv(sock, str, NAS_UE_NETL_MAXLEN, 0);
    if (t <= 0) {
        if (t < 0) perror("NAS_MIHUSERreceive : recv");
        done = 1;
    }
    printf("\nmessage from MIH-USER, length:  %d\n", t);
    switch (str[0]) {
        case 0xff:
            printf("MIH-USER ASK FOR DECREASING RSSI\n");
            g_mih_user_rssi_increment = -RSSI_INCREMENT_STEP;
            break;
        case 0x00:
            printf("MIH-USER ASK FOR NOT MODIFYING RSSI\n");
            g_mih_user_rssi_increment = 0;
            break;
        case 0x01:
            printf("MIH-USER ASK FOR INCREASING RSSI\n");
            g_mih_user_rssi_increment = RSSI_INCREMENT_STEP;
            break;
        default:
            printf("received %hx\n", str[0]);
            return -1;
    }
    return 0;
}
#endif
int NAS_IAL_sock_connect(void)
{
    struct sockaddr_un remote;
    int len,s;

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("NAS_IALconnect - socket");
        exit(1);
    }

    printf("Trying to connect...\n");
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_NAS_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(s, (struct sockaddr *)&remote, len) == -1) {
        perror("NAS_IALconnect - connect");
        return -1;
    }

    printf("Connected.\n");
    return s;
}


int NAS_IALreceive(int s)
{
  char str1[NAS_UE_NETL_MAXLEN];
  char str2[NAS_UE_NETL_MAXLEN];
  int i, t, done;
  struct nas_ue_netl_request *msgToRcve;
  struct nas_ue_netl_reply *msgToSend;

    done =0;
    t=recv(s, str1, NAS_UE_NETL_MAXLEN, 0);
    if (t <= 0) {
        if (t < 0) perror("RAL_process_command : recv");
        done = 1;
    }
    printf("\nmessage from RAL, length:  %d\n", t);

    msgToRcve = (struct nas_ue_netl_request *) str1;
    msgToSend = (struct nas_ue_netl_reply *) str2;
    memset(str2,0,NAS_UE_NETL_MAXLEN);
    switch (msgToRcve->type){
       case NAS_UE_MSG_CNX_ESTABLISH_REQUEST:
           printf("NAS_UE_MSG_CNX_ESTABLISH_REQUEST received\n");
           msgToSend->type = NAS_UE_MSG_CNX_ESTABLISH_REPLY;
           msgToSend->length = sizeof(struct nas_ue_netl_hdr)+sizeof(struct nas_ue_msg_cnx_establish_reply);
           msgToSend->ialNASPrimitive.cnx_est_rep.status = NAS_CONNECTED;
           state = NAS_CONNECTED;
           cell_id = msgToRcve->ialNASPrimitive.cnx_req.cellid;
           break;
       case NAS_UE_MSG_CNX_RELEASE_REQUEST:
           printf("NAS_UE_MSG_CNX_RELEASE_REQUEST received\n");
           msgToSend->type = NAS_UE_MSG_CNX_RELEASE_REPLY;
           msgToSend->length = sizeof(struct nas_ue_netl_hdr)+sizeof(struct nas_ue_msg_cnx_release_reply);
           msgToSend->ialNASPrimitive.cnx_rel_rep.status = NAS_DISCONNECTED;
           state = NAS_DISCONNECTED;
           cell_id = CONF_UNKNOWN_CELL_ID;
           break;
       case NAS_UE_MSG_CNX_LIST_REQUEST:
           printf("NAS_UE_MSG_CNX_LIST_REQUEST received\n");
           msgToSend->type = NAS_UE_MSG_CNX_LIST_REPLY;
           msgToSend->length = sizeof(struct nas_ue_netl_hdr)+sizeof(struct nas_ue_msg_cnx_list_reply);
           msgToSend->ialNASPrimitive.cnx_list_rep.state = state;
           msgToSend->ialNASPrimitive.cnx_list_rep.cellid = cell_id;
           msgToSend->ialNASPrimitive.cnx_list_rep.iid4 = CONF_iid4;
           msgToSend->ialNASPrimitive.cnx_list_rep.iid6[0] = conf_iid6_0[CONF_MT0];
           msgToSend->ialNASPrimitive.cnx_list_rep.iid6[1] = conf_iid6_1[CONF_MT0];
           if (state == NAS_CONNECTED){
               msgToSend->ialNASPrimitive.cnx_list_rep.num_rb = CONF_num_rb;
               msgToSend->ialNASPrimitive.cnx_list_rep.nsclassifier = CONF_num_rb;
           }
           break;
       case NAS_UE_MSG_CNX_STATUS_REQUEST:
           printf("NAS_UE_MSG_CNX_STATUS_REQUEST received\n");
           msgToSend->type = NAS_UE_MSG_CNX_STATUS_REPLY;
           msgToSend->length = sizeof(struct nas_ue_netl_hdr)+sizeof(struct nas_ue_msg_cnx_status_reply);
           msgToSend->ialNASPrimitive.cnx_stat_rep.status = state;
           msgToSend->ialNASPrimitive.cnx_stat_rep.cellid = cell_id;
           if (state == NAS_CONNECTED){
               msgToSend->ialNASPrimitive.cnx_stat_rep.num_rb = CONF_num_rb;
               msgToSend->ialNASPrimitive.cnx_stat_rep.signal_level = conf_level[0];
           }
           break;
       case NAS_UE_MSG_RB_LIST_REQUEST:
           printf("NAS_UE_MSG_RB_LIST_REQUEST received\n");
           msgToSend->type = NAS_UE_MSG_RB_LIST_REPLY;
           msgToSend->length = sizeof(struct nas_ue_netl_hdr)+sizeof(struct nas_ue_msg_rb_list_reply);
           if (state == NAS_CONNECTED){
              msgToSend->ialNASPrimitive.rb_list_rep.num_rb = CONF_num_rb;
              for (i=0; i<CONF_num_rb; i++){
                 msgToSend->ialNASPrimitive.rb_list_rep.RBList[i].rbId = conf_rbId[i];
                 msgToSend->ialNASPrimitive.rb_list_rep.RBList[i].QoSclass = conf_qoSclass[i];
                 msgToSend->ialNASPrimitive.rb_list_rep.RBList[i].dscp = conf_dscp[i];
              }
           }
           break;
       case NAS_UE_MSG_STATISTIC_REQUEST:
           printf("NAS_UE_MSG_STATISTIC_REQUEST received\n");
           msgToSend->type = NAS_UE_MSG_STATISTIC_REPLY;
           msgToSend->length = sizeof(struct nas_ue_netl_hdr)+sizeof(struct nas_ue_msg_statistic_reply);
           if (state == NAS_CONNECTED){
               msgToSend->ialNASPrimitive.statistics_rep.rx_packets = CONF_rx_packets;
               msgToSend->ialNASPrimitive.statistics_rep.tx_packets = CONF_tx_packets;
               msgToSend->ialNASPrimitive.statistics_rep.rx_bytes   = CONF_rx_bytes;
               msgToSend->ialNASPrimitive.statistics_rep.tx_bytes   = CONF_tx_bytes;
               msgToSend->ialNASPrimitive.statistics_rep.rx_errors  = CONF_rx_errors;
               msgToSend->ialNASPrimitive.statistics_rep.tx_errors  = CONF_tx_errors;
               msgToSend->ialNASPrimitive.statistics_rep.rx_dropped = CONF_rx_dropped;
               msgToSend->ialNASPrimitive.statistics_rep.tx_dropped = CONF_tx_dropped;
           }
           break;
       case NAS_UE_MSG_MEAS_REQUEST:
           printf("NAS_UE_MSG_MEAS_REQUEST received\n");
           msgToSend->type = NAS_UE_MSG_MEAS_REPLY;
           msgToSend->length = sizeof(struct nas_ue_netl_hdr)+sizeof(struct nas_ue_msg_measure_reply);
           msgToSend->ialNASPrimitive.meas_rep.num_cells = CONF_num_cells;
           printf("\tSignal levels sent ");
           for (i=0; i<CONF_num_rb; i++){
              #ifdef MIH_USER_CONTROL
              // LG TEST WITH ONLY i =0
              if (i == 0) {
                  conf_level[i] += g_mih_user_rssi_increment;
                  if (conf_level[i] > MEAS_MAX_RSSI) conf_level[i] = MEAS_MAX_RSSI;
                  if (conf_level[i] < 0)             conf_level[i] = 0;
              }
              #endif
              msgToSend->ialNASPrimitive.meas_rep.measures[i].cell_id = conf_cell_id[i];
              msgToSend->ialNASPrimitive.meas_rep.measures[i].level = conf_level[i];
              msgToSend->ialNASPrimitive.meas_rep.measures[i].provider_id = conf_provider_id[i];
              printf ("| cell %d : %d ", conf_cell_id[i], conf_level[i]);
           }
           printf("\n");
           break;
       case NAS_UE_MSG_IMEI_REQUEST:
           printf("NAS_UE_MSG_IMEI_REQUEST received\n");
           msgToSend->type = NAS_UE_MSG_IMEI_REPLY;
           msgToSend->length = sizeof(struct nas_ue_netl_hdr)+sizeof(struct nas_ue_l2id_reply);
           msgToSend->ialNASPrimitive.l2id_rep.l2id[0] = conf_iid6_0[CONF_MT0];
           msgToSend->ialNASPrimitive.l2id_rep.l2id[1] = conf_iid6_1[CONF_MT0];
           state = NAS_DISCONNECTED;
           cell_id = CONF_UNKNOWN_CELL_ID;
           break;
       default:
         printf ("Invalid message Type %d\n",msgToRcve->type);
    }

    if (send(s, str2, msgToSend->length, 0) < 0) {
        perror("IAL_process_command : send");
        done = 1;
    }

    printf ("Response message sent to RAL %d\n",msgToSend->length);

    return done;

}

int main(void)
{
    int s = 0;
    int rc, done;
    fd_set readfds;
    struct timeval tv;

    do {
        s= NAS_IAL_sock_connect();
     if (s <= 0) {
          sleep(2);
        }
    } while (s < 0);

#ifdef MIH_USER_CONTROL
    NAS_mihuser_connect();
#endif

    done = 0;
    do {
        // Create fd_set and wait for input
        FD_ZERO(&readfds);
        FD_SET (s, &readfds);
#ifdef MIH_USER_CONTROL
        FD_SET (g_sockd_mih_user, &readfds);
#endif
        tv.tv_sec = 0;
        tv.tv_usec = 100000; // timeout select for 100ms and read FIFOs

        rc = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);
        if (rc ==-1){
            perror("select");
            done = 1;
        }
        // something received !
        if (rc>=0){
            if (FD_ISSET(s,&readfds)){
                done = NAS_IALreceive(s);
            }
#ifdef MIH_USER_CONTROL
            if (FD_ISSET(g_sockd_mih_user,&readfds)){
                done = NAS_MIHUSERreceive(g_sockd_mih_user);
            }
#endif
        }

    } while (!done);


    close(s);

    return 0;
}



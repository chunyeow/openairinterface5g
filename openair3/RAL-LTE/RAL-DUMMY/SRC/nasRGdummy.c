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

#include "nas_rg_netlink.h"

//int state, cell_id;
//int rb_id;

int NAS_TQAL_sock_connect(void)
{
    struct sockaddr_un remote;
    int len,s;

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("NAS_TQAL_sock_connect - socket");
        exit(1);
    }

    printf("Trying to connect...\n");
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_TQAL_NAS_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(s, (struct sockaddr *)&remote, len) == -1) {
        perror("NAS_TQAL_sock_connect - connect() failed");
        exit(1);
    }

    printf("Connected to TQAL.\n");
    return s;
}

int NAS_TQALreceive(int s)
{
  char str1[NAS_RG_NETL_MAXLEN];
  char str2[NAS_RG_NETL_MAXLEN];
  int t, done;
  int ix;
  struct nas_rg_netl_request *msgToRcve;
  struct nas_rg_netl_reply *msgToSend;

    done = 0;
    t = recv(s, str1, NAS_RG_NETL_MAXLEN, 0);
    if (t <= 0) {
        if (t < 0) perror("NAS_TQALreceive : recv() failed");
        done = 1;
    }
    printf("message from TQAL, length:  %d\n", t);

    msgToRcve = (struct nas_rg_netl_request *) str1;
    msgToSend = (struct nas_rg_netl_reply *) str2;
    memset(str2, 0, NAS_RG_NETL_MAXLEN);

    switch (msgToRcve->type)
    {
       case NAS_RG_MSG_RB_ESTABLISH_REQUEST:
           printf("NAS_RG_MSG_RB_ESTABLISH_REQUEST received\n");
	   msgToSend->type = NAS_RG_MSG_RB_ESTABLISH_REPLY;
	   msgToSend->length = sizeof(struct nas_rg_netl_hdr)+sizeof(struct nas_rg_msg_rb_establish_reply);
           msgToSend->tqalNASPrimitive.rb_est_rep.cnxid = msgToRcve->tqalNASPrimitive.rb_est_req.cnxid;
           ix = (msgToRcve->tqalNASPrimitive.rb_est_req.cnxid-1) % 32;
           msgToSend->tqalNASPrimitive.rb_est_rep.ue_id = (msgToRcve->tqalNASPrimitive.rb_est_req.cnxid - ix - 1)/32;
           msgToSend->tqalNASPrimitive.rb_est_rep.RBParms.rbId = msgToRcve->tqalNASPrimitive.rb_est_req.RBParms.rbId;
           msgToSend->tqalNASPrimitive.rb_est_rep.RBParms.QoSclass = msgToRcve->tqalNASPrimitive.rb_est_req.RBParms.QoSclass;
           msgToSend->tqalNASPrimitive.rb_est_rep.RBParms.dscp = msgToRcve->tqalNASPrimitive.rb_est_req.RBParms.dscp;
           msgToSend->tqalNASPrimitive.rb_est_rep.result = NAS_CONNECTED;
           printf("Establishing Radio Bearer - cnx_id %d, Rb_id %d , QoSclass %d, dscp %d\n\n",
              msgToSend->tqalNASPrimitive.rb_est_rep.cnxid ,
              msgToSend->tqalNASPrimitive.rb_est_rep.RBParms.rbId ,
              msgToSend->tqalNASPrimitive.rb_est_rep.RBParms.QoSclass,
              msgToSend->tqalNASPrimitive.rb_est_rep.RBParms.dscp );
           break;

       case NAS_RG_MSG_RB_RELEASE_REQUEST:
           printf("NAS_RG_MSG_RB_RELEASE_REQUEST received\n");
	   msgToSend->type = NAS_RG_MSG_RB_RELEASE_REPLY;
	   msgToSend->length = sizeof(struct nas_rg_netl_hdr)+sizeof(struct nas_rg_msg_rb_release_reply);
           msgToSend->tqalNASPrimitive.rb_rel_rep.cnxid = msgToRcve->tqalNASPrimitive.rb_rel_req.cnxid;
           msgToSend->tqalNASPrimitive.rb_rel_rep.ue_id = msgToRcve->tqalNASPrimitive.rb_rel_req.ue_id;
           msgToSend->tqalNASPrimitive.rb_rel_rep.result = NAS_DISCONNECTED;
           printf("Releasing Radio Bearer - cnx_id %d, ue_id %d\n\n",
              msgToSend->tqalNASPrimitive.rb_rel_rep.cnxid,
              msgToSend->tqalNASPrimitive.rb_rel_rep.ue_id);
           break;

       case NAS_RG_MSG_MT_MCAST_JOIN:
           printf("NAS_RG_MSG_MT_MCAST_JOIN received\n");
	   msgToSend->type = NAS_RG_MSG_MT_MCAST_JOIN_REP;
	   msgToSend->length = sizeof(struct nas_rg_netl_hdr)+sizeof(struct nas_rg_msg_mt_mcast_join_rep);
           msgToSend->tqalNASPrimitive.mt_mc_join_rep.cnxid = msgToRcve->tqalNASPrimitive.mt_mcast_join.cnxid;
           msgToSend->tqalNASPrimitive.mt_mc_join_rep.ue_id = msgToRcve->tqalNASPrimitive.mt_mcast_join.ue_id;
           msgToSend->tqalNASPrimitive.mt_mc_join_rep.result = NAS_CONNECTED;
           printf("MT Joining Multicast - cnx_id %d, ue_id %d\n\n",
              msgToSend->tqalNASPrimitive.mt_mc_join_rep.cnxid,
              msgToSend->tqalNASPrimitive.mt_mc_join_rep.ue_id);
           break;

       case NAS_RG_MSG_MT_MCAST_LEAVE:
           printf("NAS_RG_MSG_MT_MCAST_LEAVE received\n");
	   msgToSend->type = NAS_RG_MSG_MT_MCAST_LEAVE_REP;
	   msgToSend->length = sizeof(struct nas_rg_netl_hdr)+sizeof(struct nas_rg_msg_mt_mcast_leave_rep);
           msgToSend->tqalNASPrimitive.mt_mc_leavce_rep.cnxid = msgToRcve->tqalNASPrimitive.mt_mcast_leave.cnxid;
           msgToSend->tqalNASPrimitive.mt_mc_leavce_rep.ue_id = msgToRcve->tqalNASPrimitive.mt_mcast_leave.ue_id;
           msgToSend->tqalNASPrimitive.mt_mc_leavce_rep.result = NAS_DISCONNECTED;
           printf("MT Leaving Multicast - cnx_id %d, ue_id %d\n\n",
              msgToSend->tqalNASPrimitive.mt_mc_leavce_rep.cnxid,
              msgToSend->tqalNASPrimitive.mt_mc_leavce_rep.ue_id);
           break;

       case NAS_RG_MSG_CNX_STATUS_REQUEST:
           printf("NAS_RG_MSG_CNX_STATUS_REQUEST received\n");
           break;
       case NAS_RG_MSG_RB_LIST_REQUEST:
           printf("NAS_RG_MSG_RB_LIST_REQUEST received\n");
           break;
       case NAS_RG_MSG_STATISTIC_REQUEST:
           printf("NAS_RG_MSG_STATISTIC_REQUEST received\n");
           break;
       case NAS_RG_MSG_MEASUREMENT_REQUEST:
           printf("NAS_RG_MSG_MEASUREMENT_REQUEST received\n");
	   break;
       default:
	   printf ("Invalid message Type %d\n",msgToRcve->type);
	   break;
    }

    if (send(s, str2, msgToSend->length, 0) < 0) {
        perror("NAS_TQALreceive : send() failed");
        done = 1;
    }

    printf ("message sent to TQAL %d\n",msgToSend->length);

    return done;

}

int main(void)
{
    int s_ral, rc, done;
    fd_set readfds;
    struct timeval tv;

    /* Connect to the RAL-LTE */
    do {
	s_ral = NAS_TQAL_sock_connect();
	if (s_ral <= 0) {
	    sleep(2);
	}
    } while (s_ral < 0);

    done = 0;
    do {
        // Create fd_set and wait for input
        FD_ZERO(&readfds);
        FD_SET (s_ral, &readfds);
        tv.tv_sec = 0;
        tv.tv_usec = 100000; // timeout select for 100ms and read FIFOs

        rc = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);
        if (rc ==-1) {
            perror("main : select() failed");
            done = 1;
        }
        // something received !
        if (rc >= 0) {
            if (FD_ISSET(s_ral, &readfds)){
                done = NAS_TQALreceive(s_ral);
            }
        }

    } while (!done);

    close(s_ral);

    return 0;
}

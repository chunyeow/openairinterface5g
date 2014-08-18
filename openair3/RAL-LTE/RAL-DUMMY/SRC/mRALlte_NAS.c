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
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
//#include <unistd.h>
//#include <sys/types.h>
#include <sys/socket.h>
//#include <sys/un.h>
//#include <sys/time.h>

#include "mRALlte_mih_msg.h"
#include "mRALlte_variables.h"
#include "mRALlte_proto.h"
#include "MIH_C.h"
#include "mRALlte_mih_msg.h"

#include "nasUE_config.h"
#include "nas_ue_netlink.h"

extern char message[NAS_UE_NETL_MAXLEN];
extern int sd_graal, s_nas;

#define MSC_GEN_BUF_SIZE 1024
static char msc_gen_buff[MSC_GEN_BUF_SIZE];

/***************************************************************************
     Reception side
 ***************************************************************************/
//---------------------------------------------------------------------------
void print_state(uint8_t state){
//---------------------------------------------------------------------------
	switch(state){
        case NAS_DISCONNECTED:DEBUG("NAS_DISCONNECTED\n");return;
        case NAS_CONNECTED:DEBUG("NAS_CONNECTED\n");return;
        default: ERR(" Unknown state\n");
	}
}

//---------------------------------------------------------------------------
int IAL_decode_NAS_message(void){
//---------------------------------------------------------------------------
    struct nas_ue_netl_reply *msgToRcve;
    int done=0, n, i;
    char *buffer;
    #ifdef MSCGEN_PYTOOL
    unsigned int              buffer_index;
    #endif
    n = recv(s_nas, message, NAS_UE_NETL_MAXLEN, 0);
    if (n <= 0) {
        if (n < 0) perror("RAL_process_DNAS_message : recv");
        done = 1;
    }

    if (!done) {
        DEBUG(" ");
        DEBUG("RAL_decode_NAS_message: primitive from NAS\n");
        msgToRcve = (struct nas_ue_netl_reply *) message;

        switch (msgToRcve->type) {

            case NAS_UE_MSG_CNX_ESTABLISH_REPLY:
                NOTICE("[MSC_MSG][%s][nas][--- NAS_UE_MSG_CNX_ESTABLISH_REPLY --->][%s]\n", getTimeStamp4Log(), g_link_id);
                DEBUG("NAS_UE_MSG_CNX_ESTABLISH_REPLY received\n");
                ralpriv->pending_req_action = MIH_C_LINK_AC_TYPE_NONE;
                ralpriv->state = DISCONNECTED;
                if (msgToRcve->ialNASPrimitive.cnx_est_rep.status==0) {
                    ERR(" Connexion establishment failure: %d\n",msgToRcve->ialNASPrimitive.cnx_est_rep.status);
                    done = 1;
                    ralpriv->pending_req_status    = MIH_C_STATUS_SUCCESS;
                    ralpriv->pending_req_ac_result = MIH_C_LINK_AC_RESULT_FAILURE;
                    //mRALu_send_link_switch_cnf();
                    //mRALte_send_link_action_confirm();
                 } else {
                    ralpriv->pending_req_status    = MIH_C_STATUS_SUCCESS;
                    ralpriv->pending_req_ac_result = MIH_C_LINK_AC_RESULT_SUCCESS;
                    ralpriv->pending_req_flag = 1;
                    DEBUG(" Connexion establishment pending: pending_req_flag %d\n\n",ralpriv->pending_req_flag);
                }
                break;

            case NAS_UE_MSG_CNX_RELEASE_REPLY:
                NOTICE("[MSC_MSG][%s][nas][--- NAS_UE_MSG_CNX_RELEASE_REPLY --->][%s]\n",
                       getTimeStamp4Log(),
                       g_link_id);
                DEBUG("NAS_UE_MSG_CNX_RELEASE_REPLY received\n");
                ralpriv->pending_req_action = MIH_C_LINK_AC_TYPE_NONE;
                if (msgToRcve->ialNASPrimitive.cnx_rel_rep.status>0){
                    ERR(" Connexion release failure: %d", msgToRcve->ialNASPrimitive.cnx_rel_rep.status);
                    done = 1;
                    ralpriv->pending_req_status    = MIH_C_STATUS_SUCCESS;
                    ralpriv->pending_req_ac_result = MIH_C_LINK_AC_RESULT_FAILURE;
                } else {
                    ralpriv->pending_req_status    = MIH_C_STATUS_SUCCESS;
                    ralpriv->pending_req_ac_result = MIH_C_LINK_AC_RESULT_SUCCESS;
                    ralpriv->pending_req_flag = 1;
                    DEBUG(" Connexion establishment pending: pending_req_flag %d\n\n",ralpriv->pending_req_flag);
                }
                ralpriv->state = DISCONNECTED;
                //mRALu_send_link_switch_cnf();
                //added
                ralpriv->pending_req_flag = 0;
                ralpriv->cell_id = CONF_UNKNOWN_CELL_ID;
                break;

            case NAS_UE_MSG_CNX_LIST_REPLY:
                {
                    struct nas_ue_msg_cnx_list_reply *p;
                    NOTICE("[MSC_MSG][%s][nas][--- NAS_UE_MSG_CNX_LIST_REPLY --->][%s]\n",
                            getTimeStamp4Log(),
                            g_link_id);
                    DEBUG("NAS_UE_MSG_CNX_LIST_REPLY received\n");
                    p = &(msgToRcve->ialNASPrimitive.cnx_list_rep);
                    DEBUG(" CellId\tIID4\tIID6\t\t\tnum_rb\tnsclass\tState\n");

                    DEBUG(" %u\t%u\t", p->cellid, p->iid4);
                    for (i=0;i<8;++i)
                    DEBUG("%02x", *((uint8_t *)p->iid6+i));
                    DEBUG("\t%u\t%u\t", p->num_rb, p->nsclassifier);
                    print_state(p->state);

                    ralpriv->nas_state = p->state ;
                    ralpriv->num_rb = p->num_rb;
                    ralpriv->cell_id = p->cellid ;
                    //IAL_return_NAS_StatAttach();
                }
                break;

           case NAS_UE_MSG_RB_LIST_REPLY:
             {
             struct nas_ue_msg_rb_list_reply *p;
             NOTICE("[MSC_MSG][%s][nas][--- NAS_UE_MSG_RB_LIST_REPLY --->][%s]\n",
                       getTimeStamp4Log(),
                       g_link_id);
             DEBUG("NAS_UE_MSG_RB_LIST_REPLY received\n");
             p = &(msgToRcve->ialNASPrimitive.rb_list_rep);
             ralpriv->num_rb = p->num_rb;
             DEBUG("rab_id\t\tdscp\t\tQoS\t\tState\n");
             for(i=0; i<p->num_rb; i++){
               DEBUG("%u\t\t%u\t\t%u\t\t", p->RBList[i].rbId, p->RBList[i].dscp, p->RBList[i].QoSclass);
               print_state(p->RBList[i].state);
               ralpriv->rbId[i]= p->RBList[i].rbId;
               ralpriv->QoSclass[i] = p->RBList[i].QoSclass;
               ralpriv->dscp[i] = p->RBList[i].dscp;
             }
             }
             //IAL_return_NAS_StatRB();
             break;

           case NAS_UE_MSG_MEAS_REPLY:
             {
             struct nas_ue_msg_measure_reply *p;
             DEBUG("NAS_UE_MSG_MEASUREMENT_REPLY received\n");
             p = &(msgToRcve->ialNASPrimitive.meas_rep);
             #ifdef DEBUG_MRALU_MEASURES
             DEBUG("Measurement Report - Number of cells %d - Current cell %d\n", p->num_cells, ralpriv->cell_id);
             DEBUG("Cell_id\tMeasure\tProvider_id\n");
             #endif
             for (i=0;i<p->num_cells; i++){
                 #ifdef DEBUG_MRALU_MEASURES
                 DEBUG(" %d\t%d\t%d\n",p-> measures[i].cell_id,p-> measures[i].level,p-> measures[i].provider_id);
                 #endif
                 ralpriv->meas_cell_id[i] = p-> measures[i].cell_id;
                 IAL_integrate_measure(p-> measures[i].level, i);
                 ralpriv->provider_id[i] = p-> measures[i].provider_id;
             }
             #ifdef MSCGEN_PYTOOL
             memset(msc_gen_buff, 0, MSC_GEN_BUF_SIZE);
             buffer_index = 0;

             for (i=0;i<p->num_cells; i++){
                 buffer_index += sprintf(&msc_gen_buff[buffer_index], "\\nCell Id %d Level %d Provider %d",
                                        p-> measures[i].cell_id, p-> measures[i].level, p-> measures[i].provider_id);
                 ralpriv->meas_cell_id[i] = p-> measures[i].cell_id;
             }
             NOTICE("[MSC_MSG][%s][nas][--- NAS_UE_MSG_MEASUREMENT_REPLY%s --->][%s]\n",
                getTimeStamp4Log(),
                msc_gen_buff,
                g_link_id);
             #endif
             //Temp
             #ifdef RAL_DUMMY
             ralpriv->num_measures = p->num_cells;  //TEMP- To be activated later
             #endif
             //ralpriv->meas_cell_id[0] = ralpriv->cell_id;
             }
             break;

           case NAS_UE_MSG_IMEI_REPLY:
             DEBUG("NAS_UE_MSG_IMEI_REPLY received\n");
             DEBUG("IMEI value received= %d %d\n", msgToRcve->ialNASPrimitive.l2id_rep.l2id[0], msgToRcve->ialNASPrimitive.l2id_rep.l2id[1]);
             //store the received value, to be used later
             ralpriv->ipv6_l2id[0] = msgToRcve->ialNASPrimitive.l2id_rep.l2id[0];
             ralpriv->ipv6_l2id[1] = msgToRcve->ialNASPrimitive.l2id_rep.l2id[1];
             buffer = (char *)(&ralpriv->ipv6_l2id[0]);
             DEBUG ("IMEI value: = ");
             for (i = 0; i < 8; i++) {
                 DEBUG ("-%hhx-", (unsigned char) buffer[i]);
             }
             DEBUG ("\n");
             #ifdef MSCGEN_PYTOOL
             memset(msc_gen_buff, 0, MSC_GEN_BUF_SIZE);
             buffer_index = 0;
             buffer_index = sprintf(&msc_gen_buff[buffer_index], "\\nIMEI:%hhx-%hhx-%hhx-%hhx-%hhx-%hhx-%hhx-%hhx",
                                    (unsigned char) buffer[0],(unsigned char) buffer[1],
                                    (unsigned char) buffer[2],(unsigned char) buffer[3],
                                    (unsigned char) buffer[4],(unsigned char) buffer[5],
                                    (unsigned char) buffer[6],(unsigned char) buffer[7]);
             NOTICE("[MSC_MSG][%s][nas][--- NAS_UE_MSG_IMEI_REPLY --->][%s]\n",
                       getTimeStamp4Log(),
                       g_link_id);
             #endif
             break;

           default:
             ERR("IAL_decode_NAS_message : invalid message Type %d\n",msgToRcve->type);
             done = 1;
        }
    }
    return done;
}


/***************************************************************************
     Transmission side
 ***************************************************************************/

//---------------------------------------------------------------------------
int IAL_process_DNAS_message(int ioctl_obj, int ioctl_cmd, int ioctl_cellid){
//---------------------------------------------------------------------------
  struct nas_ue_netl_request *msgToSend;
  int rc;


  msgToSend = (struct nas_ue_netl_request *) message;
  memset(message,0,NAS_UE_NETL_MAXLEN);

  NOTICE(" \n");

  switch (ioctl_obj){
      case IO_OBJ_CNX:
         switch (ioctl_cmd){
      /***/
            case IO_CMD_ADD:
              {
                NOTICE("[MSC_MSG][%s][%s][--- NAS_UE_MSG_CNX_ESTABLISH_REQUEST --->][nas]\n",
                       getTimeStamp4Log(),
                       g_link_id);
                msgToSend->type=NAS_UE_MSG_CNX_ESTABLISH_REQUEST;
                msgToSend->length = sizeof(struct nas_ue_netl_hdr)+ sizeof(struct nas_ue_netl_request);
                msgToSend->ialNASPrimitive.cnx_req.cellid = ioctl_cellid;
                ralpriv->cell_id = ioctl_cellid;
                //
          			DEBUG("ioctl : Connection establishment requested\n");
                rc = 0;
              }
      		  break;
      /***/
            case IO_CMD_DEL:
              {

                NOTICE("[MSC_MSG][%s][%s][--- NAS_UE_MSG_CNX_RELEASE_REQUEST --->][nas]\n",
                       getTimeStamp4Log(),
                       g_link_id);
          			msgToSend->type=NAS_UE_MSG_CNX_RELEASE_REQUEST;
                msgToSend->length = sizeof(struct nas_ue_netl_hdr)+ sizeof(struct nas_ue_netl_request);
                //
          			DEBUG("ioctl : Connexion release requested\n");
                rc = 0;
              }
      		  break;
      /***/
            case IO_CMD_LIST:
              {
                NOTICE("[MSC_MSG][%s][%s][--- NAS_UE_MSG_CNX_LIST_REQUEST --->][nas]\n",
                       getTimeStamp4Log(),
                       g_link_id);
           			msgToSend->type=NAS_UE_MSG_CNX_LIST_REQUEST;
                msgToSend->length = sizeof(struct nas_ue_netl_hdr)+ sizeof(struct nas_ue_netl_request);
                //
          			DEBUG("ioctl : Connexion list requested\n");
                rc = 0;
              }
      		  break;
      /***/
          default:
          ERR("IAL_process_DNAS_message : invalid ioctl command %d\n",ioctl_cmd);
		      rc= -1;
      } //end switch ioctl_cmd
      break;

/***************************/
      case IO_OBJ_RB:
         switch (ioctl_cmd){
      /***/
            case IO_CMD_LIST:
              {
                NOTICE("[MSC_MSG][%s][%s][--- NAS_UE_MSG_RB_LIST_REQUEST --->][nas]\n",
                       getTimeStamp4Log(),
                       g_link_id);
                msgToSend->type=NAS_UE_MSG_RB_LIST_REQUEST;
                msgToSend->length = sizeof(struct nas_ue_netl_hdr)+ sizeof(struct nas_ue_netl_request);
                //
          	DEBUG("ioctl : Radio bearer list requested\n");
                rc = 0;
               }
      		  break;
      /***/
            default:
            ERR("IAL_process_DNAS_message : invalid ioctl command %d\n",ioctl_cmd);
  		      rc= -1;
         } //end switch ioctl_cmd
      break;
/***************************/
      case IO_OBJ_MEAS:
        //
        {
            msgToSend->type=NAS_UE_MSG_MEAS_REQUEST;
            NOTICE("[MSC_MSG][%s][%s][--- NAS_UE_MSG_MEAS_REQUEST --->][nas]\n",
                       getTimeStamp4Log(),
                       g_link_id);
            msgToSend->length = sizeof(struct nas_ue_netl_hdr)+ sizeof(struct nas_ue_netl_request);
           //
            DEBUG("ioctl : Measurement requested\n");
            rc = 0;
        }
      break;
/***************************/
      case IO_OBJ_IMEI:
        {
            NOTICE("[MSC_MSG][%s][%s][--- NAS_UE_MSG_IMEI_REQUEST --->][nas]\n",
                       getTimeStamp4Log(),
                       g_link_id);
      		msgToSend->type=NAS_UE_MSG_IMEI_REQUEST;
          msgToSend->length = sizeof(struct nas_ue_netl_hdr)+ sizeof(struct nas_ue_netl_request);
           //
      		DEBUG("ioctl : L2Id (IMEI) requested\n");
      		rc = 0;
        }
      break;
/***************************/
      default:
        ERR("IAL_process_DNAS_message : invalid ioctl object %d\n",ioctl_obj);
		    rc= -1;
  } //end switch ioctl_obj

  if (!rc){
    if (send(s_nas, message,msgToSend->length, 0) < 0) {
        perror("IAL_process_DNAS_message : send");
        rc= -1;
    }
    DEBUG ("message sent to NAS %d\n",msgToSend->length);
  }
	return rc;
}




/***************************************************************************
    Tool functions
 ***************************************************************************/



//---------------------------------------------------------------------------
// Convert the IMEI to iid
void TQAL_NAS_imei2iid(uint8_t *imei, uint8_t *iid){
//---------------------------------------------------------------------------
// Start debug information

  if (!imei ||! iid ){
 	  ERR("TQAL_NAS_imei2iid - input parameter is NULL \n");
    return;
  }
// End debug information
	memset(iid, 0, 8);
	iid[0] = 16*imei[0]+imei[1];
	iid[1] = 16*imei[2]+imei[3];
	iid[2] = 16*imei[4]+imei[5];
	iid[3] = 16*imei[6]+imei[7];
	iid[4] = 16*imei[8]+imei[9];
	iid[5] = 16*imei[10]+imei[11];
	iid[6] = 16*imei[12]+imei[13];
	iid[7] = 16*imei[14]+imei[15];
}


//---------------------------------------------------------------------------
// Convert the IMEI to iid
void TQAL_NAS_imei2iid2(uint8_t *imei, uint32_t *iid){
//---------------------------------------------------------------------------
// Start debug information

  if (!imei ||! iid ){
 	  ERR("TQAL_NAS_imei2iid - input parameter is NULL \n");
    return;
  }
// End debug information
//	memset(iid, 0, 8);
	iid[0] = imei[0]+256 *(imei[1]+256*(imei[2]+256*(imei[3])));
	iid[1] = imei[4]+256 *(imei[5]+256*(imei[6]+256*(imei[7])));
}






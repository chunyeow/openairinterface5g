/***************************************************************************
                         lteRALue_ioctl.c  -  description
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
/*! \file lteRALue_ioctl.c
 * \brief Handling of ioctl for LTE driver in LTE-RAL-UE
 * \author WETTERWALD Michelle, GAUTHIER Lionel, MAUREL Frederic
 * \date 2013
 * \company EURECOM
 * \email: michelle.wetterwald@eurecom.fr, lionel.gauthier@eurecom.fr, frederic.maurel@eurecom.fr
 */
/*******************************************************************************/
#define MRAL_MODULE
#include "rrc_d_types.h"
#include "nas_ue_ioctl.h"
#include "rrc_nas_primitives.h"
#include "nasmt_constant.h"
#include "nasmt_iocontrol.h"

#include "lteRALue_mih_msg.h"
#include "lteRALue_variables.h"
#include "lteRALue_proto.h"
#include "MIH_C.h"
#include "lteRALue_mih_msg.h"

extern struct nas_ioctl gifr;
extern int fd;
// 0335060080149150
uint8_t NAS_DEFAULT_IMEI[16]={0x00, 0x03, 0x03, 0x05, 0x00 ,0x06, 0x00, 0x00, 0x08, 0x00, 0x01 ,0x04, 0x09, 0x01, 0x05, 0x00};

//---------------------------------------------------------------------------
void print_state(uint8_t state){
//---------------------------------------------------------------------------
    switch(state){
      case NAS_IDLE:DEBUG("NAS_IDLE\n");return;
//    case NAS_STATE_CONNECTED:DEBUG("NAS_STATE_CONNECTED\n");return;
//    case NAS_STATE_ESTABLISHMENT_REQUEST:DEBUG("NAS_STATE_ESTABLISHMENT_REQUEST\n");return;
//    case NAS_STATE_ESTABLISHMENT_FAILURE:DEBUG("NAS_STATE_ESTABLISHMENT_FAILURE\n");return;
//    case NAS_STATE_RELEASE_FAILURE:DEBUG("NAS_STATE_RELEASE_FAILURE\n");return;
      case  NAS_CX_FACH:DEBUG("NAS_CX_FACH\n");return;
      case  NAS_CX_DCH:DEBUG("NAS_CX_DCH\n");return;
      case  NAS_CX_RECEIVED:DEBUG("NAS_CX_RECEIVED\n");return;
      case  NAS_CX_CONNECTING:DEBUG("NAS_CX_CONNECTING\n");return;
      case  NAS_CX_RELEASING:DEBUG("NAS_CX_RELEASING\n");return;
      case  NAS_CX_CONNECTING_FAILURE:DEBUG("NAS_CX_CONNECTING_FAILURE\n");return;
      case  NAS_CX_RELEASING_FAILURE:DEBUG("NAS_CX_RELEASING_FAILURE\n");return;
      case  NAS_RB_ESTABLISHING:DEBUG("NAS_RB_ESTABLISHING\n");return;
      case  NAS_RB_RELEASING:DEBUG("NAS_RB_RELEASING\n");return;
      case  NAS_RB_DCH:DEBUG("NAS_RB_DCH\n");return;
      default: ERR(" Unknown state\n");
    }
}


//---------------------------------------------------------------------------
// Convert the IMEI to iid
void RAL_imei2iid(uint8_t *imei, uint8_t *iid){
//---------------------------------------------------------------------------
// Start debug information
  if (!imei ||! iid ){
       ERR("RAL_imei2iid - input parameter is NULL \n");
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
  iid[7] = 16*imei[14]+imei[15]+1;
}

//---------------------------------------------------------------------------
int RAL_process_NAS_message(int ioctl_obj, int ioctl_cmd, int ioctl_cellid){
//---------------------------------------------------------------------------
  int err, rc;

  NOTICE(" \n");

  switch (ioctl_obj){
/***************************/
      case IO_OBJ_STATS:
          {
            struct nas_msg_statistic_reply *msgrep;

            //printf("Statistics requested -0\n");
            gifr.type=NAS_MSG_STATISTIC_REQUEST;
            //              gifr.msg=(char *)malloc(sizeof(struct nas_msg_statistic_reply));
            memset (ralpriv->buffer,0,800);
            gifr.msg= &(ralpriv->buffer[0]);
            msgrep=(struct nas_msg_statistic_reply *)(gifr.msg);
            NOTICE("[MSC_MSG][%s][%s][--- ioctl : NAS_MSG_STATISTIC_REQUEST --->][nas]\n", getTimeStamp4Log(), g_link_id);
            err=ioctl(fd, NASMT_IOCTL_RAL, &gifr);
            if (err<0){
              ERR("IOCTL error, err=%d\n",err);
              rc = -1;
            } else {
              DEBUG("tx_packets = %u, rx_packets = %u\n", msgrep->tx_packets, msgrep->rx_packets);
              DEBUG("tx_bytes = %u, rx_bytes = %u\n", msgrep->tx_bytes, msgrep->rx_bytes);
              DEBUG("tx_errors = %u, rx_errors = %u\n", msgrep->tx_errors, msgrep->rx_errors);
              DEBUG("tx_dropped = %u, rx_dropped = %u\n", msgrep->tx_dropped, msgrep->rx_dropped);
              memcpy(&(ralpriv->rx_packets), &(msgrep->rx_packets), sizeof(struct nas_msg_statistic_reply));
              //IAL_return_NAS_Statistics();
              rc = 0;
            }
          }
      break;
/***************************/
      case IO_OBJ_CNX:
         switch (ioctl_cmd){
      /***/
            case IO_CMD_ADD:
              {
                //printf("Usage: gioctl cx add <lcr> <cellid>\n");
                 struct nas_msg_cx_establishment_request *msgreq;
                 struct nas_msg_cx_establishment_reply *msgrep;
                 gifr.type=NAS_MSG_CX_ESTABLISHMENT_REQUEST;
                 //gifr.msg=(char *)malloc(sizeof(msgrep)<sizeof(msgreq)?sizeof(msgreq):sizeof(msgrep));
                 memset (ralpriv->buffer,0,800);
                 gifr.msg= &(ralpriv->buffer[0]);
                 msgreq=(struct nas_msg_cx_establishment_request *)(gifr.msg);
                 msgrep=(struct nas_msg_cx_establishment_reply *)(gifr.msg);
                 msgreq->lcr=0; //Temp <lcr>
                 msgreq->cellid=ioctl_cellid;
                 // send IOCTL
                 NOTICE("[MSC_MSG][%s][%s][--- ioctl : NAS_MSG_CX_ESTABLISHMENT_REQUEST --->][nas]\n", getTimeStamp4Log(), g_link_id);
                 err=ioctl(fd, NASMT_IOCTL_RAL, &gifr);
                 if (err<0){
                     ERR("IOCTL error, err=%d\n",err);
                 }
                 NOTICE("[MSC_MSG][%s][nas][--- NAS_MSG_CX_ESTABLISHMENT_REPLY --->][%s]\n", getTimeStamp4Log(), g_link_id);
                 ralpriv->pending_req_action = MIH_C_LINK_AC_TYPE_NONE;
                 ralpriv->state = DISCONNECTED;
                 if ((err<0)||(msgrep->status<0)){
                     ERR(" Connexion establishment failure: %d\n",msgrep->status);
                     //ralpriv->state = DISCONNECTED;
                     rc = -1;
                     ralpriv->pending_req_status    = MIH_C_STATUS_SUCCESS;
                     ralpriv->pending_req_ac_result = MIH_C_LINK_AC_RESULT_FAILURE;
                 }else{
                     //ralpriv->state = CONNECTED;
                     ralpriv->cell_id = ioctl_cellid;
                     ralpriv->pending_req_status    = MIH_C_STATUS_SUCCESS;
                     ralpriv->pending_req_ac_result = MIH_C_LINK_AC_RESULT_SUCCESS;
                     ralpriv->pending_req_flag = 1;
                     DEBUG(" Connexion establishment pending: pending_req_flag %d\n",ralpriv->pending_req_flag);
                 }
                 //mRAL_send_link_action_confirm(&ralpriv->pending_req_transaction_id, &ralpriv->pending_req_status, NULL, &ralpriv->pending_req_ac_result);
                 rc = 0;
              }
              break;
      /***/
            case IO_CMD_DEL:
              {
                 //printf("Usage: gioctl cx del <lcr>\n");
                 struct nas_msg_cx_release_request *msgreq;
                 struct nas_msg_cx_release_reply *msgrep;
                 gifr.type=NAS_MSG_CX_RELEASE_REQUEST;
                 //gifr.msg=(char *)malloc(sizeof(msgrep)<sizeof(msgreq)?sizeof(msgreq):sizeof(msgrep));
                 memset (ralpriv->buffer,0,800);
                 gifr.msg= &(ralpriv->buffer[0]);
                 msgreq=(struct nas_msg_cx_release_request *)(gifr.msg);
                 msgrep=(struct nas_msg_cx_release_reply *)(gifr.msg);
                 msgreq->lcr=0; //Temp <lcr>
                 //
                 NOTICE("[MSC_MSG][%s][%s][--- ioctl : NAS_MSG_CX_RELEASE_REQUEST --->][nas]\n", getTimeStamp4Log(), g_link_id);
                 err=ioctl(fd, NASMT_IOCTL_RAL, &gifr);
                 if (err<0){
                     ERR("IOCTL error, err=%d\n",err);
                     rc = -1;
                 }
                 //
                 NOTICE("[MSC_MSG][%s][nas][--- NAS_MSG_CX_RELEASE_REPLY --->][%s]\n", getTimeStamp4Log(), g_link_id);
                 ralpriv->pending_req_action = MIH_C_LINK_AC_TYPE_NONE;
                 if (msgrep->status<0){
                     ERR(" Connexion release failure: %d", msgrep->status);
                     rc = -1;
                 }
                 if (rc == -1){ // something failed
                     ralpriv->pending_req_ac_result = MIH_C_LINK_AC_RESULT_FAILURE;
                 } else {
                     ralpriv->pending_req_ac_result = MIH_C_LINK_AC_RESULT_SUCCESS;
                 }
                 ralpriv->pending_req_status = MIH_C_STATUS_SUCCESS;
                 ralpriv->state = DISCONNECTED;
                 //mRALu_send_link_switch_cnf();
                 ralpriv->pending_req_flag = 0;
                 ralpriv->cell_id = CONF_UNKNOWN_CELL_ID;
                 rc = 0;
              }
              break;
      /***/
            case IO_CMD_LIST:
              {
                 //printf("Usage: gioctl cx list\n");
                 uint8_t *msgrep;
                 uint8_t i;
                 struct nas_msg_cx_list_reply *list;
                 uint8_t lcr;
                 gifr.type=NAS_MSG_CX_LIST_REQUEST;
                 //gifr.msg=(char *)malloc(NAS_LIST_CX_MAX*sizeof(struct nas_msg_cx_list_reply)+1);
                 memset (ralpriv->buffer,0,800);
                 gifr.msg= &(ralpriv->buffer[0]);
                 msgrep=(uint8_t *)(gifr.msg);
                //
                 NOTICE("[MSC_MSG][%s][%s][--- ioctl : NAS_MSG_CX_LIST_REQUEST --->][nas]\n", getTimeStamp4Log(), g_link_id);
                 err=ioctl(fd, NASMT_IOCTL_RAL, &gifr);
                 if (err<0){
                     ERR("IOCTL error, err=%d\n",err);
                     rc = -1;
                 } else {
                     NOTICE("[MSC_MSG][%s][nas][--- NAS_UE_MSG_CNX_LIST_REPLY --->][%s]\n", getTimeStamp4Log(), g_link_id);
                     DEBUG("Lcr\t\tCellId\tIID4\tIID6\t\t\tnum_rb\tnsclass\tState\n");
                     list=(struct nas_msg_cx_list_reply *)(msgrep+1);
                     lcr=0;
                     DEBUG("%u\t\t%u\t%u\t", list[lcr].lcr, list[lcr].cellid, list[lcr].iid4);
                     for (i=0;i<8;++i)
                         DEBUG("%02x", *((uint8_t *)list[lcr].iid6+i));
                     DEBUG("\t%u\t%u\t", list[lcr].num_rb, list[lcr].nsclassifier);
                     print_state(list[lcr].state);

                     ralpriv->nas_state = list[lcr].state ;
                     ralpriv->num_rb = list[lcr].num_rb;
                     ralpriv->num_class = list[lcr].nsclassifier;
                     ralpriv->cell_id = list[lcr].cellid ;
                     //IAL_return_NAS_StatAttach();
                     rc = 0;
                 }
              }
              break;
      /***/
          default:
          ERR ("IAL_process_NAS_message : IO_OBJ_CNX : invalid ioctl command %d\n",ioctl_cmd);
              rc= -1;
      } //end switch ioctl_cmd
      break;

/***************************/
      case IO_OBJ_RB:
         switch (ioctl_cmd){
      /***/
            case IO_CMD_LIST:
              {
                 //printf("Usage: gioctl rb list <lcr>\n");
                 uint8_t *msgrep;
                 uint8_t rbi;
                 struct nas_msg_rb_list_reply *list;
                 struct nas_msg_rb_list_request *msgreq;
                 gifr.type=NAS_MSG_RB_LIST_REQUEST;
                 // gifr.msg=(char *)malloc(NAS_LIST_RB_MAX*sizeof(struct nas_msg_rb_list_reply)+1);
                 memset (ralpriv->buffer,0,800);
                 gifr.msg= &(ralpriv->buffer[0]);
                 msgreq=(struct nas_msg_rb_list_request *)(gifr.msg);
                 msgrep=(uint8_t *)(gifr.msg);
                 msgreq->lcr=0; //Temp <lcr> - MT
                //
                 NOTICE("[MSC_MSG][%s][%s][--- ioctl : NAS_MSG_RB_LIST_REQUEST --->][nas]\n", getTimeStamp4Log(), g_link_id);
                 err=ioctl(fd, NASMT_IOCTL_RAL, &gifr);
                 if (err<0){
                     ERR("IOCTL error, err=%d\n",err);
                     rc = -1;
                 } else {
                    DEBUG("rab_id\t\tSapi\t\tQoS\t\tState\n");
                    list=(struct nas_msg_rb_list_reply *)(msgrep+1);
                    for (rbi=0; rbi<msgrep[0]; ++rbi){
                        DEBUG("%u\t\t%u\t\t%u\t\t", list[rbi].rab_id, list[rbi].sapi, list[rbi].qos);
                        print_state(list[rbi].state);
                        rc = 0;
                    }
                    //
                    ralpriv->num_rb = msgrep[0];
                    for (rbi=0; rbi<ralpriv->num_rb; rbi++){
                        ralpriv->rbId[rbi]= list[rbi].rab_id;
                        ralpriv->QoSclass[rbi] = list[rbi].qos;
                        ralpriv->dscp[rbi] = rbi+4;
                    }
                    //IAL_return_NAS_StatRB();
                 }
              }
              break;
      /***/
            default:
            ERR ("IAL_process_NAS_message : IO_OBJ_RB : invalid ioctl command %d\n",ioctl_cmd);
                rc= -1;
         } //end switch ioctl_cmd    
      break;
/***************************/
      case IO_OBJ_MEAS:
        //
         {
           uint8_t i;
           struct nas_msg_measure_request *msgreq;
           struct nas_msg_measure_reply *msgrep;
           gifr.type=NAS_MSG_MEAS_REQUEST;
           // gifr.msg=(char *)malloc(sizeof(msgrep)<sizeof(msgreq)?sizeof(msgreq):sizeof(msgrep));
           memset (ralpriv->buffer,0,800);
           gifr.msg= &(ralpriv->buffer[0]);
           msgreq=(struct nas_msg_measure_request *)(gifr.msg);
           msgrep=(struct nas_msg_measure_reply *)(gifr.msg);
           //
           NOTICE("[MSC_MSG][%s][%s][--- ioctl : NAS_MSG_MEAS_REQUEST --->][nas]\n", getTimeStamp4Log(), g_link_id);
           err=ioctl(fd, NASMT_IOCTL_RAL, &gifr);
           if (err<0){
               ERR("IOCTL error, err=%d\n",err);
               rc = -1;
           } else {
           //
               NOTICE("[MSC_MSG][%s][nas][--- NAS_MSG_MEAS_REPLY --->][%s]\n", getTimeStamp4Log(), g_link_id);
               // ralpriv->num_measures = msgrep->num_cells;  TEMP- To be activated later
               for (i=0;i<msgrep->num_cells; i++){
                   ralpriv->meas_cell_id[i] = msgrep-> measures[i].cell_id;
                   IAL_integrate_measure(msgrep-> measures[i].level, i);
                   ralpriv->provider_id[i] = msgrep-> measures[i].provider_id;
               }
               DEBUG("Measurement Report - Number of cells %d - Current cell %d\n", msgrep->num_cells, ralpriv->cell_id);
               #ifdef DEBUG_MRALU_MEASURES
               DEBUG("Cell_id\tMeasure\tProvider_id\n");
               for (i=0;i<msgrep->num_cells; i++){
                   DEBUG(" %d\t%d\t%d\n",msgrep-> measures[i].cell_id,msgrep-> measures[i].level,msgrep-> measures[i].provider_id);
               }
               #endif
               //Temp
               //ralpriv->meas_cell_id[0] = ralpriv->cell_id;
               rc = 0;
           }
        }
      break;
/***************************/
      case IO_OBJ_IMEI:
         {
           struct nas_msg_l2id_reply *msgrep;
           gifr.type=NAS_MSG_IMEI_REQUEST;
           // gifr.msg=(char *)malloc(sizeof(struct nas_msg_statistic_reply));
           memset (ralpriv->buffer,0,800);
           gifr.msg= &(ralpriv->buffer[0]);
           msgrep=(struct nas_msg_l2id_reply *)(gifr.msg);
           NOTICE("[MSC_MSG][%s][%s][--- ioctl : NAS_MSG_IMEI_REQUEST (L2Id) --->][nas]\n", getTimeStamp4Log(), g_link_id);
           err=ioctl(fd, NASMT_IOCTL_RAL, &gifr);
           if (err<0){
               ERR("IOCTL error, err=%d\n",err);
               rc = -1;
               // default IMEI address : NAS_DEFAULT_IMEI
               RAL_imei2iid(NAS_DEFAULT_IMEI, (uint8_t *)&(ralpriv->ipv6_l2id[0]));
           } else {
               NOTICE("[MSC_MSG][%s][nas][--- NAS_MSG_IMEI_REPLY --->][%s]\n", getTimeStamp4Log(), g_link_id);
               DEBUG("IMEI value received= %d %d\n", msgrep->l2id[0], msgrep->l2id[1]);
               //store the received value, to be used later
               ralpriv->ipv6_l2id[0] = msgrep->l2id[0];
               ralpriv->ipv6_l2id[1] = msgrep->l2id[1];
               rc = 0;
          }
        }
      break;
/***************************/
      default:
        ERR ("IAL_process_NAS_message : invalid ioctl object %d\n",ioctl_obj);
        rc= -1;
  } //end switch ioctl_obj    

    return rc;
}


/***************************************************************************
                          lteRALenb_ioctl.c  -  description
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
/*! \file lteRALenb_ioctl.c
 * \brief Handling of ioctl for LTE driver in LTE-RAL-ENB
 * \author WETTERWALD Michelle, GAUTHIER Lionel, MAUREL Frederic
 * \date 2013
 * \company EURECOM
 * \email: michelle.wetterwald@eurecom.fr, lionel.gauthier@eurecom.fr, frederic.maurel@eurecom.fr
 */
/*******************************************************************************
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <net/if.h>
//#include <linux/ipv6.h>
//#include <linux/in.h>
//#include <linux/in6.h>

#include <netinet/in.h>

//
#include "rrc_d_types.h"
//-----------------------------------------------------------------------------
// LTE AS sub-system
//#include "rrc_nas_primitives.h"
//#include "nasrg_constant.h"
//#include "nasrg_iocontrol.h"
//-----------------------------------------------------------------------------
#include "lteRALenb_mih_msg.h"
#include "lteRALenb_constants.h"
#include "lteRALenb_variables.h"
#include "lteRALenb_proto.h"
#include "MIH_C.h"
//#include "MIH_C_Types.h"

extern struct nas_ioctl gifr;
extern int fd;
extern int s_mgr; //socket with QoS Mgr
extern int init_flag;

//---------------------------------------------------------------------------
void print_state(uint8_t state){
//---------------------------------------------------------------------------
 switch(state){
    case  NAS_IDLE:printf("NAS_IDLE\n");return;
    case  NAS_CX_FACH:printf("NAS_CX_FACH\n");return;
    case  NAS_CX_DCH:printf("NAS_CX_DCH\n");return;
    case  NAS_CX_RECEIVED:printf("NAS_CX_RECEIVED\n");return;
    case  NAS_CX_CONNECTING:printf("NAS_CX_CONNECTING\n");return;
    case  NAS_CX_RELEASING:printf("NAS_CX_RELEASING\n");return;
    case  NAS_CX_CONNECTING_FAILURE:printf("NAS_CX_CONNECTING_FAILURE\n");return;
    case  NAS_CX_RELEASING_FAILURE:printf("NAS_CX_RELEASING_FAILURE\n");return;
    case  NAS_RB_ESTABLISHING:printf("NAS_RB_ESTABLISHING\n");return;
    case  NAS_RB_RELEASING:printf("NAS_RB_RELEASING\n");return;
    case  NAS_RB_ESTABLISHED:printf("NAS_RB_ESTABLISHED\n");return;

   default: printf(" Unknown state\n");
 }
}

//---------------------------------------------------------------------------
void RAL_NASinitMTlist(uint8_t *msgrep, int num_mts){
//---------------------------------------------------------------------------
  int mt_ix, ch_ix;
  struct nas_msg_cx_list_reply *list;

  memcpy(g_enb_ral_obj[instanceP].plmn, DefaultPLMN, DEFAULT_PLMN_SIZE); // DUMMY
  list=(struct nas_msg_cx_list_reply *)(msgrep+1);
  num_mts = msgrep[0];
  for(mt_ix=0; mt_ix<num_mts; ++mt_ix){
    if (list[mt_ix].state != NAS_IDLE){
      g_enb_ral_obj[instanceP].cell_id =  list[mt_ix].cellid;
      g_enb_ral_obj[instanceP].mt[mt_ix].ue_id = list[mt_ix].lcr;
      g_enb_ral_obj[instanceP].mt[mt_ix].ipv6_l2id[0]= list[mt_ix].iid6[0];
      g_enb_ral_obj[instanceP].mt[mt_ix].ipv6_l2id[1]= list[mt_ix].iid6[1];
      g_enb_ral_obj[instanceP].mt[mt_ix].num_rbs = list[mt_ix].num_rb;
      g_enb_ral_obj[instanceP].mt[mt_ix].num_class = list[mt_ix].nsclassifier;
      g_enb_ral_obj[instanceP].mt[mt_ix].nas_state = list[mt_ix].state;
      if (g_enb_ral_obj[instanceP].mt[mt_ix].num_class>=2)
        g_enb_ral_obj[instanceP].mt[mt_ix].mt_state= NAS_CONNECTED;
      // enter default rb
      ch_ix = 0;
      g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix].rbId = RAL_DEFAULT_MC_RAB_ID+1;
      g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix].RadioQoSclass = 2;
      g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix].cnx_id = (RAL_MAX_RB_PER_UE*mt_ix)+ch_ix+1;
      g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix].dscpUL = 0;
      g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix].dscpDL = 0;
      g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix].nas_state = NAS_CX_DCH;
      g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix].status = RB_CONNECTED;
      g_enb_ral_obj[instanceP].num_connected_mts++;
      //RAL_printMobileData(mt_ix);
      DEBUG(" MT%d initialized : address %d %d\n", mt_ix, g_enb_ral_obj[instanceP].mt[mt_ix].ipv6_l2id[0], g_enb_ral_obj[instanceP].mt[mt_ix].ipv6_l2id[1]);
    }
  }
}

//---------------------------------------------------------------------------
void RAL_NASupdatetMTlist(uint8_t *msgrep, int num_mts){
//---------------------------------------------------------------------------
  int mt_ix, ch_ix;
  struct nas_msg_cx_list_reply *list;
  //MIH_C_LINK_TUPLE_ID_T* ltid;
  MIH_C_LINK_ADDR_T new_ar;
  MIH_C_LINK_DN_REASON_T reason_code;
  MIH_C_TRANSACTION_ID_T transaction_id;
  int previous_num_class;

  list=(struct nas_msg_cx_list_reply *)(msgrep+1);
  num_mts = msgrep[0];
  for(mt_ix=0; mt_ix<num_mts; ++mt_ix){
  // check if MT already known
    if ((g_enb_ral_obj[instanceP].mt[mt_ix].ipv6_l2id[0]== list[mt_ix].iid6[0])&&
          (g_enb_ral_obj[instanceP].mt[mt_ix].ipv6_l2id[1]== list[mt_ix].iid6[1])){
      // MT already known - update
        g_enb_ral_obj[instanceP].mt[mt_ix].num_rbs = list[mt_ix].num_rb;
        previous_num_class = g_enb_ral_obj[instanceP].mt[mt_ix].num_class;
        g_enb_ral_obj[instanceP].mt[mt_ix].num_class = list[mt_ix].nsclassifier;
        if (g_enb_ral_obj[instanceP].mt[mt_ix].num_class>=2)
          g_enb_ral_obj[instanceP].mt[mt_ix].mt_state= RB_CONNECTED;
      //check if state has changed - MT disconnected FFS
      if ((g_enb_ral_obj[instanceP].mt[mt_ix].nas_state==NAS_CX_DCH)&&(list[mt_ix].state == NAS_IDLE)){
         DEBUG ("\n\n");
         DEBUG (" MOBILE TERMINAL %d IS NOW IDLE.\n\n",mt_ix);
         // TODO Send linkdown
      }
      //check if state has changed - MT reconnected FFS
      if ((g_enb_ral_obj[instanceP].mt[mt_ix].nas_state==NAS_IDLE)&&(list[mt_ix].state == NAS_CX_DCH)){
         DEBUG ("\n\n");
         DEBUG (" MOBILE TERMINAL %d WAS IDLE AND IS NOW CONNECTED.\n\n",mt_ix);
      }
      //check if MT is completely connected
      if ((g_enb_ral_obj[instanceP].mt[mt_ix].num_class - previous_num_class)&&(list[mt_ix].state == NAS_CX_DCH)){
         DEBUG ("\n\n");
         DEBUG (" MOBILE TERMINAL %d IS NOW COMPLETELY CONNECTED.\n\n",mt_ix);
         // send linkup: new_ar will contain the address from the MT
         new_ar.choice = MIH_C_CHOICE_3GPP_ADDR;
         MIH_C_3GPP_ADDR_set(&(new_ar._union._3gpp_addr), (u_int8_t*)&(g_enb_ral_obj[instanceP].mt[mt_ix].ipv6_l2id[0]), strlen(DEFAULT_ADDRESS_3GPP));

         eRALlte_send_link_up_indication(&g_enb_ral_obj[instanceP].pending_req_transaction_id, &g_enb_ral_obj[instanceP].mt[mt_ix].ltid, NULL, &new_ar, NULL, NULL);
         //eRALlte_send_link_up_indication(&g_enb_ral_obj[instanceP].pending_req_transaction_id, &g_enb_ral_obj[instanceP].mt[mt_ix].ltid, NULL, NULL, NULL, NULL);
          // if RAL realtime and MEASURES are enabled, start the measuring process in RRC+Driver
          #ifdef RAL_REALTIME
          #ifdef ENABLE_MEDIEVAL_DEMO3
          RAL_process_NAS_message(IO_OBJ_MEAS, IO_CMD_ADD,0,0);
          #endif
          #endif

      }
	  //check enter sleep mode
      if ((g_enb_ral_obj[instanceP].mt[mt_ix].nas_state==NAS_CX_DCH)&&(list[mt_ix].state == NAS_CX_RELEASING)){
         DEBUG ("\n\n");
         DEBUG (" MOBILE TERMINAL %d is entering sleep mode. Send LinkDown.\n\n",mt_ix);
         // send linkdown: old_ar (represented by new_ar variable) will contain the address from the MT
         transaction_id = MIH_C_get_new_transaction_id();
         reason_code = MIH_C_LINK_DOWN_REASON_EXPLICIT_DISCONNECT;
         new_ar.choice = MIH_C_CHOICE_3GPP_ADDR;
         MIH_C_3GPP_ADDR_set(&(new_ar._union._3gpp_addr), (u_int8_t*)&(g_enb_ral_obj[instanceP].mt[mt_ix].ipv6_l2id[0]), strlen(DEFAULT_ADDRESS_3GPP));
         eRALlte_send_link_down_indication(&transaction_id, &g_enb_ral_obj[instanceP].mt[mt_ix].ltid, &new_ar, &reason_code);
      }
 	  // check leave sleep mode
      if ((g_enb_ral_obj[instanceP].mt[mt_ix].nas_state==NAS_CX_RELEASING)&&(list[mt_ix].state == NAS_CX_DCH)){
         DEBUG ("\n\n");
         DEBUG (" MOBILE TERMINAL %d WAS IN SLEEP MODE AND IS NOW ACTIVATED.\n\n",mt_ix);
         // send linkup: new_ar will contain the address from the MT
         new_ar.choice = MIH_C_CHOICE_3GPP_ADDR;
         MIH_C_3GPP_ADDR_set(&(new_ar._union._3gpp_addr), (u_int8_t*)&(g_enb_ral_obj[instanceP].mt[mt_ix].ipv6_l2id[0]), strlen(DEFAULT_ADDRESS_3GPP));
         eRALlte_send_link_up_indication(&g_enb_ral_obj[instanceP].pending_req_transaction_id, &g_enb_ral_obj[instanceP].mt[mt_ix].ltid, NULL, &new_ar, NULL, NULL);
      }
        g_enb_ral_obj[instanceP].mt[mt_ix].nas_state = list[mt_ix].state;
    }else{
      // MT unknown or different
      if (list[mt_ix].state != NAS_IDLE){
        DEBUG ("\n\n");
        DEBUG (" NEW TERMINAL %d DETECTED.\n\n",mt_ix);
        g_enb_ral_obj[instanceP].mt[mt_ix].ue_id = list[mt_ix].lcr;
        g_enb_ral_obj[instanceP].mt[mt_ix].ipv6_l2id[0]= list[mt_ix].iid6[0];
        g_enb_ral_obj[instanceP].mt[mt_ix].ipv6_l2id[1]= list[mt_ix].iid6[1];
        g_enb_ral_obj[instanceP].mt[mt_ix].num_rbs = list[mt_ix].num_rb;
        g_enb_ral_obj[instanceP].mt[mt_ix].num_class = list[mt_ix].nsclassifier;
        // initialize ltid (MIH_C_LINK_TUPLE_ID_T) for that mobile
        // first version
        //g_enb_ral_obj[instanceP].mt[mt_ix].ltid.link_id.link_type = MIH_C_WIRELESS_UMTS;
        //g_enb_ral_obj[instanceP].mt[mt_ix].ltid.link_id.link_addr.choice = MIH_C_CHOICE_3GPP_ADDR;
        //MIH_C_3GPP_ADDR_set(&g_enb_ral_obj[instanceP].mt[mt_ix].ltid.link_id.link_addr._union._3gpp_addr, (u_int8_t*)&(g_enb_ral_obj[instanceP].mt[mt_ix].ipv6_l2id[0]), strlen(DEFAULT_ADDRESS_3GPP));
        //g_enb_ral_obj[instanceP].mt[mt_ix].ltid.choice = MIH_C_LINK_TUPLE_ID_CHOICE_LINK_ADDR;
        // SECOND Version
        g_enb_ral_obj[instanceP].mt[mt_ix].ltid.link_id.link_type = MIH_C_WIRELESS_UMTS;
        g_enb_ral_obj[instanceP].mt[mt_ix].ltid.choice = MIH_C_LINK_TUPLE_ID_CHOICE_NULL;
        g_enb_ral_obj[instanceP].mt[mt_ix].ltid.link_id.link_addr.choice = MIH_C_CHOICE_3GPP_3G_CELL_ID;
        Bit_Buffer_t *plmn = new_BitBuffer_0();
        BitBuffer_wrap(plmn, (unsigned char*) g_enb_ral_obj[instanceP].plmn, DEFAULT_PLMN_SIZE);
        MIH_C_PLMN_ID_decode(plmn, &g_enb_ral_obj[instanceP].mt[mt_ix].ltid.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id);
        free_BitBuffer(plmn);
        g_enb_ral_obj[instanceP].mt[mt_ix].ltid.link_id.link_addr._union._3gpp_3g_cell_id.cell_id = g_enb_ral_obj[instanceP].cell_id;
        g_enb_ral_obj[instanceP].mt[mt_ix].ltid.choice = MIH_C_LINK_TUPLE_ID_CHOICE_NULL;

        // check state of the UE connection
        g_enb_ral_obj[instanceP].mt[mt_ix].nas_state = list[mt_ix].state;
        if (g_enb_ral_obj[instanceP].mt[mt_ix].num_class>=2){
          g_enb_ral_obj[instanceP].mt[mt_ix].mt_state= RB_CONNECTED;
          // send linkup
          //ltid = &g_enb_ral_obj[instanceP].mt[mt_ix].ltid;
          DEBUG (" MOBILE TERMINAL %d IS COMPLETELY CONNECTED.\n\n",mt_ix);
          // new_ar will contain the address from the MT
          new_ar.choice = MIH_C_CHOICE_3GPP_ADDR;
          MIH_C_3GPP_ADDR_set(&(new_ar._union._3gpp_addr), (u_int8_t*)&(g_enb_ral_obj[instanceP].mt[mt_ix].ipv6_l2id[0]), strlen(DEFAULT_ADDRESS_3GPP));

          eRALlte_send_link_up_indication(&g_enb_ral_obj[instanceP].pending_req_transaction_id, &g_enb_ral_obj[instanceP].mt[mt_ix].ltid, NULL, &new_ar, NULL, NULL);
          // if RAL realtime and MEASURES are enabled, start the measuring process in RRC+Driver
          #ifdef RAL_REALTIME
          #ifdef ENABLE_MEDIEVAL_DEMO3
          RAL_process_NAS_message(IO_OBJ_MEAS, IO_CMD_ADD,0,0);
          #endif
          #endif
        }
        // enter default rb
        ch_ix = 0;
        g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix].rbId = RAL_DEFAULT_MC_RAB_ID+1;
        g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix].RadioQoSclass = 2;
        g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix].cnx_id = (RAL_MAX_RB_PER_UE*mt_ix)+ch_ix+1;
        g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix].dscpUL = 0;
        g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix].dscpDL = 0;
        g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix].nas_state = NAS_CX_DCH;
        g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix].status = RB_CONNECTED;
        g_enb_ral_obj[instanceP].num_connected_mts++;
        //RAL_printMobileData(mt_ix);
      }
    } // end if MT unknonwn
  } // end for loop
}

//---------------------------------------------------------------------------
void RAL_verifyPendingRbStatus(void){
//---------------------------------------------------------------------------
// g_enb_ral_obj[instanceP].mcast.radio_channel.status = RB_CONNECTED;
    int mt_ix, ch_ix;
    MIH_C_LINK_TUPLE_ID_T* ltid;
    //int is_unicast;

    mt_ix =  g_enb_ral_obj[instanceP].pending_req_mt_ix;
    ch_ix =  g_enb_ral_obj[instanceP].pending_req_ch_ix;

    if ((g_enb_ral_obj[instanceP].pending_req_flag)%5==0){
        DEBUG("Pending Req Flag %d, Mobile %d, channel %d\n", g_enb_ral_obj[instanceP].pending_req_flag, mt_ix, ch_ix);
        if (g_enb_ral_obj[instanceP].pending_req_multicast == RAL_TRUE){
          mt_ix =0;
          ch_ix =1;
        }
        RAL_process_NAS_message(IO_OBJ_RB, IO_CMD_LIST,mt_ix,0);
        if ((g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix].status == RB_CONNECTED)||((g_enb_ral_obj[instanceP].pending_req_flag) > 100)){
           DEBUG("RAL_verifyPendingRbStatus -in- mt_ix %d - ch_ix %d \n", mt_ix, ch_ix );
           // send confirmation to upper layer
           g_enb_ral_obj[instanceP].pending_req_status = MIH_C_STATUS_SUCCESS;
           if ((g_enb_ral_obj[instanceP].pending_req_flag) > 100){
              g_enb_ral_obj[instanceP].pending_req_status = MIH_C_STATUS_REJECTED;
              if (mt_ix == RAL_MAX_MT)
                 eRALlte_process_clean_channel(&(g_enb_ral_obj[instanceP].mcast.radio_channel));
              else
                 eRALlte_process_clean_channel(&(g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix]));
           }
           if (mt_ix == RAL_MAX_MT)
              ltid = &g_enb_ral_obj[instanceP].mcast.ltid;
           else
              ltid = &g_enb_ral_obj[instanceP].mt[mt_ix].ltid;

           if (!g_enb_ral_obj[instanceP].pending_mt_flag)
              // To be updated and completed
              //aRALu_send_link_res_activate_cnf();
              eRALlte_send_link_up_indication(&g_enb_ral_obj[instanceP].pending_req_transaction_id, ltid, NULL, NULL, NULL, NULL);
           else
              g_enb_ral_obj[instanceP].pending_mt_flag = 0;
           g_enb_ral_obj[instanceP].pending_req_flag = 0;
           DEBUG("After response, Pending Req Flag = 0 , %d\n", g_enb_ral_obj[instanceP].pending_req_flag);
        }
        //DEBUG("RAL_verifyPendingRbStatus - 2-  \n");
    }
}

//---------------------------------------------------------------------------
int RAL_process_NAS_message(int ioctl_obj, int ioctl_cmd, int mt_ix, int ch_ix){
//---------------------------------------------------------------------------
 int err, rc;
//  int mt_ix, ch_ix;
  unsigned int cnxid;

// DEBUG ("\n%d , %d,", mt_ix, ch_ix);
  switch (ioctl_obj){
      case IO_OBJ_STATS:
          {
            DEBUG("Statistics requested -- FFS \n");
          }
      break;
      case IO_OBJ_CNX:
         switch (ioctl_cmd){
            case IO_CMD_LIST:
              {
                 // printf("Usage: gioctl cx list\n");
                uint8_t *msgrep;
                uint8_t i;
                struct nas_msg_cx_list_reply *list;
                uint8_t lcr;
                short int num_mts;

                gifr.type=NAS_MSG_CX_LIST_REQUEST;
                //gifr.msg=(char *)malloc(NAS_LIST_CX_MAX*sizeof(struct nas_msg_cx_list_reply)+1);
                memset (g_enb_ral_obj[instanceP].buffer,0,800);
                gifr.msg= &(g_enb_ral_obj[instanceP].buffer[0]);
                msgrep=(uint8_t *)(gifr.msg);
                //
                DEBUG("--\n");
                DEBUG("Connexion list requested\n");
                err=ioctl(fd, NASRG_IOCTL_RAL, &gifr);
                if (err<0){
                  ERR("IOCTL error, err=%d\n",err);
                  rc = -1;
                }
                // Print result
                DEBUG("Lcr\t\tCellId\tIID4\tIID6\t\t\tnum_rb\tnsclass\tState\n");
                list=(struct nas_msg_cx_list_reply *)(msgrep+1);
                num_mts = msgrep[0];
                for(lcr=0; lcr<num_mts; ++lcr){
                  DEBUG("%u\t\t%u\t%u\t", list[lcr].lcr, list[lcr].cellid, list[lcr].iid4);
                  for (i=0;i<8;++i)
                    DEBUG("%02x", *((uint8_t *)list[lcr].iid6+i));
                  DEBUG("\t%u\t%u\t", list[lcr].num_rb, list[lcr].nsclassifier);
                  print_state(list[lcr].state);
                }
                if (init_flag){
                  RAL_NASinitMTlist(msgrep, num_mts);
                }else{
                  RAL_NASupdatetMTlist(msgrep, num_mts);
                }
                rc = 0;
              }
          break;
          default:
          ERR ("RAL_process_NAS_message : invalid ioctl command %d\n",ioctl_cmd);
        rc= -1;
      } //end switch ioctl_cmd 
      break;

      case IO_OBJ_RB:
         switch (ioctl_cmd){
            case IO_CMD_ADD:
              {
                // printf("Usage: gioctl rb add <lcr> <rab_id> <qos>\n");
                struct nas_msg_rb_establishment_request *msgreq;
                struct nas_msg_rb_establishment_reply *msgrep;
                struct ral_lte_channel *currChannel;
                MIH_C_LINK_TUPLE_ID_T* ltid;
                MIH_C_LINK_DN_REASON_T reason_code;
                //
                gifr.type=NAS_MSG_RB_ESTABLISHMENT_REQUEST;
                memset (g_enb_ral_obj[instanceP].buffer,0,800);
                gifr.msg= &(g_enb_ral_obj[instanceP].buffer[0]);
                msgreq=(struct nas_msg_rb_establishment_request *)(gifr.msg);
                msgrep=(struct nas_msg_rb_establishment_reply *)(gifr.msg);
                //
                if (mt_ix == RAL_MAX_MT){
                    // multicast
                    currChannel = &(g_enb_ral_obj[instanceP].mcast.radio_channel);
                    msgreq->lcr = mt_ix;
                    memcpy ((char *)&(msgreq->mcast_group), (char *)&(g_enb_ral_obj[instanceP].mcast.mc_group_addr), 16);
                }else{
                    // unicast
                    currChannel = &(g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix]);
                    msgreq->lcr = g_enb_ral_obj[instanceP].mt[mt_ix].ue_id;
                }
                msgreq->cnxid  = currChannel->cnx_id;
                msgreq->rab_id = currChannel->rbId;
                msgreq->qos    = currChannel->RadioQoSclass;
                msgreq->dscp_ul = currChannel->dscpUL;
                msgreq->dscp_dl = currChannel->dscpDL;
                msgreq->mcast_flag  = currChannel->multicast;
                cnxid = msgreq->cnxid;
                currChannel->status = RB_DISCONNECTED;
                //
                DEBUG("Radio bearer establishment requested, cnxid %d\n", cnxid);
                err=ioctl(fd, NASRG_IOCTL_RAL, &gifr);
                if (err<0){
                  ERR("IOCTL error, err=%d\n",err);
                  rc = -1;
                }
                //  check answer from NAS
                msgrep->cnxid = cnxid; // Temp - hardcoded

                if ((msgrep->status<0)||(msgrep->cnxid!=cnxid)||(err<0)){
                  ERR(" Radio bearer establishment failure: %d\n",msgrep->status);
                  currChannel->status = RB_DISCONNECTED;
                  rc = -1;
                  g_enb_ral_obj[instanceP].pending_req_status = MIH_C_STATUS_REJECTED;
                  if (g_enb_ral_obj[instanceP].pending_mt_flag){
                     reason_code = MIH_C_LINK_DOWN_REASON_NO_RESOURCE;
                     if (mt_ix == RAL_MAX_MT)
                         ltid = &g_enb_ral_obj[instanceP].mcast.ltid;
                     else
                         ltid = &g_enb_ral_obj[instanceP].mt[mt_ix].ltid;
                     eRALlte_send_link_down_indication(&g_enb_ral_obj[instanceP].pending_req_transaction_id, ltid, NULL, &reason_code);
                    // aRALu_send_link_res_activate_cnf();
                  }
                  if (mt_ix == RAL_MAX_MT)
                     eRALlte_process_clean_channel(&(g_enb_ral_obj[instanceP].mcast.radio_channel));
                  else
                     eRALlte_process_clean_channel(&(g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix]));
                }else{
                  rc = 0;
                  g_enb_ral_obj[instanceP].pending_req_flag = 1;
                  g_enb_ral_obj[instanceP].pending_req_mt_ix = mt_ix;
                  g_enb_ral_obj[instanceP].pending_req_ch_ix = ch_ix;
                  g_enb_ral_obj[instanceP].pending_req_multicast = currChannel->multicast;
                  DEBUG("-1- pending_req_mt_ix %d, pending_req_ch_ix %d\n", g_enb_ral_obj[instanceP].pending_req_mt_ix, g_enb_ral_obj[instanceP].pending_req_ch_ix);
                }
              }
          break;

             case IO_CMD_DEL:
              {
                //  printf("Usage: gioctl rb del <lcr> <rab_id>\n");
                struct nas_msg_rb_release_request *msgreq;
                struct nas_msg_rb_release_reply *msgrep;
                struct ral_lte_channel *currChannel;
                MIH_C_LINK_TUPLE_ID_T* ltid;
                MIH_C_LINK_DN_REASON_T reason_code;
                //
                gifr.type=NAS_MSG_RB_RELEASE_REQUEST;
                memset (g_enb_ral_obj[instanceP].buffer,0,800);
                gifr.msg= &(g_enb_ral_obj[instanceP].buffer[0]);
                msgreq=(struct nas_msg_rb_release_request *)(gifr.msg);
                msgrep=(struct nas_msg_rb_release_reply *)(gifr.msg);
                //
                if (mt_ix == RAL_MAX_MT){
                    currChannel = &(g_enb_ral_obj[instanceP].mcast.radio_channel);
                    msgreq->lcr = mt_ix;
                }else{
                    currChannel = &(g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix]);
                    msgreq->lcr = g_enb_ral_obj[instanceP].mt[mt_ix].ue_id;
                }
                msgreq->rab_id = currChannel->rbId;
                msgreq->cnxid = currChannel->cnx_id;
                msgreq->mcast_flag  = currChannel->multicast;
                cnxid = msgreq->cnxid;
                //
                DEBUG(" Radio Bearer release requested\n");
                err=ioctl(fd, NASRG_IOCTL_RAL, &gifr);
                if (err<0){
                  ERR("IOCTL error, err=%d\n",err);
                  rc = -1;
                }
                //  check answer from NAS
                msgrep->cnxid = cnxid; // Temp - hardcoded
                if ((msgrep->status<0)||(msgrep->cnxid!=cnxid)||(err<0)){
                  ERR(" Radio bearer release failure: status %d\n", msgrep->status);
                  rc = -1;
                  g_enb_ral_obj[instanceP].pending_req_status = MIH_C_STATUS_REJECTED;
                }else{
                  g_enb_ral_obj[instanceP].pending_req_status = MIH_C_STATUS_SUCCESS;
                }
                if (g_enb_ral_obj[instanceP].pending_mt_flag){
                   reason_code = MIH_C_LINK_DOWN_REASON_EXPLICIT_DISCONNECT;
                   if (mt_ix == RAL_MAX_MT)
                       ltid = &g_enb_ral_obj[instanceP].mcast.ltid;
                   else
                       ltid = &g_enb_ral_obj[instanceP].mt[mt_ix].ltid;
                   eRALlte_send_link_down_indication(&g_enb_ral_obj[instanceP].pending_req_transaction_id, ltid, NULL, &reason_code);
                    // aRALu_send_link_res_deactivate_cnf();
                }

                // mark resource as free again anyway
                if (mt_ix == RAL_MAX_MT)
                   eRALlte_process_clean_channel(&(g_enb_ral_obj[instanceP].mcast.radio_channel));
                else
                   eRALlte_process_clean_channel(&(g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix]));
                DEBUG("Channel released : UE %d, channel %d, cnx_id %d \n\n",mt_ix, ch_ix, cnxid);
              }
          break;


            case IO_CMD_LIST:
              {
               // printf("Usage: gioctl rb list <lcr>\n");
                uint8_t *msgrep;
                uint8_t rbi, i;
                uint8_t num_rbs;
                struct nas_msg_rb_list_reply *list;
                struct nas_msg_rb_list_request *msgreq;
                gifr.type=NAS_MSG_RB_LIST_REQUEST;
                memset (g_enb_ral_obj[instanceP].buffer,0,800);
                gifr.msg= &(g_enb_ral_obj[instanceP].buffer[0]);
                msgreq=(struct nas_msg_rb_list_request *)(gifr.msg);
                msgrep=(uint8_t *)(gifr.msg);
                if (mt_ix < RAL_MAX_MT){
                    msgreq->lcr = g_enb_ral_obj[instanceP].mt[mt_ix].ue_id;
                }else{
                    msgreq->lcr = 0;
                    mt_ix =0;  //Temp
                }
                //
                DEBUG(" Radio bearer list requested\n");
                err=ioctl(fd, NASRG_IOCTL_RAL, &gifr);
                if (err<0){
                  ERR("IOCTL error, err=%d\n",err);
                  rc = -1;
                }
                num_rbs = msgrep[0];
                //RAL_print_buffer (msgrep, 50);
                DEBUG("number of radio bearers %d \n", num_rbs);
                DEBUG("rab_id\tcnxid\tSapi\t\tQoS\t\tState\n");
                list=(struct nas_msg_rb_list_reply *)(msgrep+1);
                for(rbi=0; rbi<num_rbs; ++rbi){
                  DEBUG("%u\t%u\t%u\t\t%u\t\t", list[rbi].rab_id,list[rbi].cnxid, list[rbi].sapi, list[rbi].qos);
                  print_state(list[rbi].state);
                  rc = 0;
                  // store channel status
                  for (i=0;i<num_rbs; i++){
                    if (g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[i].cnx_id == list[rbi].cnxid){
                      g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[i].nas_state = list[rbi].state;
                      if (list[rbi].state == NAS_CX_DCH)
                        g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[i].status = RB_CONNECTED;
                    }
                  }
                  if ((mt_ix==0)&&(i==num_rbs)){
                    if (g_enb_ral_obj[instanceP].mcast.radio_channel.cnx_id == list[rbi].cnxid){
                      g_enb_ral_obj[instanceP].mcast.radio_channel.nas_state = list[rbi].state;
                      if (list[rbi].state == NAS_CX_DCH){
                        g_enb_ral_obj[instanceP].mcast.radio_channel.status = RB_CONNECTED;
                        g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[1].status = RB_CONNECTED;
                      }
                    }
                  }
                }
                DEBUG("List complete \n");
               }
            break;
            default:
            ERR ("RAL_process_NAS_message : invalid ioctl command %d\n",ioctl_cmd);
            rc= -1;
         } //end switch ioctl_cmd 
      break;
      case IO_OBJ_MEAS:
         switch (ioctl_cmd){
            case IO_CMD_ADD:
              {
                struct nas_msg_enb_measure_trigger *msgreq;
                struct nas_msg_enb_measure_trigger_reply *msgrep;
                //
                gifr.type=NAS_MSG_ENB_MEAS_TRIGGER;
                memset (g_enb_ral_obj[instanceP].buffer,0,800);
                gifr.msg= &(g_enb_ral_obj[instanceP].buffer[0]);
                msgreq=(struct nas_msg_enb_measure_trigger *)(gifr.msg);
                msgrep=(struct nas_msg_enb_measure_trigger_reply *)(gifr.msg);
                //
                msgreq->cell_id = g_enb_ral_obj[instanceP].cell_id;
                //
                DEBUG("eNB measures triggered, cell_id %d\n", msgreq->cell_id);
                err=ioctl(fd, NASRG_IOCTL_RAL, &gifr);
                if (err<0){
                  ERR("IOCTL error, err=%d\n",err);
                  rc = -1;
                }
                //  check answer from NAS
                if (msgrep->status != 0){
                  ERR(" eNB measures trigger failure: %d\n",msgrep->status);
                  rc = -1;
                }else{
                  DEBUG(" eNB measures triggered successfully \n");
                  rc = 0;
                }
              }
            break;
            case IO_CMD_LIST:
              {
                struct nas_msg_enb_measure_retrieve *msgrep;
				int ix;
                //
                gifr.type=NAS_MSG_ENB_MEAS_RETRIEVE;
                memset (g_enb_ral_obj[instanceP].buffer,0,800);
                gifr.msg= &(g_enb_ral_obj[instanceP].buffer[0]);
                msgrep=(struct nas_msg_enb_measure_retrieve *)(gifr.msg);
                //
                DEBUG("Retrieving measure from NAS\n");
                err=ioctl(fd, NASRG_IOCTL_RAL, &gifr);
                if (err<0){
                  ERR("IOCTL error, err=%d\n",err);
                  rc = -1;
                }
                // Store the values received
                g_enb_ral_obj[instanceP].num_UEs = msgrep->num_UEs;
                for (ix=0; ix<g_enb_ral_obj[instanceP].num_UEs; ix++){
                   g_enb_ral_obj[instanceP].rlcBufferOccupancy[ix] = msgrep->measures[ix].rlcBufferOccupancy;
                   g_enb_ral_obj[instanceP].scheduledPRB[ix] = msgrep->measures[ix].scheduledPRB;
                   g_enb_ral_obj[instanceP].totalDataVolume[ix] = msgrep->measures[ix].totalDataVolume;
                }
                g_enb_ral_obj[instanceP].totalNumPRBs = msgrep->totalNumPRBs;
                #ifdef DEBUG_RAL_DETAILS
                DEBUG("Measures received- cell %d, Number of UEs %d, Total number of PRBs %d\n", msgrep->cell_id, msgrep->num_UEs, msgrep->totalNumPRBs);
                for (ix=0; ix<g_enb_ral_obj[instanceP].num_UEs; ix++)
                  DEBUG("UE%d : RLC Buffers %d, scheduledPRB %d, totalDataVolume %d\n", ix, msgrep->measures[ix].rlcBufferOccupancy,
                        msgrep->measures[ix].scheduledPRB, msgrep->measures[ix].totalDataVolume);
                #endif
                RAL_NAS_measures_analyze();
              }
            break;
            default:
            ERR ("RAL_process_NAS_message : invalid ioctl command %d\n",ioctl_cmd);
            rc= -1;
         } //end switch ioctl_cmd 
      break;

      case IO_OBJ_MC:
         switch (ioctl_cmd){
            case IO_CMD_ADD:
              {
                struct nas_msg_mt_mcast_join *msgreq;
                struct nas_msg_mt_mcast_reply *msgrep;
                struct ral_lte_channel *currChannel;
                //
                gifr.type=NAS_RG_MSG_MT_MCAST_JOIN;
                memset (g_enb_ral_obj[instanceP].buffer,0,800);
                gifr.msg= &(g_enb_ral_obj[instanceP].buffer[0]);
                msgreq=(struct nas_msg_mt_mcast_join *)(gifr.msg);
                msgrep=(struct nas_msg_mt_mcast_reply *)(gifr.msg);
                //
                currChannel = &(g_enb_ral_obj[instanceP].mcast.radio_channel);
                msgreq->ue_id = mt_ix;
                msgreq->rab_id = currChannel->rbId;
                msgreq->cnxid = currChannel->cnx_id;
                //
                DEBUG("UE multicast join notification requested, ue_id %d\n", mt_ix);
                err=ioctl(fd, NASRG_IOCTL_RAL, &gifr);
                if (err<0){
                  ERR("IOCTL error, err=%d\n",err);
                  rc = -1;
                }
                //  check answer from NAS
                if ((msgrep->result<0)||(msgrep->ue_id!=mt_ix)||(err<0)){
                  ERR(" UE multicast join notification failure: %d\n",msgrep->result);
                  g_enb_ral_obj[instanceP].pending_req_status = MIH_C_STATUS_REJECTED;
                  rc = -1;
                }else{
                  DEBUG(" ++ UE multicast join notification transmitted to MT \n");
                  g_enb_ral_obj[instanceP].pending_req_status = MIH_C_STATUS_SUCCESS;
                  rc = 0;
                }
                // TODO aRALu_send_link_mc_join_cnf();
              }
            break;

             case IO_CMD_DEL:
              {
                struct nas_msg_mt_mcast_leave *msgreq;
                struct nas_msg_mt_mcast_reply *msgrep;
                struct ral_lte_channel *currChannel;
                //
                gifr.type=NAS_RG_MSG_MT_MCAST_LEAVE;
                memset (g_enb_ral_obj[instanceP].buffer,0,800);
                gifr.msg= &(g_enb_ral_obj[instanceP].buffer[0]);
                msgreq=(struct nas_msg_mt_mcast_leave *)(gifr.msg);
                msgrep=(struct nas_msg_mt_mcast_reply *)(gifr.msg);
                //
                currChannel = &(g_enb_ral_obj[instanceP].mcast.radio_channel);
                msgreq->ue_id = mt_ix;
                msgreq->rab_id = currChannel->rbId;
                msgreq->cnxid = currChannel->cnx_id;
                //
                DEBUG("UE multicast leave notification requested, ue_id %d\n", mt_ix);
                err=ioctl(fd, NASRG_IOCTL_RAL, &gifr);
                if (err<0){
                  ERR("IOCTL error, err=%d\n",err);
                  rc = -1;
                }
                //  check answer from NAS
                if ((msgrep->result<0)||(msgrep->ue_id!=mt_ix)||(err<0)){
                  ERR(" UE multicast leave notification failure: %d\n",msgrep->result);
                  g_enb_ral_obj[instanceP].pending_req_status = MIH_C_STATUS_REJECTED;
                  rc = -1;
                }else{
                  DEBUG(" ++ UE multicast leave notification transmitted to MT \n" );
                  g_enb_ral_obj[instanceP].pending_req_status = MIH_C_STATUS_SUCCESS;
                  rc = 0;
                }
                // TODO aRALu_send_link_mc_leave_cnf();
              }
            break;
            default:
            ERR ("RAL_process_NAS_message : invalid ioctl command %d\n",ioctl_cmd);
            rc= -1;
         } //end switch ioctl_cmd 
      break;
      default:
        ERR ("RAL_process_NAS_message : invalid ioctl object %d\n",ioctl_obj);
        rc= -1;
  } //end switch ioctl_obj 
  //rc=0;
 return rc;
}
*/



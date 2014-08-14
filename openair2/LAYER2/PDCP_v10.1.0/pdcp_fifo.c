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

/*! \file pdcp_fifo.c
 * \brief pdcp interface with linux IP interface, have a look at http://man7.org/linux/man-pages/man7/netlink.7.html for netlink
 * \author  Lionel GAUTHIER and Navid Nikaein
 * \date 2009
 * \version 0.5
 * \warning This component can be runned only in user-space
 * @ingroup pdcp
 */

#define PDCP_FIFO_C
#define PDCP_DEBUG 1
//#define IDROMEL_NEMO 1

#ifndef OAI_EMU
extern int otg_enabled;
#endif

#include "pdcp.h"
#include "pdcp_primitives.h"

#ifdef USER_MODE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define rtf_put write
#define rtf_get read
#else
#include <rtai_fifos.h>
#endif //USER_MODE

#include "../MAC/extern.h"
#include "RRC/L2_INTERFACE/openair_rrc_L2_interface.h"
#include "NAS/DRIVER/LITE/constant.h"
#include "SIMULATION/ETH_TRANSPORT/extern.h"
#include "UTIL/OCG/OCG.h"
#include "UTIL/OCG/OCG_extern.h"
#include "UTIL/LOG/log.h"
#include "UTIL/OTG/otg_tx.h"
#include "UTIL/FIFO/pad_list.h"
#include "platform_constants.h"

#include "assertions.h"

#ifdef NAS_NETLINK
#include <sys/socket.h>
#include <linux/netlink.h>

extern char nl_rx_buf[NL_MAX_PAYLOAD];
extern struct sockaddr_nl nas_src_addr, nas_dest_addr;
extern struct nlmsghdr *nas_nlh_tx;
extern struct nlmsghdr *nas_nlh_rx;
extern struct iovec nas_iov_tx;
extern struct iovec nas_iov_rx;
extern int nas_sock_fd;
extern struct msghdr nas_msg_tx;
extern struct msghdr nas_msg_rx;

#define MAX_PAYLOAD 1600

unsigned char pdcp_read_state_g = 0;
//unsigned char pdcp_read_payload[MAX_PAYLOAD];
#endif

extern Packet_OTG_List_t *otg_pdcp_buffer;

#if defined(LINK_PDCP_TO_GTPV1U)
#  include "gtpv1u_eNB_task.h"
#endif

pdcp_data_req_header_t pdcp_read_header_g;

//-----------------------------------------------------------------------------
int pdcp_fifo_flush_sdus(frame_t frameP, eNB_flag_t enb_flagP, module_id_t enb_mod_idP, module_id_t ue_mod_idP)
{
  //-----------------------------------------------------------------------------

  mem_block_t     *sdu_p            = list_get_head (&pdcp_sdu_list);
  int              bytes_wrote      = 0;
  int              pdcp_nb_sdu_sent = 0;
  uint8_t          cont             = 1;
#if defined(LINK_PDCP_TO_GTPV1U)
  //MessageDef      *message_p        = NULL;
#endif

#if defined(NAS_NETLINK) && defined(LINUX)
  int ret = 0;
#endif

  while (sdu_p && cont) {

#if ! defined(OAI_EMU)
      ((pdcp_data_ind_header_t *)(sdu_p->data))->inst = 0;
#endif

#if defined(LINK_PDCP_TO_GTPV1U)
      if (enb_flagP) {
          LOG_I(PDCP,"Sending to GTPV1U\n");
          /*message_p = itti_alloc_new_message(TASK_PDCP_ENB, GTPV1U_TUNNEL_DATA_REQ);
          GTPV1U_TUNNEL_DATA_REQ(message_p).buffer       = &(((uint8_t *) sdu_p->data)[sizeof (pdcp_data_ind_header_t)]);
          GTPV1U_TUNNEL_DATA_REQ(message_p).length       = ((pdcp_data_ind_header_t *)(sdu_p->data))->data_size;
          GTPV1U_TUNNEL_DATA_REQ(message_p).ue_module_id = ue_mod_idP;
          GTPV1U_TUNNEL_DATA_REQ(message_p).rab_id;      = ((pdcp_data_ind_header_t *)(sdu_p->data))->rb_id;
          */
          gtpv1u_new_data_req(
              enb_mod_idP, //gtpv1u_data_t *gtpv1u_data_p,
              ue_mod_idP,//rb_id/maxDRB, TO DO UE ID
              ((pdcp_data_ind_header_t *)(sdu_p->data))->rb_id + 4,
              &(((uint8_t *) sdu_p->data)[sizeof (pdcp_data_ind_header_t)]),
              ((pdcp_data_ind_header_t *)(sdu_p->data))->data_size);

          list_remove_head (&pdcp_sdu_list);
          free_mem_block (sdu_p);
          cont = 1;
          pdcp_nb_sdu_sent += 1;
          sdu_p = list_get_head (&pdcp_sdu_list);
          LOG_I(OTG,"After  GTPV1U\n");
          continue; // loop again
       }
#endif /* defined(ENABLE_USE_MME) */
#ifdef PDCP_DEBUG
      LOG_I(PDCP, "PDCP->IP TTI %d INST %d: Preparing %d Bytes of data from rab %d to Nas_mesh\n",
          frameP, ((pdcp_data_ind_header_t *)(sdu_p->data))->inst,
          ((pdcp_data_ind_header_t *)(sdu_p->data))->data_size, ((pdcp_data_ind_header_t *)(sdu_p->data))->rb_id);
#endif //PDCP_DEBUG
      cont = 0;

      if (!pdcp_output_sdu_bytes_to_write) {
          if (!pdcp_output_header_bytes_to_write) {
              pdcp_output_header_bytes_to_write = sizeof (pdcp_data_ind_header_t);
          }

#ifdef NAS_FIFO
          bytes_wrote = rtf_put (PDCP2NAS_FIFO,
              &(((uint8_t *) sdu->data)[sizeof (pdcp_data_ind_header_t) - pdcp_output_header_bytes_to_write]),
              pdcp_output_header_bytes_to_write);

#else
#ifdef NAS_NETLINK
#ifdef LINUX
          memcpy(NLMSG_DATA(nas_nlh_tx), &(((uint8_t *) sdu_p->data)[sizeof (pdcp_data_ind_header_t) - pdcp_output_header_bytes_to_write]),
              pdcp_output_header_bytes_to_write);
          nas_nlh_tx->nlmsg_len = pdcp_output_header_bytes_to_write;
#endif //LINUX
#endif //NAS_NETLINK

          bytes_wrote = pdcp_output_header_bytes_to_write;
#endif //NAS_FIFO

#ifdef PDCP_DEBUG
          LOG_I(PDCP, "Frame %d Sent %d Bytes of header to Nas_mesh\n",
              frameP,
              bytes_wrote);
#endif //PDCP_DEBUG

          if (bytes_wrote > 0) {
              pdcp_output_header_bytes_to_write = pdcp_output_header_bytes_to_write - bytes_wrote;

              if (!pdcp_output_header_bytes_to_write) { // continue with sdu
                  pdcp_output_sdu_bytes_to_write = ((pdcp_data_ind_header_t *) sdu_p->data)->data_size;

#ifdef NAS_FIFO
                  bytes_wrote = rtf_put (PDCP2NAS_FIFO, &(sdu->data[sizeof (pdcp_data_ind_header_t)]), pdcp_output_sdu_bytes_to_write);
#else

#ifdef NAS_NETLINK
#ifdef LINUX
                  memcpy(NLMSG_DATA(nas_nlh_tx)+sizeof(pdcp_data_ind_header_t), &(sdu_p->data[sizeof (pdcp_data_ind_header_t)]), pdcp_output_sdu_bytes_to_write);
                  nas_nlh_tx->nlmsg_len += pdcp_output_sdu_bytes_to_write;
                  ret = sendmsg(nas_sock_fd,&nas_msg_tx,0);
                  if (ret<0) {
                      LOG_D(PDCP, "[PDCP_FIFOS] sendmsg returns %d (errno: %d)\n", ret, errno);
                      mac_xface->macphy_exit("sendmsg failed for nas_sock_fd\n");
                      break;
                  }
#endif // LINUX
#endif //NAS_NETLINK
                  bytes_wrote= pdcp_output_sdu_bytes_to_write;
#endif // NAS_FIFO

#ifdef PDCP_DEBUG
                  LOG_I(PDCP, "PDCP->IP Frame %d INST %d: Sent %d Bytes of data from rab %d to Nas_mesh\n",
                      frameP,
                      ((pdcp_data_ind_header_t *)(sdu_p->data))->inst,
                      bytes_wrote,
                      ((pdcp_data_ind_header_t *)(sdu_p->data))->rb_id);
#endif //PDCP_DEBUG
                  if (bytes_wrote > 0) {
                      pdcp_output_sdu_bytes_to_write -= bytes_wrote;

                      if (!pdcp_output_sdu_bytes_to_write) { // OK finish with this SDU
                          // LOG_D(PDCP, "rb sent a sdu qos_sap %d\n", sapiP);
                          LOG_D(PDCP,
                              "[FRAME %05d][xxx][PDCP][MOD xx/xx][RB %u][--- PDCP_DATA_IND / %d Bytes --->][IP][INSTANCE %u][RB %u]\n",
                              frameP,
                              ((pdcp_data_ind_header_t *)(sdu_p->data))->rb_id,
                              ((pdcp_data_ind_header_t *)(sdu_p->data))->data_size,
                              ((pdcp_data_ind_header_t *)(sdu_p->data))->inst,
                              ((pdcp_data_ind_header_t *)(sdu_p->data))->rb_id);

                          list_remove_head (&pdcp_sdu_list);
                          free_mem_block (sdu_p);
                          cont = 1;
                          pdcp_nb_sdu_sent += 1;
                          sdu_p = list_get_head (&pdcp_sdu_list);
                      }
                  } else {
                      LOG_W(PDCP, "RADIO->IP SEND SDU CONGESTION!\n");
                  }
              } else {
                  LOG_W(PDCP, "RADIO->IP SEND SDU CONGESTION!\n");
              }
          }
      } else {
          // continue writing sdu
#ifdef NAS_FIFO
          bytes_wrote = rtf_put (PDCP2NAS_FIFO,
              (uint8_t *) (&(sdu_p->data[sizeof (pdcp_data_ind_header_t) + ((pdcp_data_ind_header_t *) sdu_p->data)->data_size - pdcp_output_sdu_bytes_to_write])),
              pdcp_output_sdu_bytes_to_write);
#else  // NAS_FIFO
          bytes_wrote = pdcp_output_sdu_bytes_to_write;
#endif  // NAS_FIFO

          if (bytes_wrote > 0) {
              pdcp_output_sdu_bytes_to_write -= bytes_wrote;

              if (!pdcp_output_sdu_bytes_to_write) {     // OK finish with this SDU
                  //PRINT_RB_SEND_OUTPUT_SDU ("[PDCP] RADIO->IP SEND SDU\n");
                  list_remove_head (&pdcp_sdu_list);
                  free_mem_block (sdu_p);
                  cont = 1;
                  pdcp_nb_sdu_sent += 1;
                  sdu_p = list_get_head (&pdcp_sdu_list);
                  // LOG_D(PDCP, "rb sent a sdu from rab\n");
              }
          }
      }
  }
#ifdef NAS_FIFO
  if ((pdcp_nb_sdu_sent)) {
      if ((pdcp_2_nas_irq > 0)) {
#ifdef PDCP_DEBUG
          LOG_I(PDCP, "Frame %d : Trigger NAS RX interrupt\n",
              frameP);
#endif //PDCP_DEBUG

          rt_pend_linux_srq (pdcp_2_nas_irq);
      } else {
          LOG_E(PDCP, "Frame %d: ERROR IF IP STACK WANTED : NOTIF PACKET(S) pdcp_2_nas_irq not initialized : %d\n",
              frameP,
              pdcp_2_nas_irq);
      }
  }
#endif  //NAS_FIFO

  return pdcp_nb_sdu_sent;
}

//-----------------------------------------------------------------------------
/*
 * returns a positive value if whole bytes that had to be read were read
 * returns zero  value if whole bytes that had to be read were not read at all
 * returns a negative  value if an error was encountered while reading the rt fifo
 */
int pdcp_fifo_read_input_sdus_remaining_bytes (frame_t frameP, eNB_flag_t enb_flagP)
{
  //-----------------------------------------------------------------------------
  sdu_size_t             bytes_read = 0;
  rb_id_t                rab_id     = 0;
  pdcp_t                *pdcp_p     = NULL;
  module_id_t            ue_inst    = 0;
  module_id_t            enb_inst   = 0;
  rb_id_t                rb_id      = 0;
  int                    result     = -1;

  // if remaining bytes to read
  if (pdcp_input_sdu_remaining_size_to_read > 0) {
      bytes_read = rtf_get (NAS2PDCP_FIFO,
          &(pdcp_input_sdu_buffer[pdcp_input_sdu_size_read]),
          pdcp_input_sdu_remaining_size_to_read);

      if (bytes_read > 0) {
          LOG_D(PDCP, "[PDCP_FIFOS] Read %d remaining bytes of data from Nas_mesh\n", bytes_read);

          pdcp_input_sdu_remaining_size_to_read = pdcp_input_sdu_remaining_size_to_read - bytes_read;
          pdcp_input_sdu_size_read = pdcp_input_sdu_size_read + bytes_read;

          if (pdcp_input_sdu_remaining_size_to_read != 0) {
              return 0;
          } else {
#ifdef PDCP_DEBUG
              LOG_I(PDCP, "Frame %d: IP->RADIO RECEIVED COMPLETE SDU size %d inst %d rb %d\n",
                  frameP,
                  pdcp_input_sdu_size_read,
                  pdcp_input_header.inst,
                  pdcp_input_header.rb_id);
#endif //PDCP_DEBUG
              pdcp_input_sdu_size_read = 0;
#ifdef IDROMEL_NEMO
              pdcp_read_header_g.inst = 0;
#endif

              if (enb_flagP == 0) {
                  ue_inst  = pdcp_read_header_g.inst;
                  rb_id    = pdcp_read_header_g.rb_id;
                  enb_inst = 0;
                  pdcp_p   = &pdcp_array_drb_ue[ue_inst][rb_id-1];
              } else {
                  ue_inst  = pdcp_read_header_g.rb_id / maxDRB;
                  rb_id    = pdcp_read_header_g.rb_id % maxDRB;
                  enb_inst = pdcp_read_header_g.inst;
                  pdcp_p   = &pdcp_array_drb_eNB[enb_inst][ue_inst][rb_id-1];
              }
              AssertFatal (enb_inst < NB_eNB_INST, "eNB module id is too high (%u/%d)!\n",       enb_inst, NB_eNB_INST);
              AssertFatal (ue_inst  >= NB_eNB_INST,
                           "UE module id is too low (%u/%d)!\n",
                           ue_inst,
                           NB_eNB_INST);
              AssertFatal (ue_inst  < (NB_eNB_INST + NB_UE_INST),
                           "UE module id is too high (%u/%d)!\n",
                           ue_inst,
                           NB_eNB_INST + NB_UE_INST);
              AssertFatal (rb_id    < maxDRB,                       "RB id is too high (%u/%d)!\n", rab_id, maxDRB);
              AssertFatal (rb_id    > 0     ,                       "RB id is too low (%u/%d)!\n", rab_id, maxDRB);

              if (pdcp_input_header.rb_id != 0) {
                  LOG_D(PDCP, "[FRAME %5u][%s][IP][INSTANCE %u][RB %u][--- PDCP_DATA_REQ / %d Bytes --->][PDCP][MOD %u/%u][RB %u]\n",
                      frameP,
                      (enb_flagP) ? "eNB" : "UE",
                      pdcp_read_header_g.inst,
                      pdcp_read_header_g.rb_id,
                      pdcp_read_header_g.data_size,
                      enb_inst,
                      ue_inst,
                      rb_id);

                  if (pdcp_p->instanciated_instance) {
                      result = pdcp_data_req (
                              enb_inst,
                              ue_inst,
                              frameP,
                              enb_flagP,
                              SRB_FLAG_NO,
                              rb_id % maxDRB,
                              RLC_MUI_UNDEFINED,
                              RLC_SDU_CONFIRM_NO,
                              pdcp_input_header.data_size,
                              pdcp_input_sdu_buffer,
                              PDCP_TRANSMISSION_MODE_DATA);
                      AssertFatal (result == TRUE, "PDCP data request failed!\n");
                  }

              } else if ((pdcp_input_header.traffic_type == TRAFFIC_IPV6_TYPE_MULTICAST) || (pdcp_input_header.traffic_type == TRAFFIC_IPV4_TYPE_MULTICAST)) {
                  LOG_D(PDCP, "[FRAME %5u][%s][IP][INSTANCE %u][RB %u][--- PDCP_DATA_REQ on MBMS bearer/ %d Bytes --->][PDCP][MOD %u/%u][RB %u]\n",
                      frameP,
                      (enb_flagP) ? "eNB" : "UE",
                          pdcp_read_header_g.inst,
                          pdcp_read_header_g.rb_id,
                          pdcp_read_header_g.data_size,
                          enb_inst,
                          ue_inst,
                          rb_id);

                  if (pdcp_p->instanciated_instance) {
                      result = pdcp_data_req (
                          enb_inst,
                          ue_inst,
                          frameP,
                          enb_flagP,
                          SRB_FLAG_NO,
                          rb_id,
                          RLC_MUI_UNDEFINED,
                          RLC_SDU_CONFIRM_NO,
                          pdcp_input_header.data_size,
                          pdcp_input_sdu_buffer,
                          PDCP_TRANSMISSION_MODE_TRANSPARENT);
                      AssertFatal (result == TRUE, "PDCP data request failed!\n");
                  }

              } else if (enb_flagP) {
                  // is a broadcast packet, we have to send this packet on all default RABS of all connected UEs
                  LOG_D(PDCP, "Checking if could sent on default rabs\n");
                  for (ue_inst = 0; ue_inst < NUMBER_OF_UE_MAX; ue_inst++) {
                      LOG_D(PDCP, "Checking if could sent on default rab id %d\n", DEFAULT_RAB_ID);
                      pdcp_p = &pdcp_array_drb_eNB[enb_inst][ue_inst][DEFAULT_RAB_ID-1];
                      if (pdcp_p->instanciated_instance) {
                          LOG_D(PDCP, "[FRAME %5u][%s][IP][INSTANCE %u][RB %u][--- PDCP_DATA_REQ / %d Bytes --->][PDCP][MOD %u/%u][RB DEFAULT_RAB_ID %u]\n",
                              frameP,
                              (enb_flagP) ? "eNB" : "UE",
                              pdcp_read_header_g.inst,
                              pdcp_read_header_g.rb_id,
                              pdcp_read_header_g.data_size,
                              enb_inst,
                              ue_inst,
                              DEFAULT_RAB_ID);
                          result = pdcp_data_req (
                              enb_inst,
                              ue_inst,
                              frameP,
                              enb_flagP,
                              SRB_FLAG_NO,
                              DEFAULT_RAB_ID,
                              RLC_MUI_UNDEFINED,
                              RLC_SDU_CONFIRM_NO,
                              pdcp_input_header.data_size,
                              pdcp_input_sdu_buffer,
                              PDCP_TRANSMISSION_MODE_DATA);
                          AssertFatal (result == TRUE, "PDCP data request failed!\n");
                      }
                  }
              } else {
                  LOG_D(PDCP, "Forcing send on DEFAULT_RAB_ID\n");
                  LOG_D(PDCP, "[FRAME %5u][%s][IP][INSTANCE %u][RB %u][--- PDCP_DATA_REQ / %d Bytes --->][PDCP][MOD %u/%u][RB DEFAULT_RAB_ID %u]\n",
                      frameP,
                      (enb_flagP) ? "eNB" : "UE",
                      pdcp_read_header_g.inst,
                      pdcp_read_header_g.rb_id,
                      pdcp_read_header_g.data_size,
                      enb_inst,
                      ue_inst,
                      DEFAULT_RAB_ID);
                  result = pdcp_data_req (
                      enb_inst,
                      ue_inst,
                      frameP,
                      enb_flagP,
                      SRB_FLAG_NO,
                      DEFAULT_RAB_ID,
                      RLC_MUI_UNDEFINED,
                      RLC_SDU_CONFIRM_NO,
                      pdcp_input_header.data_size,
                      pdcp_input_sdu_buffer,
                      PDCP_TRANSMISSION_MODE_DATA);
                  AssertFatal (result == TRUE, "PDCP data request failed!\n");
              }
              // not necessary
              //memset(pdcp_input_sdu_buffer, 0, MAX_IP_PACKET_SIZE);
              return 1;
          }
      } else {
          return bytes_read;
      }
  }
  return 1;
}

//-----------------------------------------------------------------------------
int pdcp_fifo_read_input_sdus (frame_t frameP, eNB_flag_t enb_flagP, module_id_t ue_mod_idP, module_id_t enb_mod_idP)
{
#ifdef NAS_NETLINK
# if defined(ENABLE_PDCP_NETLINK_FIFO)
  module_id_t                    ue_id     = 0;
  rb_id_t                        rab_id    = 0;
  pdcp_t                        *pdcp      = NULL;
  pdcp_transmission_mode_t       pdcp_mode = PDCP_TRANSMISSION_MODE_UNKNOWN;
  struct pdcp_netlink_element_s *data      = NULL;

  while (pdcp_netlink_dequeue_element(enb_mod_idP, ue_mod_idP, enb_flagP, &data) != 0) {
      DevAssert(data != NULL);
      if (enb_flagP == 0) {
          rab_id = data->pdcp_read_header.rb_id % maxDRB;
          pdcp = &pdcp_array_drb_ue[ue_mod_idP][rab_id-1];
      } else {
          rab_id = data->pdcp_read_header.rb_id % maxDRB;
          ue_mod_idP = data->pdcp_read_header.rb_id / maxDRB;
          pdcp = &pdcp_array_drb_eNB[enb_mod_idP][ue_mod_idP][rab_id-1];
      }
      if (enb_flagP) {
          AssertFatal ((enb_mod_idP >= oai_emulation.info.first_enb_local) && (oai_emulation.info.nb_enb_local > 0),
              "eNB module id is too low (%u/%d)!\n",
              enb_mod_idP,
              oai_emulation.info.first_enb_local);
          AssertFatal ((enb_mod_idP < (oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local)) && (oai_emulation.info.nb_enb_local > 0),
              "eNB module id is too high (%u/%d)!\n",
              enb_mod_idP,
              oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local);
          AssertFatal (ue_mod_idP  < NB_UE_INST,
              "UE module id is too high (%u/%d)!\n",
              ue_mod_idP,
              oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local);
      } else {
          AssertFatal (ue_mod_idP  < (oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local),
              "UE module id is too high (%u/%d)!\n",
              ue_mod_idP,
              oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local);
          AssertFatal (ue_mod_idP  >= oai_emulation.info.first_ue_local,
              "UE module id is too low (%u/%d)!\n",
              ue_mod_idP,
              oai_emulation.info.first_ue_local);
      }
      AssertFatal (rab_id    < maxDRB,                       "RB id is too high (%u/%d)!\n", rab_id, maxDRB);

      if (rab_id != 0) {
          if (pdcp->instanciated_instance) {
              LOG_D(PDCP, "[FRAME %05d][%s][IP][INSTANCE %u][RB %u][--- PDCP_DATA_REQ "
                  "/ %d Bytes --->][PDCP][MOD %u/%u][RB %u]\n",
                  frameP,
                  (enb_flagP) ? "eNB" : "UE",
                      data->pdcp_read_header.inst,
                      data->pdcp_read_header.rb_id,
                      data->pdcp_read_header.data_size,
                      enb_mod_idP,
                      ue_mod_idP,
                      rab_id);
#ifdef 	OAI_NW_DRIVER_TYPE_ETHERNET
              if ((data->pdcp_read_header.traffic_type == TRAFFIC_IPV6_TYPE_MULTICAST) /*TRAFFIC_IPV6_TYPE_MULTICAST */ ||
                  (data->pdcp_read_header.traffic_type == TRAFFIC_IPV4_TYPE_MULTICAST) /*TRAFFIC_IPV4_TYPE_MULTICAST */ ||
                  (data->pdcp_read_header.traffic_type == TRAFFIC_IPV4_TYPE_BROADCAST) /*TRAFFIC_IPV4_TYPE_BROADCAST */ ) {
#if defined (Rel10)
                PDCP_TRANSMISSION_MODE_TRANSPARENT;
#else
                pdcp_mode= PDCP_TRANSMISSION_MODE_DATA;
#endif
              } else if ((data->pdcp_read_header.traffic_type == TRAFFIC_IPV6_TYPE_UNICAST) /* TRAFFIC_IPV6_TYPE_UNICAST */ ||
                  (data->pdcp_read_header.traffic_type == TRAFFIC_IPV4_TYPE_UNICAST) /*TRAFFIC_IPV4_TYPE_UNICAST*/ ) {
                pdcp_mode=  PDCP_TRANSMISSION_MODE_DATA;
              } else {
                  pdcp_mode= PDCP_TRANSMISSION_MODE_DATA;
                  LOG_W(PDCP,"unknown IP traffic type \n");
              }
#else // NASMESH driver does not curreenlty support multicast traffic
              pdcp_mode = PDCP_TRANSMISSION_MODE_DATA;
#endif
              pdcp_data_req(enb_mod_idP,
                  ue_mod_idP,
                  frameP,
                  enb_flagP,
                  SRB_FLAG_NO,
                  rab_id % maxDRB,
                  RLC_MUI_UNDEFINED,
                  RLC_SDU_CONFIRM_NO,
                  data->pdcp_read_header.data_size,
                  data->data,
                  pdcp_mode);
          } else {
              LOG_E(PDCP, "Received packet for non-instanciated instance %u with rb_id %u, UE_index %d, enb_flagP %d eNB_index %d\n",
                  data->pdcp_read_header.inst, data->pdcp_read_header.rb_id, ue_mod_idP, enb_flagP,enb_mod_idP);
          }
      } else if (enb_flagP) {
          /* rb_id = 0, thus interpreated as broadcast and transported as
           * multiple unicast is a broadcast packet, we have to send this
           * packet on all default RABS of all connected UEs
           */
          LOG_D(PDCP, "eNB Try Forcing send on DEFAULT_RAB_ID first_ue_local %u nb_ue_local %u\n", oai_emulation.info.first_ue_local, oai_emulation.info.nb_ue_local);
          for (ue_id = 0; ue_id < NB_UE_INST; ue_id++) {
              pdcp = &pdcp_array_drb_eNB[enb_mod_idP][ue_id][DEFAULT_RAB_ID-1];
              if (pdcp->instanciated_instance) {
                  LOG_D(PDCP, "eNB Try Forcing send on DEFAULT_RAB_ID UE %d\n", ue_id);
                  pdcp_data_req(
                      enb_mod_idP,
                      ue_id,
                      frameP,
                      enb_flagP,
                      SRB_FLAG_NO,
                      DEFAULT_RAB_ID,
                      RLC_MUI_UNDEFINED,
                      RLC_SDU_CONFIRM_NO,
                      data->pdcp_read_header.data_size,
                      data->data,
                      PDCP_TRANSMISSION_MODE_DATA);
              }
          }
      } else {
          LOG_D(PDCP, "Forcing send on DEFAULT_RAB_ID\n");
          pdcp_data_req(
              enb_mod_idP,
              ue_mod_idP,
              frameP,
              enb_flagP,
              SRB_FLAG_NO,
              DEFAULT_RAB_ID,
              RLC_MUI_UNDEFINED,
              RLC_SDU_CONFIRM_NO,
              data->pdcp_read_header.data_size,
              data->data,
              PDCP_TRANSMISSION_MODE_DATA);
      }
      free(data->data);
      free(data);
      data = NULL;
  }
  return 0;
# else /* ENABLE_PDCP_NETLINK_FIFO*/
  int              len = 1;
  rb_id_t          rab_id  = 0;
  while (len > 0) {
      len = recvmsg(nas_sock_fd, &nas_msg_rx, 0);
      if (len<=0) {
          // nothing in pdcp NAS socket
          //LOG_I(PDCP, "[PDCP][NETLINK] Nothing in socket, length %d \n", len);
      } else {
          for (nas_nlh_rx = (struct nlmsghdr *) nl_rx_buf;
              NLMSG_OK (nas_nlh_rx, len);
              nas_nlh_rx = NLMSG_NEXT (nas_nlh_rx, len)) {

              if (nas_nlh_rx->nlmsg_type == NLMSG_DONE) {
                  LOG_I(PDCP, "[PDCP][FIFO] RX NLMSG_DONE\n");
                  //return;
              }

              if (nas_nlh_rx->nlmsg_type == NLMSG_ERROR) {
                  LOG_I(PDCP, "[PDCP][FIFO] RX NLMSG_ERROR\n");
              }
              if (pdcp_read_state_g == 0) {
                  if (nas_nlh_rx->nlmsg_len == sizeof (pdcp_data_req_header_t) + sizeof(struct nlmsghdr)) {
                      pdcp_read_state_g = 1;  //get
                      memcpy((void *)&pdcp_read_header_g, (void *)NLMSG_DATA(nas_nlh_rx), sizeof(pdcp_data_req_header_t));
                      LOG_I(PDCP, "[PDCP][FIFO] RX pdcp_data_req_header_t inst %u, rb_id %u data_size %d\n",
                          pdcp_read_header_g.inst, pdcp_read_header_g.rb_id, pdcp_read_header_g.data_size);
                  } else {
                      LOG_E(PDCP, "[PDCP][FIFO] WRONG size %d should be sizeof (pdcp_data_req_header_t) + sizeof(struct nlmsghdr)\n",
                          nas_nlh_rx->nlmsg_len);
                  }
              } else {
                  pdcp_read_state_g = 0;
                  // print_active_requests()
#ifdef PDCP_DEBUG
                  LOG_I(PDCP, "[PDCP][FIFO] Something in socket, length %d \n",
                      nas_nlh_rx->nlmsg_len - sizeof(struct nlmsghdr));
#endif
                  //memcpy(pdcp_read_payload, (unsigned char *)NLMSG_DATA(nas_nlh_rx), nas_nlh_rx->nlmsg_len - sizeof(struct nlmsghdr));

#ifdef OAI_EMU
                  // overwrite function input parameters, because only one netlink socket for all instances
                  if (pdcp_read_header_g.inst < oai_emulation.info.nb_enb_local) {
                      enb_flagP  = 1;
                      ue_mod_idP  = pdcp_read_header_g.rb_id / maxDRB + oai_emulation.info.first_ue_local;
                      enb_mod_idP = pdcp_read_header_g.inst  +  oai_emulation.info.first_enb_local;
                      rab_id    = pdcp_read_header_g.rb_id % maxDRB;
                  } else {
                      enb_flagP  = 0;
                      ue_mod_idP  = pdcp_read_header_g.inst - oai_emulation.info.nb_enb_local + oai_emulation.info.first_ue_local;
                      enb_mod_idP = 0;
                      rab_id    = pdcp_read_header_g.rb_id;
                  }
                  AssertFatal (enb_mod_idP >= oai_emulation.info.first_enb_local,
                              "eNB inst is too low (%u/%d)!\n",
                              enb_mod_idP,
                              oai_emulation.info.first_enb_local);
                  AssertFatal (enb_mod_idP < (oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local),
                              "eNB inst is too high (%u/%d)!\n",
                              enb_mod_idP,
                              oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local);
                  AssertFatal (ue_mod_idP  >= oai_emulation.info.first_ue_local,
                               "UE inst is too low (%u/%d)!\n",
                               ue_mod_idP,
                               oai_emulation.info.first_ue_local);
                  AssertFatal (ue_mod_idP  < (oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local),
                               "UE inst is too high (%u/%d)!\n",
                               ue_mod_idP,
                               oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local);
                  AssertFatal (rab_id    < maxDRB,                       "RB id is too high (%u/%d)!\n", rab_id, maxDRB);
                  /*LGpdcp_read_header.inst = (pdcp_read_header_g.inst >= oai_emulation.info.nb_enb_local) ? \
                          pdcp_read_header_g.inst - oai_emulation.info.nb_enb_local+ NB_eNB_INST + oai_emulation.info.first_ue_local :
                          pdcp_read_header_g.inst +  oai_emulation.info.first_enb_local;*/
#else
                  pdcp_read_header_g.inst = 0;
#warning "TO DO CORRCT VALUES FOR ue mod id, enb mod id"
                  if (enb_flagP) {
		    ue_mod_idP  = 0;
		    enb_mod_idP = 0;
		    rab_id      = pdcp_read_header_g.rb_id % maxDRB;
		  } else {
		    ue_mod_idP  = 0;
		    enb_mod_idP = 0;
		    rab_id      = pdcp_read_header_g.rb_id % maxDRB;
		  }
#endif

                  if (enb_flagP) {
                      if (rab_id != 0) {
                          rab_id = rab_id % maxDRB;
                          if (pdcp_array_drb_eNB[enb_mod_idP][ue_mod_idP][rab_id-1].instanciated_instance) {
#ifdef PDCP_DEBUG
                              LOG_I(PDCP, "[FRAME %5u][eNB][NETLINK][IP->PDCP] INST %d: Received socket with length %d (nlmsg_len = %d) on Rab %d \n",
                                  frameP,
                                  pdcp_read_header_g.inst,
                                  len,
                                  nas_nlh_rx->nlmsg_len-sizeof(struct nlmsghdr),
                                  pdcp_read_header_g.rb_id);
#endif

                              LOG_D(PDCP, "[FRAME %5u][eNB][IP][INSTANCE %u][RB %u][--- PDCP_DATA_REQ / %d Bytes --->][PDCP][MOD %u/%u][RB %u]\n",
                                  frameP,
                                  pdcp_read_header_g.inst,
                                  pdcp_read_header_g.rb_id,
                                  pdcp_read_header_g.data_size,
                                  enb_mod_idP,
                                  ue_mod_idP,
                                  rab_id);

                              pdcp_data_req(enb_mod_idP,ue_mod_idP,frameP, enb_flagP,SRB_FLAG_NO,
                                  rab_id,
                                  RLC_MUI_UNDEFINED,
                                  RLC_SDU_CONFIRM_NO,
                                  pdcp_read_header_g.data_size,
                                  (unsigned char *)NLMSG_DATA(nas_nlh_rx),
                                  PDCP_TRANSMISSION_MODE_DATA);
                          } else {
                              LOG_D(PDCP, "[FRAME %5u][eNB][IP][INSTANCE %u][RB %u][--- PDCP_DATA_REQ / %d Bytes ---X][PDCP][MOD %u/%u][RB %u] NON INSTANCIATED INSTANCE, DROPPED\n",
                                  frameP,
                                  pdcp_read_header_g.inst,
                                  pdcp_read_header_g.rb_id,
                                  pdcp_read_header_g.data_size,
                                  enb_mod_idP,
                                  ue_mod_idP,
                                  rab_id);
                          }
                      } else  { // rb_id =0, thus interpreated as broadcast and transported as multiple unicast
                          // is a broadcast packet, we have to send this packet on all default RABS of all connected UEs
#warning CODE TO BE REVIEWED, ONLY WORK FOR SIMPLE TOPOLOGY CASES
                          for (ue_mod_idP = 0; ue_mod_idP < NB_UE_INST; ue_mod_idP++) {
                              if (pdcp_array_drb_eNB[enb_mod_idP][ue_mod_idP][rab_id-1].instanciated_instance == TRUE) {
                                  LOG_D(PDCP, "[FRAME %5u][eNB][IP][INSTANCE %u][RB %u][--- PDCP_DATA_REQ / %d Bytes --->][PDCP][MOD %u/%u][RB DEFAULT_RAB_ID %u]\n",
                                      frameP,
                                      pdcp_read_header_g.inst,
                                      pdcp_read_header_g.rb_id,
                                      pdcp_read_header_g.data_size,
                                      enb_mod_idP,
                                      ue_mod_idP,
                                      DEFAULT_RAB_ID);
                                  pdcp_data_req (
                                      enb_mod_idP,
                                      ue_mod_idP,
                                      frameP,
                                      enb_flagP,
                                      SRB_FLAG_NO,
                                      DEFAULT_RAB_ID,
                                      RLC_MUI_UNDEFINED,
                                      RLC_SDU_CONFIRM_NO,
                                      pdcp_read_header_g.data_size,
                                      (unsigned char *)NLMSG_DATA(nas_nlh_rx),
                                      PDCP_TRANSMISSION_MODE_DATA);
                              }
                          }
                      }
                  } else {
                      if (rab_id != 0) {
                          if (pdcp_array_drb_ue[ue_mod_idP][rab_id-1].instanciated_instance) {
#ifdef PDCP_DEBUG
                              LOG_I(PDCP, "[FRAME %5u][UE][NETLINK][IP->PDCP] INST %d: Received socket with length %d (nlmsg_len = %d) on Rab %d \n",
                                  frameP,
                                  pdcp_read_header_g.inst,
                                  len,
                                  nas_nlh_rx->nlmsg_len-sizeof(struct nlmsghdr),
                                  pdcp_read_header_g.rb_id);

                              LOG_D(PDCP, "[FRAME %5u][UE][IP][INSTANCE %u][RB %u][--- PDCP_DATA_REQ / %d Bytes --->][PDCP][MOD %u/%u][RB %u]\n",
                                  frameP,
                                  pdcp_read_header_g.inst,
                                  pdcp_read_header_g.rb_id,
                                  pdcp_read_header_g.data_size,
                                  enb_mod_idP,
                                  ue_mod_idP,
                                  rab_id);
#endif

                              pdcp_data_req(
                                  enb_mod_idP,
                                  ue_mod_idP,
                                  frameP,
                                  enb_flagP,
                                  SRB_FLAG_NO,
                                  rab_id,
                                  RLC_MUI_UNDEFINED,
                                  RLC_SDU_CONFIRM_NO,
                                  pdcp_read_header_g.data_size,
                                  (unsigned char *)NLMSG_DATA(nas_nlh_rx),
                                  PDCP_TRANSMISSION_MODE_DATA);
                          } else {
                              LOG_D(PDCP, "[FRAME %5u][UE][IP][INSTANCE %u][RB %u][--- PDCP_DATA_REQ / %d Bytes ---X][PDCP][MOD %u/%u][RB %u] NON INSTANCIATED INSTANCE, DROPPED\n",
                                  frameP,
                                  pdcp_read_header_g.inst,
                                  pdcp_read_header_g.rb_id,
                                  pdcp_read_header_g.data_size,
                                  enb_mod_idP,
                                  ue_mod_idP,
                                  rab_id);
                          }
                      }  else {
                          LOG_D(PDCP, "Forcing send on DEFAULT_RAB_ID\n");
                          LOG_D(PDCP, "[FRAME %5u][eNB][IP][INSTANCE %u][RB %u][--- PDCP_DATA_REQ / %d Bytes --->][PDCP][MOD %u/%u][RB DEFAULT_RAB_ID %u]\n",
                              frameP,
                              pdcp_read_header_g.inst,
                              pdcp_read_header_g.rb_id,
                              pdcp_read_header_g.data_size,
                              enb_mod_idP,
                              ue_mod_idP,
                              DEFAULT_RAB_ID);
                          pdcp_data_req (
                              enb_mod_idP,
                              ue_mod_idP,
                              frameP,
                              enb_flagP,
                              SRB_FLAG_NO,
                              DEFAULT_RAB_ID,
                              RLC_MUI_UNDEFINED,
                              RLC_SDU_CONFIRM_NO,
                              pdcp_read_header_g.data_size,
                              (unsigned char *)NLMSG_DATA(nas_nlh_rx),
                              PDCP_TRANSMISSION_MODE_DATA);
                      }
                  }

              }
          }
      }
  }
  return len;
# endif
#else // neither NAS_NETLINK nor NAS_FIFO
  return 0;
#endif // NAS_NETLINK
}


void pdcp_fifo_read_input_sdus_from_otg (frame_t frameP, eNB_flag_t enb_flagP, module_id_t ue_mod_idP, module_id_t enb_mod_idP) {
  unsigned char       *otg_pkt=NULL;
  module_id_t          src_id, module_id; // src for otg
  module_id_t          dst_id; // dst for otg
  rb_id_t              rb_id;
  signed long          pkt_size=0;
  static unsigned int  pkt_cnt_enb=0, pkt_cnt_ue=0;
  uint8_t              pdcp_mode, is_ue=0;
  Packet_otg_elt_t    *otg_pkt_info=NULL;
  int                  result;

  src_id = enb_mod_idP;

  // we need to add conditions to avoid transmitting data when the UE is not RRC connected.
#if defined(USER_MODE) && defined(OAI_EMU)
  if (oai_emulation.info.otg_enabled ==1 ){
      module_id = (enb_flagP == 1) ?  enb_mod_idP : ue_mod_idP+NB_eNB_INST;
      //rb_id    = (enb_flagP == 1) ? enb_mod_idP * MAX_NUM_RB + DTCH : (NB_eNB_INST + UE_index -1 ) * MAX_NUM_RB + DTCH ;
      src_id = module_id;
      while ((otg_pkt_info = pkt_list_remove_head(&(otg_pdcp_buffer[module_id]))) != NULL) {
          LOG_I(OTG,"Mod_id %d Frame %d Got a packet (%p), HEAD of otg_pdcp_buffer[%d] is %p and Nb elements is %d\n",
              module_id,frameP, otg_pkt_info, module_id, pkt_list_get_head(&(otg_pdcp_buffer[module_id])), otg_pdcp_buffer[module_id].nb_elements);
          //otg_pkt_info = pkt_list_remove_head(&(otg_pdcp_buffer[module_id]));
          dst_id = (otg_pkt_info->otg_pkt).dst_id;
          module_id = (otg_pkt_info->otg_pkt).module_id;
          rb_id = (otg_pkt_info->otg_pkt).rb_id;
          is_ue = (otg_pkt_info->otg_pkt).is_ue;
          pdcp_mode = (otg_pkt_info->otg_pkt).mode;
          //    LOG_I(PDCP,"pdcp_fifo, pdcp mode is= %d\n",pdcp_mode);

          // generate traffic if the ue is rrc reconfigured state
          // if (mac_get_rrc_status(module_id, enb_flagP, dst_id ) > 2 /*RRC_CONNECTED*/) { // not needed: this test is already done in update_otg_enb
          otg_pkt = (unsigned char*) (otg_pkt_info->otg_pkt).sdu_buffer;
          pkt_size = (otg_pkt_info->otg_pkt).sdu_buffer_size;
          if (otg_pkt != NULL) {
              if (is_ue == 0 ) {
		/*rlc_util_print_hex_octets(PDCP,
                                            otg_pkt,
                                            pkt_size);
		*/
                  //rb_id = (/*NB_eNB_INST +*/ dst_id -1 ) * MAX_NUM_RB + DTCH;
                  LOG_D(OTG,"[eNB %d] Frame %d sending packet %d from module %d on rab id %d (src %d, dst %d) pkt size %d for pdcp mode %d\n",
                      enb_mod_idP, frameP, pkt_cnt_enb++, module_id, rb_id, module_id, dst_id, pkt_size, pdcp_mode);
                  result = pdcp_data_req(enb_mod_idP, dst_id, frameP, enb_flagP, SRB_FLAG_NO, rb_id, RLC_MUI_UNDEFINED, RLC_SDU_CONFIRM_NO, pkt_size, otg_pkt,pdcp_mode);
                  AssertFatal (result == TRUE, "PDCP data request failed!\n");
              }
              else {
                  //rb_id= eNB_index * MAX_NUM_RB + DTCH;


                  LOG_D(OTG,"[UE %d] Frame %d: sending packet %d from module %d on rab id %d (src %d, dst %d) pkt size %d\n",
			ue_mod_idP, frameP, pkt_cnt_ue++, src_id, rb_id, src_id, dst_id, pkt_size);
                  result = pdcp_data_req( dst_id,
                                          ue_mod_idP,
                                          frameP,
                                          enb_flagP,
                                          SRB_FLAG_NO,
                                          rb_id,
                                          RLC_MUI_UNDEFINED,
                                          RLC_SDU_CONFIRM_NO,
                                          pkt_size,
                                          otg_pkt,
                                          PDCP_TRANSMISSION_MODE_DATA);
                  AssertFatal (result == TRUE, "PDCP data request failed!\n");
              }
              free(otg_pkt);
              otg_pkt = NULL;
          }
          // } //else LOG_D(OTG,"frameP %d enb %d-> ue %d link not yet established state %d  \n", frameP, eNB_index,dst_id - NB_eNB_INST, mac_get_rrc_status(module_id, enb_flagP, dst_id - NB_eNB_INST));

      }
  }
#else
  if ((otg_enabled==1) && (enb_flagP == 1)) { // generate DL traffic
      unsigned int ctime=0;
      ctime = frameP * 100;

      /*if  ((mac_get_rrc_status(eNB_index, enb_flagP, 0 ) > 2) &&
      (mac_get_rrc_status(eNB_index, enb_flagP, 1 ) > 2)) { */
      for (dst_id = 0; dst_id<NUMBER_OF_UE_MAX; dst_id++) {
          if (mac_get_rrc_status(enb_mod_idP, enb_flagP, dst_id ) > 2) {
              otg_pkt=packet_gen(src_id, dst_id, 0, ctime, &pkt_size);
              if (otg_pkt != NULL){
                  rb_id = dst_id * maxDRB + DTCH;
                  pdcp_data_req(enb_mod_idP, dst_id, frameP, enb_flagP, SRB_FLAG_NO, rb_id, RLC_MUI_UNDEFINED, RLC_SDU_CONFIRM_NO,pkt_size, otg_pkt, PDCP_TRANSMISSION_MODE_DATA);
                  LOG_I(OTG,"send packet from module %d on rab id %d (src %d, dst %d) pkt size %d\n", enb_mod_idP, rb_id, src_id, dst_id, pkt_size);
                  free(otg_pkt);
              }
              /*else {
      LOG_I(OTG,"nothing generated (src %d, dst %d)\n",src_id, dst_id);
      }*/
          }
          /*else {
    LOG_I(OTG,"rrc_status (src %d, dst %d) = %d\n",src_id, dst_id, mac_get_rrc_status(src_id, enb_flagP, dst_id ));
    }*/
      }
  }
#endif
}

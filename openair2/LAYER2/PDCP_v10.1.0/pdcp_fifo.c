/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2011 Eurecom

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
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

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
#include "SIMULATION/ETH_TRANSPORT/extern.h"
#include "UTIL/OCG/OCG.h"
#include "UTIL/OCG/OCG_extern.h"
#include "UTIL/LOG/log.h"
#include "UTIL/FIFO/pad_list.h"
#include "platform_constants.h"


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

unsigned char pdcp_read_state = 0;
//unsigned char pdcp_read_payload[MAX_PAYLOAD];
#endif

extern Packet_OTG_List *otg_pdcp_buffer;

pdcp_data_req_header_t pdcp_read_header;

//-----------------------------------------------------------------------------
int
    pdcp_fifo_flush_sdus (u32_t frame,u8 eNB_flag)
{
  //-----------------------------------------------------------------------------

  mem_block_t     *sdu = list_get_head (&pdcp_sdu_list);
  int             bytes_wrote = 0;
  int             pdcp_nb_sdu_sent = 0;
  u8              cont = 1;
  int ret;
  int mcs_inst;

  while (sdu && cont) {

#if defined(OAI_EMU)
    mcs_inst = ((pdcp_data_ind_header_t *)(sdu->data))->inst;
    // asjust the instance id when passing sdu to IP
    ((pdcp_data_ind_header_t *)(sdu->data))->inst = (((pdcp_data_ind_header_t *)(sdu->data))->inst >= NB_eNB_INST) ?
                                                    ((pdcp_data_ind_header_t *)(sdu->data))->inst - NB_eNB_INST +oai_emulation.info.nb_enb_local - oai_emulation.info.first_ue_local :// UE
                                                    ((pdcp_data_ind_header_t *)(sdu->data))->inst - oai_emulation.info.first_ue_local; // ENB
#else
    ((pdcp_data_ind_header_t *)(sdu->data))->inst = 0;
#endif

#ifdef PDCP_DEBUG
    LOG_I(PDCP, "PDCP->IP TTI %d INST %d: Preparing %d Bytes of data from rab %d to Nas_mesh\n",
          frame, ((pdcp_data_ind_header_t *)(sdu->data))->inst,
          ((pdcp_data_ind_header_t *)(sdu->data))->data_size, ((pdcp_data_ind_header_t *)(sdu->data))->rb_id);
#endif //PDCP_DEBUG

    cont = 0;

    if (!pdcp_output_sdu_bytes_to_write) {
      if (!pdcp_output_header_bytes_to_write) {
        pdcp_output_header_bytes_to_write = sizeof (pdcp_data_ind_header_t);
      }

#ifdef NAS_FIFO
      bytes_wrote = rtf_put (PDCP2NAS_FIFO,
                             &(((u8 *) sdu->data)[sizeof (pdcp_data_ind_header_t) - pdcp_output_header_bytes_to_write]),
                             pdcp_output_header_bytes_to_write);

#else
#ifdef NAS_NETLINK
#ifdef LINUX
      memcpy(NLMSG_DATA(nas_nlh_tx), &(((u8 *) sdu->data)[sizeof (pdcp_data_ind_header_t) - pdcp_output_header_bytes_to_write]),
             pdcp_output_header_bytes_to_write);
      nas_nlh_tx->nlmsg_len = pdcp_output_header_bytes_to_write;
#endif //LINUX
#endif //NAS_NETLINK

      bytes_wrote = pdcp_output_header_bytes_to_write;
#endif //NAS_FIFO

#ifdef PDCP_DEBUG
      LOG_I(PDCP, "Frame %d Sent %d Bytes of header to Nas_mesh\n",
            frame,
            bytes_wrote);
#endif //PDCP_DEBUG

      if (bytes_wrote > 0) {
        pdcp_output_header_bytes_to_write = pdcp_output_header_bytes_to_write - bytes_wrote;

        if (!pdcp_output_header_bytes_to_write) { // continue with sdu
          pdcp_output_sdu_bytes_to_write = ((pdcp_data_ind_header_t *) sdu->data)->data_size;

#ifdef NAS_FIFO
          bytes_wrote = rtf_put (PDCP2NAS_FIFO, &(sdu->data[sizeof (pdcp_data_ind_header_t)]), pdcp_output_sdu_bytes_to_write);
#else

#ifdef NAS_NETLINK
#ifdef LINUX
          memcpy(NLMSG_DATA(nas_nlh_tx)+sizeof(pdcp_data_ind_header_t), &(sdu->data[sizeof (pdcp_data_ind_header_t)]), pdcp_output_sdu_bytes_to_write);
          nas_nlh_tx->nlmsg_len += pdcp_output_sdu_bytes_to_write;
          ret = sendmsg(nas_sock_fd,&nas_msg_tx,0);
          if (ret<0) {
            LOG_D(PDCP, "[PDCP_FIFOS] sendmsg returns %d (errno: %d)\n", ret, errno);
            mac_xface->macphy_exit("");
            break;
          }
#endif // LINUX
#endif //NAS_NETLINK
          bytes_wrote= pdcp_output_sdu_bytes_to_write;
#endif // NAS_FIFO

#ifdef PDCP_DEBUG
          LOG_I(PDCP, "PDCP->IP Frame %d INST %d: Sent %d Bytes of data from rab %d to Nas_mesh\n",
                frame,
                ((pdcp_data_ind_header_t *)(sdu->data))->inst,
                bytes_wrote,
                ((pdcp_data_ind_header_t *)(sdu->data))->rb_id);
#endif //PDCP_DEBUG
          if (bytes_wrote > 0) {
            pdcp_output_sdu_bytes_to_write -= bytes_wrote;

            if (!pdcp_output_sdu_bytes_to_write) { // OK finish with this SDU
              // LOG_D(PDCP, "rb sent a sdu qos_sap %d\n", sapiP);
              LOG_D(PDCP,
                    "[MSC_MSG][FRAME %05d][PDCP][MOD %02d][RB %02d][--- PDCP_DATA_IND / %d Bytes --->][IP][MOD %02d][]\n",
                    frame,
                    mcs_inst, //((pdcp_data_ind_header_t *)(sdu->data))->inst,
                    ((pdcp_data_ind_header_t *)(sdu->data))->rb_id,
                    ((pdcp_data_ind_header_t *)(sdu->data))->data_size,
                    mcs_inst); //((pdcp_data_ind_header_t *)(sdu->data))->inst

              list_remove_head (&pdcp_sdu_list);
              free_mem_block (sdu);
              cont = 1;
              pdcp_nb_sdu_sent += 1;
              sdu = list_get_head (&pdcp_sdu_list);
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
                             (u8 *) (&(sdu->data[sizeof (pdcp_data_ind_header_t) + ((pdcp_data_ind_header_t *) sdu->data)->data_size - pdcp_output_sdu_bytes_to_write])),
                             pdcp_output_sdu_bytes_to_write);
#else  // NAS_FIFO
      bytes_wrote = pdcp_output_sdu_bytes_to_write;
#endif  // NAS_FIFO

      if (bytes_wrote > 0) {
        pdcp_output_sdu_bytes_to_write -= bytes_wrote;

        if (!pdcp_output_sdu_bytes_to_write) {     // OK finish with this SDU
          //PRINT_RB_SEND_OUTPUT_SDU ("[PDCP] RADIO->IP SEND SDU\n");
          list_remove_head (&pdcp_sdu_list);
          free_mem_block (sdu);
          cont = 1;
          pdcp_nb_sdu_sent += 1;
          sdu = list_get_head (&pdcp_sdu_list);
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
            frame);
#endif //PDCP_DEBUG

      rt_pend_linux_srq (pdcp_2_nas_irq);
    } else {
      LOG_E(PDCP, "Frame %d: ERROR IF IP STACK WANTED : NOTIF PACKET(S) pdcp_2_nas_irq not initialized : %d\n",
            frame,
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
int
    pdcp_fifo_read_input_sdus_remaining_bytes (u32_t frame,u8_t eNB_flag)
{
  //-----------------------------------------------------------------------------
  sdu_size_t             bytes_read = 0;
  rb_id_t                rab_id  = 0;

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
              frame,
              pdcp_input_sdu_size_read,
              pdcp_input_header.inst,
              pdcp_input_header.rb_id);
#endif //PDCP_DEBUG
        pdcp_input_sdu_size_read = 0;
#ifdef IDROMEL_NEMO
        pdcp_read_header.inst = 0;
#endif

        if (pdcp_input_header.rb_id != 0) {
          LOG_D(PDCP, "[MSC_MSG][FRAME %05d][IP][MOD %02d][][--- PDCP_DATA_REQ / %d Bytes --->][PDCP][MOD %02d][RB %02d]\n",
                frame, pdcp_read_header.inst,  pdcp_read_header.data_size, pdcp_read_header.inst, pdcp_read_header.rb_id);

            if (pdcp_array[pdcp_read_header.inst][pdcp_read_header.rb_id].instanciated_instance) {
                pdcp_data_req (pdcp_input_header.inst,
                         frame, eNB_flag,
                         pdcp_input_header.rb_id,
                         RLC_MUI_UNDEFINED,
                         RLC_SDU_CONFIRM_NO,
                         pdcp_input_header.data_size,
                         pdcp_input_sdu_buffer,
                         PDCP_DATA_PDU);
            }
        } else if (eNB_flag) {
            // is a broadcast packet, we have to send this packet on all default RABS of all connected UEs
            LOG_D(PDCP, "Checking if could sent on default rabs\n");
#warning CODE TO BE REVIEWED, ONLY WORK FOR SIMPLE TOPOLOGY CASES
            for (rab_id = DEFAULT_RAB_ID; rab_id < MAX_RB; rab_id = rab_id + NB_RB_MAX) {
                LOG_D(PDCP, "Checking if could sent on default rab id %d\n", rab_id);
                if (pdcp_array[pdcp_input_header.inst][rab_id].instanciated_instance == (pdcp_input_header.inst + 1)) {
                    pdcp_data_req (pdcp_input_header.inst,
                                frame, eNB_flag,
                                rab_id,
                                RLC_MUI_UNDEFINED,
                                RLC_SDU_CONFIRM_NO,
                                pdcp_input_header.data_size,
                                pdcp_input_sdu_buffer,
                                PDCP_DATA_PDU);
                }
            }
        } else {
            LOG_D(PDCP, "Forcing send on DEFAULT_RAB_ID\n");
            pdcp_data_req (pdcp_input_header.inst,
                         frame, eNB_flag,
                         DEFAULT_RAB_ID,
                         RLC_MUI_UNDEFINED,
                         RLC_SDU_CONFIRM_NO,
                         pdcp_input_header.data_size,
                         pdcp_input_sdu_buffer,
                         PDCP_DATA_PDU);
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
int
    pdcp_fifo_read_input_sdus (u32_t frame, u8_t eNB_flag)
{
  //-----------------------------------------------------------------------------
//#ifdef NAS_FIFO
//  int             cont;
//  int             bytes_read;
//
//  // if remaining bytes to read
//  if (pdcp_fifo_read_input_sdus_remaining_bytes (frame,eNB_flag) > 0) {
//
//    // all bytes that had to be read for a SDU were read
//    // if not overflow of list, try to get new sdus from rt fifo
//    cont = 1;
//
//    while (cont > 0) {
//      bytes_read = rtf_get (NAS2PDCP_FIFO,
//                            &(((u8 *) & pdcp_input_header)[pdcp_input_index_header]),
//                            sizeof (pdcp_data_req_header_t) - pdcp_input_index_header);
//
//      if (bytes_read > 0) {
//#ifdef PDCP_DEBUG
//        LOG_D(PDCP, "[PDCP_FIFOS] TTI %d Read %d Bytes of data (header %d) from Nas_mesh\n",
//              frame,
//              bytes_read,
//              sizeof(pdcp_data_req_header_t));
//#endif // PDCP_DEBUG
//        pdcp_input_index_header += bytes_read;
//
//        if (pdcp_input_index_header == sizeof (pdcp_data_req_header_t)) {
//#ifdef PDCP_DEBUG
//          LOG_D(PDCP, "TTI %d IP->RADIO READ HEADER sdu size %d\n",
//                frame,
//                pdcp_input_header.data_size);
//#endif //PDCP_DEBUG
//          pdcp_input_index_header = 0;
//          if (pdcp_input_header.data_size < 0) {
//            LOG_E(PDCP, "READ_FIFO: DATA_SIZE %d < 0\n",pdcp_input_header.data_size);
//
//            mac_xface->macphy_exit("");
//            return 0;
//          }
//          pdcp_input_sdu_remaining_size_to_read = pdcp_input_header.data_size;
//          pdcp_input_sdu_size_read     = 0;
//          // we know the size of the sdu, so read the sdu;
//          cont = pdcp_fifo_read_input_sdus_remaining_bytes (frame,eNB_flag);
//        } else {
//          cont = 0;
//        }
//        // error while reading rt fifo
//      } else {
//        cont = 0;
//      }
//    }
//  }
//  return bytes_read;
//
//#else //NAS_FIFO
//#ifdef NAS_NETLINK
//  int len = 1;
//  rb_id_t                rab_id  = 0;
//
//  while (len > 0) {
//      if (pdcp_read_state == 0) {
//#ifdef LINUX
//          len = recvmsg(nas_sock_fd, &nas_msg, 0);
//#else
//          len = -1;
//#endif
//
//          if (len < 0) {
//              // nothing in pdcp NAS socket
//              //LOG_I(PDCP, "[PDCP][NETLINK] Nothing in socket, length %d \n", len);
//          } else {
//#ifdef PDCP_DEBUG
//#ifdef LINUX
//              LOG_I(PDCP, "[PDCP][NETLINK] Received socket with length %d (nlmsg_len = %d)\n", \
//                      len, nas_nlh->nlmsg_len-sizeof(struct nlmsghdr));
//#else
//              LOG_I(PDCP, "[PDCP][NETLINK] nlmsg_len = %d (%d,%d)\n", \
//                       nas_nlh->nlmsg_len, sizeof(pdcp_data_req_header_t), \
//                       sizeof(struct nlmsghdr));
//#endif
//#endif
//          }
//
//#ifdef LINUX
//          if (nas_nlh->nlmsg_len == sizeof (pdcp_data_req_header_t) + sizeof(struct nlmsghdr)) {
//              pdcp_read_state = 1;  //get
//              memcpy((void *)&pdcp_read_header, (void *)NLMSG_DATA(nas_nlh), sizeof(pdcp_data_req_header_t));
//          }
//#else
//          pdcp_read_state = 1;
//#endif
//      }
//
//      if (pdcp_read_state == 1) {
//#ifdef LINUX
//          len = recvmsg(nas_sock_fd, &nas_msg, 0);
//#else
//          len = -1;
//#endif
//
//          if (len < 0) {
//              // nothing in pdcp NAS socket
//              //LOG_I(PDCP, "[PDCP][NETLINK] Nothing in socket, length %d \n", len);
//          } else {
//              pdcp_read_state = 0;
//              // print_active_requests()
//
//#ifdef LINUX
//              memcpy(pdcp_read_payload, (unsigned char *)NLMSG_DATA(nas_nlh), nas_nlh->nlmsg_len - sizeof(struct nlmsghdr));
//#endif
//
//#ifdef OAI_EMU
//              pdcp_read_header.inst = (pdcp_read_header.inst >= oai_emulation.info.nb_enb_local) ? \
//                                pdcp_read_header.inst - oai_emulation.info.nb_enb_local+ NB_eNB_INST + oai_emulation.info.first_ue_local :
//                                pdcp_read_header.inst +  oai_emulation.info.first_enb_local;
//#else
//              pdcp_read_header.inst = 0;
//#endif
//
//              if (pdcp_read_header.rb_id != 0) {
//                  if (pdcp_array[pdcp_read_header.inst][pdcp_read_header.rb_id].instanciated_instance) {
//#ifdef PDCP_DEBUG
//                      LOG_I(PDCP, "[PDCP][NETLINK][IP->PDCP] TTI %d, INST %d: Received socket with length %d (nlmsg_len = %d) on Rab %d \n", \
//                        frame, pdcp_read_header.inst, len, nas_nlh->nlmsg_len-sizeof(struct nlmsghdr), pdcp_read_header.rb_id);
//                      LOG_D(PDCP, "[MSC_MSG][FRAME %05d][IP][MOD %02d][][--- PDCP_DATA_REQ / %d Bytes --->][PDCP][MOD %02d][RB %02d]\n",
//                        frame, pdcp_read_header.inst,  pdcp_read_header.data_size, pdcp_read_header.inst, pdcp_read_header.rb_id);
//#endif
//
//                      pdcp_data_req(pdcp_read_header.inst,
//                        frame,
//                        eNB_flag,
//                        pdcp_read_header.rb_id,
//                        RLC_MUI_UNDEFINED,
//                        RLC_SDU_CONFIRM_NO,
//                        pdcp_read_header.data_size,
//                        pdcp_read_payload,
//                        PDCP_DATA_PDU);
//                  } else {
//                      LOG_E(PDCP, "Received packet for non-instanciated instance %u with rb_id %u\n",
//                                pdcp_read_header.inst, pdcp_read_header.rb_id);
//                  }
//              } else  if (eNB_flag) {
//                // is a broadcast packet, we have to send this packet on all default RABS of all connected UEs
//                  LOG_D(PDCP, "Checking if could sent on default rabs\n");
//#warning CODE TO BE REVIEWED, ONLY WORK FOR SIMPLE TOPOLOGY CASES
//                  for (rab_id = DEFAULT_RAB_ID; rab_id < MAX_RB; rab_id = rab_id + NB_RB_MAX) {
//                      LOG_D(PDCP, "Checking if could sent on default rab id %d\n", rab_id);
//                      if (pdcp_array[pdcp_input_header.inst][rab_id].instanciated_instance == (pdcp_input_header.inst + 1)) {
//                          pdcp_data_req (pdcp_input_header.inst, frame, eNB_flag, rab_id, RLC_MUI_UNDEFINED,RLC_SDU_CONFIRM_NO,
//                                pdcp_input_header.data_size,
//                                pdcp_input_sdu_buffer,
//                                PDCP_DATA_PDU);
//                      }
//                  }
//              } else {
//                  LOG_D(PDCP, "Forcing send on DEFAULT_RAB_ID\n");
//                  pdcp_data_req (pdcp_input_header.inst,
//                         frame, eNB_flag,
//                         DEFAULT_RAB_ID,
//                         RLC_MUI_UNDEFINED,
//                         RLC_SDU_CONFIRM_NO,
//                         pdcp_input_header.data_size,
//                         pdcp_input_sdu_buffer,
//                         PDCP_DATA_PDU);
//              }
//          }
//      }
//  } // end of while
//
//  return len;
//
//#else // neither NAS_NETLINK nor NAS_FIFO
//  return 0;
//#endif // NAS_NETLINK
//#endif // NAS_FIFO
#ifdef NAS_NETLINK
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
                      LOG_I(PDCP, "[PDCP][NETLINK] RX NLMSG_DONE\n");
                      //return;
                  };

                  if (nas_nlh_rx->nlmsg_type == NLMSG_ERROR) {
                      LOG_I(PDCP, "[PDCP][NETLINK] RX NLMSG_ERROR\n");
                  }
                  if (pdcp_read_state == 0) {
                      if (nas_nlh_rx->nlmsg_len == sizeof (pdcp_data_req_header_t) + sizeof(struct nlmsghdr)) {
                          pdcp_read_state = 1;  //get
                          memcpy((void *)&pdcp_read_header, (void *)NLMSG_DATA(nas_nlh_rx), sizeof(pdcp_data_req_header_t));
                          LOG_I(PDCP, "[PDCP][NETLINK] RX pdcp_data_req_header_t inst %u, rb_id %u data_size %d\n", pdcp_read_header.inst, pdcp_read_header.rb_id, pdcp_read_header.data_size);
                      } else {
                          LOG_E(PDCP, "[PDCP][NETLINK] WRONG size %d should be sizeof (pdcp_data_req_header_t) + sizeof(struct nlmsghdr)\n", nas_nlh_rx->nlmsg_len);
                      }
                  } else {
                      pdcp_read_state = 0;
                      // print_active_requests()
#ifdef PDCP_DEBUG
                      LOG_I(PDCP, "[PDCP][NETLINK] Something in socket, length %d \n", nas_nlh_rx->nlmsg_len - sizeof(struct nlmsghdr));
#endif
                      //memcpy(pdcp_read_payload, (unsigned char *)NLMSG_DATA(nas_nlh_rx), nas_nlh_rx->nlmsg_len - sizeof(struct nlmsghdr));

                      #ifdef OAI_EMU
                      pdcp_read_header.inst = (pdcp_read_header.inst >= oai_emulation.info.nb_enb_local) ? \
                          pdcp_read_header.inst - oai_emulation.info.nb_enb_local+ NB_eNB_INST + oai_emulation.info.first_ue_local :
                          pdcp_read_header.inst +  oai_emulation.info.first_enb_local;
                      #else
                      pdcp_read_header.inst = 0;
                      #endif

                      if (pdcp_read_header.rb_id != 0) {
                          if (pdcp_array[pdcp_read_header.inst][pdcp_read_header.rb_id].instanciated_instance) {
#ifdef PDCP_DEBUG
                              LOG_I(PDCP, "[PDCP][NETLINK][IP->PDCP] TTI %d, INST %d: Received socket with length %d (nlmsg_len = %d) on Rab %d \n",
                                  frame, pdcp_read_header.inst, len, nas_nlh_rx->nlmsg_len-sizeof(struct nlmsghdr), pdcp_read_header.rb_id);
                              LOG_D(PDCP, "[MSC_MSG][FRAME %05d][IP][MOD %02d][][--- PDCP_DATA_REQ / %d Bytes --->][PDCP][MOD %02d][RB %02d]\n",
                                  frame, pdcp_read_header.inst, pdcp_read_header.data_size, pdcp_read_header.inst, pdcp_read_header.rb_id);
#endif

                              pdcp_data_req(pdcp_read_header.inst,
                                        frame,
                                        eNB_flag,
                                        pdcp_read_header.rb_id,
                                        RLC_MUI_UNDEFINED,
                                        RLC_SDU_CONFIRM_NO,
                                        pdcp_read_header.data_size,
                                        (unsigned char *)NLMSG_DATA(nas_nlh_rx),
                                        PDCP_DATA_PDU);
                          } else {
                              LOG_E(PDCP, "Received packet for non-instanciated instance %u with rb_id %u\n",
                                  pdcp_read_header.inst, pdcp_read_header.rb_id);
                          }
                      } else if (eNB_flag) {
                          // is a broadcast packet, we have to send this packet on all default RABS of all connected UEs
        #warning CODE TO BE REVIEWED, ONLY WORK FOR SIMPLE TOPOLOGY CASES
                          for (rab_id = DEFAULT_RAB_ID; rab_id < MAX_RB; rab_id = rab_id + NB_RB_MAX) {
                              if (pdcp_array[pdcp_input_header.inst][rab_id].instanciated_instance == (pdcp_input_header.inst + 1)) {
                                  pdcp_data_req (pdcp_read_header.inst, frame, eNB_flag, rab_id, RLC_MUI_UNDEFINED,RLC_SDU_CONFIRM_NO,
                                            pdcp_read_header.data_size,
                                            (unsigned char *)NLMSG_DATA(nas_nlh_rx),
                                            PDCP_DATA_PDU);
                              }
                          }
                      } else {
                          LOG_D(PDCP, "Forcing send on DEFAULT_RAB_ID\n");
                          pdcp_data_req (pdcp_read_header.inst,
                                  frame, eNB_flag,
                                  DEFAULT_RAB_ID,
                                  RLC_MUI_UNDEFINED,
                                  RLC_SDU_CONFIRM_NO,
                                  pdcp_read_header.data_size,
                                  (unsigned char *)NLMSG_DATA(nas_nlh_rx),
                                  PDCP_DATA_PDU);
                      }
                  }
              }
          }
  }
  return len;

#else // neither NAS_NETLINK nor NAS_FIFO
  return 0;
#endif // NAS_NETLINK
}


void pdcp_fifo_read_input_sdus_from_otg (u32_t frame, u8_t eNB_flag, u8 UE_index, u8 eNB_index) {
  unsigned char *otg_pkt=NULL;
  int src_id, module_id; // src for otg
  int dst_id, rb_id; // dst for otg
  int pkt_size=0, pkt_cnt=0;
  u8 pdcp_mode;
  Packet_otg_elt * otg_pkt_info;
  // we need to add conditions to avoid transmitting data when the UE is not RRC connected.
#if defined(USER_MODE) && defined(OAI_EMU)
  if (oai_emulation.info.otg_enabled ==1 ){
    module_id = (eNB_flag == 1) ?  eNB_index : NB_eNB_INST + UE_index ;
    //rb_id    = (eNB_flag == 1) ? eNB_index * MAX_NUM_RB + DTCH : (NB_eNB_INST + UE_index -1 ) * MAX_NUM_RB + DTCH ;

    if (eNB_flag == 1) { // search for DL traffic
      //for (dst_id = NB_eNB_INST; dst_id < NB_UE_INST + NB_eNB_INST; dst_id++) {
      while ((otg_pkt_info = pkt_list_remove_head(&(otg_pdcp_buffer[module_id]))) != NULL) {
        LOG_I(EMU,"HEAD of otg_pdcp_buffer[%d] is %p but Nb elts = %d\n", module_id, pkt_list_get_head(&(otg_pdcp_buffer[module_id])), otg_pdcp_buffer[module_id].nb_elements);
        //otg_pkt_info = pkt_list_remove_head(&(otg_pdcp_buffer[module_id]));
        dst_id = (otg_pkt_info->otg_pkt).dst_id;
        module_id = (otg_pkt_info->otg_pkt).module_id;
        rb_id = (otg_pkt_info->otg_pkt).rb_id;
        pdcp_mode = (otg_pkt_info->otg_pkt).mode;
        // generate traffic if the ue is rrc reconfigured state
	// if (mac_get_rrc_status(module_id, eNB_flag, dst_id ) > 2 /*RRC_CONNECTED*/) { // not needed: this test is already done in update_otg_enb
	otg_pkt = (u8*) (otg_pkt_info->otg_pkt).sdu_buffer;
	pkt_size = (otg_pkt_info->otg_pkt).sdu_buffer_size;
	if (otg_pkt != NULL) {
	  //rb_id = (/*NB_eNB_INST +*/ dst_id -1 ) * MAX_NUM_RB + DTCH;
	  LOG_D(OTG,"[eNB %d] Frame %d sending packet %d from module %d on rab id %d (src %d, dst %d) pkt size %d for pdcp mode %d\n", eNB_index, frame, pkt_cnt++, module_id, rb_id, module_id, dst_id, pkt_size, pdcp_mode);
	  pdcp_data_req(module_id, frame, eNB_flag, rb_id, RLC_MUI_UNDEFINED, RLC_SDU_CONFIRM_NO, pkt_size, otg_pkt,pdcp_mode);
	  free(otg_pkt);
	}
	// } //else LOG_D(OTG,"frame %d enb %d-> ue %d link not yet established state %d  \n", frame, eNB_index,dst_id - NB_eNB_INST, mac_get_rrc_status(module_id, eNB_flag, dst_id - NB_eNB_INST));
      }
    }
    else {
      while ((otg_pkt_info = pkt_list_remove_head(&(otg_pdcp_buffer[module_id]))) != NULL) {
	//otg_pkt_info = pkt_list_remove_head(&(otg_pdcp_buffer[module_id]));
	dst_id = (otg_pkt_info->otg_pkt).dst_id;
	module_id = (otg_pkt_info->otg_pkt).module_id;
	rb_id = (otg_pkt_info->otg_pkt).rb_id;
	src_id = module_id;
	
	// if (mac_get_rrc_status(module_id, eNB_flag, eNB_index ) > 2 /*RRC_CONNECTED*/) {  // not needed: this test is already done in update_otg_ue
	otg_pkt = (u8*) (otg_pkt_info->otg_pkt).sdu_buffer;
	pkt_size = (otg_pkt_info->otg_pkt).sdu_buffer_size;
	if (otg_pkt != NULL){
	//rb_id= eNB_index * MAX_NUM_RB + DTCH;
	  LOG_D(OTG,"[UE %d] sending packet from module %d on rab id %d (src %d, dst %d) pkt size %d\n", UE_index, src_id, rb_id, src_id, dst_id, pkt_size);
	  pdcp_data_req(src_id, frame, eNB_flag, rb_id, RLC_MUI_UNDEFINED, RLC_SDU_CONFIRM_NO,pkt_size, otg_pkt, PDCP_DATA_PDU);
	  free(otg_pkt);
	}
	//} //else LOG_D(OTG,"frame %d ue %d-> enb %d link not yet established state %d  \n", frame, UE_index, eNB_index, mac_get_rrc_status(module_id, eNB_flag, eNB_index ));
      }
    }
  }
#else
  if ((otg_enabled==1) && (eNB_flag == 1)) { // generate DL traffic
    unsigned int ctime=0;
    src_id = eNB_index;
    ctime = frame * 100;

    /*if  ((mac_get_rrc_status(eNB_index, eNB_flag, 0 ) > 2) &&
  (mac_get_rrc_status(eNB_index, eNB_flag, 1 ) > 2)) { */
    for (dst_id = 0; dst_id<NUMBER_OF_UE_MAX; dst_id++) {
      if (mac_get_rrc_status(eNB_index, eNB_flag, dst_id ) > 2) {
        otg_pkt=packet_gen(src_id, dst_id, ctime, &pkt_size);
        if (otg_pkt != NULL){
          rb_id = dst_id * NB_RB_MAX + DTCH;
          pdcp_data_req(src_id, frame, eNB_flag, rb_id, RLC_MUI_UNDEFINED, RLC_SDU_CONFIRM_NO,pkt_size, otg_pkt, PDCP_DATA_PDU);
          LOG_I(OTG,"send packet from module %d on rab id %d (src %d, dst %d) pkt size %d\n", eNB_index, rb_id, src_id, dst_id, pkt_size);
          free(otg_pkt);
        }
        /*else {
            LOG_I(OTG,"nothing generated (src %d, dst %d)\n",src_id, dst_id);
            }*/
      }
      /*else {
          LOG_I(OTG,"rrc_status (src %d, dst %d) = %d\n",src_id, dst_id, mac_get_rrc_status(src_id, eNB_flag, dst_id ));
          }*/
    }
  }
#endif
}

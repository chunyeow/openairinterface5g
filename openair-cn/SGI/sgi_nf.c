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

  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.

 *******************************************************************************/
/*! \file sgi_nf.c
* \brief
* \author Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/
#include <stdio.h>
#include <string.h>

#include <netinet/in.h>
#include <linux/netfilter.h>  /* Defines verdicts (NF_ACCEPT, etc) */
#include <libnfnetlink/libnfnetlink.h>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <libnetfilter_queue/linux_nfnetlink_queue.h>

#include "mme_config.h"
#include "intertask_interface.h"
#include "sgi.h"
//-----------------------------------------------------------------------------
struct nfq_handle
{
         struct nfnl_handle *nfnlh;
         struct nfnl_subsys_handle *nfnlssh;
         struct nfq_q_handle *qh_list;
};

struct nfq_q_handle
{
         struct nfq_q_handle *next;
         struct nfq_handle *h;
         u_int16_t id;

         nfq_callback *cb;
         void *data;
};

struct nfq_data {
         struct nfattr **data;
};


enum  	{
  NFQ_XML_HW = (1 << 0), NFQ_XML_MARK = (1 << 1), NFQ_XML_DEV = (1 << 2), NFQ_XML_PHYSDEV = (1 << 3),
  NFQ_XML_PAYLOAD = (1 << 4), NFQ_XML_TIME = (1 << 5), NFQ_XML_ALL = ~0U
};
//------------------------------------------------------
#define SNPRINTF_FAILURE(ret, rem, offset, len)                 \
do {                                                            \
         if (ret < 0)                                           \
                 return ret;                                    \
         len += ret;                                            \
         if (ret > rem)                                         \
                 ret = rem;                                     \
         offset += ret;                                         \
         rem -= ret;                                            \
} while (0)

//------------------------------------------------------
static int nfq_snprintf(char *buf, size_t rem, struct nfq_data *tb, int flags)
//------------------------------------------------------
{
         struct nfqnl_msg_packet_hdr *ph;
         struct nfqnl_msg_packet_hw *hwph;
         u_int32_t mark, ifi;
         int size, offset = 0, len = 0, ret;
         unsigned char *data;

         size = snprintf(buf + offset, rem, "<pkt>");
         SNPRINTF_FAILURE(size, rem, offset, len);



         ph = nfq_get_msg_packet_hdr(tb);
         if (ph) {
                 size = snprintf(buf + offset, rem,
                                 "<hook> %u <id>%u",
                                 ph->hook, ntohl(ph->packet_id));
                 SNPRINTF_FAILURE(size, rem, offset, len);

                 hwph = nfq_get_packet_hw(tb);
                 if (hwph && (flags & NFQ_XML_HW)) {
                         int i, hlen = ntohs(hwph->hw_addrlen);

                         size = snprintf(buf + offset, rem, "<hw><proto>%04x"
                                                            "</proto>",
                                         ntohs(ph->hw_protocol));
                         SNPRINTF_FAILURE(size, rem, offset, len);

                         size = snprintf(buf + offset, rem, "<src>");
                         SNPRINTF_FAILURE(size, rem, offset, len);

                         for (i=0; i<hlen; i++) {
                                 size = snprintf(buf + offset, rem, "%02x",
                                                 hwph->hw_addr[i]);
                                 SNPRINTF_FAILURE(size, rem, offset, len);
                         }

                         size = snprintf(buf + offset, rem, "</src></hw>");
                         SNPRINTF_FAILURE(size, rem, offset, len);
                 } else if (flags & NFQ_XML_HW) {
                         size = snprintf(buf + offset, rem, "<hw><proto>%04x"
                                                     "</proto></hw>",
                                  ntohs(ph->hw_protocol));
                         SNPRINTF_FAILURE(size, rem, offset, len);
                 }
         }

         mark = nfq_get_nfmark(tb);
         if (mark && (flags & NFQ_XML_MARK)) {
                 size = snprintf(buf + offset, rem, "<mark>%u</mark>", mark);
                 SNPRINTF_FAILURE(size, rem, offset, len);
         }

         ifi = nfq_get_indev(tb);
         if (ifi && (flags & NFQ_XML_DEV)) {
                 size = snprintf(buf + offset, rem, "<indev>%u</indev>", ifi);
                 SNPRINTF_FAILURE(size, rem, offset, len);
         }

         ifi = nfq_get_outdev(tb);
         if (ifi && (flags & NFQ_XML_DEV)) {
                 size = snprintf(buf + offset, rem, "<outdev>%u</outdev>", ifi);
                 SNPRINTF_FAILURE(size, rem, offset, len);
         }

         ifi = nfq_get_physindev(tb);
         if (ifi && (flags & NFQ_XML_PHYSDEV)) {
                 size = snprintf(buf + offset, rem,
                                 "<physindev>%u</physindev>", ifi);
                 SNPRINTF_FAILURE(size, rem, offset, len);
         }

         ifi = nfq_get_physoutdev(tb);
         if (ifi && (flags & NFQ_XML_PHYSDEV)) {
                 size = snprintf(buf + offset, rem,
                                 "<physoutdev>%u</physoutdev>", ifi);
                 SNPRINTF_FAILURE(size, rem, offset, len);
         }

         ret = nfq_get_payload(tb, &data);
         if (ret >= 0 && (flags & NFQ_XML_PAYLOAD)) {
                 int i;

                 size = snprintf(buf + offset, rem, "<payload>");
                 SNPRINTF_FAILURE(size, rem, offset, len);

                 for (i=0; i<ret; i++) {
                         size = snprintf(buf + offset, rem, "%02x",
                                         data[i] & 0xff);
                         SNPRINTF_FAILURE(size, rem, offset, len);
                 }

                 size = snprintf(buf + offset, rem, "</payload>");
                 SNPRINTF_FAILURE(size, rem, offset, len);
         }

         size = snprintf(buf + offset, rem, "</pkt>");
         SNPRINTF_FAILURE(size, rem, offset, len);

         return len;
}
//------------------------------------------------------
static int sgi_nfqueue_callback(struct nfq_q_handle *myQueue, struct nfgenmsg *msg,
                    struct nfq_data *pkt, void *cbData)
//-----------------------------------------------------------------------------
{
  uint32_t                     id = 0;
  struct nfqnl_msg_packet_hdr *header;
  struct nfqnl_msg_packet_hw  *macAddr;
  MessageDef                  *message_p;
  Gtpv1uTunnelDataReq         *data_req_p;
  Teid_t                       local_teid;
  u_int32_t                    verdict;

  if ((header = nfq_get_msg_packet_hdr(pkt))) {
    id = ntohl(header->packet_id);
    SGI_IF_DEBUG("packet_id %u hw_protocol %u hook %u\n", ntohl(header->packet_id), ntohs(header->hw_protocol), header->hook);
  }

  // The HW address is only fetchable at certain hook points
  // LG Warning HW address is src address
  macAddr = nfq_get_packet_hw(pkt);
  if (macAddr) {
	  SGI_IF_DEBUG("MAC len %u addr %02X:%02X:%02X:%02X:%02X:%02X\n",
			  ntohs(macAddr->hw_addrlen),
			  macAddr->hw_addr[0],
			  macAddr->hw_addr[1],
			  macAddr->hw_addr[2],
			  macAddr->hw_addr[3],
			  macAddr->hw_addr[4],
			  macAddr->hw_addr[5]);
  } else {
	  SGI_IF_DEBUG("No MAC addr\n");
  }

  /*timeval tv;
  if (!nfq_get_timestamp(pkt, &tv)) {
    cout << "; tstamp " << tv.tv_sec << "." << tv.tv_usec;
  } else {
    cout << "; no tstamp";
  }*/

  //cout << "; mark " << nfq_get_nfmark(pkt);
  // Print the payload; in copy meta mode, only headers will be included;
  // in copy packet mode, whole packet will be returned.
  char *pktData;
  int len = nfq_get_payload(pkt, &pktData);

  // Note that you can also get the physical devices
  SGI_IF_DEBUG("pkt id %d recvd: indev %d outdev %d mark %d len %d\n",
             id,
             nfq_get_indev(pkt),
             nfq_get_outdev(pkt),
             nfq_get_nfmark(pkt),
             len);


  //sgi_print_hex_octets((unsigned char *)pktData, len);

  message_p = itti_alloc_new_message(TASK_FW_IP, GTPV1U_TUNNEL_DATA_REQ);
  if (message_p == NULL) {
      return -1;
  }
  data_req_p = &message_p->ittiMsg.gtpv1uTunnelDataReq;
  data_req_p->buffer = malloc(sizeof(uint8_t) * len);
  if (data_req_p->buffer == NULL) {
      SGI_IF_ERROR("Failed to allocate new buffer\n");
      itti_free(ITTI_MSG_ORIGIN_ID(message_p), message_p);
      message_p = NULL;
      return -1;
  } else {
	  // MAY BE TO BE CHANGED
      local_teid = nfq_get_nfmark(pkt);

      // if packet is marked, it is a packet going into a EPS bearer
      //if (local_teid != 0) {
      if (1) {
    	  data_req_p->local_S1u_teid    = local_teid;

    	  memcpy(data_req_p->buffer, pktData, len);
    	  data_req_p->length = len;

    	  if (itti_send_msg_to_task(TASK_GTPV1_U, INSTANCE_DEFAULT, message_p) < 0) {
    		  SGI_IF_ERROR("Failed to send message to task\n");
    	        itti_free(ITTI_MSG_ORIGIN_ID(message_p), message_p);
    	        message_p = NULL;
    	  }
    	  verdict = NF_STOLEN;
    	  return nfq_set_verdict(myQueue, id, verdict, 0, NULL);
      } else {
    	  verdict = NF_ACCEPT;
    	  return nfq_set_verdict(myQueue, id, verdict, len, pktData);
      }
  }
  return 0;
  // end sgi_nfqueue_callback
}
//-----------------------------------------------------------------------------
void* sgi_nf_fw_2_gtpv1u_thread(void *args_p)
//-----------------------------------------------------------------------------
{
    struct nfq_handle     *nfqHandle;
    struct nfq_q_handle   *myQueue;
    struct nfnl_handle    *netlinkHandle;
    int                    fd, res;
    volatile sgi_data_t   *sgi_data_p;

    sgi_data_p = (sgi_data_t*)args_p;

    SGI_IF_DEBUG("RX thread on netfilter queue started, operating on Device %s  iif %d\n", sgi_data_p->interface_name, sgi_data_p->interface_index);

    // Get a queue connection handle from the module
    if (!(nfqHandle = nfq_open())) {
        SGI_IF_ERROR("Error in nfq_open()");
        exit(-1);
    }

    // Unbind the handler from processing any IP packets
    // Not totally sure why this is done, or if it's necessary...
    if (nfq_unbind_pf(nfqHandle, PF_INET) < 0) {
        SGI_IF_ERROR("Error in nfq_unbind_pf()");
        exit(-1);
    }

    // Bind this handler to process IP packets...
    if (nfq_bind_pf(nfqHandle, PF_INET) < 0) {
        SGI_IF_ERROR("Error in nfq_bind_pf()");
        exit(-1);
    }

    // Install a callback on queue 1
    if (!(myQueue = nfq_create_queue(nfqHandle,  1, &sgi_nfqueue_callback, NULL))) {
        SGI_IF_ERROR("Error in nfq_create_queue()");
        exit(-1);
    }

    // Turn on packet copy mode
    if (nfq_set_mode(myQueue, NFQNL_COPY_PACKET, 0xffff) < 0) {
        SGI_IF_ERROR("Could not set packet copy mode");
        exit(-1);
    }

    netlinkHandle = nfq_nfnlh(nfqHandle);
    fd = nfnl_fd(netlinkHandle);

    SGI_IF_DEBUG("Waiting for egress packets\n");
    sgi_data_p->thread_started = 1;
    while ((res = recv(fd, &sgi_data_p->recv_buffer[sizeof(struct ether_header)], SGI_BUFFER_RECV_LEN, 0)) && res >= 0) {
        // a callback mechanism is used
        // rather than just handling it directly here, but that
        // seems to be the convention...
        nfq_handle_packet(nfqHandle, &sgi_data_p->recv_buffer[sizeof(struct ether_header)], res);
        // end while receiving traffic
    }

    nfq_destroy_queue(myQueue);

    nfq_close(nfqHandle);

    return NULL;
}

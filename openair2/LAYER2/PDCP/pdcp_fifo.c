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
* \brief pdcp interface with linux IP interface
* \author  Lionel GAUTHIER and Navid Nikaein
* \date 2009
* \version 0.5
* \warning This component can be runned only in user-space
* @ingroup pdcp

*/

#define PDCP_FIFO_C
#include "pdcp.h"
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
#include "SIMULATION/ETH_TRANSPORT/extern.h"

#define PDCP_DEBUG 1

//#define IDROMEL_NEMO 1

#ifdef USER_MODE
#include "UTIL/OCG/OCG.h"
#include "UTIL/OCG/OCG_extern.h"
#endif

#ifdef NAS_NETLINK



#include <sys/socket.h>
#include <linux/netlink.h>



extern struct sockaddr_nl nas_src_addr, nas_dest_addr;
extern struct nlmsghdr *nas_nlh;
extern struct iovec nas_iov;
extern int nas_sock_fd;
extern struct msghdr nas_msg;

#define MAX_PAYLOAD 1600  /* maximum payload size*/

unsigned char pdcp_read_state = 0;

char pdcp_read_payload[MAX_PAYLOAD];
#endif
pdcp_data_req_header_t pdcp_read_header;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int
pdcp_fifo_flush_sdus ()
{
//-----------------------------------------------------------------------------

  mem_block_t     *sdu = list_get_head (&pdcp_sdu_list);
  int             bytes_wrote = 0;
  int             pdcp_nb_sdu_sent = 0;
  u8              cont = 1;
  int ret;


  while ((sdu) && (cont)) {

#ifdef USER_MODE
    // asjust the instance id when passing sdu to IP
    ((pdcp_data_ind_header_t *)(sdu->data))->inst = (((pdcp_data_ind_header_t *)(sdu->data))->inst >= NB_eNB_INST) ?
      ((pdcp_data_ind_header_t *)(sdu->data))->inst - NB_eNB_INST +oai_emulation.info.nb_enb_local - oai_emulation.info.first_ue_local :// UE
      ((pdcp_data_ind_header_t *)(sdu->data))->inst - oai_emulation.info.first_ue_local; // ENB
#else
    ((pdcp_data_ind_header_t *)(sdu->data))->inst = 0;
#endif

#ifdef PDCP_DEBUG
	  msg("[PDCP][INFO] PDCP->IP TTI %d INST %d: Preparing %d Bytes of data from rab %d to Nas_mesh\n",
	      Mac_rlc_xface->frame,
	      ((pdcp_data_ind_header_t *)(sdu->data))->inst,
	      ((pdcp_data_ind_header_t *)(sdu->data))->data_size,
	      ((pdcp_data_ind_header_t *)(sdu->data))->rb_id);
#endif //PDCP_DEBUG

    cont = 0;

    if (!pdcp_output_sdu_bytes_to_write) {
      if (!pdcp_output_header_bytes_to_write) {
        pdcp_output_header_bytes_to_write = sizeof (pdcp_data_ind_header_t);
      }

#ifndef USER_MODE
      bytes_wrote = rtf_put (PDCP2NAS_FIFO,
			     &(((u8 *) sdu->data)[sizeof (pdcp_data_ind_header_t) - pdcp_output_header_bytes_to_write]),
			     pdcp_output_header_bytes_to_write);

#else
#ifdef NAS_NETLINK
#ifdef LINUX
      memcpy(NLMSG_DATA(nas_nlh), &(((u8 *) sdu->data)[sizeof (pdcp_data_ind_header_t) - pdcp_output_header_bytes_to_write]),
                             pdcp_output_header_bytes_to_write);
      nas_nlh->nlmsg_len = pdcp_output_header_bytes_to_write;
#endif //LINUX
#endif //NAS_NETLINK

      bytes_wrote = pdcp_output_header_bytes_to_write;
#endif //USER_MODE


#ifdef PDCP_DEBUG
      msg("[PDCP][INFO] TTI %d Sent %d Bytes of header to Nas_mesh\n",
	  Mac_rlc_xface->frame,
	  bytes_wrote);
#endif //PDCP_DEBUG

      if (bytes_wrote > 0) {
        pdcp_output_header_bytes_to_write = pdcp_output_header_bytes_to_write - bytes_wrote;

        if (!pdcp_output_header_bytes_to_write) { // continue with sdu

          pdcp_output_sdu_bytes_to_write = ((pdcp_data_ind_header_t *) sdu->data)->data_size;

#ifndef USER_MODE
          bytes_wrote = rtf_put (PDCP2NAS_FIFO, &(sdu->data[sizeof (pdcp_data_ind_header_t)]), pdcp_output_sdu_bytes_to_write);
#else

#ifdef NAS_NETLINK
#ifdef LINUX
	  memcpy(NLMSG_DATA(nas_nlh)+sizeof(pdcp_data_ind_header_t), &(sdu->data[sizeof (pdcp_data_ind_header_t)]), pdcp_output_sdu_bytes_to_write);
	  nas_nlh->nlmsg_len += pdcp_output_sdu_bytes_to_write;
	  ret = sendmsg(nas_sock_fd,&nas_msg,0);
	  if (ret<0) {
	    msg("[PDCP_FIFOS] sendmsg returns %d\n",ret);
	    perror("error code:");
	    mac_xface->macphy_exit("");
	    break;
	  }
#endif // LINUX
#endif //NAS_NETLINK
	  bytes_wrote= pdcp_output_sdu_bytes_to_write;
#endif // USER_MODE

#ifdef PDCP_DEBUG
	  msg("[PDCP][INFO] PDCP->IP TTI %d INST %d: Sent %d Bytes of data from rab %d to Nas_mesh\n",
	      Mac_rlc_xface->frame,
	      ((pdcp_data_ind_header_t *)(sdu->data))->inst,
	      bytes_wrote,
	      ((pdcp_data_ind_header_t *)(sdu->data))->rb_id);
#endif //PDCP_DEBUG
          if (bytes_wrote > 0) {
            pdcp_output_sdu_bytes_to_write -= bytes_wrote;

            if (!pdcp_output_sdu_bytes_to_write) { // OK finish with this SDU
	      // msg("rb sent a sdu qos_sap %d\n",sapiP);

              list_remove_head (&pdcp_sdu_list);
              free_mem_block (sdu);
              cont = 1;
              pdcp_nb_sdu_sent += 1;
              sdu = list_get_head (&pdcp_sdu_list);
            }
          } else {
            msg ("[PDCP] RADIO->IP SEND SDU CONGESTION!\n");
          }
        } else {
          msg ("[PDCP] RADIO->IP SEND SDU CONGESTION!\n");
        }
      }
    } else {
      // continue writing sdu
#ifndef USER_MODE
      bytes_wrote = rtf_put (PDCP2NAS_FIFO,
                             (u8 *) (&(sdu->data[sizeof (pdcp_data_ind_header_t) + ((pdcp_data_ind_header_t *) sdu->data)->data_size - pdcp_output_sdu_bytes_to_write])),
                             pdcp_output_sdu_bytes_to_write);
#else  // USER_MODE
      bytes_wrote = pdcp_output_sdu_bytes_to_write;
#endif  // USER_MODE

      if (bytes_wrote > 0) {
        pdcp_output_sdu_bytes_to_write -= bytes_wrote;

        if (!pdcp_output_sdu_bytes_to_write) {     // OK finish with this SDU
          //PRINT_RB_SEND_OUTPUT_SDU ("[PDCP] RADIO->IP SEND SDU\n");
          list_remove_head (&pdcp_sdu_list);
          free_mem_block (sdu);
          cont = 1;
          pdcp_nb_sdu_sent += 1;
          sdu = list_get_head (&pdcp_sdu_list);
	  // msg("rb sent a sdu from rab\n");
        }
      }
    }
  }
#ifndef USER_MODE
    if ((pdcp_nb_sdu_sent)) {
      if ((pdcp_2_nas_irq > 0)) {
#ifdef PDCP_DEBUG
	msg("[PDCP][INFO] TTI %d : Trigger NAS RX interrupt\n",
	    Mac_rlc_xface->frame);
#endif //PDCP_DEBUG

           rt_pend_linux_srq (pdcp_2_nas_irq);
      } else {
        msg ("[PDCP] TTI %d: ERROR IF IP STACK WANTED : NOTIF PACKET(S) pdcp_2_nas_irq not initialized : %d\n",
	     Mac_rlc_xface->frame,
	     pdcp_2_nas_irq);
      }
    }
#endif  //USER_MODE

  return pdcp_nb_sdu_sent;
}

//-----------------------------------------------------------------------------
/*
 * returns a positive value if whole bytes that had to be read were read
 * returns zero  value if whole bytes that had to be read were not read at all
 * returns a negative  value if an error was encountered while reading the rt fifo
 */
int
pdcp_fifo_read_input_sdus_remaining_bytes ()
{
//-----------------------------------------------------------------------------
  sdu_size_t             bytes_read=0;
  // if remaining bytes to read



  if (pdcp_input_sdu_remaining_size_to_read > 0) {

    // printk("[PDCP][INFO] read_input_sdus pdcp_input_sdu_remaining_size_to_read = %d \n", pdcp_input_sdu_remaining_size_to_read);
    bytes_read = rtf_get (NAS2PDCP_FIFO,
			  &(pdcp_input_sdu_buffer[pdcp_input_sdu_size_read]),
			  pdcp_input_sdu_remaining_size_to_read);

    //printk("[PDCP][INFO] read fifo returned %d \n", bytes_read);
    if (bytes_read > 0) {

      //msg("[PDCP_FIFOS] Read %d remaining Bytes of data from Nas_mesh\n",bytes_read);

      pdcp_input_sdu_remaining_size_to_read = pdcp_input_sdu_remaining_size_to_read - bytes_read;
      pdcp_input_sdu_size_read = pdcp_input_sdu_size_read + bytes_read;

      if (pdcp_input_sdu_remaining_size_to_read != 0) {
        return 0;
      } else {
#ifdef PDCP_DEBUG
	msg("[PDCP][INFO]  TTI %d: IP->RADIO RECEIVED COMPLETE SDU size %d inst %d rb %d\n",
	    Mac_rlc_xface->frame,
	    pdcp_input_sdu_size_read,
	    pdcp_input_header.inst,
	    pdcp_input_header.rb_id);
#endif //PDCP_DEBUG
        pdcp_input_sdu_size_read = 0;
#ifdef IDROMEL_NEMO
	pdcp_read_header.inst = 0;
#endif
        pdcp_data_req (pdcp_input_header.inst,
		       pdcp_input_header.rb_id,
		       pdcp_input_header.data_size,
		       pdcp_input_sdu_buffer);

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

#ifndef NAS_NETLINK

//-----------------------------------------------------------------------------
int
pdcp_fifo_read_input_sdus ()
{
//-----------------------------------------------------------------------------
  int             cont;
  int             bytes_read;

  // if remaining bytes to read
  if (pdcp_fifo_read_input_sdus_remaining_bytes () > 0) {

    // all bytes that had to be read for a SDU were read
    // if not overflow of list, try to get new sdus from rt fifo
    cont = 1;

    while (cont > 0) {

      bytes_read = rtf_get (NAS2PDCP_FIFO,
                            &(((u8 *) & pdcp_input_header)[pdcp_input_index_header]),
                            sizeof (pdcp_data_req_header_t) - pdcp_input_index_header);


      if (bytes_read > 0) {

#ifdef PDCP_DEBUG
	msg("[PDCP_FIFOS] TTI %d Read %d Bytes of data (header %d) from Nas_mesh\n",
	    Mac_rlc_xface->frame,
	    bytes_read,
	    sizeof(pdcp_data_req_header_t));
#endif // PDCP_DEBUG
        pdcp_input_index_header += bytes_read;

        if (pdcp_input_index_header == sizeof (pdcp_data_req_header_t)) {
#ifdef PDCP_DEBUG
	  msg("[PDCP] TTI %d IP->RADIO READ HEADER sdu size %d\n",
	      Mac_rlc_xface->frame,
	      pdcp_input_header.data_size);
#endif //PDCP_DEBUG
          pdcp_input_index_header = 0;
	  if(pdcp_input_header.data_size<0){
	    msg("[PDCP][FATAL ERROR] READ_FIFO: DATA_SIZE %d < 0\n",pdcp_input_header.data_size);

	    mac_xface->macphy_exit("");
	    return 0;
	  }
	  pdcp_input_sdu_remaining_size_to_read = pdcp_input_header.data_size;
          pdcp_input_sdu_size_read     = 0;
          // we know the size of the sdu, so read the sdu;
          cont = pdcp_fifo_read_input_sdus_remaining_bytes ();
        } else {
          cont = 0;
        }
        // error while reading rt fifo
      } else {
        cont = 0;
      }
    }
  }
  return 0;
}

#else //NAS_NETLINK



//-----------------------------------------------------------------------------
int
pdcp_fifo_read_input_sdus ()
{
//-----------------------------------------------------------------------------
  int             cont;
  int             bytes_read;
  int len;



    if (pdcp_read_state == 0) {

#ifdef LINUX
      len = recvmsg(nas_sock_fd, &nas_msg, 0);
#else
      len = -1;
#endif

      if (len<0) {
	//		printf("[PDCP][NETLINK %d] nothing in pdcp NAS socket\n", nas_sock_fd);
       }
      else {
#ifdef PDCP_DEBUG
#ifdef LINUX
		printf("[PDCP][NETLINK] Received socket with length %d (nlmsg_len = %d)\n",len,nas_nlh->nlmsg_len-sizeof(struct nlmsghdr));
#endif PDCP_DEBUG
#ifdef PDCP_DEBUG
		printf("[PDCP][NETLINK] nlmsg_len = %d (%d,%d)\n",nas_nlh->nlmsg_len,
		       sizeof(pdcp_data_req_header_t),
		       sizeof(struct nlmsghdr));
#endif LINUX
#endif PDCP_DEBUG
      }
#ifdef LINUX
      if (nas_nlh->nlmsg_len == sizeof (pdcp_data_req_header_t) + sizeof(struct nlmsghdr)) {

	pdcp_read_state = 1;  //get
	memcpy((void *)&pdcp_read_header,
	       (void *)NLMSG_DATA(nas_nlh),
	       sizeof(pdcp_data_req_header_t));
      }
#else //LINUX
      pdcp_read_state = 1;
#endif //LINUX
    }

    if (pdcp_read_state == 1) {

#ifdef LINUX
      len = recvmsg(nas_sock_fd, &nas_msg, 0);
#else
      len = -1;
#endif //LINUX

      if (len<0) {
	// nothing in pdcp NAS socket
      }
      else {


	pdcp_read_state = 0;
	//	print_active_requests()

#ifdef LINUX
	memcpy(pdcp_read_payload,
	       (unsigned char *)NLMSG_DATA(nas_nlh),
	       nas_nlh->nlmsg_len - sizeof(struct nlmsghdr));
#endif

#ifdef IDROMEL_NEMO
	pdcp_read_header.inst = 0;
#endif
	pdcp_read_header.inst = (pdcp_read_header.inst >= oai_emulation.info.nb_enb_local) ?
	  pdcp_read_header.inst - oai_emulation.info.nb_enb_local+ NB_eNB_INST + oai_emulation.info.first_ue_local :
	  pdcp_read_header.inst +  oai_emulation.info.first_enb_local;

#ifdef PDCP_DEBUG
	printf("[PDCP][NETLINK][IP->PDCP] TTI %d, INST %d: Received socket with length %d (nlmsg_len = %d) on Rab %d \n",
	       Mac_rlc_xface->frame,
	       pdcp_read_header.inst,
	       len,
	       nas_nlh->nlmsg_len-sizeof(struct nlmsghdr),
	       pdcp_read_header.rb_id);
#endif PDCP_DEBUG

	pdcp_data_req(pdcp_read_header.inst,
		      pdcp_read_header.rb_id,
		      pdcp_read_header.data_size,
		      pdcp_read_payload);
      }

    }
}
#endif

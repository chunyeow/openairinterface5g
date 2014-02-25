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

/*! \file pdcp.c
 * \brief pdcp interface with RLC
 * \author  Lionel GAUTHIER and Navid Nikaein
 * \date 2009-2012
 * \version 1.0
 */

#define PDCP_C
#ifndef USER_MODE
#include <rtai_fifos.h>
#endif
#include "assertions.h"
#include "pdcp.h"
#include "pdcp_util.h"
#include "pdcp_sequence_manager.h"
#include "LAYER2/RLC/rlc.h"
#include "LAYER2/MAC/extern.h"
#include "RRC/L2_INTERFACE/openair_rrc_L2_interface.h"
#include "pdcp_primitives.h"
#include "OCG.h"
#include "OCG_extern.h"
#include "UTIL/LOG/log.h"
#include <inttypes.h>
#include "platform_constants.h"
#include "UTIL/LOG/vcd_signal_dumper.h"

#if defined(ENABLE_SECURITY)
# include "UTIL/OSA/osa_defs.h"
#endif

#if defined(ENABLE_ITTI)
# include "intertask_interface.h"
#endif

#ifndef OAI_EMU
extern int otg_enabled;
#endif

//extern char *packet_gen(int src, int dst, int ctime, int *pkt_size);
extern int otg_rx_pkt( int src, int dst, int ctime, char *buffer_tx, unsigned int size);

//-----------------------------------------------------------------------------
/*
 * If PDCP_UNIT_TEST is set here then data flow between PDCP and RLC is broken
 * and PDCP has no longer anything to do with RLC. In this case, after it's handed
 * an SDU it appends PDCP header and returns (by filling in incoming pointer parameters)
 * this mem_block_t to be dissected for testing purposes. For further details see test
 * code at targets/TEST/PDCP/test_pdcp.c:test_pdcp_data_req()
 */
BOOL pdcp_data_req(module_id_t enb_mod_idP, module_id_t ue_mod_idP, frame_t frameP, eNB_flag_t enb_flagP, rb_id_t rb_idP, mui_t muiP, u32 confirmP,
    sdu_size_t sdu_buffer_sizeP, unsigned char* sdu_buffer_pP, u8 modeP)
{
  //-----------------------------------------------------------------------------
  pdcp_t            *pdcp_p          = NULL;
  u8                 i               = 0;
  u8                 pdcp_header_len = 0;
  u8                 pdcp_tailer_len = 0;
  u16                pdcp_pdu_size   = 0;
  u16                current_sn      = 0;
  mem_block_t       *pdcp_pdu_p      = NULL;
  rlc_op_status_t    rlc_status;
  rb_id_t            rb_id_rlc       = 0;

  AssertError (enb_mod_idP < NUMBER_OF_eNB_MAX, return FALSE, "eNB id is too high (%u/%d) %u %u!\n", enb_mod_idP, NUMBER_OF_eNB_MAX, ue_mod_idP, rb_idP);
  AssertError (ue_mod_idP < NUMBER_OF_UE_MAX, return FALSE, "UE id is too high (%u/%d) %u %u!\n", ue_mod_idP, NUMBER_OF_UE_MAX, enb_mod_idP, rb_idP);
  AssertError (rb_idP < NB_RB_MAX, return FALSE, "RB id is too high (%u/%d) %u %u!\n", rb_idP, NB_RB_MAX, ue_mod_idP, enb_mod_idP);

  if (enb_flagP == 0) {
      pdcp_p = &pdcp_array_ue[ue_mod_idP][rb_idP];
  } else {
      pdcp_p = &pdcp_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP];
  }

  if ((pdcp_p->instanciated_instance == 0) && (modeP != PDCP_TM)) {
      if (enb_flagP == 0) {
          LOG_W(PDCP, "[UE %d] Instance is not configured for eNB %d, rb_id %d Ignoring SDU...\n",
              ue_mod_idP, enb_mod_idP, rb_idP);
      } else {
          LOG_W(PDCP, "[eNB %d] Instance is not configured for UE %d, rb_id %d Ignoring SDU...\n",
              enb_mod_idP, ue_mod_idP, rb_idP);
      }
      return FALSE;
  }
  if (sdu_buffer_sizeP == 0) {
      LOG_W(PDCP, "Handed SDU is of size 0! Ignoring...\n");
      return FALSE;
  }
  /*
   * XXX MAX_IP_PACKET_SIZE is 4096, shouldn't this be MAX SDU size, which is 8188 bytes?
   */

  if (sdu_buffer_sizeP > MAX_IP_PACKET_SIZE) {
      LOG_E(PDCP, "Requested SDU size (%d) is bigger than that can be handled by PDCP (%u)!\n",
          sdu_buffer_sizeP, MAX_IP_PACKET_SIZE);
      // XXX What does following call do?
      mac_xface->macphy_exit("PDCP sdu buffer size > MAX_IP_PACKET_SIZE");
  }

  // PDCP transparent mode for MBMS traffic 

  if (modeP == PDCP_TM) {
      LOG_D(PDCP, " [TM] Asking for a new mem_block of size %d\n",sdu_buffer_sizeP);
      pdcp_pdu_p = get_free_mem_block(sdu_buffer_sizeP);
      if (pdcp_pdu_p != NULL) {
          memcpy(&pdcp_pdu_p->data[0], sdu_buffer_pP, sdu_buffer_sizeP);
          rlc_status = rlc_data_req(enb_mod_idP, ue_mod_idP, frameP, enb_flagP, RLC_MBMS_YES, rb_idP, muiP, confirmP, sdu_buffer_sizeP, pdcp_pdu_p);
      } else {
        rlc_status = RLC_OP_STATUS_OUT_OF_RESSOURCES;
#if defined(STOP_ON_IP_TRAFFIC_OVERLOAD)
        AssertFatal(0, "[FRAME %5u][%s][PDCP][MOD %u/%u][RB %u] PDCP_DATA_REQ SDU DROPPED, OUT OF MEMORY \n",
            frameP,
            (enb_flagP) ? "eNB" : "UE",
            enb_mod_idP,
            ue_mod_idP,
            rb_idP);
#endif
      }
  } else {
      // calculate the pdcp header and trailer size
      if (rb_idP < DTCH) {
          pdcp_header_len = PDCP_CONTROL_PLANE_DATA_PDU_SN_SIZE;
          pdcp_tailer_len = PDCP_CONTROL_PLANE_DATA_PDU_MAC_I_SIZE;
      } else {
          pdcp_header_len = PDCP_USER_PLANE_DATA_PDU_LONG_SN_HEADER_SIZE;
          pdcp_tailer_len = 0;
      }
      pdcp_pdu_size = sdu_buffer_sizeP + pdcp_header_len + pdcp_tailer_len;

      LOG_I(PDCP, "Data request notification for PDCP entity %s enb id %u ue_id %u and radio bearer ID %d pdu size %d (header%d, trailer%d)\n",
          (enb_flagP) ? "eNB" : "UE",
              enb_mod_idP,
              ue_mod_idP,
              rb_idP,
              pdcp_pdu_size,
              pdcp_header_len,
              pdcp_tailer_len);

      /*
       * Allocate a new block for the new PDU (i.e. PDU header and SDU payload)
       */
      LOG_D(PDCP, "Asking for a new mem_block of size %d\n", pdcp_pdu_size);
      pdcp_pdu_p = get_free_mem_block(pdcp_pdu_size);

      if (pdcp_pdu_p != NULL) {
          /*
           * Create a Data PDU with header and append data
           *
           * Place User Plane PDCP Data PDU header first
           */

          if ((rb_idP % NB_RB_MAX) < DTCH) { // this Control plane PDCP Data PDU
              pdcp_control_plane_data_pdu_header pdu_header;
              pdu_header.sn = pdcp_get_next_tx_seq_number(pdcp_p);
              current_sn = pdu_header.sn;
              memset(&pdu_header.mac_i[0],0,PDCP_CONTROL_PLANE_DATA_PDU_MAC_I_SIZE);
              if (pdcp_serialize_control_plane_data_pdu_with_SRB_sn_buffer((unsigned char*)pdcp_pdu_p->data, &pdu_header) == FALSE) {
                  LOG_E(PDCP, "Cannot fill PDU buffer with relevant header fields!\n");
                  return FALSE;
              }
          } else {
              pdcp_user_plane_data_pdu_header_with_long_sn pdu_header;
              pdu_header.dc = (modeP == 1) ? PDCP_DATA_PDU :  PDCP_CONTROL_PDU;
              pdu_header.sn = pdcp_get_next_tx_seq_number(pdcp_p);
              current_sn = pdu_header.sn ;
              if (pdcp_serialize_user_plane_data_pdu_with_long_sn_buffer((unsigned char*)pdcp_pdu_p->data, &pdu_header) == FALSE) {
                  LOG_E(PDCP, "Cannot fill PDU buffer with relevant header fields!\n");
                  return FALSE;
              }
          }
          /*
           * Validate incoming sequence number, there might be a problem with PDCP initialization
           */
          if (current_sn > pdcp_calculate_max_seq_num_for_given_size(pdcp_p->seq_num_size)) {
              LOG_E(PDCP, "Generated sequence number (%lu) is greater than a sequence number could ever be!\n", current_sn);
              LOG_E(PDCP, "There must be a problem with PDCP initialization, ignoring this PDU...\n");

              free_mem_block(pdcp_pdu_p);
              return FALSE;
          }

          LOG_D(PDCP, "Sequence number %d is assigned to current PDU\n", current_sn);

          /* Then append data... */
          memcpy(&pdcp_pdu_p->data[pdcp_header_len], sdu_buffer_pP, sdu_buffer_sizeP);

          //For control plane data that are not integrity protected,
          // the MAC-I field is still present and should be padded with padding bits set to 0.
          // NOTE: user-plane data are never integrity protected
          for (i=0;i<pdcp_tailer_len;i++)
            pdcp_pdu_p->data[pdcp_header_len + sdu_buffer_sizeP + i] = 0x00;// pdu_header.mac_i[i];

#if defined(ENABLE_SECURITY)
          if ((pdcp->security_activated != 0) &&
              ((pdcp->cipheringAlgorithm) != 0) &&
              ((pdcp->integrityProtAlgorithm) != 0)) {
              pdcp_apply_security(pdcp, rb_id % NB_RB_MAX,
                  pdcp_header_len, current_sn, pdcp_pdu->data,
                  sdu_buffer_size);
          }
#endif

          /* Print octets of outgoing data in hexadecimal form */
          LOG_D(PDCP, "Following content with size %d will be sent over RLC (PDCP PDU header is the first two bytes)\n",
              pdcp_pdu_size);
          //util_print_hex_octets(PDCP, (unsigned char*)pdcp_pdu_p->data, pdcp_pdu_size);
          //util_flush_hex_octets(PDCP, (unsigned char*)pdcp_pdu->data, pdcp_pdu_size);
      } else {
          LOG_E(PDCP, "Cannot create a mem_block for a PDU!\n");
#if defined(STOP_ON_IP_TRAFFIC_OVERLOAD)
        AssertFatal(0, "[FRAME %5u][%s][PDCP][MOD %u/%u][RB %u] PDCP_DATA_REQ SDU DROPPED, OUT OF MEMORY \n",
            frameP,
            (enb_flagP) ? "eNB" : "UE",
            enb_mod_idP,
            ue_mod_idP,
            rb_idP);
#endif
          return FALSE;
      }
      /*
       * Ask sublayer to transmit data and check return value
       * to see if RLC succeeded
       */
      rlc_status = rlc_data_req(enb_mod_idP, ue_mod_idP, frameP, enb_flagP, 0, rb_idP, muiP, confirmP, pdcp_pdu_size, pdcp_pdu_p);
  }
  switch (rlc_status) {
  case RLC_OP_STATUS_OK:
    LOG_D(PDCP, "Data sending request over RLC succeeded!\n");
    break;

  case RLC_OP_STATUS_BAD_PARAMETER:
    LOG_W(PDCP, "Data sending request over RLC failed with 'Bad Parameter' reason!\n");
    return FALSE;

  case RLC_OP_STATUS_INTERNAL_ERROR:
    LOG_W(PDCP, "Data sending request over RLC failed with 'Internal Error' reason!\n");
    return FALSE;

  case RLC_OP_STATUS_OUT_OF_RESSOURCES:
    LOG_W(PDCP, "Data sending request over RLC failed with 'Out of Resources' reason!\n");
    return FALSE;

  default:
    LOG_W(PDCP, "RLC returned an unknown status code after PDCP placed the order to send some data (Status Code:%d)\n", rlc_status);
    return FALSE;
  }

  /*
   * Control arrives here only if rlc_data_req() returns RLC_OP_STATUS_OK
   * so we return TRUE afterwards
   */
  /*  
   if (rb_id>=DTCH) {
    if (eNB_flag == 1) {
      Pdcp_stats_tx[module_id][(rb_id & RAB_OFFSET2 )>> RAB_SHIFT2][(rb_id & RAB_OFFSET)-DTCH]++;
      Pdcp_stats_tx_bytes[module_id][(rb_id & RAB_OFFSET2 )>> RAB_SHIFT2][(rb_id & RAB_OFFSET)-DTCH] += sdu_buffer_size;
    } else {
      Pdcp_stats_tx[module_id][(rb_id & RAB_OFFSET2 )>> RAB_SHIFT2][(rb_id & RAB_OFFSET)-DTCH]++;
      Pdcp_stats_tx_bytes[module_id][(rb_id & RAB_OFFSET2 )>> RAB_SHIFT2][(rb_id & RAB_OFFSET)-DTCH] += sdu_buffer_size;
    }
    }*/
  return TRUE;

}


BOOL pdcp_data_ind(module_id_t enb_mod_idP, module_id_t ue_mod_idP, frame_t frameP, eNB_flag_t enb_flagP, u8_t MBMS_flagP, rb_id_t rb_idP, sdu_size_t sdu_buffer_sizeP, \
    mem_block_t* sdu_buffer_pP, u8 is_data_planeP)
{
  //-----------------------------------------------------------------------------
  pdcp_t      *pdcp_p          = NULL;
  list_t      *sdu_list_p      = NULL;
  mem_block_t *new_sdu_p       = NULL;
  u8           pdcp_header_len = 0;
  u8           pdcp_tailer_len = 0;
  u16          sequence_number = 0;
  u8           payload_offset  = 0;

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
  DevCheck4(rb_idP < NB_RB_MAX, rb_idP, NB_RB_MAX, enb_mod_idP, ue_mod_idP);

  if (enb_flagP == 0) {
      pdcp_p = &pdcp_array_ue[ue_mod_idP][rb_idP];

      LOG_I(PDCP, "Data indication notification for PDCP entity from eNB %u to UE %u "
          "and radio bearer ID %d rlc sdu size %d eNB_flag %d\n",
          ue_mod_idP, enb_mod_idP, rb_idP, sdu_buffer_sizeP, enb_flagP);
  } else {
      pdcp_p = &pdcp_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP];

      LOG_I(PDCP, "Data indication notification for PDCP entity from UE %u to eNB %u "
          "and radio bearer ID %d rlc sdu size %d eNB_flag %d eNB_id %d\n",
          enb_mod_idP, ue_mod_idP, rb_idP, sdu_buffer_sizeP, enb_flagP, enb_mod_idP);
  }
  sdu_list_p = &pdcp_sdu_list;
#

  if (sdu_buffer_sizeP == 0) {
      LOG_W(PDCP, "SDU buffer size is zero! Ignoring this chunk!\n");
      return FALSE;
  }

  /*
   * Check if incoming SDU is long enough to carry a PDU header
   */
  if (MBMS_flagP == 0 ) {
      if ((rb_idP % NB_RB_MAX) < DTCH) {
          pdcp_header_len = PDCP_CONTROL_PLANE_DATA_PDU_SN_SIZE;
          pdcp_tailer_len = PDCP_CONTROL_PLANE_DATA_PDU_MAC_I_SIZE;
      } else {
          pdcp_header_len = PDCP_USER_PLANE_DATA_PDU_LONG_SN_HEADER_SIZE;
          pdcp_tailer_len = 0;
      }

      if (sdu_buffer_sizeP < pdcp_header_len + pdcp_tailer_len ) {
          LOG_W(PDCP, "Incoming (from RLC) SDU is short of size (size:%d)! Ignoring...\n", sdu_buffer_sizeP);
          free_mem_block(sdu_buffer_pP);
          return FALSE;
      }

      /*
       * Parse the PDU placed at the beginning of SDU to check
       * if incoming SN is in line with RX window
       */

      if (pdcp_header_len == PDCP_USER_PLANE_DATA_PDU_LONG_SN_HEADER_SIZE) { // DRB
          sequence_number =     pdcp_get_sequence_number_of_pdu_with_long_sn((unsigned char*)sdu_buffer_pP->data);
          //       u8 dc = pdcp_get_dc_filed((unsigned char*)sdu_buffer->data);
      } else { //SRB1/2
          sequence_number =   pdcp_get_sequence_number_of_pdu_with_SRB_sn((unsigned char*)sdu_buffer_pP->data);
      }
      if (pdcp_is_rx_seq_number_valid(sequence_number, pdcp_p) == TRUE) {
          LOG_D(PDCP, "Incoming PDU has a sequence number (%d) in accordance with RX window\n", sequence_number);
          /* if (dc == PDCP_DATA_PDU )
	   LOG_D(PDCP, "Passing piggybacked SDU to NAS driver...\n");
	   else
	   LOG_D(PDCP, "Passing piggybacked SDU to RRC ...\n");*/
      } else {
          LOG_W(PDCP, "Incoming PDU has an unexpected sequence number (%d), RX window snychronisation have probably been lost!\n", sequence_number);
          /*
           * XXX Till we implement in-sequence delivery and duplicate discarding
           * mechanism all out-of-order packets will be delivered to RRC/IP
           */
#if 0
          LOG_D(PDCP, "Ignoring PDU...\n");
          free_mem_block(sdu_buffer);
          return FALSE;
#else
          LOG_W(PDCP, "Delivering out-of-order SDU to upper layer...\n");
#endif
      }
      // SRB1/2: control-plane data
      if ( (rb_idP % NB_RB_MAX) <  DTCH ){
#if defined(ENABLE_SECURITY)
          if (pdcp->security_activated == 1) {
              pdcp_validate_security(pdcp, rb_id, pdcp_header_len,
                  sequence_number, sdu_buffer->data,
                  sdu_buffer_size - pdcp_tailer_len);
          }
#endif
//rrc_lite_data_ind(module_id, //Modified MW - L2 Interface
          pdcp_rrc_data_ind(enb_mod_idP,
              ue_mod_idP,
              frameP,
              enb_flagP,
              rb_idP,
              sdu_buffer_sizeP - pdcp_header_len - pdcp_tailer_len,
              (u8*)&sdu_buffer_pP->data[pdcp_header_len]);
          free_mem_block(sdu_buffer_pP);
          // free_mem_block(new_sdu);
          return TRUE;
      }
      payload_offset=PDCP_USER_PLANE_DATA_PDU_LONG_SN_HEADER_SIZE;
#if defined(ENABLE_SECURITY)
      if (pdcp->security_activated == 1) {
          pdcp_validate_security(pdcp_p, rb_idP % NB_RB_MAX, pdcp_header_len,
              sequence_number, sdu_buffer->data,
              sdu_buffer_size - pdcp_tailer_len);
      }
#endif
  } else {
      payload_offset=0;
  }
#if defined(USER_MODE) && defined(OAI_EMU)
  if (oai_emulation.info.otg_enabled == 1) {
      module_id_t src_id, dst_id;
      int    ctime;

      src_id = (enb_flagP != 0) ? ue_mod_idP : enb_mod_idP;
      dst_id = (enb_flagP == 0) ? ue_mod_idP : enb_mod_idP;
      ctime = oai_emulation.info.time_ms; // avg current simulation time in ms : we may get the exact time through OCG?
      LOG_D(PDCP, "Check received buffer : enb_flag %d  rab id %d (src %d, dst %d)\n",
          enb_flagP, rb_idP, src_id, dst_id);

      if (otg_rx_pkt(src_id, dst_id,ctime,&sdu_buffer_pP->data[payload_offset],
          sdu_buffer_sizeP - payload_offset ) == 0 ) {
          free_mem_block(sdu_buffer_pP);
          return TRUE;
      }
  }
#else
  if (otg_enabled==1) {
      LOG_D(OTG,"Discarding received packed\n");
      free_mem_block(sdu_buffer_pP);
      return TRUE;
  }
#endif
  new_sdu_p = get_free_mem_block(sdu_buffer_sizeP - payload_offset + sizeof (pdcp_data_ind_header_t));

  if (new_sdu_p) {
      /*
       * Prepend PDCP indication header which is going to be removed at pdcp_fifo_flush_sdus()
       */
      memset(new_sdu_p->data, 0, sizeof (pdcp_data_ind_header_t));
      ((pdcp_data_ind_header_t *) new_sdu_p->data)->data_size = sdu_buffer_sizeP - payload_offset;

      // Here there is no virtualization possible
      // set ((pdcp_data_ind_header_t *) new_sdu_p->data)->inst for IP layer here
      if (enb_flagP == 0) {
          ((pdcp_data_ind_header_t *) new_sdu_p->data)->rb_id = rb_idP;
#if defined(OAI_EMU)
          ((pdcp_data_ind_header_t *) new_sdu_p->data)->inst  = ue_mod_idP + oai_emulation.info.nb_enb_local - oai_emulation.info.first_ue_local;
#endif
      } else {
          ((pdcp_data_ind_header_t *) new_sdu_p->data)->rb_id = rb_idP + (ue_mod_idP * NB_RB_MAX);
#if defined(OAI_EMU)
          ((pdcp_data_ind_header_t *) new_sdu_p->data)->inst  = enb_mod_idP - oai_emulation.info.first_enb_local;
#endif
      }


      // XXX Decompression would be done at this point

      /*
       * After checking incoming sequence number PDCP header
       * has to be stripped off so here we copy SDU buffer starting
       * from its second byte (skipping 0th and 1st octets, i.e.
       * PDCP header)
       */
      memcpy(&new_sdu_p->data[sizeof (pdcp_data_ind_header_t)], \
          &sdu_buffer_pP->data[payload_offset], \
          sdu_buffer_sizeP - payload_offset);
      list_add_tail_eurecom (new_sdu_p, sdu_list_p);

      /* Print octets of incoming data in hexadecimal form */
      LOG_D(PDCP, "Following content has been received from RLC (%d,%d)(PDCP header has already been removed):\n",
          sdu_buffer_sizeP  - payload_offset + sizeof(pdcp_data_ind_header_t),
          sdu_buffer_sizeP  - payload_offset);
      //util_print_hex_octets(PDCP, &new_sdu_p->data[sizeof (pdcp_data_ind_header_t)], sdu_buffer_sizeP - payload_offset);
      //util_flush_hex_octets(PDCP, &new_sdu_p->data[sizeof (pdcp_data_ind_header_t)], sdu_buffer_sizeP - payload_offset);

      /*
       * Update PDCP statistics
       * XXX Following two actions are identical, is there a merge error?
       */

      /*if (enb_flagP == 1) {
      Pdcp_stats_rx[module_id][(rb_idP & RAB_OFFSET2) >> RAB_SHIFT2][(rb_idP & RAB_OFFSET) - DTCH]++;
      Pdcp_stats_rx_bytes[module_id][(rb_idP & RAB_OFFSET2) >> RAB_SHIFT2][(rb_idP & RAB_OFFSET) - DTCH] += sdu_buffer_sizeP;
    } else {
      Pdcp_stats_rx[module_id][(rb_idP & RAB_OFFSET2) >> RAB_SHIFT2][(rb_idP & RAB_OFFSET) - DTCH]++;
      Pdcp_stats_rx_bytes[module_id][(rb_idP & RAB_OFFSET2) >> RAB_SHIFT2][(rb_idP & RAB_OFFSET) - DTCH] += sdu_buffer_sizeP;
    }*/
  }
#if defined(STOP_ON_IP_TRAFFIC_OVERLOAD)
  else {
      AssertFatal(0, "[FRAME %5u][%s][PDCP][MOD %u/%u][RB %u] PDCP_DATA_IND SDU DROPPED, OUT OF MEMORY \n",
            frameP,
            (enb_flagP) ? "eNB" : "UE",
            enb_mod_idP,
            ue_mod_idP,
            rb_idP);
  }
#endif

  free_mem_block(sdu_buffer_pP);

  return TRUE;
}

//-----------------------------------------------------------------------------
void pdcp_run (frame_t frameP, eNB_flag_t  eNB_flag, module_id_t ue_mod_idP, module_id_t enb_mod_idP) {
  //-----------------------------------------------------------------------------
#if defined(ENABLE_ITTI)
  MessageDef   *msg_p;
  const char   *msg_name;
  instance_t    instance;
  int           result;
#endif

  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_PDCP_RUN, VCD_FUNCTION_IN);

#if defined(ENABLE_ITTI)
  do {
      // Checks if a message has been sent to PDCP sub-task
      itti_poll_msg (eNB_flag ? TASK_PDCP_ENB : TASK_PDCP_UE, &msg_p);

      if (msg_p != NULL) {
          msg_name = ITTI_MSG_NAME (msg_p);
          instance = ITTI_MSG_INSTANCE (msg_p);

          switch (ITTI_MSG_ID(msg_p)) {
          case RRC_DCCH_DATA_REQ:
            LOG_I(PDCP, "Received %s from %s: instance %d, frame %d, eNB_flag %d, rb_id %d, muiP %d, confirmP %d, mode %d\n",
                msg_name, ITTI_MSG_ORIGIN_NAME(msg_p), instance,
                RRC_DCCH_DATA_REQ (msg_p).frame, RRC_DCCH_DATA_REQ (msg_p).enb_flag, RRC_DCCH_DATA_REQ (msg_p).rb_id,
                RRC_DCCH_DATA_REQ (msg_p).muip, RRC_DCCH_DATA_REQ (msg_p).confirmp, RRC_DCCH_DATA_REQ (msg_p).mode);

            result = pdcp_data_req (RRC_DCCH_DATA_REQ (msg_p).eNB_index, RRC_DCCH_DATA_REQ (msg_p).ue_index, RRC_DCCH_DATA_REQ (msg_p).frame, RRC_DCCH_DATA_REQ (msg_p).enb_flag,
                RRC_DCCH_DATA_REQ (msg_p).rb_id, RRC_DCCH_DATA_REQ (msg_p).muip,
                RRC_DCCH_DATA_REQ (msg_p).confirmp, RRC_DCCH_DATA_REQ (msg_p).sdu_size,
                RRC_DCCH_DATA_REQ (msg_p).sdu_p, RRC_DCCH_DATA_REQ (msg_p).mode);
            AssertFatal (result == TRUE, "PDCP data request failed!\n");

            // Message buffer has been processed, free it now.
            result = itti_free (ITTI_MSG_ORIGIN_ID(msg_p), RRC_DCCH_DATA_REQ (msg_p).sdu_p);
            AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
            break;

          default:
            LOG_E(PDCP, "Received unexpected message %s\n", msg_name);
            break;
          }

          result = itti_free (ITTI_MSG_ORIGIN_ID(msg_p), msg_p);
          AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
      }
  } while(msg_p != NULL);

# if 0
  {
      MessageDef *msg_resp_p;

      msg_resp_p = itti_alloc_new_message(TASK_PDCP_ENB, MESSAGE_TEST);

      itti_send_msg_to_task(TASK_RRC_ENB, 1, msg_resp_p);
  }
  {
    MessageDef *msg_resp_p;

    msg_resp_p = itti_alloc_new_message(TASK_PDCP_ENB, MESSAGE_TEST);

    itti_send_msg_to_task(TASK_ENB_APP, 2, msg_resp_p);
  }
  {
    MessageDef *msg_resp_p;

    msg_resp_p = itti_alloc_new_message(TASK_PDCP_ENB, MESSAGE_TEST);

    itti_send_msg_to_task(TASK_MAC_ENB, 3, msg_resp_p);
  }
# endif
#endif

  pdcp_fifo_read_input_sdus_from_otg(frameP, eNB_flag, ue_mod_idP, enb_mod_idP);

  // IP/NAS -> PDCP traffic : TX, read the pkt from the upper layer buffer
  pdcp_fifo_read_input_sdus(frameP, eNB_flag, ue_mod_idP, enb_mod_idP);

  // PDCP -> NAS/IP traffic: RX
  pdcp_fifo_flush_sdus(frameP, eNB_flag, enb_mod_idP, ue_mod_idP);

  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_PDCP_RUN, VCD_FUNCTION_OUT);
}

BOOL rrc_pdcp_config_asn1_req (module_id_t               enb_mod_idP,
    module_id_t               ue_mod_idP,
    frame_t              frameP,
    eNB_flag_t           enb_flagP,
    SRB_ToAddModList_t  *srb2add_list_pP,
    DRB_ToAddModList_t  *drb2add_list_pP,
    DRB_ToReleaseList_t *drb2release_list_pP,
    u8                   security_modeP,
    u8                  *kRRCenc_pP,
    u8                  *kRRCint_pP,
    u8                  *kUPenc_pP
#ifdef Rel10
,PMCH_InfoList_r9_t*  pmch_InfoList_r9_pP
#endif
)
{
  long int        rb_id          = 0;
  long int        lc_id          = 0;
  long int        srb_id         = 0;
  long int        mch_id         = 0;
  rlc_mode_t      rlc_type       = RLC_NONE;
  DRB_Identity_t  drb_id         = 0;
  DRB_Identity_t *pdrb_id_p      = NULL;
  u8              drb_sn         = 0;
  u8              srb_sn         = 5; // fixed sn for SRBs
  u8              drb_report     = 0;
  long int        cnt            = 0;
  u16 header_compression_profile = 0;
  u32 action                     = ACTION_ADD;
  SRB_ToAddMod_t *srb_toaddmod_p = NULL;
  DRB_ToAddMod_t *drb_toaddmod_p = NULL;
  pdcp_t         *pdcp_p         = NULL;
  module_id_t     module_id      = -1;

#ifdef Rel10
  int i,j;
  MBMS_SessionInfoList_r9_t *mbms_SessionInfoList_r9_p = NULL;
  MBMS_SessionInfo_r9_t     *MBMS_SessionInfo_p        = NULL;
#endif

  if (enb_flagP == 0) {
      LOG_D(PDCP, "[UE %d] CONFIG REQ ASN1 for eNB %d\n", ue_mod_idP, enb_mod_idP);
  } else {
      LOG_D(PDCP, "[eNB %d] CONFIG REQ ASN1 for UE %d\n", enb_mod_idP, ue_mod_idP);
  }
  // srb2add_list does not define pdcp config, we use rlc info to setup the pdcp dcch0 and dcch1 channels

  if (srb2add_list_pP != NULL) {
      for (cnt=0;cnt<srb2add_list_pP->list.count;cnt++) {
          srb_id = srb2add_list_pP->list.array[cnt]->srb_Identity;
          srb_toaddmod_p = srb2add_list_pP->list.array[cnt];
          rlc_type = RLC_MODE_AM;
          rb_id = srb_id;
          lc_id = srb_id;

          if (enb_flagP == 0) {
              pdcp_p = &pdcp_array_ue[ue_mod_idP][srb_id];
          } else {
              pdcp_p = &pdcp_array_eNB[enb_mod_idP][ue_mod_idP][srb_id];
          }

          if (pdcp_p->instanciated_instance == 1) {
              action = ACTION_MODIFY;
          } else {
              action = ACTION_ADD;
          }

          if (srb_toaddmod_p->rlc_Config) {
              switch (srb_toaddmod_p->rlc_Config->present) {
              case SRB_ToAddMod__rlc_Config_PR_NOTHING:
                break;
              case SRB_ToAddMod__rlc_Config_PR_explicitValue:
                switch (srb_toaddmod_p->rlc_Config->choice.explicitValue.present) {
                case RLC_Config_PR_NOTHING:
                  break;
                default:
                  pdcp_config_req_asn1 (pdcp_p,
                      enb_mod_idP,
                      ue_mod_idP,
                      frameP,
                      enb_flagP, // not really required
                      rlc_type,
                      action,
                      lc_id,
                      mch_id,
                      rb_id,
                      srb_sn,
                      0, // drb_report
                      0, // header compression
                      security_modeP,
                      kRRCenc_pP,
                      kRRCint_pP,
                      kUPenc_pP);
                  break;
                }
                break;
                case SRB_ToAddMod__rlc_Config_PR_defaultValue:
                  // already the default values
                  break;
                default:
                  DevParam(srb_toaddmod_p->rlc_Config->present, ue_mod_idP, enb_mod_idP);
                  break;
              }
          }
      }
  }
  // reset the action

  if (drb2add_list_pP != NULL) {
      for (cnt=0;cnt<drb2add_list_pP->list.count;cnt++) {

          drb_toaddmod_p = drb2add_list_pP->list.array[cnt];

          drb_id = drb_toaddmod_p->drb_Identity;

          if (drb_toaddmod_p->logicalChannelIdentity != null) {
              lc_id = *drb_toaddmod_p->logicalChannelIdentity;
          } else {
              lc_id = -1;
          }
          rb_id = lc_id;

          DevCheck4(rb_id < NB_RB_MAX, rb_id, NB_RB_MAX, ue_mod_idP, enb_mod_idP);

          if (enb_flagP == 0) {
              pdcp_p = &pdcp_array_ue[ue_mod_idP][rb_id];
          } else {
              pdcp_p = &pdcp_array_eNB[enb_mod_idP][ue_mod_idP][rb_id];
          }

          if (pdcp_p->instanciated_instance == 1)
            action = ACTION_MODIFY;
          else
            action = ACTION_ADD;

          if (drb_toaddmod_p->pdcp_Config){
              if (drb_toaddmod_p->pdcp_Config->discardTimer) {
                  // set the value of the timer
              }
              if (drb_toaddmod_p->pdcp_Config->rlc_AM) {
                  drb_report = drb_toaddmod_p->pdcp_Config->rlc_AM->statusReportRequired;
                  rlc_type =RLC_MODE_AM;
              }
              if (drb_toaddmod_p->pdcp_Config->rlc_UM){
                  drb_sn = drb_toaddmod_p->pdcp_Config->rlc_UM->pdcp_SN_Size;
                  rlc_type =RLC_MODE_UM;
              }
              switch (drb_toaddmod_p->pdcp_Config->headerCompression.present) {
              case PDCP_Config__headerCompression_PR_NOTHING:
              case PDCP_Config__headerCompression_PR_notUsed:
                header_compression_profile=0x0;
                break;
              case PDCP_Config__headerCompression_PR_rohc:
                // parse the struc and get the rohc profile
                if(drb_toaddmod_p->pdcp_Config->headerCompression.choice.rohc.profiles.profile0x0001)
                  header_compression_profile=0x0001;
                else if(drb_toaddmod_p->pdcp_Config->headerCompression.choice.rohc.profiles.profile0x0002)
                  header_compression_profile=0x0002;
                else if(drb_toaddmod_p->pdcp_Config->headerCompression.choice.rohc.profiles.profile0x0003)
                  header_compression_profile=0x0003;
                else if(drb_toaddmod_p->pdcp_Config->headerCompression.choice.rohc.profiles.profile0x0004)
                  header_compression_profile=0x0004;
                else if(drb_toaddmod_p->pdcp_Config->headerCompression.choice.rohc.profiles.profile0x0006)
                  header_compression_profile=0x0006;
                else if(drb_toaddmod_p->pdcp_Config->headerCompression.choice.rohc.profiles.profile0x0101)
                  header_compression_profile=0x0101;
                else if(drb_toaddmod_p->pdcp_Config->headerCompression.choice.rohc.profiles.profile0x0102)
                  header_compression_profile=0x0102;
                else if(drb_toaddmod_p->pdcp_Config->headerCompression.choice.rohc.profiles.profile0x0103)
                  header_compression_profile=0x0103;
                else if(drb_toaddmod_p->pdcp_Config->headerCompression.choice.rohc.profiles.profile0x0104)
                  header_compression_profile=0x0104;
                else {
                    header_compression_profile=0x0;
                    LOG_W(PDCP,"unknown header compresion profile\n");
                }
                // set the applicable profile
                break;
              default:
                LOG_W(PDCP,"[MOD_id %d][RB %d] unknown drb_toaddmod->PDCP_Config->headerCompression->present \n",
                    module_id, drb_id);
                break;
              }
              pdcp_config_req_asn1 (pdcp_p,
                  enb_mod_idP,
                  ue_mod_idP,
                  frameP,
                  enb_flagP, // not really required
                  rlc_type,
                  action,
                  lc_id,
                  mch_id,
                  rb_id,
                  drb_sn,
                  drb_report,
                  header_compression_profile,
                  security_modeP,
                  kRRCenc_pP,
                  kRRCint_pP,
                  kUPenc_pP);
          }
      }
  }

  if (drb2release_list_pP != NULL) {
      for (cnt=0;cnt<drb2release_list_pP->list.count;cnt++) {
          pdrb_id_p = drb2release_list_pP->list.array[cnt];
          rb_id =  *pdrb_id_p;
          if (enb_flagP == 0) {
              pdcp_p = &pdcp_array_ue[ue_mod_idP][rb_id];
          } else {
              pdcp_p = &pdcp_array_eNB[enb_mod_idP][ue_mod_idP][rb_id];
          }
          action = ACTION_REMOVE;
          pdcp_config_req_asn1 (pdcp_p,
              enb_mod_idP,
              ue_mod_idP,
              frameP,
              enb_flagP, // not really required
              rlc_type,
              action,
              lc_id,
              mch_id,
              rb_id,
              0,
              0,
              0,
              security_modeP,
              kRRCenc_pP,
              kRRCint_pP,
              kUPenc_pP);
      }
  }

#ifdef Rel10
  if (pmch_InfoList_r9 != NULL) {
      for (i=0;i<pmch_InfoList_r9->list.count;i++) {
          mbms_SessionInfoList_r9 = &(pmch_InfoList_r9->list.array[i]->mbms_SessionInfoList_r9);
          for (j=0;j<mbms_SessionInfoList_r9->list.count;j++) {
              MBMS_SessionInfo = mbms_SessionInfoList_r9->list.array[j];
              //lc_id = MBMS_SessionInfo->logicalChannelIdentity_r9; // lcid
              lc_id = MBMS_SessionInfo->sessionId_r9->buf[0];
              mch_id = MBMS_SessionInfo->tmgi_r9.serviceId_r9.buf[2]; //serviceId is 3-octet string

              // can set the mch_id = i
              if (eNB_flag)
                rb_id =  (mch_id * maxSessionPerPMCH ) + lc_id;
              else
                rb_id =  (mch_id * maxSessionPerPMCH ) + lc_id + (maxDRB + 3);

              if (pdcp_mbms_array[module_id][rb_id].instanciated_instance == module_id + 1)
                action = ACTION_MBMS_MODIFY;
              else
                action = ACTION_MBMS_ADD;

              rlc_type = RLC_MODE_UM;
              pdcp_config_req_asn1 (NULL,
                  enb_mod_idP,
                  ue_mod_idP,
                  frameP,
                  eNB_flag,
                  rlc_type,
                  action,
                  lc_id,
                  mch_id,
                  rb_id,
                  0, // set to deafult
                  0,
                  0,
                  security_mode,
                  kRRCenc,
                  kRRCint,
                  kUPenc);
          }
      }
  }
#endif

  return 1;

}

BOOL pdcp_config_req_asn1 (pdcp_t   *pdcp_pP,
    module_id_t     enb_mod_idP,
    module_id_t     ue_mod_idP,
    frame_t    frameP,
    eNB_flag_t enb_flagP,
    rlc_mode_t rlc_modeP,
    u32        actionP,
    u16        lc_idP,
    u16        mch_idP,
    rb_id_t    rb_idP,
    u8         rb_snP,
    u8         rb_reportP,
    u16        header_compression_profileP,
    u8         security_modeP,
    u8        *kRRCenc_pP,
    u8        *kRRCint_pP,
    u8        *kUPenc_pP)
{

  switch (actionP) {
  case ACTION_ADD:
    DevAssert(pdcp_pP != NULL);
    pdcp_pP->instanciated_instance = 1;
    pdcp_pP->is_ue = (enb_flagP == 0) ? 1 : 0;
    pdcp_pP->lcid = lc_idP;
    pdcp_pP->header_compression_profile = header_compression_profileP;
    pdcp_pP->status_report = rb_reportP;

    if (rb_snP == PDCP_Config__rlc_UM__pdcp_SN_Size_len7bits) {
        pdcp_pP->seq_num_size = 7;
    } else if (rb_snP == PDCP_Config__rlc_UM__pdcp_SN_Size_len12bits) {
        pdcp_pP->seq_num_size=12;
    } else {
        pdcp_pP->seq_num_size=5;
    }

    pdcp_pP->rlc_mode = rlc_modeP;
    pdcp_pP->next_pdcp_tx_sn = 0;
    pdcp_pP->next_pdcp_rx_sn = 0;
    pdcp_pP->tx_hfn = 0;
    pdcp_pP->rx_hfn = 0;
    pdcp_pP->last_submitted_pdcp_rx_sn = 4095;
    pdcp_pP->first_missing_pdu = -1;

    if (enb_flagP == 0) {
        LOG_I(PDCP, "[UE %d] Config request : Action ADD for eNB %d: Frame %d LCID %d (rb id %d) "
            "configured with SN size %d bits and RLC %s\n",
            ue_mod_idP, enb_mod_idP, frameP, lc_idP, rb_idP, pdcp_pP->seq_num_size,
            (rlc_modeP == 1) ? "AM" : (rlc_modeP == 2) ? "TM" : "UM");
    } else {
        LOG_I(PDCP, "[eNB %d] Config request : Action ADD for UE %d: Frame %d LCID %d (rb id %d) "
            "configured with SN size %d bits and RLC %s\n",
            enb_mod_idP, ue_mod_idP, frameP, lc_idP, rb_idP, pdcp_pP->seq_num_size,
            (rlc_modeP == 1) ? "AM" : (rlc_modeP == 2) ? "TM" : "UM");
    }

    /* Setup security */
    if (security_modeP != 0xff) {
        pdcp_config_set_security(pdcp_pP, enb_mod_idP, ue_mod_idP, frameP, enb_flagP, rb_idP, lc_idP, security_modeP, kRRCenc_pP, kRRCint_pP, kUPenc_pP);
    }

    LOG_D(PDCP, "[FRAME %5u][%s][PDCP][MOD %u/%u][RB %u]\n", frameP, (enb_flagP == 0) ? "UE" : "eNB",  enb_mod_idP, ue_mod_idP, rb_idP);
    break;

  case ACTION_MODIFY:
    DevAssert(pdcp_pP != NULL);
    pdcp_pP->header_compression_profile=header_compression_profileP;
    pdcp_pP->status_report = rb_reportP;
    pdcp_pP->rlc_mode = rlc_modeP;

    /* Setup security */
    if (security_modeP != 0xff) {
        pdcp_config_set_security(pdcp_pP, enb_mod_idP, ue_mod_idP, frameP, enb_flagP, rb_idP, lc_idP, security_modeP, kRRCenc_pP, kRRCint_pP, kUPenc_pP);
    }

    if (rb_snP == PDCP_Config__rlc_UM__pdcp_SN_Size_len7bits) {
        pdcp_pP->seq_num_size = 7;
    } else if (rb_snP == PDCP_Config__rlc_UM__pdcp_SN_Size_len12bits) {
        pdcp_pP->seq_num_size = 12;
    } else {
        pdcp_pP->seq_num_size=5;
    }

    if (enb_flagP == 0) {
        LOG_I(PDCP,"[UE %d] Config request : Action MODIFY for eNB %d: Frame %d LCID %d "
            "RB id %d configured with SN size %d and RLC %s \n",
            ue_mod_idP, enb_mod_idP, frameP, lc_idP, rb_idP, rb_snP,
            (rlc_modeP == 1) ? "AM" : (rlc_modeP == 2) ? "TM" : "UM");
    } else {
        LOG_I(PDCP,"[eNB %d] Config request : Action MODIFY for UE %d: Frame %d LCID %d "
            "RB id %d configured with SN size %d and RLC %s \n",
            enb_mod_idP, ue_mod_idP, frameP, lc_idP, rb_idP, rb_snP,
            (rlc_modeP == 1) ? "AM" : (rlc_modeP == 2) ? "TM" : "UM");
    }
    break;
  case ACTION_REMOVE:
    DevAssert(pdcp_pP != NULL);
    pdcp_pP->instanciated_instance = 0;
    pdcp_pP->lcid = 0;
    pdcp_pP->header_compression_profile = 0x0;
    pdcp_pP->cipheringAlgorithm = 0xff;
    pdcp_pP->integrityProtAlgorithm = 0xff;
    pdcp_pP->status_report = 0;
    pdcp_pP->rlc_mode = RLC_NONE;
    pdcp_pP->next_pdcp_tx_sn = 0;
    pdcp_pP->next_pdcp_rx_sn = 0;
    pdcp_pP->tx_hfn = 0;
    pdcp_pP->rx_hfn = 0;
    pdcp_pP->last_submitted_pdcp_rx_sn = 4095;
    pdcp_pP->seq_num_size = 0;
    pdcp_pP->first_missing_pdu = -1;
    pdcp_pP->security_activated = 0;

    if (enb_flagP == 0) {
        LOG_I(PDCP, "[UE %d] Config request : ACTION_REMOVE for eNB %d: Frame %d LCID %d RBID %d configured\n",
            ue_mod_idP, enb_mod_idP, frameP, lc_idP, rb_idP);
    } else {
        LOG_I(PDCP, "[eNB %d] Config request : ACTION_REMOVE for UE %d: Frame %d LCID %d RBID %d configured\n",
            enb_mod_idP, ue_mod_idP, frameP, lc_idP, rb_idP);
    }
    /* Security keys */
    if (pdcp_pP->kUPenc != NULL) {
        free(pdcp_pP->kUPenc);
    }
    if (pdcp_pP->kRRCint != NULL) {
        free(pdcp_pP->kRRCint);
    }
    if (pdcp_pP->kRRCenc != NULL) {
        free(pdcp_pP->kRRCenc);
    }
    break;
  case ACTION_MBMS_ADD:
  case ACTION_MBMS_MODIFY:
    if (enb_flagP == 0) {
        LOG_I(PDCP,"[UE %d] Config request for eNB %d: %s: Frame %d service_id/mch index %d, session_id/lcid %d, rbid %d configured\n",
            ue_mod_idP, enb_mod_idP, actionP == ACTION_MBMS_ADD ? "ACTION_MBMS_ADD" : "ACTION_MBMS_MODIFY", frameP, mch_idP, lc_idP, rb_idP);
    } else {
        LOG_I(PDCP,"[eNB %d] Config request for UE %d: %s: Frame %d service_id/mch index %d, session_id/lcid %d, rbid %d configured\n",
            enb_mod_idP, ue_mod_idP, actionP == ACTION_MBMS_ADD ? "ACTION_MBMS_ADD" : "ACTION_MBMS_MODIFY", frameP, mch_idP, lc_idP, rb_idP);
    }
    if (enb_flagP == 1) {
        pdcp_mbms_array_eNB[enb_mod_idP][rb_idP].instanciated_instance = 1 ;
        pdcp_mbms_array_eNB[enb_mod_idP][rb_idP].service_id = mch_idP;
        pdcp_mbms_array_eNB[enb_mod_idP][rb_idP].session_id = lc_idP;
        pdcp_mbms_array_eNB[enb_mod_idP][rb_idP].rb_id = rb_idP;
    } else {
        pdcp_mbms_array_eNB[ue_mod_idP][rb_idP].instanciated_instance = 1 ;
        pdcp_mbms_array_eNB[ue_mod_idP][rb_idP].service_id = mch_idP;
        pdcp_mbms_array_eNB[ue_mod_idP][rb_idP].session_id = lc_idP;
        pdcp_mbms_array_eNB[ue_mod_idP][rb_idP].rb_id = rb_idP;
    }
    break;
  case ACTION_SET_SECURITY_MODE:
    pdcp_config_set_security(pdcp_pP, enb_mod_idP, ue_mod_idP, frameP, enb_flagP, rb_idP, lc_idP, security_modeP, kRRCenc_pP, kRRCint_pP, kUPenc_pP);
    break;
  default:
    DevParam(actionP, enb_mod_idP, ue_mod_idP);
    break;
  }
  return 0;
}

void pdcp_config_set_security(pdcp_t    *pdcp_pP,
    module_id_t     enb_mod_idP,
    module_id_t     ue_mod_idP,
    frame_t    frameP,
    eNB_flag_t eNB_flag,
    rb_id_t    rb_idP,
    u16        lc_idP,
    u8         security_modeP,
    u8        *kRRCenc,
    u8        *kRRCint,
    u8        *kUPenc)
{
  DevAssert(pdcp_pP != NULL);

  if ((security_modeP >= 0) && (security_modeP <= 0x77)) {
      pdcp_pP->cipheringAlgorithm     = security_modeP & 0x0f;
      pdcp_pP->integrityProtAlgorithm = (security_modeP>>4) & 0xf;

      if (eNB_flag == 0) {
          LOG_D(PDCP,"[UE %d][RB %02d] Set security mode : ACTION_SET_SECURITY_MODE: "
              "Frame %d  cipheringAlgorithm %d integrityProtAlgorithm %d\n",
              ue_mod_idP, rb_idP, frameP, pdcp_pP->cipheringAlgorithm, pdcp_pP->integrityProtAlgorithm);
      } else {
          LOG_D(PDCP,"[eNB %d][UE %d][RB %02d] Set security mode : ACTION_SET_SECURITY_MODE: "
              "Frame %d  cipheringAlgorithm %d integrityProtAlgorithm %d\n",
              enb_mod_idP, ue_mod_idP, rb_idP, frameP, pdcp_pP->cipheringAlgorithm, pdcp_pP->integrityProtAlgorithm);
      }
      pdcp_pP->kRRCenc = kRRCenc;
      pdcp_pP->kRRCint = kRRCint;
      pdcp_pP->kUPenc  = kUPenc;

      /* Activate security */
      pdcp_pP->security_activated = 1;
  } else {
      LOG_E(PDCP,"[%s %d] bad security mode %d", security_modeP);
  }
}

void rrc_pdcp_config_req (module_id_t enb_mod_idP, module_id_t ue_mod_idP, frame_t frameP, eNB_flag_t enb_flagP, u32 actionP, rb_id_t rb_idP, u8 security_modeP)
{
  pdcp_t *pdcp_p = NULL;

  if (enb_mod_idP == 0) {
      pdcp_p = &pdcp_array_ue[ue_mod_idP][rb_idP];
  } else {
      pdcp_p = &pdcp_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP];
  }

  /*
   * Initialize sequence number state variables of relevant PDCP entity
   */
  switch (actionP) {
  case ACTION_ADD:
    pdcp_p->instanciated_instance = 1;

    pdcp_p->next_pdcp_tx_sn = 0;
    pdcp_p->next_pdcp_rx_sn = 0;
    pdcp_p->tx_hfn = 0;
    pdcp_p->rx_hfn = 0;
    /* SN of the last PDCP SDU delivered to upper layers */
    pdcp_p->last_submitted_pdcp_rx_sn = 4095;

    if (rb_idP < DTCH) { // SRB
        pdcp_p->seq_num_size = 5;
    } else { // DRB
        pdcp_p->seq_num_size = 12;
    }
    pdcp_p->first_missing_pdu = -1;
    LOG_D(PDCP,"[%s %d] Config request : Action ADD: Frame %d radio bearer id %d configured\n",
        (enb_flagP) ? "eNB" : "UE", (enb_flagP) ? enb_mod_idP : ue_mod_idP, frameP, rb_idP);
    break;
  case ACTION_MODIFY:
    break;
  case ACTION_REMOVE:
    pdcp_p->instanciated_instance = 0;
    pdcp_p->next_pdcp_tx_sn = 0;
    pdcp_p->next_pdcp_rx_sn = 0;
    pdcp_p->tx_hfn = 0;
    pdcp_p->rx_hfn = 0;
    pdcp_p->last_submitted_pdcp_rx_sn = 4095;
    pdcp_p->seq_num_size = 0;
    pdcp_p->first_missing_pdu = -1;
    pdcp_p->security_activated = 0;
    LOG_D(PDCP,"[%s %d] Config request : ACTION_REMOVE: Frame %d radio bearer id %d configured\n",
        (enb_flagP) ? "eNB" : "UE",  (enb_flagP) ? enb_mod_idP : ue_mod_idP, frameP, rb_idP);

    break;
  case ACTION_SET_SECURITY_MODE:
    if ((security_modeP >= 0) && (security_modeP <= 0x77)) {
        pdcp_p->cipheringAlgorithm= security_modeP & 0x0f;
        pdcp_p->integrityProtAlgorithm = (security_modeP>>4) & 0xf;
        LOG_D(PDCP,"[%s %d]Set security mode : ACTION_SET_SECURITY_MODE: Frame %d  cipheringAlgorithm %d integrityProtAlgorithm %d\n",
            (enb_flagP) ? "eNB" : "UE",  (enb_flagP) ? enb_mod_idP : ue_mod_idP, frameP,
                pdcp_p->cipheringAlgorithm,
                pdcp_p->integrityProtAlgorithm );
    } else {
        LOG_D(PDCP,"[%s %d] bad security mode %d", security_modeP);
    }
    break;
  default:
    DevParam(actionP, ue_mod_idP, enb_mod_idP);
    break;
  }
}

// TODO PDCP module initialization code might be removed
int pdcp_module_init (void) {
  //-----------------------------------------------------------------------------
#ifdef NAS_FIFO
  int ret;

  ret=rtf_create(PDCP2NAS_FIFO,32768);

  if (ret < 0) {
      LOG_E(PDCP, "Cannot create PDCP2NAS fifo %d (ERROR %d)\n", PDCP2NAS_FIFO, ret);

      return -1;
  } else {
      LOG_D(PDCP, "Created PDCP2NAS fifo %d\n", PDCP2NAS_FIFO);
      rtf_reset(PDCP2NAS_FIFO);
  }

  ret=rtf_create(NAS2PDCP_FIFO,32768);

  if (ret < 0) {
      LOG_E(PDCP, "Cannot create NAS2PDCP fifo %d (ERROR %d)\n", NAS2PDCP_FIFO, ret);

      return -1;
  } else {
      LOG_D(PDCP, "Created NAS2PDCP fifo %d\n", NAS2PDCP_FIFO);
      rtf_reset(NAS2PDCP_FIFO);
  }

  pdcp_2_nas_irq = 0;
  pdcp_input_sdu_remaining_size_to_read=0;
  pdcp_input_sdu_size_read=0;
#endif

  return 0;

}

//-----------------------------------------------------------------------------
void pdcp_module_cleanup (void)
//-----------------------------------------------------------------------------
{
#ifdef NAS_FIFO
  rtf_destroy(NAS2PDCP_FIFO);
  rtf_destroy(PDCP2NAS_FIFO);
#endif
}

//-----------------------------------------------------------------------------
void pdcp_layer_init(void)
{
  //-----------------------------------------------------------------------------
  module_id_t  instance, instance2;
  rb_id_t rb_id;
  int     j;
  /*
   * Initialize SDU list
   */
  list_init(&pdcp_sdu_list, NULL);

  for (instance = 0; instance < NUMBER_OF_UE_MAX; instance++) {
      for (rb_id = 0; rb_id < NB_RB_MAX; rb_id++) {
          memset(&pdcp_array_ue[instance][rb_id], 0, sizeof(pdcp_t));
      }
      for (j = 0; j < 16*29; j++) {
          memset(&pdcp_mbms_array_ue[instance][j], 0, sizeof(pdcp_mbms_t));
      }
  }
  for (instance = 0; instance < NUMBER_OF_eNB_MAX; instance++) {
      for (instance2 = 0; instance2 < NUMBER_OF_UE_MAX; instance2++) {
          for (rb_id = 0; rb_id < NB_RB_MAX; rb_id++) {
              memset(&pdcp_array_eNB[instance][instance2][rb_id], 0, sizeof(pdcp_t));
          }
      }
      for (j = 0; j < 16*29; j++) {
          memset(&pdcp_mbms_array_eNB[instance][j], 0, sizeof(pdcp_mbms_t));
      }
  }

  LOG_I(PDCP, "PDCP layer has been initialized\n");

  pdcp_output_sdu_bytes_to_write=0;
  pdcp_output_header_bytes_to_write=0;
  pdcp_input_sdu_remaining_size_to_read=0;

  memset(Pdcp_stats_tx, 0, sizeof(Pdcp_stats_tx));
  memset(Pdcp_stats_tx_bytes, 0, sizeof(Pdcp_stats_tx_bytes));
  memset(Pdcp_stats_tx_bytes_last, 0, sizeof(Pdcp_stats_tx_bytes_last));
  memset(Pdcp_stats_tx_rate, 0, sizeof(Pdcp_stats_tx_rate));

  memset(Pdcp_stats_rx, 0, sizeof(Pdcp_stats_rx));
  memset(Pdcp_stats_rx_bytes, 0, sizeof(Pdcp_stats_rx_bytes));
  memset(Pdcp_stats_rx_bytes_last, 0, sizeof(Pdcp_stats_rx_bytes_last));
  memset(Pdcp_stats_rx_rate, 0, sizeof(Pdcp_stats_rx_rate));
}

//-----------------------------------------------------------------------------
void pdcp_layer_cleanup (void)
//-----------------------------------------------------------------------------
{
  list_free (&pdcp_sdu_list);
}

#ifdef NAS_FIFO
EXPORT_SYMBOL(pdcp_2_nas_irq);
#endif //NAS_FIFO

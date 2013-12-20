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

#include "assertions.h"

#define PDCP_DATA_REQ_DEBUG 0
#define PDCP_DATA_IND_DEBUG 0

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
#ifdef PDCP_UNIT_TEST
BOOL pdcp_data_req(module_id_t module_id, u32_t frame, u8_t eNB_flag, rb_id_t rb_id, sdu_size_t sdu_buffer_size, \
                   unsigned char* sdu_buffer, pdcp_t* test_pdcp_entity, list_t* test_list)
#else
BOOL pdcp_data_req(u8 eNB_id, u8 UE_id, u32_t frame, u8_t eNB_flag, rb_id_t rb_id, u32 muiP, u32 confirmP,
                   sdu_size_t sdu_buffer_size, unsigned char* sdu_buffer, u8 mode)
#endif
{
  //-----------------------------------------------------------------------------
  pdcp_t* pdcp;
  u8 i;
  u8 pdcp_header_len=0, pdcp_tailer_len=0;
  u16 pdcp_pdu_size=0, current_sn;
  mem_block_t* pdcp_pdu = NULL;
  rlc_op_status_t rlc_status;
  module_id_t module_id;
  rb_id_t rb_id_rlc = 0;

  DevCheck4(eNB_id < NUMBER_OF_eNB_MAX, eNB_id, NUMBER_OF_eNB_MAX, UE_id, rb_id);
  DevCheck4(UE_id < NUMBER_OF_UE_MAX, UE_id, NUMBER_OF_UE_MAX, eNB_id, rb_id);
  DevCheck4(rb_id < NB_RB_MAX, rb_id, NB_RB_MAX, UE_id, eNB_id);

#ifdef PDCP_UNIT_TEST
  pdcp = test_pdcp_entity;
#else
  if (eNB_flag == 0) {
    pdcp = &pdcp_array_ue[UE_id][rb_id];
    module_id = NB_eNB_INST + UE_id;
    rb_id_rlc = rb_id;
  } else {
    pdcp = &pdcp_array_eNB[eNB_id][UE_id][rb_id];
    module_id = eNB_id;
    rb_id_rlc = rb_id + (NB_RB_MAX * UE_id);
  }
#endif

  if ((pdcp->instanciated_instance == 0) && (mode != PDCP_TM)) {
    if (eNB_flag == 0) {
      LOG_W(PDCP, "[UE %d] Instance is not configured for eNB %d, rb_id %d Ignoring SDU...\n",
          UE_id, eNB_id, rb_id);
    } else {
      LOG_W(PDCP, "[eNB %d] Instance is not configured for UE %d, rb_id %d Ignoring SDU...\n",
          eNB_id, UE_id, rb_id);
    }
    return FALSE;
  }
  if (sdu_buffer_size == 0) {
    LOG_W(PDCP, "Handed SDU is of size 0! Ignoring...\n");
    return FALSE;
  }
  /*
   * XXX MAX_IP_PACKET_SIZE is 4096, shouldn't this be MAX SDU size, which is 8188 bytes?
   */

  if (sdu_buffer_size > MAX_IP_PACKET_SIZE) {
    LOG_E(PDCP, "Requested SDU size (%d) is bigger than that can be handled by PDCP (%u)!\n",
          sdu_buffer_size, MAX_IP_PACKET_SIZE);
    // XXX What does following call do?
    mac_xface->macphy_exit("PDCP sdu buffer size > MAX_IP_PACKET_SIZE");
  }

  // PDCP transparent mode for MBMS traffic 

  if (mode == PDCP_TM) { 
    LOG_D(PDCP, " [TM] Asking for a new mem_block of size %d\n",sdu_buffer_size);
    pdcp_pdu = get_free_mem_block(sdu_buffer_size);
    if (pdcp_pdu != NULL) {
      memcpy(&pdcp_pdu->data[0], sdu_buffer, sdu_buffer_size); 
      rlc_status = rlc_data_req(module_id, frame, eNB_flag, RLC_MBMS_YES, rb_id_rlc, muiP, confirmP, sdu_buffer_size, pdcp_pdu);
    } else
      rlc_status = RLC_OP_STATUS_OUT_OF_RESSOURCES;
  } else {
    // calculate the pdcp header and trailer size
    if ((rb_id % NB_RB_MAX) < DTCH) {
      pdcp_header_len = PDCP_CONTROL_PLANE_DATA_PDU_SN_SIZE;
      pdcp_tailer_len = PDCP_CONTROL_PLANE_DATA_PDU_MAC_I_SIZE;
    } else {
      pdcp_header_len = PDCP_USER_PLANE_DATA_PDU_LONG_SN_HEADER_SIZE;
      pdcp_tailer_len = 0;
    }
    pdcp_pdu_size= sdu_buffer_size + pdcp_header_len + pdcp_tailer_len;

    LOG_I(PDCP, "Data request notification for PDCP entity with module ID %d and radio bearer ID %d pdu size %d (header%d, trailer%d)\n", module_id, rb_id,pdcp_pdu_size, pdcp_header_len,pdcp_tailer_len);

    /*
     * Allocate a new block for the new PDU (i.e. PDU header and SDU payload)
     */
    LOG_D(PDCP, "Asking for a new mem_block of size %d\n", pdcp_pdu_size);
    pdcp_pdu = get_free_mem_block(pdcp_pdu_size);

    if (pdcp_pdu != NULL) {
      /*
       * Create a Data PDU with header and append data
       *
       * Place User Plane PDCP Data PDU header first
       */
      
      if ((rb_id % NB_RB_MAX) < DTCH) { // this Control plane PDCP Data PDU
        pdcp_control_plane_data_pdu_header pdu_header;
        pdu_header.sn = pdcp_get_next_tx_seq_number(pdcp);
        current_sn = pdu_header.sn;
        memset(&pdu_header.mac_i[0],0,PDCP_CONTROL_PLANE_DATA_PDU_MAC_I_SIZE);
        if (pdcp_serialize_control_plane_data_pdu_with_SRB_sn_buffer((unsigned char*)pdcp_pdu->data, &pdu_header) == FALSE) {
          LOG_E(PDCP, "Cannot fill PDU buffer with relevant header fields!\n");
          return FALSE;
        }
      } else {
        pdcp_user_plane_data_pdu_header_with_long_sn pdu_header;
        pdu_header.dc = (mode == 1) ? PDCP_DATA_PDU :  PDCP_CONTROL_PDU;
        pdu_header.sn = pdcp_get_next_tx_seq_number(pdcp);
        current_sn = pdu_header.sn ;
        if (pdcp_serialize_user_plane_data_pdu_with_long_sn_buffer((unsigned char*)pdcp_pdu->data, &pdu_header) == FALSE) {
          LOG_E(PDCP, "Cannot fill PDU buffer with relevant header fields!\n");
          return FALSE;
        }
      }
      /*
       * Validate incoming sequence number, there might be a problem with PDCP initialization
       */
      if (current_sn > pdcp_calculate_max_seq_num_for_given_size(pdcp->seq_num_size)) {
        LOG_E(PDCP, "Generated sequence number (%lu) is greater than a sequence number could ever be!\n", current_sn);
        LOG_E(PDCP, "There must be a problem with PDCP initialization, ignoring this PDU...\n");

        free_mem_block(pdcp_pdu);
        return FALSE;
      }

      LOG_D(PDCP, "Sequence number %d is assigned to current PDU\n", current_sn);

      /* Then append data... */
      memcpy(&pdcp_pdu->data[pdcp_header_len], sdu_buffer, sdu_buffer_size);

      //For control plane data that are not integrity protected,
      // the MAC-I field is still present and should be padded with padding bits set to 0.
      // NOTE: user-plane data are never integrity protected
      for (i=0;i<pdcp_tailer_len;i++)
          pdcp_pdu->data[pdcp_header_len + sdu_buffer_size + i] = 0x00;// pdu_header.mac_i[i];

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
      util_print_hex_octets(PDCP, (unsigned char*)pdcp_pdu->data, pdcp_pdu_size);
      //util_flush_hex_octets(PDCP, (unsigned char*)pdcp_pdu->data, pdcp_pdu_size);
    } else {
      LOG_E(PDCP, "Cannot create a mem_block for a PDU!\n");
      return FALSE;
    }      
#ifdef PDCP_UNIT_TEST
    /*
     * Here we add PDU to the list and return to test code without
     * handing it off to RLC
     */
    list_add_tail_eurecom(pdcp_pdu, test_list);
    return TRUE;
#else
    /*
     * Ask sublayer to transmit data and check return value
     * to see if RLC succeeded
     */
    rlc_status = rlc_data_req(module_id, frame, eNB_flag, 0, rb_id_rlc, muiP, confirmP, pdcp_pdu_size, pdcp_pdu);
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
#endif // PDCP_UNIT_TEST

}

//-----------------------------------------------------------------------------
#ifdef PDCP_UNIT_TEST
BOOL pdcp_data_ind(module_id_t module_id, u32_t frame, u8_t eNB_flag, rb_id_t rb_id, sdu_size_t sdu_buffer_size, \
                   mem_block_t* sdu_buffer, pdcp_t* pdcp_test_entity, list_t* test_list)
#else
BOOL pdcp_data_ind(u8 eNB_id, u8 UE_id, u32_t frame, u8_t eNB_flag, u8_t MBMS_flagP, rb_id_t rb_id, sdu_size_t sdu_buffer_size, \
                   mem_block_t* sdu_buffer, u8 is_data_plane)
#endif
{
  //-----------------------------------------------------------------------------
#ifdef PDCP_UNIT_TEST
  pdcp_t* pdcp;
  list_t* sdu_list;
#else
  pdcp_t* pdcp;
  list_t* sdu_list;
#endif
  mem_block_t *new_sdu = NULL;
  u8 pdcp_header_len=0, pdcp_tailer_len=0;
  u16 sequence_number;
  u8 payload_offset = 0;
  module_id_t module_id;

#ifdef PDCP_UNIT_TEST
  pdcp = pdcp_test_entity;
  sdu_list = test_list;

  LOG_I(PDCP, "Data indication notification for PDCP entity for module"
        "ID %d and radio bearer ID %d rlc sdu size %d eNB_flag %d\n",
        module_id, rb_id, sdu_buffer_size, eNB_flag);
#else
  DevCheck4(UE_id < NUMBER_OF_UE_MAX, UE_id, NUMBER_OF_UE_MAX, eNB_id, rb_id);
  DevCheck4(eNB_id < NUMBER_OF_eNB_MAX, eNB_id, NUMBER_OF_eNB_MAX, UE_id, rb_id);
  DevCheck4(rb_id < NB_RB_MAX, rb_id, NB_RB_MAX, eNB_id, UE_id);

  if (eNB_flag == 0) {
    pdcp = &pdcp_array_ue[UE_id][rb_id];
    module_id = NB_eNB_INST + UE_id;

    LOG_I(PDCP, "Data indication notification for PDCP entity from UE %u to eNB %u "
          "and radio bearer ID %d rlc sdu size %d eNB_flag %d\n",
          UE_id, eNB_id, rb_id, sdu_buffer_size, eNB_flag);
  } else {
    pdcp = &pdcp_array_eNB[UE_id][eNB_id][rb_id];
    module_id = eNB_id;

    LOG_I(PDCP, "Data indication notification for PDCP entity from eNB %u to UE %u "
          "and radio bearer ID %d rlc sdu size %d eNB_flag %d\n",
          eNB_id, UE_id, rb_id, sdu_buffer_size, eNB_flag);
  }
  sdu_list = &pdcp_sdu_list;
#endif

  if (sdu_buffer_size == 0) {
    LOG_W(PDCP, "SDU buffer size is zero! Ignoring this chunk!\n");
    return FALSE;
  }

  /*
     * Check if incoming SDU is long enough to carry a PDU header
     */
  if (MBMS_flagP == 0 ) {
    if ((rb_id % NB_RB_MAX) < DTCH) {
      pdcp_header_len = PDCP_CONTROL_PLANE_DATA_PDU_SN_SIZE;
      pdcp_tailer_len = PDCP_CONTROL_PLANE_DATA_PDU_MAC_I_SIZE;
    } else {
      pdcp_header_len = PDCP_USER_PLANE_DATA_PDU_LONG_SN_HEADER_SIZE;
      pdcp_tailer_len = 0;
    }

    if (sdu_buffer_size < pdcp_header_len + pdcp_tailer_len ) {
      LOG_W(PDCP, "Incoming (from RLC) SDU is short of size (size:%d)! Ignoring...\n", sdu_buffer_size);
#ifndef PDCP_UNIT_TEST
      free_mem_block(sdu_buffer);
#endif
      return FALSE;
    }

    /*
       * Parse the PDU placed at the beginning of SDU to check
       * if incoming SN is in line with RX window
       */

    if (pdcp_header_len == PDCP_USER_PLANE_DATA_PDU_LONG_SN_HEADER_SIZE) { // DRB
      sequence_number =     pdcp_get_sequence_number_of_pdu_with_long_sn((unsigned char*)sdu_buffer->data);
//       u8 dc = pdcp_get_dc_filed((unsigned char*)sdu_buffer->data);
    } else { //SRB1/2
      sequence_number =   pdcp_get_sequence_number_of_pdu_with_SRB_sn((unsigned char*)sdu_buffer->data);
    }
    if (pdcp_is_rx_seq_number_valid(sequence_number, pdcp) == TRUE) {
      LOG_D(PDCP, "Incoming PDU has a sequence number (%d) in accordance with RX window, yay!\n", sequence_number);
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
    if ( (rb_id % NB_RB_MAX) <  DTCH ){
#if defined(ENABLE_SECURITY)
      if (pdcp->security_activated == 1) {
        pdcp_validate_security(pdcp, rb_id, pdcp_header_len,
                               sequence_number, sdu_buffer->data,
                               sdu_buffer_size - pdcp_tailer_len);
      }
#endif
      //rrc_lite_data_ind(module_id, //Modified MW - L2 Interface
      pdcp_rrc_data_ind(eNB_id,
                        UE_id,
                        frame,
                        eNB_flag,
                        rb_id,
                        sdu_buffer_size - pdcp_header_len - pdcp_tailer_len,
                        (u8*)&sdu_buffer->data[pdcp_header_len]);
      free_mem_block(sdu_buffer);
      // free_mem_block(new_sdu);
      return TRUE;
    }
    payload_offset=PDCP_USER_PLANE_DATA_PDU_LONG_SN_HEADER_SIZE;
#if defined(ENABLE_SECURITY)
    if (pdcp->security_activated == 1) {
        pdcp_validate_security(pdcp, rb_id % NB_RB_MAX, pdcp_header_len,
                               sequence_number, sdu_buffer->data,
                               sdu_buffer_size - pdcp_tailer_len);
    }
#endif
  } else {
    payload_offset=0;
  }
#if defined(USER_MODE) && defined(OAI_EMU)
  if (oai_emulation.info.otg_enabled == 1) {
    int src_id, dst_id, ctime;

    src_id = (eNB_flag != 0) ? UE_id + NB_eNB_INST : eNB_id;
    dst_id = (eNB_flag == 0) ? UE_id + NB_eNB_INST : eNB_id;
    ctime = oai_emulation.info.time_ms; // avg current simulation time in ms : we may get the exact time through OCG?
    LOG_D(PDCP, "Check received buffer : enb_flag %d mod id %d, rab id %d (src %d, dst %d)\n",
          eNB_flag, module_id, rb_id, src_id, dst_id);

    if (otg_rx_pkt(src_id, dst_id,ctime,&sdu_buffer->data[payload_offset],
                   sdu_buffer_size - payload_offset ) == 0 ) {
      free_mem_block(sdu_buffer);
      return TRUE;
    }
  }
#else
  if (otg_enabled==1) {
    LOG_D(OTG,"Discarding received packed\n");
    free_mem_block(sdu_buffer);
    return TRUE;
  }
#endif
  new_sdu = get_free_mem_block(sdu_buffer_size - payload_offset + sizeof (pdcp_data_ind_header_t));

  if (new_sdu) {
    /*
       * Prepend PDCP indication header which is going to be removed at pdcp_fifo_flush_sdus()
       */
    memset(new_sdu->data, 0, sizeof (pdcp_data_ind_header_t));
    if (eNB_flag == 0) {
      ((pdcp_data_ind_header_t *) new_sdu->data)->rb_id     = rb_id;
    } else {
      /* RB id for an UE on eNB is instantiated */
      ((pdcp_data_ind_header_t *) new_sdu->data)->rb_id     = rb_id + (UE_id * NB_RAB_MAX);
    }
    ((pdcp_data_ind_header_t *) new_sdu->data)->data_size = sdu_buffer_size - payload_offset;

    // Here there is no virtualization possible
#ifdef IDROMEL_NEMO
    if (eNB_flag == 0)
      ((pdcp_data_ind_header_t *) new_sdu->data)->inst = rb_id / NB_RAB_MAX;
    else
      ((pdcp_data_ind_header_t *) new_sdu->data)->inst = 0;
#else
    ((pdcp_data_ind_header_t *) new_sdu->data)->inst = module_id;
#endif
    
    // XXX Decompression would be done at this point

    /*
       * After checking incoming sequence number PDCP header
       * has to be stripped off so here we copy SDU buffer starting
       * from its second byte (skipping 0th and 1st octets, i.e.
       * PDCP header)
       */
    memcpy(&new_sdu->data[sizeof (pdcp_data_ind_header_t)], \
           &sdu_buffer->data[payload_offset], \
           sdu_buffer_size - payload_offset);
    list_add_tail_eurecom (new_sdu, sdu_list);

    /* Print octets of incoming data in hexadecimal form */
    LOG_D(PDCP, "Following content has been received from RLC (%d,%d)(PDCP header has already been removed):\n", sdu_buffer_size  - payload_offset + sizeof(pdcp_data_ind_header_t),
          sdu_buffer_size  - payload_offset);
    //util_print_hex_octets(PDCP, (unsigned char*)new_sdu->data, sdu_buffer_size  - PDCP_USER_PLANE_DATA_PDU_LONG_SN_HEADER_SIZE + sizeof(pdcp_data_ind_header_t));
    util_flush_hex_octets(PDCP, (unsigned char*)new_sdu->data, sdu_buffer_size  - payload_offset  + sizeof(pdcp_data_ind_header_t));

    /*
       * Update PDCP statistics
       * XXX Following two actions are identical, is there a merge error?
       */
    if (eNB_flag == 1) {
      Pdcp_stats_rx[module_id][(rb_id & RAB_OFFSET2) >> RAB_SHIFT2][(rb_id & RAB_OFFSET) - DTCH]++;
      Pdcp_stats_rx_bytes[module_id][(rb_id & RAB_OFFSET2) >> RAB_SHIFT2][(rb_id & RAB_OFFSET) - DTCH] += sdu_buffer_size;
    } else {
      Pdcp_stats_rx[module_id][(rb_id & RAB_OFFSET2) >> RAB_SHIFT2][(rb_id & RAB_OFFSET) - DTCH]++;
      Pdcp_stats_rx_bytes[module_id][(rb_id & RAB_OFFSET2) >> RAB_SHIFT2][(rb_id & RAB_OFFSET) - DTCH] += sdu_buffer_size;
    }
  }

  free_mem_block(sdu_buffer);

  return TRUE;
}

//-----------------------------------------------------------------------------
void pdcp_run (u32_t frame, u8 eNB_flag, u8 UE_index, u8 eNB_index) {
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

          pdcp_data_req (RRC_DCCH_DATA_REQ (msg_p).eNB_index, RRC_DCCH_DATA_REQ (msg_p).ue_index, RRC_DCCH_DATA_REQ (msg_p).frame, RRC_DCCH_DATA_REQ (msg_p).enb_flag,
                         RRC_DCCH_DATA_REQ (msg_p).rb_id, RRC_DCCH_DATA_REQ (msg_p).muip,
                         RRC_DCCH_DATA_REQ (msg_p).confirmp, RRC_DCCH_DATA_REQ (msg_p).sdu_size,
                         RRC_DCCH_DATA_REQ (msg_p).sdu_p, RRC_DCCH_DATA_REQ (msg_p).mode);

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
#endif

  pdcp_fifo_read_input_sdus_from_otg(frame, eNB_flag, UE_index, eNB_index);

  // IP/NAS -> PDCP traffic : TX, read the pkt from the upper layer buffer
  pdcp_fifo_read_input_sdus(frame, eNB_flag, UE_index, eNB_index);

  // PDCP -> NAS/IP traffic: RX
  pdcp_fifo_flush_sdus(frame, eNB_flag, eNB_index, UE_index);

  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_PDCP_RUN, VCD_FUNCTION_OUT);
}

BOOL rrc_pdcp_config_asn1_req (u8 eNB_id, u8 UE_id, u32_t frame, u8_t eNB_flag,
                               SRB_ToAddModList_t* srb2add_list,
                               DRB_ToAddModList_t* drb2add_list,
                               DRB_ToReleaseList_t*  drb2release_list,
                               u8 security_mode,
                               u8 *kRRCenc,
                               u8 *kRRCint,
                               u8 *kUPenc
#ifdef Rel10
                              ,PMCH_InfoList_r9_t*  pmch_InfoList_r9
#endif
                               )
{
  long int        rb_id          = 0;
  long int        lc_id          = 0;
  long int        srb_id         = 0;
  long int        mch_id         = 0;
  rlc_mode_t      rlc_type       = RLC_NONE;
  DRB_Identity_t  drb_id         = 0;
  DRB_Identity_t* pdrb_id        = NULL;
  u8              drb_sn         = 0;
  u8              srb_sn         = 5; // fixed sn for SRBs
  u8              drb_report     = 0;
  long int        cnt            = 0;
  u16 header_compression_profile = 0;
  u32 action                     = ACTION_ADD;
  SRB_ToAddMod_t* srb_toaddmod   = NULL;
  DRB_ToAddMod_t* drb_toaddmod   = NULL;
  pdcp_t        *pdcp;
  module_id_t    module_id;

#ifdef Rel10
  int i,j;
  MBMS_SessionInfoList_r9_t *mbms_SessionInfoList_r9;
  MBMS_SessionInfo_r9_t     *MBMS_SessionInfo= NULL;
#endif

  if (eNB_flag == 0) {
    module_id = NB_eNB_INST + UE_id;
    LOG_D(PDCP, "[UE %d] CONFIG REQ ASN1 for eNB %d (Mod_id %u)\n", UE_id, eNB_id, module_id);
  } else {
    module_id = eNB_id;
    LOG_D(PDCP, "[eNB %d] CONFIG REQ ASN1 for UE %d (Mod_id %u)\n", eNB_id, UE_id, module_id);
  }
  // srb2add_list does not define pdcp config, we use rlc info to setup the pdcp dcch0 and dcch1 channels

  if (srb2add_list != NULL) {
    for (cnt=0;cnt<srb2add_list->list.count;cnt++) {
      srb_id = srb2add_list->list.array[cnt]->srb_Identity;
      srb_toaddmod = srb2add_list->list.array[cnt];
      rlc_type = RLC_MODE_AM;
      rb_id = srb_id;
      lc_id = srb_id;

      if (eNB_flag == 0) {
        pdcp = &pdcp_array_ue[UE_id][srb_id];
      } else {
        pdcp = &pdcp_array_eNB[eNB_id][UE_id][srb_id];
      }

      if (pdcp->instanciated_instance == 1) {
        action = ACTION_MODIFY;
      } else {
        action = ACTION_ADD;
      }

      if (srb_toaddmod->rlc_Config) {
        switch (srb_toaddmod->rlc_Config->present) {
          case SRB_ToAddMod__rlc_Config_PR_NOTHING:
            break;
          case SRB_ToAddMod__rlc_Config_PR_explicitValue:
            switch (srb_toaddmod->rlc_Config->choice.explicitValue.present) {
            case RLC_Config_PR_NOTHING:
              break;
            default:
              pdcp_config_req_asn1 (pdcp,
                                    eNB_id,
                                    UE_id,
                                    frame,
                                    eNB_flag, // not really required
                                    rlc_type,
                                    action,
                                    lc_id,
                                    mch_id,
                                    rb_id,
                                    srb_sn,
                                    0, // drb_report
                                    0, // header compression
                                    security_mode,
                                    kRRCenc,
                                    kRRCint,
                                    kUPenc);
              break;
            }
            break;
          case SRB_ToAddMod__rlc_Config_PR_defaultValue:
            // already the default values
            break;
          default:
            DevParam(srb_toaddmod->rlc_Config->present, UE_id, eNB_id);
            break;
        }
      }
    }
  }
  // reset the action

  if (drb2add_list != NULL) {
    for (cnt=0;cnt<drb2add_list->list.count;cnt++) {

      drb_toaddmod = drb2add_list->list.array[cnt];

      drb_id = drb_toaddmod->drb_Identity;

      if (drb_toaddmod->logicalChannelIdentity != null) {
        lc_id = *drb_toaddmod->logicalChannelIdentity;
      } else {
        lc_id = -1;
      }
      rb_id = lc_id;

      DevCheck4(rb_id < NB_RB_MAX, rb_id, NB_RB_MAX, UE_id, eNB_id);

      if (eNB_flag == 0) {
        pdcp = &pdcp_array_ue[UE_id][rb_id];
      } else {
        pdcp = &pdcp_array_eNB[eNB_id][UE_id][rb_id];
      }

      if (pdcp->instanciated_instance == 1)
        action = ACTION_MODIFY;
      else
        action = ACTION_ADD;

      if (drb_toaddmod->pdcp_Config){
        if (drb_toaddmod->pdcp_Config->discardTimer) {
          // set the value of the timer
        }
        if (drb_toaddmod->pdcp_Config->rlc_AM) {
          drb_report = drb_toaddmod->pdcp_Config->rlc_AM->statusReportRequired;
          rlc_type =RLC_MODE_AM;
        }
        if (drb_toaddmod->pdcp_Config->rlc_UM){
          drb_sn = drb_toaddmod->pdcp_Config->rlc_UM->pdcp_SN_Size;
          rlc_type =RLC_MODE_UM;
        }
        switch (drb_toaddmod->pdcp_Config->headerCompression.present) {
        case PDCP_Config__headerCompression_PR_NOTHING:
        case PDCP_Config__headerCompression_PR_notUsed:
          header_compression_profile=0x0;
          break;
        case PDCP_Config__headerCompression_PR_rohc:
          // parse the struc and get the rohc profile
          if(drb_toaddmod->pdcp_Config->headerCompression.choice.rohc.profiles.profile0x0001)
            header_compression_profile=0x0001;
          else if(drb_toaddmod->pdcp_Config->headerCompression.choice.rohc.profiles.profile0x0002)
            header_compression_profile=0x0002;
          else if(drb_toaddmod->pdcp_Config->headerCompression.choice.rohc.profiles.profile0x0003)
            header_compression_profile=0x0003;
          else if(drb_toaddmod->pdcp_Config->headerCompression.choice.rohc.profiles.profile0x0004)
            header_compression_profile=0x0004;
          else if(drb_toaddmod->pdcp_Config->headerCompression.choice.rohc.profiles.profile0x0006)
            header_compression_profile=0x0006;
          else if(drb_toaddmod->pdcp_Config->headerCompression.choice.rohc.profiles.profile0x0101)
            header_compression_profile=0x0101;
          else if(drb_toaddmod->pdcp_Config->headerCompression.choice.rohc.profiles.profile0x0102)
            header_compression_profile=0x0102;
          else if(drb_toaddmod->pdcp_Config->headerCompression.choice.rohc.profiles.profile0x0103)
            header_compression_profile=0x0103;
          else if(drb_toaddmod->pdcp_Config->headerCompression.choice.rohc.profiles.profile0x0104)
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
        pdcp_config_req_asn1 (pdcp,
                              eNB_id,
                              UE_id,
                              frame,
                              eNB_flag, // not really required
                              rlc_type,
                              action,
                              lc_id,
                              mch_id,
                              rb_id,
                              drb_sn,
                              drb_report,
                              header_compression_profile,
                              security_mode,
                              kRRCenc,
                              kRRCint,
                              kUPenc);
      }
    }
  }

  if (drb2release_list != NULL) {
    for (cnt=0;cnt<drb2release_list->list.count;cnt++) {
      pdrb_id = drb2release_list->list.array[cnt];
      rb_id =  *pdrb_id;
      if (eNB_flag == 0) {
        pdcp = &pdcp_array_ue[UE_id][rb_id];
      } else {
        pdcp = &pdcp_array_eNB[eNB_id][UE_id][rb_id];
      }
      action = ACTION_REMOVE;
      pdcp_config_req_asn1 (pdcp,
                            eNB_id,
                            UE_id,
                            frame,
                            eNB_flag, // not really required
                            rlc_type,
                            action,
                            lc_id,
                            mch_id,
                            rb_id,
                            0,
                            0,
                            0,
                            security_mode,
                            kRRCenc,
                            kRRCint,
                            kUPenc);
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
                              eNB_id,
                              UE_id,
                              frame,
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

BOOL pdcp_config_req_asn1 (pdcp_t *pdcp, u8 eNB_id, u8 UE_id, u32 frame, u8_t eNB_flag,
                           rlc_mode_t rlc_mode, u32  action, u16 lc_id,u16 mch_id, rb_id_t rb_id,
                           u8 rb_sn, u8 rb_report, u16 header_compression_profile,
                           u8 security_mode,
                           u8 *kRRCenc,
                           u8 *kRRCint,
                           u8 *kUPenc)
{
  module_id_t module_id = 0;

  switch (action) {
    case ACTION_ADD:
      DevAssert(pdcp != NULL);
      pdcp->instanciated_instance = 1;
      pdcp->is_ue = (eNB_flag == 0) ? 1 : 0;
      pdcp->lcid = lc_id;
      pdcp->header_compression_profile = header_compression_profile;
      pdcp->status_report = rb_report;

      if (rb_sn == PDCP_Config__rlc_UM__pdcp_SN_Size_len7bits) {
        pdcp->seq_num_size = 7;
      } else if (rb_sn == PDCP_Config__rlc_UM__pdcp_SN_Size_len12bits) {
        pdcp->seq_num_size=12;
      } else {
        pdcp->seq_num_size=5;
      }

      pdcp->rlc_mode = rlc_mode;
      pdcp->next_pdcp_tx_sn = 0;
      pdcp->next_pdcp_rx_sn = 0;
      pdcp->tx_hfn = 0;
      pdcp->rx_hfn = 0;
      pdcp->last_submitted_pdcp_rx_sn = 4095;
      pdcp->first_missing_pdu = -1;

      if (eNB_flag == 0) {
        LOG_I(PDCP, "[UE %d] Config request : Action ADD for eNB %d: Frame %d LCID %d (rb id %d) "
              "configured with SN size %d bits and RLC %s\n",
              UE_id, eNB_id, frame, lc_id, rb_id, pdcp->seq_num_size,
              (rlc_mode == 1) ? "AM" : (rlc_mode == 2) ? "TM" : "UM");
      } else {
        LOG_I(PDCP, "[eNB %d] Config request : Action ADD for UE %d: Frame %d LCID %d (rb id %d) "
              "configured with SN size %d bits and RLC %s\n",
              eNB_id, UE_id, frame, lc_id, rb_id, pdcp->seq_num_size,
              (rlc_mode == 1) ? "AM" : (rlc_mode == 2) ? "TM" : "UM");
      }

      /* Setup security */
      if (security_mode != 0xff) {
        pdcp_config_set_security(pdcp, eNB_id, UE_id, frame, eNB_flag, rb_id, lc_id, security_mode, kRRCenc, kRRCint, kUPenc);
      }

      LOG_D(PDCP, "[MSC_NEW][FRAME %05d][PDCP][MOD %02d][RB %02d]\n", frame, (eNB_flag == 0) ? NB_eNB_INST + UE_id : eNB_id, rb_id);
      break;

    case ACTION_MODIFY:
      DevAssert(pdcp != NULL);
      pdcp->header_compression_profile=header_compression_profile;
      pdcp->status_report = rb_report;
      pdcp->rlc_mode = rlc_mode;

      /* Setup security */
      if (security_mode != 0xff) {
        pdcp_config_set_security(pdcp, eNB_id, UE_id, frame, eNB_flag, rb_id, lc_id, security_mode, kRRCenc, kRRCint, kUPenc);
      }

      if (rb_sn == PDCP_Config__rlc_UM__pdcp_SN_Size_len7bits) {
        pdcp->seq_num_size = 7;
      } else if (rb_sn == PDCP_Config__rlc_UM__pdcp_SN_Size_len12bits) {
        pdcp->seq_num_size = 12;
      } else {
        pdcp->seq_num_size=5;
      }

      if (eNB_flag == 0) {
        LOG_I(PDCP,"[UE %d] Config request : Action MODIFY for eNB %d: Frame %d LCID %d "
              "RB id %d configured with SN size %d and RLC %s \n",
              UE_id, eNB_id, frame, lc_id, rb_id, rb_sn,
              (rlc_mode == 1) ? "AM" : (rlc_mode == 2) ? "TM" : "UM");
      } else {
        LOG_I(PDCP,"[eNB %d] Config request : Action MODIFY for UE %d: Frame %d LCID %d "
              "RB id %d configured with SN size %d and RLC %s \n",
              eNB_id, UE_id, frame, lc_id, rb_id, rb_sn,
              (rlc_mode == 1) ? "AM" : (rlc_mode == 2) ? "TM" : "UM");
      }
      break;
    case ACTION_REMOVE:
      DevAssert(pdcp != NULL);
      pdcp->instanciated_instance = 0;
      pdcp->lcid = 0;
      pdcp->header_compression_profile = 0x0;
      pdcp->cipheringAlgorithm = 0xff;
      pdcp->integrityProtAlgorithm = 0xff;
      pdcp->status_report = 0;
      pdcp->rlc_mode = RLC_NONE;
      pdcp->next_pdcp_tx_sn = 0;
      pdcp->next_pdcp_rx_sn = 0;
      pdcp->tx_hfn = 0;
      pdcp->rx_hfn = 0;
      pdcp->last_submitted_pdcp_rx_sn = 4095;
      pdcp->seq_num_size = 0;
      pdcp->first_missing_pdu = -1;
      pdcp->security_activated = 0;

      if (eNB_flag == 0) {
        LOG_I(PDCP, "[UE %d] Config request : ACTION_REMOVE for eNB %d: Frame %d LCID %d RBID %d configured\n",
              UE_id, eNB_id, frame, lc_id, rb_id);
      } else {
        LOG_I(PDCP, "[eNB %d] Config request : ACTION_REMOVE for UE %d: Frame %d LCID %d RBID %d configured\n",
              eNB_id, UE_id, frame, lc_id, rb_id);
      }
      /* Security keys */
      if (pdcp->kUPenc != NULL) {
        free(pdcp->kUPenc);
      }
      if (pdcp->kRRCint != NULL) {
        free(pdcp->kRRCint);
      }
      if (pdcp->kRRCenc != NULL) {
        free(pdcp->kRRCenc);
      }
      break;
    case ACTION_MBMS_ADD:
    case ACTION_MBMS_MODIFY:
      if (eNB_flag == 0) {
        module_id = UE_id + NB_eNB_INST;
        LOG_I(PDCP,"[UE %d] Config request for eNB %d: %s: Frame %d service_id/mch index %d, session_id/lcid %d, rbid %d configured\n",
              UE_id, eNB_id, action == ACTION_MBMS_ADD ? "ACTION_MBMS_ADD" : "ACTION_MBMS_MODIFY", frame, mch_id, lc_id, rb_id);
      } else {
        module_id = eNB_id;
        LOG_I(PDCP,"[eNB %d] Config request for UE %d: %s: Frame %d service_id/mch index %d, session_id/lcid %d, rbid %d configured\n",
              eNB_id, UE_id, action == ACTION_MBMS_ADD ? "ACTION_MBMS_ADD" : "ACTION_MBMS_MODIFY", frame, mch_id, lc_id, rb_id);
      }
      pdcp_mbms_array[module_id][rb_id].instanciated_instance = module_id + 1 ;
      pdcp_mbms_array[module_id][rb_id].service_id = mch_id;
      pdcp_mbms_array[module_id][rb_id].session_id = lc_id;
      pdcp_mbms_array[module_id][rb_id].rb_id = rb_id;
      break;
    case ACTION_SET_SECURITY_MODE:
      pdcp_config_set_security(pdcp, eNB_id, UE_id, frame, eNB_flag, rb_id, lc_id, security_mode, kRRCenc, kRRCint, kUPenc);
      break;
    default:
      DevParam(action, eNB_id, UE_id);
      break;
  }
  return 0;
}

void pdcp_config_set_security(pdcp_t *pdcp, u8 eNB_id, u8 UE_id, u32 frame, u8 eNB_flag, rb_id_t rb_id,
                              u16 lc_id, u8 security_mode, u8 *kRRCenc, u8 *kRRCint, u8 *kUPenc)
{
  DevAssert(pdcp != NULL);

  if ((security_mode >= 0) && (security_mode <= 0x77)) {
    pdcp->cipheringAlgorithm     = security_mode & 0x0f;
    pdcp->integrityProtAlgorithm = (security_mode>>4) & 0xf;

    if (eNB_flag == 0) {
      LOG_D(PDCP,"[UE %d][RB %02d] Set security mode : ACTION_SET_SECURITY_MODE: "
            "Frame %d  cipheringAlgorithm %d integrityProtAlgorithm %d\n",
            UE_id, rb_id, frame, pdcp->cipheringAlgorithm, pdcp->integrityProtAlgorithm);
    } else {
      LOG_D(PDCP,"[eNB %d][UE %d][RB %02d] Set security mode : ACTION_SET_SECURITY_MODE: "
            "Frame %d  cipheringAlgorithm %d integrityProtAlgorithm %d\n",
            eNB_id, UE_id, rb_id, frame, pdcp->cipheringAlgorithm, pdcp->integrityProtAlgorithm);
    }
    pdcp->kRRCenc = kRRCenc;
    pdcp->kRRCint = kRRCint;
    pdcp->kUPenc  = kUPenc;

    /* Activate security */
    pdcp->security_activated = 1;
  } else {
    LOG_E(PDCP,"[%s %d] bad security mode %d", security_mode);
  }
}

void rrc_pdcp_config_req (u8 eNB_id, u8 UE_id, u32 frame, u8_t eNB_flag, u32 action, rb_id_t rb_id, u8 security_mode)
{
  pdcp_t *pdcp;
  module_id_t module_id;

  if (eNB_flag == 0) {
    pdcp = &pdcp_array_ue[UE_id][rb_id];
    module_id = NB_eNB_INST + UE_id;
  } else {
    pdcp = &pdcp_array_eNB[eNB_id][UE_id][rb_id];
    module_id = eNB_id;
  }

  /*
     * Initialize sequence number state variables of relevant PDCP entity
     */
  switch (action) {
    case ACTION_ADD:
      pdcp->instanciated_instance = 1;

      pdcp->next_pdcp_tx_sn = 0;
      pdcp->next_pdcp_rx_sn = 0;
      pdcp->tx_hfn = 0;
      pdcp->rx_hfn = 0;
      /* SN of the last PDCP SDU delivered to upper layers */
      pdcp->last_submitted_pdcp_rx_sn = 4095;

      if (rb_id < DTCH) { // SRB
        pdcp->seq_num_size = 5;
      } else { // DRB
        pdcp->seq_num_size = 12;
      }
      pdcp->first_missing_pdu = -1;
      LOG_D(PDCP,"[%s %d] Config request : Action ADD: Frame %d radio bearer id %d configured\n",
            (eNB_flag) ? "eNB" : "UE", module_id, frame, rb_id);
      LOG_D(PDCP,  "[MSC_NEW][FRAME %05d][PDCP][MOD %02d][RB %02d]\n", frame, module_id,rb_id);
      break;
    case ACTION_MODIFY:
      break;
    case ACTION_REMOVE:
      pdcp->instanciated_instance = 0;
      pdcp->next_pdcp_tx_sn = 0;
      pdcp->next_pdcp_rx_sn = 0;
      pdcp->tx_hfn = 0;
      pdcp->rx_hfn = 0;
      pdcp->last_submitted_pdcp_rx_sn = 4095;
      pdcp->seq_num_size = 0;
      pdcp->first_missing_pdu = -1;
      pdcp->security_activated = 0;
      LOG_D(PDCP,"[%s %d] Config request : ACTION_REMOVE: Frame %d radio bearer id %d configured\n",
            (eNB_flag) ? "eNB" : "UE", module_id, frame, rb_id);

      break;
    case ACTION_SET_SECURITY_MODE:
      if ((security_mode >= 0) && (security_mode <= 0x77)) {
        pdcp->cipheringAlgorithm= security_mode & 0x0f;
        pdcp->integrityProtAlgorithm = (security_mode>>4) & 0xf;
        LOG_D(PDCP,"[%s %d]Set security mode : ACTION_SET_SECURITY_MODE: Frame %d  cipheringAlgorithm %d integrityProtAlgorithm %d\n",
              (eNB_flag) ? "eNB" : "UE", module_id, frame,
              pdcp->cipheringAlgorithm,
              pdcp->integrityProtAlgorithm );
      } else {
        LOG_D(PDCP,"[%s %d] bad security mode %d", security_mode);
      }
      break;
    default:
      DevParam(action, UE_id, eNB_id);
      break;
  }
}

// TODO PDCP module initialization code might be removed
int
    pdcp_module_init ()
{
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
void
    pdcp_module_cleanup ()
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

  /*
    * Initialize SDU list
    */
  list_init(&pdcp_sdu_list, NULL);

  memset(pdcp_array_ue, 0, sizeof(pdcp_array_ue));
  memset(pdcp_array_eNB, 0, sizeof(pdcp_array_eNB));

  memset((void*)&pdcp_mbms_array, 0, sizeof(pdcp_mbms_array));

  LOG_I(PDCP, "PDCP layer has been initialized\n");

  pdcp_output_sdu_bytes_to_write=0;
  pdcp_output_header_bytes_to_write=0;
  pdcp_input_sdu_remaining_size_to_read=0;

  /*
    * Initialize PDCP entities (see pdcp_t at pdcp.h)
    */
  // set RB for eNB : this is now down by RRC for each mod id and rab id when needed.
  /*  for (i=0;i  < NB_eNB_INST; i++) {
       for (j=NB_eNB_INST; j < NB_eNB_INST+NB_UE_INST; j++ ) {
       pdcp_config_req(i, (j-NB_eNB_INST) * NB_RB_MAX + DCCH, DCCH  ); // default DRB
       pdcp_config_req(i, (j-NB_eNB_INST) * NB_RB_MAX + DCCH1, DCCH1  ); // default DRB
       pdcp_config_req(i, (j-NB_eNB_INST) * NB_RB_MAX + DTCH, DTCH  ); // default DRB
       }
       }
       // set RB for UE
       for (i=NB_eNB_INST;i<NB_eNB_INST+NB_UE_INST; i++) {
       for (j=0;j<NB_eNB_INST; j++) {
       pdcp_config_req(i, j * NB_RB_MAX + DCCH, DCCH ); // default DRB
       pdcp_config_req(i, j * NB_RB_MAX + DCCH1, DCCH1 ); // default DRB
       pdcp_config_req(i, j * NB_RB_MAX + DTCH, DTCH ); // default DRB
       }
       }*/

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
void
    pdcp_layer_cleanup (void)
    //-----------------------------------------------------------------------------
{
  list_free (&pdcp_sdu_list);
}

#ifdef NAS_FIFO
EXPORT_SYMBOL(pdcp_2_nas_irq);
#endif //NAS_FIFO

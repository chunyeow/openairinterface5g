/*******************************************************************************

Eurecom OpenAirInterface 2
Copyright(c) 1999 - 2010 Eurecom

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

#define RLC_C
#include "rlc.h"
#include "mem_block.h"
#include "../MAC/extern.h"
#include "UTIL/LOG/log.h"

#include "assertions.h"

extern void pdcp_data_ind (u8 eNB_id, u8 UE_id, u32_t frame, u8_t eNB_flag, u8_t MBMS_flag, rb_id_t rab_idP, sdu_size_t data_sizeP, mem_block_t * sduP, u8 is_data_plane);

//#define DEBUG_RLC_PDCP_INTERFACE

//#define DEBUG_RLC_DATA_REQ 1

//-----------------------------------------------------------------------------
void rlc_util_print_hex_octets(comp_name_t componentP, unsigned char* dataP, unsigned long sizeP)
//-----------------------------------------------------------------------------
{
  unsigned long octet_index = 0;

  if (dataP == NULL) {
    return;
  }


  LOG_T(componentP, "      |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f |\n");
  LOG_T(componentP, "------+-------------------------------------------------|\n");
  for (octet_index = 0; octet_index < sizeP; octet_index++) {
    if ((octet_index % 16) == 0){
      if (octet_index != 0) {
        LOG_T(componentP, " |\n");
      }
      LOG_T(componentP, " %04d |", octet_index);
    }
    /*
     * Print every single octet in hexadecimal form
     */
    LOG_T(componentP, " %02x", dataP[octet_index]);
    /*
     * Align newline and pipes according to the octets in groups of 2
     */
  }

  /*
   * Append enough spaces and put final pipe
   */
  unsigned char index;
  for (index = octet_index; index < 16; ++index)
    LOG_T(componentP, "   ");
  LOG_T(componentP, " |\n");
}

//-----------------------------------------------------------------------------
rlc_op_status_t rlc_stat_req     (module_id_t module_idP,
                  u32_t frame,
                  rb_id_t        rb_idP,
                  unsigned int* stat_tx_pdcp_sdu,
                  unsigned int* stat_tx_pdcp_bytes,
                  unsigned int* stat_tx_pdcp_sdu_discarded,
                  unsigned int* stat_tx_pdcp_bytes_discarded,
                  unsigned int* stat_tx_data_pdu,
                  unsigned int* stat_tx_data_bytes,
                  unsigned int* stat_tx_retransmit_pdu_by_status,
                  unsigned int* stat_tx_retransmit_bytes_by_status,
                  unsigned int* stat_tx_retransmit_pdu,
                  unsigned int* stat_tx_retransmit_bytes,
                  unsigned int* stat_tx_control_pdu,
                  unsigned int* stat_tx_control_bytes,
                  unsigned int* stat_rx_pdcp_sdu,
                  unsigned int* stat_rx_pdcp_bytes,
                  unsigned int* stat_rx_data_pdus_duplicate,
                  unsigned int* stat_rx_data_bytes_duplicate,
                  unsigned int* stat_rx_data_pdu,
                  unsigned int* stat_rx_data_bytes,
                  unsigned int* stat_rx_data_pdu_dropped,
                  unsigned int* stat_rx_data_bytes_dropped,
                  unsigned int* stat_rx_data_pdu_out_of_window,
                  unsigned int* stat_rx_data_bytes_out_of_window,
                  unsigned int* stat_rx_control_pdu,
                  unsigned int* stat_rx_control_bytes,
                  unsigned int* stat_timer_reordering_timed_out,
                  unsigned int* stat_timer_poll_retransmit_timed_out,
                  unsigned int* stat_timer_status_prohibit_timed_out) {
//-----------------------------------------------------------------------------
  if ((module_idP >= 0) && (module_idP < MAX_MODULES)) {
      if ((rb_idP >= 0) && (rb_idP < MAX_RAB)) {
            switch (rlc[module_idP].m_rlc_pointer[rb_idP].rlc_type) {
                    case RLC_NONE:
                        *stat_tx_pdcp_sdu                     = 0;
                        *stat_tx_pdcp_bytes                   = 0;
                        *stat_tx_pdcp_sdu_discarded           = 0;
                        *stat_tx_pdcp_bytes_discarded         = 0;
                        *stat_tx_data_pdu                     = 0;
                        *stat_tx_data_bytes                   = 0;
                        *stat_tx_retransmit_pdu_by_status     = 0;
                        *stat_tx_retransmit_bytes_by_status   = 0;
                        *stat_tx_retransmit_pdu               = 0;
                        *stat_tx_retransmit_bytes             = 0;
                        *stat_tx_control_pdu                  = 0;
                        *stat_tx_control_bytes                = 0;
                        *stat_rx_pdcp_sdu                     = 0;
                        *stat_rx_pdcp_bytes                   = 0;
                        *stat_rx_data_pdus_duplicate          = 0;
                        *stat_rx_data_bytes_duplicate         = 0;
                        *stat_rx_data_pdu                     = 0;
                        *stat_rx_data_bytes                   = 0;
                        *stat_rx_data_pdu_dropped             = 0;
                        *stat_rx_data_bytes_dropped           = 0;
                        *stat_rx_data_pdu_out_of_window       = 0;
                        *stat_rx_data_bytes_out_of_window     = 0;
                        *stat_rx_control_pdu                  = 0;
                        *stat_rx_control_bytes                = 0;
                        *stat_timer_reordering_timed_out      = 0;
                        *stat_timer_poll_retransmit_timed_out = 0;
                        *stat_timer_status_prohibit_timed_out = 0;
                        return RLC_OP_STATUS_BAD_PARAMETER;
                        break;

                    case RLC_AM:
                        rlc_am_stat_req     (&rlc[module_idP].m_rlc_am_array[rlc[module_idP].m_rlc_pointer[rb_idP].rlc_index],
		                  stat_tx_pdcp_sdu,
		                  stat_tx_pdcp_bytes,
		                  stat_tx_pdcp_sdu_discarded,
		                  stat_tx_pdcp_bytes_discarded,
		                  stat_tx_data_pdu,
		                  stat_tx_data_bytes,
		                  stat_tx_retransmit_pdu_by_status,
		                  stat_tx_retransmit_bytes_by_status,
		                  stat_tx_retransmit_pdu,
		                  stat_tx_retransmit_bytes,
		                  stat_tx_control_pdu,
		                  stat_tx_control_bytes,
		                  stat_rx_pdcp_sdu,
		                  stat_rx_pdcp_bytes,
		                  stat_rx_data_pdus_duplicate,
		                  stat_rx_data_bytes_duplicate,
		                  stat_rx_data_pdu,
		                  stat_rx_data_bytes,
		                  stat_rx_data_pdu_dropped,
		                  stat_rx_data_bytes_dropped,
		                  stat_rx_data_pdu_out_of_window,
		                  stat_rx_data_bytes_out_of_window,
		                  stat_rx_control_pdu,
		                  stat_rx_control_bytes,
		                  stat_timer_reordering_timed_out,
		                  stat_timer_poll_retransmit_timed_out,
		                  stat_timer_status_prohibit_timed_out);
                          return RLC_OP_STATUS_OK;
                        break;

                    case RLC_UM:
                        *stat_tx_retransmit_pdu_by_status     = 0;
                        *stat_tx_retransmit_bytes_by_status   = 0;
                        *stat_tx_retransmit_pdu               = 0;
                        *stat_tx_retransmit_bytes             = 0;
                        *stat_tx_control_pdu                  = 0;
                        *stat_tx_control_bytes                = 0;
                        *stat_rx_data_pdu_dropped             = 0;
                        *stat_rx_data_bytes_dropped           = 0;
                        *stat_rx_data_pdu_out_of_window       = 0;
                        *stat_rx_data_bytes_out_of_window     = 0;
                        *stat_timer_poll_retransmit_timed_out = 0;
                        *stat_timer_status_prohibit_timed_out = 0;
                        rlc_um_stat_req (&rlc[module_idP].m_rlc_um_array[rlc[module_idP].m_rlc_pointer[rb_idP].rlc_index],
                      		  stat_tx_pdcp_sdu,
                      		  stat_tx_pdcp_bytes,
                      		  stat_tx_pdcp_sdu_discarded,
                      		  stat_tx_pdcp_bytes_discarded,
                      		  stat_tx_data_pdu,
                      		  stat_tx_data_bytes,
                      		  stat_rx_pdcp_sdu,
                      		  stat_rx_pdcp_bytes,
                      		  stat_rx_data_pdus_duplicate,
                      		  stat_rx_data_bytes_duplicate,
                      		  stat_rx_data_pdu,
                      		  stat_rx_data_bytes,
                      		  stat_rx_data_pdu_dropped,
                      		  stat_rx_data_bytes_dropped,
                      		  stat_rx_data_pdu_out_of_window,
                      		  stat_rx_data_bytes_out_of_window,
                      		  stat_timer_reordering_timed_out);
                        return RLC_OP_STATUS_OK;
                        break;

                    case RLC_TM:
                        *stat_tx_pdcp_sdu                     = 0;
                        *stat_tx_pdcp_bytes                   = 0;
                        *stat_tx_pdcp_sdu_discarded           = 0;
                        *stat_tx_pdcp_bytes_discarded         = 0;
                        *stat_tx_data_pdu                     = 0;
                        *stat_tx_data_bytes                   = 0;
                        *stat_tx_retransmit_pdu_by_status     = 0;
                        *stat_tx_retransmit_bytes_by_status   = 0;
                        *stat_tx_retransmit_pdu               = 0;
                        *stat_tx_retransmit_bytes             = 0;
                        *stat_tx_control_pdu                  = 0;
                        *stat_tx_control_bytes                = 0;
                        *stat_rx_pdcp_sdu                     = 0;
                        *stat_rx_pdcp_bytes                   = 0;
                        *stat_rx_data_pdus_duplicate          = 0;
                        *stat_rx_data_bytes_duplicate         = 0;
                        *stat_rx_data_pdu                     = 0;
                        *stat_rx_data_bytes                   = 0;
                        *stat_rx_data_pdu_dropped             = 0;
                        *stat_rx_data_bytes_dropped           = 0;
                        *stat_rx_data_pdu_out_of_window       = 0;
                        *stat_rx_data_bytes_out_of_window     = 0;
                        *stat_rx_control_pdu                  = 0;
                        *stat_rx_control_bytes                = 0;
                        *stat_timer_reordering_timed_out      = 0;
                        *stat_timer_poll_retransmit_timed_out = 0;
                        *stat_timer_status_prohibit_timed_out = 0;
                        return RLC_OP_STATUS_BAD_PARAMETER;
                        break;

                    default:
                        *stat_tx_pdcp_sdu                     = 0;
                        *stat_tx_pdcp_bytes                   = 0;
                        *stat_tx_pdcp_sdu_discarded           = 0;
                        *stat_tx_pdcp_bytes_discarded         = 0;
                        *stat_tx_data_pdu                     = 0;
                        *stat_tx_data_bytes                   = 0;
                        *stat_tx_retransmit_pdu_by_status     = 0;
                        *stat_tx_retransmit_bytes_by_status   = 0;
                        *stat_tx_retransmit_pdu               = 0;
                        *stat_tx_retransmit_bytes             = 0;
                        *stat_tx_control_pdu                  = 0;
                        *stat_tx_control_bytes                = 0;
                        *stat_rx_pdcp_sdu                     = 0;
                        *stat_rx_pdcp_bytes                   = 0;
                        *stat_rx_data_pdus_duplicate          = 0;
                        *stat_rx_data_bytes_duplicate         = 0;
                        *stat_rx_data_pdu                     = 0;
                        *stat_rx_data_bytes                   = 0;
                        *stat_rx_data_pdu_dropped             = 0;
                        *stat_rx_data_bytes_dropped           = 0;
                        *stat_rx_data_pdu_out_of_window       = 0;
                        *stat_rx_data_bytes_out_of_window     = 0;
                        *stat_rx_control_pdu                  = 0;
                        *stat_rx_control_bytes                = 0;
                        *stat_timer_reordering_timed_out      = 0;
                        *stat_timer_poll_retransmit_timed_out = 0;
                        *stat_timer_status_prohibit_timed_out = 0;
                        return RLC_OP_STATUS_BAD_PARAMETER;
                  }
      } else {
          return RLC_OP_STATUS_BAD_PARAMETER;
      }
  } else {
      return RLC_OP_STATUS_BAD_PARAMETER;
  }
}
//-----------------------------------------------------------------------------
rlc_op_status_t rlc_data_req     (module_id_t module_idP, u32_t frame, u8_t eNB_flagP, u8_t MBMS_flagP, rb_id_t rb_idP, mui_t muiP, confirm_t confirmP, sdu_size_t sdu_sizeP, mem_block_t *sduP) {
//-----------------------------------------------------------------------------
  mem_block_t* new_sdu;
#ifdef Rel10
  rb_id_t      mbms_rb_id = 0;
#endif
#ifdef DEBUG_RLC_DATA_REQ
  LOG_D(RLC,"rlc_data_req: module_idP %d (%d), rb_idP %d (%d), muip %d, confirmP %d, sud_sizeP %d, sduP %p\n",
      module_idP,MAX_MODULES,rb_idP,MAX_RAB,muiP,confirmP,sdu_sizeP,sduP);
#endif
#ifdef Rel10
  DevCheck((module_idP < MAX_MODULES), module_idP, MAX_MODULES, MBMS_flagP);
#else
  DevCheck((module_idP < MAX_MODULES) && (MBMS_flagP == 0), module_idP, MAX_MODULES, MBMS_flagP);
#endif
  DevCheck(rb_idP < MAX_RAB, rb_idP, MAX_RB, 0);
  DevAssert(sduP != NULL);
  DevCheck(sdu_sizeP > 0, sdu_sizeP, 0, 0);

#ifndef Rel10
  DevCheck(MBMS_flagP == 0, MBMS_flagP, 0, 0);
#endif

  if (MBMS_flagP == 0) {
                  LOG_D(RLC, "[FRAME %05d][RLC][MOD %02d][RB %02d] Display of rlc_data_req:\n",
                                 frame, module_idP, rb_idP);
                  rlc_util_print_hex_octets(RLC, (unsigned char*)sduP->data, sdu_sizeP);

#ifdef DEBUG_RLC_DATA_REQ
                  LOG_D(RLC,"RLC_TYPE : %d ",rlc[module_idP].m_rlc_pointer[rb_idP].rlc_type);
#endif
                  switch (rlc[module_idP].m_rlc_pointer[rb_idP].rlc_type) {
                    case RLC_NONE:
                        free_mem_block(sduP);
      LOG_E(RLC, "Received RLC_NONE as rlc_type for module_idP %d, rb_id %d, eNB_flag %d\n", module_idP, rb_idP, eNB_flagP);
                        return RLC_OP_STATUS_BAD_PARAMETER;

                    case RLC_AM:
#ifdef DEBUG_RLC_DATA_REQ
		                msg("RLC_AM\n");
#endif
                        new_sdu = get_free_mem_block (sdu_sizeP + sizeof (struct rlc_am_data_req_alloc));

                        if (new_sdu != NULL) {
                          // PROCESS OF COMPRESSION HERE:
                          memset (new_sdu->data, 0, sizeof (struct rlc_am_data_req_alloc));
                          memcpy (&new_sdu->data[sizeof (struct rlc_am_data_req_alloc)], &sduP->data[0], sdu_sizeP);

                          ((struct rlc_am_data_req *) (new_sdu->data))->data_size = sdu_sizeP;
                          ((struct rlc_am_data_req *) (new_sdu->data))->conf = confirmP;
                          ((struct rlc_am_data_req *) (new_sdu->data))->mui  = muiP;
                          ((struct rlc_am_data_req *) (new_sdu->data))->data_offset = sizeof (struct rlc_am_data_req_alloc);
                    	  free_mem_block(sduP);
                          LOG_D(RLC, "%s\n",RLC_FG_BRIGHT_COLOR_RED);

                          if (rlc[module_idP].m_rlc_am_array[rlc[module_idP].m_rlc_pointer[rb_idP].rlc_index].is_data_plane) {
                              LOG_D(RLC, "[MSC_MSG][FRAME %05d][PDCP][MOD %02d][RB %02d][--- RLC_AM_DATA_REQ/%d Bytes --->][RLC_AM][MOD %02d][RB %02d]\n",
                                 frame, module_idP, rb_idP, sdu_sizeP, module_idP, rb_idP);
                          } else {
                              if (eNB_flagP) {
                                  LOG_D(RLC, "[MSC_MSG][FRAME %05d][RRC_eNB][MOD %02d][][--- RLC_AM_DATA_REQ/%d Bytes --->][RLC_AM][MOD %02d][RB %02d]\n",
                                     frame, module_idP, sdu_sizeP, module_idP, rb_idP);
                              } else {
                                  LOG_D(RLC, "[MSC_MSG][FRAME %05d][RRC_UE][MOD %02d][][--- RLC_AM_DATA_REQ/%d Bytes --->][RLC_AM][MOD %02d][RB %02d]\n",
                                     frame, module_idP,  sdu_sizeP, module_idP, rb_idP);
                              }
                          }
                          LOG_D(RLC, "%s\n",RLC_FG_COLOR_DEFAULT);
                          rlc_am_data_req(&rlc[module_idP].m_rlc_am_array[rlc[module_idP].m_rlc_pointer[rb_idP].rlc_index], frame, new_sdu);
                          return RLC_OP_STATUS_OK;
                        } else {
                          return RLC_OP_STATUS_INTERNAL_ERROR;
                        }
                        break;

                    case RLC_UM:
                        new_sdu = get_free_mem_block (sdu_sizeP + sizeof (struct rlc_um_data_req_alloc));

                        if (new_sdu != NULL) {
                          // PROCESS OF COMPRESSION HERE:
                          memset (new_sdu->data, 0, sizeof (struct rlc_um_data_req_alloc));
                          memcpy (&new_sdu->data[sizeof (struct rlc_um_data_req_alloc)], &sduP->data[0], sdu_sizeP);

                          ((struct rlc_um_data_req *) (new_sdu->data))->data_size = sdu_sizeP;
                          ((struct rlc_um_data_req *) (new_sdu->data))->data_offset = sizeof (struct rlc_um_data_req_alloc);
                          free_mem_block(sduP);

                          LOG_D(RLC, "%s\n",RLC_FG_BRIGHT_COLOR_RED);
                          if (rlc[module_idP].m_rlc_um_array[rlc[module_idP].m_rlc_pointer[rb_idP].rlc_index].is_data_plane) {
                              LOG_D(RLC, "[MSC_MSG][FRAME %05d][PDCP][MOD %02d][RB %02d][--- RLC_UM_DATA_REQ/%d Bytes --->][RLC_UM][MOD %02d][RB %02d]\n",
                                 frame, module_idP, rb_idP, sdu_sizeP, module_idP, rb_idP);
                          } else {
                              if (eNB_flagP) {
                                  LOG_D(RLC, "[MSC_MSG][FRAME %05d][RRC_eNB][MOD %02d][][--- RLC_UM_DATA_REQ/%d Bytes --->][RLC_UM][MOD %02d][RB %02d]\n",
                                     frame, module_idP,  sdu_sizeP, module_idP, rb_idP);
                              } else {
                                  LOG_D(RLC, "[MSC_MSG][FRAME %05d][RRC_UE][MOD %02d][][--- RLC_UM_DATA_REQ/%d Bytes --->][RLC_UM][MOD %02d][RB %02d]\n",
                                     frame, module_idP,  sdu_sizeP, module_idP, rb_idP);
                              }
                          }
                          LOG_D(RLC, "%s\n",RLC_FG_COLOR_DEFAULT);
                          rlc_um_data_req(&rlc[module_idP].m_rlc_um_array[rlc[module_idP].m_rlc_pointer[rb_idP].rlc_index], frame, new_sdu);

                          //free_mem_block(new_sdu);
                          return RLC_OP_STATUS_OK;
                        } else {
                          return RLC_OP_STATUS_INTERNAL_ERROR;
                        }
                        break;

                    case RLC_TM:
                        new_sdu = get_free_mem_block (sdu_sizeP + sizeof (struct rlc_tm_data_req_alloc));

                        if (new_sdu != NULL) {
                          // PROCESS OF COMPRESSION HERE:
                          memset (new_sdu->data, 0, sizeof (struct rlc_tm_data_req_alloc));
                          memcpy (&new_sdu->data[sizeof (struct rlc_tm_data_req_alloc)], &sduP->data[0], sdu_sizeP);

                          ((struct rlc_tm_data_req *) (new_sdu->data))->data_size = sdu_sizeP;
                          ((struct rlc_tm_data_req *) (new_sdu->data))->data_offset = sizeof (struct rlc_tm_data_req_alloc);
                          free_mem_block(sduP);
                          LOG_D(RLC, "%s\n",RLC_FG_BRIGHT_COLOR_RED);
                          if (rlc[module_idP].m_rlc_tm_array[rlc[module_idP].m_rlc_pointer[rb_idP].rlc_index].is_data_plane) {
                              LOG_D(RLC, "[MSC_MSG][FRAME %05d][PDCP][MOD %02d][RB %02d][--- RLC_TM_DATA_REQ/%d Bytes --->][RLC_TM][MOD %02d][RB %02d]\n",
                                 frame, module_idP, rb_idP, sdu_sizeP, module_idP, rb_idP);
                          } else {
                              if (eNB_flagP) {
                                  LOG_D(RLC, "[MSC_MSG][FRAME %05d][RRC_eNB][MOD %02d][][--- RLC_TM_DATA_REQ/%d Bytes --->][RLC_TM][MOD %02d][RB %02d]\n",
                                     frame, module_idP, rb_idP, sdu_sizeP, module_idP, rb_idP);
                              } else {
                                  LOG_D(RLC, "[MSC_MSG][FRAME %05d][RRC_UE][MOD %02d][][--- RLC_TM_DATA_REQ/%d Bytes --->][RLC_TM][MOD %02d][RB %02d]\n",
                                     frame, module_idP, rb_idP, sdu_sizeP, module_idP, rb_idP);
                              }
                          }
                          LOG_D(RLC, "%s\n",RLC_FG_COLOR_DEFAULT);
                          rlc_tm_data_req(&rlc[module_idP].m_rlc_tm_array[rlc[module_idP].m_rlc_pointer[rb_idP].rlc_index], new_sdu);
                          return RLC_OP_STATUS_OK;
                        } else {
                          //handle_event(ERROR,"FILE %s FONCTION rlc_data_req() LINE %s : out of memory\n", __FILE__, __LINE__);
                          return RLC_OP_STATUS_INTERNAL_ERROR;
                        }
                        break;

                    default:
                   free_mem_block(sduP);
                        //handle_event(ERROR,"FILE %s FONCTION rlc_data_req() LINE %s : no RLC found for this radio bearer %d\n", __FILE__, __LINE__, rb_idP);
                        return RLC_OP_STATUS_INTERNAL_ERROR;

                  }

#ifdef Rel10
  } else { /* MBMS_flag != 0 */
      if (rb_idP < (maxSessionPerPMCH * maxServiceCount)) {
          if (eNB_flagP) {
              mbms_rb_id = rb_idP + (maxDRB + 3) * MAX_MOBILES_PER_RG;
          } else {
              mbms_rb_id = rb_idP + (maxDRB + 3);
          }
	  //  LOG_I(RLC,"DUY rlc_data_req: mbms_rb_id in RLC instant is: %d\n", mbms_rb_id);
          if (sduP != NULL) {
              if (sdu_sizeP > 0) {
                  LOG_I(RLC,"received a packet with size %d for MBMS \n", sdu_sizeP);
                  new_sdu = get_free_mem_block (sdu_sizeP + sizeof (struct rlc_um_data_req_alloc));
                  if (new_sdu != NULL) {
                      // PROCESS OF COMPRESSION HERE:
                      memset (new_sdu->data, 0, sizeof (struct rlc_um_data_req_alloc));
                      memcpy (&new_sdu->data[sizeof (struct rlc_um_data_req_alloc)], &sduP->data[0], sdu_sizeP);
                      ((struct rlc_um_data_req *) (new_sdu->data))->data_size = sdu_sizeP;
                      ((struct rlc_um_data_req *) (new_sdu->data))->data_offset = sizeof (struct rlc_um_data_req_alloc);
                      free_mem_block(sduP);
                      LOG_D(RLC, "%s\n",RLC_FG_BRIGHT_COLOR_RED);
                      if (rlc[module_idP].m_rlc_um_array[rlc[module_idP].m_rlc_pointer[mbms_rb_id].rlc_index].is_data_plane) {
                          LOG_D(RLC, "[MSC_MSG][FRAME %05d][PDCP][MOD %02d][RB %02d][--- RLC_UM_DATA_REQ/%d Bytes (MBMS) --->][RLC_UM][MOD %02d][RB %02d]\n",
                            frame, module_idP, mbms_rb_id, sdu_sizeP, module_idP, mbms_rb_id);
                      } else {
                          if (eNB_flagP) {
                              LOG_D(RLC, "[MSC_MSG][FRAME %05d][RRC_eNB][MOD %02d][][--- RLC_UM_DATA_REQ/%d Bytes (MBMS) --->][RLC_UM][MOD %02d][RB %02d]\n",
                                 frame, module_idP,  sdu_sizeP, module_idP, mbms_rb_id);
                          } else {
                              LOG_D(RLC, "[MSC_MSG][FRAME %05d][RRC_UE][MOD %02d][][--- RLC_UM_DATA_REQ/%d Bytes (MBMS) --->][RLC_UM][MOD %02d][RB %02d]\n",
                                 frame, module_idP,  sdu_sizeP, module_idP, mbms_rb_id);
                          }
                      }
                      LOG_D(RLC, "%s\n",RLC_FG_COLOR_DEFAULT);
                      rlc_um_data_req(&rlc[module_idP].m_rlc_um_array[rlc[module_idP].m_rlc_pointer[mbms_rb_id].rlc_index], frame, new_sdu);

                      //free_mem_block(new_sdu);
                      return RLC_OP_STATUS_OK;
                  } else {
                      return RLC_OP_STATUS_BAD_PARAMETER;
                  }
              } else {
                  return RLC_OP_STATUS_BAD_PARAMETER;
              }
          } else {
              return RLC_OP_STATUS_BAD_PARAMETER;
          }
      } else {
	return RLC_OP_STATUS_BAD_PARAMETER;
      }
  }
#else
  } else {/* MBMS_flag != 0 */
    free_mem_block(sduP);
    LOG_E(RLC, "MBMS_flag != 0 while Rel10 is not defined...\n");
    //handle_event(ERROR,"FILE %s FONCTION rlc_data_req() LINE %s : parameter module_id out of bounds :%d\n", __FILE__, __LINE__, module_idP);
    return RLC_OP_STATUS_BAD_PARAMETER;
  }
#endif
}

//-----------------------------------------------------------------------------
void rlc_data_ind     (module_id_t module_idP, u8_t eNB_id, u8_t UE_id, u32_t frame, u8_t eNB_flag, u8_t MBMS_flagP, rb_id_t rb_idP, sdu_size_t sdu_sizeP, mem_block_t* sduP, boolean_t is_data_planeP) {
//-----------------------------------------------------------------------------
  char *from_str;
  char *to_str;
  u8_t from_value;
  u8_t to_value;

  if (eNB_flag == 0) {
    from_str = "UE";
    to_str = "eNB";
    from_value = UE_id;
    to_value = eNB_id;
  } else {
    from_str = "eNB";
    to_str = "UE";
    from_value = eNB_id;
    to_value = UE_id;
  }
    LOG_D(RLC, "[FRAME %05d][RLC][MOD %02d][RB %02d] Display of rlc_data_ind:\n", frame, module_idP, rb_idP);
    rlc_util_print_hex_octets(RLC, (unsigned char*)sduP->data, sdu_sizeP);
    //check_mem_area();
  // now demux is done at PDCP
  //  if ((is_data_planeP)) {
#ifdef DEBUG_RLC_PDCP_INTERFACE
      msg("[RLC] TTI %d, INST %d : Receiving SDU (%p) of size %d bytes to Rb_id %d\n",
	  frame, module_idP,
	  sduP,
	  sdu_sizeP,
          rb_idP);
#endif //DEBUG_RLC_PDCP_INTERFACE
      switch (rlc[module_idP].m_rlc_pointer[rb_idP].rlc_type) {
         case RLC_AM:
    LOG_D(RLC, "[MSC_MSG][FRAME %05d][RLC_AM][%s %02d][RB %02d][--- RLC_DATA_IND/%d Bytes --->][PDCP][%s %02d][RB %02d]\n",
        frame, from_str, from_value, rb_idP, sdu_sizeP, to_str, to_value, rb_idP);
             break;
         case RLC_UM:
    LOG_D(RLC, "[MSC_MSG][FRAME %05d][RLC_UM][%s %02d][RB %02d][--- RLC_DATA_IND/%d Bytes --->][PDCP][%s %02d][RB %02d]\n",
        frame, from_str, from_value, rb_idP, sdu_sizeP, to_str, to_value, rb_idP);
             break;
         case RLC_TM:
    LOG_D(RLC, "[MSC_MSG][FRAME %05d][RLC_TM][%s %02d][RB %02d][--- RLC_DATA_IND/%d Bytes --->][PDCP][%s %02d][RB %02d]\n",
        frame, from_str, from_value, rb_idP, sdu_sizeP, to_str, to_value, rb_idP);
             break;
      }
  pdcp_data_ind (eNB_id, UE_id, frame, eNB_flag, MBMS_flagP, rb_idP % NB_RB_MAX, sdu_sizeP, sduP, is_data_planeP);
}
//-----------------------------------------------------------------------------
void rlc_data_conf     (module_id_t module_idP, u32_t frame, u8_t eNB_flag, rb_id_t rb_idP, mui_t muiP, rlc_tx_status_t statusP, boolean_t is_data_planeP) {
//-----------------------------------------------------------------------------
    if (!(is_data_planeP)) {
        if (rlc_rrc_data_conf != NULL) {
            LOG_D(RLC, "%s\n",RLC_FG_BRIGHT_COLOR_RED);
            switch (rlc[module_idP].m_rlc_pointer[rb_idP].rlc_type) {
                case RLC_AM:
                    LOG_D(RLC, "[MSC_MSG][FRAME %05d][RLC_AM][MOD %02d][RB %02d][--- RLC_DATA_CONF /MUI %d --->][RRC_%s][MOD %02d][][RLC_DATA_CONF/ MUI %d]\n",frame, module_idP,rb_idP, ( eNB_flag == 1) ? "eNB":"UE", muiP, module_idP);
                    break;
                case RLC_UM:
                    LOG_D(RLC, "[MSC_MSG][FRAME %05d][RLC_UM][MOD %02d][RB %02d][--- RLC_DATA_CONF /MUI %d --->][RRC_%s][MOD %02d][][RLC_DATA_CONF/ MUI %d]\n",frame, module_idP,rb_idP, ( eNB_flag == 1) ? "eNB":"UE", muiP, module_idP);
                    break;
                case RLC_TM:
                    LOG_D(RLC, "[MSC_MSG][FRAME %05d][RLC_TM][MOD %02d][RB %02d][--- RLC_DATA_CONF /MUI %d --->][RRC_%s][MOD %02d][][RLC_DATA_CONF/ MUI %d]\n",frame, module_idP,rb_idP, ( eNB_flag == 1) ? "eNB":"UE", muiP, module_idP);
                    break;
            }
            LOG_D(RLC, "%s\n",RLC_FG_COLOR_DEFAULT);
            rlc_rrc_data_conf (module_idP , rb_idP , muiP, statusP);
        }
    }
}
//-----------------------------------------------------------------------------
int
rlc_module_init (void)
{
//-----------------------------------------------------------------------------
   int i;
   LOG_D(RLC, "MODULE INIT\n");
   rlc_rrc_data_ind  = NULL;
   rlc_rrc_data_conf = NULL;
   memset(rlc, 0, sizeof(rlc_t) * MAX_MODULES);
   for (i=0; i < MAX_MODULES; i++) {
     memset(rlc[i].m_lcid2rbid, -1, sizeof(rb_id_t)*RLC_MAX_LC);
   }
   pool_buffer_init();

   return(0);
}
//-----------------------------------------------------------------------------
void
rlc_module_cleanup (void)
//-----------------------------------------------------------------------------
{
}
//-----------------------------------------------------------------------------
void
rlc_layer_init (void)
{
//-----------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------
void
rlc_layer_cleanup (void)
//-----------------------------------------------------------------------------
{
}


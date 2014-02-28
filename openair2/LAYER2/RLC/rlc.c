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
#include "UTIL/OCG/OCG_vars.h"

#include "assertions.h"

extern void pdcp_data_ind (module_id_t enb_mod_idP, module_id_t ue_mod_idP, frame_t frameP, eNB_flag_t enb_flagP, MBMS_flag_t MBMS_flag, rb_id_t rab_idP, sdu_size_t data_sizeP, mem_block_t * sdu_pP, uint8_t is_data_plane);

#define DEBUG_RLC_PDCP_INTERFACE

#define DEBUG_RLC_DATA_REQ 1

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
rlc_op_status_t rlc_stat_req     (
                  module_id_t   enb_mod_idP,
                  module_id_t   ue_mod_idP,
                  frame_t       frameP,
                  eNB_flag_t    enb_flagP,
                  rb_id_t       rb_idP,
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
    rlc_mode_t             rlc_mode = RLC_MODE_NONE;
    void                  *rlc_p      = NULL;

#ifdef OAI_EMU
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
#endif
    AssertFatal (rb_idP < NB_RB_MAX, "RB id is too high (%u/%d)!\n", rb_idP, NB_RB_MAX);
    if (enb_flagP) {
        rlc_mode = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].mode;
        switch (rlc_mode) {
          case RLC_MODE_NONE:
              AssertFatal (0 , "enB RLC not configured rb id %u  module eNB id %u!\n", rb_idP, enb_mod_idP);
              break;
          case RLC_MODE_AM:
              rlc_p = (void*)&rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.am;
              break;
          case RLC_MODE_UM:
              rlc_p = (void*)&rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.um;
              break;
          case RLC_MODE_TM:
              rlc_p = (void*)&rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.tm;
              break;
          default:
              AssertFatal (0 , "enB RLC internal memory error rb id %u module eNB id %u!\n", rb_idP, enb_mod_idP);
        }
    } else {
        rlc_mode = rlc_array_ue[ue_mod_idP][rb_idP].mode;
        switch (rlc_mode) {
          case RLC_MODE_NONE:
              AssertFatal (0 , "UE RLC not configured rb id %u module ue id %u!\n", rb_idP, ue_mod_idP);
              break;
          case RLC_MODE_AM:
              rlc_p = (void*)&rlc_array_ue[ue_mod_idP][rb_idP].rlc.am;
              break;
          case RLC_MODE_UM:
              rlc_p = (void*)&rlc_array_ue[ue_mod_idP][rb_idP].rlc.um;
              break;
          case RLC_MODE_TM:
              rlc_p = (void*)&rlc_array_ue[ue_mod_idP][rb_idP].rlc.tm;
              break;
          default:
              AssertFatal (0 , "UE RLC internal memory error rb id %u module ue id %u!\n", rb_idP, ue_mod_idP);
        }
    }
    switch (rlc_mode) {
        case RLC_MODE_NONE:
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

        case RLC_MODE_AM:
            rlc_am_stat_req((rlc_am_entity_t*)rlc_p,
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

       case RLC_MODE_UM:
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
           rlc_um_stat_req ((rlc_um_entity_t*)rlc_p,
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

       case RLC_MODE_TM:
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

           *stat_timer_poll_retransmit_timed_out = 0;
           *stat_timer_status_prohibit_timed_out = 0;
           return RLC_OP_STATUS_BAD_PARAMETER;
    }
}
//-----------------------------------------------------------------------------
rlc_op_status_t rlc_data_req     (module_id_t  enb_mod_idP,
                                  module_id_t  ue_mod_idP,
                                  frame_t      frameP,
                                  eNB_flag_t   enb_flagP,
                                  MBMS_flag_t  MBMS_flagP,
                                  rb_id_t      rb_idP,
                                  mui_t        muiP,
                                  confirm_t    confirmP,
                                  sdu_size_t   sdu_sizeP,
                                  mem_block_t *sdu_pP) {
//-----------------------------------------------------------------------------
  mem_block_t           *new_sdu_p    = NULL;
  rlc_mode_t             rlc_mode     = RLC_MODE_NONE;
  void                  *rlc_p        = NULL;
#ifdef Rel10
  rb_id_t                mbms_rb_id = 0;
  rlc_um_entity_t       *rlc_um_p   = NULL;
#endif
#ifdef DEBUG_RLC_DATA_REQ
  LOG_D(RLC,"rlc_data_req: %s enb id  %u  ue id %u, rb_id %u (MAX %d), muip %d, confirmP %d, sud_sizeP %d, sdu_pP %p\n",
        (enb_flagP) ? "eNB" : "UE",
        enb_mod_idP,
        ue_mod_idP,
        rb_idP,
        NB_RAB_MAX,
        muiP,
        confirmP,
        sdu_sizeP,
        sdu_pP);
#endif
#ifdef Rel10
#else
  AssertFatal(MBMS_flagP == 0, "MBMS_flagP %u", MBMS_flagP);
#endif
#ifdef OAI_EMU
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
#endif
  AssertFatal (rb_idP < NB_RB_MAX, "RB id is too high (%u/%d)!\n", rb_idP, NB_RB_MAX);
  DevAssert(sdu_pP != NULL);
  DevCheck(sdu_sizeP > 0, sdu_sizeP, 0, 0);

#ifndef Rel10
  DevCheck(MBMS_flagP == 0, MBMS_flagP, 0, 0);
#endif

  if (enb_flagP) {
      rlc_mode = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].mode;
      switch (rlc_mode) {
        case RLC_MODE_NONE:
            AssertFatal (0 , "enB RLC not configured rb id %u module %u!\n", rb_idP, enb_mod_idP);
            break;
        case RLC_MODE_AM:
            rlc_p = (void*)&rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.am;
            break;
        case RLC_MODE_UM:
            rlc_p = (void*)&rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.um;
            break;
        case RLC_MODE_TM:
            rlc_p = (void*)&rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.tm;
            break;
        default:
            AssertFatal (0 , "enB RLC internal memory error rb id %u module %u!\n", rb_idP, enb_mod_idP);
      }
  } else {
      rlc_mode = rlc_array_ue[ue_mod_idP][rb_idP].mode;
      switch (rlc_mode) {
        case RLC_MODE_NONE:
            AssertFatal (0 , "UE RLC not configured rb id %u module %u!\n", rb_idP, ue_mod_idP);
            break;
        case RLC_MODE_AM:
            rlc_p = (void*)&rlc_array_ue[ue_mod_idP][rb_idP].rlc.am;
            break;
        case RLC_MODE_UM:
            rlc_p = (void*)&rlc_array_ue[ue_mod_idP][rb_idP].rlc.um;
            break;
        case RLC_MODE_TM:
            rlc_p = (void*)&rlc_array_ue[ue_mod_idP][rb_idP].rlc.tm;
            break;
        default:
            AssertFatal (0 , "UE RLC internal memory error rb id %u module %u!\n", rb_idP, ue_mod_idP);
      }
  }


  if (MBMS_flagP == 0) {
      LOG_D(RLC, "[FRAME %5u][%s][RLC][INST %u/%u][RB %u] Display of rlc_data_req:\n",
          frameP,
          (enb_flagP) ? "eNB" : "UE",
          enb_mod_idP,
          ue_mod_idP,
          rb_idP);
      rlc_util_print_hex_octets(RLC, (unsigned char*)sdu_pP->data, sdu_sizeP);

#ifdef DEBUG_RLC_DATA_REQ
      LOG_D(RLC,"RLC_TYPE : %d ",rlc_mode);
#endif
      switch (rlc_mode) {
          case RLC_MODE_NONE:
              free_mem_block(sdu_pP);
              LOG_E(RLC, "Received RLC_MODE_NONE as rlc_type for %s eNB id  %u, ue id %u, rb_id %u\n",
                    (enb_flagP) ? "eNB" : "UE",
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP);
              return RLC_OP_STATUS_BAD_PARAMETER;

          case RLC_MODE_AM:
#ifdef DEBUG_RLC_DATA_REQ
              msg("RLC_MODE_AM\n");
#endif
              new_sdu_p = get_free_mem_block (sdu_sizeP + sizeof (struct rlc_am_data_req_alloc));

              if (new_sdu_p != NULL) {
                  // PROCESS OF COMPRESSION HERE:
                  memset (new_sdu_p->data, 0, sizeof (struct rlc_am_data_req_alloc));
                  memcpy (&new_sdu_p->data[sizeof (struct rlc_am_data_req_alloc)], &sdu_pP->data[0], sdu_sizeP);

                  ((struct rlc_am_data_req *) (new_sdu_p->data))->data_size = sdu_sizeP;
                  ((struct rlc_am_data_req *) (new_sdu_p->data))->conf = confirmP;
                  ((struct rlc_am_data_req *) (new_sdu_p->data))->mui  = muiP;
                  ((struct rlc_am_data_req *) (new_sdu_p->data))->data_offset = sizeof (struct rlc_am_data_req_alloc);
                  free_mem_block(sdu_pP);
                  LOG_D(RLC, "%s\n",RLC_FG_BRIGHT_COLOR_RED);

                  if (((rlc_am_entity_t*)rlc_p)->is_data_plane) {
                      LOG_D(RLC, "[FRAME %5u][%s][PDCP][INST %u/%u][RB %u][--- RLC_AM_DATA_REQ/%d Bytes --->][RLC_AM][INST %u/%u][RB %u]\n",
                          frameP,
                          (enb_flagP) ? "eNB" : "UE",
                          enb_mod_idP,
                          ue_mod_idP,
                          rb_idP,
                          sdu_sizeP,
                          ue_mod_idP,
                          rb_idP,
                          rb_idP);
                  } else {
                      LOG_D(RLC, "[FRAME %5u][%s][RRC][INST %u/%u][][--- RLC_AM_DATA_REQ/%d Bytes --->][RLC_AM][INST %u/%u][RB %u]\n",
                          frameP,
                          (enb_flagP) ? "eNB" : "UE",
                          enb_mod_idP,
                          ue_mod_idP,
                          rb_idP,
                          sdu_sizeP,
                          enb_mod_idP,
                          ue_mod_idP,
                          rb_idP);
                  }
                  LOG_D(RLC, "%s\n",RLC_FG_COLOR_DEFAULT);
                  rlc_am_data_req((rlc_am_entity_t*)rlc_p, frameP, new_sdu_p);
                  return RLC_OP_STATUS_OK;
              } else {
                  return RLC_OP_STATUS_INTERNAL_ERROR;
              }
              break;

          case RLC_MODE_UM:
            new_sdu_p = get_free_mem_block (sdu_sizeP + sizeof (struct rlc_um_data_req_alloc));

              if (new_sdu_p != NULL) {
                  // PROCESS OF COMPRESSION HERE:
                  memset (new_sdu_p->data, 0, sizeof (struct rlc_um_data_req_alloc));
                  memcpy (&new_sdu_p->data[sizeof (struct rlc_um_data_req_alloc)], &sdu_pP->data[0], sdu_sizeP);

                  ((struct rlc_um_data_req *) (new_sdu_p->data))->data_size = sdu_sizeP;
                  ((struct rlc_um_data_req *) (new_sdu_p->data))->data_offset = sizeof (struct rlc_um_data_req_alloc);
                  free_mem_block(sdu_pP);

                  LOG_D(RLC, "%s\n",RLC_FG_BRIGHT_COLOR_RED);
                  if (((rlc_am_entity_t*)rlc_p)->is_data_plane) {
                      LOG_D(RLC, "[FRAME %5u][%s][PDCP][INST %u/%u][RB %u][--- RLC_UM_DATA_REQ/%d Bytes --->][RLC_UM][INST %u/%u][RB %u]\n",
                          frameP,
                          (enb_flagP) ? "eNB" : "UE",
                          enb_mod_idP,
                          ue_mod_idP,
                          rb_idP,
                          sdu_sizeP,
                          enb_mod_idP,
                          ue_mod_idP,
                          rb_idP);
                  } else {
                      LOG_D(RLC, "[FRAME %5u][%s][RRC][INST %u/%u][][--- RLC_UM_DATA_REQ/%d Bytes --->][RLC_UM][INST %u/%u][RB %u]\n",
                                     frameP,
                                     (enb_flagP) ? "eNB" : "UE",
                                     enb_mod_idP,
                                     ue_mod_idP,
                                     rb_idP,
                                     sdu_sizeP,
                                     enb_mod_idP,
                                     ue_mod_idP,
                                     rb_idP);
                  }
                  LOG_D(RLC, "%s\n",RLC_FG_COLOR_DEFAULT);
                  rlc_um_data_req((rlc_um_entity_t*)rlc_p, frameP, new_sdu_p);

                  //free_mem_block(new_sdu);
                  return RLC_OP_STATUS_OK;
              } else {
                  return RLC_OP_STATUS_INTERNAL_ERROR;
              }
              break;

          case RLC_MODE_TM:
            new_sdu_p = get_free_mem_block (sdu_sizeP + sizeof (struct rlc_tm_data_req_alloc));

              if (new_sdu_p != NULL) {
                  // PROCESS OF COMPRESSION HERE:
                  memset (new_sdu_p->data, 0, sizeof (struct rlc_tm_data_req_alloc));
                  memcpy (&new_sdu_p->data[sizeof (struct rlc_tm_data_req_alloc)], &sdu_pP->data[0], sdu_sizeP);

                  ((struct rlc_tm_data_req *) (new_sdu_p->data))->data_size = sdu_sizeP;
                  ((struct rlc_tm_data_req *) (new_sdu_p->data))->data_offset = sizeof (struct rlc_tm_data_req_alloc);
                  free_mem_block(sdu_pP);
                  LOG_D(RLC, "%s\n",RLC_FG_BRIGHT_COLOR_RED);
                  if (((rlc_tm_entity_t*)rlc_p)->is_data_plane) {
                      LOG_D(RLC, "[FRAME %5u][%s][PDCP][INST %u/%u][RB %u][--- RLC_TM_DATA_REQ/%d Bytes --->][RLC_TM][INST %u/%u][RB %u]\n",
                                 frameP,
                                 (enb_flagP) ? "eNB" : "UE",
                                 enb_mod_idP,
                                 ue_mod_idP,
                                 rb_idP,
                                 sdu_sizeP,
                                 enb_mod_idP,
                                 ue_mod_idP,
                                 rb_idP);
                  } else {
                       LOG_D(RLC, "[FRAME %5u][%s][RRC][INST %u/%u][][--- RLC_TM_DATA_REQ/%d Bytes --->][RLC_TM][INST %u/%u][RB %u]\n",
                                     frameP,
                                     (enb_flagP) ? "eNB" : "UE",
                                     enb_mod_idP,
                                     ue_mod_idP,
                                     rb_idP,
                                     sdu_sizeP,
                                     enb_mod_idP,
                                     ue_mod_idP,
                                     rb_idP);
                  }
                  LOG_D(RLC, "%s\n",RLC_FG_COLOR_DEFAULT);
                  rlc_tm_data_req((rlc_tm_entity_t*)rlc_p, new_sdu_p);
                  return RLC_OP_STATUS_OK;
              } else {
                  //handle_event(ERROR,"FILE %s FONCTION rlc_data_req() LINE %s : out of memory\n", __FILE__, __LINE__);
                  return RLC_OP_STATUS_INTERNAL_ERROR;
              }
              break;

          default:
              free_mem_block(sdu_pP);
              return RLC_OP_STATUS_INTERNAL_ERROR;

      }

#ifdef Rel10
  } else { /* MBMS_flag != 0 */
      if (rb_idP < (maxSessionPerPMCH * maxServiceCount)) {
          if (enb_flagP) {
              mbms_rb_id = rb_idP + (maxDRB + 3) * MAX_MOBILES_PER_RG;
              //rlc_um_p   = rlc_mbms_array_eNB[enb_mod_idP][mbms_rb_id].;
          } else {
              mbms_rb_id = rb_idP + (maxDRB + 3);
              //rlc_um_p   = rlc_mbms_array_ue[ue_mod_idP][mbms_rb_id];
          }
	  //  LOG_I(RLC,"DUY rlc_data_req: mbms_rb_id in RLC instant is: %d\n", mbms_rb_id);
          if (sdu_pP != NULL) {
              if (sdu_sizeP > 0) {
                  LOG_I(RLC,"received a packet with size %d for MBMS \n", sdu_sizeP);
                  new_sdu_p = get_free_mem_block (sdu_sizeP + sizeof (struct rlc_um_data_req_alloc));
                  if (new_sdu_p != NULL) {
                      // PROCESS OF COMPRESSION HERE:
                      memset (new_sdu_p->data, 0, sizeof (struct rlc_um_data_req_alloc));
                      memcpy (&new_sdu_p->data[sizeof (struct rlc_um_data_req_alloc)], &sdu_pP->data[0], sdu_sizeP);
                      ((struct rlc_um_data_req *) (new_sdu_p->data))->data_size = sdu_sizeP;
                      ((struct rlc_um_data_req *) (new_sdu_p->data))->data_offset = sizeof (struct rlc_um_data_req_alloc);
                      free_mem_block(sdu_pP);
                      LOG_D(RLC, "%s\n",RLC_FG_BRIGHT_COLOR_RED);
                      if (rlc_um_p->is_data_plane) {
                          LOG_D(RLC, "[FRAME %5u][PDCP][INST %u/%u][RB %u][--- RLC_UM_DATA_REQ/%d Bytes (MBMS) --->][RLC_UM][INST %u/%u][RB %u]\n",
                            frameP,
                            enb_mod_idP,
                            ue_mod_idP,
                            rb_idP,
                            mbms_rb_id,
                            sdu_sizeP,
                            enb_mod_idP,
                            ue_mod_idP,
                            rb_idP,
                            mbms_rb_id);
                      } else {
                          if (enb_flagP) {
                              LOG_D(RLC, "[FRAME %5u][RRC_eNB][INST %u/%u][][--- RLC_UM_DATA_REQ/%d Bytes (MBMS) --->][RLC_UM][INST %u/%u][RB %u]\n",
                                 frameP,
                                 enb_mod_idP,
                                 ue_mod_idP,
                                 rb_idP,
                                 sdu_sizeP,
                                 enb_mod_idP,
                                 ue_mod_idP,
                                 rb_idP,
                                 mbms_rb_id);
                          } else {
                              LOG_D(RLC, "[FRAME %5u][RRC_UE][INST %u/%u][][--- RLC_UM_DATA_REQ/%d Bytes (MBMS) --->][RLC_UM][INST %u/%u][RB %u]\n",
                                 frameP,
                                 enb_mod_idP,
                                 ue_mod_idP,
                                 rb_idP,
                                 sdu_sizeP,
                                 enb_mod_idP,
                                 ue_mod_idP,
                                 rb_idP,
                                 mbms_rb_id);
                          }
                      }
                      LOG_D(RLC, "%s\n",RLC_FG_COLOR_DEFAULT);
                      rlc_um_data_req(rlc_um_p, frameP, new_sdu_p);

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
    free_mem_block(sdu_pP);
    LOG_E(RLC, "MBMS_flag != 0 while Rel10 is not defined...\n");
    //handle_event(ERROR,"FILE %s FONCTION rlc_data_req() LINE %s : parameter module_id out of bounds :%d\n", __FILE__, __LINE__, module_idP);
    return RLC_OP_STATUS_BAD_PARAMETER;
  }
#endif
}

//-----------------------------------------------------------------------------
void rlc_data_ind     (module_id_t enb_mod_idP, module_id_t ue_mod_idP, frame_t frameP, eNB_flag_t enb_flagP, MBMS_flag_t MBMS_flagP, rb_id_t rb_idP, sdu_size_t sdu_sizeP, mem_block_t* sdu_pP, boolean_t is_data_planeP) {
//-----------------------------------------------------------------------------
  rlc_mode_t             rlc_mode   = RLC_MODE_NONE;

  if (enb_flagP) {
      rlc_mode = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].mode;
  } else {
      rlc_mode = rlc_array_ue[ue_mod_idP][rb_idP].mode;
  }

  LOG_D(RLC, "[FRAME %5u][%s][RLC][INST %u/%u][RB %u] Display of rlc_data_ind: size %u\n",
        frameP,
        (enb_flagP) ? "eNB" : "UE",
        enb_mod_idP,
        ue_mod_idP,
        rb_idP,
        sdu_sizeP);

  rlc_util_print_hex_octets(RLC, (unsigned char*)sdu_pP->data, sdu_sizeP);
    // now demux is done at PDCP
    //  if ((is_data_planeP)) {


  switch (rlc_mode) {
      case RLC_MODE_NONE:
        break;
      case RLC_MODE_AM:
          LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][INST %u/%u][RB %u][--- RLC_DATA_IND/%d Bytes --->][PDCP][INST %u/%u][RB %u]\n",
                   frameP,
                   (enb_flagP) ? "eNB" : "UE",
                   enb_mod_idP,
                   ue_mod_idP,
                   rb_idP,
                   sdu_sizeP,
                   enb_mod_idP,
                   ue_mod_idP,
                   rb_idP);
          break;
      case RLC_MODE_UM:
          LOG_D(RLC, "[FRAME %5u][%s][RLC_UM][INST %u/%u][RB %u][--- RLC_DATA_IND/%d Bytes --->][PDCP][INST %u/%u][RB %u]\n",
                   frameP,
                   (enb_flagP) ? "eNB" : "UE",
                   enb_mod_idP,
                   ue_mod_idP,
                   rb_idP,
                   sdu_sizeP,
                   enb_mod_idP,
                   ue_mod_idP,
                   rb_idP);
          break;
      case RLC_MODE_TM:
          LOG_D(RLC, "[FRAME %5u][%s][RLC_TM][INST %u/%u][RB %u][--- RLC_DATA_IND/%d Bytes --->][PDCP][INST %u/%u][RB %u]\n",
                 frameP,
                 (enb_flagP) ? "eNB" : "UE",
                 enb_mod_idP,
                 ue_mod_idP,
                 rb_idP,
                 sdu_sizeP,
                 enb_mod_idP,
                 ue_mod_idP,
                 rb_idP);
          break;
  }
  pdcp_data_ind (enb_mod_idP, ue_mod_idP, frameP, enb_flagP, MBMS_flagP, rb_idP % NB_RB_MAX, sdu_sizeP, sdu_pP, is_data_planeP);
}
//-----------------------------------------------------------------------------
void rlc_data_conf     (module_id_t     enb_mod_idP,
                        module_id_t     ue_mod_idP,
                        frame_t         frameP,
                        eNB_flag_t      enb_flagP,
                        rb_id_t         rb_idP,
                        mui_t           muiP,
                        rlc_tx_status_t statusP,
                        boolean_t       is_data_planeP) {
//-----------------------------------------------------------------------------
    rlc_mode_t             rlc_mode   = RLC_MODE_NONE;

    if (enb_flagP) {
        rlc_mode = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].mode;
    } else {
        rlc_mode = rlc_array_ue[ue_mod_idP][rb_idP].mode;
    }
    if (!(is_data_planeP)) {
        if (rlc_rrc_data_conf != NULL) {
            LOG_D(RLC, "%s\n",RLC_FG_BRIGHT_COLOR_RED);
            switch (rlc_mode) {
                case RLC_MODE_NONE:
                    break;
                case RLC_MODE_AM:
                    LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][INST %u/%u][RB %u][--- RLC_DATA_CONF /MUI %d --->][RRC][INST %u/%u][][RLC_DATA_CONF/ MUI %d]\n",
                            frameP,
                            (enb_flagP) ? "eNB" : "UE",
                            enb_mod_idP,
                            ue_mod_idP,
                            muiP,
                            enb_mod_idP,
                            ue_mod_idP,
                            muiP);
                    break;
                case RLC_MODE_UM:
                    LOG_D(RLC, "[FRAME %5u][%s][RLC_UM][INST %u/%u][RB %u][--- RLC_DATA_CONF /MUI %d --->][RRC][INST %u/%u][][RLC_DATA_CONF/ MUI %d]\n",
                            frameP,
                            (enb_flagP) ? "eNB" : "UE",
                            enb_mod_idP,
                            ue_mod_idP,
                            muiP,
                            enb_mod_idP,
                            ue_mod_idP,
                            muiP);
                    break;
                case RLC_MODE_TM:
                    LOG_D(RLC, "[FRAME %5u][%s][RLC_TM][INST %u/%u][RB %u][--- RLC_DATA_CONF /MUI %d --->][RRC][INST %u/%u][][RLC_DATA_CONF/ MUI %d]\n",
                            frameP,
                            (enb_flagP) ? "eNB" : "UE",
                            enb_mod_idP,
                            ue_mod_idP,
                            muiP,
                            enb_mod_idP,
                            ue_mod_idP,
                            muiP);
                    break;
            }
            LOG_D(RLC, "%s\n",RLC_FG_COLOR_DEFAULT);
            rlc_rrc_data_conf (enb_mod_idP , ue_mod_idP, enb_flagP, rb_idP , muiP, statusP);
        }
    }
}
//-----------------------------------------------------------------------------
int
rlc_module_init (void)
{
//-----------------------------------------------------------------------------
   int          k;
   module_id_t  module_id1;
   module_id_t  module_id2;
   rb_id_t      rb_id;
#if defined(Rel10)
  mbms_session_id_t session_id;
  mbms_service_id_t service_id;
#endif

   LOG_D(RLC, "MODULE INIT\n");
   rlc_rrc_data_ind  = NULL;
   rlc_rrc_data_conf = NULL;


   for (module_id1=0; module_id1 < NUMBER_OF_UE_MAX; module_id1++) {
       for (k=0; k < RLC_MAX_LC; k++) {
           lcid2rbid_ue[module_id1][k] = RLC_RB_UNALLOCATED;
       }
#if defined(Rel10)
      for (service_id = 0; service_id < maxServiceCount; service_id++) {
          for (session_id = 0; session_id < maxSessionPerPMCH; session_id++) {
              memset(&rlc_mbms_array_ue[module_id1][service_id][session_id], 0, sizeof(rlc_mbms_t));
          }
      }
#endif
       for (rb_id=0; rb_id < NB_RB_MAX; rb_id++) {
           memset(&rlc_array_ue[module_id1][rb_id], 0, sizeof(rlc_t));
       }
   }

   for (module_id1=0; module_id1 < NUMBER_OF_eNB_MAX; module_id1++) {
#if defined(Rel10)
      for (service_id = 0; service_id < maxServiceCount; service_id++) {
          for (session_id = 0; session_id < maxSessionPerPMCH; session_id++) {
              memset(&rlc_mbms_array_eNB[module_id1][service_id][session_id], 0, sizeof(rlc_mbms_t));
          }
      }
#endif
       for (module_id2=0; module_id2 < NUMBER_OF_UE_MAX; module_id2++) {
           for (rb_id=0; rb_id < NB_RB_MAX; rb_id++) {
               memset(&rlc_array_eNB[module_id1][module_id2][rb_id], 0, sizeof(rlc_t));
           }
           for (k=0; k < RLC_MAX_LC; k++) {
               lcid2rbid_eNB[module_id1][module_id2][k] = RLC_RB_UNALLOCATED;
           }
       }
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


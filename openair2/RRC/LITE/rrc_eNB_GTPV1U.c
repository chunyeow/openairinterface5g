/*******************************************************************************
Eurecom OpenAirInterface 2
Copyright(c) 1999 - 2014 Eurecom

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
Address      : EURECOM,
               Campus SophiaTech,
               450 Route des Chappes,
               CS 50193
               06904 Biot Sophia Antipolis cedex,
               FRANCE
*******************************************************************************/
/*! \file rrc_eNB_GTPV1U.c
 * \brief rrc GTPV1U procedures for eNB
 * \author Lionel GAUTHIER
 * \version 1.0
 * \company Eurecom
 * \email: lionel.gauthier@eurecom.fr
 */

#if defined(ENABLE_USE_MME)
# include "defs.h"
# include "extern.h"
# include "RRC/LITE/MESSAGES/asn1_msg.h"
# include "rrc_eNB_GTPV1U.h"

# if defined(ENABLE_ITTI)
#   include "asn1_conversions.h"
#   include "intertask_interface.h"
# endif

int rrc_eNB_process_GTPV1U_CREATE_TUNNEL_RESP(MessageDef *msg_pP, const char *msg_name_pP, instance_t instanceP) {
  uint8_t                ue_index;
  int                    i;

  if (msg_pP) {
      LOG_D(RRC, "[eNB] RX %s num tunnels %u \n",
          msg_name_pP,
          GTPV1U_ENB_CREATE_TUNNEL_RESP(msg_pP).num_tunnels);

      ue_index = GTPV1U_ENB_CREATE_TUNNEL_RESP(msg_pP).ue_index;

      // !!!
      instanceP = 0;

      for (i = 0; i < GTPV1U_ENB_CREATE_TUNNEL_RESP(msg_pP).num_tunnels; i++) {
          LOG_D(RRC, "[eNB] instance %u ue index %u rrc_eNB_process_GTPV1U_CREATE_TUNNEL_RESP tunnel %u bearer index %u id %u\n",
              instanceP,
              ue_index,
              GTPV1U_ENB_CREATE_TUNNEL_RESP(msg_pP).enb_S1u_teid[i],
              i,
              GTPV1U_ENB_CREATE_TUNNEL_RESP(msg_pP).eps_bearer_id[i]);
          eNB_rrc_inst[instanceP].Info.UE[ue_index].enb_gtp_teid[i]  = GTPV1U_ENB_CREATE_TUNNEL_RESP(msg_pP).enb_S1u_teid[i];
          eNB_rrc_inst[instanceP].Info.UE[ue_index].enb_gtp_addrs[i] = GTPV1U_ENB_CREATE_TUNNEL_RESP(msg_pP).enb_addr;
          eNB_rrc_inst[instanceP].Info.UE[ue_index].enb_gtp_ebi[i]   = GTPV1U_ENB_CREATE_TUNNEL_RESP(msg_pP).eps_bearer_id[i];
      }
      LOG_D(RRC, "[eNB] RX %s END\n");
      return 0;
  } else {
      return -1;
  }
}
#endif

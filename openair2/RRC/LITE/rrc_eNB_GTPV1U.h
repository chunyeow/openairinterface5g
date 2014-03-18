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
/*! \file rrc_eNB_GTPV1U.h
 * \brief rrc GTPV1U procedures for eNB
 * \author Lionel GAUTHIER
 * \version 1.0
 * \company Eurecom
 * \email: lionel.gauthier@eurecom.fr
 */

#ifndef RRC_ENB_GTPV1U_H_
#define RRC_ENB_GTPV1U_H_

# if defined(ENABLE_USE_MME)


#   if defined(ENABLE_ITTI)

/*! \fn rrc_eNB_process_GTPV1U_CREATE_TUNNEL_RESP(MessageDef *msg_pP, const char *msg_name_pP, instance_t instanceP)
 *\brief Process GTPV1U_ENB_CREATE_TUNNEL_RESP message received from GTPV1U, retrieve the enb teid created.
 *\param msg_p Message received by RRC.
 *\param msg_name Message name.
 *\param instance Message instance.
 *\return 0 when successful, -1 if the UE index can not be retrieved. */
int rrc_eNB_process_GTPV1U_CREATE_TUNNEL_RESP(MessageDef *msg_pP, const char *msg_name_pP, instance_t instanceP);

#   endif
# endif /* defined(ENABLE_USE_MME) */
#endif /* RRC_ENB_GTPV1U_H_ */

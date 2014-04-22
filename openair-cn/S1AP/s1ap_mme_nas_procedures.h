/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2013 Eurecom

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
  Forums       : http://forums.eurecom.fr/openairinterface
  Address      : EURECOM, Campus SophiaTech, 450 Route des Chappes
                 06410 Biot FRANCE

*******************************************************************************/

#include <stdint.h>

#include "s1ap_ies_defs.h"
#include "intertask_interface.h"

#ifndef S1AP_MME_NAS_PROCEDURES_H_
#define S1AP_MME_NAS_PROCEDURES_H_

/** \brief Handle an Initial UE message.
 * \param assocId lower layer assoc id (SCTP)
 * \param stream SCTP stream on which data had been received
 * \param message The message as decoded by the ASN.1 codec
 * @returns -1 on failure, 0 otherwise
 **/
int s1ap_mme_handle_initial_ue_message(uint32_t assocId, uint32_t stream,
                                       struct s1ap_message_s *message);

/** \brief Handle an Uplink NAS transport message.
 * Process the RRC transparent container and forward it to NAS entity.
 * \param assocId lower layer assoc id (SCTP)
 * \param stream SCTP stream on which data had been received
 * \param message The message as decoded by the ASN.1 codec
 * @returns -1 on failure, 0 otherwise
 **/
int s1ap_mme_handle_uplink_nas_transport(uint32_t assocId, uint32_t stream,
                                         struct s1ap_message_s *message);

/** \brief Handle a NAS non delivery indication message from eNB
 * \param assocId lower layer assoc id (SCTP)
 * \param stream SCTP stream on which data had been received
 * \param message The message as decoded by the ASN.1 codec
 * @returns -1 on failure, 0 otherwise
 **/
int s1ap_mme_handle_nas_non_delivery(uint32_t assocId, uint32_t stream,
                                     struct s1ap_message_s *message);

#if defined(DISABLE_USE_NAS)
int s1ap_handle_attach_accepted(nas_attach_accept_t *attach_accept_p);
#else
void s1ap_handle_conn_est_cnf(const mme_app_connection_establishment_cnf_t * const conn_est_cnf_p);
#endif

int s1ap_generate_downlink_nas_transport(const uint32_t ue_id, void * const data,
                                         const uint32_t size);
#endif /* S1AP_MME_NAS_PROCEDURES_H_ */

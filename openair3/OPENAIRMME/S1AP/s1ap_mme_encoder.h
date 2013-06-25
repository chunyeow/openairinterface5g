/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2012 Eurecom

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

#include "s1ap_mme.h"

#ifndef S1AP_MME_ENCODER_H_
#define S1AP_MME_ENCODER_H_

int s1ap_mme_encode_s1setupfailure(
    S1SetupFailureIEs_t *s1SetupFailureIEs,
    uint32_t assocId);

int s1ap_mme_encode_s1setupresponse(
    S1SetupResponseIEs_t *s1SetupResponseIEs,
    uint32_t assocId);

int s1ap_mme_encode_initial_context_setup_request(
    InitialContextSetupRequestIEs_t *initialContextSetupRequestIEs,
    ue_description_t                *ue_ref);

int s1ap_mme_encode_downlink_nas_transport(
    DownlinkNASTransportIEs_t *downlinkNasTransportIEs,
    ue_description_t          *ue_ref);

int s1ap_mme_encode_ue_context_release_command(
    UEContextReleaseCommandIEs_t *ueContextReleaseCommandIEs,
    ue_description_t             *ue_ref);

#endif /* S1AP_MME_ENCODER_H_ */

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
#ifndef GTPV1_U_MESSAGES_TYPES_H_
#define GTPV1_U_MESSAGES_TYPES_H_

#include "../SGW-LITE/sgw_lite_ie_defs.h"

typedef struct {
    Teid_t           context_teid;               ///< Tunnel Endpoint Identifier
    ebi_t            eps_bearer_id;
} Gtpv1uCreateTunnelReq;

typedef struct {
    uint8_t  status;           ///< Status of S1U endpoint creation (Failed = 0xFF or Success = 0x0)
    Teid_t   context_teid;     ///< local SGW S11 Tunnel Endpoint Identifier
    Teid_t   S1u_teid;         ///< Tunnel Endpoint Identifier
    ebi_t    eps_bearer_id;
} Gtpv1uCreateTunnelResp;

typedef struct {
    Teid_t           context_teid;     ///< S11 Tunnel Endpoint Identifier
    Teid_t           sgw_S1u_teid;     ///< SGW S1U local Tunnel Endpoint Identifier
    Teid_t           enb_S1u_teid;     ///< eNB S1U Tunnel Endpoint Identifier
    ip_address_t     enb_ip_address_for_S1u;
    ebi_t            eps_bearer_id;
} Gtpv1uUpdateTunnelReq;

typedef struct {
    uint8_t          status;           ///< Status (Failed = 0xFF or Success = 0x0)
    Teid_t           context_teid;     ///< S11 Tunnel Endpoint Identifier
    Teid_t           sgw_S1u_teid;     ///< SGW S1U local Tunnel Endpoint Identifier
    Teid_t           enb_S1u_teid;     ///< eNB S1U Tunnel Endpoint Identifier
    ebi_t            eps_bearer_id;
} Gtpv1uUpdateTunnelResp;

typedef struct {
    Teid_t           context_teid;   ///< local SGW S11 Tunnel Endpoint Identifier
    Teid_t           S1u_teid;       ///< local S1U Tunnel Endpoint Identifier to be deleted
} Gtpv1uDeleteTunnelReq;

typedef struct {
    uint8_t  status;           ///< Status of S1U endpoint deleteion (Failed = 0xFF or Success = 0x0)
    Teid_t   context_teid;     ///< local SGW S11 Tunnel Endpoint Identifier
    Teid_t   S1u_teid;         ///< local S1U Tunnel Endpoint Identifier to be deleted
} Gtpv1uDeleteTunnelResp;

typedef struct {
    uint8_t  *buffer;
    uint32_t  length;
    Teid_t    local_S1u_teid;               ///< Tunnel Endpoint Identifier
} Gtpv1uTunnelDataInd;

typedef struct {
    uint8_t  *buffer;
    uint32_t  length;
    Teid_t    local_S1u_teid;               ///< Tunnel Endpoint Identifier
    Teid_t    S1u_enb_teid;                 ///< Tunnel Endpoint Identifier
} Gtpv1uTunnelDataReq;

#endif /* GTPV1_U_MESSAGES_TYPES_H_ */

/*******************************************************************************
Eurecom OpenAirInterface core network
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
/*! \file sgw_lite_handlers.h
* \brief
* \author Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/

#ifndef SGW_LITE_HANDLERS_H_
#define SGW_LITE_HANDLERS_H_

int sgw_lite_handle_create_session_request(SgwCreateSessionRequest *session_req_p);
int sgw_lite_handle_sgi_endpoint_created  (SGICreateEndpointResp   *resp_p);
int sgw_lite_handle_sgi_endpoint_updated  (SGIUpdateEndpointResp   *resp_p);
int sgw_lite_handle_gtpv1uCreateTunnelResp(Gtpv1uCreateTunnelResp  *endpoint_created_p);
int sgw_lite_handle_gtpv1uUpdateTunnelResp(Gtpv1uUpdateTunnelResp  *endpoint_updated_p);
int sgw_lite_handle_modify_bearer_request (SgwModifyBearerRequest  *modify_bearer_p);
int sgw_lite_handle_delete_session_request(SgwDeleteSessionRequest *delete_session_p);

#endif /* SGW_LITE_HANDLERS_H_ */

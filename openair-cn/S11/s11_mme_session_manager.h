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

#ifndef S11_MME_SESSION_MANAGER_H_
#define S11_MME_SESSION_MANAGER_H_

/* @brief Create a new Create Session Request and send it to provided S-GW.
 */
int s11_mme_create_session_request(NwGtpv2cStackHandleT    *stack_p,
                                   SgwCreateSessionRequest *create_session_p);

/* @brief Handle a Create Session Response received from S-GW.
 */
int s11_mme_handle_create_session_response(NwGtpv2cStackHandleT *stack_p,
                                           NwGtpv2cUlpApiT      *pUlpApi);

#endif /* S11_MME_SESSION_MANAGER_H_ */

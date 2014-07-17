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

#ifndef S6A_MESSAGES_H_
#define S6A_MESSAGES_H_

int s6a_generate_update_location(s6a_update_location_req_t *ulr_p);
int s6a_generate_authentication_info_req(s6a_auth_info_req_t *uar_p);

int s6a_ula_cb(struct msg **msg, struct avp *paramavp,
               struct session *sess, void *opaque,
               enum disp_action *act);
int s6a_aia_cb(struct msg **msg, struct avp *paramavp,
               struct session *sess, void *opaque,
               enum disp_action *act);

int s6a_parse_subscription_data(struct avp *avp_subscription_data,
                                subscription_data_t *subscription_data);

#endif /* S6A_MESSAGES_H_ */

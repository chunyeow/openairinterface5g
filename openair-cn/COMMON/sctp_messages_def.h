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
//WARNING: Do not include this header directly. Use intertask_interface.h instead.

MESSAGE_DEF(SCTP_INIT_MSG,          MESSAGE_PRIORITY_MED, SctpInit,                 sctpInit)
MESSAGE_DEF(SCTP_DATA_REQ,          MESSAGE_PRIORITY_MED, sctp_data_req_t,          sctp_data_req)
MESSAGE_DEF(SCTP_DATA_IND,          MESSAGE_PRIORITY_MED, sctp_data_ind_t,          sctp_data_ind)
MESSAGE_DEF(SCTP_NEW_ASSOCIATION,   MESSAGE_PRIORITY_MAX, sctp_new_peer_t,          sctp_new_peer)
MESSAGE_DEF(SCTP_CLOSE_ASSOCIATION, MESSAGE_PRIORITY_MAX, sctp_close_association_t, sctp_close_association)

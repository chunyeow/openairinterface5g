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

#ifndef S1AP_MME_TA_H_
#define S1AP_MME_TA_H_

enum {
    TA_LIST_UNKNOWN_TAC = -2,
    TA_LIST_UNKNOWN_PLMN = -1,
    TA_LIST_RET_OK = 0,
    TA_LIST_NO_MATCH = 0x1,
    TA_LIST_AT_LEAST_ONE_MATCH = 0x2,
    TA_LIST_COMPLETE_MATCH = 0x3,
};

int s1ap_mme_compare_ta_lists(S1ap_SupportedTAs_t *ta_list);

#endif /* S1AP_MME_TA_H_ */

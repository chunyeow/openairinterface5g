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


#include "as_message.h"
#include "common_types.h"
#include "s6a_defs.h"
#include "mme_app_defs.h"

nas_cause_t s6a_error_2_nas_cause(uint32_t s6a_error, int experimental)
{
    if (experimental == 0) {
        /* Base protocol errors */
        switch (s6a_error) {
            /* 3002 */
            case ER_DIAMETER_UNABLE_TO_DELIVER: /* Fall through */
            /* 3003 */
            case ER_DIAMETER_REALM_NOT_SERVED: /* Fall through */
            /* 5003 */
            case ER_DIAMETER_AUTHORIZATION_REJECTED:
                return NAS_CAUSE_NO_SUITABLE_CELLS_IN_TRACKING_AREA;
            /* 5012 */
            case ER_DIAMETER_UNABLE_TO_COMPLY: /* Fall through */
            /* 5004 */
            case ER_DIAMETER_INVALID_AVP_VALUE: /* Fall through */
            /* Any other permanent errors from the diameter base protocol */
            default:
                break;
        }
    } else {
        switch (s6a_error) {
            /* 5001 */
            case DIAMETER_ERROR_USER_UNKNOWN:
                return NAS_CAUSE_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED;
            /* TODO: distinguish GPRS_DATA_SUBSCRIPTION */
            /* 5420 */
            case DIAMETER_ERROR_UNKNOWN_EPS_SUBSCRIPTION:
                return NAS_CAUSE_NO_SUITABLE_CELLS_IN_TRACKING_AREA;
            /* 5421 */
            case DIAMETER_ERROR_RAT_NOT_ALLOWED:
                /* One of the following parameter can be sent depending on
                 * operator preference:
                 * ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA
                 * TRACKING_AREA_NOT_ALLOWED
                 * NO_SUITABLE_CELLS_IN_TRACKING_AREA
                 */
                return NAS_CAUSE_TRACKING_AREA_NOT_ALLOWED;
            /* 5004 without error diagnostic */
            case DIAMETER_ERROR_ROAMING_NOT_ALLOWED:
                return NAS_CAUSE_PLMN_NOT_ALLOWED;
            /* TODO: 5004 with error diagnostic of ODB_HPLMN_APN or
             * ODB_VPLMN_APN
             */
            /* TODO: 5004 with error diagnostic of ODB_ALL_APN */
            default:
                break;
        }
    }
    return NAS_CAUSE_NETWORK_FAILURE;
}

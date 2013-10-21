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

#include "as_message.h"
#include "common_types.h"
#include "s6a_defs.h"

// int send_nas_failure(uint32_t nas_cause)
// {
//     MessageDef *message_p;
// 
//     message_p = alloc_new_message(TASK_MME_APP, TASK_NAS,
//                                   SGW_CREATE_SESSION_REQUEST);
// 
//     return send_msg_to_task(TASK_NAS, message_p);
// }

int s6a_error_2_nas_cause(uint32_t s6a_error, int experimental)
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
                return NO_SUITABLE_CELLS_IN_TRACKING_AREA;
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
                return EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED;
            /* TODO: distinguish GPRS_DATA_SUBSCRIPTION */
            /* 5420 */
            case DIAMETER_ERROR_UNKNOWN_EPS_SUBSCRIPTION:
                return NO_SUITABLE_CELLS_IN_TRACKING_AREA;
            /* 5421 */
            case DIAMETER_ERROR_RAT_NOT_ALLOWED:
                /* One of the following parameter can be sent depending on
                 * operator preference:
                 * ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA
                 * TRACKING_AREA_NOT_ALLOWED
                 * NO_SUITABLE_CELLS_IN_TRACKING_AREA
                 */
                return TRACKING_AREA_NOT_ALLOWED;
            /* 5004 without error diagnostic */
            case DIAMETER_ERROR_ROAMING_NOT_ALLOWED:
                return PLMN_NOT_ALLOWED;
            /* TODO: 5004 with error diagnostic of ODB_HPLMN_APN or
             * ODB_VPLMN_APN
             */
            /* TODO: 5004 with error diagnostic of ODB_ALL_APN */
            default:
                break;
        }
    }
    return NETWORK_FAILURE;
}

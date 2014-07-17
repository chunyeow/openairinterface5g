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

#include "hss_config.h"
#include "db_proto.h"
#include "s6a_proto.h"

inline char *experimental_retcode_2_string(int ret_code)
{
    switch(ret_code) {
        /* Experimental-Result-Codes */
        case DIAMETER_ERROR_USER_UNKNOWN:
            return "DIAMETER_ERROR_USER_UNKNOWN";
        case DIAMETER_ERROR_ROAMING_NOT_ALLOWED:
            return "DIAMETER_ERROR_ROAMING_NOT_ALLOWED";
        case DIAMETER_ERROR_UNKNOWN_EPS_SUBSCRIPTION:
            return "DIAMETER_ERROR_UNKNOWN_EPS_SUBSCRIPTION";
        case DIAMETER_ERROR_RAT_NOT_ALLOWED:
            return "DIAMETER_ERROR_RAT_NOT_ALLOWED";
        case DIAMETER_ERROR_EQUIPMENT_UNKNOWN:
            return "DIAMETER_ERROR_EQUIPMENT_UNKNOWN";
        case DIAMETER_ERROR_UNKOWN_SERVING_NODE:
            return "DIAMETER_ERROR_UNKOWN_SERVING_NODE";
        case DIAMETER_AUTHENTICATION_DATA_UNAVAILABLE:
            return "DIAMETER_AUTHENTICATION_DATA_UNAVAILABLE";
        default:
            break;
    }
    return "DIAMETER_AVP_UNSUPPORTED";
}

inline char *retcode_2_string(int ret_code)
{
    switch(ret_code) {
        case ER_DIAMETER_SUCCESS:
            return "DIAMETER_SUCCESS";
        case ER_DIAMETER_MISSING_AVP:
            return "DIAMETER_MISSING_AVP";
        case ER_DIAMETER_INVALID_AVP_VALUE:
            return "DIAMETER_INVALID_AVP_VALUE";
        default:
            break;
    }
    return "DIAMETER_AVP_UNSUPPORTED";
}


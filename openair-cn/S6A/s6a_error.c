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

#include <stdint.h>

#include "common_types.h"
#include "s6a_defs.h"

#include "assertions.h"

int s6a_parse_experimental_result(struct avp *avp, s6a_experimental_result_t *ptr)
{
    struct avp_hdr *hdr;
    struct avp *child_avp = NULL;

    if (!avp) {
        return EINVAL;
    }

    CHECK_FCT(fd_msg_avp_hdr(avp, &hdr));
    DevAssert(hdr->avp_code == AVP_CODE_EXPERIMENTAL_RESULT);
    CHECK_FCT(fd_msg_browse(avp, MSG_BRW_FIRST_CHILD, &child_avp, NULL));
    while(child_avp) {
        CHECK_FCT(fd_msg_avp_hdr(child_avp, &hdr));
        switch(hdr->avp_code) {
            case AVP_CODE_EXPERIMENTAL_RESULT_CODE:
                S6A_ERROR("Got experimental error %u:%s\n", hdr->avp_value->u32,
                          experimental_retcode_2_string(hdr->avp_value->u32));
                if (ptr) {
                    *ptr = (s6a_experimental_result_t)hdr->avp_value->u32;
                }
                break;
            case AVP_CODE_VENDOR_ID:
                DevCheck(hdr->avp_value->u32 == 10415, hdr->avp_value->u32,
                         AVP_CODE_VENDOR_ID, 10415);
                break;
            default:
                return -1;
        }
        /* Go to next AVP in the grouped AVP */
        CHECK_FCT(fd_msg_browse(child_avp, MSG_BRW_NEXT, &child_avp, NULL));
    }
    return 0;
}

inline char *experimental_retcode_2_string(uint32_t ret_code)
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

inline char *retcode_2_string(uint32_t ret_code)
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


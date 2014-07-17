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

int s6a_add_result_code(struct msg *ans, struct avp *failed_avp, int result_code, int experimental)
{
    struct avp *avp;
    union avp_value value;

    if (DIAMETER_ERROR_IS_VENDOR(result_code) && experimental != 0) {
        struct avp *experimental_result;
        CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_experimental_result,
                                 0, &experimental_result));
        CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_vendor_id,
                                 0, &avp));
        value.u32 = VENDOR_3GPP;
        CHECK_FCT(fd_msg_avp_setvalue(avp, &value));
        CHECK_FCT(fd_msg_avp_add(experimental_result, MSG_BRW_LAST_CHILD, avp));
        CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_experimental_result_code,
                                 0, &avp));
        value.u32 = result_code;
        CHECK_FCT(fd_msg_avp_setvalue(avp, &value));
        CHECK_FCT(fd_msg_avp_add(experimental_result, MSG_BRW_LAST_CHILD, avp));
        CHECK_FCT(fd_msg_avp_add(ans, MSG_BRW_LAST_CHILD, experimental_result));

        /* Add Origin_Host & Origin_Realm AVPs */
        CHECK_FCT(fd_msg_add_origin(ans, 0));
    } else {
        /* This is a code defined in the base protocol: result-code AVP should
         * be used.
         */
        CHECK_FCT(fd_msg_rescode_set(ans, retcode_2_string(result_code), NULL,
                                     failed_avp, 1));
    }
    return 0;
}

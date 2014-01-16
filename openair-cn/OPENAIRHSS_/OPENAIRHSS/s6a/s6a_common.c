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

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

/*! \file s6a_purge_ue.c
 * \brief Handle a purge UE request and generate the corresponding answer
 * \author Sebastien ROUX <sebastien.roux@eurecom.fr>
 * \date 2013
 * \version 0.1
 */

#include "hss_config.h"
#include "db_proto.h"
#include "s6a_proto.h"

int s6a_purge_ue_cb(struct msg **msg, struct avp *paramavp,
                    struct session *sess, void *opaque,
                    enum disp_action *act)
{
    struct msg *ans, *qry;
    struct avp *avp, *failed_avp = NULL;
    struct avp_hdr *hdr;

    int ret = 0;
    int result_code = ER_DIAMETER_SUCCESS;
    int experimental = 0;
    uint32_t pur_flags = 0;

    /* MySQL requests and asnwer data */
    mysql_pu_req_t pu_req;
    mysql_pu_ans_t pu_ans;

    if (msg == NULL) {
        return EINVAL;
    }

    memset(&pu_req, 0, sizeof(mysql_pu_req_t));

    qry = *msg;
    /* Create the answer */
    CHECK_FCT(fd_msg_new_answer_from_req(fd_g_config->cnf_dict, msg, 0));
    ans = *msg;

    /* Retrieving IMSI AVP */
    CHECK_FCT(fd_msg_search_avp(qry, s6a_cnf.dataobj_s6a_imsi, &avp));
    if (avp) {
        CHECK_FCT(fd_msg_avp_hdr(avp, &hdr));
        if (hdr->avp_value->os.len > IMSI_LENGTH) {
            result_code = ER_DIAMETER_INVALID_AVP_VALUE;
            goto out;
        }
        sprintf(pu_req.imsi, "%*s", (int)hdr->avp_value->os.len, hdr->avp_value->os.data);
    } else {
        result_code = ER_DIAMETER_MISSING_AVP;
        goto out;
    }

    /* Retrieving the PUR-Flags if present */
    CHECK_FCT(fd_msg_search_avp(qry, s6a_cnf.dataobj_s6a_pur_flags, &avp));
    if (avp) {
        CHECK_FCT(fd_msg_avp_hdr(avp, &hdr));
        pur_flags = hdr->avp_value->u32;
        if (FLAG_IS_SET(pur_flags, PUR_UE_PURGED_IN_SGSN)) {
            /* This bit shall not be set by a standalone MME. */
            result_code = ER_DIAMETER_INVALID_AVP_VALUE;
            goto out;
        }
    }

    if ((ret = hss_mysql_purge_ue(&pu_req, &pu_ans)) != 0) {
        /* We failed to find the IMSI in the database. Replying to the request
         * with the user unknown cause.
         */
        experimental = 1;
        result_code = DIAMETER_ERROR_USER_UNKNOWN;
        goto out;
    }

    /* Retrieving Origin host AVP */
    CHECK_FCT(fd_msg_search_avp(qry, s6a_cnf.dataobj_s6a_origin_host, &avp));
    if (!avp) {
        result_code = ER_DIAMETER_MISSING_AVP;
        goto out;
    }
    /* Retrieve the header from origin host and realm avps */
    CHECK_FCT(fd_msg_avp_hdr(avp, &hdr));
    if (strncmp(pu_ans.mme_host, (char *)hdr->avp_value->os.data, hdr->avp_value->os.len) != 0)
    {
        result_code = DIAMETER_ERROR_UNKOWN_SERVING_NODE;
        experimental = 1;
        goto out;
    }

    /* Retrieving Origin realm AVP */
    CHECK_FCT(fd_msg_search_avp(qry, s6a_cnf.dataobj_s6a_origin_realm, &avp));
    if (!avp) {
        result_code = ER_DIAMETER_MISSING_AVP;
        goto out;
    }
    CHECK_FCT(fd_msg_avp_hdr(avp, &hdr));
    if (strncmp(pu_ans.mme_realm, (char *)hdr->avp_value->os.data, hdr->avp_value->os.len) != 0)
    {
        result_code = DIAMETER_ERROR_UNKOWN_SERVING_NODE;
        experimental = 1;
        goto out;
    }

out:
    /* Append the result code to the answer */
    CHECK_FCT(s6a_add_result_code(ans, failed_avp, result_code, experimental));

    CHECK_FCT(fd_msg_send(msg, NULL, NULL ));
    return 0;
}

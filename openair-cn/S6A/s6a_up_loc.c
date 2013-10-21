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

#include <stdio.h>
#include <stdint.h>

#include "mme_config.h"

#include "assertions.h"
#include "conversions.h"

#include "intertask_interface.h"
#include "s6a_defs.h"
#include "s6a_messages.h"

int s6a_ula_cb(struct msg **msg, struct avp *paramavp,
               struct session *sess, void *opaque,
               enum disp_action *act)
{
    struct msg *ans;
    struct msg *qry;
    struct avp *avp;
    struct avp_hdr *hdr;

    MessageDef                *message_p;
    s6a_update_location_ans_t *s6a_update_location_ans_p;

    DevAssert(msg != NULL);

    ans = *msg;
    /* Retrieve the original query associated with the asnwer */
    CHECK_FCT(fd_msg_answ_getq(ans, &qry));

    DevAssert(qry != NULL);

    message_p = alloc_new_message(TASK_S6A, S6A_UPDATE_LOCATION_ANS);

    s6a_update_location_ans_p = &message_p->msg.s6a_update_location_ans;

    CHECK_FCT(fd_msg_search_avp(qry, s6a_fd_cnf.dataobj_s6a_user_name, &avp));
    if (avp) {
        CHECK_FCT(fd_msg_avp_hdr(avp, &hdr));
        memcpy(s6a_update_location_ans_p->imsi, hdr->avp_value->os.data, hdr->avp_value->os.len);
        s6a_update_location_ans_p->imsi[hdr->avp_value->os.len] = '\0';
        s6a_update_location_ans_p->imsi_length = hdr->avp_value->os.len;
        S6A_DEBUG("Received s6a ula for imsi=%*s\n",
                  (int)hdr->avp_value->os.len,
                  hdr->avp_value->os.data);
    } else {
        DevMessage("Query has been freed before we received the answer\n");
    }

    /* Retrieve the result-code */
    CHECK_FCT(fd_msg_search_avp(ans, s6a_fd_cnf.dataobj_s6a_result_code, &avp));
    if (avp) {
        CHECK_FCT(fd_msg_avp_hdr(avp, &hdr));

        s6a_update_location_ans_p->result.present = S6A_RESULT_BASE;
        s6a_update_location_ans_p->result.choice.base = hdr->avp_value->u32;

        if (hdr->avp_value->u32 != ER_DIAMETER_SUCCESS) {
            S6A_ERROR("Got error %u:%s\n", hdr->avp_value->u32,
                      retcode_2_string(hdr->avp_value->u32));
            goto err;
        }
    } else {
        /* The result-code is not present, may be it is an experimental result
         * avp indicating a 3GPP specific failure.
         */
        CHECK_FCT(fd_msg_search_avp(ans, s6a_fd_cnf.dataobj_s6a_experimental_result, &avp));
        if (avp) {
            /* The procedure has failed within the HSS.
             * NOTE: contrary to result-code, the experimental-result is a grouped
             * AVP and requires parsing its childs to get the code back.
             */
            s6a_update_location_ans_p->result.present = S6A_RESULT_EXPERIMENTAL;
            s6a_parse_experimental_result(avp, &s6a_update_location_ans_p->result.choice.experimental);

            goto err;
        } else {
            /* Neither result-code nor experimental-result is present ->
             * totally incorrect behaviour here.
             */
            S6A_ERROR("Experimental-Result and Result-Code are absent: "
                      "This is not a correct behaviour\n");
            goto err;
        }
    }

    /* Retrieving the ULA flags */
    CHECK_FCT(fd_msg_search_avp(ans, s6a_fd_cnf.dataobj_s6a_ula_flags, &avp));
    if (avp) {
        CHECK_FCT(fd_msg_avp_hdr(avp, &hdr));
        /* This bit, when set, indicates that the HSS stores SGSN number
         * and MME number in separate memory. A Rel-8 HSS shall set
         * the bit.
         */
        if (!FLAG_IS_SET(hdr->avp_value->u32, ULA_SEPARATION_IND)) {
            S6A_ERROR("ULA-Flags does not indicate the HSS is post Rel.8: "
                      "This behaviour is not compliant\n");
            goto err;
        }
    } else {
        /* ULA-Flags is absent while the error code indicates DIAMETER_SUCCESS:
         * this is not a compliant behaviour...
         * TODO: handle this case.
         */
        S6A_ERROR("ULA-Flags AVP is absent while result code indicates "
                  "DIAMETER_SUCCESS\n");
        goto err;
    }

    CHECK_FCT(fd_msg_search_avp(ans, s6a_fd_cnf.dataobj_s6a_subscription_data, &avp));
    if (avp) {
        CHECK_FCT(s6a_parse_subscription_data(avp, &s6a_update_location_ans_p->subscription_data));

#if defined(DISABLE_USE_NAS)

#else
        DevParam(0, 0, 0);
#endif
    }

err:

    ans = NULL;

    send_msg_to_task(TASK_MME_APP, INSTANCE_DEFAULT, message_p);

    S6A_DEBUG("Sending S6A_UPDATE_LOCATION_ANS to task MME_APP\n");

    return 0;
}

int s6a_generate_update_location(s6a_update_location_req_t *ulr_p)
{
    struct avp     *avp;
    struct msg     *msg;
    struct session *sess;

    union avp_value value;

    DevAssert(ulr_p != NULL);

    /* Create the new update location request message */
    CHECK_FCT(fd_msg_new(s6a_fd_cnf.dataobj_s6a_ulr, 0, &msg));

    /* Create a new session */
    CHECK_FCT(fd_sess_new(&sess, fd_g_config->cnf_diamid,
                          fd_g_config->cnf_diamid_len, (os0_t)"apps6a", 6));
    {
        os0_t sid;
        size_t sidlen;
        CHECK_FCT(fd_sess_getsid(sess, &sid, &sidlen));
        CHECK_FCT(fd_msg_avp_new(s6a_fd_cnf.dataobj_s6a_session_id, 0, &avp));
        value.os.data = sid;
        value.os.len  = sidlen;
        CHECK_FCT(fd_msg_avp_setvalue(avp, &value));
        CHECK_FCT(fd_msg_avp_add(msg, MSG_BRW_FIRST_CHILD, avp));
    }

    CHECK_FCT(fd_msg_avp_new(s6a_fd_cnf.dataobj_s6a_auth_session_state, 0, &avp));
    /* No State maintained */
    value.i32 = 1;
    CHECK_FCT(fd_msg_avp_setvalue(avp, &value));
    CHECK_FCT(fd_msg_avp_add(msg, MSG_BRW_LAST_CHILD, avp));

    /* Add Origin_Host & Origin_Realm */
    CHECK_FCT(fd_msg_add_origin(msg, 0));

    config_read_lock(&mme_config);

    /* Destination Host */
    {
        char host[40]  = "hss.";
        size_t hostlen;

        strcat(host, mme_config.realm);

        hostlen = strlen(host);

        CHECK_FCT(fd_msg_avp_new(s6a_fd_cnf.dataobj_s6a_destination_host, 0, &avp));
        value.os.data = (unsigned char *)host;
        value.os.len  = hostlen;
        CHECK_FCT(fd_msg_avp_setvalue(avp, &value));

        CHECK_FCT(fd_msg_avp_add(msg, MSG_BRW_LAST_CHILD, avp));
    }
    /* Destination_Realm */
    {
        char *realm     = mme_config.realm;
        size_t realmlen = strlen(realm);

        CHECK_FCT(fd_msg_avp_new(s6a_fd_cnf.dataobj_s6a_destination_realm, 0, &avp));
        value.os.data = (unsigned char *)realm;
        value.os.len  = realmlen;
        CHECK_FCT(fd_msg_avp_setvalue(avp, &value));

        CHECK_FCT(fd_msg_avp_add(msg, MSG_BRW_LAST_CHILD, avp));
    }

    config_unlock(&mme_config);

    /* Adding the User-Name (IMSI) */
    CHECK_FCT(fd_msg_avp_new(s6a_fd_cnf.dataobj_s6a_user_name, 0, &avp));
    value.os.data = (unsigned char *)ulr_p->imsi;
    value.os.len  = strlen(ulr_p->imsi);
    CHECK_FCT(fd_msg_avp_setvalue(avp, &value));
    CHECK_FCT(fd_msg_avp_add(msg, MSG_BRW_LAST_CHILD, avp));

    /* Adding the visited plmn id */
    {
        uint8_t plmn[3];

        CHECK_FCT(fd_msg_avp_new(s6a_fd_cnf.dataobj_s6a_visited_plmn_id, 0, &avp));

        PLMN_T_TO_TBCD(ulr_p->visited_plmn, plmn);

        printf("PLMN: %3s\n", plmn);

        value.os.data = plmn;
        value.os.len  = 3;
        CHECK_FCT(fd_msg_avp_setvalue(avp, &value));

        CHECK_FCT(fd_msg_avp_add(msg, MSG_BRW_LAST_CHILD, avp));
    }

    /* Adding the RAT-Type */
    CHECK_FCT(fd_msg_avp_new(s6a_fd_cnf.dataobj_s6a_rat_type, 0, &avp));
    DevCheck(ulr_p->rat_type == RAT_EUTRAN, ulr_p->rat_type, 0, 0);
    value.u32 = ulr_p->rat_type;
    CHECK_FCT(fd_msg_avp_setvalue(avp, &value));
    CHECK_FCT(fd_msg_avp_add(msg, MSG_BRW_LAST_CHILD, avp));

    /* Adding ULR-Flags */
    CHECK_FCT(fd_msg_avp_new(s6a_fd_cnf.dataobj_s6a_ulr_flags, 0, &avp));
    value.u32 = 0;

    /* Identify the ULR as coming from S6A interface (i.e. from MME) */
    FLAGS_SET(value.u32, ULR_S6A_S6D_INDICATOR);

    /* Set the ulr-flags as indicated by upper layer */
    if (ulr_p->skip_subsriber_data) {
        FLAGS_SET(value.u32, ULR_SKIP_SUBSCRIBER_DATA);
    }
    if (ulr_p->initial_attach) {
        FLAGS_SET(value.u32, ULR_INITIAL_ATTACH_IND);
    }
    CHECK_FCT(fd_msg_avp_setvalue(avp, &value));
    CHECK_FCT(fd_msg_avp_add(msg, MSG_BRW_LAST_CHILD, avp));

    CHECK_FCT(fd_msg_send(&msg, NULL, NULL));

    S6A_DEBUG("Sending s6a ulr for imsi=%s\n", ulr_p->imsi);

    return 0;
}

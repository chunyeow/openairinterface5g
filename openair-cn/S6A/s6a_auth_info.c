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

#include <stdio.h>
#include <stdint.h>

#include "mme_config.h"

#include "assertions.h"
#include "conversions.h"

#include "intertask_interface.h"
#include "security_types.h"
#include "s6a_defs.h"
#include "s6a_messages.h"

static
int s6a_parse_rand(struct avp_hdr *hdr, uint8_t *rand_p)
{
    int ret = 0;
    DevCheck(hdr->avp_value->os.len == RAND_LENGTH_OCTETS, RAND_LENGTH_OCTETS,
             hdr->avp_value->os.len, 0);
    DevAssert(rand_p != NULL);

    STRING_TO_RAND(hdr->avp_value->os.data, rand_p, ret);
    return ret;
}

static
int s6a_parse_xres(struct avp_hdr *hdr, res_t *xres)
{
    int ret = 0;
    DevCheck(hdr->avp_value->os.len >= XRES_LENGTH_MIN &&
             hdr->avp_value->os.len <= XRES_LENGTH_MAX, XRES_LENGTH_MIN,
             XRES_LENGTH_MAX, hdr->avp_value->os.len);
    DevAssert(xres != NULL);

    STRING_TO_XRES(hdr->avp_value->os.data, hdr->avp_value->os.len, xres, ret);
    return ret;
}

static
int s6a_parse_autn(struct avp_hdr *hdr, uint8_t *autn)
{
    int ret = 0;
    DevCheck(hdr->avp_value->os.len == AUTN_LENGTH_OCTETS, AUTN_LENGTH_OCTETS,
             hdr->avp_value->os.len, 0);
    DevAssert(autn != NULL);

    STRING_TO_AUTN(hdr->avp_value->os.data, autn, ret);
    return ret;
}

static
int s6a_parse_kasme(struct avp_hdr *hdr, uint8_t *kasme)
{
    int ret = 0;
    DevCheck(hdr->avp_value->os.len == KASME_LENGTH_OCTETS, KASME_LENGTH_OCTETS,
             hdr->avp_value->os.len, 0);
    DevAssert(kasme != NULL);

    STRING_TO_KASME(hdr->avp_value->os.data, kasme, ret);
    return ret;
}

static inline
int s6a_parse_e_utran_vector(struct avp *avp_vector, eutran_vector_t *vector)
{
    int ret = 0x0f;
    struct avp *avp;
    struct avp_hdr *hdr;

    CHECK_FCT(fd_msg_avp_hdr(avp_vector, &hdr));

    DevCheck(hdr->avp_code == AVP_CODE_E_UTRAN_VECTOR,
             hdr->avp_code, AVP_CODE_E_UTRAN_VECTOR, 0);

    CHECK_FCT(fd_msg_browse(avp_vector, MSG_BRW_FIRST_CHILD, &avp, NULL));
    while(avp) {
        CHECK_FCT(fd_msg_avp_hdr(avp, &hdr));

        switch(hdr->avp_code) {
            case AVP_CODE_RAND:
                CHECK_FCT(s6a_parse_rand(hdr, vector->rand));
                ret &= ~0x01;
                break;
            case AVP_CODE_XRES:
                CHECK_FCT(s6a_parse_xres(hdr, &vector->xres));
                ret &= ~0x02;
                break;
            case AVP_CODE_AUTN:
                CHECK_FCT(s6a_parse_autn(hdr, vector->autn));
                ret &= ~0x04;
                break;
            case AVP_CODE_KASME:
                CHECK_FCT(s6a_parse_kasme(hdr, vector->kasme));
                ret &= ~0x08;
                break;
            default:
                /* Unexpected AVP */
                S6A_ERROR("Unexpected AVP with code %d\n", hdr->avp_code);
                return -1;
        }
        /* Go to next AVP in the grouped AVP */
        CHECK_FCT(fd_msg_browse(avp, MSG_BRW_NEXT, &avp, NULL));
    }
    if (ret) {
        S6A_ERROR("Missing AVP for E-UTRAN vector: %c%c%c%c\n",
                  ret & 0x01 ? 'R':'-', ret & 0x02 ? 'X':'-',
                  ret & 0x04 ? 'A':'-', ret & 0x08 ? 'K':'-');
        return -1;
    }
    return 0;
}

static inline
int s6a_parse_authentication_info_avp(struct avp *avp_auth_info,
                                      authentication_info_t *authentication_info)
{
    struct avp *avp;
    struct avp_hdr *hdr;

    CHECK_FCT(fd_msg_avp_hdr(avp_auth_info, &hdr));

    DevCheck(hdr->avp_code == AVP_CODE_AUTHENTICATION_INFO,
             hdr->avp_code, AVP_CODE_AUTHENTICATION_INFO, 0);

    authentication_info->nb_of_vectors = 0;

    CHECK_FCT(fd_msg_browse(avp_auth_info, MSG_BRW_FIRST_CHILD, &avp, NULL));
    while(avp) {
        CHECK_FCT(fd_msg_avp_hdr(avp, &hdr));

        switch(hdr->avp_code) {
            case AVP_CODE_E_UTRAN_VECTOR: {
                DevAssert(authentication_info->nb_of_vectors == 0);
                CHECK_FCT(s6a_parse_e_utran_vector(avp, &authentication_info->eutran_vector));
                authentication_info->nb_of_vectors ++;
            } break;
            default:
                /* We should only receive E-UTRAN-Vectors */
                S6A_ERROR("Unexpected AVP with code %d\n", hdr->avp_code);
                return -1;
        }
        /* Go to next AVP in the grouped AVP */
        CHECK_FCT(fd_msg_browse(avp, MSG_BRW_NEXT, &avp, NULL));
    }
    return 0;
}

int s6a_aia_cb(struct msg **msg, struct avp *paramavp,
               struct session *sess, void *opaque,
               enum disp_action *act)
{
    struct msg           *ans                = NULL;
    struct msg           *qry                = NULL;
    struct avp           *avp                = NULL;
    struct avp_hdr       *hdr                = NULL;

    MessageDef          *message_p           = NULL;
    s6a_auth_info_ans_t *s6a_auth_info_ans_p = NULL;

    int skip_auth_res = 0;

    DevAssert(msg != NULL);

    ans = *msg;
    /* Retrieve the original query associated with the asnwer */
    CHECK_FCT(fd_msg_answ_getq(ans, &qry));

    DevAssert(qry != NULL);

    message_p = itti_alloc_new_message(TASK_S6A, S6A_AUTH_INFO_ANS);
    s6a_auth_info_ans_p = &message_p->ittiMsg.s6a_auth_info_ans;

    S6A_DEBUG("Received S6A Authentication Information Answer (AIA)\n");

    CHECK_FCT(fd_msg_search_avp(qry, s6a_fd_cnf.dataobj_s6a_user_name, &avp));
    if (avp) {
        CHECK_FCT(fd_msg_avp_hdr(avp, &hdr));
        sprintf(s6a_auth_info_ans_p->imsi, "%*s", (int)hdr->avp_value->os.len,
                hdr->avp_value->os.data);
    } else {
        DevMessage("Query has been freed before we received the answer\n");
    }

    /* Retrieve the result-code */
    CHECK_FCT(fd_msg_search_avp(ans, s6a_fd_cnf.dataobj_s6a_result_code, &avp));
    if (avp) {
        CHECK_FCT(fd_msg_avp_hdr(avp, &hdr));

        s6a_auth_info_ans_p->result.present = S6A_RESULT_BASE;
        s6a_auth_info_ans_p->result.choice.base = hdr->avp_value->u32;

        if (hdr->avp_value->u32 != ER_DIAMETER_SUCCESS) {
            S6A_ERROR("Got error %u:%s\n", hdr->avp_value->u32,
                      retcode_2_string(hdr->avp_value->u32));
            goto err;
        } else {
            S6A_DEBUG("Received S6A Result code %u:%s\n",
                s6a_auth_info_ans_p->result.choice.base,
                retcode_2_string(s6a_auth_info_ans_p->result.choice.base));
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
            s6a_auth_info_ans_p->result.present = S6A_RESULT_EXPERIMENTAL;
            s6a_parse_experimental_result(avp, &s6a_auth_info_ans_p->result.choice.experimental);

            skip_auth_res = 1;
        } else {
            /* Neither result-code nor experimental-result is present ->
             * totally incorrect behaviour here.
             */
            S6A_ERROR("Experimental-Result and Result-Code are absent: "
                      "This is not a correct behaviour\n");
            goto err;
        }
    }

    if (skip_auth_res == 0) {
        CHECK_FCT(fd_msg_search_avp(ans, s6a_fd_cnf.dataobj_s6a_authentication_info, &avp));
        if (avp) {
            CHECK_FCT(s6a_parse_authentication_info_avp(avp, &s6a_auth_info_ans_p->auth_info));
        } else {
            DevMessage("We requested E-UTRAN vectors with an immediate response...\n");
        }
    }

    itti_send_msg_to_task(TASK_MME_APP, INSTANCE_DEFAULT, message_p);
err:
    return 0;
}

int s6a_generate_authentication_info_req(s6a_auth_info_req_t *air_p)
{
    struct avp     *avp;
    struct msg     *msg;
    struct session *sess;

    union avp_value value;

    DevAssert(air_p != NULL);

    /* Create the new update location request message */
    CHECK_FCT(fd_msg_new(s6a_fd_cnf.dataobj_s6a_air, 0, &msg));

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
    value.os.data = (unsigned char *)air_p->imsi;
    value.os.len  = strlen(air_p->imsi);
    CHECK_FCT(fd_msg_avp_setvalue(avp, &value));
    CHECK_FCT(fd_msg_avp_add(msg, MSG_BRW_LAST_CHILD, avp));

    /* Adding the visited plmn id */
    {
      uint8_t plmn[3] = { 0x00, 0x00, 0x00 }; //{ 0x02, 0xF8, 0x29 };

        CHECK_FCT(fd_msg_avp_new(s6a_fd_cnf.dataobj_s6a_visited_plmn_id, 0, &avp));


        PLMN_T_TO_TBCD(air_p->visited_plmn,
                       plmn,
                       mme_config_find_mnc_length(air_p->visited_plmn.MCCdigit1,
                               air_p->visited_plmn.MCCdigit2,
                               air_p->visited_plmn.MCCdigit3,
                               air_p->visited_plmn.MNCdigit1,
                               air_p->visited_plmn.MNCdigit2,
                               air_p->visited_plmn.MNCdigit3
                                                 )
                      );
        value.os.data = plmn;
        value.os.len  = 3;
        CHECK_FCT(fd_msg_avp_setvalue(avp, &value));

        CHECK_FCT(fd_msg_avp_add(msg, MSG_BRW_LAST_CHILD, avp));
        S6A_DEBUG("%s plmn: %02X%02X%02X\n",
            __FUNCTION__,
            plmn[0],
            plmn[1],
            plmn[2]);
        S6A_DEBUG("%s visited_plmn: %02X%02X%02X\n",
            __FUNCTION__,
            value.os.data[0],
            value.os.data[1],
            value.os.data[2]);
    }
    /* Adding the requested E-UTRAN authentication info AVP */
    {
        struct avp *child_avp;
        CHECK_FCT(fd_msg_avp_new(s6a_fd_cnf.dataobj_s6a_req_eutran_auth_info, 0, &avp));

        /* Add the number of requested vectors */
        CHECK_FCT(fd_msg_avp_new(s6a_fd_cnf.dataobj_s6a_number_of_requested_vectors, 0, &child_avp));
        value.u32 = air_p->nb_of_vectors;
        CHECK_FCT(fd_msg_avp_setvalue(child_avp, &value));
        CHECK_FCT(fd_msg_avp_add(avp, MSG_BRW_LAST_CHILD, child_avp));

        /* We want to use the vectors immediately in HSS so we have to add
         * the Immediate-Response-Preferred AVP.
         * Value of this AVP is not significant.
         */
        CHECK_FCT(fd_msg_avp_new(s6a_fd_cnf.dataobj_s6a_immediate_response_pref, 0, &child_avp));
        value.u32 = 0;
        CHECK_FCT(fd_msg_avp_setvalue(child_avp, &value));
        CHECK_FCT(fd_msg_avp_add(avp, MSG_BRW_LAST_CHILD, child_avp));

        /* Re-synchronization information containing the AUTS computed at USIM */
        if (air_p->re_synchronization) {
            CHECK_FCT(fd_msg_avp_new(s6a_fd_cnf.dataobj_s6a_re_synchronization_info, 0, &child_avp));
            value.os.len = AUTS_LENGTH;
            value.os.data = air_p->auts;
            CHECK_FCT(fd_msg_avp_setvalue(child_avp, &value));
            CHECK_FCT(fd_msg_avp_add(avp, MSG_BRW_LAST_CHILD, child_avp));
        }

        CHECK_FCT(fd_msg_avp_add(msg, MSG_BRW_LAST_CHILD, avp));
    }
    CHECK_FCT(fd_msg_send(&msg, NULL, NULL));

    return 0;
}

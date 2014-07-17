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

/*! \file s6a_subscription_data.c
 * \brief Add the subscription data to a message. Data are retrieved from database
 * \author Sebastien ROUX <sebastien.roux@eurecom.fr>
 * \date 2013
 * \version 0.1
 */

int s6a_add_subscription_data_avp(struct msg *message, mysql_ul_ans_t *mysql_ans)
{
    int             ret = -1, i = 0;
    mysql_pdn_t    *pdns = NULL;
    uint8_t         nb_pdns = 0;
    struct avp     *avp = NULL, *child_avp = NULL;
    union avp_value value;

    if (mysql_ans == NULL) {
        return -1;
    }

    ret = hss_mysql_query_pdns(mysql_ans->imsi, &pdns, &nb_pdns);

    if (ret != 0) {
        /* mysql query failed:
         * - maybe no more memory
         * - maybe user is not known (should have failed before)
         * - maybe imsi has no EPS subscribed
         */
        goto out;
    }

    if (nb_pdns == 0)
    {
        /* No PDN for this user -> DIAMETER_ERROR_UNKNOWN_EPS_SUBSCRIPTION */
        return -1;
    }

    /* Create the Subscription-Data AVP */
    CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_subscription_data, 0, &avp));

    {
        uint8_t msisdn_len = strlen(mysql_ans->msisdn);
        /* The MSISDN is known in the HSS, add it to the subscription data */
        if (msisdn_len > 0) {
            CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_msisdn, 0, &child_avp));
            value.os.data = (uint8_t *)mysql_ans->msisdn;
            value.os.len  = msisdn_len;
            CHECK_FCT(fd_msg_avp_setvalue(child_avp, &value));
            CHECK_FCT(fd_msg_avp_add(avp, MSG_BRW_LAST_CHILD, child_avp));
        }
    }

    /* We have to include the acess-restriction-data if the value stored in DB
     * indicates that at least one restriction is applied to the USER.
     */
    if (mysql_ans->access_restriction != 0) {
        CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_access_restriction_data, 0, &child_avp));
        value.u32 = (uint32_t)mysql_ans->access_restriction;
        CHECK_FCT(fd_msg_avp_setvalue(child_avp, &value));
        CHECK_FCT(fd_msg_avp_add(avp, MSG_BRW_LAST_CHILD, child_avp));
    }

    /* Add the Subscriber-Status to the list of AVP.
     * It shall indicate if the service is barred or granted.
     * TODO: normally this parameter comes from DB...
     */
    CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_subscriber_status, 0, &child_avp));
    /* SERVICE_GRANTED */
    value.u32 = 0;
    CHECK_FCT(fd_msg_avp_setvalue(child_avp, &value));
    CHECK_FCT(fd_msg_avp_add(avp, MSG_BRW_LAST_CHILD, child_avp));

    /* Add the Network-Access-Mode to the list of AVP.
     * LTE Standalone HSS/MME: ONLY_PACKET.
     */
    CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_network_access_mode, 0, &child_avp));
    /* SERVICE_GRANTED */
    value.u32 = 2;
    CHECK_FCT(fd_msg_avp_setvalue(child_avp, &value));
    CHECK_FCT(fd_msg_avp_add(avp, MSG_BRW_LAST_CHILD, child_avp));

    /* Add the AMBR to list of AVPs */
    {
        struct avp *bandwidth;
        CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_ambr, 0, &child_avp));

        /* Uplink bandwidth */
        CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_max_bandwidth_ul, 0, &bandwidth));
        value.u32 = mysql_ans->aggr_ul;
        CHECK_FCT(fd_msg_avp_setvalue(bandwidth, &value));
        CHECK_FCT(fd_msg_avp_add(child_avp, MSG_BRW_LAST_CHILD, bandwidth));

        /* Downlink bandwidth */
        CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_max_bandwidth_dl, 0, &bandwidth));
        value.u32 = mysql_ans->aggr_dl;
        CHECK_FCT(fd_msg_avp_setvalue(bandwidth, &value));
        CHECK_FCT(fd_msg_avp_add(child_avp, MSG_BRW_LAST_CHILD, bandwidth));

        CHECK_FCT(fd_msg_avp_add(avp, MSG_BRW_LAST_CHILD, child_avp));
    }

    /* Add the APN-Configuration-Profile only if at least one APN is subscribed */
    if (nb_pdns > 0)
    {
        struct avp *apn_profile;
        CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_apn_configuration_profile,
                                 0, &apn_profile));

        /* Context-Identifier */
        CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_context_identifier, 0, &child_avp));
        value.u32 = 0;
        /* TODO: this is the reference to the default APN... */
        CHECK_FCT(fd_msg_avp_setvalue(child_avp, &value));
        CHECK_FCT(fd_msg_avp_add(apn_profile, MSG_BRW_LAST_CHILD, child_avp));

        /* All-APN-Configurations-Included-Indicator */
        CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_all_apn_conf_inc_ind, 0, &child_avp));
        value.u32 = 0;
        CHECK_FCT(fd_msg_avp_setvalue(child_avp, &value));
        CHECK_FCT(fd_msg_avp_add(apn_profile, MSG_BRW_LAST_CHILD, child_avp));

        for (i = 0; i < nb_pdns; i++)
        {
            struct avp *apn_configuration;
            mysql_pdn_t *pdn_elm;

            pdn_elm = &pdns[i];

            /* APN-Configuration */
            CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_apn_configuration, 0, &apn_configuration));

            /* Context-Identifier */
            CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_context_identifier, 0, &child_avp));
            value.u32 = i;
            CHECK_FCT(fd_msg_avp_setvalue(child_avp, &value));
            CHECK_FCT(fd_msg_avp_add(apn_configuration, MSG_BRW_LAST_CHILD, child_avp));

            /* PDN-Type */
            CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_pdn_type, 0, &child_avp));
            value.u32 = pdn_elm->pdn_type;
            CHECK_FCT(fd_msg_avp_setvalue(child_avp, &value));
            CHECK_FCT(fd_msg_avp_add(apn_configuration, MSG_BRW_LAST_CHILD, child_avp));

            if ((pdn_elm->pdn_type == IPV4) ||
                (pdn_elm->pdn_type == IPV4_OR_IPV6) ||
                (pdn_elm->pdn_type == IPV4V6))
            {
                s6a_add_ipv4_address(apn_configuration, pdn_elm->pdn_address.ipv4_address);
            }
            if ((pdn_elm->pdn_type == IPV6) ||
                (pdn_elm->pdn_type == IPV4_OR_IPV6) ||
                (pdn_elm->pdn_type == IPV4V6))
            {
                s6a_add_ipv6_address(apn_configuration, pdn_elm->pdn_address.ipv6_address);
            }

            /* Service-Selection */
            CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_service_selection, 0, &child_avp));
            value.os.data = (uint8_t*)pdn_elm->apn;
            value.os.len  = strlen(pdn_elm->apn);
            CHECK_FCT(fd_msg_avp_setvalue(child_avp, &value));
            CHECK_FCT(fd_msg_avp_add(apn_configuration, MSG_BRW_LAST_CHILD, child_avp));

            /* Add the eps subscribed qos profile */
            {
                struct avp *qos_profile, *allocation_priority;
                CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_eps_subscribed_qos_profile, 0, &qos_profile));

                CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_qos_class_identifier, 0, &child_avp));
                /* For a QCI_1 */
                value.u32 = (uint32_t)pdn_elm->qci;
                CHECK_FCT(fd_msg_avp_setvalue(child_avp, &value));
                CHECK_FCT(fd_msg_avp_add(qos_profile, MSG_BRW_LAST_CHILD, child_avp));

                /* Allocation retention priority */
                {
                    CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_allocation_retention_priority, 0, &allocation_priority));

                    /* Priority level */
                    CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_priority_level, 0, &child_avp));
                    value.u32 = (uint32_t)pdn_elm->priority_level;
                    CHECK_FCT(fd_msg_avp_setvalue(child_avp, &value));
                    CHECK_FCT(fd_msg_avp_add(allocation_priority, MSG_BRW_LAST_CHILD, child_avp));

                    /* Pre-emption-capability */
                    CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_pre_emption_capability, 0, &child_avp));
                    value.u32 = (uint32_t)pdn_elm->pre_emp_cap;
                    CHECK_FCT(fd_msg_avp_setvalue(child_avp, &value));
                    CHECK_FCT(fd_msg_avp_add(allocation_priority, MSG_BRW_LAST_CHILD, child_avp));

                    /* Pre-emption-vulnerability */
                    CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_pre_emption_vulnerability, 0, &child_avp));
                    value.u32 = (uint32_t)pdn_elm->pre_emp_vul;
                    CHECK_FCT(fd_msg_avp_setvalue(child_avp, &value));
                    CHECK_FCT(fd_msg_avp_add(allocation_priority, MSG_BRW_LAST_CHILD, child_avp));

                    CHECK_FCT(fd_msg_avp_add(qos_profile, MSG_BRW_LAST_CHILD, allocation_priority));
                }
                CHECK_FCT(fd_msg_avp_add(apn_configuration, MSG_BRW_LAST_CHILD, qos_profile));
            }

            /* Add the AMBR to list of AVPs */
            {
                struct avp *bandwidth;
                CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_ambr, 0, &bandwidth));

                /* Uplink bandwidth */
                CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_max_bandwidth_ul, 0, &child_avp));
                value.u32 = (uint32_t)pdn_elm->aggr_ul;
                CHECK_FCT(fd_msg_avp_setvalue(child_avp, &value));
                CHECK_FCT(fd_msg_avp_add(bandwidth, MSG_BRW_LAST_CHILD, child_avp));

                /* Downlink bandwidth */
                CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_max_bandwidth_dl, 0, &child_avp));
                value.u32 = (uint32_t)pdn_elm->aggr_dl;
                CHECK_FCT(fd_msg_avp_setvalue(child_avp, &value));
                CHECK_FCT(fd_msg_avp_add(bandwidth, MSG_BRW_LAST_CHILD, child_avp));

                CHECK_FCT(fd_msg_avp_add(apn_configuration, MSG_BRW_LAST_CHILD, bandwidth));
            }

            CHECK_FCT(fd_msg_avp_add(apn_profile, MSG_BRW_LAST_CHILD, apn_configuration));
        }

        CHECK_FCT(fd_msg_avp_add(avp, MSG_BRW_LAST_CHILD, apn_profile));
    }

    /* Subscribed-Periodic-RAU-TAU-Timer */
    CHECK_FCT(fd_msg_avp_new(s6a_cnf.dataobj_s6a_subscribed_rau_tau_timer, 0, &child_avp));
    /* Request an RAU/TAU update every x seconds */
    value.u32 = (uint32_t)mysql_ans->rau_tau;
    CHECK_FCT(fd_msg_avp_setvalue(child_avp, &value));
    CHECK_FCT(fd_msg_avp_add(avp, MSG_BRW_LAST_CHILD, child_avp));

    /* Add the AVP to the message */
    CHECK_FCT(fd_msg_avp_add(message, MSG_BRW_LAST_CHILD, avp));

out:
    if (pdns) {
        free(pdns);
    }
    return ret;
}

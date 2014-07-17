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

static inline
int s6a_parse_subscriber_status(struct avp_hdr *hdr_sub_status,
                                subscriber_status_t *sub_status)
{
    DevCheck(hdr_sub_status->avp_value->u32 < SS_MAX,
             hdr_sub_status->avp_value->u32, SS_MAX, 0);

    *sub_status = hdr_sub_status->avp_value->u32;

    return 0;
}

static inline
int s6a_parse_msisdn(struct avp_hdr *hdr_msisdn, char *msisdn, uint8_t *length)
{
    int ret;

    DevCheck(hdr_msisdn->avp_value->os.len <= MSISDN_LENGTH,
             hdr_msisdn->avp_value->os.len, MSISDN_LENGTH, 0);

    if (hdr_msisdn->avp_value->os.len == 0)
        return 0;
    ret = sprintf(msisdn, "%*s", (int)hdr_msisdn->avp_value->os.len,
                  hdr_msisdn->avp_value->os.data);

    *length = ret;

    return 0;
}

static inline
int s6a_parse_network_access_mode(struct avp_hdr *hdr_network_am,
                                  network_access_mode_t *access_mode)
{
    DevCheck(hdr_network_am->avp_value->u32 < NAM_MAX &&
             hdr_network_am->avp_value->u32 != NAM_RESERVED,
             hdr_network_am->avp_value->u32, NAM_MAX, NAM_RESERVED);

    *access_mode = hdr_network_am->avp_value->u32;

    return 0;
}

static inline
int s6a_parse_access_restriction_data(struct avp_hdr *hdr_access_restriction,
                                      access_restriction_t *access_restriction)
{
    DevCheck(hdr_access_restriction->avp_value->u32 < ARD_MAX,
             hdr_access_restriction->avp_value->u32, ARD_MAX, 0);
    *access_restriction = hdr_access_restriction->avp_value->u32;
    return 0;
}

static inline
int s6a_parse_bitrate(struct avp_hdr *hdr_bitrate, bitrate_t *bitrate)
{
    *bitrate = hdr_bitrate->avp_value->u32;
    return 0;
}

static inline
int s6a_parse_ambr(struct avp *avp_ambr, ambr_t *ambr)
{
    struct avp *avp = NULL;
    struct avp_hdr *hdr;

    CHECK_FCT(fd_msg_browse(avp_ambr, MSG_BRW_FIRST_CHILD, &avp, NULL));
    if (!avp) {
        /* Child avps for ambr are mandatory */
        return -1;
    }
    while(avp) {
        CHECK_FCT(fd_msg_avp_hdr(avp, &hdr));
        switch(hdr->avp_code) {
            case AVP_CODE_BANDWIDTH_UL:
                CHECK_FCT(s6a_parse_bitrate(hdr, &ambr->br_ul));
                break;
            case AVP_CODE_BANDWIDTH_DL:
                CHECK_FCT(s6a_parse_bitrate(hdr, &ambr->br_dl));
                break;
            default:
                return -1;
        }
        /* Go to next AVP in the grouped AVP */
        CHECK_FCT(fd_msg_browse(avp, MSG_BRW_NEXT, &avp, NULL));
    }
    return 0;
}

static inline
int s6a_parse_all_apn_conf_inc_ind(struct avp_hdr *hdr, all_apn_conf_ind_t *ptr)
{
    DevCheck(hdr->avp_value->u32 < ALL_APN_MAX,
             hdr->avp_value->u32, ALL_APN_MAX, 0);
    *ptr = hdr->avp_value->u32;
    return 0;
}

static inline
int s6a_parse_pdn_type(struct avp_hdr *hdr, pdn_type_t *pdn_type)
{
    DevCheck(hdr->avp_value->u32 < IP_MAX,
             hdr->avp_value->u32, IP_MAX, 0);
    *pdn_type = hdr->avp_value->u32;
    return 0;
}

static inline
int s6a_parse_service_selection(struct avp_hdr *hdr_service_selection,
                                char *service_selection, int *length)
{
    DevCheck(hdr_service_selection->avp_value->os.len <= APN_MAX_LENGTH,
             hdr_service_selection->avp_value->os.len, APN_MAX_LENGTH, 0);
    *length = sprintf(service_selection, "%*s",
                      (int)hdr_service_selection->avp_value->os.len,
                      hdr_service_selection->avp_value->os.data);
    return 0;
}

static inline
int s6a_parse_qci(struct avp_hdr *hdr, qci_t *qci)
{
    DevCheck(hdr->avp_value->u32 < QCI_MAX,
             hdr->avp_value->u32, QCI_MAX, 0);
    *qci = hdr->avp_value->u32;
    return 0;
}

static inline
int s6a_parse_priority_level(struct avp_hdr *hdr, priority_level_t *priority_level)
{
    DevCheck(hdr->avp_value->u32 <= PRIORITY_LEVEL_MAX &&
             hdr->avp_value->u32 >= PRIORITY_LEVEL_MIN, hdr->avp_value->u32,
             PRIORITY_LEVEL_MAX, PRIORITY_LEVEL_MIN);
    *priority_level = (priority_level_t)hdr->avp_value->u32;
    return 0;
}

static inline
int s6a_parse_pre_emp_capability(struct avp_hdr *hdr, pre_emp_capability_t *pre_emp_capability)
{
    DevCheck(hdr->avp_value->u32 < PRE_EMPTION_CAPABILITY_MAX, hdr->avp_value->u32,
             PRE_EMPTION_CAPABILITY_MAX, 0);
    *pre_emp_capability = hdr->avp_value->u32;
    return 0;
}

static inline
int s6a_parse_pre_emp_vulnerability(struct avp_hdr *hdr, pre_emp_vulnerability_t *pre_emp_vulnerability)
{
    DevCheck(hdr->avp_value->u32 < PRE_EMPTION_VULNERABILITY_MAX, hdr->avp_value->u32,
             PRE_EMPTION_VULNERABILITY_MAX, 0);
    *pre_emp_vulnerability = hdr->avp_value->u32;
    return 0;
}

static inline
int s6a_parse_allocation_retention_priority(struct avp *avp_arp,
                                            allocation_retention_priority_t *ptr)
{
    struct avp *avp = NULL;
    struct avp_hdr *hdr;

    /* If the Pre-emption-Capability AVP is not present in the
     * Allocation-Retention-Priority AVP, the default value shall be
     * PRE-EMPTION_CAPABILITY_DISABLED (1).
     */
    ptr->pre_emp_capability = PRE_EMPTION_CAPABILITY_DISABLED;

    /* If the Pre-emption-Vulnerability AVP is not present in the
     * Allocation-Retention-Priority AVP, the default value shall be
     * PRE-EMPTION_VULNERABILITY_ENABLED (0).
     */
    ptr->pre_emp_vulnerability = PRE_EMPTION_VULNERABILITY_ENABLED;

    CHECK_FCT(fd_msg_browse(avp_arp, MSG_BRW_FIRST_CHILD, &avp, NULL));
    while(avp) {
        CHECK_FCT(fd_msg_avp_hdr(avp, &hdr));
        switch(hdr->avp_code) {
            case AVP_CODE_PRIORITY_LEVEL:
                CHECK_FCT(s6a_parse_priority_level(hdr, &ptr->priority_level));
                break;
            case AVP_CODE_PRE_EMPTION_CAPABILITY:
                CHECK_FCT(s6a_parse_pre_emp_capability(hdr, &ptr->pre_emp_capability));
                break;
            case AVP_CODE_PRE_EMPTION_VULNERABILITY:
                CHECK_FCT(s6a_parse_pre_emp_vulnerability(hdr, &ptr->pre_emp_vulnerability));
                break;
            default:
                return -1;
        }
        /* Go to next AVP in the grouped AVP */
        CHECK_FCT(fd_msg_browse(avp, MSG_BRW_NEXT, &avp, NULL));
    }
    return 0;
}

static inline
int s6a_parse_eps_subscribed_qos_profile(struct avp *avp_qos,
                                         eps_subscribed_qos_profile_t *ptr)
{
    struct avp *avp = NULL;
    struct avp_hdr *hdr;

    CHECK_FCT(fd_msg_browse(avp_qos, MSG_BRW_FIRST_CHILD, &avp, NULL));
    while(avp) {
        CHECK_FCT(fd_msg_avp_hdr(avp, &hdr));
        switch(hdr->avp_code) {
            case AVP_CODE_QCI:
                CHECK_FCT(s6a_parse_qci(hdr, &ptr->qci));
                break;
            case AVP_CODE_ALLOCATION_RETENTION_PRIORITY:
                CHECK_FCT(s6a_parse_allocation_retention_priority(avp, &ptr->allocation_retention_priority));
                break;
            default:
                return -1;
        }
        /* Go to next AVP in the grouped AVP */
        CHECK_FCT(fd_msg_browse(avp, MSG_BRW_NEXT, &avp, NULL));
    }
    return 0;
}

static inline
int s6a_parse_ip_address(struct avp_hdr *hdr, ip_address_t *ip_address)
{
    uint16_t ip_type;

    DevCheck(hdr->avp_value->os.len >= 2, hdr->avp_value->os.len, 2, 0);

    ip_type = (hdr->avp_value->os.data[0] << 8) | (hdr->avp_value->os.data[1]);

    if (ip_type == IANA_IPV4) {
        /* This is an IPv4 address */
        ip_address->pdn_type = IPv4;
        DevCheck(hdr->avp_value->os.len == 6, hdr->avp_value->os.len, 6, ip_type);
        memcpy(ip_address->address.ipv4_address, &hdr->avp_value->os.data[2], 4);
    } else if (ip_type == IANA_IPV6) {
        /* This is an IPv6 address */
        ip_address->pdn_type = IPv6;
        DevCheck(hdr->avp_value->os.len == 18, hdr->avp_value->os.len, 18, ip_type);
        memcpy(ip_address->address.ipv6_address, &hdr->avp_value->os.data[2], 16);
    } else {
        /* unhandled case... */
        return -1;
    }
    return 0;
}

static inline
int s6a_parse_apn_configuration(struct avp *avp_apn_conf_prof, apn_configuration_t *apn_config)
{
    struct avp *avp = NULL;
    struct avp_hdr *hdr;

    CHECK_FCT(fd_msg_browse(avp_apn_conf_prof, MSG_BRW_FIRST_CHILD, &avp, NULL));
    while(avp) {
        CHECK_FCT(fd_msg_avp_hdr(avp, &hdr));
        switch(hdr->avp_code) {
            case AVP_CODE_CONTEXT_IDENTIFIER:
                apn_config->context_identifier = hdr->avp_value->u32;
                break;
            case AVP_CODE_SERVED_PARTY_IP_ADDRESS:
                if (apn_config->nb_ip_address == 2) {
                    DevMessage("Only two IP addresses can be provided");
                }
                CHECK_FCT(s6a_parse_ip_address(hdr, &apn_config->ip_address[apn_config->nb_ip_address]));
                apn_config->nb_ip_address++;
                break;
            case AVP_CODE_PDN_TYPE:
                CHECK_FCT(s6a_parse_pdn_type(hdr, &apn_config->pdn_type));
                break;
            case AVP_CODE_SERVICE_SELECTION:
                CHECK_FCT(s6a_parse_service_selection(hdr, apn_config->service_selection,
                                                      &apn_config->service_selection_length));
                break;
            case AVP_CODE_EPS_SUBSCRIBED_QOS_PROFILE:
                CHECK_FCT(s6a_parse_eps_subscribed_qos_profile(avp, &apn_config->subscribed_qos));
                break;
            case AVP_CODE_AMBR:
                CHECK_FCT(s6a_parse_ambr(avp, &apn_config->ambr));
                break;
        }
        /* Go to next AVP in the grouped AVP */
        CHECK_FCT(fd_msg_browse(avp, MSG_BRW_NEXT, &avp, NULL));
    }
    return 0;
}

static inline
int s6a_parse_apn_configuration_profile(struct avp *avp_apn_conf_prof,
                                        apn_config_profile_t *apn_config_profile)
{
    struct avp *avp = NULL;
    struct avp_hdr *hdr;

    CHECK_FCT(fd_msg_browse(avp_apn_conf_prof, MSG_BRW_FIRST_CHILD, &avp, NULL));
    while(avp) {
        CHECK_FCT(fd_msg_avp_hdr(avp, &hdr));
        switch(hdr->avp_code) {
            case AVP_CODE_CONTEXT_IDENTIFIER:
                apn_config_profile->context_identifier = hdr->avp_value->u32;
                break;
            case AVP_CODE_ALL_APN_CONFIG_INC_IND:
                CHECK_FCT(s6a_parse_all_apn_conf_inc_ind(hdr, &apn_config_profile->all_apn_conf_ind));
                break;
            case AVP_CODE_APN_CONFIGURATION: {
                DevCheck(apn_config_profile->nb_apns < MAX_APN_PER_UE,
                         apn_config_profile->nb_apns, MAX_APN_PER_UE, 0);
                CHECK_FCT(s6a_parse_apn_configuration(
                    avp, &apn_config_profile->apn_configuration[apn_config_profile->nb_apns]));
                apn_config_profile->nb_apns++;
            } break;
        }
        /* Go to next AVP in the grouped AVP */
        CHECK_FCT(fd_msg_browse(avp, MSG_BRW_NEXT, &avp, NULL));
    }
    return 0;
}

int s6a_parse_subscription_data(struct avp *avp_subscription_data,
                                subscription_data_t *subscription_data)
{
    struct avp *avp = NULL;
    struct avp_hdr *hdr;

    CHECK_FCT(fd_msg_browse(avp_subscription_data, MSG_BRW_FIRST_CHILD, &avp, NULL));
    while(avp) {
        CHECK_FCT(fd_msg_avp_hdr(avp, &hdr));

        switch(hdr->avp_code) {
            case AVP_CODE_SUBSCRIBER_STATUS:
                CHECK_FCT(s6a_parse_subscriber_status(hdr, &subscription_data->subscriber_status));
                break;
            case AVP_CODE_MSISDN:
                CHECK_FCT(s6a_parse_msisdn(hdr, subscription_data->msisdn,
                                           &subscription_data->msisdn_length));
                break;
            case AVP_CODE_NETWORK_ACCESS_MODE:
                CHECK_FCT(s6a_parse_network_access_mode(hdr, &subscription_data->access_mode));
                break;
            case AVP_CODE_ACCESS_RESTRICTION_DATA:
                CHECK_FCT(s6a_parse_access_restriction_data(hdr, &subscription_data->access_restriction));
                break;
            case AVP_CODE_AMBR:
                CHECK_FCT(s6a_parse_ambr(avp, &subscription_data->subscribed_ambr));
                break;
            case AVP_CODE_APN_CONFIGURATION_PROFILE:
                CHECK_FCT(s6a_parse_apn_configuration_profile(avp, &subscription_data->apn_config_profile));
                break;
            case AVP_CODE_SUBSCRIBED_PERIODIC_RAU_TAU_TIMER:
                subscription_data->rau_tau_timer = hdr->avp_value->u32;
                break;
            default:
                return -1;
        }
        /* Go to next AVP in the grouped AVP */
        CHECK_FCT(fd_msg_browse(avp, MSG_BRW_NEXT, &avp, NULL));
    }
    return 0;
}

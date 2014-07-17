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

#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdcore.h>

#include "hss_config.h"

#ifndef S6A_PROTO_H_
#define S6A_PROTO_H_

#define VENDOR_3GPP (10415)
#define APP_S6A     (16777251)

/* Errors that fall within the Permanent Failures category shall be used to
 * inform the peer that the request has failed, and should not be attempted
 * again. The Result-Code AVP values defined in Diameter Base Protocol RFC 3588
 * shall be applied. When one of the result codes defined here is included in a
 * response, it shall be inside an Experimental-Result AVP and the Result-Code
 * AVP shall be absent.
 */
#define DIAMETER_ERROR_USER_UNKNOWN             (5001)
#define DIAMETER_ERROR_ROAMING_NOT_ALLOWED      (5004)
#define DIAMETER_ERROR_UNKNOWN_EPS_SUBSCRIPTION (5420)
#define DIAMETER_ERROR_RAT_NOT_ALLOWED          (5421)
#define DIAMETER_ERROR_EQUIPMENT_UNKNOWN        (5422)
#define DIAMETER_ERROR_UNKOWN_SERVING_NODE      (5423)

/* Result codes that fall within the transient failures category shall be used
 * to inform a peer that the request could not be satisfied at the time it was
 * received, but may be able to satisfy the request in the future. The
 * Result-Code AVP values defined in Diameter Base Protocol RFC 3588 shall be
 * applied. When one of the result codes defined here is included in a response,
 * it shall be inside an Experimental-Result AVP and the Result-Code AVP shall
 * be absent.
 */
#define DIAMETER_AUTHENTICATION_DATA_UNAVAILABLE (4181)

#define DIAMETER_ERROR_IS_VENDOR(x)                    \
   ((x == DIAMETER_ERROR_USER_UNKNOWN)              || \
    (x == DIAMETER_ERROR_ROAMING_NOT_ALLOWED)       || \
    (x == DIAMETER_ERROR_UNKNOWN_EPS_SUBSCRIPTION)  || \
    (x == DIAMETER_ERROR_RAT_NOT_ALLOWED)           || \
    (x == DIAMETER_ERROR_EQUIPMENT_UNKNOWN)         || \
    (x == DIAMETER_AUTHENTICATION_DATA_UNAVAILABLE) || \
    (x == DIAMETER_ERROR_UNKOWN_SERVING_NODE))

#define AVP_CODE_IMEI                    (1402)
#define AVP_CODE_SOFTWARE_VERSION        (1403)
#define AVP_CODE_NUMBER_OF_REQ_VECTORS   (1410)
#define AVP_CODE_RE_SYNCHRONIZATION_INFO (1411)
#define AVP_CODE_IMMEDIATE_RESP_PREF     (1412)
#define AVP_CODE_3GPP2_MEID              (1471)
#define AVP_CODE_FEATURE_LIST            (629)
#define AVP_CODE_VENDOR_ID               (266)

typedef struct {
    struct dict_object *dataobj_s6a_vendor;     /* s6a vendor object */
    struct dict_object *dataobj_s6a_app;        /* s6a application object */
    /* Commands */
    struct dict_object *dataobj_s6a_auth_cmd;   /* s6a authentication command */
    struct dict_object *dataobj_s6a_loc_up;     /* s6a update location */
    struct dict_object *dataobj_s6a_purge_ue;     /* s6a purge ue req */
    struct dict_object *dataobj_s6a_cancel_loc_req; /* s6a Cancel Location req */
    struct dict_object *dataobj_s6a_cancel_loc_ans; /* s6a Cancel Location ans */
    /* AVPs */
    struct dict_object *dataobj_s6a_origin_host;
    struct dict_object *dataobj_s6a_origin_realm;
    struct dict_object *dataobj_s6a_imsi;            /* s6a imsi avp */
    struct dict_object *dataobj_s6a_imei;
    struct dict_object *dataobj_s6a_software_version;
    struct dict_object *dataobj_s6a_supported_features;
    struct dict_object *dataobj_s6a_req_e_utran_auth_info;
    struct dict_object *dataobj_s6a_req_resync_info;
    struct dict_object *dataobj_s6a_req_nb_of_req_vectors;
    struct dict_object *dataobj_s6a_req_geran_auth_info;
    struct dict_object *dataobj_s6a_immediate_response_pref;
    struct dict_object *dataobj_s6a_visited_plmn_id;
    struct dict_object *dataobj_s6a_result_code;
    struct dict_object *dataobj_s6a_experimental_result;
    struct dict_object *dataobj_s6a_vendor_id;
    struct dict_object *dataobj_s6a_experimental_result_code;
    struct dict_object *dataobj_s6a_auth_session_state;
    struct dict_object *dataobj_s6a_authentication_info;
    struct dict_object *dataobj_s6a_e_utran_vector;
    struct dict_object *dataobj_s6a_rand;
    struct dict_object *dataobj_s6a_xres;
    struct dict_object *dataobj_s6a_autn;
    struct dict_object *dataobj_s6a_kasme;
    struct dict_object *dataobj_s6a_ulr_flags;
    struct dict_object *dataobj_s6a_ula_flags;
    struct dict_object *dataobj_s6a_pur_flags;
    struct dict_object *dataobj_s6a_pua_flags;
    struct dict_object *dataobj_s6a_rat_type;
    struct dict_object *dataobj_s6a_terminal_info;
    struct dict_object *dataobj_s6a_ue_srvcc_cap;
    struct dict_object *dataobj_s6a_gmlc_addr;
    struct dict_object *dataobj_s6a_subscription_data;
    struct dict_object *dataobj_s6a_subscriber_status;
    struct dict_object *dataobj_s6a_msisdn;
    struct dict_object *dataobj_s6a_ambr;
    struct dict_object *dataobj_s6a_network_access_mode;
    struct dict_object *dataobj_s6a_access_restriction_data;
    struct dict_object *dataobj_s6a_apn_configuration_profile;
    struct dict_object *dataobj_s6a_subscribed_rau_tau_timer;
    struct dict_object *dataobj_s6a_context_identifier;
    /* All-APN-Configurations-Included-Indicator */
    struct dict_object *dataobj_s6a_all_apn_conf_inc_ind;
    struct dict_object *dataobj_s6a_apn_configuration;
    /* Max-Requested-Bandwidth-UL */
    struct dict_object *dataobj_s6a_max_bandwidth_ul;
    /* Max-Requested-Bandwidth-DL */
    struct dict_object *dataobj_s6a_max_bandwidth_dl;
    struct dict_object *dataobj_s6a_pdn_type;
    struct dict_object *dataobj_s6a_service_selection;
    struct dict_object *dataobj_s6a_eps_subscribed_qos_profile;
    struct dict_object *dataobj_s6a_qos_class_identifier;
    struct dict_object *dataobj_s6a_allocation_retention_priority;
    struct dict_object *dataobj_s6a_priority_level;
    struct dict_object *dataobj_s6a_pre_emption_capability;
    struct dict_object *dataobj_s6a_pre_emption_vulnerability;
    struct dict_object *dataobj_s6a_served_party_ip_addr;
} s6a_cnf_t;

/* External definition of the S6A decriptor */
extern s6a_cnf_t s6a_cnf;

/* Length of IMSI should not exceed 15 digits */
#define IMSI_LENGTH (15)
#define IMEI_LENGTH (15)
#define SV_LENGTH   (2)

/* ULR-Flags meaning: */
#define ULR_SINGLE_REGISTRATION_IND       (1U)
#define ULR_S6A_S6D_INDICATOR             (1U << 1)
#define ULR_SKIP_SUBSCRIBER_DATA          (1U << 2)
#define ULR_GPRS_SUBSCRIPTION_DATA_IND    (1U << 3)
#define ULR_NODE_TYPE_IND                 (1U << 4)
#define ULR_INITIAL_ATTACH_IND            (1U << 5)
#define ULR_PS_LCS_SUPPORTED_BY_UE        (1U << 6)
#define ULR_PAD_VALID(x)                 ((x & ~0x7f) == 0)

/* ULA-Flags */
#define ULA_SEPARATION_IND          (1U)

/* PUR-Flags */
#define PUR_UE_PURGED_IN_MME      (1U)
#define PUR_UE_PURGED_IN_SGSN     (1U << 1)
#define PUR_PAD_VALID(x)         ((x & ~0x3) == 0)

/* PUA-FLAGS */
#define PUA_FREEZE_M_TMSI   (1U)
#define PUA_FREEZE_P_TMSI   (1U << 1)

/* Access-restriction-data bits */
#define UTRAN_NOT_ALLOWED            (1U)
#define GERAN_NOT_ALLOWED            (1U << 1)
#define GAN_NOT_ALLOWED              (1U << 2)
#define I_HSDPA_EVO_NOT_ALLOWED      (1U << 3)
#define E_UTRAN_NOT_ALLOWED          (1U << 4)
#define HO_TO_NON_3GPP_NOT_ALLOWED   (1U << 5)
#define ARD_CHECK_PAD(x)            ((x & ~0x3f) == 0)

#define FLAG_IS_SET(x, flag)  ((x) & (flag))

#define FLAGS_SET(x, flags)   ((x) |= (flags))

#define FLAGS_CLEAR(x, flags) ((x) = (x) & ~(flags))

/** \brief Initialize the s6a layer using freeDiameter
 * \param hss_config_p pointer the global HSS configuration
 * @returns 0 if the init was successfull, != 0 in case of failure
 */
int s6a_init(hss_config_t *hss_config_p);

/** \brief Callback function for new equipements connections to the HSS. See
 * libfdproto from freeDiameter for more informations on the peer validate
 * callback
 * \param info Peer freeDiameter info
 * \param auth Result of the peer authentication, possible values:
 *  * 1  = accept
 *  * 0  = unknown
 *  * -1 = reject
 * \param cb2 possible second callback to validate the TLS authentication
 * @return 0 if the init was successfull, != 0 in case of failure
 */
int s6a_peer_validate(struct peer_info *info, int *auth, int (**cb2)(struct peer_info *));

/* Callback called when corresponding request/answer is received */
int s6a_auth_info_cb(struct msg **msg, struct avp *paramavp,
                     struct session *sess, void *opaque,
                     enum disp_action *act);

int s6a_up_loc_cb(struct msg **msg, struct avp *paramavp,
                  struct session *sess, void *opaque,
                  enum disp_action *act);

int s6a_purge_ue_cb(struct msg **msg, struct avp *paramavp,
                    struct session *sess, void *opaque,
                    enum disp_action *act);

int s6a_add_subscription_data_avp(struct msg *message, mysql_ul_ans_t *msql_ans);

int s6a_add_result_code(struct msg *ans, struct avp *failed_avp,
                        int result_code, int experimental);

int s6a_add_ipv4_address(struct avp *avp, const char *ipv4_addr);
int s6a_add_ipv6_address(struct avp *avp, const char *ipv6_addr);
inline char *experimental_retcode_2_string(int ret_code);
inline char *retcode_2_string(int ret_code);

#endif /* S6A_PROTO_H_ */

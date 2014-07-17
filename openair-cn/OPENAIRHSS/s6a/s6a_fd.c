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

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdcore.h>

#include "hss_config.h"
#include "db_proto.h"
#include "s6a_proto.h"

/* session handler for s6a sessions state machine */
static struct session_handler *s6a_reg = NULL;

/* handler for s6a server callback */
static struct disp_hdl *handle;
s6a_cnf_t s6a_cnf;

#if !defined(FREEDIAMETER_VERSION_1_2_0)
void s6a_cli_sess_cleanup(void * arg, char * sid, void * opaque);

void s6a_cli_sess_cleanup(void * arg, char * sid, void * opaque)
{
    
}
#else
void s6a_cli_sess_cleanup(struct sess_state * state, os0_t sid, void * opaque);
void s6a_cli_sess_cleanup(struct sess_state * state, os0_t sid, void * opaque)
{

}

#endif


static int s6a_init_objs(void)
{
    vendor_id_t vendor_3gpp = VENDOR_3GPP;
    application_id_t app_s6a = APP_S6A;

    memset(&s6a_cnf, 0, sizeof(s6a_cnf));

    /* Pre-loading vendor object */
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_VENDOR, VENDOR_BY_ID,
                             (void*)&vendor_3gpp, &s6a_cnf.dataobj_s6a_vendor, ENOENT));
    /* Pre-loading application object */
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_APPLICATION,
                             APPLICATION_BY_ID, (void*)&app_s6a,
                             &s6a_cnf.dataobj_s6a_app, ENOENT));

    /* Pre-loading commands objects */
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_COMMAND,
                             CMD_BY_NAME, "Authentication-Information-Request",
                             &s6a_cnf.dataobj_s6a_auth_cmd, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_COMMAND,
                             CMD_BY_NAME, "Update-Location-Request",
                             &s6a_cnf.dataobj_s6a_loc_up, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_COMMAND,
                             CMD_BY_NAME, "Purge-UE-Request",
                             &s6a_cnf.dataobj_s6a_purge_ue, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_COMMAND,
                             CMD_BY_NAME, "Cancel-Location-Request",
                             &s6a_cnf.dataobj_s6a_cancel_loc_req, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_COMMAND,
                             CMD_BY_NAME, "Cancel-Location-Answer",
                             &s6a_cnf.dataobj_s6a_cancel_loc_ans, ENOENT));

    /* Pre-loading AVPs objects */
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME, "Origin-Host",
                             &s6a_cnf.dataobj_s6a_origin_host, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME, "Origin-Realm",
                             &s6a_cnf.dataobj_s6a_origin_realm, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "User-Name",
                             &s6a_cnf.dataobj_s6a_imsi, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "IMEI",
                             &s6a_cnf.dataobj_s6a_imei, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Software-Version",
                             &s6a_cnf.dataobj_s6a_software_version, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Supported-Features",
                             &s6a_cnf.dataobj_s6a_supported_features, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Requested-EUTRAN-Authentication-Info",
                             &s6a_cnf.dataobj_s6a_req_e_utran_auth_info, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Number-Of-Requested-Vectors",
                             &s6a_cnf.dataobj_s6a_req_nb_of_req_vectors, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Re-Synchronization-Info",
                             &s6a_cnf.dataobj_s6a_req_resync_info, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Requested-UTRAN-GERAN-Authentication-Info",
                             &s6a_cnf.dataobj_s6a_req_geran_auth_info, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Immediate-Response-Preferred",
                             &s6a_cnf.dataobj_s6a_immediate_response_pref, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Visited-PLMN-Id",
                             &s6a_cnf.dataobj_s6a_visited_plmn_id, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME, "Result-Code",
                             &s6a_cnf.dataobj_s6a_result_code, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME, "Experimental-Result",
                             &s6a_cnf.dataobj_s6a_experimental_result, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME, "Vendor-Id",
                             &s6a_cnf.dataobj_s6a_vendor_id, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME, "Experimental-Result-Code",
                             &s6a_cnf.dataobj_s6a_experimental_result_code, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Auth-Session-State",
                             &s6a_cnf.dataobj_s6a_auth_session_state, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Authentication-Info",
                             &s6a_cnf.dataobj_s6a_authentication_info, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "E-UTRAN-Vector",
                             &s6a_cnf.dataobj_s6a_e_utran_vector, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "RAND",
                             &s6a_cnf.dataobj_s6a_rand, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "XRES",
                             &s6a_cnf.dataobj_s6a_xres, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "AUTN",
                             &s6a_cnf.dataobj_s6a_autn, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "KASME",
                             &s6a_cnf.dataobj_s6a_kasme, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "ULR-Flags",
                             &s6a_cnf.dataobj_s6a_ulr_flags, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "ULA-Flags",
                             &s6a_cnf.dataobj_s6a_ula_flags, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "PUR-Flags",
                             &s6a_cnf.dataobj_s6a_pur_flags, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "PUA-Flags",
                             &s6a_cnf.dataobj_s6a_pua_flags, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "RAT-Type",
                             &s6a_cnf.dataobj_s6a_rat_type, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Terminal-Information",
                             &s6a_cnf.dataobj_s6a_terminal_info, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "UE-SRVCC-Capability",
                             &s6a_cnf.dataobj_s6a_ue_srvcc_cap, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "GMLC-Address",
                             &s6a_cnf.dataobj_s6a_gmlc_addr, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Subscription-Data",
                             &s6a_cnf.dataobj_s6a_subscription_data, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Subscriber-Status",
                             &s6a_cnf.dataobj_s6a_subscriber_status, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "MSISDN",
                             &s6a_cnf.dataobj_s6a_msisdn, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "AMBR",
                             &s6a_cnf.dataobj_s6a_ambr, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Network-Access-Mode",
                             &s6a_cnf.dataobj_s6a_network_access_mode, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Access-Restriction-Data",
                             &s6a_cnf.dataobj_s6a_access_restriction_data, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "APN-Configuration-Profile",
                             &s6a_cnf.dataobj_s6a_apn_configuration_profile, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Subscribed-Periodic-RAU-TAU-Timer",
                             &s6a_cnf.dataobj_s6a_subscribed_rau_tau_timer, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Context-Identifier",
                             &s6a_cnf.dataobj_s6a_context_identifier, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "All-APN-Configurations-Included-Indicator",
                             &s6a_cnf.dataobj_s6a_all_apn_conf_inc_ind, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "APN-Configuration",
                             &s6a_cnf.dataobj_s6a_apn_configuration, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Max-Requested-Bandwidth-UL",
                             &s6a_cnf.dataobj_s6a_max_bandwidth_ul, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Max-Requested-Bandwidth-DL",
                             &s6a_cnf.dataobj_s6a_max_bandwidth_dl, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "PDN-Type",
                             &s6a_cnf.dataobj_s6a_pdn_type, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Service-Selection",
                             &s6a_cnf.dataobj_s6a_service_selection, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "EPS-Subscribed-QoS-Profile",
                             &s6a_cnf.dataobj_s6a_eps_subscribed_qos_profile, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "QoS-Class-Identifier",
                             &s6a_cnf.dataobj_s6a_qos_class_identifier, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Allocation-Retention-Priority",
                             &s6a_cnf.dataobj_s6a_allocation_retention_priority, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Priority-Level",
                             &s6a_cnf.dataobj_s6a_priority_level, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Pre-emption-Capability",
                             &s6a_cnf.dataobj_s6a_pre_emption_capability, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Pre-emption-Vulnerability",
                             &s6a_cnf.dataobj_s6a_pre_emption_vulnerability, ENOENT));
    CHECK_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                             AVP_BY_NAME_ALL_VENDORS, "Served-Party-IP-Address",
                             &s6a_cnf.dataobj_s6a_served_party_ip_addr, ENOENT));

    /* Advertise the support for the test application in the peer */
    CHECK_FCT(fd_disp_app_support(s6a_cnf.dataobj_s6a_app,
                                  s6a_cnf.dataobj_s6a_vendor, 1, 0));

    return 0;
}

int s6a_init(hss_config_t *hss_config_p)
{
    int ret = 0;
    struct disp_when when;
    char why[100];

    fprintf(stdout, "Initializing s6a layer\n");

    ret = fd_core_initialize();
    if (ret != 0) {
        strcpy(why, "fd_core_initialize");
        goto err;
    }

    /* Parse the external configuration file */
    ret = fd_core_parseconf(hss_config_p->freediameter_config);
    if (ret != 0) {
        strcpy(why, "fd_core_parseconf");
        goto err;
    }


    ret = fd_core_start();
    if (ret != 0) {
        strcpy(why, "fd_core_start");
        goto err;
    }


    /* We wait till freediameter has completed loading extensions */
    fd_core_waitstartcomplete();

    /* Register the peer acceptor/rejector */
    fd_peer_validate_register(s6a_peer_validate);

    /* Initialize useful objects */
    ret = s6a_init_objs();
    if (ret != 0) {
        strcpy(why, "s6a_init_objs");
        goto err;
    }

    /* Create handler for sessions */
#if !defined(FREEDIAMETER_VERSION_1_2_0)
    CHECK_FCT(fd_sess_handler_create(&s6a_reg, s6a_cli_sess_cleanup, NULL));
#else
    session_state_dump dumper;
    CHECK_FCT(fd_sess_handler_create(&s6a_reg, s6a_cli_sess_cleanup, dumper, NULL));
#endif

    /* Register the callback */
    memset(&when, 0, sizeof(when));
    when.command = s6a_cnf.dataobj_s6a_auth_cmd;
    when.app     = s6a_cnf.dataobj_s6a_app;

    /* Register the callbacks for S6A Application */
    CHECK_FCT(fd_disp_register(s6a_auth_info_cb, DISP_HOW_CC, &when, NULL,
                               &handle));

    if (handle == NULL) {
        strcpy(why, "cannot register authentication info req cb");
        goto err;
    }

    when.command = s6a_cnf.dataobj_s6a_loc_up;
    when.app     = s6a_cnf.dataobj_s6a_app;

    /* Register the callbacks for S6A Application */
    CHECK_FCT(fd_disp_register(s6a_up_loc_cb, DISP_HOW_CC, &when, NULL,
                               &handle));

    if (handle == NULL) {
        strcpy(why, "cannot register update location req cb");
        goto err;
    }

    when.command = s6a_cnf.dataobj_s6a_purge_ue;
    when.app     = s6a_cnf.dataobj_s6a_app;

    /* Register the callbacks for S6A Application */
    CHECK_FCT(fd_disp_register(s6a_purge_ue_cb, DISP_HOW_CC, &when, NULL,
                               &handle));

    if (handle == NULL) {
        strcpy(why, "cannot register purge ue req cb");
        goto err;
    }

    fprintf(stdout, "Initializing s6a layer: DONE\n");

    return 0;

err:
    fprintf(stdout, "Initializing s6a layer: FAILED (%s)\n", why);
    return -1;
}

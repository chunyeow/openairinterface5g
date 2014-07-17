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

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "intertask_interface.h"
#include "s6a_defs.h"
#include "s6a_messages.h"
#include "assertions.h"

#define CHECK_FD_FCT(fCT)  DevAssert(fCT == 0);

/*! \file s6a_dict.c
 * \brief Initialize s6a dictionnary and setup callbacks for procedures
 * \author Sebastien ROUX <sebastien.roux@eurecom.fr>
 * \date 2013
 * \version 0.1
 */

int s6a_fd_init_dict_objs(void)
{
    struct disp_when when;

    vendor_id_t vendor_3gpp = VENDOR_3GPP;
    application_id_t app_s6a = APP_S6A;

    /* Pre-loading vendor object */
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict,
                                DICT_VENDOR,
                                VENDOR_BY_ID,
                                (void *)&vendor_3gpp,
                                &s6a_fd_cnf.dataobj_s6a_vendor,
                                ENOENT));

    /* Pre-loading application object */
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_APPLICATION,
                                APPLICATION_BY_ID, (void *)&app_s6a,
                                &s6a_fd_cnf.dataobj_s6a_app, ENOENT));

    /* Pre-loading commands objects */
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_COMMAND,
                                CMD_BY_NAME, "Authentication-Information-Request",
                                &s6a_fd_cnf.dataobj_s6a_air, ENOENT));
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_COMMAND,
                                CMD_BY_NAME, "Authentication-Information-Answer",
                                &s6a_fd_cnf.dataobj_s6a_aia, ENOENT));

    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_COMMAND,
                                CMD_BY_NAME, "Update-Location-Request",
                                &s6a_fd_cnf.dataobj_s6a_ulr, ENOENT));
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_COMMAND,
                                CMD_BY_NAME, "Update-Location-Answer",
                                &s6a_fd_cnf.dataobj_s6a_ula, ENOENT));

    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_COMMAND,
                                CMD_BY_NAME, "Purge-UE-Request",
                                &s6a_fd_cnf.dataobj_s6a_pur, ENOENT));
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_COMMAND,
                                CMD_BY_NAME, "Purge-UE-Answer",
                                &s6a_fd_cnf.dataobj_s6a_pua, ENOENT));

    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_COMMAND,
                                CMD_BY_NAME, "Cancel-Location-Request",
                                &s6a_fd_cnf.dataobj_s6a_clr, ENOENT));
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_COMMAND,
                                CMD_BY_NAME, "Cancel-Location-Answer",
                                &s6a_fd_cnf.dataobj_s6a_cla, ENOENT));

    /* Pre-loading base avps */
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                                AVP_BY_NAME, "Destination-Host",
                                &s6a_fd_cnf.dataobj_s6a_destination_host, ENOENT));
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                                AVP_BY_NAME, "Destination-Realm",
                                &s6a_fd_cnf.dataobj_s6a_destination_realm, ENOENT));
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                                AVP_BY_NAME, "User-Name",
                                &s6a_fd_cnf.dataobj_s6a_user_name, ENOENT));
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                                AVP_BY_NAME, "Session-Id",
                                &s6a_fd_cnf.dataobj_s6a_session_id, ENOENT));
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                                AVP_BY_NAME, "Auth-Session-State",
                                &s6a_fd_cnf.dataobj_s6a_auth_session_state, ENOENT));
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                                AVP_BY_NAME, "Result-Code",
                                &s6a_fd_cnf.dataobj_s6a_result_code, ENOENT));
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                                AVP_BY_NAME, "Experimental-Result",
                                &s6a_fd_cnf.dataobj_s6a_experimental_result, ENOENT));

    /* Pre-loading S6A specifics AVPs */
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                                AVP_BY_NAME_ALL_VENDORS, "Visited-PLMN-Id",
                                &s6a_fd_cnf.dataobj_s6a_visited_plmn_id, ENOENT));
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                                AVP_BY_NAME_ALL_VENDORS, "RAT-Type",
                                &s6a_fd_cnf.dataobj_s6a_rat_type, ENOENT));
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                                AVP_BY_NAME_ALL_VENDORS, "ULR-Flags",
                                &s6a_fd_cnf.dataobj_s6a_ulr_flags, ENOENT));
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                                AVP_BY_NAME_ALL_VENDORS, "ULA-Flags",
                                &s6a_fd_cnf.dataobj_s6a_ula_flags, ENOENT));
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                                AVP_BY_NAME_ALL_VENDORS, "Subscription-Data",
                                &s6a_fd_cnf.dataobj_s6a_subscription_data, ENOENT));
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                                AVP_BY_NAME_ALL_VENDORS, "Requested-EUTRAN-Authentication-Info",
                                &s6a_fd_cnf.dataobj_s6a_req_eutran_auth_info, ENOENT));
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                                AVP_BY_NAME_ALL_VENDORS, "Number-Of-Requested-Vectors",
                                &s6a_fd_cnf.dataobj_s6a_number_of_requested_vectors, ENOENT));
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                                AVP_BY_NAME_ALL_VENDORS, "Immediate-Response-Preferred",
                                &s6a_fd_cnf.dataobj_s6a_immediate_response_pref, ENOENT));
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                                AVP_BY_NAME_ALL_VENDORS, "Authentication-Info",
                                &s6a_fd_cnf.dataobj_s6a_authentication_info, ENOENT));
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                                AVP_BY_NAME_ALL_VENDORS, "Re-Synchronization-Info",
                                &s6a_fd_cnf.dataobj_s6a_re_synchronization_info, ENOENT));
    CHECK_FD_FCT(fd_dict_search(fd_g_config->cnf_dict, DICT_AVP,
                                AVP_BY_NAME_ALL_VENDORS, "UE-SRVCC-Capability",
                                &s6a_fd_cnf.dataobj_s6a_ue_srvcc_cap, ENOENT));

    /* Register callbacks */
    memset(&when, 0, sizeof(when));

    when.command = s6a_fd_cnf.dataobj_s6a_ula;
    when.app     = s6a_fd_cnf.dataobj_s6a_app;

    /* Register the callback for Update Location Answer S6A Application */
    CHECK_FD_FCT(fd_disp_register(s6a_ula_cb, DISP_HOW_CC, &when, NULL,
                                  &s6a_fd_cnf.ula_hdl));
    DevAssert(s6a_fd_cnf.ula_hdl != NULL);

    when.command = s6a_fd_cnf.dataobj_s6a_aia;
    when.app     = s6a_fd_cnf.dataobj_s6a_app;

    /* Register the callback for Authentication Information Answer S6A Application */
    CHECK_FD_FCT(fd_disp_register(s6a_aia_cb, DISP_HOW_CC, &when, NULL,
                                  &s6a_fd_cnf.aia_hdl));
    DevAssert(s6a_fd_cnf.aia_hdl != NULL);

    /* Advertise the support for the test application in the peer */
    CHECK_FD_FCT(fd_disp_app_support(s6a_fd_cnf.dataobj_s6a_app,
                                     s6a_fd_cnf.dataobj_s6a_vendor, 1, 0));

    return 0;
}

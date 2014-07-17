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
#ifndef S1AP_MESSAGES_TYPES_H_
#define S1AP_MESSAGES_TYPES_H_

#define S1AP_ENB_DEREGISTERED_IND(mSGpTR) (mSGpTR)->ittiMsg.s1ap_eNB_deregistered_ind
#define S1AP_DEREGISTER_UE_REQ(mSGpTR)    (mSGpTR)->ittiMsg.s1ap_deregister_ue_req

typedef struct s1ap_initial_ue_message_s {
    unsigned     eNB_ue_s1ap_id:24;
    uint32_t     mme_ue_s1ap_id;
    cgi_t        e_utran_cgi;
} s1ap_initial_ue_message_t;

typedef struct s1ap_initial_ctxt_setup_req_s {
    unsigned                eNB_ue_s1ap_id:24;
    uint32_t                mme_ue_s1ap_id;

    /* Key eNB */
    uint8_t                 keNB[32];

    ambr_t                  ambr;
    ambr_t                  apn_ambr;

    /* EPS bearer ID */
    unsigned                ebi:4;

    /* QoS */
    qci_t                   qci;
    priority_level_t        prio_level;
    pre_emp_vulnerability_t pre_emp_vulnerability;
    pre_emp_capability_t    pre_emp_capability;

    /* S-GW TEID for user-plane */
    Teid_t                  teid;
    /* S-GW IP address for User-Plane */
    ip_address_t            s_gw_address;
} s1ap_initial_ctxt_setup_req_t;

typedef struct s1ap_ue_cap_ind_s {
    unsigned  eNB_ue_s1ap_id:24;
    uint32_t  mme_ue_s1ap_id;
    uint8_t   radio_capabilities[100];
    uint32_t  radio_capabilities_length;
} s1ap_ue_cap_ind_t;

#define S1AP_ITTI_UE_PER_DEREGISTER_MESSAGE 20
typedef struct s1ap_eNB_deregistered_ind_s {
    uint8_t  nb_ue_to_deregister;
    uint32_t mme_ue_s1ap_id[S1AP_ITTI_UE_PER_DEREGISTER_MESSAGE];
} s1ap_eNB_deregistered_ind_t;

typedef struct s1ap_deregister_ue_req_s {
    uint32_t mme_ue_s1ap_id;
} s1ap_deregister_ue_req_t;

#endif /* S1AP_MESSAGES_TYPES_H_ */

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

/*! \file mme_app_ue_context.h
 *  \brief MME applicative layer
 *  \author Sebastien ROUX
 *  \date 2013
 *  \version 1.0
 *  @defgroup _mme_app_impl_ MME applicative layer
 *  @ingroup _ref_implementation_
 *  @{
 */

#include <stdint.h>
#include <inttypes.h>   /* For sscanf formats */
#include <time.h>       /* to provide time_t */

#include "security_types.h"
#include "tree.h"

#ifndef MME_APP_UE_CONTEXT_H_
#define MME_APP_UE_CONTEXT_H_

typedef enum {
    ECM_IDLE,
    ECM_CONNECTED,
    ECM_DEREGISTERED,
} mm_state_t;

typedef uint64_t mme_app_imsi_t;

#define IMSI_FORMAT SCNu64

/* Convert the IMSI contained by a char string NULL terminated to uint64_t */
#define MME_APP_STRING_TO_IMSI(sTRING, iMSI) sscanf(sTRING, "%"IMSI_FORMAT, iMSI)
#define MME_APP_IMSI_TO_STRING(iMSI, sTRING) sprintf(sTRING, "%"IMSI_FORMAT, iMSI)

/** @struct bearer_context_t
 *  @brief Parameters that should be kept for an eps bearer.
 */
typedef struct bearer_context_s {
    /* S-GW Tunnel Endpoint for User-Plane */
    uint32_t     s_gw_teid;

    /* S-GW IP address for User-Plane */
    ip_address_t s_gw_address;

    /* P-GW Tunnel Endpoint for User-Plane */
    uint32_t     p_gw_teid;

    /* P-GW IP address for User-Plane */
    ip_address_t p_gw_address;

    /* QoS for this bearer */
    qci_t                   qci;
    priority_level_t        prio_level;
    pre_emp_vulnerability_t pre_emp_vulnerability;
    pre_emp_capability_t    pre_emp_capability;

    /* TODO: add TFT */
} bearer_context_t;

/** @struct ue_context_t
 *  @brief Useful parameters to know in MME application layer. They are set
 * according to 3GPP TS.23.401 #5.7.2
 */
typedef struct ue_context_s {
    /* Tree entry */
    RB_ENTRY(ue_context_s) rb_entry;

    /* Basic identifier for ue. IMSI is encoded on maximum of 15 digits of 4 bits,
     * so usage of an unsigned integer on 64 bits is necessary.
     */
    mme_app_imsi_t imsi;
#define IMSI_UNAUTHENTICATED  (0x0)
#define IMSI_AUTHENTICATED    (0x1)
    /* Indicator to show the IMSI authentication state */
    unsigned imsi_auth:1;

    unsigned eNB_ue_s1ap_id:24;
    uint32_t mme_ue_s1ap_id;

    /* ue_id is equal to mme_ue_s1ap_id */
    uint32_t ue_id;

    uint8_t nb_of_vectors;

    /* List of authentication vectors for E-UTRAN */
    eutran_vector_t  *vector_list;
    eutran_vector_t  *vector_in_use;

#define SUBSCRIPTION_UNKNOWN    0x0
#define SUBSCRIPTION_KNOWN      0x1
    unsigned subscription_known:1;

    uint8_t msisdn[MSISDN_LENGTH];
    mm_state_t mm_state;
    /* Globally Unique Temporary Identity */
    GUTI_t        guti;
    me_identity_t me_identity;

    /* TODO: Add TAI list */

    /* Last known cell identity */
    cgi_t e_utran_cgi;
    /* Time when the cell identity was acquired */
    time_t cell_age;

    /* TODO: add csg_id */
    /* TODO: add csg_membership */

    network_access_mode_t access_mode;

    /* TODO: add ue radio cap, ms classmarks, cupported codecs */

    /* TODO: add ue network capability, ms network capability */
    /* TODO: add selected NAS algorithm */

    /* TODO: add DRX parameter */

    apn_config_profile_t apn_profile;

    ard_t access_restriction_data;

    subscriber_status_t sub_status;

    ambr_t subscribed_ambr;
    ambr_t used_ambr;

    rau_tau_timer_t rau_tau_timer;

    /* Store the radio capabilities as received in S1AP UE capability indication
     * message.
     */
    char *ue_radio_capabilities;
    int   ue_radio_cap_length;

    uint32_t mme_s11_teid;
    uint32_t sgw_s11_teid;

    bearer_context_t eps_bearers[BEARERS_PER_UE];
} ue_context_t;

typedef struct {
    uint32_t nb_ue_managed;
    uint32_t nb_ue_idle;

    uint32_t nb_bearers_managed;

    uint32_t nb_ue_since_last_stat;
    uint32_t nb_bearers_since_last_stat;

    /* Entry to the root */
    RB_HEAD(ue_context_map, ue_context_s) ue_context_tree;
} mme_ue_context_t;

/** \brief Retrieve an UE context by selecting the provided IMSI
 * \param imsi Imsi to find in UE map
 * @returns an UE context matching the IMSI or NULL if the context doesn't exists
 **/
inline
ue_context_t *mme_ue_context_exists_imsi(mme_ue_context_t *mme_ue_context,
                                         mme_app_imsi_t imsi);

/** \brief Retrieve an UE context by selecting the provided S11 teid
 * \param teid The tunnel endpoint identifier used between MME and S-GW
 * @returns an UE context matching the teid or NULL if the context doesn't exists
 **/
inline
ue_context_t *mme_ue_context_exists_s11_teid(mme_ue_context_t *mme_ue_context,
                                             uint32_t teid);

/** \brief Retrieve an UE context by selecting the provided guti
 * \param guti The GUTI used by the UE
 * @returns an UE context matching the guti or NULL if the context doesn't exists
 **/
inline
struct ue_context_s *mme_ue_context_exists_guti(mme_ue_context_t *mme_ue_context,
                                                GUTI_t guti);

/** \brief Insert a new UE context in the tree of known UEs.
 * At least the IMSI should be known to insert the context in the tree.
 * \param ue_context_p The UE context to insert
 * @returns 0 in case of success, -1 otherwise
 **/
int mme_insert_ue_context(mme_ue_context_t *mme_ue_context,
                          struct ue_context_s *ue_context_p);

/** \brief Allocate memory for a new UE context
 * @returns Pointer to the new structure, NULL if allocation failed
 **/
ue_context_t *mme_create_new_ue_context(void);

/** \brief Dump the UE contexts present in the tree
 **/
void mme_app_dump_ue_contexts(mme_ue_context_t *mme_ue_context);

#endif /* MME_APP_UE_CONTEXT_H_ */

/* @} */

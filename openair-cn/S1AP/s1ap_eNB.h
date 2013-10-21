/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2012 Eurecom

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

#include "tree.h"
#include "queue.h"

#include "mme_sim.h"
#include "s1ap_eNB_defs.h"

#ifndef S1AP_ENB_H_
#define S1AP_ENB_H_

/* Served PLMN identity element */
struct plmn_identity_s {
    uint16_t mcc;
    uint16_t mnc;
    STAILQ_ENTRY(plmn_identity_s) next;
};

/* Served group id element */
struct served_group_id_s {
    uint16_t mme_group_id;
    STAILQ_ENTRY(served_group_id_s) next;
};

/* Served mme code for a particular MME */
struct mme_code_s {
    uint8_t mme_code;
    STAILQ_ENTRY(mme_code_s) next;
};

/* Served gummei element */
struct served_gummei_s {
    /* Number of MME served PLMNs */
    uint8_t nb_served_plmns;
    /* List of served PLMNs by MME */
    STAILQ_HEAD(served_plmns_s, plmn_identity_s) served_plmns;

    /* Number of group id in list */
    uint8_t nb_group_id;
    /* Served group id list */
    STAILQ_HEAD(served_group_ids_s, served_group_id_s) served_group_ids;

    /* Number of MME code */
    uint8_t nb_mme_code;
    /* MME Code to uniquely identify an MME within an MME pool area */
    STAILQ_HEAD(mme_codes_s, mme_code_s) mme_codes;

    /* Next GUMMEI element */
    STAILQ_ENTRY(served_gummei_s) next;
};

/* This structure describes association of a eNB to a MME */
typedef struct s1ap_eNB_mme_data_s {
    /* This is the optional name provided by the MME */
    char *mme_name;

    /* Remote MME IP addr */
    char *ip_addr;

    /* List of served GUMMEI per MME. There is one GUMMEI per RAT with a max
     * number of 8 RATs but in our case only one is used. The LTE related pool
     * configuration is included on the first place in the list.
     */
    STAILQ_HEAD(served_gummeis_s, served_gummei_s) served_gummei;

    /* Relative processing capacity of an MME with respect to the other MMEs
     * in the pool in order to load-balance MMEs within a pool as defined
     * in TS 23.401.
     */
    uint8_t relative_mme_capacity;

    /* Current MME overload information (if any). */
    s1ap_overload_state_t overload_state;
    /* Current eNB->MME S1AP association state */
    s1ap_eNB_state_t state;

    /* SCTP related data for this MME */
    sctp_data_t sctp_data;
    /* Next usable stream for UE signalling */
    int32_t nextstream;

    /* MME descriptors tree, ordered by sctp assoc id */
    RB_ENTRY(s1ap_eNB_mme_data_s) entry;
} s1ap_eNB_mme_data_t;

inline int s1ap_eNB_compare_assoc_id(
    struct s1ap_eNB_mme_data_s *p1, struct s1ap_eNB_mme_data_s *p2);

/* Generate the tree management functions */
RB_PROTOTYPE(s1ap_mme_map, s1ap_eNB_mme_data_s, entry,
             s1ap_eNB_compare_assoc_id);

struct s1ap_eNB_mme_data_s *s1ap_eNB_get_MME(eNB_mme_desc_t *eNB_desc_p,
        uint32_t assocId);

int s1ap_eNB_init(eNB_mme_desc_t *eNB_desc_p,
                  char *local_ip_addr[],  int nb_local_ip,
                  char *remote_ip_addr[], int nb_remote_ip);

#endif /* S1AP_ENB_H_ */

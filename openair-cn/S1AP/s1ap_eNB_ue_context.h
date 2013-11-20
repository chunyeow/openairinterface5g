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

#include "tree.h"
#include "queue.h"

#include "s1ap_eNB_defs.h"

#ifndef S1AP_ENB_UE_CONTEXT_H_
#define S1AP_ENB_UE_CONTEXT_H_

// Forward declarations
struct s1ap_eNB_mme_data_s;
struct s1ap_ue_map;
struct eNB_mme_desc_s;

typedef enum {
    /* UE has not been registered to a MME or UE association has failed. */
    S1AP_UE_DECONNECTED = 0x0,
    /* UE s1ap state is waiting for initial context setup request message. */
    S1AP_UE_WAITING_CSR = 0x1,
    /* UE association is ready and bearers are established. */
    S1AP_UE_CONNECTED   = 0x2,
    S1AP_UE_STATE_MAX,
} s1ap_ue_state;

typedef struct s1ap_eNB_ue_context_s {
    /* Tree related data */
    RB_ENTRY(s1ap_eNB_ue_context_s) entries;

    /* Uniquely identifies the UE between MME and eNB within the eNB.
     * This id is encoded on 24bits.
     */
    unsigned eNB_ue_s1ap_id:24;

    /* UE id for initial connection to S1AP */
    uint16_t ue_initial_id;

    /* Uniquely identifies the UE within MME. Encoded on 32 bits. */
    uint32_t mme_ue_s1ap_id;

    /* Stream used for this particular UE */
    int32_t stream;

    /* Current UE state. */
    s1ap_ue_state ue_state;

    /* Reference to MME data this UE is attached to */
    struct s1ap_eNB_mme_data_s *mme_ref;

    /* Reference to eNB data this UE is attached to */
    s1ap_eNB_instance_t *eNB_instance;
} s1ap_eNB_ue_context_t;

inline int s1ap_eNB_compare_eNB_ue_s1ap_id(
    struct s1ap_eNB_ue_context_s *p1, struct s1ap_eNB_ue_context_s *p2);

/* Generate the tree management functions prototypes */
RB_PROTOTYPE(s1ap_ue_map, s1ap_eNB_ue_context_s, entries,
             s1ap_eNB_compare_eNB_ue_s1ap_id);

struct s1ap_eNB_ue_context_s *s1ap_eNB_allocate_new_UE_context(void);

struct s1ap_eNB_ue_context_s *s1ap_eNB_get_ue_context(
    s1ap_eNB_instance_t *instance_p,
    uint32_t eNB_ue_s1ap_id);

#endif /* S1AP_ENB_UE_CONTEXT_H_ */

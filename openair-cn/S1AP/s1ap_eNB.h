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

/** @defgroup _s1ap_impl_ S1AP Layer Reference Implementation for eNB
 * @ingroup _ref_implementation_
 * @{
 */

#ifndef S1AP_ENB_H_
#define S1AP_ENB_H_

typedef struct s1ap_eNB_config_s {
    // MME related params
    unsigned char mme_enabled;          ///< MME enabled ?
} s1ap_eNB_config_t;

#if defined(OAI_EMU)
# define EPC_MODE_ENABLED       oai_emulation.info.s1ap_config.mme_enabled
#else
extern s1ap_eNB_config_t s1ap_config;

# define EPC_MODE_ENABLED       s1ap_config.mme_enabled
#endif

void *s1ap_eNB_task(void *arg);

uint32_t s1ap_generate_eNB_id(void);

#endif /* S1AP_ENB_H_ */

/**
 * @}
 */

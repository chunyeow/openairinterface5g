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

#include <stdint.h>
#include <sched.h>
#include "mme_default_values.h"

#ifndef MME_CONFIG_H_
#define MME_CONFIG_H_

typedef struct mme_config_s {
    uint8_t verbosity_level;
    struct {
        int root_timer_value_ms;
    } timer_config;
    struct {
        uint16_t port_number;
    } gtpv1_u_config;
    struct {
        uint16_t port_number;
    } s1ap_config;
    struct {
        /* Defines the scheduling policy for each task thread.
         * The value should be one of these macros:
         * - SCHED_NORMAL
         * - SCHED_FIFO
         * - SCHED_RR
         * - SCHED_BATCH
         */
        uint8_t sched_policy;
    } itti_config;
} mme_config_t;

int config_parse_opt_line(int argc, char *argv[], mme_config_t *mme_config);

#endif /* MME_CONFIG_H_ */

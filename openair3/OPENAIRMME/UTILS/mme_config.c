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

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "mme_config.h"

static void config_init(mme_config_t *mme_config) {
    mme_config->verbosity_level = 0;
    mme_config->timer_config.root_timer_value_ms = MME_TIMER_EXPIRY_MS;
    mme_config->gtpv1_u_config.port_number = GTPV1_U_PORT_NUMBER;
    mme_config->s1ap_config.port_number = S1AP_PORT_NUMBER;
    mme_config->itti_config.sched_policy = ITTI_TASK_SCHED_POLICY;
}

static void usage(void) {
    fprintf(stdout, "==== EURECOM %s v%s ====\n", PACKAGE_NAME, PACKAGE_VERSION);
    fprintf(stdout, "Please report any bug to: %s\n\n", PACKAGE_BUGREPORT);
    fprintf(stdout, "Usage: oaisim_mme [options]\n\n");
    fprintf(stdout, "Available options:\n");
    fprintf(stdout, "-h      Print this help and return\n");
    fprintf(stdout, "-V      Print %s version and return\n", PACKAGE_NAME);
    fprintf(stdout, "-v[1-2] Debug level:\n");
    fprintf(stdout, "            1 -> ASN1 XER printf on and ASN1 debug off\n");
    fprintf(stdout, "            2 -> ASN1 XER printf on and ASN1 debug on\n");
}

int config_parse_opt_line(int argc, char *argv[], mme_config_t *mme_config) {
    int c;
    config_init(mme_config);
    /* Parsing command line */
    while ((c = getopt (argc, argv, "hv:V")) != -1) {
        switch (c) {
            case 'v':
                mme_config->verbosity_level = atoi(optarg);
                break;
            case 'V':
                fprintf(stdout, "==== EURECOM %s v%s ====\n"
                "Please report any bug to: %s\n", PACKAGE_NAME, PACKAGE_VERSION, PACKAGE_BUGREPORT);
                exit(0);
                break;
            case 'h': /* Fall through */
            default:
                usage();
                exit(0);
        }
    }
    return 0;
}

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

/*! \file s6a_peers.c
 * \brief Authenticate a new peer connecting to the HSS by checking the database
 * \author Sebastien ROUX <sebastien.roux@eurecom.fr>
 * \date 2013
 * \version 0.1
 */

#include <stdio.h>
#include <string.h>

#include "hss_config.h"
#include "db_proto.h"
#include "s6a_proto.h"

int s6a_peer_validate(struct peer_info *info, int *auth, int (**cb2)(struct peer_info *))
{
    mysql_mme_identity_t mme_identity;

    if (info == NULL) {
        return EINVAL;
    }

    memset(&mme_identity, 0, sizeof(mysql_mme_identity_t));

    /* We received a new connection. Check the database for allowed equipments
     * on EPC
     */

    memcpy(mme_identity.mme_host, info->pi_diamid, info->pi_diamidlen);

    if (hss_mysql_check_epc_equipment(&mme_identity) != 0) {
        /* The MME has not been found in list of known peers -> reject it */
        *auth = -1;
        fprintf(stdout, "Rejecting %s: either db has no knowledge of this peer "
        "or sql query failed\n", info->pi_diamid);
    } else {
        *auth = 1;

        /* For now we don't use security */
        info->config.pic_flags.sec = PI_SEC_NONE;
        info->config.pic_flags.persist = PI_PRST_NONE;
        fprintf(stdout, "Accepting %s peer\n", info->pi_diamid);
    }

    return 0;
}

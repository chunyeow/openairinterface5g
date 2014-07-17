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

#include <stdlib.h>
#include <stdio.h>

#include "intertask_interface.h"

#include "mme_app_ue_context.h"
#include "mme_app_defs.h"
#include "mme_app_statistics.h"

int
mme_app_statistics_display(void)
{
    fprintf(stdout, "================== Statistics ==================\n");
    fprintf(stdout, "        |   Global   | Since last display |\n");
    fprintf(stdout, "UE      | %10u |     %10u     |\n",
            mme_app_desc.mme_ue_contexts.nb_ue_managed,
            mme_app_desc.mme_ue_contexts.nb_ue_since_last_stat);
    fprintf(stdout, "Bearers | %10u |     %10u     |\n",
            mme_app_desc.mme_ue_contexts.nb_bearers_managed,
            mme_app_desc.mme_ue_contexts.nb_bearers_since_last_stat);

    mme_app_desc.mme_ue_contexts.nb_ue_since_last_stat = 0;
    mme_app_desc.mme_ue_contexts.nb_bearers_since_last_stat = 0;

    return 0;
}

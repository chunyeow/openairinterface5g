/*******************************************************************************
Eurecom OpenAirInterface Core Network
Copyright(c) 1999 - 2014 Eurecom

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
Forums       : http://forums.eurecom.fsr/openairinterface
Address      : EURECOM,
               Campus SophiaTech,
               450 Route des Chappes,
               CS 50193
               06904 Biot Sophia Antipolis cedex,
               FRANCE
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

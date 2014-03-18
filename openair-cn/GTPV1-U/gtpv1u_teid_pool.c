/*******************************************************************************
Eurecom OpenAirInterface core network
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
/*! \file gtpv1u_task.c
* \brief
* \author Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/
#include <stdlib.h>
#include <stdint.h>

#include "gtpv1u.h"

//#define GTPV1U_LINEAR_TEID_ALLOCATION 1

#ifdef GTPV1U_LINEAR_TEID_ALLOCATION
static uint32_t g_gtpv1u_teid = 0;
#endif

uint32_t gtpv1u_new_teid(void)
{
#ifdef GTPV1U_LINEAR_TEID_ALLOCATION
    g_gtpv1u_teid = g_gtpv1u_teid + 1;
    return g_gtpv1u_teid;
#else
    return random() + random() % (RAND_MAX - 1) + 1;
#endif
}

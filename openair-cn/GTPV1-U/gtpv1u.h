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
/*! \file gtpv1u.h
* \brief
* \author Sebastien ROUX, Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/

#ifndef GTPV1_U_H_
#define GTPV1_U_H_

/* When gtpv1u is compiled for eNB use MACRO from UTILS/log.h,
 * otherwise use standard fprintf as logger.
 */
#if defined(ENB_MODE)
# define GTPU_DEBUG(x, args...)   LOG_I(GTPU, x, ##args)
# define GTPU_INFO(x, args...)    LOG_I(GTPU, x, ##args)
# define GTPU_WARNING(x, args...) LOG_W(GTPU, x, ##args)
# define GTPU_ERROR(x, args...)   LOG_E(GTPU, x, ##args)
#else
# define GTPU_DEBUG(x, args...)   fprintf(stdout, "[GTPU][D]"x, ##args)
# define GTPU_INFO(x, args...)    fprintf(stdout, "[GTPU][I]"x, ##args)
# define GTPU_WARNING(x, args...) fprintf(stdout, "[GTPU][W]"x, ##args)
# define GTPU_ERROR(x, args...)   fprintf(stderr, "[GTPU][E]"x, ##args)
#endif

uint32_t gtpv1u_new_teid(void);

#endif /* GTPV1_U_H_ */

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
/*! \file pgw_lite_paa.h
* \brief
* \author Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/
#ifndef PGW_LITE_PAA_H_
#define PGW_LITE_PAA_H_

void pgw_lite_load_pool_ip_addresses       (void);
int pgw_lite_get_free_ipv4_paa_address     (struct in_addr * const addr_P);
int pgw_lite_release_free_ipv4_paa_address (const struct in_addr * const addr_P);
int pgw_lite_get_free_ipv6_paa_prefix      (struct in6_addr * const addr_pP);
int pgw_lite_release_free_ipv6_paa_prefix  (const struct in6_addr * const addr_pP);

#endif

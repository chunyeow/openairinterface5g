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


#include <stdio.h>
#include <stdint.h>

/** @defgroup _x2ap_impl_ X2AP Layer Reference Implementation
 * @ingroup _ref_implementation_
 * @{
 */

#ifndef X2AP_H_
#define X2AP_H_

typedef struct x2ap_config_s {
} x2ap_config_t;

#if defined(OAI_EMU)
#else
extern x2ap_config_t x2ap_config;
#endif

void *x2ap_task(void *arg);

#endif /* X2AP_H_ */

/**
 * @}
 */

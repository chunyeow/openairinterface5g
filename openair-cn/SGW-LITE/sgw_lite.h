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

#ifndef SGW_LITE_H_
#define SGW_LITE_H_

#include "hashtable.h"
#include "tree.h"
#include "commonDef.h"
#include "common_types.h"
#include "sgw_lite_context_manager.h"

typedef struct sgw_app_s{
	
    char     *sgw_interface_name_for_S1u_S12_S4_up;
    uint32_t  sgw_ip_address_for_S1u_S12_S4_up;

    char     *sgw_interface_name_for_S11_S4; // unused now
    uint32_t  sgw_ip_address_for_S11_S4;    // unused now

    uint32_t  sgw_ip_address_for_S5_S8_up; // unused now

    // key is S11 S-GW local teid
	hash_table_t *s11teid2mme_hashtable;

	// key is S1-U S-GW local teid
	hash_table_t *s1uteid2enb_hashtable;

	// the key of this hashtable is the S11 s-gw local teid.
	hash_table_t *s11_bearer_context_information_hashtable;
	

} sgw_app_t;

#endif


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
/*! \file sgw_lite.h
* \brief
* \author Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/

#ifndef SGW_LITE_H_
#define SGW_LITE_H_
#include <netinet/in.h>
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


struct pgw_lite_ipv4_list_elm_s {
    STAILQ_ENTRY(pgw_lite_ipv4_list_elm_s) ipv4_entries;
    struct in_addr  addr;
};


struct pgw_lite_ipv6_list_elm_s {
    STAILQ_ENTRY(pgw_lite_ipv6_list_elm_s) ipv6_entries;
    struct in6_addr addr;
    int             prefix_len;
    int             num_allocated;
    int             num_free;
} ;


typedef struct pgw_app_s{

    STAILQ_HEAD(pgw_lite_ipv4_list_free_head_s,      pgw_lite_ipv4_list_elm_s) pgw_lite_ipv4_list_free;
    STAILQ_HEAD(pgw_lite_ipv4_list_allocated_head_s, pgw_lite_ipv4_list_elm_s) pgw_lite_ipv4_list_allocated;

    STAILQ_HEAD(pgw_lite_ipv6_list_free_head_s,      pgw_lite_ipv6_list_elm_s) pgw_lite_ipv6_list_free;
    STAILQ_HEAD(pgw_lite_ipv6_list_allocated_head_s, pgw_lite_ipv6_list_elm_s) pgw_lite_ipv6_list_allocated;
} pgw_app_t;

#endif


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
/*! \file pgw_lite_paa.c
* \brief
* \author Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "intertask_interface.h"
#include "assertions.h"
#include "queue.h"

#include "pgw_lite_paa.h"
#include "sgw_lite_defs.h"
#include "spgw_config.h"
#include "sgw_lite.h"


//#define PGW_LITE_FREE_ADDR_POOL_CONFIG 1

extern pgw_app_t     pgw_app;


// Load in PGW pool, configured PAA address pool
void pgw_lite_load_pool_ip_addresses(void) {
    struct pgw_lite_conf_ipv4_list_elm_s *conf_ipv4_p = NULL;
    struct pgw_lite_ipv4_list_elm_s      *ipv4_p      = NULL;

    struct pgw_lite_conf_ipv6_list_elm_s *conf_ipv6_p = NULL;
    struct pgw_lite_ipv6_list_elm_s      *ipv6_p      = NULL;
    char                                  print_buffer[INET6_ADDRSTRLEN];

    STAILQ_INIT(&pgw_app.pgw_lite_ipv4_list_free);
    STAILQ_INIT(&pgw_app.pgw_lite_ipv4_list_allocated);
    STAILQ_INIT(&pgw_app.pgw_lite_ipv6_list_free);
    STAILQ_INIT(&pgw_app.pgw_lite_ipv6_list_allocated);

    STAILQ_FOREACH(conf_ipv4_p, &spgw_config.pgw_config.pgw_lite_ipv4_pool_list, ipv4_entries) {
        ipv4_p = calloc(1, sizeof(struct pgw_lite_ipv4_list_elm_s));
        ipv4_p->addr.s_addr = ntohl(conf_ipv4_p->addr.s_addr);
        STAILQ_INSERT_TAIL(&pgw_app.pgw_lite_ipv4_list_free, ipv4_p, ipv4_entries);
        //SPGW_APP_DEBUG("Loaded IPv4 PAA address in pool: %s\n",
        //        inet_ntoa(conf_ipv4_p->addr));
    }
    STAILQ_FOREACH(conf_ipv6_p, &spgw_config.pgw_config.pgw_lite_ipv6_pool_list, ipv6_entries) {
        ipv6_p = calloc(1, sizeof(struct pgw_lite_ipv6_list_elm_s));
        ipv6_p->addr          = conf_ipv6_p->addr;
        ipv6_p->prefix_len    = conf_ipv6_p->prefix_len;
        ipv6_p->num_allocated = 0;
        STAILQ_INSERT_TAIL(&pgw_app.pgw_lite_ipv6_list_free, ipv6_p, ipv6_entries);
        if (inet_ntop(AF_INET6, &ipv6_p->addr, print_buffer, INET6_ADDRSTRLEN) == NULL) {
            SPGW_APP_ERROR("Could not Load IPv6 PAA address in pool: %s\n", strerror(errno));
        } /*else {
            SPGW_APP_DEBUG("Loaded IPv6 PAA prefix in pool: %s\n",print_buffer);
        }*/
    }
#if defined(PGW_LITE_FREE_ADDR_POOL_CONFIG)

    while ((conf_ipv4_p = STAILQ_FIRST(&spgw_config.pgw_config.pgw_lite_ipv4_pool_list))) {
        STAILQ_REMOVE_HEAD(&spgw_config.pgw_config.pgw_lite_ipv4_pool_list, ipv4_entries);
        free(conf_ipv4_p);
    }
    while ((conf_ipv6_p = STAILQ_FIRST(&spgw_config.pgw_config.pgw_lite_ipv6_pool_list))) {
        STAILQ_REMOVE_HEAD(&spgw_config.pgw_config.pgw_lite_ipv6_pool_list, ipv6_entries);
        free(conf_ipv6_p);
    }
#endif

}



int pgw_lite_get_free_ipv4_paa_address(struct in_addr * const addr_pP) {
    struct pgw_lite_ipv4_list_elm_s  *ipv4_p = NULL;

    if (STAILQ_EMPTY(&pgw_app.pgw_lite_ipv4_list_free)) {
        addr_pP->s_addr = INADDR_ANY;
        return -1;
    }
    ipv4_p = STAILQ_FIRST(&pgw_app.pgw_lite_ipv4_list_free);
    STAILQ_REMOVE(&pgw_app.pgw_lite_ipv4_list_free, ipv4_p, pgw_lite_ipv4_list_elm_s, ipv4_entries);
    STAILQ_INSERT_TAIL(&pgw_app.pgw_lite_ipv4_list_allocated, ipv4_p, ipv4_entries);
    addr_pP->s_addr = ipv4_p->addr.s_addr;
    return 0;
}

int pgw_lite_release_free_ipv4_paa_address(const struct in_addr * const addr_pP) {
    struct pgw_lite_ipv4_list_elm_s *ipv4_p = NULL;

    STAILQ_FOREACH(ipv4_p, &pgw_app.pgw_lite_ipv4_list_allocated, ipv4_entries) {
        if (ipv4_p->addr.s_addr ==  addr_pP->s_addr) {
            STAILQ_REMOVE(&pgw_app.pgw_lite_ipv4_list_allocated, ipv4_p, pgw_lite_ipv4_list_elm_s, ipv4_entries);
            STAILQ_INSERT_TAIL(&pgw_app.pgw_lite_ipv4_list_free, ipv4_p, ipv4_entries);
            return 0;
        }
    }
    return -1;
}

int pgw_lite_get_free_ipv6_paa_prefix(struct in6_addr * const addr_pP) {
    struct pgw_lite_ipv6_list_elm_s  *ipv6_p = NULL;

    if (STAILQ_EMPTY(&pgw_app.pgw_lite_ipv6_list_free)) {
        *addr_pP = in6addr_any;
        return -1;
    }
    ipv6_p = STAILQ_FIRST(&pgw_app.pgw_lite_ipv6_list_free);
    ipv6_p->num_allocated += 1;
    ipv6_p->num_free      -= 1;
    if (ipv6_p->num_free == 0) {
        STAILQ_REMOVE(&pgw_app.pgw_lite_ipv6_list_free, ipv6_p, pgw_lite_ipv6_list_elm_s, ipv6_entries);
        STAILQ_INSERT_TAIL(&pgw_app.pgw_lite_ipv6_list_allocated, ipv6_p, ipv6_entries);
    }
    *addr_pP = ipv6_p->addr;
    return 0;
}

int pgw_lite_release_free_ipv6_paa_prefix(const struct in6_addr * const addr_pP) {
    struct pgw_lite_ipv6_list_elm_s *ipv6_p = NULL;

    ipv6_p = STAILQ_FIRST(&pgw_app.pgw_lite_ipv6_list_free);
    if (IN6_ARE_ADDR_EQUAL(&ipv6_p->addr, addr_pP)) {
        ipv6_p->num_allocated -= 1;
        ipv6_p->num_free      += 1;
    }

    STAILQ_FOREACH(ipv6_p, &pgw_app.pgw_lite_ipv6_list_allocated, ipv6_entries) {
        if (IN6_ARE_ADDR_EQUAL(&ipv6_p->addr, addr_pP)) {
            STAILQ_REMOVE(&pgw_app.pgw_lite_ipv6_list_allocated, ipv6_p, pgw_lite_ipv6_list_elm_s, ipv6_entries);
            ipv6_p->num_allocated -= 1;
            ipv6_p->num_free      += 1;
            STAILQ_INSERT_TAIL(&pgw_app.pgw_lite_ipv6_list_free, ipv6_p, ipv6_entries);
            return 0;
        }
    }
    return -1;
}





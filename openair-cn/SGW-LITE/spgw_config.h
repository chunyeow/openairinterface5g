/*******************************************************************************

  Eurecom OpenAirInterface
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
  Forums       : http://forums.eurecom.fr/openairinterface
  Address      : EURECOM, Campus SophiaTech, 450 Route des Chappes
                 06410 Biot FRANCE

*******************************************************************************/

#ifndef SPGW_CONFIG_H_
#define SPGW_CONFIG_H_
#include <sys/socket.h> // inet_aton
#include <netinet/in.h> // inet_aton
#include <arpa/inet.h>  // inet_aton



#define SGW_CONFIG_STRING_SGW_CONFIG                            "S-GW"
#define SGW_CONFIG_STRING_NETWORK_INTERFACES_CONFIG             "NETWORK_INTERFACES"
#define SGW_CONFIG_STRING_SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP  "SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP"
#define SGW_CONFIG_STRING_SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP    "SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP"
#define SGW_CONFIG_STRING_SGW_INTERFACE_NAME_FOR_S5_S8_UP       "SGW_INTERFACE_NAME_FOR_S5_S8_UP"
#define SGW_CONFIG_STRING_SGW_IPV4_ADDRESS_FOR_S5_S8_UP         "SGW_IPV4_ADDRESS_FOR_S5_S8_UP"
#define SGW_CONFIG_STRING_SGW_INTERFACE_NAME_FOR_S11            "SGW_INTERFACE_NAME_FOR_S11"
#define SGW_CONFIG_STRING_SGW_IPV4_ADDRESS_FOR_S11              "SGW_IPV4_ADDRESS_FOR_S11"

#define PGW_CONFIG_STRING_PGW_CONFIG                            "P-GW"
#define PGW_CONFIG_STRING_NETWORK_INTERFACES_CONFIG             "NETWORK_INTERFACES"
#define PGW_CONFIG_STRING_PGW_INTERFACE_NAME_FOR_S5_S8          "PGW_INTERFACE_NAME_FOR_S5_S8"
#define PGW_CONFIG_STRING_PGW_IPV4_ADDRESS_FOR_S5_S8            "PGW_IPV4_ADDRESS_FOR_S5_S8"
#define PGW_CONFIG_STRING_PGW_INTERFACE_NAME_FOR_SGI            "PGW_INTERFACE_NAME_FOR_SGI"
#define PGW_CONFIG_STRING_PGW_IPV4_ADDR_FOR_SGI                 "PGW_IPV4_ADDR_FOR_SGI"

#define PGW_CONFIG_STRING_IP_ADDRESS_POOL                       "IP_ADDRESS_POOL"
#define PGW_CONFIG_STRING_IPV4_ADDRESS_LIST                     "IPV4_LIST"
#define PGW_CONFIG_STRING_IPV6_ADDRESS_LIST                     "IPV6_LIST"
#define PGW_CONFIG_STRING_IP_ADDRESS_RANGE_DELIMITERS           " -<>"
#define PGW_CONFIG_STRING_IPV6_PREFIX_DELIMITER                 " /"

#define PGW_CONFIG_STRING_INTERFACE_DISABLED                    "none"

#define IPV4_STR_ADDR_TO_INT_NWBO(AdDr_StR,NwBo,MeSsAgE ) do {\
            struct in_addr inp;\
            if ( inet_aton(AdDr_StR, &inp ) < 0 ) {\
                AssertFatal (0, MeSsAgE);\
            } else {\
                NwBo = inp.s_addr;\
            }\
        } while (0);

typedef struct sgw_config_s {
    struct {
        char     *sgw_interface_name_for_S1u_S12_S4_up;
        uint32_t  sgw_ipv4_address_for_S1u_S12_S4_up;
        int       sgw_ip_netmask_for_S1u_S12_S4_up;

        char     *sgw_interface_name_for_S5_S8_up;
        uint32_t  sgw_ipv4_address_for_S5_S8_up;
        int       sgw_ip_netmask_for_S5_S8_up;

        char     *sgw_interface_name_for_S11;
        uint32_t  sgw_ipv4_address_for_S11;
        int       sgw_ip_netmask_for_S11;
    } ipv4;
} sgw_config_t;

// may be more
#define PGW_MAX_ALLOCATED_PDN_ADDRESSES 1024

typedef struct pgw_config_s {
    struct {
        char     *pgw_interface_name_for_S5_S8;
        uint32_t  pgw_ipv4_address_for_S5_S8;
        int       pgw_ip_netmask_for_S5_S8;

        char     *pgw_interface_name_for_SGI;
        uint32_t  pgw_ipv4_address_for_SGI;
        int       pgw_ip_netmask_for_SGI;
    } ipv4;

    struct {
        int              num_ipv4_addresses;
        struct in_addr   ipv4_addresses[PGW_MAX_ALLOCATED_PDN_ADDRESSES];
        int              num_ipv6_addresses;
        struct in6_addr  ipv6_addresses[PGW_MAX_ALLOCATED_PDN_ADDRESSES];
    } pool_pdn_addresses;
} pgw_config_t;

typedef struct spgw_config_s {
    sgw_config_t sgw_config;
    pgw_config_t pgw_config;
}spgw_config_t;

#ifndef SGW_LITE
extern spgw_config_t spgw_config;
#endif

int spgw_config_init(char* lib_config_file_name_pP, spgw_config_t* config_pP);

#endif /* ENB_CONFIG_H_ */

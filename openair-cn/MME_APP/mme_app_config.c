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

/*! \file mme_app_config.c
 * \brief
 * \author Lionel GAUTHIER
 * \version 1.0
 * \company Eurecom
 * \email: lionel.gauthier@eurecom.fr
 */
#define MME_APP
#define MME_APP_CONFIG_C

#include <string.h>
#include <libconfig.h>
#include "mme_app_config.h"

int mme_app_config_init(char* lib_config_file_name_pP, mme_app_config_t* config_pP) {

  config_t          cfg;
  config_setting_t *setting_sgw                          = NULL;
  char             *sgw_interface_name_for_S1u_S12_S4_up = NULL;
  char             *sgw_ipv4_address_for_S1u_S12_S4_up   = NULL;
  char             *sgw_interface_name_for_S5_S8_up      = NULL;
  char             *sgw_ipv4_address_for_S5_S8_up        = NULL;
  char             *sgw_interface_name_for_S11           = NULL;
  char             *sgw_ipv4_address_for_S11             = NULL;

  config_setting_t *setting_pgw                  = NULL;
  config_setting_t *subsetting                   = NULL;
  config_setting_t *sub2setting                  = NULL;
  char             *pgw_interface_name_for_S5_S8 = NULL;
  char             *pgw_ipv4_address_for_S5_S8   = NULL;
  char             *pgw_interface_name_for_SGI   = NULL;
  char             *pgw_ipv4_address_for_SGI     = NULL;

  char             *delimiters=NULL;
  char             *saveptr1= NULL;
  char             *astring = NULL;
  char             *atoken  = NULL;
  char             *atoken2 = NULL;
  char             *address = NULL;
  char             *cidr    = NULL;
  char             *mask    = NULL;
  int               num     = 0;
  int               i       = 0;
  int               jh, jn;
  unsigned char     buf_in6_addr[sizeof(struct in6_addr)];
  struct in6_addr   addr6_start;
  struct in6_addr   addr6_mask;
  int               prefix_mask;
  uint64_t          counter64;
  unsigned char     buf_in_addr[sizeof(struct in_addr)];
  struct in_addr    addr_start;
  struct in_addr    addr_end;


  memset((char*)config_pP, 0 , sizeof(mme_app_config_t));

  config_init(&cfg);

  if(lib_config_file_name_pP != NULL)
  {
      /* Read the file. If there is an error, report it and exit. */
      if(! config_read_file(&cfg, lib_config_file_name_pP))
      {
          MME_APP_ERROR("%s:%d - %s\n", lib_config_file_name_pP, config_error_line(&cfg), config_error_text(&cfg));
          config_destroy(&cfg);
          AssertFatal (1 == 0, "Failed to parse eNB configuration file %s!\n", lib_config_file_name_pP);
      }
  }
  else
  {
      SPGW_APP_ERROR("No SP-GW configuration file provided!\n");
      config_destroy(&cfg);
      AssertFatal (0, "No SP-GW configuration file provided!\n");
  }

  setting_sgw = config_lookup(&cfg, SGW_CONFIG_STRING_SGW_CONFIG);
  if(setting_sgw != NULL) {
      subsetting = config_setting_get_member (setting_sgw, SGW_CONFIG_STRING_NETWORK_INTERFACES_CONFIG);
      if(subsetting != NULL) {
          if(  (
                     config_setting_lookup_string( subsetting, SGW_CONFIG_STRING_SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP, (const char **)&sgw_interface_name_for_S1u_S12_S4_up)
                  && config_setting_lookup_string( subsetting, SGW_CONFIG_STRING_SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP,   (const char **)&sgw_ipv4_address_for_S1u_S12_S4_up)
                  && config_setting_lookup_string( subsetting, SGW_CONFIG_STRING_SGW_INTERFACE_NAME_FOR_S5_S8_UP,      (const char **)&sgw_interface_name_for_S5_S8_up)
                  && config_setting_lookup_string( subsetting, SGW_CONFIG_STRING_SGW_IPV4_ADDRESS_FOR_S5_S8_UP,        (const char **)&sgw_ipv4_address_for_S5_S8_up)
                  && config_setting_lookup_string( subsetting, SGW_CONFIG_STRING_SGW_INTERFACE_NAME_FOR_S11,           (const char **)&sgw_interface_name_for_S11)
                  && config_setting_lookup_string( subsetting, SGW_CONFIG_STRING_SGW_IPV4_ADDRESS_FOR_S11,             (const char **)&sgw_ipv4_address_for_S11)
                )
            ) {
              config_pP->sgw_config.ipv4.sgw_interface_name_for_S1u_S12_S4_up = strdup(sgw_interface_name_for_S1u_S12_S4_up);
              cidr = strdup(sgw_ipv4_address_for_S1u_S12_S4_up);
              address = strtok(cidr, "/");
              mask    = strtok(NULL, "/");
              IPV4_STR_ADDR_TO_INT_NWBO ( address, config_pP->sgw_config.ipv4.sgw_ipv4_address_for_S1u_S12_S4_up, "BAD IP ADDRESS FORMAT FOR S1u_S12_S4 !\n" )
              config_pP->sgw_config.ipv4.sgw_ip_netmask_for_S1u_S12_S4_up = atoi(mask);
              free(cidr);

              config_pP->sgw_config.ipv4.sgw_interface_name_for_S5_S8_up = strdup(sgw_interface_name_for_S5_S8_up);
              cidr = strdup(sgw_ipv4_address_for_S5_S8_up);
              address = strtok(cidr, "/");
              mask    = strtok(NULL, "/");
              IPV4_STR_ADDR_TO_INT_NWBO ( address, config_pP->sgw_config.ipv4.sgw_ipv4_address_for_S5_S8_up, "BAD IP ADDRESS FORMAT FOR S5_S8 !\n" )
              config_pP->sgw_config.ipv4.sgw_ip_netmask_for_S5_S8_up = atoi(mask);
              free(cidr);

              config_pP->sgw_config.ipv4.sgw_interface_name_for_S11 = strdup(sgw_interface_name_for_S11);
              cidr = strdup(sgw_ipv4_address_for_S11);
              address = strtok(cidr, "/");
              mask    = strtok(NULL, "/");
              IPV4_STR_ADDR_TO_INT_NWBO ( address, config_pP->sgw_config.ipv4.sgw_ipv4_address_for_S11, "BAD IP ADDRESS FORMAT FOR S11 !\n" )
              config_pP->sgw_config.ipv4.sgw_ip_netmask_for_S11 = atoi(mask);
              free(cidr);
          }
      }
  }

  setting_pgw = config_lookup(&cfg, PGW_CONFIG_STRING_PGW_CONFIG);
  if(setting_pgw != NULL)
  {
      subsetting = config_setting_get_member (setting_pgw, SGW_CONFIG_STRING_NETWORK_INTERFACES_CONFIG);
      if(subsetting != NULL) {
          if(  (
                  config_setting_lookup_string(subsetting,
                          PGW_CONFIG_STRING_PGW_INTERFACE_NAME_FOR_S5_S8,
                          (const char **)&pgw_interface_name_for_S5_S8)
                  && config_setting_lookup_string(subsetting,
                          PGW_CONFIG_STRING_PGW_IPV4_ADDRESS_FOR_S5_S8,
                          (const char **)&pgw_ipv4_address_for_S5_S8)
                  && config_setting_lookup_string(subsetting,
                          PGW_CONFIG_STRING_PGW_INTERFACE_NAME_FOR_SGI,
                          (const char **)&pgw_interface_name_for_SGI)
                  && config_setting_lookup_string(subsetting,
                          PGW_CONFIG_STRING_PGW_IPV4_ADDR_FOR_SGI,
                          (const char **)&pgw_ipv4_address_for_SGI)
                )
            ) {
              config_pP->pgw_config.ipv4.pgw_interface_name_for_S5_S8 = strdup(pgw_interface_name_for_S5_S8);
              cidr = strdup(pgw_ipv4_address_for_S5_S8);
              address = strtok(cidr, "/");
              mask    = strtok(NULL, "/");
              IPV4_STR_ADDR_TO_INT_NWBO ( address, config_pP->pgw_config.ipv4.pgw_ipv4_address_for_S5_S8, "BAD IP ADDRESS FORMAT FOR S5_S8 !\n" )
              config_pP->pgw_config.ipv4.pgw_ip_netmask_for_S5_S8 = atoi(mask);
              free(cidr);

              config_pP->pgw_config.ipv4.pgw_interface_name_for_SGI = strdup(pgw_interface_name_for_SGI);
              cidr = strdup(pgw_ipv4_address_for_SGI);
              address = strtok(cidr, "/");
              mask    = strtok(NULL, "/");
              IPV4_STR_ADDR_TO_INT_NWBO ( address, config_pP->pgw_config.ipv4.pgw_ipv4_address_for_SGI, "BAD IP ADDRESS FORMAT FOR SGI !\n" )
              config_pP->pgw_config.ipv4.pgw_ip_netmask_for_SGI = atoi(mask);
              free(cidr);
          }
      }
      subsetting = config_setting_get_member (setting_pgw, PGW_CONFIG_STRING_IP_ADDRESS_POOL);
      if(subsetting != NULL) {
          sub2setting = config_setting_get_member (subsetting, PGW_CONFIG_STRING_IPV4_ADDRESS_LIST);
          if(sub2setting != NULL) {
              num     = config_setting_length(sub2setting);
              for (i = 0; i < num; i++) {
                  astring = config_setting_get_string_elem(sub2setting,i);
                  if (astring != NULL) {
                      trim(astring, strlen(astring)+1);
                      if (inet_pton(AF_INET, astring, buf_in_addr) < 1) {
                          // failure, test if there is a range specified in the string
                          atoken = strtok(astring, PGW_CONFIG_STRING_IP_ADDRESS_RANGE_DELIMITERS);
                          if (inet_pton(AF_INET, astring, buf_in_addr) == 1) {
                              memcpy (&addr_start, buf_in_addr, sizeof(struct in_addr));
                              // valid address
                              atoken2 = strtok(NULL, PGW_CONFIG_STRING_IP_ADDRESS_RANGE_DELIMITERS);
                              if (inet_pton(AF_INET, atoken2, buf_in_addr) == 1) {
                                  memcpy (&addr_end, buf_in_addr, sizeof(struct in_addr));
                                  // valid address
                                  for (jh = ntohl(addr_start.s_addr); jh <= ntohl(addr_end.s_addr); jh++) {
                                      DevAssert(PGW_MAX_ALLOCATED_PDN_ADDRESSES > config_pP->pgw_config.pool_pdn_addresses.num_ipv4_addresses);
                                      jn = htonl(jh);
                                      if (IN_CLASSA(addr_start.s_addr)) {
                                          if ((jh & 0xFF) && (jh & 0xFF) != 0xFF) {
                                              config_pP->pgw_config.pool_pdn_addresses.ipv4_addresses[config_pP->pgw_config.pool_pdn_addresses.num_ipv4_addresses++].s_addr = jn;
                                          }
                                      } else if (IN_CLASSB(addr_start.s_addr)) {
                                          if ((jh & 0xFF) && (jh & 0xFF) != 0xFF) {
                                              config_pP->pgw_config.pool_pdn_addresses.ipv4_addresses[config_pP->pgw_config.pool_pdn_addresses.num_ipv4_addresses++].s_addr = jn;
                                          }
                                      } else if (IN_CLASSC(addr_start.s_addr)) {
                                          if ((jh & 0xFF) && (jh & 0xFF) != 0xFF) {
                                              config_pP->pgw_config.pool_pdn_addresses.ipv4_addresses[config_pP->pgw_config.pool_pdn_addresses.num_ipv4_addresses++].s_addr = jn;
                                          }
                                      } else {
                                          printf("ERROR ON ADDRESS CLASS %d.%d.%d.%d\n", NIPADDR(jn));
                                      }
                                  }
                              }
                          }
                      } else {
                          DevAssert(PGW_MAX_ALLOCATED_PDN_ADDRESSES > config_pP->pgw_config.pool_pdn_addresses.num_ipv4_addresses);
                          memcpy (&addr_start, buf_in_addr, sizeof(struct in_addr));
                          config_pP->pgw_config.pool_pdn_addresses.ipv4_addresses[config_pP->pgw_config.pool_pdn_addresses.num_ipv4_addresses++].s_addr = addr_start.s_addr;
                      }
                  }
              }
          }
          sub2setting = config_setting_get_member (subsetting, PGW_CONFIG_STRING_IPV6_ADDRESS_LIST);
          if(sub2setting != NULL) {
              num     = config_setting_length(sub2setting);
              for (i = 0; i < num; i++) {
                  astring = config_setting_get_string_elem(sub2setting,i);
                  if (astring != NULL) {
                      trim(astring, strlen(astring)+1);
                      if (inet_pton(AF_INET6, astring, buf_in6_addr) < 1) {
                          // failure, test if there is a range specified in the string
                          atoken = strtok(astring, PGW_CONFIG_STRING_IPV6_PREFIX_DELIMITER);
                          if (inet_pton(AF_INET6, astring, buf_in6_addr) == 1) {
                              atoken2 = strtok(NULL, PGW_CONFIG_STRING_IPV6_PREFIX_DELIMITER);
                              prefix_mask = atoi(atoken2);
                              // arbitrary values
                              DevAssert((prefix_mask < 128) && (prefix_mask >= 64));

                              memcpy (&addr6_start, buf_in6_addr, sizeof(struct in6_addr));
                              memcpy (&addr6_mask,  buf_in6_addr, sizeof(struct in6_addr));
                              sgw_ipv6_mask_in6_addr(&addr6_mask, prefix_mask);

                              if (memcmp(&addr6_start, &addr6_mask, sizeof(struct in6_addr)) != 0) {
                                  AssertFatal(0, "BAD IPV6 ADDR CONFIG/MASK PAIRING %s/%d\n", astring, prefix_mask);
                              }

                              counter64 = 0xFFFFFFFFFFFFFFFF >> prefix_mask; // address Prefix_mask/0..0 not valid
                              do {
                                  addr6_start.s6_addr32[3] = addr6_start.s6_addr32[3] + htonl(1);
                                  if (addr6_start.s6_addr32[3] == 0) {
                                      addr6_start.s6_addr32[2] = addr6_start.s6_addr32[2] + htonl(1);
                                      if (addr6_start.s6_addr32[2] == 0) {
                                          // should not happen since mask is no less than 64
                                          addr6_start.s6_addr32[1] = addr6_start.s6_addr32[1] + htonl(1);
                                          if (addr6_start.s6_addr32[1] == 0) {
                                              addr6_start.s6_addr32[0] = addr6_start.s6_addr32[0] + htonl(1);
                                          }
                                      }
                                  }
                                  memcpy (&config_pP->pgw_config.pool_pdn_addresses.ipv6_addresses[config_pP->pgw_config.pool_pdn_addresses.num_ipv6_addresses++],
                                          &addr6_start,
                                          sizeof(struct in6_addr));
                                  counter64 = counter64 - 1;
                              } while (counter64 > 0);
                          }
                      } else {
                          DevAssert(PGW_MAX_ALLOCATED_PDN_ADDRESSES > config_pP->pgw_config.pool_pdn_addresses.num_ipv6_addresses);
                          memcpy (&config_pP->pgw_config.pool_pdn_addresses.ipv6_addresses[config_pP->pgw_config.pool_pdn_addresses.num_ipv6_addresses++],
                                  buf_in6_addr,
                                  sizeof(struct in6_addr));
                      }
                  }
              }
          }      }
  }
  return 0;
}

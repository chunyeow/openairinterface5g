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
/*! \file spgw_config.c
* \brief
* \author Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/
#define SGW_LITE
#define SPGW_CONFIG_C

#include <string.h>
#include <libconfig.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

#include "log.h"
#include "assertions.h"
#include "spgw_config.h"
#include "sgw_lite_defs.h"
#include "intertask_interface.h"

#ifdef LIBCONFIG_LONG
#define libconfig_int long
#else
#define libconfig_int int
#endif

#define NIPADDR(addr) \
        (uint8_t)(addr & 0x000000FF), \
        (uint8_t)((addr & 0x0000FF00) >> 8), \
        (uint8_t)((addr & 0x00FF0000) >> 16), \
        (uint8_t)((addr & 0xFF000000) >> 24)

#define HIPADDR(addr) \
        (uint8_t)((addr & 0xFF000000) >> 24),\
        (uint8_t)((addr & 0x00FF0000) >> 16),\
        (uint8_t)((addr & 0x0000FF00) >> 8), \
        (uint8_t)(addr & 0x000000FF)

#define NIP6ADDR(addr) \
        ntohs((addr)->s6_addr16[0]), \
        ntohs((addr)->s6_addr16[1]), \
        ntohs((addr)->s6_addr16[2]), \
        ntohs((addr)->s6_addr16[3]), \
        ntohs((addr)->s6_addr16[4]), \
        ntohs((addr)->s6_addr16[5]), \
        ntohs((addr)->s6_addr16[6]), \
        ntohs((addr)->s6_addr16[7])

#define IN6_ARE_ADDR_MASKED_EQUAL(a,b,m) \
           (((((__const uint32_t *) (a))[0] & (((__const uint32_t *) (m))[0])) == (((__const uint32_t *) (b))[0] & (((__const uint32_t *) (m))[0])))  \
         && ((((__const uint32_t *) (a))[1] & (((__const uint32_t *) (m))[1])) == (((__const uint32_t *) (b))[1] & (((__const uint32_t *) (m))[1])))  \
         && ((((__const uint32_t *) (a))[2] & (((__const uint32_t *) (m))[2])) == (((__const uint32_t *) (b))[2] & (((__const uint32_t *) (m))[2])))  \
         && ((((__const uint32_t *) (a))[3] & (((__const uint32_t *) (m))[3])) == (((__const uint32_t *) (b))[3] & (((__const uint32_t *) (m))[3]))))

void trim(char* srcP, int sizeP);
void sgw_ipv6_mask_in6_addr( struct in6_addr *addr6_pP, int maskP);


void
trim(
        char* srcP,
        int sizeP)
{
    if(srcP == NULL)
        return;

    const char* current = srcP;
    unsigned int i = 0;
    while((*current) != '\0' && (i < (sizeP-1)))
    {
        if((*current != ' ') && (*current != '\t')) {
            srcP[i++] = *current;
        }
        ++current;
    }
    srcP[i] = '\0';
}



void
sgw_ipv6_mask_in6_addr(
        struct in6_addr *addr6_pP,
        int maskP)
{
    int      addr8_idx;

    addr8_idx = maskP / 8;
    maskP     = maskP % 8;

    if (maskP > 0) {
        addr6_pP->s6_addr[addr8_idx] = addr6_pP->s6_addr[addr8_idx] & (0xFF << (8 - maskP));
        addr8_idx += 1;
    }
    while (addr8_idx < 16) {
        addr6_pP->s6_addr[addr8_idx++] = 0;
    }
}


int spgw_system(char *command_pP, spgw_system_abort_control_e abort_on_errorP) {
  int ret = -1;
  if (command_pP) {
      SPGW_APP_INFO("system command: %s\n",command_pP);
      ret = system(command_pP);
      if (ret != 0) {
          SPGW_APP_ERROR("ERROR in system command %s: %d\n",
                     command_pP,ret);
          if (abort_on_errorP) {
              exit(-1); // may be not exit
          }
      }
  }
  return ret;
}

int spgw_config_process(spgw_config_t* config_pP) {
  char              system_cmd[256];
  struct in_addr    inaddr;
  int               ret = 0;

  if (strncasecmp("tun",config_pP->sgw_config.ipv4.sgw_interface_name_for_S1u_S12_S4_up, strlen("tun")) == 0) {
        if (snprintf(system_cmd, 256,
                "ip link set %s down ;openvpn --rmtun --dev %s",
                config_pP->sgw_config.ipv4.sgw_interface_name_for_S1u_S12_S4_up,
                config_pP->sgw_config.ipv4.sgw_interface_name_for_S1u_S12_S4_up
                ) > 0) {
            ret += spgw_system(system_cmd, SPGW_ABORT_ON_ERROR);
        } else {
            SPGW_APP_ERROR("Del %s\n", config_pP->sgw_config.ipv4.sgw_interface_name_for_S1u_S12_S4_up);
            ret = -1;
        }
        if (snprintf(system_cmd, 256,
                "openvpn --mktun --dev %s;sync;ifconfig  %s up;sync",
                config_pP->sgw_config.ipv4.sgw_interface_name_for_S1u_S12_S4_up,
                config_pP->sgw_config.ipv4.sgw_interface_name_for_S1u_S12_S4_up) > 0) {
            ret += spgw_system(system_cmd, SPGW_ABORT_ON_ERROR);
        } else {
            SPGW_APP_ERROR("Create %s\n", config_pP->sgw_config.ipv4.sgw_interface_name_for_S1u_S12_S4_up);
            ret = -1;
        }
        inaddr.s_addr = config_pP->sgw_config.ipv4.sgw_ipv4_address_for_S1u_S12_S4_up;
        if (snprintf(system_cmd, 256,
                "ip -4 addr add %s/%d  dev %s",
                inet_ntoa(inaddr),
                config_pP->sgw_config.ipv4.sgw_ip_netmask_for_S1u_S12_S4_up,
                config_pP->sgw_config.ipv4.sgw_interface_name_for_S1u_S12_S4_up) > 0) {
        	ret += spgw_system(system_cmd, SPGW_ABORT_ON_ERROR);
        } else {
            SPGW_APP_ERROR("Set IPv4 address on %s\n", config_pP->sgw_config.ipv4.sgw_interface_name_for_S1u_S12_S4_up);
            ret = -1;
        }

        if (snprintf(system_cmd, 128,
            "iptables -t filter -I INPUT -i lo -d %s --protocol sctp -j DROP",
            inet_ntoa(inaddr)) > 0) {
            ret += spgw_system(system_cmd, SPGW_ABORT_ON_ERROR);
        } else {
            SPGW_APP_ERROR("Drop SCTP traffic on S1U\n");
            ret = -1;
        }
        if (snprintf(system_cmd, 128,
            "iptables -t filter -I INPUT -i lo -s %s --protocol sctp -j DROP",
            inet_ntoa(inaddr)) > 0) {
            ret += spgw_system(system_cmd, SPGW_ABORT_ON_ERROR);
        } else {
            SPGW_APP_ERROR("Drop SCTP traffic on S1U\n");
            ret = -1;
        }
    }

#if defined (ENABLE_USE_GTPU_IN_KERNEL)
  ret += spgw_system("echo 0 > /proc/sys/net/ipv4/conf/all/send_redirects", 1);
#endif

    if (snprintf(system_cmd, 256,
            "ip link set dev %s mtu %u",
            config_pP->sgw_config.ipv4.sgw_interface_name_for_S1u_S12_S4_up,
            config_pP->sgw_config.sgw_interface_mtu_for_S1u_S12_S4_up) > 0) {
        SPGW_APP_INFO("Set S1U interface MTU: %s\n",system_cmd);
        ret += spgw_system(system_cmd, SPGW_ABORT_ON_ERROR);
    } else {
        SPGW_APP_ERROR("Set S1U interface MTU\n");
        ret = -1;
    }

    if (config_pP->sgw_config.sgw_drop_uplink_traffic) {
        if (snprintf(system_cmd, 128,
                     "iptables -t raw -I PREROUTING  -i %s --protocol udp --destination-port 2152  -j DROP",
                     config_pP->sgw_config.ipv4.sgw_interface_name_for_S1u_S12_S4_up) > 0) {
            SPGW_APP_INFO("Drop uplink traffic: %s\n",system_cmd);
            ret += spgw_system(system_cmd, SPGW_ABORT_ON_ERROR);
        } else {
            SPGW_APP_ERROR("Drop uplink traffic\n");
           ret = -1;
        }
    }

    if (config_pP->pgw_config.pgw_masquerade_SGI) {
        inaddr.s_addr = config_pP->pgw_config.ipv4.pgw_ipv4_address_for_SGI;
        if (snprintf(system_cmd, 128,
                     "iptables -t nat -I POSTROUTING  -o %s  ! --protocol sctp -j SNAT --to-source %s",
                     config_pP->pgw_config.ipv4.pgw_interface_name_for_SGI,
                     inet_ntoa(inaddr)) > 0) {
            SPGW_APP_INFO("Masquerade SGI: %s\n",system_cmd);
            ret += spgw_system(system_cmd, SPGW_ABORT_ON_ERROR);
        } else {
            SPGW_APP_ERROR("Masquerade SGI\n");
            ret = -1;
        }
    }
#if defined (ENABLE_USE_GTPU_IN_KERNEL)
    if (snprintf(system_cmd, 128,
                 //"iptables -I POSTROUTING -t mangle -o %s -m state --state NEW  -m mark ! --mark 0 ! --protocol sctp  -j CONNMARK --save-mark",
                 "iptables -I POSTROUTING -t mangle -o %s -m mark ! --mark 0 ! --protocol sctp  -j CONNMARK --save-mark",
                 config_pP->pgw_config.ipv4.pgw_interface_name_for_SGI) > 0) {
        ret += spgw_system(system_cmd, SPGW_ABORT_ON_ERROR);
    } else {
        SPGW_APP_ERROR("Save mark\n");
        ret = -1;
    }
    if (snprintf(system_cmd, 128,
                 "iptables -I PREROUTING -t mangle -i %s ! --protocol sctp   -j CONNMARK --restore-mark",
                 config_pP->pgw_config.ipv4.pgw_interface_name_for_SGI) > 0) {
        ret += spgw_system(system_cmd, SPGW_ABORT_ON_ERROR);
    } else {
        SPGW_APP_ERROR("Restore mark\n");
        ret = -1;
    }
    ret += spgw_system("iptables -X INGTPU", SPGW_WARN_ON_ERROR);
    ret += spgw_system("iptables -N INGTPU", SPGW_ABORT_ON_ERROR);
    if (strncasecmp("tun",config_pP->sgw_config.ipv4.sgw_interface_name_for_S1u_S12_S4_up, strlen("tun")) == 0) {
        if (snprintf(system_cmd, 128,
                     "iptables -A INPUT -i lo --protocol udp --destination-port 2152  -j INGTPU") > 0) {
            ret += spgw_system(system_cmd, SPGW_ABORT_ON_ERROR);
        } else {
            SPGW_APP_ERROR("Trace IP traffic mark\n");
            ret = -1;
        }
    } else {
        if (snprintf(system_cmd, 128,
                     "iptables -A INPUT -i %s --protocol udp --destination-port 2152  -j INGTPU",
                     config_pP->sgw_config.ipv4.sgw_interface_name_for_S1u_S12_S4_up) > 0) {
            ret += spgw_system(system_cmd, SPGW_ABORT_ON_ERROR);
        } else {
            SPGW_APP_ERROR("Trace IP traffic mark\n");
            ret = -1;
        }
    }
    ret += spgw_system("iptables -A INGTPU  -j LOG --log-prefix ' INGTPU ' --log-ip-options --log-level 4", SPGW_ABORT_ON_ERROR);
    ret += spgw_system("iptables -A INGTPU  -j ACCEPT", SPGW_ABORT_ON_ERROR);
#endif
   return ret;
}


int spgw_config_init(char* lib_config_file_name_pP, spgw_config_t* config_pP) {

  config_t          cfg;
  config_setting_t *setting_sgw                          = NULL;
  char             *sgw_interface_name_for_S1u_S12_S4_up = NULL;
  char             *sgw_ipv4_address_for_S1u_S12_S4_up   = NULL;
  char             *sgw_interface_name_for_S5_S8_up      = NULL;
  char             *sgw_ipv4_address_for_S5_S8_up        = NULL;
  char             *sgw_interface_name_for_S11           = NULL;
  char             *sgw_ipv4_address_for_S11             = NULL;
  char             *sgw_drop_uplink_s1u_traffic          = NULL;
  char             *sgw_drop_downlink_s1u_traffic        = NULL;
  libconfig_int     sgw_interface_mtu_for_S1u_S12_S4_up  = 1500;

  config_setting_t *setting_pgw                  = NULL;
  config_setting_t *subsetting                   = NULL;
  config_setting_t *sub2setting                  = NULL;
  char             *pgw_interface_name_for_S5_S8 = NULL;
  char             *pgw_ipv4_address_for_S5_S8   = NULL;
  char             *pgw_interface_name_for_SGI   = NULL;
  char             *pgw_ipv4_address_for_SGI     = NULL;
  char             *pgw_masquerade_SGI           = NULL;
  char             *pgw_default_dns_ipv4_address = NULL;
  char             *pgw_default_dns_sec_ipv4_address = NULL;

  char             *astring  = NULL;
  char             *atoken   = NULL;
  char             *atoken2  = NULL;
  char             *address  = NULL;
  char             *cidr     = NULL;
  char             *mask     = NULL;
  int               num      = 0;
  int               i        = 0;
  unsigned char     buf_in6_addr[sizeof(struct in6_addr)];
  struct in6_addr   addr6_start;
  struct in6_addr   addr6_mask;
  int               prefix_mask = 0;
  uint64_t          counter64   = 0;
  unsigned char     buf_in_addr[sizeof(struct in_addr)];
  struct in_addr    addr_start,in_addr_var;
  struct in_addr    addr_mask;
  pgw_lite_conf_ipv4_list_elm_t *ip4_ref = NULL;
  pgw_lite_conf_ipv6_list_elm_t *ip6_ref = NULL;
#if defined (ENABLE_USE_GTPU_IN_KERNEL)
  char              system_cmd[256];
  int               tun_id               = 21;
#endif

  memset((char*)config_pP, 0 , sizeof(spgw_config_t));
  STAILQ_INIT(&config_pP->pgw_config.pgw_lite_ipv4_pool_list);
  STAILQ_INIT(&config_pP->pgw_config.pgw_lite_ipv6_pool_list);

  config_init(&cfg);

  if(lib_config_file_name_pP != NULL)
  {
      /* Read the file. If there is an error, report it and exit. */
      if(! config_read_file(&cfg, lib_config_file_name_pP))
      {
          SPGW_APP_ERROR("%s:%d - %s\n", lib_config_file_name_pP, config_error_line(&cfg), config_error_text(&cfg));
          config_destroy(&cfg);
          AssertFatal (1 == 0, "Failed to parse SP-GW configuration file %s!\n", lib_config_file_name_pP);
      }
  }
  else
  {
      SPGW_APP_ERROR("No SP-GW configuration file provided!\n");
      config_destroy(&cfg);
      AssertFatal (0, "No SP-GW configuration file provided!\n");
  }
  SPGW_APP_INFO("Parsing configuration file provided %s\n", lib_config_file_name_pP);

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

              in_addr_var.s_addr = config_pP->sgw_config.ipv4.sgw_ipv4_address_for_S1u_S12_S4_up;
              SPGW_APP_INFO("Parsing configuration file found sgw_ipv4_address_for_S1u_S12_S4_up: %s/%d on %s\n",
                      inet_ntoa(in_addr_var),
                      config_pP->sgw_config.ipv4.sgw_ip_netmask_for_S1u_S12_S4_up,
                      config_pP->sgw_config.ipv4.sgw_interface_name_for_S1u_S12_S4_up);


              config_pP->sgw_config.ipv4.sgw_interface_name_for_S5_S8_up = strdup(sgw_interface_name_for_S5_S8_up);
              cidr = strdup(sgw_ipv4_address_for_S5_S8_up);
              address = strtok(cidr, "/");
              mask    = strtok(NULL, "/");
              IPV4_STR_ADDR_TO_INT_NWBO ( address, config_pP->sgw_config.ipv4.sgw_ipv4_address_for_S5_S8_up, "BAD IP ADDRESS FORMAT FOR S5_S8 !\n" )
              config_pP->sgw_config.ipv4.sgw_ip_netmask_for_S5_S8_up = atoi(mask);
              free(cidr);
              in_addr_var.s_addr = config_pP->sgw_config.ipv4.sgw_ipv4_address_for_S5_S8_up;
              SPGW_APP_INFO("Parsing configuration file found sgw_ipv4_address_for_S5_S8_up: %s/%d on %s\n",
                      inet_ntoa(in_addr_var),
                      config_pP->sgw_config.ipv4.sgw_ip_netmask_for_S5_S8_up,
                      config_pP->sgw_config.ipv4.sgw_interface_name_for_S5_S8_up);

              config_pP->sgw_config.ipv4.sgw_interface_name_for_S11 = strdup(sgw_interface_name_for_S11);
              cidr = strdup(sgw_ipv4_address_for_S11);
              address = strtok(cidr, "/");
              mask    = strtok(NULL, "/");
              IPV4_STR_ADDR_TO_INT_NWBO ( address, config_pP->sgw_config.ipv4.sgw_ipv4_address_for_S11, "BAD IP ADDRESS FORMAT FOR S11 !\n" )
              config_pP->sgw_config.ipv4.sgw_ip_netmask_for_S11 = atoi(mask);
              free(cidr);
              in_addr_var.s_addr = config_pP->sgw_config.ipv4.sgw_ipv4_address_for_S11;
              SPGW_APP_INFO("Parsing configuration file found sgw_ipv4_address_for_S11: %s/%d on %s\n",
                      inet_ntoa(in_addr_var),
                      config_pP->sgw_config.ipv4.sgw_ip_netmask_for_S11,
                      config_pP->sgw_config.ipv4.sgw_interface_name_for_S11);
          }
          // optional
          if(config_setting_lookup_int(
                  subsetting,
                  SGW_CONFIG_STRING_SGW_INTERFACE_MTU_FOR_S1U_S12_S4_UP,
                  &sgw_interface_mtu_for_S1u_S12_S4_up)
            ) {
                config_pP->sgw_config.sgw_interface_mtu_for_S1u_S12_S4_up = sgw_interface_mtu_for_S1u_S12_S4_up;
          }
      }
      if(  (
               config_setting_lookup_string( setting_sgw, SGW_CONFIG_STRING_SGW_DROP_UPLINK_S1U_TRAFFIC,
                       (const char **)&sgw_drop_uplink_s1u_traffic)
              && config_setting_lookup_string( setting_sgw, SGW_CONFIG_STRING_SGW_DROP_DOWNLINK_S1U_TRAFFIC,
                      (const char **)&sgw_drop_downlink_s1u_traffic)
            )
        ) {
          if (strcasecmp(sgw_drop_uplink_s1u_traffic, "yes") == 0) {
              config_pP->sgw_config.sgw_drop_uplink_traffic=1;
          } else {
              config_pP->sgw_config.sgw_drop_uplink_traffic=0;
          }
          if (strcasecmp(sgw_drop_downlink_s1u_traffic, "yes") == 0) {
              config_pP->sgw_config.sgw_drop_downlink_traffic=1;
          } else {
              config_pP->sgw_config.sgw_drop_downlink_traffic=0;
          }
      }
  }

  setting_pgw = config_lookup(&cfg, PGW_CONFIG_STRING_PGW_CONFIG);
  if(setting_pgw != NULL)
  {
      subsetting = config_setting_get_member (setting_pgw, PGW_CONFIG_STRING_NETWORK_INTERFACES_CONFIG);
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
                  &&  config_setting_lookup_string(subsetting,
                          PGW_CONFIG_STRING_PGW_MASQUERADE_SGI,
                          (const char **)&pgw_masquerade_SGI)
                )
            ) {
              config_pP->pgw_config.ipv4.pgw_interface_name_for_S5_S8 = strdup(pgw_interface_name_for_S5_S8);
              cidr = strdup(pgw_ipv4_address_for_S5_S8);
              address = strtok(cidr, "/");
              mask    = strtok(NULL, "/");
              IPV4_STR_ADDR_TO_INT_NWBO ( address, config_pP->pgw_config.ipv4.pgw_ipv4_address_for_S5_S8, "BAD IP ADDRESS FORMAT FOR S5_S8 !\n" )
              config_pP->pgw_config.ipv4.pgw_ip_netmask_for_S5_S8 = atoi(mask);
              free(cidr);
              in_addr_var.s_addr = config_pP->pgw_config.ipv4.pgw_ipv4_address_for_S5_S8;
              SPGW_APP_INFO("Parsing configuration file found pgw_ipv4_address_for_S5_S8: %s/%d on %s\n",
                      inet_ntoa(in_addr_var),
                      config_pP->pgw_config.ipv4.pgw_ip_netmask_for_S5_S8,
                      config_pP->pgw_config.ipv4.pgw_interface_name_for_S5_S8);

              config_pP->pgw_config.ipv4.pgw_interface_name_for_SGI = strdup(pgw_interface_name_for_SGI);
              cidr = strdup(pgw_ipv4_address_for_SGI);
              address = strtok(cidr, "/");
              mask    = strtok(NULL, "/");
              IPV4_STR_ADDR_TO_INT_NWBO ( address, config_pP->pgw_config.ipv4.pgw_ipv4_address_for_SGI, "BAD IP ADDRESS FORMAT FOR SGI !\n" )
              config_pP->pgw_config.ipv4.pgw_ip_netmask_for_SGI = atoi(mask);
              free(cidr);
              in_addr_var.s_addr = config_pP->pgw_config.ipv4.pgw_ipv4_address_for_SGI;
              SPGW_APP_INFO("Parsing configuration file found pgw_ipv4_address_for_SGI: %s/%d on %s\n",
                      inet_ntoa(in_addr_var),
                      config_pP->pgw_config.ipv4.pgw_ip_netmask_for_SGI,
                      config_pP->pgw_config.ipv4.pgw_interface_name_for_SGI);

              if (strcasecmp(pgw_masquerade_SGI, "yes") == 0) {
                  config_pP->pgw_config.pgw_masquerade_SGI=1;
              } else {
                  config_pP->pgw_config.pgw_masquerade_SGI=0;
                  SPGW_APP_INFO("No masquerading for SGI\n");
              }
          } else {
              SPGW_APP_WARN("CONFIG P-GW / NETWORK INTERFACES parsing failed\n");
          }
      } else {
          SPGW_APP_WARN("CONFIG P-GW / NETWORK INTERFACES not found\n");
      }

      //!!!------------------------------------!!!
      spgw_config_process(config_pP);
      //!!!------------------------------------!!!

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
                          atoken = strtok(astring, PGW_CONFIG_STRING_IPV4_PREFIX_DELIMITER);
                          if (inet_pton(AF_INET, atoken, buf_in_addr) == 1) {
                              memcpy (&addr_start, buf_in_addr, sizeof(struct in_addr));
                              // valid address
                              atoken2 = strtok(NULL, PGW_CONFIG_STRING_IPV4_PREFIX_DELIMITER);
#if defined (ENABLE_USE_GTPU_IN_KERNEL)
                              in_addr_var.s_addr = config_pP->sgw_config.ipv4.sgw_ipv4_address_for_S1u_S12_S4_up;

                              if (strncasecmp("tun",config_pP->sgw_config.ipv4.sgw_interface_name_for_S1u_S12_S4_up, strlen("tun")) == 0) {
                                  if (snprintf(system_cmd, 256,
                                               "ip link set tun%d down ;openvpn --rmtun --dev tun%d",
                                               tun_id,tun_id) > 0) {
                                      spgw_system(system_cmd, SPGW_ABORT_ON_ERROR);
                                  } else {
                                      SPGW_APP_ERROR("Del tun%d\n", tun_id);
                                  }
                                  if (snprintf(system_cmd, 256,
                                               "openvpn --mktun --dev tun%d;sync;ifconfig  tun%d up;sync",
                                               tun_id,tun_id) > 0) {
                                      spgw_system(system_cmd, SPGW_ABORT_ON_ERROR);
                                  } else {
                                      SPGW_APP_ERROR("Create tun%d\n", tun_id);
                                  }
                              } else {
                                  if (snprintf(system_cmd, 128, "ip route add %s/%s via %s dev %s",
                                           astring,
                                           atoken2,
                                           inet_ntoa(in_addr_var),
                                           config_pP->sgw_config.ipv4.sgw_interface_name_for_S1u_S12_S4_up) > 0) {
                                       spgw_system(system_cmd, SPGW_ABORT_ON_ERROR);
                                   } else {
                                       SPGW_APP_ERROR("Add route: for %s\n", astring);
                                   }
                              }

                              if (config_pP->sgw_config.sgw_drop_downlink_traffic) {
                                  if (snprintf(system_cmd, 128,
                                          "iptables -t filter -I FORWARD  -d %s/%s  -j DROP",
                                          astring, atoken2) > 0) {
                                      spgw_system(system_cmd, SPGW_ABORT_ON_ERROR);
                                  } else {
                                      SPGW_APP_ERROR("Drop downlink traffic\n");
                                  }
                                  if (snprintf(system_cmd, 128,
                                          "iptables -t filter -I OUTPUT  -d %s/%s  -j DROP",
                                          astring, atoken2) > 0) {
                                      spgw_system(system_cmd, SPGW_ABORT_ON_ERROR);
                                  } else {
                                      SPGW_APP_ERROR("Drop downlink traffic\n");
                                  }
                              }
#endif
                              prefix_mask = atoi(atoken2);
                              if ((prefix_mask >= 2)&&(prefix_mask < 32)) {
#if defined (ENABLE_USE_GTPU_IN_KERNEL)
                                  // TEST ONLY
                                  if (strncasecmp("tun",config_pP->sgw_config.ipv4.sgw_interface_name_for_S1u_S12_S4_up, strlen("tun")) == 0) {
                                      memcpy (&in_addr_var, buf_in_addr, sizeof(struct in_addr));
                                      in_addr_var.s_addr = in_addr_var.s_addr + htonl(1);
                                      if (snprintf(system_cmd, 256,
                                                   "ip -4 addr add %s/%d  dev tun%d",
                                                   inet_ntoa(in_addr_var),
                                                   prefix_mask,
                                                   tun_id) > 0) {
                                          spgw_system(system_cmd, SPGW_ABORT_ON_ERROR);
                                      } else {
                                          SPGW_APP_ERROR("Set IPv4 address on tun%d\n", tun_id);
                                      }
                                      tun_id +=1;
                                  }
#endif
                                  memcpy (&addr_start, buf_in_addr, sizeof(struct in_addr));
                                  memcpy (&addr_mask,  buf_in_addr, sizeof(struct in_addr));

                                  addr_mask.s_addr = addr_mask.s_addr & htonl(0xFFFFFFFF << (32 - prefix_mask));

                                  if (memcmp(&addr_start, &addr_mask, sizeof(struct in_addr)) != 0) {
                                      AssertFatal(0, "BAD IPV4 ADDR CONFIG/MASK PAIRING %s/%d addr %X mask %X\n",
                                              astring, prefix_mask, addr_start.s_addr, addr_mask.s_addr);
                                  }

                                  counter64 = 0x00000000FFFFFFFF >> prefix_mask; // address Prefix_mask/0..0 not valid
                                  counter64 = counter64 - 2;
                                  do {
                                      addr_start.s_addr = addr_start.s_addr + htonl(2);
                                      ip4_ref = calloc(1, sizeof(pgw_lite_conf_ipv4_list_elm_t));
                                      ip4_ref->addr       = addr_start;

                                      STAILQ_INSERT_TAIL(&config_pP->pgw_config.pgw_lite_ipv4_pool_list, ip4_ref, ipv4_entries);
                                      counter64 = counter64 - 1;
                                  } while (counter64 > 0);
                              } else {
                                  SPGW_APP_ERROR("CONFIG POOL ADDR IPV4: BAD MASQ: %s\n", atoken2);
                              }
                          } else {
                              SPGW_APP_ERROR("CONFIG POOL ADDR IPV4: ADDR not recognized: %s\n", atoken);
                          }
                      } else {
                          memcpy (&addr_start, buf_in_addr, sizeof(struct in_addr));
                          ip4_ref = calloc(1, sizeof(pgw_lite_conf_ipv4_list_elm_t));
                          ip4_ref->addr = addr_start;
                          STAILQ_INSERT_TAIL(&config_pP->pgw_config.pgw_lite_ipv4_pool_list, ip4_ref, ipv4_entries);
#if defined (ENABLE_USE_GTPU_IN_KERNEL)
                          /*if (snprintf(system_cmd, 128, "ip route add %s dev %s",
                                  buf_in_addr,
                                  config_pP->sgw_config.ipv4.sgw_interface_name_for_S1u_S12_S4_up) > 0) {
                              SPGW_APP_INFO("Add route: %s\n",system_cmd);
                              spgw_system(system_cmd, SPGW_ABORT_ON_ERROR);
                          } else {
                              SPGW_APP_ERROR("Add route: for %s\n", buf_in_addr);
                          }*/
#endif
                      }
                  }
              }
          } else {
              SPGW_APP_WARN("CONFIG POOL ADDR IPV4: NO IPV4 ADDRESS FOUND\n");
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


                               ip6_ref = calloc(1, sizeof(pgw_lite_conf_ipv6_list_elm_t));
                               ip6_ref->addr       = addr6_start;
                               ip6_ref->prefix_len = prefix_mask;
                               STAILQ_INSERT_TAIL(&config_pP->pgw_config.pgw_lite_ipv6_pool_list, ip6_ref, ipv6_entries);

                          }
                      } else {
                          SPGW_APP_WARN("CONFIG POOL ADDR IPV6: FAILED WHILE PARSING %s\n", astring);
                      }
                  }
              }
          }
          if(
                  config_setting_lookup_string(subsetting,
                          PGW_CONFIG_STRING_DEFAULT_DNS_IPV4_ADDRESS,
                          (const char **)&pgw_default_dns_ipv4_address)
                  && config_setting_lookup_string(subsetting,
                          PGW_CONFIG_STRING_DEFAULT_DNS_SEC_IPV4_ADDRESS,
                          (const char **)&pgw_default_dns_sec_ipv4_address)) {
              config_pP->pgw_config.ipv4.pgw_interface_name_for_S5_S8 = strdup(pgw_interface_name_for_S5_S8);
              IPV4_STR_ADDR_TO_INT_NWBO ( pgw_default_dns_ipv4_address,     config_pP->pgw_config.ipv4.default_dns_v4, "BAD IPv4 ADDRESS FORMAT FOR DEFAULT DNS !\n" )
              IPV4_STR_ADDR_TO_INT_NWBO ( pgw_default_dns_sec_ipv4_address, config_pP->pgw_config.ipv4.default_dns_sec_v4, "BAD IPv4 ADDRESS FORMAT FOR DEFAULT DNS SEC!\n" )
          }
      }
  } else {
      SPGW_APP_WARN("CONFIG P-GW not found\n");
  }
  return 0;
}

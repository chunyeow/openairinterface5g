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

#include <pthread.h>
#include <stdint.h>

#include "mme_default_values.h"

#ifndef MME_CONFIG_H_
#define MME_CONFIG_H_

typedef struct mme_config_s {
    /* Reader/writer lock for this configuration */
    pthread_rwlock_t rw_lock;

    uint8_t verbosity_level;

    char *config_file;
    char *realm;
    int   realm_length;

    uint32_t max_eNBs;
    uint32_t max_ues;

    uint8_t relative_capacity;

    uint32_t mme_statistic_timer;

    uint8_t emergency_attach_supported;
    uint8_t unauthenticated_imsi_supported;

    struct {
        uint16_t  nb_mme_gid;
        uint16_t *mme_gid;

        uint16_t  nb_mmec;
        uint8_t  *mmec;

        uint8_t   nb_plmns;
        uint16_t *plmn_mcc;
        uint16_t *plmn_mnc;
        uint16_t *plmn_tac;
    } gummei;

    struct {
        uint16_t in_streams;
        uint16_t out_streams;
    } sctp_config;
    struct {
        uint16_t port_number;
    } gtpv1u_config;
    struct {
        uint16_t port_number;
        uint8_t  outcome_drop_timer_sec;
    } s1ap_config;
    struct {
        char     *sgw_interface_name_for_S1u_S12_S4_up;
        uint32_t  sgw_ip_address_for_S1u_S12_S4_up;
        int       sgw_ip_netmask_for_S1u_S12_S4_up;

        char     *sgw_interface_name_for_S5_S8_up;
        uint32_t  sgw_ip_address_for_S5_S8_up;
        int       sgw_ip_netmask_for_S5_S8_up;

        char     *pgw_interface_name_for_S5_S8;
        uint32_t  pgw_ip_address_for_S5_S8;
        int       pgw_ip_netmask_for_S5_S8;

        char     *pgw_interface_name_for_SGI;
        uint32_t  pgw_ip_addr_for_SGI;
        int       pgw_ip_netmask_for_SGI;

        char     *mme_interface_name_for_S1_MME;
        uint32_t  mme_ip_address_for_S1_MME;
        int       mme_ip_netmask_for_S1_MME;

        char     *mme_interface_name_for_S11;
        uint32_t  mme_ip_address_for_S11;
        int       mme_ip_netmask_for_S11;

        char     *sgw_interface_name_for_S11;
        uint32_t  sgw_ip_address_for_S11;
        int       sgw_ip_netmask_for_S11;
    } ipv4;
    struct {
        char *conf_file;
    } s6a_config;
    struct {
        uint32_t  queue_size;
        char     *log_file;
    } itti_config;
} mme_config_t;

extern mme_config_t mme_config;

int config_parse_opt_line(int argc, char *argv[], mme_config_t *mme_config);

#define config_read_lock(mMEcONFIG)  pthread_rwlock_rdlock(&(mMEcONFIG)->rw_lock)
#define config_write_lock(mMEcONFIG) pthread_rwlock_wrlock(&(mMEcONFIG)->rw_lock)
#define config_unlock(mMEcONFIG)     pthread_rwlock_unlock(&(mMEcONFIG)->rw_lock)

int yyparse(struct mme_config_s *mme_config_p);

#endif /* MME_CONFIG_H_ */

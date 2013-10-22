/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2013 Eurecom

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

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <arpa/inet.h> /* To provide inet_addr */

#include "mme_config.h"
#include "intertask_interface_conf.h"

mme_config_t mme_config;

static
void config_init(mme_config_t *mme_config_p)
{
    memset(mme_config_p, 0, sizeof(mme_config_t));

    pthread_rwlock_init(&mme_config_p->rw_lock, NULL);

    mme_config_p->verbosity_level = 0;
    mme_config_p->config_file     = NULL;
    mme_config_p->max_eNBs        = MAX_NUMBER_OF_ENB;
    mme_config_p->max_ues         = MAX_NUMBER_OF_UE;
    /* Timer configuration */
    mme_config_p->gtpv1u_config.port_number = GTPV1_U_PORT_NUMBER;
    mme_config_p->s1ap_config.port_number   = S1AP_PORT_NUMBER;
    /* IP configuration */
    mme_config_p->ipv4.sgw_interface_name_for_S1u_S12_S4_up = DEFAULT_SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP;
    mme_config_p->ipv4.sgw_ip_address_for_S1u_S12_S4_up     = inet_addr(DEFAULT_SGW_IP_ADDRESS_FOR_S1U_S12_S4_UP);
    mme_config_p->ipv4.sgw_ip_netmask_for_S1u_S12_S4_up     = DEFAULT_SGW_IP_NETMASK_FOR_S1U_S12_S4_UP;

    mme_config_p->ipv4.sgw_interface_name_for_S5_S8_up      = DEFAULT_SGW_INTERFACE_NAME_FOR_S5_S8_UP;
    mme_config_p->ipv4.sgw_ip_address_for_S5_S8_up          = inet_addr(DEFAULT_SGW_IP_ADDRESS_FOR_S5_S8_UP);
    mme_config_p->ipv4.sgw_ip_netmask_for_S5_S8_up          = DEFAULT_SGW_IP_NETMASK_FOR_S5_S8_UP;

    mme_config_p->ipv4.pgw_interface_name_for_SGI           = DEFAULT_PGW_INTERFACE_NAME_FOR_S5_S8;
    mme_config_p->ipv4.pgw_ip_addr_for_SGI                  = inet_addr(DEFAULT_PGW_IP_ADDRESS_FOR_S5_S8);
    mme_config_p->ipv4.pgw_ip_netmask_for_SGI               = DEFAULT_PGW_IP_NETMASK_FOR_S5_S8;

    mme_config_p->ipv4.mme_interface_name_for_S1_MME        = DEFAULT_MME_INTERFACE_NAME_FOR_S1_MME;
    mme_config_p->ipv4.mme_ip_address_for_S1_MME            = inet_addr(DEFAULT_MME_IP_ADDRESS_FOR_S1_MME);
    mme_config_p->ipv4.mme_ip_netmask_for_S1_MME            = DEFAULT_MME_IP_NETMASK_FOR_S1_MME;

    mme_config_p->ipv4.mme_interface_name_for_S11           = DEFAULT_MME_INTERFACE_NAME_FOR_S11;
    mme_config_p->ipv4.mme_ip_address_for_S11               = inet_addr(DEFAULT_MME_IP_ADDRESS_FOR_S11);
    mme_config_p->ipv4.mme_ip_netmask_for_S11               = DEFAULT_MME_IP_NETMASK_FOR_S11;

    mme_config_p->ipv4.sgw_interface_name_for_S11           = DEFAULT_SGW_INTERFACE_NAME_FOR_S11;
    mme_config_p->ipv4.sgw_ip_address_for_S11               = inet_addr(DEFAULT_SGW_IP_ADDRESS_FOR_S11);
    mme_config_p->ipv4.sgw_ip_netmask_for_S11               = DEFAULT_SGW_IP_NETMASK_FOR_S11;

    mme_config_p->s6a_config.conf_file    = S6A_CONF_FILE;
    mme_config_p->itti_config.queue_size  = ITTI_QUEUE_SIZE_MAX;

    mme_config_p->sctp_config.in_streams  = SCTP_IN_STREAMS;
    mme_config_p->sctp_config.out_streams = SCTP_OUT_STREAMS;

    mme_config_p->relative_capacity = RELATIVE_CAPACITY;

    mme_config_p->mme_statistic_timer = MME_STATISTIC_TIMER_S;

    mme_config_p->gummei.nb_mme_gid = 1;
    mme_config_p->gummei.mme_gid    = calloc(1, sizeof(*mme_config_p->gummei.mme_gid));
    mme_config_p->gummei.mme_gid[0] = MMEGID;
    mme_config_p->gummei.nb_mmec    = 1;
    mme_config_p->gummei.mmec       = calloc(1, sizeof(*mme_config_p->gummei.mmec));
    mme_config_p->gummei.mmec[0]    = MMEC;

    /* Set the TAI */
    mme_config_p->gummei.nb_plmns   = 1;
    mme_config_p->gummei.plmn_mcc   = calloc(1, sizeof(*mme_config_p->gummei.plmn_mcc));
    mme_config_p->gummei.plmn_mnc   = calloc(1, sizeof(*mme_config_p->gummei.plmn_mnc));
    mme_config_p->gummei.plmn_tac   = calloc(1, sizeof(*mme_config_p->gummei.plmn_tac));

    mme_config_p->gummei.plmn_mcc[0] = PLMN_MCC;
    mme_config_p->gummei.plmn_mnc[0] = PLMN_MNC;
    mme_config_p->gummei.plmn_tac[0] = PLMN_TAC;

    mme_config_p->s1ap_config.outcome_drop_timer_sec = S1AP_OUTCOME_TIMER_DEFAULT;
}

static int config_parse_file(mme_config_t *mme_config_p)
{
    extern FILE *yyin;
    int ret = -1;

    if (mme_config_p == NULL)
        return ret;
    if (mme_config_p->config_file == NULL) {
        fprintf(stderr, "No Configuration file given... Attempting default values\n");
        return 0;
    }

    yyin = fopen(mme_config_p->config_file, "r");
    if (!yyin) {
        /* We failed to open the file */
        fprintf(stderr, "Unable to open the configuration file: %s (%d:%s)\n",
                mme_config_p->config_file, errno, strerror(errno));
        return errno;
    }

    /* Call the yacc parser */
    ret = yyparse(mme_config_p);

    /* Close the file descriptor */
    if (fclose(yyin) != 0) {
        fprintf(stderr, "Unable to close the configuration file: %s (%d:%s)\n",
                mme_config_p->config_file, errno, strerror(errno));
        return errno;
    }
    return ret;
}

#define DISPLAY_ARRAY(size, format, args...)                        \
do {                                                                \
    int i;                                                          \
    for (i = 0; i < size; i++) {                                    \
        fprintf(stdout, format, args);                              \
        if ((i != (size - 1)) && ((i + 1) % 10 == 0))               \
        {                                                           \
            fprintf(stdout, "\n        ");                          \
        }                                                           \
    }                                                               \
    if (i > 0)                                                      \
        fprintf(stdout, "\n");                                      \
} while(0)

static void config_display(mme_config_t *mme_config_p)
{
    fprintf(stdout, "==== EURECOM %s v%s ====\n", PACKAGE_NAME, PACKAGE_VERSION);
    fprintf(stdout, "Configuration:\n");
    fprintf(stdout, "- File .............: %s\n", mme_config_p->config_file);
    fprintf(stdout, "- Verbosity level ..: %d\n", mme_config_p->verbosity_level);
    fprintf(stdout, "- Realm ............: %s\n", mme_config_p->realm);
    fprintf(stdout, "- Max eNBs .........: %u\n", mme_config_p->max_eNBs);
    fprintf(stdout, "- Max UEs ..........: %u\n", mme_config_p->max_ues);
    fprintf(stdout, "- Relative capa ....: %u\n\n", mme_config_p->relative_capacity);
    fprintf(stdout, "- Statistics timer .: %u (seconds)\n\n", mme_config_p->mme_statistic_timer);
    fprintf(stdout, "- S1-U:\n");
    fprintf(stdout, "    port number ....: %d\n", mme_config_p->gtpv1u_config.port_number);
    fprintf(stdout, "- S1-MME:\n");
    fprintf(stdout, "    port number ....: %d\n", mme_config_p->s1ap_config.port_number);
    fprintf(stdout, "- IP:\n");
    fprintf(stdout, "    s1-u iface .....: %s\n", mme_config_p->ipv4.sgw_interface_name_for_S1u_S12_S4_up);
    fprintf(stdout, "    s1-u ip ........: %s/%d\n",
            inet_ntoa(*((struct in_addr *)&mme_config_p->ipv4.sgw_ip_address_for_S1u_S12_S4_up)),
            mme_config_p->ipv4.sgw_ip_netmask_for_S1u_S12_S4_up);
    fprintf(stdout, "    sgi iface ......: %s\n", mme_config_p->ipv4.pgw_interface_name_for_SGI);
    fprintf(stdout, "    sgi ip .........: %s/%d\n",
            inet_ntoa(*((struct in_addr *)&mme_config_p->ipv4.pgw_ip_addr_for_SGI)),
            mme_config_p->ipv4.pgw_ip_netmask_for_SGI);
    fprintf(stdout, "    s1-MME iface ...: %s\n", mme_config_p->ipv4.mme_interface_name_for_S1_MME);
    fprintf(stdout, "    s1-MME ip ......: %s/%d\n",
            inet_ntoa(*((struct in_addr *)&mme_config_p->ipv4.mme_ip_address_for_S1_MME)),
            mme_config_p->ipv4.mme_ip_netmask_for_S1_MME);
    fprintf(stdout, "    s11 S-GW iface .: %s\n", mme_config_p->ipv4.sgw_interface_name_for_S11);
    fprintf(stdout, "    s11 S-GW ip ....: %s/%d\n",
            inet_ntoa(*((struct in_addr *)&mme_config_p->ipv4.sgw_ip_address_for_S11)),
            mme_config_p->ipv4.sgw_ip_netmask_for_S11);
    fprintf(stdout, "    s11 MME iface ..: %s\n", mme_config_p->ipv4.mme_interface_name_for_S11);
    fprintf(stdout, "    s11 S-GW ip ....: %s/%d\n",
            inet_ntoa(*((struct in_addr *)&mme_config_p->ipv4.mme_ip_address_for_S11)),
            mme_config_p->ipv4.mme_ip_netmask_for_S11);
    fprintf(stdout, "- ITTI:\n");
    fprintf(stdout, "    queue size .....: %u (bytes)\n", mme_config_p->itti_config.queue_size);
    fprintf(stdout, "- SCTP:\n");
    fprintf(stdout, "    in streams .....: %u\n", mme_config_p->sctp_config.in_streams);
    fprintf(stdout, "    out streams ....: %u\n", mme_config_p->sctp_config.out_streams);
    fprintf(stdout, "- GUMMEI:\n");
    fprintf(stdout, "    mme group ids ..:\n        ");
    DISPLAY_ARRAY(mme_config_p->gummei.nb_mme_gid, "| %u ", mme_config_p->gummei.mme_gid[i]);
    fprintf(stdout, "    mme codes ......:\n        ");
    DISPLAY_ARRAY(mme_config_p->gummei.nb_mmec, "| %u ", mme_config_p->gummei.mmec[i]);
    fprintf(stdout, "    plmns ..........: (mcc.mnc:tac)\n        ");
    DISPLAY_ARRAY(mme_config_p->gummei.nb_plmns, "| %3u.%3u:%u ",
                  mme_config_p->gummei.plmn_mcc[i], mme_config_p->gummei.plmn_mnc[i],
                  mme_config_p->gummei.plmn_tac[i]);
    fprintf(stdout, "- S6A:\n");
    fprintf(stdout, "    conf file ......: %s\n", mme_config_p->s6a_config.conf_file);
}

static void usage(void)
{
    fprintf(stdout, "==== EURECOM %s v%s ====\n", PACKAGE_NAME, PACKAGE_VERSION);
    fprintf(stdout, "Please report any bug to: %s\n\n", PACKAGE_BUGREPORT);
    fprintf(stdout, "Usage: oaisim_mme [options]\n\n");
    fprintf(stdout, "Available options:\n");
    fprintf(stdout, "-h      Print this help and return\n");
    fprintf(stdout, "-i<interface name>\n");
    fprintf(stdout, "        Set the network card to use for IPv4 forwarding\n");
    fprintf(stdout, "-c<path>\n");
    fprintf(stdout, "        Set the configuration file for mme\n");
    fprintf(stdout, "        See template in UTILS/CONF\n");
    fprintf(stdout, "-V      Print %s version and return\n", PACKAGE_NAME);
    fprintf(stdout, "-v[1-2] Debug level:\n");
    fprintf(stdout, "            1 -> ASN1 XER printf on and ASN1 debug off\n");
    fprintf(stdout, "            2 -> ASN1 XER printf on and ASN1 debug on\n");
}

extern void
nwGtpv1uDisplayBanner(void);

int config_parse_opt_line(int argc, char *argv[], mme_config_t *mme_config_p)
{
    int c;
    config_init(mme_config_p);
    /* Parsing command line */
    while ((c = getopt (argc, argv, "c:hi:v:V")) != -1) {
        switch (c) {
            case 'c': {
                /* Store the given configuration file. If no file is given,
                 * then the default values will be used.
                 */
                int config_file_len = 0;
                config_file_len = strlen(optarg);
                mme_config_p->config_file = malloc(sizeof(char) * (config_file_len + 1));
                memcpy(mme_config_p->config_file, optarg, config_file_len);
                mme_config_p->config_file[config_file_len] = '\0';
            } break;
            case 'i': {
                int interface_len = 0;

                /* Copying provided interface name to use for ipv4 forwarding */
                interface_len = strlen(optarg);
                mme_config_p->ipv4.sgw_interface_name_for_S1u_S12_S4_up = calloc(interface_len + 1, sizeof(char));
                memcpy(mme_config_p->ipv4.sgw_interface_name_for_S1u_S12_S4_up, optarg, interface_len);
                mme_config_p->ipv4.sgw_interface_name_for_S1u_S12_S4_up[interface_len] = '\0';
            } break;
            case 'v': {
                mme_config_p->verbosity_level = atoi(optarg);
            } break;
            case 'V': {
                fprintf(stdout, "==== EURECOM %s v%s ====\n"
                        "Please report any bug to: %s\n", PACKAGE_NAME, PACKAGE_VERSION,
                        PACKAGE_BUGREPORT);
                exit(0);
                nwGtpv1uDisplayBanner();
            } break;
            case 'h': /* Fall through */
            default:
                usage();
                exit(0);
        }
    }
    /* Parse the configuration file using bison */
    if (config_parse_file(mme_config_p) != 0) {
        return -1;
    }
    /* Display yhe configuration */
    config_display(mme_config_p);
    return 0;
}

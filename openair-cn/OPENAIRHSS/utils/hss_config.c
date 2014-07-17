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

/*! \file hss_config.c
 * \brief Base configuration for the HSS. Parse command line and configuration file
 * \author Sebastien ROUX <sebastien.roux@eurecom.fr>
 * \date 2013
 * \version 0.1
 */

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>

#include "hss_config.h"
#include "hss_parser.h"

/* YACC forward declarations */
extern int  yyparse (struct hss_config_s *hss_config_p);

static int config_parse_command_line(int argc, char *argv[],
                                     hss_config_t *hss_config_p);
static int config_parse_file(hss_config_t *hss_config_p);
static void display_banner(void);
static void config_display(hss_config_t *hss_config_p);

static struct option long_options[] = {
    { "config",  1, 0, 'c' },
    { "help",    0, 0, 'h' },
    { "version", 0, 0, 'v' },
    { 0, 0, 0, 0 },
};

static const char option_string[] = "c:vh";

int config_init(int argc, char *argv[], hss_config_t *hss_config_p)
{
    int ret = 0;

    if (hss_config_p == NULL) {
        return EINVAL;
    }
    if ((ret = config_parse_command_line(argc, argv, hss_config_p)) != 0) {
        return ret;
    }
    display_banner();
    if ((ret = config_parse_file(hss_config_p)) != 0) {
        /* Parsing of the file failed. -> abort */
        abort();
    }
    config_display(hss_config_p);
    return ret;
}

static void display_banner(void)
{
    fprintf(stdout, "==== EURECOM %s v%s ====\n", PACKAGE_NAME, PACKAGE_VERSION);
    fprintf(stdout, "Please report any bug to: %s\n\n", PACKAGE_BUGREPORT);
}

static void usage(void)
{
    display_banner();
    fprintf(stdout, "Usage: openair_hss [options]\n\n");
    fprintf(stdout, "Available options:\n");
    fprintf(stdout, "\t--help\n\t-h\n");
    fprintf(stdout, "\t\tPrint this help and return\n\n");
    fprintf(stdout, "\t--config=<path>\n\t-c<path>\n");
    fprintf(stdout, "\t\tSet the configuration file for hss\n");
    fprintf(stdout, "\t\tSee template in conf dir\n\n");
    fprintf(stdout, "\t--version\n\t-v\n");
    fprintf(stdout, "\t\tPrint %s version and return\n", PACKAGE_NAME);
}

static void config_display(hss_config_t *hss_config_p)
{
    fprintf(stdout, "Configuration\n");
    fprintf(stdout, "* Global:\n");
    fprintf(stdout, "\t- File .............: %s\n", hss_config_p->config);
    fprintf(stdout, "* MYSQL:\n");
    fprintf(stdout, "\t- Server ...........: %s\n", hss_config_p->mysql_server);
    fprintf(stdout, "\t- Database .........: %s\n", hss_config_p->mysql_database);
    fprintf(stdout, "\t- User .............: %s\n", hss_config_p->mysql_user);
    fprintf(stdout, "\t- Password .........: %s\n",
            (hss_config_p->mysql_password == NULL) ? "None" : "*****");
    fprintf(stdout, "* FreeDiameter:\n");
    fprintf(stdout, "\t- Conf file ........: %s\n",
            hss_config_p->freediameter_config);
}

static int config_parse_command_line(int argc, char *argv[],
                                     hss_config_t *hss_config_p)
{
    int c;
    int option_index = 0;

    while ((c = getopt_long (argc, argv, option_string, long_options,
                             &option_index)) != -1) {
        switch(c) {
            case 'c': {
                hss_config_p->config = strdup(optarg);
            }
            break;
            case 'v': {
                /* We display version and return immediately */
                display_banner();
                exit(0);
            }
            break;
            default:
            case 'h': {
                usage();
                exit(0);
            }
            break;
        }
    }
    return 0;
}

static int config_parse_file(hss_config_t *hss_config_p)
{
    extern FILE *yyin;
    int ret = -1;

    if (hss_config_p == NULL) {
        return ret;
    }
    if (hss_config_p->config == NULL) {
        return ret;
    }

    printf("Parsing configuration file: %s\n", hss_config_p->config);

    yyin = fopen(hss_config_p->config, "r");
    if (!yyin) {
        /* We failed to open the file */
        fprintf(stderr, "Unable to open the configuration file: %s (%d:%s)\n",
                hss_config_p->config, errno, strerror(errno));
        return errno;
    }

    /* Call the yacc parser */
    ret = yyparse(hss_config_p);

    /* Close the file descriptor */
    if (fclose(yyin) != 0) {
        fprintf(stderr, "Unable to close the configuration file: %s (%d:%s)\n",
                hss_config_p->config, errno, strerror(errno));
        return errno;
    }
    return ret;
}

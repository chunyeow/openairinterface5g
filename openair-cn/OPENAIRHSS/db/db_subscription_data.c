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
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <error.h>

#include <mysql/mysql.h>

#include "hss_config.h"
#include "db_proto.h"

int hss_mysql_query_pdns(const char   *imsi,
                         mysql_pdn_t **pdns_p,
                         uint8_t      *nb_pdns)
{
    int          ret;
    MYSQL_RES   *res = NULL;
    MYSQL_ROW    row;
    char         query[255];
    mysql_pdn_t *pdn_array = NULL;

    if (db_desc->db_conn == NULL) {
        return EINVAL;
    }
    if (nb_pdns == NULL || pdns_p == NULL) {
        return EINVAL;
    }

    sprintf(query, "SELECT * FROM `pdn` WHERE "
                   "`pdn`.`users_imsi`=%s LIMIT 10; ", imsi);

    printf("Query: %s\n", query);

    pthread_mutex_lock(&db_desc->db_cs_mutex);

    if (mysql_query(db_desc->db_conn, query))
    {
        pthread_mutex_unlock(&db_desc->db_cs_mutex);
        fprintf(stderr, "Query execution failed: %s\n",
                mysql_error(db_desc->db_conn));
        mysql_thread_end();
        return EINVAL;
    }

    res = mysql_store_result(db_desc->db_conn);

    pthread_mutex_unlock(&db_desc->db_cs_mutex);

    *nb_pdns = 0;

    while ((row = mysql_fetch_row(res)) != NULL)
    {
        mysql_pdn_t *pdn_elm;   /* Local PDN element in array */
        unsigned long *lengths;

        lengths = mysql_fetch_lengths(res);

        *nb_pdns += 1;
        if (*nb_pdns == 1) {
            /* First row, must malloc */
            pdn_array = malloc(sizeof(mysql_pdn_t));
        } else {
            pdn_array = realloc(pdn_array, *nb_pdns * sizeof(mysql_pdn_t));
        }
        if (pdn_array == NULL) {
            /* Error on malloc */
            ret = ENOMEM;
            goto err;
        }
        pdn_elm = &pdn_array[*nb_pdns - 1];
        /* Copying the APN */
        memset(pdn_elm, 0, sizeof(mysql_pdn_t));
        memcpy(pdn_elm->apn, row[1], lengths[1]);
        /* PDN Type + PDN address */
        if (strcmp(row[2], "IPv6") == 0) {
            pdn_elm->pdn_type = IPV6;
            memcpy(pdn_elm->pdn_address.ipv6_address, row[4], lengths[4]);
            pdn_elm->pdn_address.ipv6_address[lengths[4]] = '\0';
        } else if (strcmp(row[2], "IPv4v6") == 0) {
            pdn_elm->pdn_type = IPV4V6;
            memcpy(pdn_elm->pdn_address.ipv4_address, row[3], lengths[3]);
            pdn_elm->pdn_address.ipv4_address[lengths[3]] = '\0';
            memcpy(pdn_elm->pdn_address.ipv6_address, row[4], lengths[4]);
            pdn_elm->pdn_address.ipv6_address[lengths[4]] = '\0';
        } else if (strcmp(row[2], "IPv4_or_IPv6") == 0) {
            pdn_elm->pdn_type = IPV4_OR_IPV6;
            memcpy(pdn_elm->pdn_address.ipv4_address, row[3], lengths[3]);
            pdn_elm->pdn_address.ipv4_address[lengths[3]] = '\0';
            memcpy(pdn_elm->pdn_address.ipv6_address, row[4], lengths[4]);
            pdn_elm->pdn_address.ipv6_address[lengths[4]] = '\0';
        } else {
            pdn_elm->pdn_type = IPV4;
            memcpy(pdn_elm->pdn_address.ipv4_address, row[3], lengths[3]);
            pdn_elm->pdn_address.ipv4_address[lengths[3]] = '\0';
        }
        pdn_elm->aggr_ul = atoi(row[5]);
        pdn_elm->aggr_dl = atoi(row[6]);
        pdn_elm->qci     = atoi(row[9]);
        pdn_elm->priority_level = atoi(row[10]);
        if (strcmp(row[11], "ENABLED") == 0) {
            pdn_elm->pre_emp_cap = 0;
        } else {
            pdn_elm->pre_emp_cap = 1;
        }
        if (strcmp(row[12], "DISABLED") == 0) {
            pdn_elm->pre_emp_vul = 1;
        } else {
            pdn_elm->pre_emp_vul = 0;
        }
    }

    mysql_free_result(res);
    mysql_thread_end();

    /* We did not find any APN for the requested IMSI */
    if (*nb_pdns == 0) {
        return EINVAL;
    } else {
        *pdns_p = pdn_array;
        return 0;
    }

err:
    if (pdn_array) {
        free(pdn_array);
    }
    pdn_array = NULL;
    *pdns_p = pdn_array;
    *nb_pdns = 0;
    mysql_free_result(res);
    mysql_thread_end();
    return ret;
}

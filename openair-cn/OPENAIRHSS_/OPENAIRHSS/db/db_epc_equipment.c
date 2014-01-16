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

int hss_mysql_query_mmeidentity(const int id_mme_identity,
                                mysql_mme_identity_t *mme_identity_p)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[255];

    if ((db_desc->db_conn == NULL) || (mme_identity_p == NULL)) {
        return EINVAL;
    }

    memset(mme_identity_p, 0, sizeof(mysql_mme_identity_t));

    sprintf(query, "SELECT mmehost,mmerealm FROM mmeidentity WHERE "
                   "mmeidentity.idmmeidentity='%d' ", id_mme_identity);

    DB_DEBUG("Query: %s\n", query);

    pthread_mutex_lock(&db_desc->db_cs_mutex);

    if (mysql_query(db_desc->db_conn, query))
    {
        pthread_mutex_unlock(&db_desc->db_cs_mutex);
        DB_ERROR("Query execution failed: %s\n",
                 mysql_error(db_desc->db_conn));
        mysql_thread_end();
        return EINVAL;
    }

    res = mysql_store_result(db_desc->db_conn);

    pthread_mutex_unlock(&db_desc->db_cs_mutex);
    if ((row = mysql_fetch_row(res)) != NULL) {
        if (row[0] != NULL) {
            memcpy(mme_identity_p->mme_host, row[0], strlen(row[0]));
        } else {
            mme_identity_p->mme_host[0] = '\0';
        }

        if (row[1] != NULL) {
            memcpy(mme_identity_p->mme_realm, row[1], strlen(row[1]));
        } else {
            mme_identity_p->mme_realm[0] = '\0';
        }

        mysql_free_result(res);
        mysql_thread_end();
        return 0;
    }

    mysql_free_result(res);
    mysql_thread_end();
    return EINVAL;
}

int hss_mysql_check_epc_equipment(mysql_mme_identity_t *mme_identity_p)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[255];

    if ((db_desc->db_conn == NULL) || (mme_identity_p == NULL)) {
        return EINVAL;
    }

    sprintf(query, "SELECT idmmeidentity FROM mmeidentity WHERE mmeidentity.mmehost='%s' ",
            mme_identity_p->mme_host);

    DB_DEBUG("Query: %s\n", query);

    pthread_mutex_lock(&db_desc->db_cs_mutex);

    if (mysql_query(db_desc->db_conn, query))
    {
        pthread_mutex_unlock(&db_desc->db_cs_mutex);
        DB_ERROR("Query execution failed: %s\n",
                 mysql_error(db_desc->db_conn));
        mysql_thread_end();
        return EINVAL;
    }

    res = mysql_store_result(db_desc->db_conn);

    pthread_mutex_unlock(&db_desc->db_cs_mutex);
    if ((row = mysql_fetch_row(res)) != NULL) {
        mysql_free_result(res);
        mysql_thread_end();
        return 0;
    }

    mysql_free_result(res);
    mysql_thread_end();
    return EINVAL;
}

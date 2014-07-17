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

#include <stdio.h>
#include <stdint.h>
#include <mysql/mysql.h>

#include <netinet/in.h> /* To provide internet addresses strings helpers */

#ifndef DB_PROTO_H_
#define DB_PROTO_H_

#define DB_DEBUG(fmt, args...) fprintf(stdout, "%s:%d: "fmt, __FILE__, __LINE__, ##args)
#define DB_ERROR(fmt, args...) fprintf(stderr, "%s:%d: "fmt, __FILE__, __LINE__, ##args)

typedef struct {
    /* The mysql reference connector object */
    MYSQL *db_conn;
    char  *server;
    char  *user;
    char  *password;
    char  *database;

    pthread_mutex_t db_cs_mutex;
} database_t;

extern database_t *db_desc;

typedef uint32_t pre_emp_vul_t;
typedef uint32_t pre_emp_cap_t;
typedef uint8_t  access_restriction_t;
typedef uint32_t ambr_t;
typedef uint8_t  qci_t;
typedef uint8_t  prio_level_t;
typedef uint32_t rau_tau_t;

#define IMSI_LENGTH_MAX (15)

typedef struct {
    char imsi[IMSI_LENGTH_MAX + 1];
} mysql_auth_info_req_t;

/* Expressed in bytes */
#define KEY_LENGTH  (16)
#define SQN_LENGTH  (6)
#define RAND_LENGTH (16)

typedef struct {
    uint8_t key[KEY_LENGTH];
    uint8_t sqn[SQN_LENGTH];
    /* RAND should not be here... */
    uint8_t rand[RAND_LENGTH];
} mysql_auth_info_resp_t;

typedef struct {
    char imsi[IMSI_LENGTH_MAX + 1];
    /* New computed SQN that will be used on next auth info req */
    uint8_t sqn[SQN_LENGTH];
} mysql_sqn_push_t;

typedef struct {
    /* An MME may have already been registered as serving the UE. */
    char mme_host[255];
    char mme_realm[200];
} mysql_mme_identity_t;

typedef struct {
    char imsi[16];
    /* MSISDN this parameter may be NULL */
    char msisdn[16];

    /* Maximum aggregated bitrates for the user */
    ambr_t aggr_ul;
    ambr_t aggr_dl;

    /* Subscribed RAU-TAU timer */
    rau_tau_t rau_tau;

    access_restriction_t access_restriction;
    mysql_mme_identity_t mme_identity;
} mysql_ul_ans_t;

typedef struct {
    /* Bit masks indicating presence of optional fields */
#define MME_IDENTITY_PRESENT           (0x1)
#define MME_SUPPORTED_FEATURES_PRESENT (0x1)
#define IMEI_PRESENT                   (0x1)
#define SV_PRESENT                     (0x1)
#define UE_SRVCC_PRESENT               (0x1)

    unsigned mme_identity_present:1;
    unsigned mme_supported_features_present:1;
    unsigned imei_present:1;
    unsigned sv_present:1;
    unsigned ue_srvcc_present:1;

    /* IMSI */
    char imsi[16];
    /* Origin host and realm */
    mysql_mme_identity_t mme_identity;
    /* IMEISV */
    char imei[16];
    char software_version[2];

    uint32_t ue_srvcc;
    uint32_t mme_supported_features;
} mysql_ul_push_t;

typedef enum {
    IPV4         = 0,
    IPV6         = 1,
    IPV4V6       = 2,
    IPV4_OR_IPV6 = 3,
} pdn_type_t;

typedef struct {
    char ipv4_address[INET_ADDRSTRLEN];
    char ipv6_address[INET6_ADDRSTRLEN];
} pdn_address_t;

typedef struct {
    char          apn[61];
    pdn_type_t    pdn_type;
    pdn_address_t pdn_address;
    ambr_t        aggr_ul;
    ambr_t        aggr_dl;
    qci_t         qci;
    prio_level_t  priority_level;
    pre_emp_cap_t pre_emp_cap;
    pre_emp_vul_t pre_emp_vul;
} mysql_pdn_t;

typedef struct {
    /* IMSI */
    char imsi[16];
} mysql_pu_req_t;

typedef mysql_mme_identity_t mysql_pu_ans_t;

int hss_mysql_connect(const hss_config_t *hss_config_p);

void hss_mysql_disconnect(void);

int hss_mysql_get_user(const char *imsi);

int hss_mysql_update_loc(const char *imsi, mysql_ul_ans_t *mysql_ul_ans);

int hss_mysql_query_mmeidentity(const int id_mme_identity,
                                mysql_mme_identity_t *mme_identity_p);

int hss_mysql_check_epc_equipment(mysql_mme_identity_t *mme_identity_p);

int mysql_push_up_loc(mysql_ul_push_t *ul_push_p);

int hss_mysql_purge_ue(mysql_pu_req_t *mysql_pu_req,
                       mysql_pu_ans_t *mysql_pu_ans);

int hss_mysql_query_pdns(const char   *imsi,
                         mysql_pdn_t **pdns_p,
                         uint8_t      *nb_pdns);

int hss_mysql_auth_info(mysql_auth_info_req_t  *auth_info_req,
                        mysql_auth_info_resp_t *auth_info_resp);

int hss_mysql_push_rand_sqn(const char *imsi, uint8_t *rand_p, uint8_t *sqn);

int hss_mysql_increment_sqn(const char *imsi);

#endif /* DB_PROTO_H_ */

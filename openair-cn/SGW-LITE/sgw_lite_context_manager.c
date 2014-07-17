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
/*! \file sgw_lite_context_manager.c
* \brief
* \author Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/
#define SGW_LITE
#define SGW_LITE_CONTEXT_MANAGER_C

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "tree.h"
#include "hashtable.h"

#include "intertask_interface.h"
#include "mme_config.h"
#include "sgw_lite_defs.h"
#include "sgw_lite_context_manager.h"
#include "sgw_lite.h"

extern sgw_app_t sgw_app;


//-----------------------------------------------------------------------------
static void sgw_lite_display_s11teid2mme_mapping(uint64_t keyP, void *dataP, void* unusedParameterP)
//-----------------------------------------------------------------------------
{
    mme_sgw_tunnel_t * mme_sgw_tunnel= NULL;

    if (dataP != NULL) {
    	mme_sgw_tunnel = (mme_sgw_tunnel_t *)dataP;
        SPGW_APP_DEBUG("| %u\t<------------->\t%u\n", mme_sgw_tunnel->remote_teid, mme_sgw_tunnel->local_teid);
    } else {
        SPGW_APP_DEBUG("INVALID S11 TEID MAPPING FOUND\n");
    }
}
//-----------------------------------------------------------------------------
void sgw_lite_display_s11teid2mme_mappings(void)
//-----------------------------------------------------------------------------
{
    SPGW_APP_DEBUG("+--------------------------------------+\n");
    SPGW_APP_DEBUG("| MME <--- S11 TE ID MAPPINGS ---> SGW |\n");
    SPGW_APP_DEBUG("+--------------------------------------+\n");
    hashtable_apply_funct_on_elements(sgw_app.s11teid2mme_hashtable, sgw_lite_display_s11teid2mme_mapping, NULL);
    SPGW_APP_DEBUG("+--------------------------------------+\n");
}
//-----------------------------------------------------------------------------
static void sgw_lite_display_pdn_connection_sgw_eps_bearers(uint64_t keyP, void *dataP, void* unusedParameterP)
//-----------------------------------------------------------------------------
{
	sgw_eps_bearer_entry_t *eps_bearer_entry = NULL;

    if (dataP != NULL) {
        eps_bearer_entry = (sgw_eps_bearer_entry_t *)dataP;
        SPGW_APP_DEBUG("|\t\t\t\t%"PRId64"\t<-> ebi: %u, enb_teid_for_S1u: %u, s_gw_teid_for_S1u_S12_S4_up: %u (tbc)\n",
                keyP,
                eps_bearer_entry->eps_bearer_id,
                eps_bearer_entry->enb_teid_for_S1u,
                eps_bearer_entry->s_gw_teid_for_S1u_S12_S4_up);
    } else {
        SPGW_APP_DEBUG("\t\t\t\tINVALID eps_bearer_entry FOUND\n");
    }
}

//-----------------------------------------------------------------------------
static void sgw_lite_display_s11_bearer_context_information(uint64_t keyP, void *dataP, void* unusedParameterP)
//-----------------------------------------------------------------------------
{
    s_plus_p_gw_eps_bearer_context_information_t * sp_context_information= NULL;
    hashtable_rc_t                                   hash_rc;

    if (dataP != NULL) {
    	sp_context_information = (s_plus_p_gw_eps_bearer_context_information_t *)dataP;
        SPGW_APP_DEBUG("| KEY %"PRId64":      \n", keyP);
        SPGW_APP_DEBUG("|\tsgw_eps_bearer_context_information:     |\n");
        //Imsi_t               imsi;                           ///< IMSI (International Mobile Subscriber Identity) is the subscriber permanent identity.
        SPGW_APP_DEBUG("|\t\timsi_unauthenticated_indicator:\t%u\n", sp_context_information->sgw_eps_bearer_context_information.imsi_unauthenticated_indicator);
        //char                 msisdn[MSISDN_LENGTH];          ///< The basic MSISDN of the UE. The presence is dictated by its storage in the HSS.
        SPGW_APP_DEBUG("|\t\tmme_teid_for_S11:              \t%u\n", sp_context_information->sgw_eps_bearer_context_information.mme_teid_for_S11);
        //ip_address_t         mme_ip_address_for_S11;         ///< MME IP address the S11 interface.
        SPGW_APP_DEBUG("|\t\ts_gw_teid_for_S11_S4:          \t%u\n", sp_context_information->sgw_eps_bearer_context_information.s_gw_teid_for_S11_S4);
        //ip_address_t         s_gw_ip_address_for_S11_S4;     ///< S-GW IP address for the S11 interface and the S4 Interface (control plane).
        //cgi_t                last_known_cell_Id;             ///< This is the last location of the UE known by the network

        SPGW_APP_DEBUG("|\t\tpdn_connection:\n");
        SPGW_APP_DEBUG("|\t\t\tapn_in_use:        %s\n", sp_context_information->sgw_eps_bearer_context_information.pdn_connection.apn_in_use);
        SPGW_APP_DEBUG("|\t\t\tdefault_bearer:    %u\n", sp_context_information->sgw_eps_bearer_context_information.pdn_connection.default_bearer);
        SPGW_APP_DEBUG("|\t\t\teps_bearers:\n");

        hash_rc = hashtable_apply_funct_on_elements(sp_context_information->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers,
        		sgw_lite_display_pdn_connection_sgw_eps_bearers,
        		NULL);
        if (hash_rc != HASH_TABLE_OK) {
            SPGW_APP_DEBUG("Invalid sgw_eps_bearers hashtable for display\n");
        }

        //void                  *trxn;
        //uint32_t               peer_ip;
    } else {
        SPGW_APP_DEBUG("INVALID s_plus_p_gw_eps_bearer_context_information FOUND\n");
    }
}
//-----------------------------------------------------------------------------
void sgw_lite_display_s11_bearer_context_information_mapping(void)
//-----------------------------------------------------------------------------
{
    SPGW_APP_DEBUG("+-----------------------------------------+\n");
    SPGW_APP_DEBUG("| S11 BEARER CONTEXT INFORMATION MAPPINGS |\n");
    SPGW_APP_DEBUG("+-----------------------------------------+\n");
    hashtable_apply_funct_on_elements(sgw_app.s11_bearer_context_information_hashtable, sgw_lite_display_s11_bearer_context_information, NULL);
    SPGW_APP_DEBUG("+--------------------------------------+\n");
}
//-----------------------------------------------------------------------------
void pgw_lite_cm_free_apn(pgw_apn_t *apnP)
//-----------------------------------------------------------------------------
{
	if (apnP != NULL) {
		if (apnP->pdn_connections != NULL) {
		    obj_hashtable_destroy(apnP->pdn_connections);
		}
	}
}
//-----------------------------------------------------------------------------
Teid_t sgw_lite_get_new_S11_tunnel_id(void)
//-----------------------------------------------------------------------------
{
	// TO DO: RANDOM
	static Teid_t tunnel_id = 0;
	tunnel_id += 1;
	return tunnel_id;
}

//-----------------------------------------------------------------------------
mme_sgw_tunnel_t *sgw_lite_cm_create_s11_tunnel(Teid_t remote_teid, Teid_t local_teid)
//-----------------------------------------------------------------------------
{

    mme_sgw_tunnel_t *new_tunnel;

    new_tunnel = malloc(sizeof(mme_sgw_tunnel_t));

    if (new_tunnel == NULL) {
        /* Malloc failed, may be ENOMEM error */
        SPGW_APP_ERROR("Failed to create tunnel for remote_teid %u\n", remote_teid);
        return NULL;
    }

    new_tunnel->remote_teid = remote_teid;
    new_tunnel->local_teid  = local_teid;

    /* Trying to insert the new tunnel into the tree.
     * If collision_p is not NULL (0), it means tunnel is already present.
     */
    hashtable_insert(sgw_app.s11teid2mme_hashtable, local_teid, new_tunnel);

    return new_tunnel;
}

//-----------------------------------------------------------------------------
int sgw_lite_cm_remove_s11_tunnel(Teid_t local_teid)
//-----------------------------------------------------------------------------
{
    int  temp;

    temp = hashtable_remove(sgw_app.s11teid2mme_hashtable, local_teid);

    return temp;
}

//-----------------------------------------------------------------------------
sgw_eps_bearer_entry_t * sgw_lite_cm_create_eps_bearer_entry(void)
//-----------------------------------------------------------------------------
{
	sgw_eps_bearer_entry_t *eps_bearer_entry;

	eps_bearer_entry = malloc(sizeof(sgw_eps_bearer_entry_t));

    if (eps_bearer_entry == NULL) {
        /* Malloc failed, may be ENOMEM error */
        SPGW_APP_ERROR("Failed to create new EPS bearer object\n");
        return NULL;
    }
    return eps_bearer_entry;
}


//-----------------------------------------------------------------------------
sgw_pdn_connection_t * sgw_lite_cm_create_pdn_connection(void)
//-----------------------------------------------------------------------------
{
	sgw_pdn_connection_t *pdn_connection;

	pdn_connection = malloc(sizeof(sgw_pdn_connection_t));

    if (pdn_connection == NULL) {
        /* Malloc failed, may be ENOMEM error */
        SPGW_APP_ERROR("Failed to create new PDN connection object\n");
        return NULL;
    }
	memset(pdn_connection, 0, sizeof(sgw_pdn_connection_t));

    pdn_connection->sgw_eps_bearers = hashtable_create(12, NULL, NULL);
    if ( pdn_connection->sgw_eps_bearers == NULL) {
        SPGW_APP_ERROR("Failed to create eps bearers collection object\n");
        free(pdn_connection);
        pdn_connection = NULL;
        return NULL;
    }

    return pdn_connection;
}
//-----------------------------------------------------------------------------
void sgw_lite_cm_free_pdn_connection(sgw_pdn_connection_t *pdn_connectionP)
//-----------------------------------------------------------------------------
{
	if (pdn_connectionP != NULL) {
		if (pdn_connectionP->sgw_eps_bearers != NULL) {
		    hashtable_destroy(pdn_connectionP->sgw_eps_bearers);
		}
	}
}
//-----------------------------------------------------------------------------
void sgw_lite_cm_free_s_plus_p_gw_eps_bearer_context_information(s_plus_p_gw_eps_bearer_context_information_t *contextP)
//-----------------------------------------------------------------------------
{
	if (contextP == NULL) {
		return;
	}
	/*if (contextP->sgw_eps_bearer_context_information.pdn_connections != NULL) {
		obj_hashtable_destroy(contextP->sgw_eps_bearer_context_information.pdn_connections);
	}*/
	if (contextP->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers != NULL) {
	    hashtable_destroy(contextP->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers);
	}

	if (contextP->pgw_eps_bearer_context_information.apns != NULL) {
		obj_hashtable_destroy(contextP->pgw_eps_bearer_context_information.apns);
	}
    free(contextP);
}

//-----------------------------------------------------------------------------
s_plus_p_gw_eps_bearer_context_information_t * sgw_lite_cm_create_bearer_context_information_in_collection(Teid_t teid)
//-----------------------------------------------------------------------------
{
	s_plus_p_gw_eps_bearer_context_information_t *new_bearer_context_information;

	new_bearer_context_information = malloc(sizeof(s_plus_p_gw_eps_bearer_context_information_t));

    if (new_bearer_context_information == NULL) {
        /* Malloc failed, may be ENOMEM error */
        SPGW_APP_ERROR("Failed to create new bearer context information object for S11 remote_teid %u\n", teid);
        return NULL;
    }
	memset(new_bearer_context_information, 0, sizeof(s_plus_p_gw_eps_bearer_context_information_t));
    SPGW_APP_DEBUG("sgw_lite_cm_create_bearer_context_information_in_collection %d\n", teid);

    /*new_bearer_context_information->sgw_eps_bearer_context_information.pdn_connections = obj_hashtable_create(32, NULL, NULL, sgw_lite_cm_free_pdn_connection);

    if ( new_bearer_context_information->sgw_eps_bearer_context_information.pdn_connections == NULL) {
        SPGW_APP_ERROR("Failed to create PDN connections collection object entry for EPS bearer teid %u \n", teid);
        sgw_lite_cm_free_s_plus_p_gw_eps_bearer_context_information(new_bearer_context_information);
        return NULL;
    }*/

    new_bearer_context_information->pgw_eps_bearer_context_information.apns = obj_hashtable_create(32, NULL, NULL, pgw_lite_cm_free_apn);

    if ( new_bearer_context_information->pgw_eps_bearer_context_information.apns == NULL) {
        SPGW_APP_ERROR("Failed to create APN collection object entry for EPS bearer S11 teid %u \n", teid);
        sgw_lite_cm_free_s_plus_p_gw_eps_bearer_context_information(new_bearer_context_information);
        return NULL;
    }

    /* Trying to insert the new tunnel into the tree.
     * If collision_p is not NULL (0), it means tunnel is already present.
     */
    hashtable_insert(sgw_app.s11_bearer_context_information_hashtable, teid, new_bearer_context_information);
    SPGW_APP_DEBUG("Added new s_plus_p_gw_eps_bearer_context_information_t in s11_bearer_context_information_hashtable key teid %u\n", teid);

    return new_bearer_context_information;
}

int sgw_lite_cm_remove_bearer_context_information(Teid_t teid) {
	int temp;
    temp = hashtable_remove(sgw_app.s11_bearer_context_information_hashtable, teid);
    return temp;
}

//--- EPS Bearer Entry

//-----------------------------------------------------------------------------
sgw_eps_bearer_entry_t * sgw_lite_cm_create_eps_bearer_entry_in_collection(hash_table_t *eps_bearersP, ebi_t eps_bearer_idP)
//-----------------------------------------------------------------------------
{
	sgw_eps_bearer_entry_t *new_eps_bearer_entry;
	hashtable_rc_t            hash_rc = HASH_TABLE_OK;

    if (eps_bearersP == NULL) {
        SPGW_APP_ERROR("Failed to create EPS bearer entry for EPS bearer id %u. reason eps bearer hashtable is NULL \n", eps_bearer_idP);
        return NULL;
    }
	new_eps_bearer_entry = malloc(sizeof(sgw_eps_bearer_entry_t));

    if (new_eps_bearer_entry == NULL) {
        /* Malloc failed, may be ENOMEM error */
        SPGW_APP_ERROR("Failed to create EPS bearer entry for EPS bearer id %u \n", eps_bearer_idP);
        return NULL;
    }
	memset(new_eps_bearer_entry, 0, sizeof(sgw_eps_bearer_entry_t));

    new_eps_bearer_entry->eps_bearer_id = eps_bearer_idP;

    hash_rc = hashtable_insert(eps_bearersP, eps_bearer_idP, new_eps_bearer_entry);
    SPGW_APP_DEBUG("Inserted new EPS bearer entry for EPS bearer id %u status %s\n", eps_bearer_idP, hashtable_rc_code2string(hash_rc));

    hash_rc = hashtable_apply_funct_on_elements(eps_bearersP,
    		sgw_lite_display_pdn_connection_sgw_eps_bearers,
    		NULL);
    if (hash_rc != HASH_TABLE_OK) {
        SPGW_APP_DEBUG("Invalid sgw_eps_bearers hashtable for display\n");
    }
    /* CHECK DUPLICATES IN HASH TABLES ? if (temp == 1) {
        SPGW_APP_WARN("This EPS bearer entry already exists: %u\n", eps_bearer_idP);
        free(new_eps_bearer_entry);
        new_eps_bearer_entry = collision_p;
    }*/
    return new_eps_bearer_entry;
}
//-----------------------------------------------------------------------------
int sgw_lite_cm_remove_eps_bearer_entry(hash_table_t *eps_bearersP, ebi_t eps_bearer_idP)
//-----------------------------------------------------------------------------
{
    int temp;

    if (eps_bearersP == NULL) {
        return -1;
    }
    temp = hashtable_remove(eps_bearersP, eps_bearer_idP);

    return temp;
}


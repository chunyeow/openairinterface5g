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
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06410 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

/*!
*******************************************************************************

\file       pusu_msg.c

\brief      Fonctions permettant le formattage des donnees pour l'envoi d'un
            message sur le socket entre le RRM et PuSu (intra/inter Routing)

\author     BURLOT Pascal

\date       29/08/08

   
\par     Historique:
            $Author$  $Date$  $Revision$
            $Id$
            $Log$

*******************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

#include <pthread.h>

#include "L3_rrc_defs.h"
#include "rrm_sock.h"
#include "cmm_rrm_interface.h"
#include "pusu_msg.h"
#include "rrm_util.h"

#ifdef TRACE
//! Macro creant la chaine a partir du nom de la variable
#define STRINGIZER(x) #x
//! Tableau pour le mode trace faisant la translation entre le numero et le nom du message
const char *Str_msg_pusu_rrm[NB_MSG_RRM_PUSU] =  { 
    STRINGIZER(RRM_PUBLISH_IND      ) ,
    STRINGIZER(PUSU_PUBLISH_RESP    ) ,
    STRINGIZER(RRM_UNPUBLISH_IND    ) ,
    STRINGIZER(PUSU_UNPUBLISH_RESP  ) ,
    STRINGIZER(RRM_LINK_INFO_IND    ) ,
    STRINGIZER(PUSU_LINK_INFO_RESP  ) ,
    STRINGIZER(RRM_SENSING_INFO_IND ) ,
    STRINGIZER(PUSU_SENSING_NFO_RESP) ,
    STRINGIZER(RRM_CH_LOAD_IND      ) ,
    STRINGIZER(PUSU_CH_LOAD_RESP    )   
};
#endif

/*!
*******************************************************************************
\brief  This function initialize the message header 
\return any return value 
*/
static void init_pusu_msg_head( 
    msg_head_t     *msg_head , ///< message header to initialize 
    Instance_t      inst     , ///< Instance associated to this message
    MSG_RRM_PUSU_T  msg_type , ///< type of message to initialize
    unsigned int    size     , ///< size of message
    Transaction_t   Trans_id   ///< transaction id associated to this message
    ) 
{
    if ( msg_head != NULL )
    {
        msg_head->start    = START_MSG ; 
        msg_head->msg_type = 0xFF & msg_type ;
        msg_head->inst     = inst  ;
        msg_head->Trans_id = Trans_id  ;
        msg_head->size     = size ;
    }
}

/*!
*******************************************************************************
\brief  La fonction formate en un message d'annonce de nouveau service 
        envoyer sur le socket du PUSU.
\return message formate
*/
msg_t *msg_rrm_publish_ind(
    Instance_t      inst     , ///< Instance associated to this message
    int             service  , ///< service type
    Transaction_t   Trans_id   ///< transaction id associated to this message
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 

    if ( msg != NULL )
    {
        pusu_publish_t *p = RRM_CALLOC(pusu_publish_t , 1 );
        
        if ( p != NULL )
        {
            init_pusu_msg_head(&msg->head, inst, RRM_PUBLISH_IND, sizeof(pusu_publish_t),Trans_id);

            p->service_type     = service ;
        }       
        msg->data = (char *) p ;
    }
    
    return msg ;
}
/*!
*******************************************************************************
\brief  La fonction formate en un message de retrait d'un service 
        envoyer sur le socket du PUSU.
\return message formate
*/
msg_t *msg_rrm_unpublish_ind(
    Instance_t      inst     , ///< Instance associated to this message
    int             service  , ///< type de service a retirer
    Transaction_t   Trans_id   ///< transaction id associated to this message
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 

    if ( msg != NULL )
    {
        pusu_publish_t *p = RRM_CALLOC(pusu_publish_t , 1 );
        
        if ( p != NULL )
        {
            init_pusu_msg_head(&msg->head, inst, RRM_UNPUBLISH_IND, sizeof(pusu_unpublish_t),Trans_id);
        
            p->service_type     = service ;
        }       
        msg->data = (char *) p ;
    }
    
    return msg ;
}

/*!
*******************************************************************************
\brief  La fonction formate en un message de mesure a envoyer sur le socket du PUSU.
\return message formate
*/
msg_t *msg_rrm_link_info_ind(
    Instance_t      inst     , ///< Instance associated to this message
    L2_ID           noeud1   , ///< L2_ID du noeud 1
    L2_ID           noeud2   , ///< L2_ID du noeud 2
    RB_ID           rb_id    , ///< identtification du lien RB
    unsigned char   rssi     , ///< RSSI du noeud2 mesure  par le noeud1 
    unsigned char   spec_eff , ///< Efficacite spectrale du noeud2 mesure par le noeud1 
    Transaction_t   Trans_id   ///< transaction id associated to this message
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 

    if ( msg != NULL )
    {
        pusu_link_info_t *p = RRM_CALLOC(pusu_link_info_t , 1 );
        
        if ( p != NULL )
        {
            init_pusu_msg_head(&msg->head, inst, RRM_LINK_INFO_IND, sizeof(pusu_link_info_t),Trans_id);
            
            p->rssi             = rssi ;
            p->spec_eff         = spec_eff ;
            memcpy( &p->noeud1, &noeud1, sizeof(L2_ID) )  ;
            memcpy( &p->noeud2, &noeud2, sizeof(L2_ID) )  ;
        }
        msg->data = (char *) p ;
    }
    
    return msg ;
}

/*!
*******************************************************************************
\brief  La fonction formate en un message de mesure de voisnage a envoyer sur 
        le socket du PUSU.

\return message formate
*/
msg_t *msg_rrm_sensing_info_ind(
    Instance_t      inst     , ///< Instance associated to this message
    L2_ID           noeud1   , ///< L2_ID du noeud 1
    L2_ID           noeud2   , ///< L2_ID du noeud 2
    unsigned char   rssi     , ///< RSSI du noeud2 mesure  par le noeud1 
    Transaction_t   Trans_id   ///< transaction id associated to this message
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 

    if ( msg != NULL )
    {
        pusu_sensing_info_t *p = RRM_CALLOC(pusu_sensing_info_t , 1 );
        
        if ( p != NULL )
        {
            init_pusu_msg_head(&msg->head, inst, RRM_SENSING_INFO_IND, sizeof(pusu_sensing_info_t),Trans_id);
            
            p->rssi             = rssi ;
            memcpy( &p->noeud1, &noeud1, sizeof(L2_ID) )  ;
            memcpy( &p->noeud2, &noeud2, sizeof(L2_ID) )  ;
        }
        msg->data = (char *) p ;
    }
    
    return msg ;
}

/*!
*******************************************************************************
\brief  La fonction formate en un message de charge de cluster a envoyer sur 
        le socket du PUSU.
\return message formate
*/
msg_t *msg_rrm_ch_load_ind(
    Instance_t      inst     , ///< Instance associated to this message
    unsigned char   load     , ///< Charge du cluster 
    Transaction_t   Trans_id   ///< transaction id associated to this message
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 

    if ( msg != NULL )
    {
        pusu_ch_load_t *p = RRM_CALLOC(pusu_ch_load_t , 1 );
        
        if ( p != NULL )
        {
            init_pusu_msg_head(&msg->head, inst, RRM_CH_LOAD_IND, sizeof(pusu_ch_load_t),Trans_id);
            
            p->load             = load ;
        }
        msg->data = (char *) p ;
    }
    
    return msg ;
}

/*!
*******************************************************************************
\brief  La fonction formate en un message d'annonce de nouveau service 
        envoyer sur le socket du PUSU.
\return message formate
*/
msg_t *msg_pusu_resp(
    Instance_t      inst     , ///< Instance associated to this message
    MSG_RRM_PUSU_T response  , ///< identification de la reponse
    Transaction_t   Trans_id   ///< transaction id associated to this message
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 

    if ( msg != NULL )
    {
        // Header only
        init_pusu_msg_head(&msg->head, inst, response, 0,Trans_id);
        
        msg->data           = NULL ;
    }
    
    return msg ;
}


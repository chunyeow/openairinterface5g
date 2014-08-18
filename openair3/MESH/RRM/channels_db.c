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

\file       channels_db.c

\brief      Fonctions permettant la gestion de la base de donnée sur les canaux

\author     IACOBELLI Lorenzo

\date       16/10/09

   
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

#include "debug.h"
#include "L3_rrc_defs.h"
#include "rrm_util.h"
#include "sens_db.h"
#include "channels_db.h"


//! Selection locale du mode de debug
#define DBG_CHANNELS_DB 1

#if DBG_CHANNELS_DB==0
//! Macro inactive
#define PRINT_CHANNELS_DB(...) 
#else
//! Macro affichant  la liste des radio bearers
#define PRINT_CHANNELS_DB(...) print_channels_db( __VA_ARGS__ )
#endif 
    
/*!
*******************************************************************************
\brief  La fonction affiche a l'ecran la liste (pour du debug)
        
\return  aucune valeur retournee
*/
void print_channels_db( 
    CHANNELS_DB_T *pEntry ///< pointeur sur le debut de liste
    )
{
    //unsigned int i; //dbg
    CHANNELS_DB_T *pCurrentItem = pEntry;
    int i;
    
    //fprintf(stderr,"channels entry  @%p \n", pEntry);//dbg
    //fprintf(stderr,"sens list=[\n");
    while ( pCurrentItem != NULL)
    { 
        //fprintf(stderr,"curr channel %d is free? %d \n", pCurrentItem->channel.Ch_id,pCurrentItem->is_free);//dbg

        pCurrentItem = pCurrentItem->next ;
    }
    fprintf(stderr," ]\n");
}





/*!
*******************************************************************************
\brief  Comparaison de deux ID de niveau 2

\return si 0 alors les IDs sont identiques
*/
static int L2_ID_cmp(
    L2_ID *L2_id1, ///< ID de niveau 2
    L2_ID *L2_id2  ///< ID de niveau 2
    )
{
    return memcmp( L2_id1, L2_id2, sizeof(L2_ID) ) ;
}


/*!
*******************************************************************************
\brief  La fonction ajoute un nouveau canal.

\return  retourne la valeur du nouvel element
*/       
CHANNELS_DB_T *add_chann_db( 
        CHANNELS_DB_T **ch_entry  , ///< pointeur sur l'entree de la liste de noeuds
        CHANNEL_T channel         , ///< channel to insert in the list
        unsigned int is_free      , ///< Information about the availability of the channel //mod_lor_10_05_28 ->char instead of int
        double info_time            ///< Date of information update
     ) 
{
    //fprintf(stderr,"starting ch pointer  @%p \n", *ch_entry);//dbg
    //fprintf(stderr,"\n NB_info=%d \n", NB_info); //dbg
    CHANNELS_DB_T *pOldEntry = *ch_entry;
    CHANNELS_DB_T *pNewItem = RRM_CALLOC( CHANNELS_DB_T , 1 ) ;
    
    PNULL(pNewItem) ;
    if ( pNewItem == NULL ) 
        return NULL ;
        
    memcpy( &(pNewItem->channel) ,&channel, sizeof(CHANNEL_T) ) ;
    pNewItem->info_time =  info_time ;
    pNewItem->is_free   =  is_free;
    pNewItem->priority  =  0;
    pNewItem->is_ass    =  0;
    //fprintf(stderr,"starting ch pointer  @%p \n", pNewItem->info_hd);//dbg
    

    *ch_entry          =  pNewItem ;
    pNewItem->next     =  pOldEntry;   
    /*fprintf(stderr,"add_node\n\n");//dbg
    fprintf(stderr,"node entry  @%p \n", (*node_entry)->info_hd);//dbg
    fprintf(stderr,"node entry  @%p \n", pNewItem->info_hd);//dbg
    fprintf(stderr,"passed par  @%p \n", ch_info_hd);//dbg*/
    
    return pNewItem ;
}   
 
/*!
*******************************************************************************
\brief  La fonction detruit entierement les info sur les canaux memorizées à l'adresse indiqué 
        
\return  aucune valeur.
*/
void del_all_channels( 
    CHANNELS_DB_T **ch_entry  ///< pointeur sur l'entree de la liste 
    )
{
    CHANNELS_DB_T *pCurrentItem = *ch_entry;
    CHANNELS_DB_T *pNextItem    ;
    
    if ( (pCurrentItem == NULL)  || (ch_entry==NULL)) 
        return ;

    while ( pCurrentItem != NULL )
    { 
        pNextItem =  pCurrentItem->next ;
        RRM_FREE( pCurrentItem ) ;
        pCurrentItem = pNextItem ;
    }
    
    *ch_entry = NULL ;
}

/*!
*******************************************************************************
\brief  La fonction retire l'element identifie par le Ch_id de la liste des canaux 

\return  aucune valeur.
*/
void del_channel( 
    CHANNELS_DB_T **ch_entry  , ///< pointeur sur l'entree de la liste de noeuds
    unsigned int Ch_id          ///< channel ID 
    ) 
{
    CHANNELS_DB_T *pCurrentItem = *ch_entry;
    CHANNELS_DB_T *pNextItem    ;
    CHANNELS_DB_T **ppPrevItem  = ch_entry ;
    
    if ( (pCurrentItem == NULL)  || (ch_entry==NULL)) 
        return ;

    while ( pCurrentItem != NULL )
    { 
        pNextItem =  pCurrentItem->next ;
        if ( pCurrentItem->channel.Ch_id == Ch_id )
        { 
            *ppPrevItem = pNextItem;  
            
            RRM_FREE( pCurrentItem ) ;
            fprintf( stdout,"channel %d deleted from database\n", Ch_id);
            
            return ;
        }
        ppPrevItem   = &(pCurrentItem->next) ;
        pCurrentItem = pNextItem ;
    }
    
    if ( pCurrentItem == NULL )
        fprintf(stderr, "ERROR: channel_id unknown\n") ;
    
}



/*!
*******************************************************************************
\brief  La fonction recherche le descripteur du canal identifie par le Ch_id 
        dans la liste des canaux  
        
\return  pointeur sur le descripteur de canal trouve sinon NULL .
*/
CHANNELS_DB_T *get_chann_db_info( 
    CHANNELS_DB_T *ch_entry  , ///< pointeur sur l'entree de la liste de noeuds
    unsigned int Ch_id         ///< channel ID 
    )
{
    CHANNELS_DB_T *pCurrentItem = ch_entry;
    
    while ( pCurrentItem != NULL)
    { 
        if ( pCurrentItem->channel.Ch_id == Ch_id)
            break  ;
        
        pCurrentItem = pCurrentItem->next ;
    }
    
    return pCurrentItem ;
}

/*!
*******************************************************************************
\brief  La fonction ajoute ou update un canal.

\return  retourne la valeur du nouvel element
*/       
CHANNELS_DB_T *up_chann_db( 
        CHANNELS_DB_T **ch_entry  , ///< pointeur sur l'entree de la liste de noeuds
        CHANNEL_T channel         , ///< channel to insert in the list
        unsigned int is_free      , ///< Information about the availability of the channel //mod_lor_10_05_28 ->char instead of int
        double info_time            ///< Date of information update
     ) 
{
    if (ch_entry == NULL){
        fprintf(stderr,"ERROR! invalid database pointer \n"); 
        return(NULL);
    }
    CHANNELS_DB_T *pNewItem = get_chann_db_info( *ch_entry, channel.Ch_id );
    
    if ( pNewItem == NULL ){
        pNewItem = add_chann_db( ch_entry, channel, is_free, info_time);
        return pNewItem;
    }

    pNewItem->info_time =  info_time ;
    //mod_lor_10_05_17++
    pNewItem->is_free=is_free;
    /*if (is_free == pNewItem->is_free)
        return pNewItem;
    if (!(pNewItem->is_ass)){
        pNewItem->is_free=is_free;
        return pNewItem;
    }*/
    return pNewItem;
    //else 
    //    fprintf(stderr,"Primary User detected on channel %d in use by secondary network\n", channel.Ch_id);//mod_lor_10_05_12
     //mod_lor_10_05_17--
    //PRINT_CHANNELS_DB(*ch_entry); //dbg
    return NULL;
}

/*!
*******************************************************************************
\brief  La fonction mis à jour les canaux assignés.

\return  retourne la valeur du nouvel element
*/       
CHANNELS_DB_T *up_chann_ass(  
        CHANNELS_DB_T *ch_entry  , ///< pointeur sur l'entree de la liste de noeuds
        unsigned int Ch_id       , ///< ID of the channel to update
        unsigned int is_ass      , ///< Information about the use of the channel by the secondary network 
        L2_ID source_id          , ///< In case of channel assigned: address of the source node that is using it
        L2_ID dest_id              ///< In case of channel assigned: address of the destination node that is using it
     ) 
     
{
        //fprintf(stderr,"up_channels 1 inizio\n");//dbg
    CHANNELS_DB_T *pChannels = get_chann_db_info( ch_entry  , Ch_id );
    if (pChannels == NULL){
        fprintf(stderr,"ERROR! Channel %d not in database!\n", Ch_id);
        return (NULL);
    }
     //fprintf(stderr,"up_channels 2 trovato\n");//dbg 
    //fprintf(stderr,"up_channels 2b is_free: %d is_ass: %d\n",pChannels->is_free,pChannels->is_ass);//dbg 
    if (!(pChannels->is_free)){
        fprintf(stderr,"The channel %d is not free!\n", Ch_id);
        return (NULL); //mod_lor_10_05_17
    }else if (pChannels->is_ass == is_ass){
        //fprintf(stderr,"up_channels 3 is_ass\n");//dbg
        if (is_ass && L2_ID_cmp (&(source_id), &(pChannels->source_id))!=0)
            fprintf(stderr,"The channel %d is already assigned \n", Ch_id);
        //fprintf(stderr,"No updating performed for channel %d \n", Ch_id);//dbg

    }else {
        pChannels->is_ass = is_ass;
        //fprintf(stderr,"up_channels 4 is_ass\n");//dbg
        if (is_ass){
            //fprintf(stderr,"up_channels 5 is_ass\n");//dbg
            memcpy( pChannels->source_id.L2_id, source_id.L2_id, sizeof(L2_ID) )  ;
            memcpy( pChannels->dest_id.L2_id, dest_id.L2_id, sizeof(L2_ID) )  ;
        }
        //fprintf(stdout,"Channel %d assignement updated in %d\n", Ch_id, is_ass);//dbg
    }
    //fprintf(stderr,"up_channels 6 END\n");//dbg
    return (pChannels);
}

//mod_lor_10_05_17++
/*!
*******************************************************************************
\brief  The function select a new channel among the free ones. It selects the 
        first channel marked as free and not assigned in the channels database 
        and assigns it to the nodes whose L2_ids are passed as imput

\return  the pointer to selected channel entry; NULL if not available channels are found
*/ 
CHANNELS_DB_T *select_new_channel( 
        CHANNELS_DB_T *ch_entry  , ///< pointeur sur l'entree de la liste de noeuds
        L2_ID source_id          , ///< In case of channel assigned: address of the source node that is using it
        L2_ID dest_id              ///< In case of channel assigned: address of the destination node that is using it
     ) 
     
{
        
    CHANNELS_DB_T *pChannels = ch_entry;
    
    while ( pChannels != NULL)
    { 
        if ( pChannels->is_free && !(pChannels->is_ass) )
            break  ;
        
        pChannels = pChannels->next ;
    }
    
    
    if (pChannels == NULL){
        fprintf(stderr,"*SELECT NEW CHANNEL* -> No free channels available!\n");
        return (NULL);
    }
    else{
        pChannels->is_ass = 1;
        memcpy( pChannels->source_id.L2_id, source_id.L2_id, sizeof(L2_ID) )  ;
        memcpy( pChannels->dest_id.L2_id, dest_id.L2_id, sizeof(L2_ID) )  ;
    }
    return (pChannels);
}
//mod_lor_10_05_17--

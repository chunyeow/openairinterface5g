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

\file       sens_db.c

\brief      Fonctions permettant la gestion de la base de donnée sur les infor-
            mations du sensing 

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
#define DBG_SENS_DB 0

#if DBG_SENS_DB==0
//! Macro inactive
#define PRINT_SENS_DB(...) 
#else
//! Macro affichant  la liste des radio bearers
#define PRINT_SENS_DB(...) print_sens_db( __VA_ARGS__ )
#endif 
    
/*!
*******************************************************************************
\brief  La fonction affiche a l'ecran la liste (pour du debug)
        
\return  aucune valeur retournee
*/
void print_sens_db( 
    Sens_node_t *pEntry ///< pointeur sur le debut de liste
    )
{
    //unsigned int i; //dbg
    Sens_node_t *pCurrentItem = pEntry;
    Sens_ch_t *pCurrentChannel;
    int i,k;
    
    fprintf(stderr,"node entry  @%p \n", pEntry);//dbg
    //fprintf(stderr,"sens list=[\n");
    while ( pCurrentItem != NULL)
    { 
        //fprintf(stderr,"curr item  @%p \n", pCurrentItem);//dbg
        //fprintf(stderr,"  @%p(.info_time=%f, ",    
          //      pCurrentItem, pCurrentItem->info_time);
        
        //fprintf(stderr,", @SRC= \n"); 
        fprintf(stderr,"\nnode : ");
        for ( i=0;i<8;i++)
            fprintf(stderr,"%02X", pCurrentItem->L2_id.L2_id[i]);
       // fprintf(stderr,"\ntpc :%d     ",pCurrentItem->tpc);
       // fprintf(stderr,"overlap :%d      ",pCurrentItem->overlap);
       // fprintf(stderr,"sampl_freq :%d  \n",pCurrentItem->sampl_freq);
        pCurrentChannel=pCurrentItem->info_hd;
        while ( pCurrentChannel != NULL)
        {
            fprintf(stderr,"\n        @%p(\n.channel_id=%d, \n.start_fr=%d , \n.end_fr= %d:\n ",    
                pCurrentChannel, pCurrentChannel->Ch_id, pCurrentChannel->Start_f,pCurrentChannel->Final_f);  //mod_lor_10_03_17: intxflot
            for (k=0;k<NUM_SB;k++)
                //printf("%d %d; ",k,pCurrentChannel->is_free[k]); 
                printf ("k= %d, mu0=%d; mu1=%d; I0 = %d; is_free = %d\n",k,pCurrentChannel->mu0[k],pCurrentChannel->mu1[k],pCurrentChannel->I0[k], pCurrentChannel->is_free[k]);
            printf("\n\n");
            pCurrentChannel = pCurrentChannel->next ;
            
        }
            
        
                
        
        
        /*      
        fprintf(stderr,",\n   .Lchan_desc=%p(",&(pCurrentItem->Lchan_desc)); 
            fprintf(stderr,".transport_block_size=%d,",pCurrentItem->Lchan_desc.transport_block_size);
            fprintf(stderr,".max_transport_blocks=%d,",pCurrentItem->Lchan_desc.max_transport_blocks);
            fprintf(stderr,".Guaranteed_bit_rate=%ld,",pCurrentItem->Lchan_desc.Guaranteed_bit_rate);
            fprintf(stderr,".Max_bit_rate=%ld,",pCurrentItem->Lchan_desc.Max_bit_rate);
            fprintf(stderr,".Delay_class=%d,",pCurrentItem->Lchan_desc.Delay_class);
            fprintf(stderr,".Target_bler=%d,",pCurrentItem->Lchan_desc.Target_bler);
            fprintf(stderr,".LCHAN_t=%d),",pCurrentItem->Lchan_desc.LCHAN_t);
            
        fprintf(stderr,",\n   .Mac_rlc_meas_desc=%p(",&(pCurrentItem->Mac_rlc_meas_desc)); 
            fprintf(stderr,".Meas_trigger=%p(",&(pCurrentItem->Mac_rlc_meas_desc.Meas_trigger));
                fprintf(stderr,".Rssi=%d, ",pCurrentItem->Mac_rlc_meas_desc.Meas_trigger.Rssi);
                fprintf(stderr,".Sinr=%d, ",pCurrentItem->Mac_rlc_meas_desc.Meas_trigger.Sinr);
                fprintf(stderr,".Harq_delay=%u, ",pCurrentItem->Mac_rlc_meas_desc.Meas_trigger.Harq_delay);
                fprintf(stderr,".Bler=%u,",pCurrentItem->Mac_rlc_meas_desc.Meas_trigger.Bler);
                fprintf(stderr,".Spec_eff=%u), ",pCurrentItem->Mac_rlc_meas_desc.Meas_trigger.Spec_eff);
            fprintf(stderr,".Mac_avg=%p(",&(pCurrentItem->Mac_rlc_meas_desc.Mac_avg));
                fprintf(stderr,".Rssi_forgetting_factor=%u, ",pCurrentItem->Mac_rlc_meas_desc.Mac_avg.Rssi_forgetting_factor);
                fprintf(stderr,".Sinr_forgetting_factor=%u, ",pCurrentItem->Mac_rlc_meas_desc.Mac_avg.Sinr_forgetting_factor);
                fprintf(stderr,".Harq_delay_forgetting_factor=%u, ",pCurrentItem->Mac_rlc_meas_desc.Mac_avg.Harq_delay_forgetting_factor);
                fprintf(stderr,".Bler_forgetting_factor=%u, ",pCurrentItem->Mac_rlc_meas_desc.Mac_avg.Bler_forgetting_factor);
                fprintf(stderr,".Spec_eff_forgetting_factor=%u),\n",pCurrentItem->Mac_rlc_meas_desc.Mac_avg.Spec_eff_forgetting_factor);                
            fprintf(stderr,"   .bo_forgetting_factor=%u,",pCurrentItem->Mac_rlc_meas_desc.bo_forgetting_factor);
            fprintf(stderr,".sdu_loss_trigger=%u,",pCurrentItem->Mac_rlc_meas_desc.sdu_loss_trigger);
            fprintf(stderr,".Rep_amount=%u,",pCurrentItem->Mac_rlc_meas_desc.Rep_amount);
            fprintf(stderr,".Rep_interval=%d,",pCurrentItem->Mac_rlc_meas_desc.Rep_interval);
        */
            
        fprintf(stderr,"\n");
        
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
\brief  La fonction ajoute un noeud dans la liste des noeuds.

\return  retourne la valeur du nouvel element
*/

Sens_node_t *add_node( 
        Sens_node_t **node_entry  , ///< pointeur sur l'entree de la liste de noeuds
        L2_ID  *L2_id             , ///< node ID 
        unsigned int NB_info      , ///< Number of channel info
        Sens_ch_t *ch_info_hd     , ///< information received from the node
        double info_time
     ) 
{
    
    //fprintf(stderr,"\n NB_info=%d \n", NB_info); //dbg
    Sens_node_t *pOldEntry = *node_entry;
    Sens_node_t *pNewItem = RRM_CALLOC( Sens_node_t , 1 ) ;
    
    PNULL(pNewItem) ;
    if ( pNewItem == NULL ) 
        return NULL ;
    
    
    
    memcpy( pNewItem->L2_id.L2_id , L2_id->L2_id, sizeof(L2_ID) ) ;
    pNewItem->info_time =  info_time ;
    pNewItem->Nb_chan   =  NB_info;
    pNewItem->info_hd   =  NULL;
    //fprintf(stderr,"starting ch pointer  @%p \n", pNewItem->info_hd);//dbg
    for ( int i=0; i<NB_info; i++ ){
        if ( add_chann( &(pNewItem->info_hd), ch_info_hd[i].Start_f, 
                    ch_info_hd[i].Final_f, ch_info_hd[i].Ch_id, ch_info_hd[i].I0,ch_info_hd[i].mu0,ch_info_hd[i].mu1,
                    /*ch_info_hd[i].meas,*/ ch_info_hd[i].is_free) == NULL )
            fprintf(stderr, "ERROR: info %d problem\n", i) ;
        //fprintf(stderr,"returned pointer  @%p Channel id %d\n", pNewItem->info_hd, ch_info_hd[i].Ch_id);//dbg
    }

    *node_entry         =  pNewItem ;
    pNewItem->next      =  pOldEntry;   
    /*fprintf(stderr,"add_node\n\n");//dbg
    fprintf(stderr,"node entry  @%p \n", (*node_entry)->info_hd);//dbg
    fprintf(stderr,"node entry  @%p \n", pNewItem->info_hd);//dbg
    fprintf(stderr,"passed par  @%p \n", *node_entry);//dbg*/
    PRINT_SENS_DB( *node_entry );
    return pNewItem ;
}   
    
/*!
*******************************************************************************
\brief  La fonction ajoute un channel aux info relatives à un noeud.

\return  retourne la valeur du nouvel element
*/

Sens_ch_t *add_chann( 
        Sens_ch_t           **ch_entry , ///< pointeur sur l'entree de la liste de channels
        unsigned int        Start_f    , ///< frequence initial du canal //mod_lor_10_03_17: intxflot
        unsigned int        Final_f    , ///< frequence final du canal //mod_lor_10_03_17: intxflot
        unsigned int        Ch_id      , ///< Channel ID //mod_lor_10_03_17: intxflot
        //mod_eure_lor++
        char *I0 ,
        char *mu0,
        char *mu1,
        
        //float               meas       , ///< Sensing results 
        unsigned int        *is_free      ///< Evaluation about the availability of the channel //mod_lor_10_05_28 ->char instead of int
        //mod_eure_lor--
     ) 
{
    Sens_ch_t *pOldEntry = *ch_entry;
    Sens_ch_t *pNewItem = RRM_CALLOC( Sens_ch_t , 1 ) ;
    int i;
    
    /*fprintf(stderr,"\nadd_channel: %d\n", Ch_id);//dbg
    fprintf(stderr,"old header  @%p \n", pOldEntry);//dbg
    fprintf(stderr,"new pointer  @%p \n", pNewItem);//dbg*/
    
    PNULL(pNewItem) ;
    if ( pNewItem == NULL ) 
        return NULL ;

    pNewItem->Start_f =  Start_f ;
    pNewItem->Final_f =  Final_f ;
    pNewItem->Ch_id   =  Ch_id;
    //printf("sens_db add 1:  %d    ", pNewItem->Ch_id);//dbg
    //mod_eure_lor++
    memcpy(pNewItem->I0 ,I0, MAX_NUM_SB);
    memcpy(pNewItem->mu0,mu0, MAX_NUM_SB);
    memcpy(pNewItem->mu1,mu1, MAX_NUM_SB);
    memcpy(pNewItem->is_free,  is_free,MAX_NUM_SB*sizeof(unsigned int));//mod_lor_10_05_28 ->char instead of int
    //for (i=0; i<MAX_NUM_SB; i++)
    //        printf("up_chann: Channel %d sb %d is_free %d pitem is_free %d\n",Ch_id,i,is_free[i],pNewItem->is_free[i]);

    //mod_eure_lor--
    //pNewItem->meas    =  meas;
    //pNewItem->is_free =  is_free;
    //printf("sens_db add 2:  %d\n", pNewItem->Ch_id);//dbg
    *ch_entry         =  pNewItem ;
    pNewItem->next      =  pOldEntry;   
    //fprintf(stderr,"new pointer  @%p \n", *ch_entry);//dbg
    //fprintf(stderr,"new pointer  @%p \n", *ch_entry->next);//dbg
    //fprintf(stderr,"new pointer  @%p \n", pNewItem->next);//dbg
    
    
    return pNewItem ;   
}


/*!
*******************************************************************************
\brief  La fonction detruit entierement les info sur les canaux memorizées à l'adresse indiqué 
        
\return  aucune valeur.
*/
void del_channels_info( 
    Sens_ch_t **ch_info_entry  ///< pointeur sur l'entree des informations sur les canaux
    )
{
    Sens_ch_t *pCurrentItem = *ch_info_entry;
    Sens_ch_t *pNextItem    ;
    
    if ( (pCurrentItem == NULL)  || (ch_info_entry==NULL)) 
        return ;

    while ( pCurrentItem != NULL )
    { 
        pNextItem =  pCurrentItem->next ;
        RRM_FREE( pCurrentItem ) ;
        pCurrentItem = pNextItem ;
    }
    
    *ch_info_entry = NULL ;
}

/*!
*******************************************************************************
\brief  La fonction retire l'element identifie par le node_id de la liste des noeuds 

\return  aucune valeur.
*/
void del_node( 
    Sens_node_t **node_entry  , ///< pointeur sur l'entree de la liste de noeuds
    L2_ID       *L2_id          ///< node ID 
    ) 
{
    Sens_node_t *pCurrentItem = *node_entry;
    Sens_node_t *pNextItem    ;
    Sens_node_t **ppPrevItem  = node_entry ;
    
    if ( (pCurrentItem == NULL)  || (node_entry==NULL)) 
        return ;

    while ( pCurrentItem != NULL )
    { 
        pNextItem =  pCurrentItem->next ;
        if ( L2_ID_cmp( &(pCurrentItem->L2_id),L2_id) == 0 )
        { 
            *ppPrevItem = pNextItem;  
            del_channels_info  (&pCurrentItem->info_hd);
            RRM_FREE( pCurrentItem ) ;
            fprintf( stderr,"Noeud ");
            for ( int i=0;i<8;i++)
                msg_fct("%02X", L2_id->L2_id[i]);
            msg_fct( " deleted from database\n");
            
            return ;
        }
        ppPrevItem   = &(pCurrentItem->next) ;
        pCurrentItem = pNextItem ;
    }
    
    if ( pCurrentItem == NULL )
        fprintf(stderr, "ERROR: node_id unknown\n") ;
    
    //PRINT_SENS_DB( *node_entry );   
}

/*!
*******************************************************************************
\brief  La fonction detruit entierement la liste des noeuds 
        
\return  aucune valeur.
*/
void del_all_nodes( 
    Sens_node_t **node_entry    ///< pointeur sur l'entree de la liste de noeuds
    )
{
    Sens_node_t *pCurrentItem = *node_entry;
    Sens_node_t *pNextItem    ;
    
    if ( (pCurrentItem == NULL)  || (node_entry==NULL)) 
        return ;

    while ( pCurrentItem != NULL )
    { 
        pNextItem =  pCurrentItem->next ;
        del_channels_info  (&pCurrentItem->info_hd);
        RRM_FREE( pCurrentItem ) ;
        pCurrentItem = pNextItem ;
    }
    
    *node_entry = NULL ;
}

/*!
*******************************************************************************
\brief  La fonction recherche le descripteur de noeud identifie par le noeud_id 
        dans la liste des noeuds  
        
\return  pointeur sur le descripteur de noeud trouve sinon NULL .
*/
Sens_node_t *get_node_info( 
    Sens_node_t *node_entry , ///< pointeur sur l'entree de la liste des noeuds
    L2_ID       *L2_id        ///< Id du descripteur du noeud recherche
    )
{
    Sens_node_t *pCurrentItem = node_entry;
    
    while ( pCurrentItem != NULL)
    { 
        if ( L2_ID_cmp( &(pCurrentItem->L2_id),L2_id) == 0 )
            break ;
        
        pCurrentItem = pCurrentItem->next ;
    }
    
    return pCurrentItem ;
}

/*!
*******************************************************************************
\brief  La fonction recherche le descripteur de noeud identifie par le noeud_id 
        dans la liste des noeuds  
        
\return  pointeur sur le descripteur de noeud trouve sinon NULL .
*/
Sens_ch_t *get_chann_info( 
    Sens_ch_t *ch_entry ,   ///< pointeur sur l'entree de la liste des canaux
    unsigned int   Ch_id    ///< Id of the desired channel //mod_lor_10_03_17: intxflot
    )
{
    Sens_ch_t *pCurrentItem = ch_entry;
    
    while ( pCurrentItem != NULL)
    { 
        if ( pCurrentItem->Ch_id == Ch_id)
            break ;
        
        pCurrentItem = pCurrentItem->next ;
    }
    
    return pCurrentItem ;
}

/*!
*******************************************************************************
\brief  La fonction rafraichit  les information du sensing du noeud. Si c'est 
        la premiere fois qui le node envoye les info, il rajoute le node dans 
        la liste .
        
\return  pointer to the node
*/
Sens_node_t *update_node_info(
    Sens_node_t **node_entry, ///< pointeur sur un element de la liste des RB
    L2_ID *L2_id,             ///< id du noeud à refrechir
    unsigned int NB_info,     ///< Number of channel info
    Sens_ch_t *ch_info_hd,    ///< vector with the channel informations
    double info_time          ///< time au quel il vient mis à jour
    )
{
    Sens_node_t *pItem = get_node_info(*node_entry,L2_id);
    
       
    if ( pItem != NULL)
    { 
        //printf ("node existing\n\n");//dbg
        pItem->info_time   = info_time ;
        pItem->Nb_chan     = NB_info ;
        for ( int i=0; i<NB_info; i++ ){
           // printf ("node info: %d\n",i);//dbg
            if ( update_channel_info( &(pItem->info_hd), ch_info_hd[i].Start_f, 
                        ch_info_hd[i].Final_f, ch_info_hd[i].Ch_id,  ch_info_hd[i].I0,ch_info_hd[i].mu0,ch_info_hd[i].mu1,//ch_info_hd[i].meas, 
                        ch_info_hd[i].is_free) == NULL )
                fprintf(stderr, "ERROR: info %d problem\n", i) ;
        }
    }
    else {
        //fprintf(stderr,"DBG2 passed node entry @%p \n", node_entry);//dbg
        //fprintf(stderr,"DBG2 passed hd @%p \n", *node_entry); //dbg
        //printf ("adding node\n\n");//dbg
        pItem = add_node( node_entry, L2_id, NB_info, ch_info_hd, info_time);
    }
    
    //fprintf(stderr,"update_node\n");//dbg
    PRINT_SENS_DB( *node_entry );
    return pItem;
}

/*!
*******************************************************************************
\brief  La fonction rafraichit  les information relatives à la mesure d'un canal. Si c'est 
        la premiere fois qu'on receive des info à propos du canal, il rajoute 
        le canal.
        
\return   pointer to the channel
*/
Sens_ch_t *update_channel_info(
    Sens_ch_t           **ch_entry , ///< pointeur sur un element de la liste des RB
    unsigned int        Start_f    , ///< frequence initial du canal  //mod_lor_10_03_17: intxflot
    unsigned int        Final_f    , ///< frequence final du canal   //mod_lor_10_03_17: intxflot
    unsigned int        Ch_id      , ///< ID du canal   //mod_lor_10_03_17: intxflot
    //mod_eure_lor++
    char *I0 ,
    char *mu0,
    char *mu1,
    
    //float               meas       , ///< Sensing results
    unsigned int        *is_free      ///< Evaluation about the availability of the channel   //mod_lor_10_05_28 ->char instead of int
    //mod_eure_lor--
    )
{
    //printf("sens_db update passed ch_id: %d\n",Ch_id);//dbg
    
    Sens_ch_t *pItem = get_chann_info(*ch_entry,Ch_id);
    int i;
    
    if ( pItem != NULL)
    { 
       // printf ("channel existing\n\n");//dbg
      //  printf("sens_db update1: %d   ", pItem->Ch_id);//dbg
        //mod_eure_lor++
        memcpy( pItem->I0 ,I0, MAX_NUM_SB);
        memcpy( pItem->mu0,mu0, MAX_NUM_SB);
        memcpy( pItem->mu1,mu1, MAX_NUM_SB);
        memcpy(pItem->is_free, is_free, MAX_NUM_SB*sizeof(unsigned int));
     //pItem->meas        = meas ;
        //pItem->is_free     = is_free ;//mod_lor_10_05_06
         //for (i=0; i<NUM_SB;i++)
         //   printf ("sb %d is_free_db %d, is_free %d\n",i,pItem->is_free[i],is_free[i]);//dbg
       // printf("sens_db update2: %d\n", pItem->Ch_id);//dbg
    }
    else {
        //printf ("channel to add\n\n");//dbg
        //pItem = add_chann( ch_entry, Start_f, Final_f, Ch_id, meas, is_free);
        pItem = add_chann( ch_entry, Start_f, Final_f, Ch_id, I0, mu0,mu1,is_free);
    }
        //mod_eure_lor--
    
    return pItem;
}

//mod_lor_10_02_19++
/*!
*******************************************************************************
\brief  La fonction rafraichit les parametres du sensing du noeud et, eventuelement,
        les misurations. Si c'est la premiere fois qui le node envoye les info, 
        il rajoute le node dans la liste .
        
\return  pointer to the node
*/
Sens_node_t *update_node_par(
    Sens_node_t      **node_entry  , ///< pointeur sur un element de la liste des RB
    L2_ID                *L2_id    , ///< id du noeud à refrechir
    unsigned int         NB_info   , ///< Number of channel info
    Sens_ch_t           *ch_info_hd, ///< vector with the channel informations
    double               info_time , ///< time au quel il vient mis à jour
    unsigned int         tpc       , ///< time on each carrier           
	unsigned int         overlap   , ///< overlap factor (percentage)    
	unsigned int         sampl_freq  ///< sampling frequency (Ms/s)      
    )
{
    Sens_node_t *pItem = update_node_info(node_entry,L2_id,NB_info,ch_info_hd,info_time);
    
    pItem->tpc = tpc;
    pItem->overlap = overlap;
    pItem->sampl_freq = sampl_freq;
    
    //fprintf(stderr,"update_node\n");//dbg
    PRINT_SENS_DB( *node_entry );
    return pItem;
}
//mod_lor_10_02_19--

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

\file       rb_db.c

\brief      Fonctions permettant la gestion de la base de donnée sur les radio-
            bearers du cluster par le cluster head

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

#include "debug.h"
#include "L3_rrc_defs.h"
#include "rrm_util.h"
#include "rb_db.h"

//! Selection locale du mode de debug
#define DBG_RB_DB 0

#if DBG_RB_DB==0
//! Macro inactive
#define PRINT_RB_DB(...) 
#else
//! Macro affichant  la liste des radio bearers
#define PRINT_RB_DB(...) print_rb_db( __VA_ARGS__ )
#endif 
    
/*!
*******************************************************************************
\brief  La fonction affiche a l'ecran la liste (pour du debug)
        
\return  aucune valeur retournee
*/
static void print_rb_db( 
    RB_desc_t *pEntry ///< pointeur sur le debut de liste
    )
{
    unsigned int i ;
    RB_desc_t *pCurrentItem = pEntry;
    
    fprintf(stderr,"RB list=[\n");
    while ( pCurrentItem != NULL)
    { 
        fprintf(stderr,"  @%p(.Trans_id=%d, .Rb_id=%d, .Rb_type=%d",    
                pCurrentItem, pCurrentItem->Trans_id, 
                pCurrentItem->Rb_id,  pCurrentItem->RB_type);
        
        fprintf(stderr,", @SRC="); 
        for ( i=0;i<8;i++)
            fprintf(stderr,"%02X", pCurrentItem->L2_id[0].L2_id[i]);
            
        fprintf(stderr,", @DST="); 
        for ( i=0;i<8;i++)
            fprintf(stderr,"%02X", pCurrentItem->L2_id[1].L2_id[i]);
                
        fprintf(stderr,", .QoS_class=%d", pCurrentItem->QoS_class );
        
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
            
        fprintf(stderr,", .next=%p)\n", pCurrentItem->next );
        
        pCurrentItem = pCurrentItem->next ;
    }
    fprintf(stderr," ]\n");
}

/*!
*******************************************************************************
\brief  La fonction ajoute un rb dans la liste des RB ouverts du Cluster head.

\return  retourne la valeur du nouvel element
*/
RB_desc_t *add_rb( 
        RB_desc_t     **rb_entry  , ///< pointeur sur l'entree de la liste des RB
        Transaction_t   Trans_id  , ///< transaction associee
        QOS_CLASS_T     QoS_class , ///< index de la classe de QoS
        L2_ID          *L2_id       ///< ID de niveau 2  des noeuds associes au RB
     ) 
{
    RB_desc_t *pOldEntry = *rb_entry;
    RB_desc_t *pNewItem = RRM_CALLOC( RB_desc_t , 1 ) ;
    
    PNULL(pNewItem) ;
    if ( pNewItem == NULL ) 
        return NULL ;

    pNewItem->QoS_class  =  QoS_class   ;
    
    memcpy( &(pNewItem->L2_id[0]) , L2_id, 2*sizeof(L2_ID) ) ;
    
    pNewItem->Trans_id  =  Trans_id ;
    pNewItem->Rb_id     =  0        ;
    pNewItem->RB_type   =  0;

    *rb_entry           =  pNewItem ;
    pNewItem->next      =  pOldEntry;   
    
    PRINT_RB_DB( *rb_entry );
    return pNewItem ;   
}

/*!
*******************************************************************************
\brief  La fonction retire l'element identifie par le RB_id de la liste des RB 

\return  aucune valeur.
*/
void del_rb_by_rbid( 
    RB_desc_t **rb_entry , ///< pointeur sur l'entree de la liste des RB
    RB_ID       Rb_id      ///< ID du RB a detruire
    ) 
{
    RB_desc_t *pCurrentItem = *rb_entry;
    RB_desc_t *pNextItem    ;
    RB_desc_t **ppPrevItem  = rb_entry ;
    
    if ( (pCurrentItem == NULL)  || (rb_entry==NULL)) 
        return ;

    while ( pCurrentItem != NULL )
    { 
        pNextItem =  pCurrentItem->next ;
        if ( Rb_id == pCurrentItem->Rb_id )
        { 
            *ppPrevItem = pNextItem;    
            RRM_FREE( pCurrentItem ) ;
            PRINT_RB_DB( *rb_entry );
            return ;
        }
        ppPrevItem   = &(pCurrentItem->next) ;
        pCurrentItem = pNextItem ;
    }
    
    if ( pCurrentItem == NULL )
        fprintf(stderr, "ERROR: RB_id (%d) unknown\n", Rb_id ) ;
    
    PRINT_RB_DB( *rb_entry );   
}

/*!
*******************************************************************************
\brief  La fonction retire l'element identifie par le Trans_id de la liste des RB 

\return  aucune valeur.
*/
void del_rb_by_transid( 
    RB_desc_t     **rb_entry , ///< pointeur sur l'entree de la liste des RB
    Transaction_t   Trans_id   ///< transaction identifiant le RB
    )
{
    RB_desc_t *pCurrentItem = *rb_entry;
    RB_desc_t *pNextItem    ;
    RB_desc_t **ppPrevItem  = rb_entry ;
    
    if ( (pCurrentItem == NULL)  || (rb_entry==NULL)) 
        return ;

    while ( pCurrentItem != NULL )
    { 
        pNextItem =  pCurrentItem->next ;
        if ( Trans_id == pCurrentItem->Trans_id )
        { 
            *ppPrevItem = pNextItem;    
            RRM_FREE( pCurrentItem ) ;
            PRINT_RB_DB( *rb_entry );
            return ;
        }
        ppPrevItem   = &(pCurrentItem->next) ;
        pCurrentItem = pNextItem ;
    }
    
    PRINT_RB_DB( *rb_entry );   
}

/*!
*******************************************************************************
\brief  La fonction detruit entierement la liste des RB 
        
\return  aucune valeur.
*/
void del_all_rb( 
    RB_desc_t **rb_entry ///< pointeur sur l'entree de la liste des RB
    )
{
    RB_desc_t *pCurrentItem = *rb_entry;
    RB_desc_t *pNextItem    ;
    
    if ( (pCurrentItem == NULL)  || (rb_entry==NULL)) 
        return ;

    while ( pCurrentItem != NULL )
    { 
        pNextItem =  pCurrentItem->next ;
        RRM_FREE( pCurrentItem ) ;
        pCurrentItem = pNextItem ;
    }
    
    *rb_entry = NULL ;
}

/*!
*******************************************************************************
\brief  La fonction recherche le descripteur de RB identifie par le RB_id 
        la liste des RBs 
        
\return  pointeur sur le descripteur de RB trouve sinon NULL .
*/
RB_desc_t *get_rb_desc_by_rbid( 
    RB_desc_t *rb_entry , ///< pointeur sur l'entree de la liste des RB
    RB_ID      Rb_id      ///< Id du descripteur de RB recherchee
    )
{
    RB_desc_t *pCurrentItem = rb_entry;
    
    while ( pCurrentItem != NULL)
    { 
        if ( Rb_id == pCurrentItem->Rb_id )
            break ;
        
        pCurrentItem = pCurrentItem->next ;
    }
    
    return pCurrentItem ;
}

/*!
*******************************************************************************
\brief  La fonction recherche le descripteur de RB identifie par le Trans_id 
        la liste des RBs 
        
\return  pointeur sur le descripteur de RB trouve sinon NULL .
*/
RB_desc_t *get_rb_desc_by_transid(
    RB_desc_t    *rb_entry , ///< pointeur sur l'entree de la liste des RB
    Transaction_t Trans_id   ///< transaction du descripteur de RB recherchee
    )
{
    RB_desc_t *pCurrentItem = rb_entry;
    
    while ( pCurrentItem != NULL)
    { 
        if ( Trans_id == pCurrentItem->Trans_id )
            break ;
        
        pCurrentItem = pCurrentItem->next ;
    }
    
    return pCurrentItem ;
}

/*!
*******************************************************************************
\brief  La fonction rafraichit  les champs Rb_id, RB_type d'un descripteur 
            de RB identifie par le Trans_id la liste des RBs .
        
\return  aucune valeur
*/
void update_rb_desc(
    RB_desc_t     *rb_entry , ///< pointeur sur un element de la liste des RB
    Transaction_t  Trans_id , ///< transaction id 
    RB_ID          Rb_id    , ///< RB id
    RB_TYPE        RB_type    ///< type de RB
    )
{
    RB_desc_t *pItem = get_rb_desc_by_transid(rb_entry,Trans_id);
    
    if ( pItem != NULL)
    { 
        pItem->Trans_id= -1 ;
        pItem->Rb_id   = Rb_id ;
        pItem->RB_type = RB_type ;
    }
    PRINT_RB_DB( rb_entry );
}

/*!
*******************************************************************************
\brief  La fonction rafraichit  le champs Rb_meas d'un RB identifie dans la 
        liste des RBs .
        
\return  aucune valeur
*/
void update_rb_meas(
    RB_desc_t      *rb_entry   , ///< pointeur sur un element de la liste des RB
    RB_ID           Rb_id      , ///< RB id
    L2_ID          *L2_id      , ///< ID de niveau 2 d'un des noeuds du RB 
    MAC_RLC_MEAS_T *Rb_meas      ///< mesure RB effectue par le noeud
    )
{
    RB_desc_t *pItem = get_rb_desc_by_rbid(rb_entry,Rb_id);
    
    if ( pItem != NULL)
    {  
        if ( memcmp(L2_id, &pItem->L2_id[0], sizeof(L2_ID) ) == 0 ) 
        {
            memcpy( &pItem->Rb_meas[0],Rb_meas, sizeof(MAC_RLC_MEAS_T) ) ;
        }
        else
        if ( memcmp(L2_id, &pItem->L2_id[1], sizeof(L2_ID) ) == 0 ) 
        {
            memcpy( &pItem->Rb_meas[1],Rb_meas, sizeof(MAC_RLC_MEAS_T) ) ;      
        }
    }

}

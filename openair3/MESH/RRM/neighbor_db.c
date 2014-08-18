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

\file       neighbor_db.c

\brief      Fonctions permettant la gestion de la base de donnée sur le voisinage
            directe (1 saut) du cluster head et indirecte a 2 sauts.

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
#include "rrm_sock.h"
#include "rrc_rrm_msg.h"
#include "rrm_util.h"
#include "neighbor_db.h"

//! Selection locale du mode de debug
#define DBG_NEIGHBOR_DB 0

#if DBG_NEIGHBOR_DB==0
//! Macro inactive
#define PRINT_NEIGHBOR_DB(...) 
#else
//! Macro affichant  la liste du voisnage d'un cluster
#define PRINT_NEIGHBOR_DB(...) print_neighbor_db( __VA_ARGS__ )
#endif 

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
\brief  La fonction affiche a l'ecran la liste (pour DEBUG )
        
\return  aucune valeur retournee
*/
static void print_neighbor_db( 
    neighbor_desc_t *pEntry ///< pointeur sur le debut de liste
    )
{
    unsigned int i , j ;
    neighbor_desc_t *pCurrentItem = pEntry;
    
    fprintf(stderr,"neighbor list=[\n");
    while ( pCurrentItem != NULL)
    { 
        fprintf(stderr,"  @%p(.L2_id=", pCurrentItem );
        
        for ( i=0;i<8;i++)
            fprintf(stderr,"%02X", pCurrentItem->L2_id.L2_id[i]);
            
        fprintf(stderr,", .NB_neighbor=%u, .Sensing_meas=%p, .next=%p)\n",
                pCurrentItem->NB_neighbor, pCurrentItem->Sensing_meas,pCurrentItem->next );
        
        if ( pCurrentItem->Sensing_meas != NULL ) 
            for ( j=0;j<pCurrentItem->NB_neighbor;j++)
            {
                fprintf(stderr,"\tNeighbor(%d):.L2_id=",j);
                for ( i=0;i<8;i++)
                    fprintf(stderr,"%02X", pCurrentItem->Sensing_meas[j].L2_id.L2_id[i]);
                    
                fprintf(stderr," .RSSI=%02X\n", pCurrentItem->Sensing_meas[j].Rssi);
            }
        pCurrentItem = pCurrentItem->next ;
    }
    fprintf(stderr," ]\n");
}

/*!
*******************************************************************************
\brief  La fonction ajoute un voisin dans la liste des voisins du Cluster head.

\return  retourne la valeur du nouvel element
*/
neighbor_desc_t *add_neighbor( 
    neighbor_desc_t **neighbor_entry, ///< pointeur sur l'entree de la liste de voisinage
    L2_ID *L2_id                      ///< ID du nouveau noeud
    )
{
    neighbor_desc_t *pNewItem = get_neighbor( *neighbor_entry, L2_id );
    
    if ( pNewItem == NULL ) 
    { // If not Exist then create a new item
        neighbor_desc_t *pOldEntry = *neighbor_entry;
        
        pNewItem = RRM_MALLOC(neighbor_desc_t , 1 ) ;
        
        PNULL(pNewItem) ;
        if ( pNewItem == NULL ) 
            return NULL ;
        
        memcpy( pNewItem->L2_id.L2_id , L2_id->L2_id, sizeof(L2_ID) ) ;
        //pNewItem->RSSI      =  0   ;
        pNewItem->NB_neighbor =  0   ;
        pNewItem->Sensing_meas=  NULL;

        *neighbor_entry     =  pNewItem ;
        pNewItem->next      =  pOldEntry;   
    }
    
    PRINT_NEIGHBOR_DB( *neighbor_entry );
    return pNewItem ;
}

/*!
*******************************************************************************
\brief  La fonction retire un element de la liste des voisins 
        
\return  aucune valeur.
*/
void del_neighbor( 
    neighbor_desc_t **neighbor_entry, ///< pointeur sur l'entree de la liste de voisinage
    L2_ID            *L2_id           ///< ID niveau 2 du voisin a detruire
    ) 
{
    neighbor_desc_t *pCurrentItem = *neighbor_entry;
    neighbor_desc_t *pNextItem  ;
    neighbor_desc_t **ppPrevItem  = neighbor_entry ;
    
    if ( (pCurrentItem == NULL)  || (neighbor_entry==NULL)) 
        return ;

    while ( pCurrentItem != NULL )
    { 
        pNextItem =  pCurrentItem->next ;
        if ( L2_ID_cmp( &(pCurrentItem->L2_id),L2_id) == 0 )
        { 
            *ppPrevItem = pNextItem;
            
            RRM_FREE( pCurrentItem->Sensing_meas) ;             
            RRM_FREE( pCurrentItem ) ;
            break ;
        }
        ppPrevItem   = &(pCurrentItem->next) ;
        pCurrentItem = pNextItem ;
    }
    
    PRINT_NEIGHBOR_DB( *neighbor_entry );   
}

/*!
*******************************************************************************
\brief  La fonction detruit entierement la liste des voisins 
        
\return aucune valeur.
*/
void del_all_neighbor( 
    neighbor_desc_t **neighbor_entry ///< pointeur sur l'entree de la liste de voisinage
    ) 
{
    neighbor_desc_t *pCurrentItem = *neighbor_entry;
    neighbor_desc_t *pNextItem  ;
    
    if ( (pCurrentItem == NULL)  || (neighbor_entry==NULL)) 
        return ;

    while ( pCurrentItem != NULL )
    { 
        pNextItem =  pCurrentItem->next ;
        
        RRM_FREE( pCurrentItem->Sensing_meas ) ;
        RRM_FREE( pCurrentItem ) ;

        pCurrentItem = pNextItem ;
    }
    *neighbor_entry = NULL ;
}

/*!
*******************************************************************************
\brief  La fonction recherche les informations de voisinage d'un noeud 
        
\return descripteur sur les infos de voisinage du noeud ou NULL
*/
neighbor_desc_t *get_neighbor( 
    neighbor_desc_t *neighbor_entry, ///< pointeur sur l'entree de la liste de voisinage
    L2_ID *L2_id                     ///< ID du noeud
    ) 
{
    neighbor_desc_t *pCurrentItem = neighbor_entry;
    
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
\brief  La fonction met a jour les informations de mesure de voisinage d'un noeud 
        
\return aucune valeur
*/
void set_Sensing_meas_neighbor( 
    neighbor_desc_t *neighbor_entry, ///< pointeur sur l'entree de la liste de voisinage
    L2_ID           *L2_id         , ///< ID du noeud
    unsigned int     nb_meas       , ///< Nombre de mesure associe au noeud
    SENSING_MEAS_T *sensing_meas     ///< Mesures de voisinage
    ) 
{
    neighbor_desc_t *pCurrentItem = neighbor_entry;
    
    while ( pCurrentItem != NULL)
    { 
        if ( L2_ID_cmp( &(pCurrentItem->L2_id),L2_id) == 0 )
        {
            pCurrentItem->NB_neighbor = 0 ;
            
            RRM_FREE(pCurrentItem->Sensing_meas );
                
            if (  nb_meas > 0 )
            {
                SENSING_MEAS_T *p   = RRM_MALLOC(SENSING_MEAS_T, nb_meas ) ;
                
                if ( p != NULL )
                {
                    int size = nb_meas * sizeof(SENSING_MEAS_T) ;
                    memcpy( p,sensing_meas , size );
                    pCurrentItem->NB_neighbor  = nb_meas ;
                    pCurrentItem->Sensing_meas = p ;
                }
            }
            break ;
        }
        
        pCurrentItem = pCurrentItem->next ;
    }
    
    if (pCurrentItem==NULL)
    {
        fprintf(stderr,"ERROR: Unknown MR L2_id  :");
        print_L2_id(L2_id) ;
        fprintf(stderr,"\n");
    }
    
    PRINT_NEIGHBOR_DB( neighbor_entry );    
}

/*!
*******************************************************************************
\brief  La fonction lit l'information de RSSI entre un noeud et un noeud voisin
        
\return la valeur de RSSI
*/
unsigned char get_RSSI_neighbor(
    neighbor_desc_t *neighbor_entry , ///< pointeur sur l'entree de la liste de voisinage
    L2_ID           *L2_id          , ///< ID du noeud
    L2_ID           *neighbor_id      ///< ID du noeud voisin
    )
{
    neighbor_desc_t *pCurrentItem = neighbor_entry;
    unsigned char   rssi = 0xFF ;
    int i ;
    
    while ( pCurrentItem != NULL)
    { 
        if ( L2_ID_cmp( &(pCurrentItem->L2_id),L2_id) == 0 )
        {
            for ( i = 0 ; i < pCurrentItem->NB_neighbor ; i++ )
            {
                if ( L2_ID_cmp( &(pCurrentItem->Sensing_meas[i].L2_id),neighbor_id) == 0 )
                {
                    rssi = pCurrentItem->Sensing_meas[i].Rssi ;
                    return (rssi) ;
                }
            }
        }       
        pCurrentItem = pCurrentItem->next ;
    }
    
    return rssi ;
}

/*!
*******************************************************************************
\brief  La fonction ajoute un couple de voisin a la liste de type neighbor_list_t
        
\return le nouvel element de liste
*/
static neighbor_list_t *add_item_neighbor_list(
    neighbor_list_t **neighbor_list_entry , ///< pointeur sur l'entree de la liste des couples de voisins
    L2_ID            *L2_id1              , ///< ID du noeud 1
    L2_ID             *L2_id2               ///< ID du noeud 2
    )
{
    neighbor_list_t *pOldEntry = *neighbor_list_entry;
    
    neighbor_list_t *pNewItem = RRM_MALLOC(neighbor_list_t, 1 ) ;
    
    PNULL(pNewItem) ;
    if ( pNewItem == NULL ) 
        return NULL ;
    
    memcpy( &(pNewItem->L2_id[0]) , L2_id1, sizeof(L2_ID) ) ;
    memcpy( &(pNewItem->L2_id[1]) , L2_id2, sizeof(L2_ID) ) ;
    pNewItem->Rssi[0]       =  0xFF      ;
    pNewItem->Rssi[1]       =  0xFF      ;
    pNewItem->nb_opened_rb  =  0   ;
    pNewItem->rb_meas       =  NULL;

    *neighbor_list_entry    =  pNewItem ;
    pNewItem->next          =  pOldEntry;   
    
    return pNewItem ;
}

/*!
*******************************************************************************
\brief  La fonction touve un couple de voisin dans la liste de type neighbor_list_t
        
\return l'element trouve 
*/
static neighbor_list_t *get_item_neighbor_list( 
    neighbor_list_t *neighbor_list_entry, ///< pointeur sur l'entree de la liste des couples de voisins
    L2_ID           *L2_id1             , ///< ID du noeud 1
    L2_ID           *L2_id2               ///< ID du noeud 2
    ) 
{
    neighbor_list_t *pCurrentItem = neighbor_list_entry;
    L2_ID L2_id[2] ;
    
    memcpy( &(L2_id[0]), L2_id1, sizeof(L2_ID) ) ;
    memcpy( &(L2_id[1]), L2_id2, sizeof(L2_ID) ) ;
    
    while ( pCurrentItem != NULL)
    { 
        if ( memcmp( &(pCurrentItem->L2_id),L2_id,2*sizeof(L2_ID)) == 0 )
            break ;
        pCurrentItem = pCurrentItem->next ;
    }
    
    return pCurrentItem ;
}

/*!
*******************************************************************************
\brief  La fonction cree la liste de type neighbor_list_t a partie d'une liste
        de type neighbor_desc_t:
        - Elle construit la liste des MR en vue directe du CH (voisinage directe)
        - Elle construit la liste des MR voyant d'autre MR (voisinage a 2 sauts)

\return retourne la liste de type neighbor_list_t.
*/
neighbor_list_t *create_neighbor_list( 
    neighbor_desc_t *neighbor_entry , ///< pointeur sur l'entree de la liste de voisinage
    L2_ID           *L2_id_CH         ///< ID du Cluster Head
    ) 
{
    neighbor_desc_t *pCurrentItem = neighbor_entry;
    neighbor_list_t *pList=NULL ;
    
    // construit la liste des MR voyant le CH en directe (voisinage directe)
    while ( pCurrentItem != NULL)
    { 
        add_item_neighbor_list( &pList, L2_id_CH, &(pCurrentItem->L2_id) ) ;
        pCurrentItem = pCurrentItem->next ;
    }
    
    // construit la liste des MR voyant d'autre MR (voisinage a 2 sauts) 
    pCurrentItem = neighbor_entry;
    while ( pCurrentItem != NULL)
    { 
        int i ;
        for ( i = 0 ; i<pCurrentItem->NB_neighbor; i++ )
        {
            neighbor_list_t *pItemFound ;
            // Recherche du couple de voisin dans la liste
            pItemFound = get_item_neighbor_list(pList ,
                                        &(pCurrentItem->L2_id) , 
                                        &(pCurrentItem->Sensing_meas[i].L2_id)  
                                        );

            if ( pItemFound == NULL )
            { // si le couple n'est pas trouve alors on inverse les adresses MAC
                pItemFound = get_item_neighbor_list(pList ,
                                            &(pCurrentItem->Sensing_meas[i].L2_id) , 
                                            &(pCurrentItem->L2_id)
                                            );
                if ( pItemFound == NULL )
                { // si ce couple n'est pas dans la liste alors on l'y ajoute
                    pItemFound = add_item_neighbor_list( &pList, &(pCurrentItem->L2_id), &(pCurrentItem->Sensing_meas[i].L2_id)) ;
                    pItemFound->Rssi[1] = pCurrentItem->Sensing_meas[i].Rssi ;
                }
            }
            else
            { // Met a jour le champs Rssi
                pItemFound->Rssi[0] = pCurrentItem->Sensing_meas[i].Rssi ;
            }
        }
        pCurrentItem = pCurrentItem->next ;     
    }   
    return pList ;
}

/*!
*******************************************************************************
\brief  La fonction detruit la liste de type neighbor_list_t .

\return Aucune valeur
*/
void del_neighbor_list( 
    neighbor_list_t **neighbor_list_entry ///< pointeur sur l'entree de la liste de voisinage 
    )
{
    neighbor_list_t *pCurrentItem = *neighbor_list_entry;
    neighbor_list_t *pNextItem  ;
    
    if ( (pCurrentItem == NULL)  || (neighbor_list_entry==NULL)) 
        return ;

    while ( pCurrentItem != NULL )
    { 
        pNextItem =  pCurrentItem->next ;
            
        RRM_FREE( pCurrentItem->rb_meas ) ;
        RRM_FREE( pCurrentItem ) ;

        pCurrentItem = pNextItem ;
    }
    *neighbor_list_entry = NULL ;
}

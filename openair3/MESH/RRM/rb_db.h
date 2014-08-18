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

\file       rb_db.h

\brief      Fichier d'entete contenant les declarations des types, des defines ,
            et des fonctions relatives a la gestion de la base de donnée sur 
            les radio bearers par le cluster head.

\author     BURLOT Pascal

\date       29/08/08

   
\par     Historique:
            $Author$  $Date$  $Revision$
            $Id$
            $Log$

*******************************************************************************
*/
#ifndef RB_DB_H
#define RB_DB_H

#ifdef __cplusplus
extern "C" {
#endif

/*!
*******************************************************************************
\brief Structure de liste decrivant un radio bearer (RB)        
*/
typedef struct  RB_desc_s { 
    Transaction_t       Trans_id   ; ///< transaction associe au RB
    RB_ID               Rb_id      ; ///< ID du RB
    RB_TYPE             RB_type    ; ///< type de RB
    QOS_CLASS_T         QoS_class  ; ///< index sur la classe de QoS du RB
    L2_ID               L2_id[2]   ; ///< ID de niveau 2 des noeuds du RB 
    MAC_RLC_MEAS_T      Rb_meas[2] ; ///< Description de mesures RB associes a chaque noeud 
    struct  RB_desc_s  *next       ; ///< pointeur sur le prochain descripteur de RB
} RB_desc_t ;

// ---------------------------------------------------------------------------


RB_desc_t *add_rb( RB_desc_t **rb_entry,Transaction_t  Trans_id, QOS_CLASS_T QoS_class, L2_ID *L2_id ) ;
void del_rb_by_rbid( RB_desc_t **rb_entry, RB_ID Rb_id ) ;
void del_rb_by_transid( RB_desc_t **rb_entry, Transaction_t Trans_id ) ;
void del_all_rb( RB_desc_t **rb_entry ) ;
RB_desc_t *get_rb_desc_by_rbid( RB_desc_t *rb_entry, RB_ID Rb_id ) ;
RB_desc_t *get_rb_desc_by_transid(RB_desc_t *rb_entry, Transaction_t Trans_id ) ;
void update_rb_desc( RB_desc_t *rb_entry, Transaction_t Trans_id, RB_ID Rb_id, RB_TYPE RB_type );
void update_rb_meas( RB_desc_t *rb_entry, RB_ID Rb_id, L2_ID*L2_id, MAC_RLC_MEAS_T *Rb_meas );

#ifdef __cplusplus
}
#endif

#endif /* RB_DB_H */

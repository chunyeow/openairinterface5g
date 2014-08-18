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

\file       transact.h

\brief      Fichier d'entete contenant les declarations de type, des defines ,
            les fonctions relatifs a la gestion des transactions.

\author     BURLOT Pascal

\date       17/07/08
 
\par     Historique:
            $Author$  $Date$  $Revision$
            $Id$
            $Log$

*******************************************************************************
*/

#ifndef TRANSACT_H
#define TRANSACT_H

#ifdef __cplusplus
extern "C" {
#endif

//! Valeur par defaut du TTL de transaction
#define TTL_DEFAULT_VALUE       20


//typedef void (*fct_abort_transaction_t)( void *data) ;

///< \brief Transaction en suspend ( non-traite )       
typedef struct transact_s {
    unsigned int                    id          ; ///< Transaction ID
    enum  { INT_RRC=0, ///< Interface avec le RRC
            INT_CMM,   ///< Interface avec le CMM
            INT_PUSU   ///< Interface avec le PUSU
            }                       interf_id   ; ///< interface 
    unsigned int                    funct_id    ; ///< Transaction type
    unsigned int                    ttl         ; ///< Time to live  of transaction
    struct transact_s              *next        ; ///< next Transaction to process
    
    unsigned int                    parent_id   ; ///< Transaction parent id
    enum  { NO_PARENT=0 , 
            PARENT
            }                       parent_status; ///< Transaction parent status 
} transact_t ;

transact_t *add_item_transact( 
                    transact_t **pEntry , 
                    unsigned int   id,
                    unsigned int   interf_id,
                    unsigned int   funct_id,
                    unsigned int   parent,
                    unsigned int   status
                    );
void del_item_transact( transact_t **pEntry , unsigned int   id );

transact_t *get_item_transact( 
                    transact_t *pEntry , 
                    unsigned int   id 
                    );

void set_ttl_transact( transact_t *pEntry , Transaction_t   id , unsigned int value );
void dec_all_ttl_transact( transact_t *pEntry );
void del_all_obseleted_transact( transact_t **pEntry );

#ifdef __cplusplus
}
#endif

#endif /* TRANSACT_H */

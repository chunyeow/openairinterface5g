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

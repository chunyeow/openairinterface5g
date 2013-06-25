/*!
*******************************************************************************

\file       channels_db.h

\brief      Fichier d'entete contenant les declarations des types, des defines ,
            et des fonctions relatives a la gestion de la base de donnée sur 
            les canaux.

\author     IACOBELLI Lorenzo

\date       16/10/09

   
\par     Historique:
            $Author$  $Date$  $Revision$
            $Id$
            $Log$

*******************************************************************************
*/
#ifndef CHANNELS_DB_H
#define CHANNELS_DB_H

#ifdef __cplusplus
extern "C" {
#endif



// ---------------------------------------------------------------------------

CHANNELS_DB_T *add_chann_db( CHANNELS_DB_T **ch_entry, CHANNEL_T channel, unsigned int is_free, double info_time);        //mod_lor_10_05_28 ->char instead of int
void del_channel( CHANNELS_DB_T **ch_entry, unsigned int Ch_id);
void del_all_channels( CHANNELS_DB_T **ch_entry ) ;
CHANNELS_DB_T *get_chann_db_info( CHANNELS_DB_T *ch_entry  , unsigned int Ch_id );
CHANNELS_DB_T *up_chann_db( CHANNELS_DB_T **ch_entry, CHANNEL_T channel, unsigned int is_free, double info_time); //mod_lor_10_05_28 ->char instead of int
CHANNELS_DB_T *up_chann_ass( CHANNELS_DB_T *ch_entry, unsigned int Ch_id, unsigned int is_ass, L2_ID source_id, L2_ID dest_id); 
CHANNELS_DB_T *select_new_channel( CHANNELS_DB_T *ch_entry, L2_ID source_id, L2_ID dest_id); //mod_lor_10_05_17
#ifdef __cplusplus
}
#endif

#endif /* CHANNELS_DB_H */

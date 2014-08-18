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

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

\file    	msg_mngt.h

\brief   	Fichier d'entete contenant les declarations des types, des defines ,
			et des fonctions relatives a la gestion de l'envoi et la reception 
			de message.

\author  	BURLOT Pascal

\date    	17/07/08
 
\par     Historique:
			$Author$  $Date$  $Revision$
			$Id$
			$Log$

*******************************************************************************
*/

#ifndef MSG_MNGT_H
#define MSG_MNGT_H

#ifdef __cplusplus
extern "C" {
#endif

/*!
*******************************************************************************
\brief File des messages reçus ou a envoyer	
*/	
typedef struct msg_file_s {
    //mod_lor_10_01_25
    int s_type                  ; /// socket type: 0 -> unix; 1 -> internet
	sock_rrm_t 			*s		; ///< socket origine ou destinataire si unix
	sock_rrm_int_t      *s_int  ; ///< socket origine ou destinataire si internet
	
	msg_t    			*msg	; ///< data of message 
	struct msg_file_s 	*next	; ///< next message of file
} file_msg_t ;

/*!
*******************************************************************************
\brief Entete de la file des messages reçus ou a envoyer		
*/
typedef struct { 
	int 			     id 	; ///< id
	pthread_mutex_t      mutex  ; ///< Mutex associee
	file_msg_t          *file   ; ///< point d'entree sur la file des messages
} file_head_t ;

/*
*******************************************************************************
*/
void init_file_msg( file_head_t *file_hd , int id ) ;
file_msg_t *put_msg( file_head_t *file_hd , int s_type, void *s, msg_t *msg) ; //mod_lor_10_01_25
file_msg_t *get_msg( file_head_t *file_hd  ) ;

#ifdef __cplusplus
}
#endif

#endif /* MSG_MNGT_H */

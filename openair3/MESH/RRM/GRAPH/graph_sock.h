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

\file    	sensing_rrm_interface.h

\brief   	Fichier d'entete contenant les declarations des types, des defines ,
			et des fonctions relatives aux fonctions de communication.

\author  	IACOBELLI Lorenzo, KNOPP Raymond

\date    	01/04/2010

   
\par     Historique:
			$Author$  $Date$  $Revision$
			$Id$
			$Log$

*******************************************************************************
*/

#ifndef __SENSING_RRM_INTERFACE_H__
#define __SENSING_RRM_INTERFACE_H__


//#ifdef SENSING_RRM_XFACE

/*!
*******************************************************************************
\brief	 Entete des messages de RRM/CMM/RRC/SENSING
*/
typedef struct { 
	unsigned short start    ; ///< Identification du debut de message
	unsigned char  inst     ; ///< Identification de l'instance RRM
	unsigned char  msg_type ; ///< Identification du type message
	unsigned int   size     ; ///< taille du message
	unsigned int   Trans_id ; ///< Identification de la transaction
} msg_head_t ;

/*!
*******************************************************************************
\brief	 Definition de la structure d'un message a envoyer sur un socket:
         	- RRM->RRC
         	- RRC->RRM
         	- RRCI->RRC
         	- RRC->RRCI
         	- CMM->RRM
         	- RRM->CMM
*/
typedef struct {
  msg_head_t 	head  ; ///< entete du message
  char 		*data ; ///< message
} msg_t ;




#ifdef __cplusplus
extern "C" {
#endif


  
#define RRM_SOCK_PATH "/tmp/rrm_socket"
#define TO_RRM_SOCK_PATH "/tmp/to_rrm_socket"
  
  //! \brief Identification of the RRM/CMM/RRC message begin 		
#define START_MSG      0xA533
  //! \brief Identification of the PUSU message begin 		
#define START_MSG_PUSU 0xCC



#include <sys/socket.h>
#include <sys/un.h>

  
  /*!
*******************************************************************************
\brief  Definition de la structure definissant le socket pour envoyer les messages
  */
  typedef struct {
    int s 									; ///< identification du socket	
    struct 	sockaddr_un un_local_addr 		; ///< Adresse local si unix socket
    struct 	sockaddr_un un_dest_addr 		; ///< Adresse destinataire si unix socket
  } sock_rrm_t ;

  
  /* *** Fonctions relatives aux interfaces CMM ou SENSING *** */
  
  int open_socket( sock_rrm_t *s 	,char *path_local, char *path_dest , int rrm_inst ) ;
  void close_socket(sock_rrm_t *sock ) ;
  int send_msg_sock(sock_rrm_t *s 	,msg_t *msg ) ;
  char *recv_msg( sock_rrm_t *s ) ;		
#ifdef __cplusplus
}
#endif 


#endif

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

\file       rrm.h

\brief      Fichier d'entete contenant les declarations des types, des defines ,
et des fonctions relatives aux fonctions du RRM (Radio Resource Management ).

\author     BURLOT Pascal

\date       15/07/08

   
\par     Historique:
P.BURLOT 2009-01-20 
+ separation de la file de message CMM/RRM a envoyer en 2 files 
distinctes ( file_send_cmm_msg, file_send_rrc_msg)
L.IACOBELLI 2009-10-19
+ sensing database
+ Fusion centre and BTS role

*******************************************************************************
*/

#ifndef RRM_H
#define RRM_H

#ifdef __cplusplus
extern "C" {
#endif

  /*!
*******************************************************************************
\brief Nombre max. d'instance de RRM (Emulation)        
  */
#define MAX_RRM     10


  /*!
*******************************************************************************
\brief Structure definissant une instance RRM       
  */


  typedef struct {
    int  id                                 ; ///< identification de l'instance RRM
    enum { 
      ISOLATEDNODE=0   , ///< Node is in a isolated State
      CLUSTERHEAD_INIT0, ///< Node is in a Cluster Head initialization State  
      CLUSTERHEAD_INIT1, ///< Node is in a Cluster Head initialization State 
      CLUSTERHEAD      , ///< Node is in a Cluster Head State 
      MESHROUTER       , ///< Node is in a Mesh Router State
    } state                             ; ///< etat de l'instance
    enum { 
      NOROLE=0         , ///< Node has not a specific role
      FUSIONCENTER     , ///< Node acts as Fusion Center
      BTS              , ///< Node acts as BTS
      CH_COLL            ///< Node acts as Cluster Head collaborating with the CH at the address L2_id_FC
    } role                              ; ///< role of the node
    
    L2_ID               L2_id               ; ///< identification de niveau L2 
    L2_ID               L2_id_FC            ; ///< Fusion Centre or Cluster Head address. In CH1 of sendora scenario 2 centralized it is the address of the other CH           
    L3_INFO_T           L3_info_t           ; ///< type de l'identification de niveau L3       

    unsigned char       L3_info[MAX_L3_INFO]; ///< identification de niveau L3   
    //mod_lor_10_01_25
    unsigned char       L3_info_corr[MAX_L3_INFO]; ///< identification de niveau L3 du correspondant: FC si le noeud est un senseur ou la BS, BS si le noeud est le FC, CH1 for CH2, CH2 for CH1
    
    file_head_t         file_send_cmm_msg   ; ///< File des messages en emission
    file_head_t         file_send_rrc_msg   ; ///< File des messages en emission
    file_head_t         file_send_sensing_msg   ; ///< File des messages en emission
    file_head_t         file_send_ip_msg    ; ///< File des messages en emission //mod_lor_10_01_25
    file_head_t         file_recv_msg       ; ///< File des messages en reception
    
    struct {
      sock_rrm_t      *s                  ; ///< Socket associé a l'interface CMM
      unsigned int    trans_cnt           ; ///< Compteur de transaction avec l'interface CMM
      transact_t     *transaction         ; ///< liste des transactions non terminees
      pthread_mutex_t exclu               ; ///< mutex pour le partage de structure


    } cmm 								; ///<  info relatif a l'interface CMM
	
    struct {
#ifdef TRACE    
      FILE *fd 							; ///< Fichier pour trace de debug : action RRM->RRC
#endif
      sock_rrm_t  	*s 					; ///< Socket associé a l'interface RRC
      unsigned int 	trans_cnt 			; ///< Compteur de transaction avec l'interface RRC
      transact_t 	*transaction		; ///< liste des transactions non terminees
      pthread_mutex_t exclu				; ///< mutex pour le partage de structure

      neighbor_desc_t *pNeighborEntry 	; ///< Descripteur sur le voisinage
      RB_desc_t 		*pRbEntry 	    ; ///< Descripteur sur les RB (radio bearer) ouverts
      Sens_node_t     *pSensEntry       ; ///< Desrcipteur sur les info du sensing
      CHANNELS_DB_T   *pChannelsEntry   ; ///< Desrcipteur sur les info des canaux
    } rrc 								; ///<  info relatif a l'interface rrc
	
    struct {
      sock_rrm_t      *s                  ; ///< Socket associé a l'interface PUSU
      unsigned int    trans_cnt           ; ///< Compteur de transaction avec l'interface PUSU
      transact_t      *transaction        ; ///< liste des transactions non terminees
      pthread_mutex_t exclu               ; ///< mutex pour le partage de structure
    } pusu 	                              ; ///<  info relatif a l'interface pusu

    struct {
      sock_rrm_t      *s                  ; ///< Socket associé a l'interface SENSING
      unsigned int    trans_cnt           ; ///< Compteur de transaction avec l'interface SENSING
      transact_t      *transaction        ; ///< liste des transactions non terminees
      unsigned int    sens_active         ; ///< flag to determine if node is performing sensing //mod_lor_10_04_21
      pthread_mutex_t exclu               ; ///< mutex pour le partage de structure
    } sensing	                          ; ///<  info relatif a l'interface SENSING
	
	//mod_lor_10_04_20++
	struct {
      sock_rrm_t      *s                  ; ///< Socket associé a l'interface SENSING
      pthread_mutex_t exclu               ; ///< mutex pour le partage de structure
    } graph	;
	//mod_lor_10_04_20--
	
    //mod_lor_10_01_25++
    struct {
      sock_rrm_int_t  *s                                ; ///< Socket associé a l'IP
      unsigned int    trans_cnt                         ; ///< Compteur de transaction avec l'interface IP
      transact_t      *transaction                      ; ///< liste des transactions non terminees
      unsigned int    waiting_SN_update                 ; ///< flag to know if an update is expected from SN about frequencies in use //mod_lor_10_05_18
      unsigned int    users_waiting_update              ; ///< flag to know if there are users that wait for channels //add_lor_10_11_08
      L2_ID           L2_id_wait_users[NB_SENS_MAX][2]  ; ///< vector of users waiting for channels //add_lor_10_11_08
      pthread_mutex_t exclu                             ; ///< mutex pour le partage de structure
    } ip     							                ; ///<  info relatif a l'interface IP
    //mod_lor_10_01_25--

  } rrm_t ;

  extern rrm_t rrm_inst[MAX_RRM] ;
  extern int   nb_inst ;

#ifdef __cplusplus
}
#endif

#endif /* RRM_H */

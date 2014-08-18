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

\file       cmm_msg.h

\brief      Fichier d'entete contenant les declarations des types, des defines ,
            et des fonctions relatives aux messages CMM-RRM ou CMM-RRCI. 
            
            Les fonctions servent à créer le buffer de message, remplir l'entete 
            et copier les parametres de fonction. Chaque fonction retourne le 
            message qui pourra être envoye sur le socket entre le CMM et le 
            RRM ou RRCI .

\author     BURLOT Pascal

\date       16/07/08

   
\par     Historique:
            $Author$  $Date$  $Revision$
            $Id$
            $Log$

*******************************************************************************
*/

#ifndef CMM_MSG_H
#define CMM_MSG_H

#ifdef __cplusplus
extern "C" {
#endif

/*!
*******************************************************************************
\brief  Enumeration des messages entre le RRM/RRCI et le CMM
*/
typedef enum { 
    CMM_CX_SETUP_REQ = 0    , ///< Message CMM->RRM : requete de configuration d'une connexion
    RRM_CX_SETUP_CNF        , ///< Message RRM->CMM : confirmation de configuration d'une connexion 
    CMM_CX_MODIFY_REQ       , ///< Message CMM->RRM : requete de modification d'une connexion
    RRM_CX_MODIFY_CNF       , ///< Message RRM->CMM : confirmation de modification d'une connexion
    CMM_CX_RELEASE_REQ      , ///< Message CMM->RRM : requete de liberation d'une connexion
    RRM_CX_RELEASE_CNF      , ///< Message RRM->CMM : confirmation de liberation d'une connexion
    CMM_CX_RELEASE_ALL_REQ  , ///< Message CMM->RRM : requete de liberation de toutes les connexions
    RRM_CX_RELEASE_ALL_CNF  , ///< Message RRM->CMM : confirmation de liberation de toutes les connexions 
    RRCI_ATTACH_REQ         , ///< Message RRCI->CMM : requete de d'attachement d'un MR
    RRM_ATTACH_IND          , ///< Message RRM->CMM : indicateur d'attachement d'un MR
    CMM_ATTACH_CNF          , ///< Message CMM->RRM : confirmation d'attachement d'un MR
    RRM_MR_ATTACH_IND       , ///< Message RRM->CMM : indicateur d'attachement d'un MR
    ROUTER_IS_CH_IND        , ///< Message RRM->CMM : indicateur que le noeud est un cluster head
    RRCI_CH_SYNCH_IND       , ///< Message RRCI->CMM : indicateur que le noeud est synchronise sur un CH
    CMM_INIT_MR_REQ         , ///< Message CMM->RRM : requete d'initialisation de l'attachement d'un MR
    RRM_MR_SYNCH_IND        , ///< Message RRM->CMM : indicateur que le noeud est synchronise sur un MR
    RRM_NO_SYNCH_IND        , ///< Message RRM->CMM : indicateur que le noeud n'est pas synchronise sur un CH ou MR
    CMM_INIT_CH_REQ         , ///< Message CMM->RRM : requete d'initialisation d'un CH
    CMM_INIT_SENSING        , ///< Message CMM->RRM : requete d'initialisation du sensing
    CMM_STOP_SENSING        , ///< Message CMM->RRM : requete de stop du sensing
    CMM_ASK_FREQ            , ///< Message CMM->RRM : in BTS, message to start an open freq. query
    CMM_NEED_TO_TX          , ///< Message CMM->RRM : in SU, second scenario centr, message to start an open freq. query
    CMM_INIT_COLL_SENSING   , ///< Message CMM->RRM : requete d'initialisation du sensing collaborative //add_lor_10_11_08
    CMM_USER_DISC           , ///< Message CMM->RRM : user disconnected -> delete all active com //add_lor_10_11_08
    CMM_LINK_DISC           , ///< Message CMM->RRM : stop comm -> delete an active link //add_lor_10_11_08
    //CMM_INIT_TRANS_REQ      , ///< Message CMM->RRM : in SU, second scenario distr, message to start a connection with another SU
    //RRM_INIT_TRANS_CONF     , ///< Message RRM->CMM : confirmation de l'ouverture de la transition
    NB_MSG_CMM_RRM            ///< Nombre de message de l'interface
} MSG_CMM_RRM_T ;


/*!
*******************************************************************************
\brief  Definition des parametres de la fonction cmm_cx_setup_req() dans 
        une structure permettant le passage des parametres via un socket
*/
typedef struct   { 
    L2_ID           Src                     ; //!< L2 source MAC address 
    L2_ID           Dst                     ; //!< L2 destination MAC address
    QOS_CLASS_T     QoS_class               ; //!< QOS class index
} cmm_cx_setup_req_t ;

/*!
*******************************************************************************
\brief  Definition des parametres des fonctions rrm_cx_setup_cnf() et cmm_cx_release_req() 
        dans une structure permettant le passage des parametres via un socket
*/
typedef struct   { 
    RB_ID           Rb_id                   ; //!< L2 Rb_id
} rrm_cx_setup_cnf_t , 
  cmm_cx_release_req_t ;

/*!
*******************************************************************************
\brief  Definition des parametres de la fonction cmm_cx_modify_req() 
        dans une structure permettant le passage des parametres via un socket
*/
typedef struct   { 
    RB_ID           Rb_id                   ; //!< L2 Rb_id
    QOS_CLASS_T     QoS_class               ; //!< QOS class index
} cmm_cx_modify_req_t;

/*!
*******************************************************************************
\brief  Definition des parametres de la fonction cmm_cx_release_all_req() 
        dans une structure permettant le passage des parametres via un socket
*/
typedef struct {
    L2_ID           L2_id                   ; //!< L2 Rb_id 
} cmm_cx_release_all_req_t;

/*!
*******************************************************************************
\brief  Definition des parametres de la fonction rrci_attach_req() 
        dans une structure permettant le passage des parametres via un socket
*/
typedef struct {
    L2_ID           L2_id                   ; //!< Layer 2 (MAC) ID              
    L3_INFO_T       L3_info_t               ; //!< Type of L3 Information     
    unsigned char   L3_info[MAX_L3_INFO]    ; //!< L3 addressing Information   
    RB_ID           DTCH_B_id               ; //!< RBID of broadcast IP service (MR only)          
    RB_ID           DTCH_id                 ; //!< RBID of default IP service (MR only)            
} rrci_attach_req_t ;

/*!
*******************************************************************************
\brief  Definition des parametres de la fonction rrm_attach_ind() 
        dans une structure permettant le passage des parametres via un socket
*/
typedef struct {
    L2_ID           L2_id                   ;  //!< Layer 2 (MAC) ID       
    L3_INFO_T       L3_info_t               ;  //!< Type of L3 Information     
    unsigned char   L3_info[MAX_L3_INFO]    ;  //!< L3 addressing Information  
    RB_ID           DTCH_id                 ;  //!< RBID of default IP service (MR only)          
} rrm_attach_ind_t;

/*!
*******************************************************************************
\brief  Definition des parametres des fonctions rrm_MR_attach_ind() et router_is_CH_ind() 
        dans une structure permettant le passage des parametres via un socket
*/
typedef struct {
    L2_ID           L2_id                   ; //!< MR Layer 2 (MAC) ID
} rrm_MR_attach_ind_t ,
  router_is_CH_ind_t ;
              
/*!
*******************************************************************************
\brief  Definition des parametres de la fonction cmm_init_ch_req() 
        dans une structure permettant le passage des parametres via un socket
*/
typedef struct {
    L3_INFO_T       L3_info_t               ; //!< Type of L3 Information
    unsigned char   L3_info[MAX_L3_INFO]    ; //!< L3 addressing Information  
} cmm_init_ch_req_t ;

/*!
*******************************************************************************
\brief  Definition des parametres de la fonction cmm_attach_cnf() 
        dans une structure permettant le passage des parametres via un socket
*/
typedef struct {
    L2_ID           L2_id;
    L3_INFO_T       L3_info_t               ; //!< Type of L3 Information
    unsigned char   L3_info[MAX_L3_INFO]    ; //!< L3 addressing Information             
} cmm_attach_cnf_t ;

/*!
*******************************************************************************
\brief  Definition des parametres de la fonction cmm_attach_cnf() 
        dans une structure permettant le passage des parametres via un socket
*/
//mod_lor_10_03_12++
typedef struct {
    unsigned int     Start_fr;
    unsigned int     Stop_fr;
    unsigned int     Meas_band;
    unsigned int     Meas_tpf;
    unsigned int     Nb_channels;
    unsigned int     Overlap;
    unsigned int     Sampl_freq;
} cmm_init_sensing_t, cmm_init_coll_sensing_t ;
//mod_lor_10_03_12++

/*!//add_lor_10_11_03
*******************************************************************************
\brief  Definition des parametres de la fonction cmm_cx_setup_req() dans 
        une structure permettant le passage des parametres via un socket
*/
typedef struct   { 
    Instance_t      dest                    ; //!< Destination of the message
    QOS_CLASS_T     QoS_class               ; //!< QOS class index
} cmm_need_to_tx_t ;

/*!
*******************************************************************************
\brief  Definition des parametres de la fonction cmm_init_trans_req() 
        dans une structure permettant le passage des parametres via un socket
*/
typedef struct {
    L2_ID           L2_id                   ; //!< Layer 2 (MAC) ID of SU2             
    unsigned int    Session_id              ; //!< Session id     
    QOS_CLASS_T     QoS_class               ; //!< QOS class index           
} cmm_init_trans_req_t ;

/*!
*******************************************************************************
\brief  Definition des parametres de la fonction rrm_init_trans_conf() 
        dans une structure permettant le passage des parametres via un socket
*/
typedef struct {
    unsigned int    Session_id              ; //!< Session id     
    CHANNEL_T       all_channel             ; //!< QOS class index           
} rrm_init_trans_conf_t ;

/*!//add_lor_10_11_09
*******************************************************************************
\brief  Definition des parametres de la fonction cmm_cx_setup_req() dans 
        une structure permettant le passage des parametres via un socket
*/
typedef struct   { 
    Instance_t      dest                    ; //!< Destination of the message
} cmm_link_disk_t ;




#ifdef TRACE
extern const char *Str_msg_cmm_rrm[NB_MSG_CMM_RRM] ;
#endif

/* Rappel : les paramètres sont identiques aux fonctions 
 *          (sans le prefixe msg_ ) du fichier: cmm_rrm_interface.h
 */          
msg_t *msg_cmm_cx_setup_req( Instance_t inst, L2_ID Src, L2_ID Dst, QOS_CLASS_T QoS_class,  Transaction_t Trans_id );
msg_t *msg_rrm_cx_setup_cnf( Instance_t inst, RB_ID Rb_id, Transaction_t Trans_id );
msg_t *msg_cmm_cx_modify_req( Instance_t inst, RB_ID Rb_id , QOS_CLASS_T QoS_class, Transaction_t Trans_id  );
msg_t *msg_rrm_cx_modify_cnf( Instance_t inst, Transaction_t Trans_id  );
msg_t *msg_cmm_cx_release_req( Instance_t inst, RB_ID Rb_id , Transaction_t Trans_id );
msg_t *msg_rrm_cx_release_cnf( Instance_t inst, Transaction_t Trans_id );
msg_t *msg_cmm_cx_release_all_req( Instance_t inst, L2_ID L2_id , Transaction_t Trans_id );
msg_t *msg_rrm_cx_release_all_cnf( Instance_t inst, Transaction_t Trans_id );
msg_t *msg_rrci_attach_req( Instance_t inst, L2_ID L2_id,  L3_INFO_T L3_info_t, unsigned char *L3_info,
                            RB_ID DTCH_B_id, RB_ID DTCH_id, Transaction_t Trans_id );
msg_t *msg_rrm_attach_ind( Instance_t inst, L2_ID L2_id, L3_INFO_T L3_info_t, unsigned char *L3_info, RB_ID DTCH_id  );
msg_t *msg_cmm_attach_cnf( Instance_t inst, L2_ID L2_id, L3_INFO_T L3_info_t, unsigned char *L3_info,Transaction_t Trans_id );
msg_t *msg_rrm_MR_attach_ind( Instance_t inst, L2_ID L2_id );
msg_t *msg_router_is_CH_ind( Instance_t inst, L2_ID L2_id );
msg_t *msg_rrci_CH_synch_ind( Instance_t inst);
msg_t *msg_cmm_init_mr_req( Instance_t inst);
msg_t *msg_rrm_MR_synch_ind(Instance_t inst);
msg_t *msg_rrm_no_synch_ind( Instance_t inst);
msg_t *msg_cmm_init_ch_req( Instance_t inst, L3_INFO_T L3_info_t, void *L3_info  );
msg_t *msg_cmm_init_sensing( Instance_t inst, unsigned int  Start_fr, unsigned int  Stop_fr,unsigned int Meas_band,
        unsigned int Meas_tpf, unsigned int Nb_channels,unsigned int Overlap, unsigned int Sampl_freq ); //mod_lor_10_03_12
msg_t *msg_cmm_stop_sensing( Instance_t inst);
msg_t *msg_cmm_ask_freq( Instance_t inst);
msg_t *msg_cmm_need_to_tx( Instance_t inst, Instance_t dest,QOS_CLASS_T QoS_class);
msg_t *msg_cmm_init_trans_req( Instance_t inst, L2_ID L2_id , unsigned int Session_id, QOS_CLASS_T QoS_class, Transaction_t Trans_id );
msg_t *msg_rrm_init_trans_conf( Instance_t inst, unsigned int Session_id, CHANNEL_T all_channel, Transaction_t Trans_id );
msg_t *msg_cmm_ask_freq( Instance_t inst);
msg_t *msg_cmm_init_sensing( Instance_t inst, unsigned int  Start_fr, unsigned int  Stop_fr,unsigned int Meas_band,
        unsigned int Meas_tpf, unsigned int Nb_channels,unsigned int Overlap, unsigned int Sampl_freq ); //add_lor_10_11_08
msg_t *msg_cmm_user_disc( Instance_t inst); //add_lor_10_11_08
msg_t *msg_cmm_link_disc( Instance_t inst, Instance_t dest); //add_lor_10_11_09
#ifdef __cplusplus
}
#endif

#endif /* CMM_MSG_H */

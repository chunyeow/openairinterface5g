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

\file       rrc_msg.h

\brief      Fichier d'entete contenant les declarations des types, des defines ,
            et des fonctions relatives aux messages RRC-RRM ou RRC-RRCI. 
            
            Les fonctions servent à créer le buffer de message, remplir 
            l'entete et copier les parametres de fonction. Chaque fonction 
            retourne le message qui pourra être envoye sur le socket entre le 
            CMM et le RRM ou RRCI.

\author     BURLOT Pascal

\date       17/07/08

\par     Historique:
        L.IACOBELLI 2009-10-19
            + sensing messages 

*******************************************************************************
*/
#ifndef __RRC_RRM_MSG_H
#define __RRC_RRM_MSG_H

#include "L3_rrc_defs.h"
#include "COMMON/mac_rrc_primitives.h"

#ifdef OPENAIR2_IN
#include "rrm_sock.h"
#else
#include "RRC/MESH/rrc_rrm_interface.h" 
#endif
//#include "../../../openair2/RRC/MESH/rrc_rrm_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
*******************************************************************************
\brief  Enumeration des messages entre RRM/RRCI et le RRC
*/
typedef enum { 
    RRM_RB_ESTABLISH_REQ = 0    , ///< Message RRM->RRC : requete d'etablissement d'un RB
    RRC_RB_ESTABLISH_RESP       , ///< Message RRC->RRM : reponse d'etablissement d'un RB
    RRC_RB_ESTABLISH_CFM        , ///< Message RRC->RRM : confirmation d'etablissement d'un RB
    RRM_RB_MODIFY_REQ           , ///< Message RRM->RRC : requete de modification d'un RB
    RRC_RB_MODIFY_RESP          , ///< Message RRC->RRM : reponse de modification d'un RB
    RRC_RB_MODIFY_CFM           , ///< Message RRC->RRM : confirmation de modification d'un RB  
    RRM_RB_RELEASE_REQ          , ///< Message RRM->RRC : requete de liberation d'un RB
    RRC_RB_RELEASE_RESP         , ///< Message RRC->RRM : reponse de liberation d'un RB
    RRC_MR_ATTACH_IND           , ///< Message RRC->RRM : indication d'attachement d'un MR
    RRM_SENSING_MEAS_REQ        , ///< Message RRM->RRC : requete de configuration de mesure sensing
    RRC_SENSING_MEAS_RESP       , ///< 10Message RRC->RRM : reponse de configuration de mesure sensing
    RRC_CX_ESTABLISH_IND        , ///< Message RRC->RRM : indication de connexion etablie
    RRC_PHY_SYNCH_TO_MR_IND     , ///< Message RRC->RRM : indication de synchronisation physique a un MR
    RRC_PHY_SYNCH_TO_CH_IND     , ///< Message RRC->RRM : indication de synchronisation physique a un CH
    RRCI_CX_ESTABLISH_RESP      , ///< Message RRCI->RRC : reponse de connexion etablie
    RRC_SENSING_MEAS_IND        , ///< Message RRC->RRM : indication de nouvel mesure de sensing 
    RRM_SENSING_MEAS_RESP       , ///< Message RRM->RRC : reponse a l'indication de nouvel mesure de sensing
    RRC_RB_MEAS_IND             , ///< Message RRC->RRM : indication de nouvel mesure sur un RB 
    RRM_RB_MEAS_RESP            , ///< Message RRM->RRC : reponse a l'indication de nouvel mesure sur un RB
    RRM_INIT_CH_REQ             , ///< Message RRM->RRC : init d'un CH
    RRCI_INIT_MR_REQ            , ///< 20Message RRM->RRC : init d'un MR
    RRM_INIT_MON_REQ            , ///< Message RRM->RRC : initiation of a scanning monitoring
    RRM_INIT_SCAN_REQ           , ///< Message RRM->RRC : initiation of a scanning process
    RRC_INIT_SCAN_REQ           , ///< Message RRC->RRM : initiation of a scanning process
    //UPDATE_SENS_RESULTS_3       , ///< Message IP       : update to send to CH/FC //mod_lor_10_01_25
    RRM_END_SCAN_REQ            , ///< Message RRM->RRC : end of a scanning process
    RRC_END_SCAN_REQ            , ///< Message RRC->RRM : end of a scanning process
    RRC_END_SCAN_CONF           , ///< Message RRC->RRM : end of a scanning process ack
    RRC_INIT_MON_REQ            , ///< Message RRM->RRC : initiation of a scanning monitoring
    //OPEN_FREQ_QUERY_4           , ///< Message IP   : BTS to ask free frequencies to FC
    //UPDATE_OPEN_FREQ_7          , ///< 30Message IP       : list of frequencies usable by the secondary network
    //UPDATE_SN_OCC_FREQ_5        , ///< Message IP       : BTS sends used freq. to FC
    RRM_UP_FREQ_ASS             , ///< Message RRM->RRC : BTS assigns channels to SUs (scen1)
    RRM_END_SCAN_CONF           , ///< Message RRM->RRC : end of a scanning process
    RRC_UP_FREQ_ASS             , ///< Message RRC->RRM ://mod_lor_10_06_04
    RRM_UP_FREQ_ASS_SEC         , ///< Message RRM->RRC : CH assigns channels to SUs (scen2) //add_lor_10_11_05
    RRC_UP_FREQ_ASS_SEC         , ///< Message RRC->RRM : frequencies assigned by CH (scen2)  //add_lor_10_11_05
    /*RRC_ASK_FOR_FREQ            , ///< Message RRC->RRM : in FC/CH to report a frequency query
    RRM_OPEN_FREQ               , ///< Message RRM->RRC : FC communicates open frequencies 
    RRM_UPDATE_SN_FREQ          , ///< Message RRM->RRC : BTS sends used freq. to FC
    RRC_UPDATE_SN_FREQ          , ///< Message RRC->RRM : FC receives used freq. from BTS
    RRM_CLUST_SCAN_REQ          , ///< Message RRM->RRC : CH1 contacts CH2 to collaborate for the sensing process 
    RRC_CLUST_SCAN_REQ          , ///< Message RRC->RRM : CH2 receive request to collaborate for the sensing process from CH1
    RRM_CLUST_SCAN_CONF         , ///< Message RRM->RRC : CH2 confirmrs the beginning of a collaboration process
    RRM_CLUST_MON_REQ           , ///< Message RRM->RRC : CH1 contacts CH2 to collaborate for the monitoring process 
    RRC_CLUST_MON_REQ           , ///< Message RRC->RRM : CH2 receive request to collaborate for the monitoring process from CH1
    RRM_CLUST_MON_CONF          , ///< Message RRM->RRC : CH2 confirmrs the beginning of a monitoring process
    RRM_END_SCAN_CONF           , ///< Message RRM->RRC : CH2 confirmrs the end of the collaborative sensing process at CH1
    RRM_INIT_CONN_REQ           , ///< Message RRM->RRC : SU 1 requests a connection to SU 2
    RRC_INIT_CONN_CONF          , ///< Message RRM->RRC : Confirm from SU2 about the requested connection
    RRM_FREQ_ALL_PROP           , ///< Message RRM->RRC : SU1 sends a proposition of teh frequencies to use
    RRC_FREQ_ALL_PROP_CONF      , ///< Message RRC->RRM : SU1 receives a confirm on the usable channels
    RRM_REP_FREQ_ALL            , ///< Message RRM->RRC : SU1 reports to CH the possible channels
    RRC_REP_FREQ_ACK            , ///< Message RRC->RRM : CH informs all SUs about a channel allocated to 2 SUs
    RRC_INIT_CONN_REQ           , ///< Message RRC->RRM : Request from SU1 to establish a connection
    RRM_CONN_SET                , ///< Message RRC->RRM : Response to SU1 and connection settings communicated to rcc
    RRC_FREQ_ALL_PROP           , ///< Message RRC->RRM : SU1 proposition of frequencies to use
    RRM_FREQ_ALL_PROP_CONF      , ///< Message RRC->RRM : SU2 choise of frequencies to use
    RRC_REP_FREQ_ALL            , ///< Message RRM->RRC : SU1 reports to CH the possible channels
    RRM_REP_FREQ_ACK            , ///< Message RRC->RRM : CH informs all SUs about a channel allocated to 2 SUs
    */NB_MSG_RRC_RRM                ///< Nombre de message RRM-RRC

} MSG_RRC_RRM_T ;

/*!
*******************************************************************************
\brief  Definition des parametres de la fonction rrm_init_ch_req() dans 
        une structure permettant le passage des parametres via un socket
*/

typedef struct   { 
    LCHAN_DESC          Lchan_desc_srb0      ; //!< Logical Channel Descriptor Array
    LCHAN_DESC          Lchan_desc_srb1      ; //!< Logical Channel Descriptor Array
    L2_ID               L2_id                ; //!< Layer 2 (MAC) IDs for link
} rrm_init_ch_req_t ;


/*!
*******************************************************************************
\brief  Definition des parametres de la fonction rrm_init_mr_req() dans 
        une structure permettant le passage des parametres via un socket
*/
typedef struct   { 
    LCHAN_DESC          Lchan_desc_srb0      ; //!< Logical Channel Descriptor Array
    LCHAN_DESC          Lchan_desc_srb1      ; //!< Logical Channel Descriptor Array
    unsigned char       CH_index             ; //!< Layer 2 (MAC) IDs for CH
} rrci_init_mr_req_t ;



/*!
*******************************************************************************
\brief  Definition des parametres de la fonction rrm_rb_establish_req() dans 
        une structure permettant le passage des parametres via un socket
*/
typedef struct   { 
    L2_ID               L2_id[2]             ; //!< Layer 2 (MAC) IDs for link
    LCHAN_DESC          Lchan_desc           ; //!< Logical Channel Descriptor Array
    MAC_RLC_MEAS_DESC   Mac_rlc_meas_desc    ; //!< MAC/RLC Measurement descriptors for RB
    L3_INFO_T           L3_info_t            ; //!< Optional L3 Information
    unsigned char       L3_info[MAX_L3_INFO] ; //!< Type of L3 Information
} rrm_rb_establish_req_t ;

/*! 
*******************************************************************************
\brief  Definition des parametres de la fonction rrc_rb_establish_cfm() dans 
        une structure permettant le passage des parametres via un socket
*/
typedef struct {
    RB_ID               Rb_id                ; //!< Radio Bearer ID used by RRC
    RB_TYPE             RB_type              ; //!< Radio Bearer Type
} rrc_rb_establish_cfm_t        ;

/*! 
*******************************************************************************
\brief  Definition des parametres de la fonction rrm_rb_modify_req() dans 
        une structure permettant le passage des parametres via un socket
*/
typedef struct   { 
    LCHAN_DESC          Lchan_desc           ; //!< Logical Channel Descriptor Array
    MAC_RLC_MEAS_DESC   Mac_meas_desc        ; //!< MAC/RLC Measurement descriptors for RB
    RB_ID               Rb_id                ; //!< Radio Bearer ID
} rrm_rb_modify_req_t ;

/*! 
*******************************************************************************
\brief  Definition des parametres des fonctions rrc_rb_modify_cfm() et 
        rrm_rb_release_req() dans une structure  permettant le passage des 
        parametres via un socket
*/
typedef struct {
    RB_ID               Rb_id                ; //!< Radio Bearer ID
} rrc_rb_modify_cfm_t   ,
  rrm_rb_release_req_t  ;
  
/*! 
*******************************************************************************
\brief  Definition des parametres de la fonction rrc_MR_attach_ind() dans 
        une structure permettant le passage des parametres via un socket
*/
typedef struct {
    L2_ID               L2_id               ; //!< Layer 2 (MAC) ID
} rrc_MR_attach_ind_t  ;

/*! 
*******************************************************************************
\brief  Definition des parametres de la fonction rrm_sensing_meas_req() dans 
        une structure permettant le passage des parametres via un socket
*/
typedef struct {
    L2_ID               L2_id                ; //!< Layer 2 (MAC) ID
    SENSING_MEAS_DESC   Sensing_meas_desc    ; //!< Sensing Measurement Descriptor
} rrm_sensing_meas_req_t  ;

/*! 
*******************************************************************************
\brief  Definition des parametres de la fonction rrc_cx_establish_ind() dans 
        une structure permettant le passage des parametres via un socket
*/
typedef struct {
    L2_ID               L2_id                ; //!< Layer 2 (MAC) ID
    L3_INFO_T           L3_info_t            ; //!< Type of L3 Information
    unsigned char       L3_info[MAX_L3_INFO] ; //!< Optional L3 Information
    RB_ID               DTCH_B_id            ; //!< RBID of broadcast IP service (MR only)
    RB_ID               DTCH_id              ; //!< RBID of default IP service (MR only)
} rrc_cx_establish_ind_t ;  

/*! 
*******************************************************************************
\brief  Definition des parametres de la fonction rrc_phy_synch_to_CH_ind() dans 
        une structure permettant le passage des parametres via un socket
*/
typedef struct {
  unsigned int          Ch_index             ; //!< Clusterhead index
  L2_ID                 L2_id                ; //!< L2_ID du MR
} rrc_phy_synch_to_CH_ind_t ;

/*! 
*******************************************************************************
\brief  Definition des parametres de la fonction rrc_phy_synch_to_MR_ind() dans 
        une structure permettant le passage des parametres via un socket
*/
typedef struct {
        L2_ID           L2_id                ; //!< L2_ID du CH
} rrc_phy_synch_to_MR_ind_t ;


/*! 
*******************************************************************************
\brief  Definition des parametres de la fonction rrci_cx_establish_resp() dans 
        une structure permettant le passage des parametres via un socket
*/
typedef struct {
    L2_ID               L2_id                 ;
    unsigned char       L3_info[MAX_L3_INFO]  ; //!< Optional L3 Information
    L3_INFO_T           L3_info_t             ; //!< Type of L3 Information
} rrci_cx_establish_resp_t ;

/*! 
*******************************************************************************
\brief  Definition des parametres de la fonction rrc_sensing_meas_ind_t() dans 
        une structure permettant le passage des parametres via un socket
*/
typedef struct {
    L2_ID               L2_id                 ; //!< Layer 2 ID (MAC) of sensing node
    unsigned int        NB_meas               ; //!< Layer 2 ID (MAC) of sensing node
    SENSING_MEAS_T      Sensing_meas[NB_SENS_MAX]; //!< first Sensing Information
} rrc_sensing_meas_ind_t ;  

/*! 
*******************************************************************************
\brief  Definition des parametres de la fonction rrc_rb_meas_ind() dans 
        une structure permettant le passage des parametres via un socket
*/
typedef struct { 
    RB_ID              Rb_id                 ; //!< Radio Bearer ID
    L2_ID              L2_id                 ; //!< Layer 2 (MAC) IDs for link
    MEAS_MODE          Meas_mode             ; //!< Measurement mode (periodic or event-driven)
    MAC_RLC_MEAS_T     Mac_rlc_meas          ; //!< MAC/RLC measurements
} rrc_rb_meas_ind_t;



/*! 
*******************************************************************************
\brief  Definition des parametres de les fonctions  
        rrm_update_sens()dans une structure permettant le passage 
        des parametres via un socket
*/
/*typedef struct {
    L2_ID               L2_id                  ; //!< Layer 2 ID (MAC) of sensing node
    unsigned int        NB_info                ; //!< number of sensed channels
    Sens_ch_t           Sens_meas[NB_SENS_MAX] ; //!< sensing information
} rrm_update_sens_t ;  */ 

/*! 
*******************************************************************************
\brief  Definition des parametres des fonctions rrm_init_mon_req(), 
        rrc_init_mon_req(), rrm_clust_mon_req(), rrc_clust_mon_req() dans 
        une structure permettant le passage des parametres via un socket
*/
typedef struct {
    L2_ID               L2_id                ; //!< Layer 2 (MAC) ID of destination
    unsigned int        NB_chan              ; //!< Number of channels to scan
    unsigned int        interval             ; //!< Time between two sensing sessions
    unsigned int        ch_to_scan[NB_SENS_MAX]; //!< Vector of channels to scan
} rrm_init_mon_req_t,
 rrc_init_mon_req_t/*,
 rrm_clust_mon_req_t,
 rrc_clust_mon_req_t*/;  

/*! 
*******************************************************************************
\brief  Definition des parametres de la fonction rrm_init_scan_req() dans 
        une structure permettant le passage des parametres via un socket
*/
//mod_lor_10_03_12++
typedef struct {
    unsigned int     Start_fr;      //!< Lowest frequency considered (MHz)
    unsigned int     Stop_fr;       //!< Highest frequency considered (MHz)
    unsigned int     Meas_band;     //!< Measurement bandwidth (KHz)
    unsigned int     Meas_tpf;      //!< Measurement time per sub-band
    unsigned int     Nb_channels;   //!< Number of sub-bands
    unsigned int     Overlap;       //!< Overlap factor (%)
    unsigned int     Sampl_freq;    //!< Sampling frequency (Ms/s)
} rrm_init_scan_req_t;

/*! 
*******************************************************************************
\brief  Definition des parametres de la fonction rrc_init_scan_req() dans 
        une structure permettant le passage des parametres via un socket
*/
typedef struct {
    L2_ID      L2_id              ; //!< Layer 2 (MAC) ID of Fusion Centre
    unsigned int     Start_fr;      //!< Lowest frequency considered (MHz)
    unsigned int     Stop_fr;       //!< Highest frequency considered (MHz)
    unsigned int     Meas_band;     //!< Measurement bandwidth (KHz)
    unsigned int     Meas_tpf;      //!< Measurement time per sub-band
    unsigned int     Nb_channels;   //!< Number of sub-bands
    unsigned int     Overlap;       //!< Overlap factor (%)
    unsigned int     Sampl_freq;    //!< Sampling frequency (Ms/s)
} rrc_init_scan_req_t;  
//mod_lor_10_03_12--

/*! 
*******************************************************************************
\brief  Definition des parametres de la fonction rrm_end_scan_req() dans 
        une structure permettant le passage des parametres via un socket
*/
typedef struct {
    L2_ID      L2_id              ; //!< Layer 2 (MAC) ID of destination sensor/FC
} rrm_end_scan_req_t,
rrc_end_scan_req_t, 
rrc_end_scan_conf_t;

/*! 
*******************************************************************************
\brief  Definition des parametres de la fonction rrm_up_freq_ass() dans 
        une structure permettant le passage des parametres via un socket
*/
typedef struct {
    L2_ID               L2_id                ; //!< Layer 2 (MAC) ID of SU
    unsigned int        NB_chan              ; //!< Number of channels
    CHANNEL_T           ass_channels[NB_SENS_MAX]; //!< description of assigned channelS
} rrm_up_freq_ass_t ;

/*! //add_lor_10_11_05
*******************************************************************************
\brief  Definition des parametres de la fonction rrm_up_freq_ass() dans 
        une structure permettant le passage des parametres via un socket
*/
typedef struct {
    L2_ID               L2_id [NB_SENS_MAX]      ; //!< Layer 2 (MAC) ID of SU source
    L2_ID               L2_id_dest [NB_SENS_MAX] ; //!< Layer 2 (MAC) ID of SU source
    unsigned int        NB_all                   ; //!< Number of allocated channels
    CHANNEL_T           ass_channels[NB_SENS_MAX]; //!< description of assigned channelS
} rrm_up_freq_ass_sec_t ;

///< TYPEDEF VIA IP
//mod_lor_10_04_27++
/*! 
*******************************************************************************
\brief  Definition des parametres de les fonctions  
        rrm_update_sens()dans une structure permettant le passage 
        des parametres via un socket
*/
/*typedef struct {
    L2_ID               L2_id                  ; //!< Layer 2 ID (MAC) of sensing node
    unsigned int        NB_info                ; //!< Number of sensed channels
    Sens_ch_t           Sens_meas[NB_SENS_MAX] ; //!< Sensing information
    double              info_time              ; //!< Data of the information
} rrm_update_sens_t ; */


/*! 
*******************************************************************************
\brief  Definition des parametres de les fonctions rrm_ask_for_freq() et rrc_ask_for_freq()dans 
        une structure permettant le passage des parametres via un socket
*/
/*typedef struct {
    L2_ID      L2_id              ; //!< Layer 2 (MAC) ID of Fusion Centre
    QOS_CLASS_T      QoS          ; //!< QoS required; if 0 all QoS at disposition
} open_freq_query_t;*/

/*! 
*******************************************************************************
\brief  Definition des parametres de les fonctions  
        rrc_open_freq() et rrm_open_freq() dans une structure permettant le passage 
        des parametres via un socket
*/
/*typedef struct {
    double              date;
    L2_ID               L2_id                  ; //!< Layer 2 ID (MAC) of FC/CH
    unsigned int        NB_chan                ; //!< number of free channels
    CHANNEL_T           fr_channels[NB_SENS_MAX]; //!< description of free channelS
} update_open_freq_t;*/

/*! 
*******************************************************************************
\brief  Definition des parametres des fonctions rrm_update_SN_freq() et 
        rrm_update_SN_freq()dans 
        une structure permettant le passage des parametres via un socket
*/
/*typedef struct {
    L2_ID               L2_id                ; //!< Layer 2 (MAC) ID of FC/BTS
    unsigned int        NB_chan              ; //!< Number of channels 
    unsigned int        occ_channels[NB_SENS_MAX]; //!< Vector of channels
} update_SN_occ_freq_t;*/
//mod_lor_10_04_27--

#ifdef TRACE
extern const char *Str_msg_rrc_rrm[NB_MSG_RRC_RRM] ; 
#endif

/* Rappel : les paramètres sont identiques aux fonctions 
 *          (sans le prefixe msg_ ) du fichier: L3_rrc_interface.h
 */          


msg_t *msg_rrm_rb_establish_req( Instance_t inst,    
            const LCHAN_DESC *Lchan_desc, const MAC_RLC_MEAS_DESC *Mac_rlc_meas_desc, 
            L2_ID *L2_id, Transaction_t Trans_id, unsigned char *L3_info, L3_INFO_T L3_info_t );           


msg_t *msg_rrm_rb_modify_req( Instance_t inst, 
            const LCHAN_DESC  *Lchan_desc, const MAC_RLC_MEAS_DESC *Mac_meas_desc, 
            RB_ID Rb_id, Transaction_t Trans_id );

msg_t *msg_rrm_rb_release_req( Instance_t inst, RB_ID Rb_id, Transaction_t Trans_id );

msg_t *msg_rrm_sensing_meas_req( Instance_t inst, 
            L2_ID L2_id, SENSING_MEAS_DESC Sensing_meas_desc, Transaction_t Trans_id ) ;

msg_t *msg_rrci_cx_establish_resp( Instance_t inst, 
            Transaction_t Trans_id,L2_ID L2_id,
            unsigned char *L3_info, L3_INFO_T L3_info_t );

msg_t *msg_rrm_sensing_meas_resp( Instance_t inst, Transaction_t Trans_id );


msg_t *msg_rrm_rb_meas_resp( Instance_t inst, Transaction_t Trans_id );

msg_t *msg_rrm_init_ch_req( Instance_t inst, Transaction_t Trans_id, 
            const LCHAN_DESC *Lchan_desc_srb0, const LCHAN_DESC *Lchan_desc_srb1, 
            L2_ID L2_id )  ;

msg_t *msg_rrci_init_mr_req( Instance_t inst, 
            Transaction_t Trans_id, const LCHAN_DESC *Lchan_desc_srb0, 
            const LCHAN_DESC  *Lchan_desc_srb1, unsigned char CH_index); 
            
msg_t *msg_rrm_init_mon_req(Instance_t inst, L2_ID L2_id, unsigned int NB_chan, 
            unsigned int interval, unsigned int *ch_to_scan, Transaction_t Trans_id );
msg_t *msg_rrm_init_scan_req(Instance_t inst, unsigned int  Start_fr, unsigned int  Stop_fr,unsigned int Meas_band,
        unsigned int Meas_tpf, unsigned int Nb_channels,unsigned int Overlap, unsigned int Sampl_freq, Transaction_t Trans_id ); //mod_lor_10_03_12
msg_t *msg_rrm_end_scan_req( Instance_t inst, L2_ID L2_id, Transaction_t Trans_id );
msg_t *msg_rrm_up_freq_ass( Instance_t inst, L2_ID L2_id, unsigned int NB_chan, CHANNEL_T *ass_channels);
msg_t *msg_rrm_end_scan_conf( Instance_t inst, Transaction_t Trans_id);
msg_t *msg_rrm_up_freq_ass_sec( Instance_t inst, L2_ID *L2_id, L2_ID *L2_id_dest,unsigned int NB_all, CHANNEL_T *ass_channels);//add_lor_10_11_05


/*//mod_lor_10_04_27++             
///MESSAGES VIA IP
msg_t *msg_update_sens_results_3( Instance_t inst, L2_ID L2_id, unsigned int NB_chan, Sens_ch_t *Sens_meas, Transaction_t Trans_id ); 
msg_t *msg_open_freq_query_4( Instance_t inst, L2_ID L2_id, QOS_CLASS_T QoS, Transaction_t Trans_id );
msg_t *msg_update_open_freq_7( Instance_t inst, L2_ID L2_id, unsigned int NB_free_ch, CHANNEL_T *fr_channels, Transaction_t Trans_id);
msg_t *msg_update_SN_occ_freq_5( Instance_t inst, L2_ID L2_id, unsigned int NB_chan, unsigned int *occ_channels, Transaction_t Trans_id);
//mod_lor_10_04_27--*/

#ifdef __cplusplus
}
#endif

#endif /* RRC_MSG_H */

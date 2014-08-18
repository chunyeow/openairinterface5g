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

/*!  @addtogroup _mesh_layer3_ 
 @{
 */

/*! 
  \brief  CMM connection setup request.  Only in CH.
  \return status indication
*/
int cmm_cx_setup_req(
    Instance_t    inst      , //!< Identification de l'instance
    L2_ID         Src       , //!< L2 source MAC address
    L2_ID         Dst       , //!< L2 destination MAC address
    QOS_CLASS_T   QoS_class , //!< QOS class index
    Transaction_t Trans_id    //!< Transaction ID
    );

/*!
  \brief RRM connection confirm.  Only in CH. Confirms a cmm_cx_setup_req
*/
void rrm_cx_setup_cnf(
    Instance_t    inst     , //!< Identification de l'instance
    RB_ID         Rb_id    , //!< L2 Rb_id
    Transaction_t Trans_id   //!< Transaction ID
    );

/*!
  \brief  CMM connection modify request.  Only in CH.
  \return status indication
*/
int cmm_cx_modify_req(
    Instance_t    inst      , //!< Identification de l'instance
    RB_ID         Rb_id     , //!< L2 Rb_id
    QOS_CLASS_T   QoS_class , //!< QOS class index
    Transaction_t Trans_id    //!< Transaction ID
    );

/*!
  \brief RRM connection modify confirm.  Only in CH. Confirms a cmm_cx_modify_req
*/
void rrm_cx_modify_cnf(
   Instance_t    inst     , //!< Identification de l'instance
   Transaction_t Trans_id   //!< Transaction ID
   );

/*!
  \brief CMM connection release request.  Only in CH.
  \return status indication
*/
int cmm_cx_release_req(
    Instance_t    inst     , //!< Identification de l'instance
    RB_ID         Rb_id    , //!< L2 Rb_id
    Transaction_t Trans_id   //!< Transaction ID
    );

/*!
  \brief RRM connection modify confirm.  Only in CH. Confirms a cmm_cx_modify_req
*/
void rrm_cx_release_cnf(
    Instance_t    inst     , //!< Identification de l'instance
    Transaction_t Trans_id   //!< Transaction ID
    );

/*!
  \brief  CMM connection release all resources request.  Only in CH.
  \return status indication
*/
int cmm_cx_release_all_req(
    Instance_t    inst     , //!< Identification de l'instance
    L2_ID         L2_id    , //!< L2 Rb_id
    Transaction_t Trans_id   //!< Transaction ID
    );

/*!
  \brief RRM connection release all confirm.  Only in CH. Confirms a 
          cmm_cx_release_all_req.
*/
void rrm_cx_release_all_cnf(
    Instance_t    inst     , //!< Identification de l'instance
    Transaction_t Trans_id   //!< Transaction ID
    );

/*!
  \brief  L3 Connection attachment request.  Message sent by RRCI in MR after 
           configuration of initial RBs and reception of CH IPAddr.  Here L3_info 
           contains CH IPAddr.  The RBID's of basic IP services are also required.
  \return status indication
*/
int rrci_attach_req(
    Instance_t     inst      , //!< Identification de l'instance
    L2_ID          L2_id     , //!< Layer 2 (MAC) ID
    L3_INFO_T      L3_info_t , //!< Type of L3 Information
    unsigned char *L3_info   , //!< L3 addressing Information
    RB_ID          DTCH_B_id , //!< RBID of broadcast IP service (MR only)
    RB_ID          DTCH_id   , //!< RBID of default IP service (MR only)
    Transaction_t  Trans_id    //!< Transaction ID
    );

/*!
  \brief Connection Attachment indication.  Message sent by RRM in CH at 
          completion of attachment phase of a new MR (after configuration 
          MR IPAddr). Here L3_info contains MR IPAddr. 
*/
void rrm_attach_ind(
    Instance_t     inst      , //!< Identification de l'instance
    L2_ID          L2_id     , //!< Layer 2 (MAC) ID
    L3_INFO_T      L3_info_t , //!< Type of L3 Information
    unsigned char *L3_info   , //!< L3 addressing Information
    RB_ID          DTCH_id     //!< RBID of default IP service (MR only)
    );

/*!
  \brief L3 Connection Attachment confirmation.  Message sent by CMM in MR at 
          completion of L3 attachment phase of a new MR Here L3_info contains 
          MR IPAddr. 
*/
void cmm_attach_cnf(
    Instance_t     inst      , //!< Identification de l'instance
    L2_ID          L2_id     , //!< L2_id of CH ( Mesh Router can see 2 CH )
    L3_INFO_T      L3_info_t , //!< Type of L3 Information
    unsigned char *L3_info   , //!< L3 addressing Information
    Transaction_t  Trans_id    //!< Transaction ID
    );


/*!
  \brief  Message sent by RRM to CMM to indicate attachement at layer 2 of 
           a new MR. 
*/
void rrm_MR_attach_ind(
    Instance_t inst  , //!< Identification de l'instance
    L2_ID      L2_id   //!< MR Layer 2 (MAC) ID
    );

/*!
  \brief  Message sent by RRM to CMM to indicate that the node function is 
           Cluster head. CMM initializes then the CH configuration. 
*/
void router_is_CH_ind(
    Instance_t inst  , //!< Identification de l'instance
    L2_ID      L2_id   //!< CH Layer 2 (MAC) ID
    );

/*!
  \brief 
*/
void rrci_CH_synch_ind(
    Instance_t inst    //!< Identification de l'instance
    );

/*!
  \brief 
*/
void cmm_init_mr_req(
    Instance_t inst    //!< Identification de l'instance
    );

/*!
  \brief 
*/
void rrm_MR_synch_ind(
    Instance_t inst    //!< Identification de l'instance
    );

/*!
  \brief 
*/
void rrm_no_synch_ind(
    Instance_t inst    //!< Identification de l'instance
    );

/*!
  \brief 
*/
void cmm_init_ch_req(
    Instance_t  inst     ,  //!< Identification de l'instance
    L3_INFO_T   L3_info_t,
    void       *L3_info
    );
    
/*!
  \brief 
*/
void cmm_init_sensing( 
    Instance_t       inst,            //!< identification de l'instance
    unsigned int     Start_fr,
    unsigned int     Stop_fr,
    unsigned int     Meas_band,
    unsigned int     Meas_tpf,
    unsigned int     Nb_channels,
    unsigned int     Overlap,
    unsigned int     Sampl_freq
    );
    
/*!
  \brief 
*/
void cmm_stop_sensing( 
    Instance_t inst            //!< identification de l'instance
    );
    
/*!
  \brief 
*/
void cmm_ask_freq( 
    Instance_t inst            //!< identification de l'instance
    );
   
/*!
  \brief  add_lor_10_11_03
*/
    
void cmm_need_to_tx( 
    Instance_t inst             ,//!< identification de l'instance
    Instance_t dest             ,//!< identification de l'instance du noeud destinataire
    QOS_CLASS_T QoS_class        //!< Required quality of service (i.e. number of channels)
    );
    
/*!
  \brief  add_lor_10_11_08
*/
void cmm_init_coll_sensing( 
    Instance_t       inst,            //!< identification de l'instance
    unsigned int     Start_fr,
    unsigned int     Stop_fr,
    unsigned int     Meas_band,
    unsigned int     Meas_tpf,
    unsigned int     Nb_channels,
    unsigned int     Overlap,
    unsigned int     Sampl_freq
    );
/*!
  \brief  add_lor_10_11_09
*/
void cmm_user_disc( 
    Instance_t inst            //!< identification de l'instance
    );
/*!
  \brief  add_lor_10_11_09
*/
void cmm_link_disc( 
    Instance_t inst           ,//!< identification de l'instance
    Instance_t dest            //!< identification du destinataire
    );
/*!
  \brief  add_lor_10_11_09
*/
void disconnect_user( 
    Instance_t    inst     , //!< identification de l'instance
    L2_ID         L2_id      //!< L2_id of the SU 
    );
 /*!
  \brief  add_lor_10_11_09
*/   
int close_active_link( 
    Instance_t    inst       , //!< identification de l'instance
    L2_ID         L2_id      , //!< L2_id of the SU 
    L2_ID         L2_id_dest   //!< L2_id of the SU dest
    );





/*! @} */

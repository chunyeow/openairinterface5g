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

/*! @defgroup _mesh_layer3_ OpenAirInterface MESH L2/L3 Interfaces 
 * @{
 */

/*!
\brief Ask RRC to establish a radio bearer.  Used mainly by CH, except during 
        initialization phase of MR for default bearers (SRB0,SRB1).  Sends 
        CH IPAddr to RRC for attachment signaling (for example during DTCH_B 
        configuration).
 */
void rrm_rb_establish_req(
	Instance_t         inst             , //!< Identification de l'instance
	LCHAN_DESC        *Lchan_desc       , //!< Logical Channel Descriptor Array
	MAC_RLC_MEAS_DESC *Mac_rlc_meas_desc, //!< MAC/RLC Measurement descriptors for RB 
	L2_ID             *L2_id            , //!< Layer 2 (MAC) IDs for link
	Transaction_t      Trans_id         , //!< Transaction ID
	unsigned char     *L3_info          , //!< Optional L3 Information
	L3_INFO_T          L3_info_t          //!< Type of L3 Information
	);            
/*!
\brief RRC response to rb_establish_req.  RRC Acknowledgement of reception of 
       rrc_rb_establishment_req.
 */
void rrc_rb_establish_resp(
	Instance_t         inst             , //!< Identification de l'instance
	Transaction_t      Trans_id           //!< Transaction ID
    );
/*!
 \brief RRC confirmation of rb_establish_req.  RRC confirmation of 
         rrc_rb_establishment_req after transactions are complete. Essentially 
         for CH only (except SRB0/1)
 */
void rrc_rb_establish_cfm(
	Instance_t         inst             , //!< Identification de l'instance
	RB_ID              Rb_id            , //!< Radio Bearer ID used by RRC
	RB_TYPE            RB_type          , //!< Radio Bearer Type
	Transaction_t      Trans_id           //!< Transaction ID
	);
/*!
\brief RRC Connection Establishment indication.  Message received by RRM in CH 
       at completion of attachment phase of a new MR (after configuration MR 
       IPAddr). Here L3_info contains MR IPAddr. Message received by RRCI in MR 
       after configuration of initial RBs and reception of CH IPAddr.  Here 
       L3_info contains CH IPAddr.  For MR the RBID's of basic IP services are 
       also required.
*/
void rrc_cx_establish_ind(
	Instance_t         inst             , //!< Identification de l'instance
	L2_ID              L2_id            , //!< Layer 2 (MAC) ID
	Transaction_t      Trans_id         , //!< Transaction ID
	unsigned char     *L3_info          , //!< Optional L3 Information
	L3_INFO_T          L3_info_t        , //!< Type of L3 Information
	RB_ID              DTCH_B_id        , //!< RBID of broadcast IP service (MR only)
	RB_ID              DTCH_id            //!< RBID of default IP service (MR only)
    );
/*!
\brief RRCI Connection Establishment response.  Received by RRC in MR at 
       completion of attachment phase and address configuration of a new MR. 
       L3_info contains IPAddr of MR.
*/
void rrci_cx_establish_resp(
	Instance_t         inst             , //!< Identification de l'instance
	Transaction_t      Trans_id         , //!< Transaction ID
	unsigned char     *L3_info          , //!< Optional L3 Information
	L3_INFO_T          L3_info_t          //!< Type of L3 Information
    );
/*!
\brief Ask RRC to modify the QoS/Measurements of a radio bearer
 */
void rrm_rb_modify_req(
	Instance_t         inst             , //!< Identification de l'instance
	LCHAN_DESC        *Lchan_desc       , //!< Logical Channel Descriptor Array
	MAC_RLC_MEAS_DESC *Mac_meas_desc    , //!< MAC/RLC Measurement descriptors for RB 
	RB_ID              Rb_id            , //!< Radio Bearer ID
	Transaction_t      Trans_id           //!< Transaction ID
    );
/*!
\brief RRC response to rb_modify_req
 */
void rrc_rb_modify_resp(
	Instance_t         inst             , //!< Identification de l'instance
	Transaction_t      Trans_id           //!< Transaction ID
    );
/*!
\brief RRC confirmation of rb_modify_req
 */
void rrc_rb_modify_cfm(
	Instance_t         inst             , //!< Identification de l'instance
	RB_ID              Rb_id            , //!< Radio Bearer ID used by RRC
	Transaction_t      Trans_id           //!< Transaction ID
    );
/*!
\brief Ask RRC to release a radio bearer
 */
void rrm_rb_release_req(
	Instance_t         inst             , //!< Identification de l'instance
	RB_ID              Rb_id            , //!< Radio Bearer ID
	Transaction_t      Trans_id           //!< Transaction ID
	);
/*!
\brief RRC response to rb_release_req
 */
void rrc_rb_release_resp(
	Instance_t         inst             , //!< Identification de l'instance
	Transaction_t      Trans_id           //!< Transaction ID
    );
/*!
\brief RRC measurement indication 
 */
void rrc_rb_meas_ind(
	Instance_t         inst             , //!< Identification de l'instance
	RB_ID              Rb_id            , //!< Radio Bearer ID
	L2_ID              L2_id            , //!< Layer 2 (MAC) IDs for link
	MEAS_MODE          Meas_mode        , //!< Measurement mode (periodic or event-driven)
	MAC_RLC_MEAS_T     Mac_rlc_meas     , //!< MAC/RLC measurements
	Transaction_t      Trans_id           //!< Transaction ID
    );

/*!
\brief RRM response to rb_meas_ind
 */
void rrm_rb_meas_resp(
	Instance_t         inst             , //!< Identification de l'instance
	Transaction_t      Trans_id           //!< Transaction ID
    );
/*!
\brief Configure a sensing measurement
 */
void rrm_sensing_meas_req(
	Instance_t         inst             ,  //!< Identification de l'instance
	L2_ID              L2_id            ,  //!< Layer 2 (MAC) ID
	SENSING_MEAS_DESC  Sensing_meas_desc,  //!< Sensing Measurement Descriptor
	Transaction_t      Trans_id            //!< Transaction ID
    );
/*!
\brief RRC response to sensing_meas_req
 */
void rrc_sensing_meas_resp(
	Instance_t         inst             , //!< Identification de l'instance
	Transaction_t      Trans_id           //!< Transaction ID
    );
/*!
\brief RRC sensing measurement indication 
 */
void rrc_sensing_meas_ind(
	Instance_t         inst             , //!< Identification de l'instance
	L2_ID              L2_id            , //!< Layer 2 ID (MAC) of sensing node
	unsigned int       NB_meas          , //!< Layer 2 ID (MAC) of sensing node
	SENSING_MEAS_T    *Sensing_meas     , //!< Sensing Information
	Transaction_t      Trans_id           //!< Transaction ID
    );
/*!
\brief RRM response to sensing_meas_resp
 */
void rrm_sensing_meas_resp(
	Instance_t         inst             , //!< Identification de l'instance
	Transaction_t      Trans_id           //!< Transaction ID
    ); 
/*!
\brief Clusterhead PHY-Synch Indication
 */
void rrc_phy_synch_to_CH_ind(
	Instance_t         inst             , //!< Identification de l'instance
	unsigned int       Ch_index         , //!< Clusterhead index
	L2_ID              L2_id              //!< Layer 2 ID (MAC) of CH
    );
/*!
\brief Mesh router PHY-Synch Indication
 */
void rrc_phy_synch_to_MR_ind(
	Instance_t         inst             , //!< Identification de l'instance
	L2_ID              L2_id              //!< Layer 2 ID (MAC) of MR 
    );        

/*!
\brief Clusterhead PHY-Out-of-Synch Indication
 */
void rrc_phy_out_of_synch_CH_ind(
	Instance_t          inst             , //!< Identification de l'instance
	unsigned int        Ch_index           //!< Clusterhead Index
    );

/*!
\brief MR loss indication
 */
void rrc_MR_loss_ind(
	Instance_t          inst             , //!< Identification de l'instance
	L2_ID               L2_id              //!< Layer 2 (MAC) ID
    );
/*!
\brief Release all resources for MR
 */
void rrm_MR_release_all(
	Instance_t          inst             , //!< Identification de l'instance
	L2_ID               L2_id              //!< Layer 2 (MAC) ID
    );
/*!
\brief MR attachement indication. Sent by RRC to RRM to indicate the MAC ID of 
       a new MR attached to CH at layer 2 
 */
void rrc_MR_attach_ind(
	Instance_t         inst              , //!< Identification de l'instance
	L2_ID              L2_id               //!< Layer 2 (MAC) ID
    );
/*!
\brief initialization CH request . Sent by RRM to RRC to create the default 
       Radio bearer : SRB0 and SRB1
 */
            
void rrm_init_ch_req( 
	Instance_t         inst             , //!< Identification de l'instance
	Transaction_t      Trans_id         , //!< Transaction ID
	LCHAN_DESC        *Lchan_desc_srb0  , //!< Logical Channel Descriptor Array for SRB0 
	LCHAN_DESC        *Lchan_desc_srb1  , //!< Logical Channel Descriptor Array for SRB1 
	L2_ID             *L2_id              //!< Layer 2 (MAC) ID
    )  ;

/*!
\brief initialization MR request . Sent by RRCI to RRC to create the default 
       Radio bearer : SRB0 and SRB1
 */
void rrci_init_mr_req( 
	Instance_t         inst             , //!< Identification de l'instance
	Transaction_t      Trans_id         , //!< Transaction ID
	LCHAN_DESC        *Lchan_desc_srb0  , //!< Logical Channel Descriptor Array for SRB0 
	LCHAN_DESC        *Lchan_desc_srb1  , //!< Logical Channel Descriptor Array for SRB1  
	unsigned char      CH_index         , //!< index to identify the CH 
	L2_ID             *L2_id              //!< Layer 2 (MAC) ID
    ); 

/*!
\brief sns and ip sensing measurement indication 
 */
void rrc_update_sens( //mod_lor_10_01_25: RRC function, but also IP function
	Instance_t         inst             , //!< Identification de l'instance
	L2_ID              L2_id            , //!< Layer 2 ID (MAC) of sensing node
    unsigned int       NB_info          , //!< Number of channels info
	Sens_ch_t          *Sense_meas      , //!< Sensing Information
	double info_time                      //!< Info time
    );
    
/*!
\brief RRC starting sensing request 
 */
void rrc_init_scan_req(
    Instance_t        inst            , //!< instance ID
    L2_ID             L2_id           , //!< FC address
    unsigned int     Start_fr,
    unsigned int     Stop_fr,
    unsigned int     Meas_band,
    unsigned int     Meas_tpf,
    unsigned int     Nb_channels,
    unsigned int     Overlap,
    unsigned int     Sampl_freq,
    Transaction_t     Trans_id          //!< Transaction ID
    );
    
    
/*!
\brief RRC ending sensing confirmation 
 */
void rrc_end_scan_conf(
    Instance_t        inst            , //!< instance ID
    L2_ID             L2_id           ,
    Transaction_t     Trans_id          //!< Transaction ID
    
    );

/*!
\brief RRC ending sensing request (sensors side) 
 */
void rrc_end_scan_req(
    Instance_t        inst            , //!< instance ID
    L2_ID             L2_id           ,
    Transaction_t     Trans_id          //!< Transaction ID
    
    );

/*!
\brief RRC starting monitoring request 
 */    
void rrc_init_mon_req( 
    Instance_t inst           , //!< identification de l'instance
    L2_ID     L2_id           , //!< FC address
    unsigned int  *ch_to_scan ,
    unsigned int  NB_chan     , 
    unsigned int  interv          , //!< sensing freq.
    Transaction_t  Trans_id     //!< Transaction ID
    );
 
 
 /*!
\brief update open frequencies -> correspondent message via IP
 */    
unsigned int update_open_freq( //mod_lor_10_01_25: IP function; //mod_lor_10_05_18: return unsigned int instead of void
    Instance_t inst,            //!< instance ID
    L2_ID L2_id,                //!< L2_id of the FC/CH
    unsigned int NB_chan,
    unsigned int *occ_channels  , //!< vector on wich the selected frequencies will be saved //mod_lor_10_05_18
    CHANNELS_DB_T *fr_channels, 
    Transaction_t Trans_id
    );

/*!
\brief ask for frequencies   -> correspondent message via IP
 */     
unsigned int open_freq_query( //mod_lor_10_01_25: IP function
    Instance_t    inst, 
    L2_ID         L2_id           ,
    QOS_CLASS_T   QoS             ,
    Transaction_t Trans_id 
    );

/*!
\brief update secondary network frequencies in use  -> correspondent message via IP
 */ 
unsigned int update_SN_occ_freq( //mod_lor_10_01_25: IP function
    Instance_t inst             , //!< instance ID 
    L2_ID L2_id                 , //!< Layer 2 (MAC) ID of BTS
    unsigned int NB_chan        ,
    unsigned int *occ_channels  , 
    Transaction_t Trans_id        //!< Transaction ID
    );
     
/*!
\brief RRC cluster scan request from CH1 reported to RRM of CH2  
 */
void rrc_clust_scan_req( 
    Instance_t inst             , //!< instance ID 
    L2_ID L2_id                 , //!< Layer 2 (MAC) ID of CH2
    float interv                ,
    COOPERATION_T coop          ,
    Transaction_t Trans_id
    );
    
/*!
\brief RRC starting monitoring request ordered from another CH
 */    
void rrc_clust_mon_req( 
    Instance_t inst           , //!< identification de l'instance
    L2_ID     L2_id           , //!< CH1 address
    unsigned int  *ch_to_scan ,
    unsigned int  NB_chan     , 
    float     interv          , //!< sensing freq.
    Transaction_t  Trans_id     //!< Transaction ID
    );
    
/*!
\brief RRC reported confirmation about the connection 
 */ 
void rrc_init_conn_conf(
    Instance_t    inst      , //!< identification de l'instance 
    L2_ID L2_id             , 
    unsigned int Session_id ,
    Transaction_t Trans_id  
    );
 
/*!
\brief RRC reported confirmation from SU2 on the proposed channels 
 */    
void rrc_freq_all_prop_conf( 
    Instance_t    inst              , 
    L2_ID         L2_id             ,
    unsigned int Session_id         ,
    unsigned int NB_free_ch         ,
    CHANNEL_T   *fr_channels        ,
    Transaction_t Trans_id 
    );

/*!
\brief RRC reported assignement from CH of a channel (SENDORA scenario 2 distr) 
 */
void rrc_rep_freq_ack( 
    Instance_t    inst              , 
    L2_ID         L2_id_ch          ,
    L2_ID         L2_id_source      ,
    L2_ID         L2_id_dest        ,
    unsigned int  Session_id        ,
    CHANNEL_T     all_channel       ,
    Transaction_t Trans_id 
    );
    
/*!
\brief RRC init connection request from another SU (SENDORA scenario 2 distr) 
 */
void rrc_init_conn_req( 
    Instance_t    inst            , 
    L2_ID         L2_id           ,
    unsigned int Session_id       ,
    QOS_CLASS_T QoS_class         ,
    Transaction_t Trans_id 
    );
    
/*!
\brief RRC reported proposed channels from SU1 in SU2
 */    
void rrc_freq_all_prop( 
    Instance_t    inst              , 
    L2_ID         L2_id             ,
    unsigned int Session_id         ,
    unsigned int NB_free_ch         ,
    CHANNEL_T   *fr_channels        ,
    Transaction_t Trans_id 
    );
    
/*!
\brief RRC reported proposed channels from SU1 in CH
 */    
void rrc_rep_freq_all( 
    Instance_t    inst              ,
    L2_ID         L2_id_source      , 
    L2_ID         L2_id_dest        ,
    unsigned int  Session_id        ,
    unsigned int  NB_prop_ch        ,
    CHANNEL_T     *pr_channels      ,
    Transaction_t Trans_id 
    );
    
/*!
\brief  IP interface. Updating of the sensing measures -> correspondent message via IP
*/
unsigned int update_sens_results( 
	Instance_t inst         , //!< Identification de l'instance
	L2_ID L2_id             , //!< Adresse L2 of the source of information 
	unsigned int NB_info    , //!< Number of channel info
	Sens_ch_t *Sens_meas    , //!< Pointer to the sensing information
	double info_time
	);
	
/*!
\brief  Sensing Unit interface. End sensing confirmation
*/	
void sns_end_scan_conf( 
	Instance_t inst          //!< Identification de l'instance
	);

//mod_lor_10_05_10++
/*!
\brief  Updating sensing information received from collaborative Cluster
*/	
void up_coll_sens_results( //AAA: to add weights for the collaborative cluster information
	Instance_t inst         , //!< Identification de l'instance
	L2_ID L2_id             , //!< Adresse L2 of the source of information 
	unsigned int NB_info    , //!< Number of channel info
	Sens_ch_t *Sens_meas    , //!< Pointer to the sensing information
	double info_time
	);//mod_lor_10_05_10--

/*!
\brief  Received request of frequencies from secondary user -> attribute available channels
*/	
unsigned int ask_freq_to_CH(
    Instance_t    inst                   , //!< identification de l'instance
    L2_ID         L2_id[NB_SENS_MAX]     , //!< L2_id of the SU
    L2_ID         L2_id_dest[NB_SENS_MAX], //!< L2_id of the SU dest
    unsigned int  N_users                , //!< quality of service required (i.e. number of channels required)
    Transaction_t Trans_id                 //!< Transaction ID
    );


/*! @} */


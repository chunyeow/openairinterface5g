/**  @addtogroup _mesh_layer3_ 
 @{
 */


/** 
  \brief Establish a new classification rule.
  @returns status indication
*/
int cmm_class_setup_req(L3ID_T L3id,           //!< L3 ID type (IPv4 Addr,IPv6 Addr,MPLS Labels)
			void *InA,             //!< L3 ID A
			void *InB,             //!< L3 ID B
			RBID_T Rbid,           //!< L2 RBID
			L3QOS_T L3QoS,         //!< L3 QoS type (DSCP,EXP,ICMPV6)
			void *QoS_id,          //!< L3 QoS id (DSCP #, EXP #, ICMPv6 message type)
			Transaction_t Trans_id  //!< Transaction ID
			);

/**
  \brief Release a classification rule which was previously established.
  @returns status indication
*/
int cmm_class_release_req(L3ID_T L3id,           //!< L3 ID type (IPv4 Addr,IPv6 Addr,MPLS Labels)
			  void *InA,             //!< L3 ID A
			  void *InB,             //!< L3 ID B
			  RBID_T Rbid,           //!< L2 RBID
			  L3QOS_T L3QoS,         //!< L3 QoS type (DSCP,EXP,ICMPV6)
			  void *QoS_id,          //!< L3 QoS id (DSCP #, EXP #, ICMPv6 message type)
			  Transaction_t Trans_id  //!< Transaction ID
			  );


/** @} */

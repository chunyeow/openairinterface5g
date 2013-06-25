/*
 * This file is part of the PMIP, Proxy Mobile IPv6 for Linux.
 *
 * Authors: OPENAIR3 <openair_tech@eurecom.fr>
 *
 * Copyright 2010-2011 EURECOM (Sophia-Antipolis, FRANCE)
 * 
 * Proxy Mobile IPv6 (or PMIPv6, or PMIP) is a network-based mobility 
 * management protocol standardized by IETF. It is a protocol for building 
 * a common and access technology independent of mobile core networks, 
 * accommodating various access technologies such as WiMAX, 3GPP, 3GPP2 
 * and WLAN based access architectures. Proxy Mobile IPv6 is the only 
 * network-based mobility management protocol standardized by IETF.
 * 
 * PMIP Proxy Mobile IPv6 for Linux has been built above MIPL free software;
 * which it involves that it is under the same terms of GNU General Public
 * License version 2. See MIPL terms condition if you need more details. 
 */
/*! \file pmip_msgs.h
* \brief
* \author OpenAir3 Group
* \date 12th of October 2010
* \version 1.0
* \company Eurecom
* \project OpenAirInterface
* \email: openair_tech@eurecom.fr
*/

/** \defgroup MESSAGES MESSAGES
 * \ingroup PMIP6D
 *  PMIP Messages (MSGs)
 *  @{
 */

#ifndef __PMIP_MSGS_H__
#    define __PMIP_MSGS_H__
//-----------------------------------------------------------------------------
#    ifdef PMIP_MSGS_C
#        define private_pmip_msgs(x) x
#        define protected_pmip_msgs(x) x
#        define public_pmip_msgs(x) x
#    else
#        ifdef PMIP
#            define private_pmip_msgs(x)
#            define protected_pmip_msgs(x) extern x
#            define public_pmip_msgs(x) extern x
#        else
#            define private_pmip_msgs(x)
#            define protected_pmip_msgs(x)
#            define public_pmip_msgs(x) extern x
#        endif
#    endif
//-----------------------------------------------------------------------------
#include <netinet/ip6mh.h>
#include "icmp6.h"
#include "mh.h"
//-----------------------------------------------------------------------------
#include "pmip_cache.h"
//-GLOBAL VARIABLES----------------------------------------------------------------------------
/*! \var int g_mag_sequence_number
\brief Sequence number to be used for constructing PBU messages on a MAG.
*/
private_pmip_msgs(int g_mag_sequence_number;)

//-PROTOTYPES----------------------------------------------------------------------------
/*! \fn void init_pbu_sequence_number(void)
* \brief Initialize the sequence number generator for PBU messages.
*/
protected_pmip_msgs(void init_pbu_sequence_number(void);)

/*! \fn int get_new_pbu_sequence_number(void)
* \brief Return a valid sequence number to be used for constructing PBU messages on a MAG.
* \return   A valid sequence number.
*/
protected_pmip_msgs(int get_new_pbu_sequence_number(void);)

/*! \fn int is_pba_is_response_to_last_pbu_sent(msg_info_t * , pmip_entry_t *)
* \brief Check if a PBA response is the response to the last sent PBU, this function can be called only on a MAG.
* \param[in]  pba_info The message information structure
* \param[in]  bce      The binding cache entry
* \return   1 if true, 0 if false.
*/
protected_pmip_msgs(int is_pba_is_response_to_last_pbu_sent(msg_info_t * pba_info, pmip_entry_t *bce);)

/*! \fn struct in6_addr get_node_id(struct in6_addr *)
* \brief Translate a IPv6 address into a mobile interface identifier
* \param[in]  mn_addr The mobile address
* \return   The mobile interface identifier in a struct in6_addr.
*/
private_pmip_msgs(struct in6_addr get_node_id(struct in6_addr *mn_addr);)
/*! \fn struct in6_addr get_node_prefix(struct in6_addr *)
* \brief Retrieve the prefix of a IPv6 address
* \param[in]  mn_addr A mobile IPv6 address
* \return   The prefix.
* \note The prefix len is 64 bits
*/
protected_pmip_msgs(struct in6_addr get_node_prefix(struct in6_addr *mn_addr);)
/*! \fn int mh_create_opt_home_net_prefix(struct iovec *, struct in6_addr *)
* \brief Creates the Home Network Prefix option.
* \param[in,out]  iov Storage
* \param[in]  Home_Network_Prefix Option home network prefix value
* \return   Zero if success, negative value otherwise.
*/
private_pmip_msgs(int mh_create_opt_home_net_prefix(struct iovec *iov, struct in6_addr *Home_Network_Prefix);)

/*! \fn int mh_create_opt_handoff_indicator(struct iovec *, int handoff_indicator)
* \brief Creates the Home Network Prefix option.
* \param[in,out]  iov Storage
* \param[in]  handoff_indicator Type of handoff
* \return   Zero if success, negative value otherwise.
*/
private_pmip_msgs(int mh_create_opt_handoff_indicator(struct iovec *iov, int handoff_indicator);)

/*! \fn int mh_create_opt_access_technology_type(struct iovec *iov, int att))
* \brief Creates the Access Technology Type option.
* \param[in,out]  iov Storage
* \param[in]  att Access technology through which the mobile node is connected to the access link on the mobile access gateway
* \return   Zero if success, negative value otherwise.
*/
private_pmip_msgs(int mh_create_opt_access_technology_type(struct iovec *iov, int att);)

/*! \fn int mh_create_opt_mobile_node_link_layer_identifier(struct iovec *, int, ip6mnid_t *)
* \brief Creates the mobile interface identifier option.
* \param[in-out]  iov Storage
* \param[in]  mnlli  Option mobile node link layer identifier value
* \return   Zero if success, negative value otherwise.
*/
private_pmip_msgs(int mh_create_opt_mobile_node_link_layer_identifier(struct iovec *iov, ip6mnid_t * mnlli);)
/*! \fn int mh_create_opt_time_stamp(struct iovec *iov, ip6ts_t *)
* \brief Creates the timestamp option.
* \param[in-out]  iov Storage
* \param[in]  Timestamp  Option timestamp value
* \return   Zero if success, negative value otherwise.
*/
private_pmip_msgs(int mh_create_opt_time_stamp(struct iovec *iov, ip6ts_t * Timestamp);)
/*! \fn int mh_create_opt_link_local_address(struct iovec *, struct in6_addr *)
* \brief Creates the link local address option.
* \param[in-out]  iov Storage
* \param[in]  LinkLocal Option link local address value
* \return   Zero if success, negative value otherwise.
*/
private_pmip_msgs(int mh_create_opt_link_local_address(struct iovec *iov, struct in6_addr *LinkLocal);)
/*! \fn int mh_create_opt_dst_mn_addr(struct iovec *, struct in6_addr *)
* \brief Creates the Destination MN address option.
* \param[in-out]  iov Storage
* \param[in]  dst_mn_addr  Destinantion mobile node address option value
* \return   Zero if success, negative value otherwise.
*/
private_pmip_msgs(int mh_create_opt_dst_mn_addr(struct iovec *iov, struct in6_addr *dst_mn_addr);)

/*! \fn int mh_create_opt_serv_mag_addr(struct iovec *, struct in6_addr *)
* \brief Creates the Serving MAG address option.
* \param[in-out]  iov Storage
* \param[in]  serv_MAG_addr  Serving MAG address option value
* \return   Zero if success, negative value otherwise.
*/
private_pmip_msgs(int mh_create_opt_serv_mag_addr(struct iovec *iov, struct in6_addr *serv_MAG_addr);)
/*! \fn int mh_create_opt_serv_lma_addr(struct iovec *iov, struct in6_addr *)
* \brief Creates the Serving LMA address option.
* \param[in-out]  iov Storage
* \param[in]  serv_lma_addr  Serving LMA address option value.
* \return   Zero if success, negative value otherwise.
*/
private_pmip_msgs(int mh_create_opt_serv_lma_addr(struct iovec *iov, struct in6_addr *serv_lma_addr);)
/*! \fn int mh_create_opt_src_mn_addr(struct iovec *, struct in6_addr *)
* \brief Creates the source mobile node address option.
* \param[in-out]  iov Storage
* \param[in]  src_mn_addr  Source mobile node address option value.
* \return   Zero if success, negative value otherwise.
*/
private_pmip_msgs(int mh_create_opt_src_mn_addr(struct iovec *iov, struct in6_addr *src_mn_addr);)
/*! \fn int mh_create_opt_src_mag_addr(struct iovec *, struct in6_addr *)
* \brief Creates the mobile interface identifier option.
* \param[in-out]  iov Storage
* \param[in]  src_mag_addr  Source MAG address option value.
* \return   Zero if success, negative value otherwise.
*/
private_pmip_msgs(int mh_create_opt_src_mag_addr(struct iovec *iov, struct in6_addr *src_mag_addr);)
/*! \fn int mh_pbu_parse(msg_info_t * info, struct ip6_mh_binding_update *pbu, ssize_t len, const struct in6_addr_bundle *in_addrs, int iif)
* \brief Parse PBU message.
* \param[in-out]   info Storage containing all necessary informations about the message received.
* \param[in]  pbu  Mobility header "Binding Update".
* \param[in]  len  Length of pbu.
* \param[in]  in_addrs  Source and Destination address of PBU message.
* \param[in]  iif  Interface identifier.
* \return   Zero.
*/
protected_pmip_msgs(int mh_pbu_parse(msg_info_t * info, struct ip6_mh_binding_update *pbu, ssize_t len, const struct in6_addr_bundle *in_addrs, int iif);)
/*! \fn int mh_pba_parse(msg_info_t * , struct ip6_mh_binding_ack *, ssize_t , const struct in6_addr_bundle *, int)
* \brief Parse PBA message.
* \param[in-out]   info Storage containing all necessary informations about the message received.
* \param[in]  pba  Mobility header "Binding Acknowledgment".
* \param[in]  len  Length of pbu.
* \param[in]  in_addrs  Source and Destination address of PBU message.
* \param[in]  iif  Interface identifier.
* \return   Zero.
*/
protected_pmip_msgs(int mh_pba_parse(msg_info_t * info, struct ip6_mh_binding_ack *pba, ssize_t len, const struct in6_addr_bundle *in_addrs, int iif);)
/*! \fn int icmp_rs_parse(msg_info_t * , struct nd_router_solicit *, const struct in6_addr *, const struct in6_addr *, int , int )
* \brief Parse ICMPv6 RS message.
* \param[in-out]   info Storage containing all necessary informations about the message received.
* \param[in]  rs  Router sollicitation infos.
* \param[in]  saddr  Source address of the message.
* \param[in]  daddr  Destination address of the message.
* \param[in]  iif  Interface identifier.
* \param[in]  hoplimit  Hop limit value found in RS message.
* \return   Zero.
*/
protected_pmip_msgs(int icmp_rs_parse(msg_info_t * info, __attribute__ ((unused)) struct nd_router_solicit *rs, const struct in6_addr *saddr, const struct in6_addr *daddr, int iif, int hoplimit);)
/*! \fn int icmp_na_parse(msg_info_t *, struct nd_neighbor_advert *, const struct in6_addr *, const struct in6_addr *, int , int )
* \brief Parse ICMPv6 NA message.
* \param[in-out]   info Storage containing all necessary informations about the message received.
* \param[in]  na  Neighbour advertisement infos.
* \param[in]  saddr  Source address of the message.
* \param[in]  daddr  Destination address of the message.
* \param[in]  iif  Interface identifier.
* \param[in]  hoplimit  Hop limit value found in NA message.
* \return   Zero.
*/
protected_pmip_msgs(int icmp_na_parse(msg_info_t * info, struct nd_neighbor_advert *na, const struct in6_addr *saddr, const struct in6_addr *daddr, int iif, int hoplimit);)
/*! \fn int pmip_mh_send(const struct in6_addr_bundle *, const struct iovec *, int , int )
* \brief Send MH message.
* \param[in]  addrs  Source and destination address of the message.
* \param[in]  mh_vec  Storage of the message.
* \param[in]  iovlen  Len of the storage.
* \param[in]  oif  Outgoing interface identifier.
* \return   Zero if success, negative value otherwise.
*/
protected_pmip_msgs(int pmip_mh_send(const struct in6_addr_bundle *addrs, const struct iovec *mh_vec, int iovlen, int oif);)
/*! \fn int mh_send_pbu(const struct in6_addr_bundle *, pmip_entry_t *, struct timespec *lifetime, int oif)
* \brief Send PBU message.
* \param[in]  addrs  Source and destination address of the message.
* \param[in]  bce  Binding cache entry corresponding to the binding.
* \param[in]  lifetime  Lifetime of the binding.
* \param[in]  oif  Outgoing interface identifier.
* \return   Zero if success, negative value otherwise.
*/
protected_pmip_msgs(int mh_send_pbu(const struct in6_addr_bundle *addrs, pmip_entry_t * bce, struct timespec *lifetime, int oif);)
/*! \fn int mh_send_pba(const struct in6_addr_bundle *, pmip_entry_t *, struct timespec *, int)
* \brief Send PBA message.
* \param[in]  addrs  Source and destination address of the message.
* \param[in]  bce  Binding cache entry corresponding to the binding.
* \param[in]  lifetime  Lifetime of the binding.
* \param[in]  oif  Outgoing interface identifier.
* \return   Zero if success, negative value otherwise.
*/
protected_pmip_msgs(int mh_send_pba(const struct in6_addr_bundle *addrs, pmip_entry_t * bce, struct timespec *lifetime, int oif);)
#endif
/** @}*/

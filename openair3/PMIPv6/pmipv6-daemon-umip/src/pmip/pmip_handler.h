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
/*! \file pmip_handler.h
* \brief
* \author OpenAir3 Group
* \date 12th of October 2010
* \version 1.0
* \company Eurecom
* \project OpenAirInterface
* \email: openair_tech@eurecom.fr
*/

/** \defgroup HANDLER MESSAGE HANDLER
 * \ingroup PMIP6D
 *  PMIP Handler
 *  @{
 */

#ifndef __PMIP_HANDLER_H__
#    define __PMIP_HANDLER_H__
//-----------------------------------------------------------------------------
#    ifdef PMIP_HANDLER_C
#        define private_pmip_handler(x) x
#        define protected_pmip_handler(x) x
#        define public_pmip_handler(x) x
#    else
#        ifdef PMIP
#            define private_pmip_handler(x)
#            define protected_pmip_handler(x) extern x
#            define public_pmip_handler(x) extern x
#        else
#            define private_pmip_handler(x)
#            define protected_pmip_handler(x)
#            define public_pmip_handler(x) extern x
#        endif
#    endif
//-----------------------------------------------------------------------------
#    include <netinet/ip6mh.h>
#    include "pmip_cache.h"
#    include "pmip_tunnel.h"
//-----------------------------------------------------------------------------
protected_pmip_handler(struct icmp6_handler pmip_mag_rs_handler;)
protected_pmip_handler(struct mh_handler    pmip_mag_pba_handler;)
protected_pmip_handler(struct mh_handler    pmip_lma_pbu_handler;)
protected_pmip_handler(struct icmp6_handler pmip_mag_recv_na_handler;)
//-PROTOTYPES----------------------------------------------------------------------------
/*! \fn struct in6_addr  *link_local_addr(struct in6_addr *id)
* \brief
* \param[in]  id The "ethernet type" address.
* \return   The link local address corresponding to the supplied ethernet address.
*/
protected_pmip_handler(struct in6_addr  *link_local_addr(struct in6_addr *id);)
/*! \fn struct in6_addr  *CONVERT_ID2ADDR(struct in6_addr *result, struct in6_addr *prefix, struct in6_addr *id)
* \brief Converts an ID & a prefix into an address.
* \param[out] result The final address.
* \param[in]  prefix The prefix of the address.
* \param[in]  id     The "ethernet type" address.
* \return   The final address.
*/
protected_pmip_handler(struct in6_addr  *CONVERT_ID2ADDR(struct in6_addr *result, struct in6_addr *prefix, struct in6_addr *id);)
/*! \fn struct in6_addr  *get_mn_addr(pmip_entry_t * bce)
* \brief   Computes the address of the mobile node corresponding to the supplied binding cache entry, store it in the binding cache entry and return it.
* \param[in]  bce A binding cache entry
* \return   The address of the mobile node.
* \note		The returned address is returned using CONVERT_ID2ADDR function.
*/
protected_pmip_handler(struct in6_addr  *get_mn_addr(pmip_entry_t * bce);)
/*! \fn struct in6_addr  *solicited_mcast(struct in6_addr *id)
* \brief  Converts an ethernet address into a Multicast Address for NS Unreachability
* \param[in]  id The "ethernet type" address of a mobile node.
* \return   The corresponding multicast address.
*/
protected_pmip_handler(struct in6_addr  *solicited_mcast(struct in6_addr *id);)
/*! \fn  void             pmip_timer_retrans_pbu_handler(struct tq_elem *tqe)
* \brief  Handler triggered by add_task_abs for entry expiry and deletion, retransmit PBU
* \param[in]  tqe A task queue element
*/
private_pmip_handler(  void             pmip_timer_retrans_pbu_handler(struct tq_elem *tqe);)
/*! \fn void             pmip_timer_bce_expired_handler(struct tq_elem *tqe)
* \brief  Handler triggered by add_task_abs for entry expiry and deletion, expire PMIP binding cache entry and NS on MAG
* \param[in]  tqe A task queue element
*/
protected_pmip_handler(void             pmip_timer_bce_expired_handler(struct tq_elem *tqe);)
/*! \fn void             pmip_timer_tunnel_expired_handler(struct tq_elem *tqe)
* \brief  Handler triggered by add_task_abs for tunnel expiry and deletion
* \param[in]  tqe A task queue element
*/
protected_pmip_handler(void             pmip_timer_tunnel_expired_handler(struct tq_elem *tqe);)

/*! \fn void pmip_lma_recv_pbu(const struct ip6_mh *mh, ssize_t len, const struct in6_addr_bundle *in_addrs, int iif)
 * \brief  Handler triggered by the reception of a PBU with P flag set
 * \param[in]  mh       Mobility header
 * \param[in]  len      size of message
 * \param[in]  in_addrs Bundle of address (src and dst)
 * \param[in]  iif      Interface id
 */
public_pmip_handler(void pmip_lma_recv_pbu(const struct ip6_mh *mh, ssize_t len, const struct in6_addr_bundle *in_addrs, int iif);)

#endif
/** @}*/

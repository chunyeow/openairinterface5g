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
/*! \file pmip_lma_proc.h
* \brief
* \author OpenAir3 Group
* \date 12th of October 2010
* \version 1.0
* \company Eurecom
* \project OpenAirInterface
* \email: openair_tech@eurecom.fr
*/

/** \defgroup LMA_Processing LMA_Processing
 * \ingroup PMIP6D
 *  PMIP Processing for LMA
 *  @{
 */

#ifndef __PMIP_LMA_PROC_H__
#    define __PMIP_LMA_PROC_H__
//-----------------------------------------------------------------------------
#   ifdef PMIP_LMA_PROC_C
#       define private_pmip_lma_proc(x) x
#       define protected_pmip_lma_proc(x) x
#       define public_pmip_lma_proc(x) x
#   else
#       ifdef PMIP
#           define private_pmip_lma_proc(x)
#           define protected_pmip_lma_proc(x) extern x
#           define public_pmip_lma_proc(x) extern x
#       else
#           define private_pmip_lma_proc(x)
#           define protected_pmip_lma_proc(x)
#           define public_pmip_lma_proc(x) extern x
#       endif
#   endif
//-----------------------------------------------------------------------------
#   include <netinet/ip6.h>
#   include "pmip_cache.h"
#   include "pmip_msgs.h"
//-PROTOTYPES----------------------------------------------------------------------------
/*! \fn int lma_setup_route(struct in6_addr *, int )
* \brief Set a route by establishing a tunnel.
* \param[in]  pmip6_addr Destination address
* \param[in]  tunnel     Tunnel identifier
* \return    Returns zero on success, negative otherwise.
*/
private_pmip_lma_proc(int lma_setup_route(struct in6_addr *pmip6_addr, int tunnel);)
/*! \fn int lma_remove_route(struct in6_addr *pmip6_addr, int tunnel)
* \brief  Remove a route established by a tunnel
* \param[in]  pmip6_addr Destination address
* \param[in]  tunnel     Tunnel identifier
* \return   Returns zero on success, negative otherwise.
*/
protected_pmip_lma_proc(int lma_remove_route(struct in6_addr *pmip6_addr, int tunnel);)
/*! \fn int lma_reg(pmip_entry_t * bce)
* \brief  Register a binding cache entry, then create a tunnel between LMA and MAG (LMA->MAG) for this mobile node and add a route for peer address.
* \param[in]  bce A binding cache entry
* \return    Zero if bce is not NULL, else -1
*/
protected_pmip_lma_proc(int lma_reg(pmip_entry_t * bce);)
/*! \fn int lma_reg_no_new_tunnel(pmip_entry_t * bce)
* \brief  Register a binding cache entry.
* \param[in]  bce A binding cache entry
* \return    Zero if bce is not NULL, else -1
*/
protected_pmip_lma_proc(int lma_reg_no_new_tunnel(pmip_entry_t * bce);)
/*! \fn int lma_dereg(pmip_entry_t * bce, msg_info_t * info, int propagate)
* \brief  Deregister a binding cache entry
* \param[in]  bce A binding cache entry
* \param[in]  info Informations contained in the message received
* \param[in]  propagate Deregistration to MAG
* \return   the number of binding cache entries in the cache.
*/
protected_pmip_lma_proc(int lma_dereg(pmip_entry_t * bce, msg_info_t * info, int propagate);)
/*! \fn int lma_update_binding_entry(pmip_entry_t * bce, msg_info_t * info)
* \brief Update a binding cache entry with received message informations
* \param[in]  bce A binding cache entry
* \param[in]  info Informations contained in the message received
* \return   1 if a new tunnel must br created, else 0, if error in parameters return -1.
*/
protected_pmip_lma_proc(int lma_update_binding_entry(pmip_entry_t * bce, msg_info_t * info);)
#endif
/** @}*/


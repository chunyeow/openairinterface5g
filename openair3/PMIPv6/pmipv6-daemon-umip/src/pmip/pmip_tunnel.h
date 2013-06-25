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
/*! \file pmip_tunnel.h
* \brief
* \author OpenAir3 Group
* \date 12th of October 2010
* \version 1.0
* \company Eurecom
* \project OpenAirInterface
* \email: openair_tech@eurecom.fr
*/

/** \defgroup TUNNEL_Management TUNNELING
 * \ingroup PMIP6D
 *  PMIP Tunnel management (Creation/Deletion)
 *  @{
 */

#ifndef __PMIP_TUNNEL_H__
#    define __PMIP_TUNNEL_H__
//-----------------------------------------------------------------------------
#	ifdef PMIP_TUNNEL_C
#		define private_pmip_tunnel(x) x
#		define protected_pmip_tunnel(x) x
#		define public_pmip_tunnel(x) x
#	else
#		ifdef PMIP
#			define private_pmip_tunnel(x)
#			define protected_pmip_tunnel(x) extern x
#			define public_pmip_tunnel(x) extern x
#		else
#			define private_pmip_tunnel(x)
#			define protected_pmip_tunnel(x)
#			define public_pmip_tunnel(x) extern x
#		endif
#	endif
//-----------------------------------------------------------------------------
#   include "tqueue.h"
#   include "pmip_consts.h"
//-----------------------------------------------------------------------------
#	include <netinet/ip6.h>
//-----------------------------------------------------------------------------
/*! \struct  mnid_hnp_t
* \brief Data structure to store the association of a network prefix and a mobile interface identifier.
*/typedef struct tunnel_timer {
    struct timespec         lifetime;   /*!< \brief Delay for the tunnel before being deleted */
    struct tq_elem          tqe;        /*!< \brief Timer queue entry for expire*/
    int                     tunnel;     /*!< \brief Tunnel id*/
    struct in6_addr         remote;     /*!< \brief Remote Address of the tunnel*/
} __attribute__((__packed__))tunnel_timer_t;
//-----------------------------------------------------------------------------
/*! \var tunnel_timer_t g_tunnel_timer_table
\brief Table of timers managing the deletion of tunnels.
*/
protected_pmip_tunnel(tunnel_timer_t g_tunnel_timer_table[PMIP_MAX_MAGS];)

//-PROTOTYPES----------------------------------------------------------------------------

/*! \fn int pmip_tunnel_get_timer_index(struct in6_addr  *remoteP)
* \brief Return the index pointing the tunnel_timer_t struct in g_tunnel_timer_table which have the corresponding *remoteP struct in6_addr.
* \param[in]  remoteP    Remote tunnel address.
* \return   index found, else return the first free area in g_tunnel_timer_table.
*/
protected_pmip_tunnel(int pmip_tunnel_get_timer_index(struct in6_addr  *remoteP);)
/*! \fn void pmip_tunnels_init(void)
* \brief Initialize PMIP tunnels, and PMIP tunnels timers.
* \return   Zero if success, else other values.
*/
protected_pmip_tunnel(int pmip_tunnels_init(void);)
/*! \fn int pmip_tunnel_set_timer(struct in6_addr  *remoteP, struct timespec time_outP)
* \brief Set a timer for deleting the tunnel associated with link.
* \param[in]  remoteP    Remote tunnel address.
* \param[in]  tunnelP    Tunnel id.
* \param[in]  time_outP  time-out init value for the timer.
* \return   Zero if success, else other values.
*/
protected_pmip_tunnel(int pmip_tunnel_set_timer(struct in6_addr  *remoteP, int tunnelP, struct timespec time_outP);)
/*! \fn int pmip_tunnel_add(struct in6_addr *local, struct in6_addr *remote, int link)
* \brief Create an IP6-IP6 tunnel between \a local and \a remote.
* \param[in]  local  Local tunnel address.
* \param[in]  remote Remote tunnel address.
* \param[in]  linkP   Interface id.
* \return    Returns interface index of the newly created tunnel, or negative on error.
*/
protected_pmip_tunnel(int pmip_tunnel_add(struct in6_addr *local, struct in6_addr *remote, int link);)
/*! \fn int pmip_tunnel_del(int ifindex)
* \brief Deletes a tunnel identified by ifindex.
* \param[in]  tunnelP Tunnel id
* \return   Returns negative if tunnel does not exist, otherwise zero.
*/
protected_pmip_tunnel(int pmip_tunnel_del(int tunnelP);)
#endif
/** @}*/

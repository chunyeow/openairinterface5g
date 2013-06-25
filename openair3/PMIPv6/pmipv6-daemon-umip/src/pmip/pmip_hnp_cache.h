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
/*! \file pmip_hnp_cache.h
* \brief
* \author OpenAir3 Group
* \date 12th of October 2010
* \version 1.0
* \company Eurecom
* \project OpenAirInterface
* \email: openair_tech@eurecom.fr
*/

/**
 * \ingroup PMIP6D
 * \defgroup HNP_CACHE HOME NETWORK PREFIX CACHE
 *  PMIP HNP Cache
 *  @{
 */

#ifndef __PMIP_HNP_CACHE_H__
#    define __PMIP_HNP_CACHE_H__
//-----------------------------------------------------------------------------
#    ifdef PMIP_HNP_CACHE_C
#        define private_pmip_hnp_cache(x) x
#        define protected_pmip_hnp_cache(x) x
#        define public_pmip_hnp_cache(x) x
#    else
#        ifdef PMIP
#            define private_pmip_hnp_cache(x)
#            define protected_pmip_hnp_cache(x) extern x
#            define public_pmip_hnp_cache(x) extern x
#        else
#            define private_pmip_hnp_cache(x)
#            define protected_pmip_hnp_cache(x)
#            define public_pmip_hnp_cache(x) extern x
#        endif
#    endif
//-----------------------------------------------------------------------------
#include <netinet/ip6.h>
//-----------------------------------------------------------------------------
/*! \struct  mnid_hnp_t
* \brief Data structure to store the association of a network prefix and a mobile interface identifier.
*/typedef struct mnid_hnp_t {
    struct in6_addr mn_prefix;  /*!< \brief Network Address Prefix for mobile node */
    struct in6_addr mn_iid;     /*!< \brief Mobile node MAC address (IID) */
} mnid_hnp_t;
//-PROTOTYPES----------------------------------------------------------------------------
/*! \fn struct in6_addr EUI64_to_EUI48(struct in6_addr )
* \brief Translate an interface id (ipv6 suffix of 8 bytes) into a MAC address (6 bytes)
* \param[in]  iid A IPV6 interface id
* \return   The computed MAC address.
*/
protected_pmip_hnp_cache(struct in6_addr EUI64_to_EUI48(struct in6_addr iid);)
/*! \fn struct in6_addr EUI48_to_EUI64(struct in6_addr )
* \brief Translate a MAC address (6 bytes) into an interface id (ipv6 suffix of 8 bytes).
* \param[in]  macaddr A MAC address.
* \return   The IPV6 interface id.
*/
protected_pmip_hnp_cache(struct in6_addr EUI48_to_EUI64(struct in6_addr macaddr);)
/*! \fn void pmip_insert_into_hnp_cache(struct in6_addr mn_iid, struct in6_addr addr)
* \brief Insert into the cache, the mapping between the mobile node iid and the assigned ipv6 address.
* \param[in]  mn_iid Mobile node interface identifier (MAC address).
* \param[in]  addr   Network Address Prefix for the mobile node
* \note This function check for duplicates
*/
protected_pmip_hnp_cache(void pmip_insert_into_hnp_cache(struct in6_addr mn_iid, struct in6_addr addr);)
/*! \fn void pmip_lma_mn_to_hnp_cache_init(void)
* \brief Initialize the home network prefix cache in the LMA.
*/
protected_pmip_hnp_cache(void pmip_lma_mn_to_hnp_cache_init(void);)
/*! \fn struct in6_addr lma_mnid_hnp_map(struct in6_addr mnid, int *result)
* \brief Search if the mobile node id is already associated with a prefix in the hnp map.
* \param[in]  mn_iid     Mobile node interface identifier (MAC address).
* \param[in]  result     Status of the search, 0 if success, else -1
* \return   A valid prefix if the mobile node id is already associated with a prefix in the hnp map.
*/
protected_pmip_hnp_cache(struct in6_addr lma_mnid_hnp_map(struct in6_addr mnid, int *aaa_result);)
/*! \fn int pmip_mn_to_hnp_cache_init(void)
* \brief Initialize the home network prefix cache, if RADIUS is not configured the matching between MAC addresse and prefixes is read from a FILE "match".
* \return   Zero if success, else other values.
*/
protected_pmip_hnp_cache(int pmip_mn_to_hnp_cache_init(void);)
/*! \fn struct in6_addr mnid_hnp_map(struct in6_addr mnid, int *aaa_result)
* \brief Search if the mobile node id is already associated with a prefix in the hnp map.
* \param[in]  mn_iid     Mobile node interface identifier (MAC address).
* \param[in]  aaa_result Status of the search, greater than, or equal to zero if success, else -1.
* \return   A valid prefix if the mobile node id is already associated with a prefix in the hnp map.
* \note   Called in MAG only. If the association is not found in the map, then the radius server is queried for a network prefix, once the response has been received, if successful, the association is stored in the cach, so the radius server will not be queried for this mobile node on this MAG, even if the mobile node leaves the geographical area covered by the MAG and re-enter again.
*/
protected_pmip_hnp_cache(struct in6_addr mnid_hnp_map(struct in6_addr mnid, int *aaa_result);)
#endif
/** @}*/

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
/*! \file pmip_cache.h
* \brief
* \author OpenAir3 Group
* \date 12th of October 2010
* \version 1.0
* \company Eurecom
* \project OpenAirInterface
* \email: openair_tech@eurecom.fr
*/

/** \defgroup CACHE BINDING CACHE
 * \ingroup PMIP6D
 *  PMIP CACHE
 *  @{
 */

#ifndef __PMIP_CACHE_H__
#    define __PMIP_CACHE_H__
//-----------------------------------------------------------------------------
#    ifdef PMIP_CACHE_C
#        define private_pmip_cache(x) x
#        define protected_pmip_cache(x) x
#        define public_pmip_cache(x) x
#    else
#        ifdef PMIP
#            define private_pmip_cache(x)
#            define protected_pmip_cache(x) extern x
#            define public_pmip_cache(x) extern x
#        else
#            define private_pmip_cache(x)
#            define protected_pmip_cache(x)
#            define public_pmip_cache(x) extern x
#        endif
#    endif
//-----------------------------------------------------------------------------
#    include <pthread.h>
#    include <arpa/inet.h>
#    include <netinet/in.h>
//-----------------------------------------------------------------------------
#    include "tqueue.h"
#    include "util.h"
#    include "hash.h"
//-----------------------------------------------------------------------------
#    include "pmip_types.h"


/*! \def PMIP_CACHE_BUCKETS
* \brief A macro that defines the number of buckets in the hashtable "binding cache entry".
*/
#    define PMIP_CACHE_BUCKETS				128

#    ifndef ND_OPT_PI_FLAG_RADDR
#        define ND_OPT_PI_FLAG_RADDR		0x20
#    endif
#    define DFLT_AdvValidLifetime			86400   /* seconds */
#    define DFLT_AdvOnLinkFlag				1
#    define DFLT_AdvPreferredLifetime		14400    /* seconds */
#    define DFLT_AdvAutonomousFlag			1
#    ifndef ND_OPT_HAI_FLAG_SUPPORT_MR
#        if BYTE_ORDER== BIG_ENDIAN
#            define ND_OPT_HAI_FLAG_SUPPORT_MR  0x8000
#        else
#            define ND_OPT_HAI_FLAG_SUPPORT_MR  0x0080
#        endif
#    endif
#    define DFLT_AdvSendAdv				1
#    define DFLT_MaxRtrAdvInterval		1.5
#    define DFLT_MinRtrAdvInterval		1 //(iface) (0.33 * (iface)->MaxRtrAdvInterval)
#    define DFLT_AdvCurHopLimit			64  /* as per RFC 1700 or the next incarnation of it :) */
#    define DFLT_AdvReachableTime		0
#    define DFLT_AdvRetransTimer		0
#    define DFLT_HomeAgentPreference	20
#    define DFLT_AdvHomeAgentFlag		1
#    define DFLT_AdvIntervalOpt			1
#    define DFLT_AdvHomeAgentInfo		1
#    define DFLT_AdvRouterAddr			1
#    define MSG_SIZE 					4096

/*! \struct  adv_prefix_t
* \brief Data structure to store router advertisment informations.
*/
typedef struct AdvPrefix_t {
	struct in6_addr		Prefix; 				/*!< \brief The IPv6 prefix  */
	uint8_t				PrefixLen;				/*!< \brief The len of the IPv6 prefix  */
	int					AdvOnLinkFlag;			/*!< \brief When set, indicates that this prefix can be used for on-link determination. When not set the advertisement makes no statement about on-link or off-link properties of the prefix. For instance, the prefix might be used for address configuration with some of the addresses belonging to the prefix being on-link and others being off-link.*/
	int					AdvAutonomousFlag;		/*!< \brief When set, indicates that this prefix can be used for autonomous address configuration as specified in RFC 2462.*/
	uint32_t			AdvValidLifetime;		/*!< \brief The length of time in seconds (relative to the time the packet is sent) that the prefix is valid for the purpose of on-link determination.*/
	uint32_t			AdvPreferredLifetime;	/*!< \brief The length of time in seconds (relative to the time the packet is sent) that addresses generated from the prefix via stateless address autoconfiguration remain preferred.*/
	/* Mobile IPv6 extensions */
	int					AdvRouterAddr;			/*!< \brief When set, indicates that the address of interface is sent instead of network prefix, as is required by Mobile IPv6. When set, minimum limits specified by Mobile IPv6 are used for MinRtrAdvInterval and MaxRtrAdvInterval.*/
} adv_prefix_t;


/*! \struct  home_agent_info_t
* \brief Data structure to store home agent informations. Used for building a Router Advertisement message.
*/
typedef struct HomeAgentInfo_t {
	uint8_t 		type;			/*!< \brief Always set to ND_OPT_HOME_AGENT_INFO */
    uint8_t 		length;
    uint16_t 		flags_reserved;
	uint16_t 		preference;		/*!< \brief The preference for the Home Agent sending this Router Advertisement. Values greater than 0 indicate more preferable Home Agent, values less than 0 indicate less preferable Home Agent. This option is ignored, if AdvHomeAgentInfo is not set.*/
	uint16_t 		lifetime;		/*!< \brief The length of time in seconds (relative to the time the packet is sent) that the router is offering Mobile IPv6 Home Agent services. A value 0 must not be used. The maximum lifetime is 65520 seconds (18.2 hours). This option is ignored, if AdvHomeAgentInfo is not set.*/
} home_agent_info_t;


/*! \struct  router_ad_iface_t
* \brief Data structure to router advertisement informations. Used for building a Router Advertisement message.
*/
typedef struct ra_iface_t {
	int				AdvSendAdvert;			/*!< \brief A flag indicating whether or not the router sends periodic router advertisements and responds to router solicitations. This option no longer has to be specified first, but it needs to be on to enable advertisement on this interface */
	double			MaxRtrAdvInterval;		/*!< \brief The maximum time allowed between sending unsolicited multicast router advertisements from the interface, in seconds. Must be no less than 4 seconds and no greater than 1800 seconds. Minimum when using Mobile IPv6 extensions: 0.07.*/
	double			MinRtrAdvInterval;		/*!< \brief The minimum time allowed between sending unsolicited multicast router advertisements from the interface, in seconds. Must be no less than 3 seconds and no greater than 0.75 * MaxRtrAdvInterval. Minimum when using Mobile IPv6 extensions: 0.03.*/
	uint32_t		AdvReachableTime;		/*!< \brief The time, in milliseconds, that a node assumes a neighbor is reachable after having received a reachability confirmation. Used by the Neighbor Unreachability Detection algorithm (see Section 7.3 of RFC 2461). A value of zero means unspecified (by this router) */
	uint32_t		AdvRetransTimer;		/*!< \brief The time, in milliseconds, between retransmitted Neighbor Solicitation messages. Used by address resolution and the Neighbor Unreachability Detection algorithm (see Sections 7.2 and 7.3 of RFC 2461). A value of zero means unspecified (by this router). */
	int32_t			AdvDefaultLifetime;		/*!< \brief The lifetime associated with the default router in units of seconds. The maximum value corresponds to 18.2 hours. A lifetime of 0 indicates that the router is not a default router and should not appear on the default router list. The router lifetime applies only to the router's usefulness as a default router; it does not apply to information contained in other message fields or options. Options that need time limits for their information include their own lifetime fields. */
	int				AdvMobRtrSupportFlag;		/*!< \brief When set, the Home Agent signals it supports Mobile Router registrations (specified by NEMO Basic). AdvHomeAgentInfo must also be set when using this option.*/
	uint8_t			AdvCurHopLimit;			/*!< \brief The default value that should be placed in the Hop Count field of the IP header for outgoing (unicast) IP packets. The value should be set to the current diameter of the Internet. The value zero means unspecified (by this router).*/
    /* Mobile IPv6 extensions */
	int				AdvIntervalOpt;			/*!< \brief When set, Advertisement Interval Option (specified by Mobile IPv6) is included in Router Advertisements. When set, minimum limits specified by Mobile IPv6 are used for MinRtrAdvInterval and MaxRtrAdvInterval. */
	int				AdvHomeAgentInfo;		/*!< \brief When set, Home Agent Information Option (specified by Mobile IPv6) is included in Router Advertisements. AdvHomeAgentFlag must also be set when using this option. */
	int				AdvHomeAgentFlag;		/*!< \brief When set, indicates that sending router is able to serve as Mobile IPv6 Home Agent. When set, minimum limits specified by Mobile IPv6 are used for MinRtrAdvInterval and MaxRtrAdvInterval. */
	uint16_t		HomeAgentPreference;	/*!< \brief The preference for the Home Agent sending this Router Advertisement. Values greater than 0 indicate more preferable Home Agent, values less than 0 indicate less preferable Home Agent. This option is ignored, if AdvHomeAgentInfo is not set. */
	int32_t			HomeAgentLifetime;		/*!< \brief The length of time in seconds (relative to the time the packet is sent) that the router is offering Mobile IPv6 Home Agent services. A value 0 must not be used. The maximum lifetime is 65520 seconds (18.2 hours). This option is ignored, if AdvHomeAgentInfo is not set. */
	int				AdvManagedFlag;			/*!< \brief When set, hosts use the administered (stateful) protocol for address autoconfiguration in addition to any addresses autoconfigured using stateless address autoconfiguration. The use of this flag is described in RFC 2462. */
	int				AdvOtherConfigFlag;		/*!< \brief When set, hosts use the administered (stateful) protocol for autoconfiguration of other (non-address) information. The use of this flag is described in RFC 2462. */
	adv_prefix_t	Adv_Prefix;				/*!< \brief see  adv_prefix_t*/
} router_ad_iface_t;


/*! \struct  pmip_entry_t
* \brief Data structure "binding cache entry", to store all binding informations relative to a mobile node.
*/
typedef struct pmip_entry_t {
	struct in6_addr			mn_prefix;			/*!< \brief Network Address Prefix for MN */
	struct in6_addr			our_addr;			/*!< \brief Address to which we got BU */
	struct in6_addr			mn_suffix;			/*!< \brief MN IID */
	struct in6_addr			mn_hw_address;		/*!< \brief MAC ADDR */
	struct in6_addr			mn_addr;			/*!< \brief Full MN Address */
	struct in6_addr			mn_serv_mag_addr;	/*!< \brief Serving MAG Address */
	struct in6_addr			mn_serv_lma_addr;	/*!< \brief Serving LMA Address (attribute filled only on MAGs)*/
	struct in6_addr			mn_link_local_addr;	/*!< \brief Link Local Address  for MN */
	struct timespec			add_time;			/*!< \brief When was the binding added or modified */
	struct timespec			lifetime;			/*!< \brief Lifetime sent in the Binding Update, in seconds */
	uint16_t				seqno_in;			/*!< \brief Sequence number for response messages */
	uint16_t				seqno_out;			/*!< \brief Sequence number for created messages */
	uint16_t				PBU_flags;			/*!< \brief PBU flags */
	uint8_t					PBA_flags;			/*!< \brief PBA flags */
	int 					type;				/*!< \brief Entry type */
	int						unreach;			/*!< \brief ICMP dest unreach count */
	int						tunnel;				/*!< \brief Tunnel interface index */
	int						link;				/*!< \brief Home link interface index */
	/* PBU/PBRR message for retransmissions */
    struct iovec 			mh_vec[2 * (IP6_MHOPT_MAX + 1)];          /* was 7 */
    int 					iovlen;
	/* info_block status flags */
	uint8_t 				status;				/*!< \brief Info_block status flag*/
	ip6ts_t 				timestamp;			/*!< \brief Info_block status timestamp*/
	uint32_t 				msg_event;			/*!< \brief Info_block status msg_event*/

	struct timespec			br_lastsent;		/*!< \brief BR ratelimit, for internal use only*/
	int 					br_count;			/*!< \brief BR ratelimit, for internal use only*/
	int 					n_rets_counter;		/*!< \brief Counter for N retransmissions before deleting the entry, for internal use only*/
	pthread_rwlock_t 		lock;				/*!< \brief Protects the entry, for internal use only*/
	struct tq_elem 			tqe;				/*!< \brief Timer queue entry for expire, for internal use only*/
	void (*cleanup) (struct pmip_entry_t * bce);/*!< \brief Clean up bce data, for internal use only*/
} pmip_entry_t;


//Dedicated to PMIP cache
/*! \def BCE_NO_ENTRY
\brief A macro that defines the state of a binding cache entry, the state here is "unknown entry".
*/
#    define BCE_NO_ENTRY (-1)
/*! \def BCE_PMIP
\brief A macro that defines the state of a binding cache entry, the state here is "registered pmip entry".
*/
#    define BCE_PMIP 5
/*! \def BCE_TEMP
\brief A macro that defines the state of a binding cache entry, the state here is "binding cache entry waiting for registration".
* \note This state is possible only on MAG entities, not LMA.
*/
#    define BCE_TEMP 6


//-GLOBAL VARIABLES----------------------------------------------------------------------------
/*! \var pthread_rwlock_t pmip_lock
\brief Global var mutex on the binding cache
*/
protected_pmip_cache(pthread_rwlock_t pmip_lock;)
/*! \var router_ad_iface_t router_ad_iface
\brief Router advertisement structure containing all information that will be sent in the router advertisement message by MAGs.
*/
protected_pmip_cache(router_ad_iface_t router_ad_iface;)


//-PROTOTYPES----------------------------------------------------------------------------
/*! \fn int get_pmip_cache_count(int type)
* \brief
* \param[in]  type The type of binding cache entry
* \return   The number of binding cache entries in the cache.
*/
private_pmip_cache(int get_pmip_cache_count(int type);)
/*! \fn void dump_pbce(void *bce, void *os)
* \brief Dump in an output stream, the content of a binding cache entry.
* \param[in]  bce A binding cache entry.
* \param[in]  os  An output stream.
*/
private_pmip_cache(void dump_pbce(void *bce, void *os);)
/*! \fn int pmip_cache_init(void)
* \brief Initialize the pmip cache
* \return   The status of the initialization.
*/
protected_pmip_cache(int pmip_cache_init(void);)
/*! \fn void init_iface_ra(void)
* \brief Set once the router advertisement parameters
*/
protected_pmip_cache(void init_iface_ra(void);)
/*! \fn pmip_entry_t * pmip_cache_alloc(int type)
* \brief Allocates a new binding cache entry. Returns allocated space (pmip_entry_t*) for an entry or NULL if errors.
* \param[in]  type The type of binding cache entry
* \return   The allocated binding cache entry or NULL if memory space available or if initialization of the binding cache entry failled.
*/
protected_pmip_cache(pmip_entry_t * pmip_cache_alloc(int type);)
/*! \fn int pmip_cache_start(pmip_entry_t *bce)
* \brief Start a timer on a binding cache entry.
* \param[in]  bce a binding cache entry
* \return   Always 0.
* \note A task queue element (see tqueue.c) is created and managed by a timed thread
*/
protected_pmip_cache(int pmip_cache_start(pmip_entry_t * bce);)
/*! \fn pmip_entry_t * pmip_cache_add(pmip_entry_t *bce)
* \brief Add an entry in the binding cache.
* \param[in]  bce a binding cache entry
* \return   The binding cache entry if success, else NULL.
* \note If the binding cache entry type is BCE_PMIP, then a timer is started by a call to pmip_cache_start.
*/
protected_pmip_cache(pmip_entry_t * pmip_cache_add(pmip_entry_t * bce);)
/*! \fn pmip_entry_t * pmip_cache_get(const struct in6_addr *our_addr, const struct in6_addr *peer_addr)
* \brief Add an entry in the binding cache.
* \param[in]  our_addr  the IPv6 fixed address of the PMIP entity
* \param[in]  peer_addr peer hardware address (link-layer address)
* \return   The corresponding cache entry, else NULL.
* \note 	If a binding cache entry is returned, its mutex is write locked, pmip mutex "pmip_lock" is also read-write locked.
*/
protected_pmip_cache(pmip_entry_t * pmip_cache_get(const struct in6_addr *our_addr, const struct in6_addr *peer_addr);)
/*! \fn void pmipcache_release_entry(pmip_entry_t *bce)
* \brief Unlocks a binding cache entry.
* \param[in]  bce  a binding cache entry
* \note 	"pmip_lock" mutex is also unlocked.
*/
protected_pmip_cache(void pmipcache_release_entry(pmip_entry_t * bce);)
/*! \fn int pmip_cache_exists(const struct in6_addr*, const struct in6_addr*)
* \brief Check the existence of a binding cache entry for a tuple of addresses in the binding cache.
* \param[in]  our_addr  the IPv6 fixed address of the PMIP entity
* \param[in]  peer_addr peer hardware address (link-layer address)
* \return   The type of the binding cache entry if found, -1 if entry not found.
* \note 	If a binding cache entry is found, its mutex is unlocked.
*/
protected_pmip_cache(int pmip_cache_exists(const struct in6_addr *our_addr, const struct in6_addr *peer_addr);)
/*! \fn void pmipcache_free(pmip_entry_t *bce)
* \brief Free the allocated memory of a binding cache entry.
* \param[in]  bce  a binding cache entry
*/
private_pmip_cache(void pmipcache_free(pmip_entry_t * bce);)
/*! \fn void pmip_bce_delete(pmip_entry_t*)
* \brief Remove a binding cache entry from the binding cache and free it.
* \param[in]  bce  a binding cache entry
*/
protected_pmip_cache(void pmip_bce_delete(pmip_entry_t * bce);)
/*! \fn void pmip_cache_delete(const struct in6_addr*, const struct in6_addr*)
* \brief Search a binding cache entry in the binding cache corresponding to a tupple of addresses and remove this binding cache entry from the binding cache and free it.
* \param[in]  our_addr  the IPv6 fixed address of the PMIP entity
* \param[in]  peer_addr peer hardware address (link-layer address)
*/
protected_pmip_cache(void pmip_cache_delete(const struct in6_addr *our_addr, const struct in6_addr *peer_addr);)
/*! \fn int pmip_cache_iterate(int (*func) (void *, void *), void *arg)
* \brief Apply function to every BC entry. Iterates through proxy binding cache, calling \a func for each entry. Extra data may be passed to \a func in \a arg.\a func takes a bcentry as its first argument and \a arg as second argument.
* \param[in]  func  the int (*func) (void *, void *) function pointer to apply to every binding cache entry.
* \param[in]  arg   extra data for func
* \return	The iterate status.
*/
public_pmip_cache(int pmip_cache_iterate(int (*func) (void *, void *), void *arg);)
#endif
/** @}*/

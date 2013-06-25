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
/*! \file pmip_consts.h
* \brief Describe all constants for pmip
* \author OpenAir3 Group
* \date 12th of October 2010
* \version 1.0
* \company Eurecom
* \project OpenAirInterface
* \email: openair_tech@eurecom.fr
*/

/** \defgroup CONSTANTS CONSTANTS
 * \ingroup PMIP6D
 *  PMIP CONSTANTS
 *  @{
 */

#ifndef __pmip_consts_h
#    define __pmip_consts_h
#    include <netinet/in.h>
#    include "conf.h"

#    define MAX_MOBILES 64

/*
* Mobility Header Message Option Types
* new mobility header options types defined
*/
#    define IP6_MHOPT_MOBILE_NODE_IDENTIFIER            0x08   /* Mobile Node Identifier Option */
/*
* Mobility Header Message Option Types
*/
#    define IP6_MHOPT_DST_MN_ADDR                       0x0B   /* Source Mobile Node address */
#    define IP6_MHOPT_SERV_MAG_ADDR                     0x0C   /* Serving MAG address */
#    define IP6_MHOPT_SERV_LMA_ADDR                     0x0D   /* Source Mobile Node address */
#    define IP6_MHOPT_SRC_MN_ADDR                       0x0E   /* Source Mobile Node address */
#    define IP6_MHOPT_SRC_MAG_ADDR                      0x0F   /* Serving MAG address */
#    define IP6_MHOPT_HOME_NETWORK_PREFIX               0x16   /* Home Network Prefix */
#    define IP6_MHOPT_HANDOFF_INDICATOR                 0x17   /* Handoff Indicator Option */
#    define IP6_MHOPT_ACCESS_TECHNOLOGY_TYPE            0x18   /* Access Technology Type Option */
#    define IP6_MHOPT_MOBILE_NODE_LINK_LAYER_IDENTIFIER 0x19   /* Mobile Node Link Local Identifier Option */
#    define IP6_MHOPT_LINK_LOCAL_ADDRESS                0x1A   /* link local address */
#    define IP6_MHOPT_TIME_STAMP                        0x1B   /* Timestamp */
#    define IP6_MHOPT_PMIP_MAX                          IP6_MHOPT_TIME_STAMP


#    define IP6_MHOPT_HI_RESERVED                                                  0
#    define IP6_MHOPT_HI_ATTACHMENT_OVER_NEW_INTERFACE                             1
#    define IP6_MHOPT_HI_HANDOFF_BETWEEN_2_DIFF_INTERFACES_OF_SAME_MOBILE_NODE     2
#    define IP6_MHOPT_HI_HANDOFF_BETWEEN_MAGS_FOR_SAME_INTERFACE                   3
#    define IP6_MHOPT_HI_HANDOFF_STATE_UNKNOWN                                     4
#    define IP6_MHOPT_HI_HANDOFF_STATE_NOT_CHANGED                                 5

#    define IP6_MHOPT_ATT_RESERVED                                                 0
#    define IP6_MHOPT_ATT_VIRTUAL                                                  1
#    define IP6_MHOPT_ATT_PPP                                                      2
#    define IP6_MHOPT_ATT_IEEE802_3                                                3
#    define IP6_MHOPT_ATT_IEEE802_11ABG                                            4
#    define IP6_MHOPT_ATT_IEEE802_16E                                              5

#    define IP6_MH_TYPE_PBREQ   8   /* Proxy Binding Request */
#    define IP6_MH_TYPE_PBRES   9   /* Proxy Binding Response */


//Define STATUS FLAGS for FSM.
#    define hasDEREG        0x00000050  /* Has a DEREG */
/*!< \brief WLCCP message originating from access point, captured on MAG, message informing about wireless association of a mobile node with the access point */
#    define hasWLCCP        0x00000040  /* Has a WLCCP CISCO protocol */
#    define hasRS           0x00000030  /* Has a RS */
#    define hasNA           0x00000020  /* Has a NA */
#    define hasNS           0x00000010  /* Has a NS */
#    define hasPBU          0x00000008  /* Has a PBU */
#    define hasPBA          0x00000004  /* Has a PBA */
#    define hasPBREQ        0x00000002  /* Has a PBRR */
#    define hasPBRES        0x00000001  /* Has a PBRE */
#    define PREFIX_LENGTH   64
#endif
/** @}*/

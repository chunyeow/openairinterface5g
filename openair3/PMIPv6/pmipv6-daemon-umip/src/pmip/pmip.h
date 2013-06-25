/** @file pmip.h
* @brief Proxy Mobile IPv6 implementation - EURECOM
*/

/** @defgroup PMIP6D ProxyMobileIPv6
 *  Proxy Mobile IPv6 EURECOM Implementation
 *  @{
 */

/** @mainpage Proxy MobileIPv6 Daemon - OpenAirInterface
*
* @authors OpenAir3 - EURECOM
*
* @section tablecontent Table of contents
*
* 1. Introduction \n
* 2. PMIPv6 Architecture \n
* 3. PMIPv6 Message flow scheme \n
* 4. PMIPv6 Software architecture\n
* 5. Details of our real PMIPv6 testbed \n
*
* @section intro1 1.) Introduction
*
* Proxy Mobile IPv6 (PMIPv6) is a network-based mobility management protocol for localized domains, which has been actively standardized by the IETF NETLMM Working Group. PMIPv6 is starting to attract much attention among telecommunication and internet communities due to its salient features and it is expected to expedite the real deployment of IP-based mobility management.
*
* PMIPv6 has been chosen by 3GPP and LTE and does not require any IPv6 stack modification in the Mobile Node (MN). Its main idea is to have a unique Home Network Prefix per MN.
* Moreover, the whole mobility support is managed by Local Mobility Anchor (LMA) and Mobile Access Gateway (MAG) and not from the MN.
*
* @section intro2 2.) PMIPv6 Architecture
*\image html PMIP6D.domain.jpg "Overview of PMIPv6 architecure"
* In the PMIPv6 architecture, there are two main core functionnal entities :
* - Local Mobility Anchor (LMA): it is similar to HA in MIPv6. LMA is responsible for maintaining the MN's reachability state and it is the topological anchor point for the MN's Home Network Prefix (HNP). LMA includes a binding cache entry for each currently registered MN with MN-Identifier, the MN's HNP, a flag indicating the proxy registration and the interface identifier of the bidirectional tunnel between the LMA and MAG.
*
* - Mobile Access Gateway (MAG): it is the entity that performs the mobility management on behalf of the MN and it resides on the access link where the MN is anchored. The MAG is responsible for detecting the MN's movements to and from the access link and for initiating binding registrations to the MN's LMA. Moreover, the MAG establishes a tunnel with the LMA for enabling the MN to use an address from its HNP and emulates the MN's home network on the access network for each MN.
*
*
* @section intro3 3.) PMIPv6 Message flow scheme
* The execution of the message flow of the overall operations in PMIPv6 is show in the figure below\n
*\image html message_flow.jpg "Message flow in PMIPv6"
* The main steps in the PMIPv6 mobility management scheme are achieved as follows :
* - MN attachment: once a MN enters a PMIPv6 domain and attaches to an access link, the MAG on that access link performs the access authentication procedure with a policy server using the MN's profile, which contains MN-Identifier, LMA address and other related configuration parameters;
* - Proxy Binding exchange: the MAG sends to the LMA a Proxy Binding Update (PBU) message on behalf of the MN including the MN-Identifier. Upon accepting the message, the LMA replies with a Proxy Binding Acknowledgment (PBA) message including the MN's HNP. With this procedure the LMA creates a Binding Cache Entry (BCE) for the MN and a bi-directional tunnel between the LMA and the MAG is set up;
* - Address Configuration procedure: at this point the MAG has all the required information for emulating the MN's home link. It sends Router Advertisement message to the MN on the access link advertising the MN's HNP as the hosted on-link-prefix. On receiving this message, the MN configures its interface either using stateful or stateless address configuration modes. Finally the MN ends up with an address from its HNP that it can use while moving in the PMIPv6 domain.
*
* @section intro4 4.) PMIP6D Software Architecture
* Here is an overview of the PMIP6D Software overview.
*\image html MIPL.jpg "PMIP6D Software architecture"
*
* All the basic bricks of MIPL are reused in an efficient way. In MIPL v2.0, Mobile IPv6 is implemented using multi threads: one for handling the ICMPv6 messages, one for handling Mobility Header messages, and another one for handling tasks and time events.
To support Proxy Mobile IPv6, we extend these elements and implement handlers for all necessary messages and events. All ICMPv6 messages or Mobility Header messages are parsed as inputs to the finite state machine, which is the heart of the system. This finite state machine makes appropriate decisions and controls all other elements to provide a correct predefined protocol behavior. The PMIPv6 binding cache stores all information about MNs' points of attachment and it is kept up-to-date with the mobility of MNs. As Proxy Mobile IPv6 implementation is built on top of MIPL version 2.0, it could be, in the future, easily integrated in MIPL, growing in line with the standards as well as with MIPL source code
*
*

* @section intro5 5.) Details of our real PMIPv6 testbed
*\image html PMIP_realtestbed.jpg "Our real PMIPv6 testbed details"
*
* It is totally based on IPv6. The functionalities developed for LMA and MAGs under MIPL v2.0.2 are used for creating the important entities in the PMIPv6 domain. An unmodified MN, which does not have any specific software for mobility, uses its Netgear wireless card to attach to one of the two Cisco Aironet 1100 series Access Points (APs), which support IEEE 802.11a/g specifications. Each AP is directly connected with a MAG. The implementation of MAG functionalities contains additional features and modifications of MIPL to handle PBU and PBA messages and mobility options, and a modified Router Advertisement daemon (RADVD), which unicasts RAs with a specific HNP per MN. Each MAG is connected to the LMA. The LMA is configured as a modified HA in MIPL which stores the HNP in the BCE for each MN and it is able to handle PBU and PBA messages. Finally, an unmodified CN is connected to the LMA. All the network entities in the test-bed are running Ubuntu with generic Linux kernel.
*
* <hr>
* @section requirements requirements
* @verbinclude requirements Each Access Point in your testbed should have a SYSLOG client feature and MAC address masquerading feature.
* <hr>
*
*/
#ifndef __PMIP_H__
#    define __PMIP_H__
//-----------------------------------------------------------------------------
#   include <unistd.h> // for NULL definition
#   include <linux/types.h>
//-----------------------------------------------------------------------------
#   include <netinet/in.h>
#   include <netinet/ip6.h>
#   include <netinet/icmp6.h>
#   include <netinet/ip6mh.h>
//-----------------------------------------------------------------------------
#   include "mh.h"
//-----------------------------------------------------------------------------
#   include "pmip_cache.h"
#   include "pmip_consts.h"
#   include "pmip_fsm.h"
#   include "pmip_handler.h"
#   include "pmip_hnp_cache.h"
#   include "pmip_init.h"
#   include "pmip_lma_proc.h"
#   include "pmip_mag_proc.h"
#   include "pmip_msgs.h"
#   include "pmip_pcap.h"
#   include "pmip_tunnel.h"
#   include "pmip_types.h"
#   ifdef ENABLE_VT
#       include "vt.h"
#   endif
#   include "conf.h"
#endif
/*@}*/

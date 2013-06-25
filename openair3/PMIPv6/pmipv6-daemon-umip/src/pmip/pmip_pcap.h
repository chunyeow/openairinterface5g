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
/*! \file pmip_pcap.h
* \brief
* \author OpenAir3 Group
* \date 12th of October 2010
* \version 1.0
* \company Eurecom
* \project OpenAirInterface
* \email: openair_tech@eurecom.fr
*/

/** \defgroup PACKET_CAPTURE PACKET CAPTURE
 * \ingroup PMIP6D
 *  PMIP PACKet CAPture (PCAP)
 *  @{
 */

#ifndef __PMIP_PCAP_H__
#    define __PMIP_PCAP_H__
//-----------------------------------------------------------------------------
#    ifdef PMIP_PCAP_C
#        define private_pmip_pcap(x) x
#        define protected_pmip_pcap(x) x
#        define public_pmip_pcap(x) x
#    else
#        ifdef PMIP
#            define private_pmip_pcap(x)
#            define protected_pmip_pcap(x) extern x
#            define public_pmip_pcap(x) extern x
#        else
#            define private_pmip_pcap(x)
#            define protected_pmip_pcap(x)
#            define public_pmip_pcap(x) extern x
#        endif
#    endif
//-----------------------------------------------------------------------------
#    include <pcap.h>
#    include <netinet/ip6.h>
//-----------------------------------------------------------------------------
#    define PCAPMAXBYTES2CAPTURE 65535
#    define PCAPTIMEDELAYKERNEL2USERSPACE 1000
#    define PCAP_CAPTURE_SYSLOG_MESSAGE_FRAME_OFFSET   42
//-VARS----------------------------------------------------------------------------
/*! \var pcap_t * pcap_descr
\brief PCAP descriptor for capturing packets on MAG ingress interface.
*/
public_pmip_pcap(pcap_t * pcap_descr);
/*! \var char* g_pcap_devname
\brief PCAP device name on which the capture of packets is done.
*/
public_pmip_pcap(char g_pcap_devname[32]);
/*! \var int   g_pcap_iif
\brief PCAP device interface index on which the capture of packets is done.
*/
public_pmip_pcap(int   g_pcap_iif);
//-PROTOTYPES----------------------------------------------------------------------------
/*! \fn int pmip_pcap_loop_start(void)
* \brief Start the PCAP listener thread.
* \return 0 on success.
*/
public_pmip_pcap(int pmip_pcap_loop_start(void);)
/*! \fn void pmip_pcap_loop_stop(void)
* \brief Stop the PCAP listener thread.
*/
public_pmip_pcap(void pmip_pcap_loop_stop(void);)
/*! \fn void* pmip_pcap_loop(void*)
* \brief
* \param[in]  devname The name of the device (ex "eth1") that will be listened for capturing packets.
* \param[in]  iif     The interface identifier that will be listened for capturing packets.
*/
public_pmip_pcap(void* pmip_pcap_loop(void*);)
/*! \fn void pmip_pcap_msg_handler_associate(struct in6_addr , int )
* \brief  Construct amessage event telling that a mobile node is associated with the radio technology of the MAG, and send it to the MAG FSM.
* \param[in]  mn_iidP  The MAC address of the mobile node.
* \param[in]  iifP     The interface identifier that is listened for capturing packets.
*/
private_pmip_pcap(void pmip_pcap_msg_handler_associate(struct in6_addr mn_iidP, int iifP);)
/*! \fn void pmip_pcap_msg_handler_deassociate(struct in6_addr , int )
* \brief  Construct amessage event telling that a mobile node is de-associated with the radio technology of the MAG, and send it to the MAG FSM.
* \param[in]  mn_iidP  The MAC address of the mobile node.
* \param[in]  iifP     The interface identifier that is listened for capturing packets.
*/
private_pmip_pcap(void pmip_pcap_msg_handler_deassociate(struct in6_addr mn_iidP, int iifP);)
#endif
/** @}*/



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
#ifndef __PMIP_INIT_H__
#    define __PMIP_INIT_H__
//-----------------------------------------------------------------------------
#    ifdef PMIP_C
#        define private_pmip_init(x) x
#        define protected_pmip_init(x) x
#        define public_pmip_init(x) x
#    else
#        ifdef PMIP
#            define private_pmip_init(x)
#            define protected_pmip_init(x) extern x
#            define public_pmip_init(x) extern x
#        else
#            define private_pmip_init(x)
#            define protected_pmip_init(x)
#            define public_pmip_init(x) extern x
#        endif
#    endif
//-----------------------------------------------------------------------------
//-PROTOTYPES----------------------------------------------------------------------------
/*! \fn void  init_mag_icmp_sock(void)
* \brief Set necessary option on the icmpv6 socket.
* @ingroup  PMIP6D
*/
private_pmip_init(void  init_mag_icmp_sock(void);)
/*! \fn void  pmip_cleanup      (void)
* \brief Release all resources handled by the LMA or MAG entity.
* @ingroup  PMIP6D
*/
public_pmip_init( void  pmip_cleanup      (void);)
/*! \fn int   pmip_common_init  (void)
* \brief Initialization common to LMA and MAGs.
* \return   Status of the initialization, zero if success, else -1.
* @ingroup  PMIP6D
*/
private_pmip_init(int   pmip_common_init  (void);)
/*! \fn int   pmip_mag_init  (void)
* \brief Initialization of the MAG.
* \return   Status of the initialization, zero if success, else -1.
* \note   This function has to be called after pmip_common_init().
* @ingroup  PMIP6D
*/
public_pmip_init( int   pmip_mag_init     (void);)
/*! \fn int   pmip_lma_init  (void)
* \brief Initialization of the LMA.
* \return   Status of the initialization, zero if success, else -1.
* \note   This function has to be called after pmip_common_init().
* @ingroup  PMIP6D
*/
public_pmip_init( int   pmip_lma_init     (void);)
#endif

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
/*! \file pmip_fsm.h
* \brief
* \author OpenAir3 Group
* \date 12th of October 2010
* \version 1.0
* \company Eurecom
* \project OpenAirInterface
* \email: openair_tech@eurecom.fr
*/

/** @defgroup FINITE_STATE_MACHINE FINITE STATE MACHINE
 * @ingroup PMIP6D
 *  PMIP Finite State Machine (FSM)
 *  @{
 */

#ifndef __PMIP_FSM_H__
#    define __PMIP_FSM_H__
//-----------------------------------------------------------------------------
#    ifdef PMIP_FSM_C
#        define private_pmip_fsm(x) x
#        define protected_pmip_fsm(x) x
#        define public_pmip_fsm(x) x
#    else
#        ifdef PMIP
#            define private_pmip_fsm(x)
#            define protected_pmip_fsm(x) extern x
#            define public_pmip_fsm(x) extern x
#        else
#            define private_pmip_fsm(x)
#            define protected_pmip_fsm(x)
#            define public_pmip_fsm(x) extern x
#        endif
#    endif
//-----------------------------------------------------------------------------
#    include <pthread.h>
#    include "pmip_msgs.h"
//-----------------------------------------------------------------------------
/*! \fn int mag_init_fsm(void)
* \brief Initialization of the Finite state machine of the MAG.
* \return   The status of the initialization, 0 for success, else -1.
* \note  Initialization of the mutex only.
*/
protected_pmip_fsm(int mag_init_fsm(void);)

/*! \fn int mag_fsm(msg_info_t *info)
* \brief Finite state machine of the MAG.
* \param[in]  info All informations about the event received.
* \return   0 for success and -1 if error
*/
protected_pmip_fsm(int mag_fsm(msg_info_t *info);)
/*! \fn int lma_fsm(msg_info_t *info)
* \brief Finite state machine of the LMA.
* \param[in]  info All informations about the event received.
* \return   0 for success and -1 if error
*/
protected_pmip_fsm(int lma_fsm(msg_info_t *info);)
/*! \var pthread_rwlock_t fsm_lock
\brief Global var mutex on the MAG finite state machine.
*/
private_pmip_fsm(pthread_rwlock_t fsm_lock;)
#endif
/** @}*/

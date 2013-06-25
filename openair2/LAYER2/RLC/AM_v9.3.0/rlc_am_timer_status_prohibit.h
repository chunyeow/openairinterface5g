/*******************************************************************************

Eurecom OpenAirInterface 2
Copyright(c) 1999 - 2010 Eurecom

This program is free software; you can redistribute it and/or modify it
under the terms and conditions of the GNU General Public License,
version 2, as published by the Free Software Foundation.

This program is distributed in the hope it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

The full GNU General Public License is included in this distribution in
the file called "COPYING".

Contact Information
Openair Admin: openair_admin@eurecom.fr
Openair Tech : openair_tech@eurecom.fr
Forums       : http://forums.eurecom.fsr/openairinterface
Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/
/*! \file rlc_am_timer_status_prohibit.h
* \brief This file defines the prototypes of the functions manipulating the t-StatusProhibit timer.
* \author GAUTHIER Lionel
* \date 2010-2011
* \version
* \note
* \bug
* \warning
*/
/** @defgroup _rlc_am_timers_impl_ RLC AM Timers Reference Implementation
* @ingroup _rlc_am_internal_impl_
* @{
*/
#ifndef __RLC_AM_TIMER_STATUS_PROHIBIT_H__
#    define __RLC_AM_TIMER_STATUS_PROHIBIT_H__
#    ifdef RLC_AM_TIMER_STATUS_PROHIBIT_C
#        define private_rlc_am_timer_status_prohibit(x)    x
#        define protected_rlc_am_timer_status_prohibit(x)  x
#        define public_rlc_am_timer_status_prohibit(x)     x
#    else
#        ifdef RLC_AM_MODULE
#            define private_rlc_am_timer_status_prohibit(x)
#            define protected_rlc_am_timer_status_prohibit(x)  extern x
#            define public_rlc_am_timer_status_prohibit(x)     extern x
#        else
#            define private_rlc_am_timer_status_prohibit(x)
#            define protected_rlc_am_timer_status_prohibit(x)
#            define public_rlc_am_timer_status_prohibit(x)     extern x
#        endif
#    endif

/*! \fn void rlc_am_check_timer_status_prohibit(rlc_am_entity_t *rlcP,u32_t frame)
* \brief      Check if timer status-prohibit has timed-out, if so it is stopped and has the status "timed-out".
* \param[in]  rlcP              RLC AM protocol instance pointer.
* \param[in]  frame             Frame index
*/
protected_rlc_am_timer_status_prohibit(void rlc_am_check_timer_status_prohibit(rlc_am_entity_t *rlcP,u32_t frame));

/*! \fn void rlc_am_stop_and_reset_timer_status_prohibit(rlc_am_entity_t *rlcP)
* \brief      Stop and reset the timer status-prohibit.
* \param[in]  rlcP              RLC AM protocol instance pointer.
* \param[in]  frame             Frame index
*/
protected_rlc_am_timer_status_prohibit(void rlc_am_stop_and_reset_timer_status_prohibit(rlc_am_entity_t *rlcP,u32_t frame);)

/*! \fn void rlc_am_start_timer_status_prohibit(rlc_am_entity_t *rlcP,u32_t frame)
* \brief      Re-arm (based on RLC AM config parameter) and start timer status-prohibit.
* \param[in]  rlcP              RLC AM protocol instance pointer.
* \param[in]  frame             Frame index
*/
protected_rlc_am_timer_status_prohibit(void rlc_am_start_timer_status_prohibit(rlc_am_entity_t *rlcP,u32_t frame);)

/*! \fn void rlc_am_init_timer_status_prohibit(rlc_am_entity_t *rlcP, u32_t time_outP)
* \brief      Initialize the timer status-prohibit with RLC AM time-out config parameter.
* \param[in]  rlcP              RLC AM protocol instance pointer.
* \param[in]  time_outP         Time-out in frame units.
*/
protected_rlc_am_timer_status_prohibit(void rlc_am_init_timer_status_prohibit(rlc_am_entity_t *rlcP, u32_t time_outP);)
/** @} */
#endif

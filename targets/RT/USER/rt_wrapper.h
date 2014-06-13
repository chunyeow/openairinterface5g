/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2011 Eurecom

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

/*! \file rt_wrapper.h
* \brief provides a wrapper for the timing function for real-time opeartions depending on weather RTAI is used or not
* \author F. Kaltenberger
* \date 2013
* \version 0.1
* \company Eurecom
* \email: florian.kaltenberger@eurecom.fr
* \note
* \warning
*/

#ifndef RTAI
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>

#define RTIME long long int

#define rt_printk printf

RTIME rt_get_time_ns (void);

int rt_sleep_ns (RTIME x);

void check_clock(void);

#else
#include <rtai_lxrt.h>
#include <rtai_sem.h>
#include <rtai_msg.h>

int rt_sleep_ns(RTIME x);

#endif

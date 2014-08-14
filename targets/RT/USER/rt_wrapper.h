/*******************************************************************************
    OpenAirInterface 
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is 
    included in this distribution in the file called "COPYING". If not, 
    see <http://www.gnu.org/licenses/>.

   Contact Information
   OpenAirInterface Admin: openair_admin@eurecom.fr
   OpenAirInterface Tech : openair_tech@eurecom.fr
   OpenAirInterface Dev  : openair4g-devel@eurecom.fr
  
   Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.

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
#include <rtai_hal.h>
#include <rtai_lxrt.h>
#include <rtai_sem.h>
#include <rtai_msg.h>

int rt_sleep_ns(RTIME x);

#endif

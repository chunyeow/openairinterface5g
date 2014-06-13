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

#include "rt_wrapper.h"

#ifndef RTAI

struct timespec interval, next, now, res;
clockid_t clock_id = CLOCK_REALTIME; //other options are CLOCK_MONOTONIC, CLOCK_REALTIME, CLOCK_PROCESS_CPUTIME_ID, CLOCK_THREAD_CPUTIME_ID 
RTIME rt_get_time_ns (void) {
  clock_gettime(clock_id, &now);
  return(now.tv_sec*1e9+now.tv_nsec); 
}

int rt_sleep_ns (RTIME x) {
  int ret;
  clock_gettime(clock_id, &now);
  interval.tv_sec = x/((RTIME)1000000000); 
  interval.tv_nsec = x%((RTIME)1000000000); 
  //rt_printk("sleeping for %d sec and %d ns\n",interval.tv_sec,interval.tv_nsec);
  next = now;
  next.tv_sec += interval.tv_sec;
  next.tv_nsec += interval.tv_nsec;

  if (next.tv_nsec>=1000000000) {
    next.tv_nsec -= 1000000000;
    next.tv_sec++;
  }

  ret = clock_nanosleep(clock_id, TIMER_ABSTIME, &next, NULL);

  /*
  if (ret==EFAULT)
    rt_printk("rt_sleep_ns returned EFAULT (%d), reqested %d sec and %d ns\n",ret,next.tv_sec,next.tv_nsec);
  if (ret==EINVAL)
    rt_printk("rt_sleep_ns returned EINVAL (%d), reqested %d sec and %d ns\n",ret,next.tv_sec,next.tv_nsec);
  if (ret==EINTR)
    rt_printk("rt_sleep_ns returned EINTR (%d), reqested %d sec and %d ns\n",ret,next.tv_sec,next.tv_nsec);
  */

  return(ret);
}

void check_clock(void) {
  if (clock_getres(clock_id, &res)) {
    printf("clock_getres failed");
  } else {
    printf("reported resolution = %llu ns\n", (long long int) ((int) 1e9 * res.tv_sec) + (long long int) res.tv_nsec);
  }
}

#else

int rt_sleep_ns(RTIME x) {
  rt_sleep(nano2count(x));
  return(0);
}

#endif

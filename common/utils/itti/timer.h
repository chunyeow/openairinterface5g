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
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

#ifndef TIMER_H_
#define TIMER_H_

#include <signal.h>

#define SIGTIMER SIGRTMIN

typedef enum timer_type_s {
    TIMER_PERIODIC,
    TIMER_ONE_SHOT,
    TIMER_TYPE_MAX,
} timer_type_t;

int timer_handle_signal(siginfo_t *info);

/** \brief Request a new timer
 *  \param interval_sec timer interval in seconds
 *  \param interval_us  timer interval in micro seconds
 *  \param task_id      task id of the task requesting the timer
 *  \param instance     instance of the task requesting the timer
 *  \param type         timer type
 *  \param timer_id     unique timer identifier
 *  @returns -1 on failure, 0 otherwise
 **/
int timer_setup(
    uint32_t      interval_sec,
    uint32_t      interval_us,
    task_id_t     task_id,
    int32_t       instance,
    timer_type_t  type,
    void         *timer_arg,
    long         *timer_id);

/** \brief Remove the timer from list
 *  \param timer_id unique timer id
 *  @returns -1 on failure, 0 otherwise
 **/

int timer_remove(long timer_id);
#define timer_stop timer_remove

/** \brief Initialize timer task and its API
 *  \param mme_config MME common configuration
 *  @returns -1 on failure, 0 otherwise
 **/
int timer_init(void);

#endif

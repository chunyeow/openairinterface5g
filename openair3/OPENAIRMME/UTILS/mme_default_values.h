/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2012 Eurecom

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
  Forums       : http://forums.eurecom.fr/openairinterface
  Address      : EURECOM, Campus SophiaTech, 450 Route des Chappes
                 06410 Biot FRANCE

*******************************************************************************/

#ifndef MME_DEFAULT_VALUES_H_
#define MME_DEFAULT_VALUES_H_

/*******************************************************************************
 * Timer Constants
 ******************************************************************************/
#define MME_TIMER_EXPIRY_MS    (1000)

/*******************************************************************************
 * GTPV1 User Plane Constants
 ******************************************************************************/
#define GTPV1_U_PORT_NUMBER    (2152)

/*******************************************************************************
 * S1AP Constants
 ******************************************************************************/

#define S1AP_PORT_NUMBER (36412) ///< S1AP SCTP IANA ASSIGNED Port Number
#define S1AP_SCTP_PPID   (18)    ///< S1AP SCTP Payload Protocol Identifier (PPID)

/*******************************************************************************
 * SCTP Constants
 ******************************************************************************/

#define SCTP_BUFFER_SIZE (1<<16)

/*******************************************************************************
 * Intertask Interface Constants
 ******************************************************************************/

#define ITTI_PORT               (3632)
#define ITTI_QUEUE_SIZE_MAX     (1 * 1024 * 1024) /* 1 MBytes */

/* Default scheduler policy :
 * SCHED_FIFO : First in first out -> don't break thread execution.
 */
#define ITTI_TASK_SCHED_POLICY  (SCHED_FIFO)

#endif /* MME_DEFAULT_VALUES_H_ */

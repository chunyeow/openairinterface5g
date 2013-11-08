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

/*
 * intertask_interface_conf.h
 *
 *  Created on: Oct 21, 2013
 *      Author: winckel
 */

#ifndef INTERTASK_INTERFACE_CONF_H_
#define INTERTASK_INTERFACE_CONF_H_

/*******************************************************************************
 * Intertask Interface Constants
 ******************************************************************************/

#define ITTI_PORT                (10006)

/* This is the queue size for signal dumper */
#define ITTI_QUEUE_SIZE_MAX      (1 * 1024 * 1024) /* 1 MBytes */
#define ITTI_DUMP_MAX_CON        (5)    /* Max connections in parallel */

#endif /* INTERTASK_INTERFACE_CONF_H_ */

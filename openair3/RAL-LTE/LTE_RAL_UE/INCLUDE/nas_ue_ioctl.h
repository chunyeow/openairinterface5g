/***************************************************************************
                          nas_ue_ioctl.h  -  description
 ***************************************************************************
  Eurecom OpenAirInterface 3
  Copyright(c) 1999 - 2013 Eurecom

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
  Address      : Eurecom, 450 route des Chappes, 06410 Biot Sophia Antipolis, France
*******************************************************************************/
/*! \file nas_ue_ioctl.h
 * \brief This file defines the constants for triggering the ioctl interface
 * \author WETTERWALD Michelle, GAUTHIER Lionel, MAUREL Frederic
 * \date 2013
 * \company EURECOM
 * \email: michelle.wetterwald@eurecom.fr, lionel.gauthier@eurecom.fr, frederic.maurel@eurecom.fr
 */
/*******************************************************************************/
#ifndef _IAL_IOCTL_H
#define _IAL_IOCTL_H

#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <net/if.h>

#include <netinet/in.h>

// Temp constants = arguments gioctl
//arg[0]
#define IO_OBJ_STATS 0
#define IO_OBJ_CNX   1
#define IO_OBJ_RB    2
#define IO_OBJ_MEAS  3
#define IO_OBJ_IMEI  4
//arg[1]
#define IO_CMD_ADD   0
#define IO_CMD_DEL   1
#define IO_CMD_LIST  2



//LGextern int inet_pton(int af, const char *src, void *dst);
//LGextern char *inet_ntop(int af, const void *src, char *dst, size_t sise);

#endif

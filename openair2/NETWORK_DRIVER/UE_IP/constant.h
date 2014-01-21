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


#ifndef _UE_IP_CST
#define _UE_IP_CST

#define UE_IP_MAX_LENGTH 180

// General Constants
#define UE_IP_MTU                    1500
#define UE_IP_TX_QUEUE_LEN           100
#define UE_IP_ADDR_LEN               8
#define UE_IP_INET6_ADDRSTRLEN       46
#define UE_IP_INET_ADDRSTRLEN        16
#define UE_IP_DEFAULT_RAB_ID         5

#define UE_IP_RESET_RX_FLAGS         0


#define UE_IP_RETRY_LIMIT_DEFAULT    5

#define UE_IP_MESSAGE_MAXLEN         5004

#define UE_IP_TIMER_ESTABLISHMENT_DEFAULT 12
#define UE_IP_TIMER_RELEASE_DEFAULT       2
#define UE_IP_TIMER_IDLE                  UINT_MAX
#define UE_IP_TIMER_TICK                  HZ

#define UE_IP_PDCPH_SIZE                  sizeof(struct pdcp_data_req_header_t)
#define UE_IP_IPV4_SIZE                   20
#define UE_IP_IPV6_SIZE                   40




#define UE_IP_NB_INSTANCES_MAX       8


#endif


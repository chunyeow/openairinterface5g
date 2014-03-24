/*******************************************************************************

  Eurecom OpenAirInterface
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
  Forums       : http://forums.eurecom.fr/openairinterface
  Address      : EURECOM, Campus SophiaTech, 450 Route des Chappes
                 06410 Biot FRANCE

*******************************************************************************/

/*! \file sctp_common.h
 *  \brief eNB/MME SCTP related common procedures
 *  \author Sebastien ROUX
 *  \date 2013
 *  \version 1.0
 *  @ingroup _sctp
 */

#ifndef SCTP_COMMON_H_
#define SCTP_COMMON_H_

#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>

#if defined(ENB_MODE)
# include "UTIL/LOG/log.h"
# define SCTP_ERROR(x, args...) LOG_E(SCTP, x, ##args)
# define SCTP_WARN(x, args...)  LOG_W(SCTP, x, ##args)
# define SCTP_DEBUG(x, args...) LOG_I(SCTP, x, ##args)
#else
# define SCTP_ERROR(x, args...) do { fprintf(stderr, "[SCTP][E]"x, ##args); } while(0)
# define SCTP_DEBUG(x, args...) do { fprintf(stdout, "[SCTP][D]"x, ##args); } while(0)
# define SCTP_WARN(x, args...)  do { fprintf(stdout, "[SCTP][W]"x, ##args); } while(0)
#endif

int sctp_set_init_opt(int sd, uint16_t instreams, uint16_t outstreams,
                      uint16_t max_attempts, uint16_t init_timeout);

int sctp_get_sockinfo(int sock, uint16_t *instream, uint16_t *outstream,
                      int32_t *assoc_id);

int sctp_get_peeraddresses(int sock, struct sockaddr **remote_addr,
                           int *nb_remote_addresses);

int sctp_get_localaddresses(int sock, struct sockaddr **local_addr,
                            int *nb_local_addresses);

#endif /* SCTP_COMMON_H_ */

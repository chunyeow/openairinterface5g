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
/** @defgroup _sctp_impl_ SCTP Layer Reference Implementation
 * @ingroup _ref_implementation_
 * @{
 */

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "mme_config.h"

#if !defined(HAVE_LIBSCTP)
# error "You must install libsctp-dev"
#endif

#include <netinet/in.h>
#include <netinet/sctp.h>

#ifndef SCTP_PRIMITIVES_H_
#define SCTP_PRIMITIVES_H_

typedef struct sctp_descriptor_s {
    struct sctp_descriptor_s *next_assoc;       ///< Next association in the list
    struct sctp_descriptor_s *previous_assoc;   ///< Previous association in the list
    int      fd;            ///< Socket descriptor
    uint32_t ppid;          ///< Payload protocol Identifier
    uint16_t instreams;     ///< Number of input streams negociated for this connection
    uint16_t outstreams;    ///< Number of output strams negotiated for this connection
    int32_t  assoc_id;      ///< SCTP association id for the connection
    uint32_t messages_recv; ///< Number of messages received on this connection
    uint32_t messages_sent; ///< Number of messages sent on this connection
    struct   sockaddr_in sin;
} sctp_descriptor_t;

#ifndef SCTP_MAX_INSTREAMS
# define SCTP_MAX_INSTREAMS  64
#endif

#ifndef SCTP_MAX_OUTSTREAMS
# define SCTP_MAX_OUTSTREAMS 64
#endif

#ifndef SCTP_MAX_ATTEMPTS
# define SCTP_MAX_ATTEMPTS   5
#endif

/** \brief SCTP data received callback
 \param buffer pointer to buffer received
 \param length pointer to the length of buffer
 **/
typedef void (*sctp_recv_callback)(uint8_t *buffer, uint32_t length);

/** \brief SCTP Init function. Initialize SCTP layer
 \param NONE
 @returns -1 on error, 0 otherwise.
 **/
int sctp_init(const mme_config_t *mme_config);

#endif /* SCTP_PRIMITIVES_H_ */

/* @} */

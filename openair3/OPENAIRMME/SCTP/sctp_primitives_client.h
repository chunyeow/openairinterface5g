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

#include <stdint.h>
#include <pthread.h>

#ifndef SCTP_PRIMITIVES_CLIENT_H_
#define SCTP_PRIMITIVES_CLIENT_H_

/** @defgroup _sctp_impl_ SCTP Layer Reference Implementation
 * @ingroup _ref_implementation_
 * @{
 */

/** \brief SCTP recv callback prototype. Will be called every time a message is
 * received on socket.
 * \param assocId SCTP association ID
 * \param stream SCTP stream on which data had been received
 * \param buffer Pointer to data (should be freed by user)
 * \param length Length of message received
 * @return Execution result
 */
typedef int (*sctp_recv_callback)(uint32_t  assocId,
                                  uint32_t  stream,
                                  uint8_t  *buffer,
                                  uint32_t  length);

/** \brief SCTP connected callback prototype. Will be called once the
 * association is ready.
 * \param args argument provided by upper layer
 * \param assocId SCTP association ID
 * \param instreams Number of input streams negotiated with remote peer
 * \param outstreams Number of output streams negotiated with remote peer
 * @return Execution result
 */
typedef int (*sctp_connected_callback)(void     *args,
                                       uint32_t  assocId,
                                       uint32_t  instreams,
                                       uint32_t  outstreams);

/** \brief Perform association to a remote peer
 * \param ip_addr Peer IPv4 address
 * \param port Remote port to connect to
 * \param args Upper layer args that will be provided to connected callback
 * \param connected_callback Connected callback
 * \param recv_callback Data received callback
 * @return < 0 in case of failure
 */
int sctp_connect_to_remote_host(
    const char              *ip_addr,
    uint16_t                 port,
    uint32_t                 ppid,
    void                    *args,
    sctp_connected_callback  connected_callback,
    sctp_recv_callback       recv_callback);

/** \brief Send message over SCTP
 * \param assocId SCTP association ID
 * \param stream SCTP stream on which data will be sent
 * \param buffer Pointer to buffer
 * \param length Buffer length
 * @return < 0 in case of failure
 */
int sctp_send_msg(
    uint32_t assocId,
    uint16_t stream,
    const uint8_t *buffer,
    const uint32_t length);

void sctp_disconnect(uint32_t assocId);

void sctp_terminate(void);

/* @} */
#endif /* SCTP_PRIMITIVES_CLIENT_H_ */

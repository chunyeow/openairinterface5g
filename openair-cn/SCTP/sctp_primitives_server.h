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

/*! \file sctp_primitives_server.c
 *  \brief Main server primitives
 *  \author Sebastien ROUX
 *  \date 2013
 *  \version 1.0
 *  @ingroup _sctp
 *  @{
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

#ifndef SCTP_PRIMITIVES_SERVER_H_
#define SCTP_PRIMITIVES_SERVER_H_

/** \brief SCTP data received callback
 \param buffer pointer to buffer received
 \param length pointer to the length of buffer
 **/
typedef void (*sctp_recv_callback)(uint8_t *buffer, uint32_t length);

/** \brief SCTP Init function. Initialize SCTP layer
 \param mme_config The global MME configuration structure
 @returns -1 on error, 0 otherwise.
 **/
int sctp_init(const mme_config_t *mme_config_p);

#endif /* SCTP_PRIMITIVES_SERVER_H_ */

/* @} */

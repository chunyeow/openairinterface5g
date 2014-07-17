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
#ifndef SCTP_MESSAGES_TYPES_H_
#define SCTP_MESSAGES_TYPES_H_

#define SCTP_DATA_IND(mSGpTR)           (mSGpTR)->ittiMsg.sctp_data_ind
#define SCTP_DATA_REQ(mSGpTR)           (mSGpTR)->ittiMsg.sctp_data_req
#define SCTP_INIT_MSG(mSGpTR)           (mSGpTR)->ittiMsg.sctpInit
#define SCTP_CLOSE_ASSOCIATION(mSGpTR)  (mSGpTR)->ittiMsg.sctp_close_association

typedef struct sctp_data_req_s {
    uint8_t  *buffer;
    uint32_t  bufLen;
    uint32_t  assocId;
    uint16_t  stream;
} sctp_data_req_t;

typedef struct sctp_data_ind_s {
    uint8_t  *buffer;           ///< SCTP buffer
    uint32_t  buf_length;       ///< SCTP buffer length
    int32_t   assoc_id;         ///< SCTP physical association ID
    uint8_t   stream;           ///< Stream number on which data had been received
    uint16_t  instreams;        ///< Number of input streams for the SCTP connection between peers
    uint16_t  outstreams;       ///< Number of output streams for the SCTP connection between peers
} sctp_data_ind_t;

typedef struct sctp_init_s {
    /* Request usage of ipv4 */
    unsigned  ipv4:1;
    /* Request usage of ipv6 */
    unsigned  ipv6:1;
    uint8_t   nb_ipv4_addr;
    uint32_t  ipv4_address[10];
    uint8_t   nb_ipv6_addr;
    char     *ipv6_address[10];
    uint16_t  port;
    uint32_t  ppid;
} SctpInit;

typedef struct sctp_close_association_s {
    uint32_t  assoc_id;
} sctp_close_association_t;

typedef struct sctp_new_peer_s {
    uint32_t instreams;
    uint32_t outstreams;
    uint32_t assoc_id;
} sctp_new_peer_t;

#endif /* SCTP_MESSAGES_TYPES_H_ */

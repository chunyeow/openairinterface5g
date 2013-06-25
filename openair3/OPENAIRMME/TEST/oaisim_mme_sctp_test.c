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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>

#include "sctp_primitives_client.h"

#include "s1ap_common.h"
#include "s1ap_eNB.h"
#include "s1ap_mme.h"
#include "s1ap_ies_defs.h"

#include "s1ap_eNB_encoder.h"
#include "s1ap_eNB_decoder.h"

int recv_callback(uint32_t  assocId,
                  uint32_t  stream,
                  uint8_t  *buffer,
                  uint32_t  length) {

    free(buffer);

    return 0;
}

int sctp_connected(void     *args,
                   uint32_t  assocId,
                   uint32_t  instreams,
                   uint32_t  outstreams) {

    return 0;
}

int main(int argc, char *argv[])
{
    asn_enc_rval_t retVal;

    int i;

    for (i = 0; i < nb_eNB; i++) {
        sctp_connect_to_remote_host(ip_addr, 36412, NULL, sctp_connected, recv_callback);
    }
    while (1) {
        sleep(1);
    }

    sctp_terminate();

    return(0);
}
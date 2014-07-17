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
#include "config.h"

#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdcore.h>

#include "test_utils.h"
#include "test_fd.h"

extern int fd_ext_add( char * filename, char * conffile );

void s6a_fd_init(void)
{
    struct peer_info peer;

    fd_g_debug_lvl = NONE;

    memset(&peer, 0, sizeof(struct peer_info));

    peer.pi_diamid = "hss.test.fr";
    peer.pi_diamidlen = strlen(peer.pi_diamid);

    /* Only SCTP */
    peer.config.pic_flags.pro4 = PI_P4_SCTP;
    peer.config.pic_flags.sec  = PI_SEC_NONE;
    peer.config.pic_flags.exp  = PI_EXP_NONE;

    peer.config.pic_port = 18678;

    if (fd_core_initialize() != 0) {
        fail("fd_core_initialize failed");
    }
    if (fd_core_start() != 0) {
        fail("fd_core_start failed");
    }
    if (fd_core_parseconf("../../conf/hss_fd.conf") != 0) {
        fail("fd_core_waitstartcomplete failed");
    }
    if (fd_core_waitstartcomplete() != 0) {
        fail("fd_core_waitstartcomplete failed");
    }
//     if (fd_peer_add(&peer, NULL, NULL, NULL) != 0) {
//         fail("fd_peer_add failed");
//     }
}

void s6a_fd_stop(void)
{
    if (fd_core_shutdown() != 0) {
        fail("fd_core_shutdown failed");
    }
    if (fd_core_wait_shutdown_complete() != 0) {
        fail("fd_core_shutdown failed");
    }
}

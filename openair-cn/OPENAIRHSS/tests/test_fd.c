
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

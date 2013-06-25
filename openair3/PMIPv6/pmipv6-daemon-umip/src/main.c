/*
 * $Id: main.c 1.67 06/05/05 19:40:57+03:00 anttit@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 * 
 * Authors: Antti Tuominen <anttit@tcs.hut.fi>
 *          Ville Nuorvala <vnuorval@tcs.hut.fi>
 *
 * Copyright 2003-2005 Go-Core Project
 * Copyright 2003-2006 Helsinki University of Technology
 *
 * MIPL Mobile IPv6 for Linux is free software; you can redistribute
 * it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; version 2 of
 * the License.
 *
 * MIPL Mobile IPv6 for Linux is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MIPL Mobile IPv6 for Linux; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>
#include <netinet/icmp6.h>

#include "conf.h"
#include "cn.h"
#include "ha.h"
#include "mn.h"
#include "mh.h"
#include "keygen.h"
#include "debug.h"
#include "bcache.h"
#include "policy.h"
#include "xfrm.h"
#include "icmp6.h"
#ifdef ENABLE_VT
#include "vt.h"
#endif
#include "tunnelctl.h"
#include "statistics.h"
#include "pmip.h"

static void sig_child(__attribute__ ((unused)) int unused)
{
	int pid, status;

	while ((pid = waitpid(0, &status, WNOHANG)) > 0);
}

static void reinit(void)
{
	/* got SIGHUP, reread configuration and reinitialize */
	dbg("got SIGHUP, reinitilize\n");
	return;
}


struct mip6_config conf;
struct mip6_stat mipl_stat;

static void terminate(void)
{
	/* got SIGINT, cleanup and exit */
	syslog(LOG_INFO, "terminated (SIGINT)");
	dbg("got SIGINT, exiting\n");
	pmip_cleanup();
	debug_close();
	pthread_exit(NULL);
}

/*
 * Detach from any tty.
 */
static void daemon_start(int ignsigcld)
{
	register int childpid, fd;

	if (getppid() == 1) goto out;

#ifdef SIGTTOU
	signal(SIGTTOU, SIG_IGN);
#endif
#ifdef SIGTTIN
	signal(SIGTTIN, SIG_IGN);
#endif
#ifdef SIGTSTP
	signal(SIGTSTP, SIG_IGN);
#endif

	if ((childpid = fork()) < 0)
		fprintf(stderr, "can't fork first child\n");
	else if (childpid > 0)
		exit(0);

	if (setpgrp() == -1)
		fprintf(stderr, "can't change process group\n");
	if ((fd = open("/dev/tty", O_RDWR)) >= 0) {
		ioctl(fd, TIOCNOTTY, (char *)NULL);
		close(fd);
	}

 out:
	for (fd = 0; fd < NOFILE; fd++) close(fd);
	errno = 0;

	chdir("/tmp");
	umask(0);

	if (ignsigcld) {
#ifdef SIGTSTP
		signal(SIGCLD, sig_child);
#else
		signal(SIGCLD, SIG_IGN);
#endif
	}
}

static void *sigh(__attribute__ ((unused)) void *arg)
{
	int signum;
	sigset_t sigcatch;

	pthread_dbg("thread started");

	sigemptyset(&sigcatch);
	sigaddset(&sigcatch, SIGHUP);
	sigaddset(&sigcatch, SIGINT);
	sigaddset(&sigcatch, SIGTERM);
#ifdef ENABLE_VT
	sigaddset(&sigcatch, SIGPIPE);
#endif

	for ( ;; ) {
		sigwait(&sigcatch, &signum);
		switch (signum) {
		case SIGHUP:
			reinit();
			break;
		case SIGINT:
		case SIGTERM:
#ifdef ENABLE_VT
		case SIGPIPE:
#endif
			terminate();
		default:
			break;
		}
	}
	pthread_exit(NULL);
}

const char *entity_string[4] = {
	"Correspondent Node",
	"Mobile Node",
	"Home Agent-Local Mobility Anchor",
	"Mobile Access Gateway"};

int main(int argc, char **argv)
{
	pthread_t sigth;
	sigset_t sigblock;
	int logflags = 0;
	int ret = 1;

	debug_init();

	sigemptyset(&sigblock);
	sigaddset(&sigblock, SIGHUP);
	sigaddset(&sigblock, SIGINT);
	sigaddset(&sigblock, SIGTERM);
#ifdef ENABLE_VT
	sigaddset(&sigblock, SIGPIPE);
#endif
	pthread_sigmask(SIG_BLOCK, &sigblock, NULL);

	if (conf_parse(&conf, argc, argv))
		return 1;

	if (conf.debug_level > 0)
		logflags = LOG_PERROR;

	openlog(basename(argv[0]), LOG_PID|logflags, LOG_DAEMON);

	syslog(LOG_INFO, "%s v%s started (%s)", PACKAGE_NAME, PACKAGE_VERSION,
	       entity_string[conf.mip6_entity]);
#ifdef ENABLE_VT
	if (vt_init() < 0)
		goto vt_failed;
#endif

	/* if not debugging, detach from tty */
	if (conf.debug_level == 0)
		daemon_start(1);
	else {
		/* if debugging with debug log file, detach from tty */
		if (conf.debug_log_file) {
			daemon_start(1);

			ret = debug_open(conf.debug_log_file);
			if (ret < 0) {
				fprintf(stderr, "can't init debug log:%s\n",
					strerror(-ret));
				goto debug_failed;
			}
			dbg("%s started in debug mode\n", PACKAGE_NAME);
		} else {
			dbg("%s started in debug mode, not detaching from terminal\n",
			    PACKAGE_NAME);
		}
		conf_show(&conf);
	}

	srandom(time(NULL));

	if (rr_cn_init() < 0)
		goto rr_cn_failed;
	if (policy_init() < 0)
		goto policy_failed;
	if (taskqueue_init() < 0)
		goto taskqueue_failed;
	if (bcache_init() < 0)
		goto bcache_failed;
	if (mh_init() < 0)
		goto mh_failed;
	if (icmp6_init() < 0)
		goto icmp6_failed;
	if (!(is_ha()) &&  !(is_mag()))
	if (xfrm_init() < 0)
		goto xfrm_failed;
	cn_init();
	if ((is_ha() || is_mn() ||  is_mag()) && tunnelctl_init() < 0)
		goto tunnelctl_failed;
	if (is_ha() && ha_init() < 0)
		goto pmip_failed;
		//goto ha_failed;
	if (is_mn() && mn_init() < 0)
		goto mn_failed;
	if (is_mag() && pmip_mag_init() < 0)
		goto pmip_failed;

#ifdef ENABLE_VT
	if (vt_start(conf.vt_hostname, conf.vt_service) < 0)
		goto vt_start_failed;
#endif
	if (pthread_create(&sigth, NULL, sigh, NULL))
		goto sigth_failed;
	pthread_join(sigth, NULL);
	ret = 0;
sigth_failed:
#ifdef ENABLE_VT
	vt_fini();
vt_start_failed:
#endif
	if (is_mn())
		mn_cleanup();
pmip_failed:
	pmip_cleanup();
mn_failed:
	if (is_ha())
		ha_cleanup();
ha_failed:
	if (is_ha() || is_mn())
		tunnelctl_cleanup();
tunnelctl_failed:
	cn_cleanup();
	xfrm_cleanup();
xfrm_failed:
	icmp6_cleanup();
icmp6_failed:
	mh_cleanup();
mh_failed:
	bcache_cleanup();
bcache_failed:
	taskqueue_destroy();
taskqueue_failed:
	policy_cleanup();
policy_failed:
rr_cn_failed:
	debug_close();
debug_failed:
#ifdef ENABLE_VT
vt_failed:
#endif
	syslog(LOG_INFO, "%s v%s stopped (%s)", PACKAGE_NAME, PACKAGE_VERSION,
	       entity_string[conf.mip6_entity]);
	closelog();
	return ret;
}

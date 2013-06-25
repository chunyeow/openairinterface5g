/*
 * Copyright (C)2004,2005 USAGI/WIDE Project
 * Copyright (C)2005 Go-Core Project
 * Copyright (C)2005,2006 Helsinki University of Technology
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
 * Authors:
 *	Noriaki TAKAMIYA @USAGI
 *	Masahide NAKAMURA @USAGI
 *	Ville Nuorvala @HUT
 */

/*
 * VT server performs select(2) and only one client access is allowed.
 * To be accept multiple connect, fix "vt_connect_handle".
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <pthread.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <netdb.h>
#include <string.h>
#include <netinet/ip6mh.h>

#include "list.h"
#include "util.h"
#include "debug.h"
#include "conf.h"
#include "vt.h"
#include "mh.h"
#include "bul.h"
#include "retrout.h"
#include "bcache.h"
#include "prefix.h"
#include "ha.h"
#include "cn.h"
#include "mn.h"
#include "mpdisc_mn.h"
#include "mpdisc_ha.h"
#include "statistics.h"
#include "pmip_cache.h"

#define VT_PKT_BUFLEN		(8192)
#define VT_REPLY_BUFLEN		(LINE_MAX)
#define VT_SERVER_BACKLOG	(1)
#define VT_CMD_PROMPT		("mip6d> ")
#define VT_CMD_HELP_STR		("help")
#define VT_CMD_HELP_LINE_MAX	(60)

struct vt_server_entry {
	struct list_head list;
	int vse_sock;
	union {
		struct sockaddr sa;
		struct sockaddr_storage ss;
	} vse_sockaddr;
	socklen_t vse_sockaddrlen;

#define vse_sa		vse_sockaddr.sa
#define vse_salen	vse_sockaddrlen
};

static pthread_rwlock_t vt_lock;
static pthread_t vt_listener;

static struct vt_cmd_entry vt_cmd_root;

static LIST_HEAD(vt_server_list);
static struct vt_handle *vt_connect_handle = NULL;

static int vt_server_fini(void);
static int vt_connect_close(struct vt_handle *vh);

/* Find a handle which is able to be modified */
static struct vt_handle *vt_handle_get(void)
{
	return vt_connect_handle;
}

static int vt_handle_full(void)
{
	if (vt_connect_handle != NULL)
		return 1;
	else
		return 0;
}

static int vt_handle_add(struct vt_handle *vh)
{
	if (vt_connect_handle != NULL) {
		return -EINVAL;
	}
	vt_connect_handle = vh;
	return 0;
}

#define VTDECOR_B_START vh->vh_opt.fancy == VT_BOOL_TRUE ? "\033[1m" : ""
#define VTDECOR_BU_START vh->vh_opt.fancy == VT_BOOL_TRUE ? "\033[1;4m" : ""
#define VTDECOR_END vh->vh_opt.fancy == VT_BOOL_TRUE ? "\033[0m" : ""

ssize_t fprintf_decor(int decor, const struct vt_handle *vh, 
		      const char *fmt, ...)
{
	char buf[VT_REPLY_BUFLEN];
	va_list ap;

	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);

	return fprintf(vh->vh_stream, "%s%s%s", 
		       decor == VTDECOR_B ? VTDECOR_B_START :VTDECOR_BU_START, 
		       buf, VTDECOR_END);
}

static const char *yes = "yes";
static const char *no = "no";

static const char *bool_str(vt_bool_t b)
{
	return b == VT_BOOL_TRUE ? yes : no;
}

static int vt_cmd_sys_fancy_off(const struct vt_handle *vh,
				__attribute__ ((unused)) const char *str)
{
	struct vt_handle *sysvh = vt_handle_get();
	sysvh->vh_opt.fancy = VT_BOOL_FALSE;
	fprintf(vh->vh_stream, "%s\n", bool_str(sysvh->vh_opt.fancy));
	return 0;
}

static int vt_cmd_sys_fancy_on(const struct vt_handle *vh,
			       __attribute__ ((unused)) const char *str)
{
	struct vt_handle *sysvh = vt_handle_get();
	sysvh->vh_opt.fancy = VT_BOOL_TRUE;
	fprintf(vh->vh_stream, "%s\n", bool_str(sysvh->vh_opt.fancy));
	return 0;
}

static int vt_cmd_sys_fancy(const struct vt_handle *vh,
			    __attribute__ ((unused)) const char *str)
{
	struct vt_handle *sysvh = vt_handle_get();
	fprintf(vh->vh_stream, "%s\n", bool_str(sysvh->vh_opt.fancy));
	return 0;
}

static int vt_cmd_sys_verbose_off(const struct vt_handle *vh,
				  __attribute__ ((unused)) const char *str)
{
	struct vt_handle *sysvh = vt_handle_get();
	sysvh->vh_opt.verbose = VT_BOOL_FALSE;
	fprintf(vh->vh_stream, "%s\n", bool_str(sysvh->vh_opt.verbose));
	return 0;
}

static int vt_cmd_sys_verbose_on(const struct vt_handle *vh,
				 __attribute__ ((unused)) const char *str)
{
	struct vt_handle *sysvh = vt_handle_get();
	sysvh->vh_opt.verbose = VT_BOOL_TRUE;
	fprintf(vh->vh_stream, "%s\n", bool_str(sysvh->vh_opt.verbose));
	return 0;
}

static int vt_cmd_sys_verbose(const struct vt_handle *vh,
			      __attribute__ ((unused)) const char *str)
{
	struct vt_handle *sysvh = vt_handle_get();
	fprintf(vh->vh_stream, "%s\n", bool_str(sysvh->vh_opt.verbose));
	return 0;
}

static int vt_cmd_sys_prompt_off(const struct vt_handle *vh,
				 __attribute__ ((unused)) const char *str)
{
	struct vt_handle *sysvh = vt_handle_get();
	sysvh->vh_opt.prompt = VT_BOOL_FALSE;
	fprintf(vh->vh_stream, "%s\n", bool_str(sysvh->vh_opt.prompt));
	return 0;
}

static int vt_cmd_sys_prompt_on(const struct vt_handle *vh,
				__attribute__ ((unused)) const char *str)
{
	struct vt_handle *sysvh = vt_handle_get();
	sysvh->vh_opt.prompt = VT_BOOL_TRUE;
	fprintf(vh->vh_stream, "%s\n", bool_str(sysvh->vh_opt.prompt));
	return 0;
}

static int vt_cmd_sys_prompt(const struct vt_handle *vh,
			     __attribute__ ((unused)) const char *str)
{
	struct vt_handle *sysvh = vt_handle_get();
	fprintf(vh->vh_stream, "%s\n", bool_str(sysvh->vh_opt.prompt));
	return 0;
}

static int vt_cmd_showstats(const struct vt_handle *vh, __attribute__ ((unused)) const char *str)
{
	pthread_mutex_lock(&mipl_stat.lock);
	fprintf(vh->vh_stream, "Input Statistics:\n");
	fprintf(vh->vh_stream, "     %lu Mobility Headers\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_MH]);
	fprintf(vh->vh_stream, "     %lu HoTI messages\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_HOTI]);
	fprintf(vh->vh_stream, "     %lu CoTI messages\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_COTI]);
	fprintf(vh->vh_stream, "     %lu HoT messages\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_HOT]);
	fprintf(vh->vh_stream, "     %lu CoT messages\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_COT]);
	fprintf(vh->vh_stream, "     %lu BU messages\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_BU]);
	fprintf(vh->vh_stream, "     %lu BA messages\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_BA]);
	fprintf(vh->vh_stream, "     %lu BR messages\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_BRR]);
	fprintf(vh->vh_stream, "     %lu BE messages\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_BE]);

	fprintf(vh->vh_stream, "     %lu DHAAD request\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_DHAAD_REQ]);
	fprintf(vh->vh_stream, "     %lu DHAAD reply\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_DHAAD_REP]);
	fprintf(vh->vh_stream, "     %lu MPA\n", 0L);
	fprintf(vh->vh_stream, "     %lu MPS\n", 0L);
	fprintf(vh->vh_stream, "     %lu Home Address Option\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_HAO]);
	fprintf(vh->vh_stream, "     %lu unverified Home Address Option\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_X_HAO]);
	fprintf(vh->vh_stream, "     %lu Routing Header type 2\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_RH2]);

	fprintf(vh->vh_stream, "     %lu reverse tunnel input\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_RTUN]);
	fprintf(vh->vh_stream, "     %lu bad MH checksum\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_X_MH_CHK]);
	fprintf(vh->vh_stream, "     %lu bad payload protocol\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_X_PAYLOAD]);
	fprintf(vh->vh_stream, "     %lu unknown MH type\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_UNKNOWN_MH]);
	fprintf(vh->vh_stream, "     %lu not my home address\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_NOT_HOA]);
	fprintf(vh->vh_stream, "     %lu no related binding update entry\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_NO_BU]);
	fprintf(vh->vh_stream, "     %lu home init cookie mismatch\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_X_H_COOKIE]);
	fprintf(vh->vh_stream, "     %lu careof init cookie mismatch\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_X_C_COOKIE]);
	fprintf(vh->vh_stream, "     %lu unprotected binding signaling packets\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_UNSAFEBU]);
	fprintf(vh->vh_stream, "     %lu BUs discarded due to bad HAO\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_X_BU_HAO]);
	fprintf(vh->vh_stream, "     %lu RR authentication failed\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_RR_FAIL]);
	fprintf(vh->vh_stream, "     %lu seqno mismatch\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_X_SEQ]);
	fprintf(vh->vh_stream, "     %lu parameter problem for HAO\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_X_HAO_PARAM]);
	fprintf(vh->vh_stream, "     %lu parameter problem for MH\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_X_MH_PARAM]);
	fprintf(vh->vh_stream, "     %lu Invalid Care-of address\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_X_COA]);
	fprintf(vh->vh_stream, "     %lu Invalid mobility options\n",
		   	mipl_stat.values[MIPL_STATISTICS_IN_X_MOBOPT]);

	fprintf(vh->vh_stream, "Output Statistics:\n");

	fprintf(vh->vh_stream, "     %lu Mobility Headers\n",
		   	mipl_stat.values[MIPL_STATISTICS_OUT_MH]);
	fprintf(vh->vh_stream, "     %lu HoTI messages\n",
		   	mipl_stat.values[MIPL_STATISTICS_OUT_HOTI]);
	fprintf(vh->vh_stream, "     %lu CoTI messages\n",
		   	mipl_stat.values[MIPL_STATISTICS_OUT_COTI]);
	fprintf(vh->vh_stream, "     %lu HoT messages\n",
		   	mipl_stat.values[MIPL_STATISTICS_OUT_HOT]);
	fprintf(vh->vh_stream, "     %lu CoT messages\n",
		   	mipl_stat.values[MIPL_STATISTICS_OUT_COT]);
	fprintf(vh->vh_stream, "     %lu BU messages\n",
		   	mipl_stat.values[MIPL_STATISTICS_OUT_BU]);
	fprintf(vh->vh_stream, "     %lu BA messages\n",
		   	mipl_stat.values[MIPL_STATISTICS_OUT_BA]);
	fprintf(vh->vh_stream, "     %lu BR messages\n",
		   	mipl_stat.values[MIPL_STATISTICS_OUT_BRR]);
	fprintf(vh->vh_stream, "     %lu BE messages\n",
		   	mipl_stat.values[MIPL_STATISTICS_OUT_BE]);

	fprintf(vh->vh_stream, "     %lu DHAAD request\n",
		   	mipl_stat.values[MIPL_STATISTICS_OUT_DHAAD_REQ]);
	fprintf(vh->vh_stream, "     %lu DHAAD reply\n",
		   	mipl_stat.values[MIPL_STATISTICS_OUT_DHAAD_REP]);
	fprintf(vh->vh_stream, "     %lu MPA\n",
		   	mipl_stat.values[MIPL_STATISTICS_OUT_MPA]);
	fprintf(vh->vh_stream, "     %lu MPS\n",
		   	mipl_stat.values[MIPL_STATISTICS_OUT_MPS]);
	fprintf(vh->vh_stream, "     %lu Home Address Option\n",
		   	mipl_stat.values[MIPL_STATISTICS_OUT_HAO]);
	fprintf(vh->vh_stream, "     %lu Routing Header type 2\n",
		   	mipl_stat.values[MIPL_STATISTICS_OUT_RH2]);
	fprintf(vh->vh_stream, "     %lu reverse tunneled input\n",
		   	mipl_stat.values[MIPL_STATISTICS_OUT_RTUN]);

	pthread_mutex_unlock(&mipl_stat.lock);

	return 0;
}

static int vt_cmd_sys_quit(const struct vt_handle *vh,
			   __attribute__ ((unused)) const char *str)
{
	struct vt_handle *sysvh = vt_handle_get();

	if (strlen(str) > 0) {
		int ret = fprintf(vh->vh_stream, "unknown args\n");
		if (ret < 0)
			return ret;
		return 0;
	}

	vt_connect_close(sysvh);
	return 0;
}

static struct vt_cmd_entry vt_cmd_quit = {
	.cmd = "quit",
	.cmd_alias = "exit",
	.parser = vt_cmd_sys_quit,
};

static struct vt_cmd_entry vt_cmd_prompt = {
	.cmd = "prompt",
	.parser = vt_cmd_sys_prompt,
};

static struct vt_cmd_entry vt_cmd_prompt_on = {
	.cmd = "yes",
	.cmd_alias = "y",
	.parser = vt_cmd_sys_prompt_on,
};

static struct vt_cmd_entry vt_cmd_prompt_off = {
	.cmd = "no",
	.cmd_alias = "n",
	.parser = vt_cmd_sys_prompt_off,
};

static struct vt_cmd_entry vt_cmd_verbose = {
	.cmd = "verbose",
	.parser = vt_cmd_sys_verbose,
};

static struct vt_cmd_entry vt_cmd_verbose_on = {
	.cmd = "yes",
	.cmd_alias = "y",
	.parser = vt_cmd_sys_verbose_on,
};

static struct vt_cmd_entry vt_cmd_verbose_off = {
	.cmd = "no",
	.cmd_alias = "n",
	.parser = vt_cmd_sys_verbose_off,
};

static struct vt_cmd_entry vt_cmd_fancy = {
	.cmd = "fancy",
	.parser = vt_cmd_sys_fancy,
};

static struct vt_cmd_entry vt_cmd_fancy_on = {
	.cmd = "yes",
	.cmd_alias = "y",
	.parser = vt_cmd_sys_fancy_on,
};

static struct vt_cmd_entry vt_cmd_fancy_off = {
	.cmd = "no",
	.cmd_alias = "n",
	.parser = vt_cmd_sys_fancy_off,
};

static struct vt_cmd_entry vt_cmd_stats = {
	.cmd = "stats",
	.parser = vt_cmd_showstats,
};

static int vt_cmd_sys_init(void)
{
	int ret;

	INIT_LIST_HEAD(&vt_cmd_root.list);
	vt_cmd_root.parent = NULL;
	INIT_LIST_HEAD(&vt_cmd_root.child_list);

	ret = vt_cmd_add_root(&vt_cmd_quit);
	if (ret < 0)
		return ret;

	ret = vt_cmd_add_root(&vt_cmd_prompt);
	if (ret < 0)
		return ret;
	ret = vt_cmd_add(&vt_cmd_prompt, &vt_cmd_prompt_on);
	if (ret < 0)
		return ret;
	ret = vt_cmd_add(&vt_cmd_prompt, &vt_cmd_prompt_off);
	if (ret < 0)
		return ret;

	ret = vt_cmd_add_root(&vt_cmd_verbose);
	if (ret < 0)
		return ret;
	ret = vt_cmd_add(&vt_cmd_verbose, &vt_cmd_verbose_on);
	if (ret < 0)
		return ret;
	ret = vt_cmd_add(&vt_cmd_verbose, &vt_cmd_verbose_off);
	if (ret < 0)
		return ret;

	ret = vt_cmd_add_root(&vt_cmd_fancy);
	if (ret < 0)
		return ret;
	ret = vt_cmd_add(&vt_cmd_fancy, &vt_cmd_fancy_on);
	if (ret < 0)
		return ret;
	ret = vt_cmd_add(&vt_cmd_fancy, &vt_cmd_fancy_off);
	if (ret < 0)
		return ret;

	ret = vt_cmd_add_root(&vt_cmd_stats);
	if (ret < 0)
		return ret;

	return 0;
}

static int vt_cmd_dump_candidates(const struct vt_handle *vh,
				  const struct vt_cmd_entry *ent, int line_max)
{
	const char sep[] = " ";
	const int seplen = strlen(sep);
	int llen = 0;
	int n = 0;
	int ret;
	struct list_head *lp;

	list_for_each (lp, &ent->child_list) {
		const struct vt_cmd_entry *e;
		int cmdlen;

		e = list_entry(lp, const struct vt_cmd_entry, list);

		if (!e->cmd)
			continue;

		cmdlen = strlen(e->cmd);

		/* check whether new-line is needed or not */
		if (n > 0) {
			if (llen + seplen + cmdlen >= line_max) {
				ret = fprintf(vh->vh_stream, "\n");
				llen = 0;
			} else {
				/* add a separator */
				ret = fprintf(vh->vh_stream, sep);
				llen += seplen;
			}
			if (ret < 0)
				return ret;
		}
		llen += cmdlen;

		ret = fprintf(vh->vh_stream, e->cmd);
		if (ret < 0)
			return ret;

		n++;
	}

	ret = fprintf(vh->vh_stream, "\n");
	if (ret < 0)
		return ret;

	return 0;
}


struct bul_vt_arg {
	const struct vt_handle *vh;
};

static int bul_vt_dump(void *data, void *arg)
{
	struct bulentry *bule = (struct bulentry *)data;
	struct bul_vt_arg *bva = (struct bul_vt_arg *)arg;
	const struct vt_handle *vh = bva->vh;
	struct timespec ts_now;
	int ts_now_broken = 0;

	dump_bule(data, vh->vh_stream);

	fprintf(vh->vh_stream, " ack %s", (bule->wait_ack) ? "wait" : "ready");

	if (!(bule->flags & IP6_MH_BU_HOME)) {
		fprintf(vh->vh_stream, " RR state %s",
			  (bule->rr.state == RR_READY) ? "ready" :
			  (bule->rr.state == RR_STARTED) ? "started" :
			  (bule->rr.state == RR_H_EXPIRED) ?
			  "home nonce expired" :
			  (bule->rr.state == RR_C_EXPIRED) ?
			  "care-of nonce expired" :
			  (bule->rr.state == RR_C_EXPIRED) ?
			  "care-of nonce expired" :
			  (bule->rr.state == RR_C_EXPIRED) ? "expired" :
			  (bule->rr.state == RR_C_EXPIRED) ? "none" : "?");
	}
	fprintf(vh->vh_stream, "\n");

	if (vh->vh_opt.verbose == VT_BOOL_TRUE) {
		char buf[IF_NAMESIZE + 1];
		char *dev = if_indextoname(bule->if_coa, buf);

		fprintf(vh->vh_stream, " dev ");

		if (!dev || strlen(dev) == 0)
			fprintf(vh->vh_stream, "(%d)", bule->if_coa);
		else
			fprintf(vh->vh_stream, "%s", dev);

		fprintf(vh->vh_stream, " last_coa %x:%x:%x:%x:%x:%x:%x:%x",
			  NIP6ADDR(&bule->last_coa));

		fprintf(vh->vh_stream, "\n");

		if (!(bule->flags & IP6_MH_BU_HOME)) {
			fprintf(vh->vh_stream, "care-of nonce index %u",
				  bule->rr.co_ni);

			fprintf(vh->vh_stream, "home nonce index %u",
				  bule->rr.ho_ni);

			fprintf(vh->vh_stream, "\n");
		}
	}
	if (clock_gettime(CLOCK_REALTIME, &ts_now) != 0)
		ts_now_broken = 1;

	fprintf(vh->vh_stream, " lifetime ");
	if (!ts_now_broken) {
		if (tsafter(ts_now, bule->lastsent))
			fprintf(vh->vh_stream, "(broken)");
		else {
			struct timespec ts;

			tssub(ts_now, bule->lastsent, ts);
			tssub(bule->lifetime, ts, ts);
			fprintf(vh->vh_stream, "%ld", ts.tv_sec);
		}
	} else
		fprintf(vh->vh_stream, "(error)");

	fprintf(vh->vh_stream, " / %ld", bule->lifetime.tv_sec);

	fprintf(vh->vh_stream, " seq %u", bule->seq);

	fprintf(vh->vh_stream, " resend %d", bule->consecutive_resends);
	fprintf(vh->vh_stream, " delay %ld(after %lds)", bule->delay.tv_sec,
		  bule->lastsent.tv_sec +
		  bule->delay.tv_sec - ts_now.tv_sec);
	if (vh->vh_opt.verbose == VT_BOOL_TRUE) {
		fprintf(vh->vh_stream, " expires ");
		if (tsisset(bule->expires)) {
			if (!ts_now_broken) {
				struct timespec ts;
				tssub(bule->expires, ts_now, ts);
				fprintf(vh->vh_stream, "%ld", ts.tv_sec);
			} else
				fprintf(vh->vh_stream, "(error)");
		} else
			fprintf(vh->vh_stream, "-");
	}
	fprintf(vh->vh_stream, "\n");

	if ((bule->flags & IP6_MH_BU_HOME) && !ts_now_broken) {
		struct timespec delay, lastsent;
		if (!mpd_poll_mps(&bule->hoa,
				  &bule->peer_addr, &delay, &lastsent)) {
			fprintf(vh->vh_stream, " mps ");
			if (tsafter(ts_now, lastsent))
				fprintf(vh->vh_stream, "(broken)");
			else {
				struct timespec ts;
				tssub(ts_now, lastsent, ts);
				tssub(delay, ts, ts);
				fprintf(vh->vh_stream, "%ld", ts.tv_sec);
			}
			fprintf(vh->vh_stream, " / %ld", delay.tv_sec);
		}
	}
	fprintf(vh->vh_stream, "\n");
	fflush(vh->vh_stream);

	return 0;
}

static int bul_vt_cmd_bul(const struct vt_handle *vh, const char *str)
{
	struct bul_vt_arg bva;
	bva.vh = vh;

	if (strlen(str) > 0) {
		fprintf(vh->vh_stream, "unknown args\n");
		return 0;
	}
	pthread_rwlock_rdlock(&mn_lock);
	bul_iterate(NULL, bul_vt_dump, &bva);
	pthread_rwlock_unlock(&mn_lock);
	return 0;
}

static int bul_vt_cmd_rr(const struct vt_handle *vh, const char *str)
{
	if (strlen(str) > 0) {
		fprintf(vh->vh_stream, "unknown args\n");
		return 0;
	}
	pthread_rwlock_rdlock(&mn_lock);
	rrl_dump(vh->vh_stream);
	pthread_rwlock_unlock(&mn_lock);
	return 0;
}

static struct vt_cmd_entry vt_cmd_bul = {
	.cmd = "bul",
	.parser = bul_vt_cmd_bul,
};

static struct vt_cmd_entry vt_cmd_rr = {
	.cmd = "rr",
	.parser = bul_vt_cmd_rr,
};

struct bcache_vt_arg {
	const struct vt_handle *vh;
	int is_nb;
};

static int bcache_vt_dump(void *data, void *arg)
{
	struct bcentry *bce = (struct bcentry *)data;
	struct bcache_vt_arg *bva = (struct bcache_vt_arg *)arg;
	const struct vt_handle *vh = bva->vh;
	int is_nb = bva->is_nb;
	struct timespec ts_now;

	if (is_nb) {
		if (bce->type != BCE_NONCE_BLOCK)
			return 0;
	} else {
		if (bce->type == BCE_NONCE_BLOCK)
			return 0;
	}

	tsclear(ts_now);

	fprintf_bl(vh, "hoa %x:%x:%x:%x:%x:%x:%x:%x",
		   NIP6ADDR(&bce->peer_addr));

	if (vh->vh_opt.verbose == VT_BOOL_TRUE)
		fprintf(vh->vh_stream, " nonce %u", bce->nonce_hoa);

	fprintf_b(vh, " status %s",
		  (bce->type == BCE_HOMEREG) ? "registered" :
		  (bce->type == BCE_CACHED) ? "cached" :
		  (bce->type == BCE_NONCE_BLOCK) ? "nonce-block" :
		  (bce->type == BCE_CACHE_DYING) ? "dying" :
		  (bce->type == BCE_DAD) ? "dad" :
		  "(unknown)");

	fprintf(vh->vh_stream, "\n");

	fprintf(vh->vh_stream, " coa %x:%x:%x:%x:%x:%x:%x:%x",
		NIP6ADDR(&bce->coa));

	if (vh->vh_opt.verbose == VT_BOOL_TRUE)
		fprintf(vh->vh_stream, " nonce %u", bce->nonce_coa);

	fprintf(vh->vh_stream, " flags %c%c%c%c",
		((bce->flags & IP6_MH_BU_ACK) ? 'A' : '-'),
		((bce->flags & IP6_MH_BU_HOME) ? 'H' : '-'),
		((bce->flags & IP6_MH_BU_LLOCAL) ? 'L' : '-'),
		((bce->flags & IP6_MH_BU_KEYM) ? 'K' : '-'));

	fprintf(vh->vh_stream, "\n");

	fprintf(vh->vh_stream, " local %x:%x:%x:%x:%x:%x:%x:%x",
		NIP6ADDR(&bce->our_addr));

	if (vh->vh_opt.verbose == VT_BOOL_TRUE) {
		char buf[IF_NAMESIZE + 1];
		char *dev;

		if (bce->tunnel) {
			fprintf(vh->vh_stream, " tunnel ");

			dev = if_indextoname(bce->tunnel, buf);
			if (!dev || strlen(dev) == 0)
				fprintf(vh->vh_stream, "(%d)", bce->tunnel);
			else
				fprintf(vh->vh_stream, "%s", dev);
		}
		if (bce->link) {
			fprintf(vh->vh_stream, " link ");

			dev = if_indextoname(bce->link, buf);
			if (!dev || strlen(dev) == 0)
				fprintf(vh->vh_stream, "(%d)", bce->link);
			else
				fprintf(vh->vh_stream, "%s", dev);
		}
	}

	fprintf(vh->vh_stream, "\n");

	fprintf(vh->vh_stream, " lifetime ");
	if (bce->type == BCE_DAD)
		fprintf(vh->vh_stream, "-");
	else if (clock_gettime(CLOCK_REALTIME, &ts_now) != 0)
		fprintf(vh->vh_stream, "(error)");
	else {
		if (tsafter(ts_now, bce->add_time))
			fprintf(vh->vh_stream, "(broken)");
		else {
			struct timespec ts;

			tssub(ts_now, bce->add_time, ts);
			/* "ts" is now time how log it alives */
			if (tsafter(bce->lifetime, ts)) {
				tssub(ts, bce->lifetime, ts);
				fprintf(vh->vh_stream, "-%ld", ts.tv_sec);
			} else {
				tssub(bce->lifetime, ts, ts);
				fprintf(vh->vh_stream, "%ld", ts.tv_sec);
			}
		}
	}
	fprintf(vh->vh_stream, " / %ld", bce->lifetime.tv_sec);

	fprintf(vh->vh_stream, " seq %u", bce->seqno);

	fprintf(vh->vh_stream, " unreach %d", bce->unreach);

	if ((bce->flags & IP6_MH_BU_HOME) && tsisset(ts_now)) {
		struct timespec delay, lastsent;
		int retries = mpd_poll_mpa(&bce->our_addr, &bce->peer_addr,
					   &delay, &lastsent);
		if (retries >= 0) {
			fprintf(vh->vh_stream, " mpa ");
			if (!tsisset(lastsent))
				fprintf(vh->vh_stream, "-");
			else if (tsafter(ts_now, lastsent))
				fprintf(vh->vh_stream, "(broken)");
			else {
				struct timespec ts;

				tssub(ts_now, lastsent, ts);
				/* "ts" is now time how log it alives */
				if (tsafter(delay, ts)) {
					tssub(ts, delay, ts);
					fprintf(vh->vh_stream, "-%ld", ts.tv_sec);
				} else {
					tssub(delay, ts, ts);
					fprintf(vh->vh_stream, "%ld", ts.tv_sec);
				}
			}
		}
		fprintf(vh->vh_stream, " / %ld", delay.tv_sec);
		fprintf(vh->vh_stream, " retry %d", retries);
	}

	fprintf(vh->vh_stream, "\n");

	/* Dump the registered MNP */
	{
		struct list_head *l;
		list_for_each(l, &bce->mob_net_prefixes) {
			struct prefix_list_entry *p;
			p = list_entry(l, struct prefix_list_entry, list);
			fprintf(vh->vh_stream, " MNP: %x:%x:%x:%x:%x:%x:%x:%x/%d\n", 
                    NIP6ADDR(&p->ple_prefix), p->ple_plen);
		}
	}

	return 0;
}

//////////////////////////////////////////////////
//Defined for PMIP///////////////////////////////
/////////////////////////////////////////////////
struct pmip_cache_vt_arg {
    const struct vt_handle *vh;
};

static int pmip_cache_vt_dump(void *data, void *arg)
{
    pmip_entry_t *bce = (pmip_entry_t *)data;
    struct pmip_cache_vt_arg *bva = (struct pmip_cache_vt_arg *)arg;
    const struct vt_handle *vh = bva->vh;
    struct timespec ts_now;

    tsclear(ts_now);

    fprintf_bl(vh, "peer_addr %x:%x:%x:%x:%x:%x:%x:%x",
           NIP6ADDR(&bce->mn_addr));
           //NIP6ADDR(&bce->mn_suffix));


    fprintf_b(vh, " status %s",
          (bce->type == BCE_PMIP) ? "PMIP" :
          (bce->type == BCE_TEMP) ? "TEMP" :
          (bce->type == BCE_NO_ENTRY) ? "EMPTY" :
          "(unknown)");

    fprintf(vh->vh_stream, "\n");

    if (is_ha()) {
        fprintf(vh->vh_stream, " Serv_MAG_addr %x:%x:%x:%x:%x:%x:%x:%x",
            NIP6ADDR(&bce->mn_serv_mag_addr));
    }

    if (is_mag()) {
        fprintf(vh->vh_stream, " LMA_addr %x:%x:%x:%x:%x:%x:%x:%x",
            NIP6ADDR(&bce->mn_serv_lma_addr));
    }

    fprintf(vh->vh_stream, " local %x:%x:%x:%x:%x:%x:%x:%x",
        NIP6ADDR(&bce->our_addr));

    if (vh->vh_opt.verbose == VT_BOOL_TRUE) {
        char buf[IF_NAMESIZE + 1];
        char *dev;

        if (bce->tunnel) {
            fprintf(vh->vh_stream, " tunnel %d ",bce->tunnel);

            dev = if_indextoname(bce->tunnel, buf);
            if (!dev || strlen(dev) == 0)
                fprintf(vh->vh_stream, "(%d)", bce->tunnel);
            else
                fprintf(vh->vh_stream, "%s", dev);
        }
        if (bce->link) {
            fprintf(vh->vh_stream, " link ");

            dev = if_indextoname(bce->link, buf);
            if (!dev || strlen(dev) == 0)
                fprintf(vh->vh_stream, "(%d)", bce->link);
            else
                fprintf(vh->vh_stream, "%s", dev);
        }
    }

    fprintf(vh->vh_stream, "\n");

    fprintf(vh->vh_stream, " lifetime ");

    if (clock_gettime(CLOCK_REALTIME, &ts_now) != 0)
        fprintf(vh->vh_stream, "(error)");
    else {
        if (tsafter(ts_now, bce->add_time))
            fprintf(vh->vh_stream, "(broken)");
        else {
            struct timespec ts;

            tssub(ts_now, bce->add_time, ts);
            /* "ts" is now time how log it alives */
            if (tsafter(bce->lifetime, ts)) {
                tssub(ts, bce->lifetime, ts);
                fprintf(vh->vh_stream, "-%ld", ts.tv_sec);
            } else {
                tssub(bce->lifetime, ts, ts);
                fprintf(vh->vh_stream, "%ld", ts.tv_sec);
            }
        }
    }
    fprintf(vh->vh_stream, " / %ld", bce->lifetime.tv_sec);

    fprintf(vh->vh_stream, " seq %u", bce->seqno_out);

    fprintf(vh->vh_stream, "\n");

    return 0;
}

static int pmip_cache_vt_cmd_pbc(const struct vt_handle *vh, const char *str)
{
    struct pmip_cache_vt_arg bva;
    bva.vh = vh;

    if (strlen(str) > 0) {
        fprintf(vh->vh_stream, "unknown args\n");
        return 0;
    }

    pmip_cache_iterate(pmip_cache_vt_dump, &bva);
    return 0;
}

static struct vt_cmd_entry vt_cmd_pbc = {
    .cmd = "pmip",
    .parser = pmip_cache_vt_cmd_pbc,
};
////////////////////////////////////////////////////////////


static int vt_str_to_uint32(const struct vt_handle *vh, const char *str,
			    uint32_t *val)
{
	unsigned long int v;
	char *ptr = NULL;

	v = strtoul(str, &ptr, 0);
	if (!ptr || ptr == str || ptr[0] != '\0') {
		fprintf(vh->vh_stream, "invalid integer:%s\n", str);
		return -EINVAL;
	}
	if (v == ULONG_MAX) {
		fprintf(vh->vh_stream, "overflow:%s\n", str);
		return -ERANGE;
	}

	*val = (uint32_t)v;
	return 0;
}

/* get the first address with given string */
static int vt_str_to_addr6(const struct vt_handle *vh, const char *str,
			   struct in6_addr *addr)
{
	struct addrinfo hints;
	struct addrinfo *res = NULL;
	struct sockaddr_in6 *sin6;
	int err;

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_RAW;
	hints.ai_family = AF_INET6;
	hints.ai_flags = AI_PASSIVE;

	err = getaddrinfo(str, NULL, &hints, &res);
	if (err != 0) {
		fprintf(vh->vh_stream,
			"getaddrinfo: \"%s\" :%s\n", str, gai_strerror(err));
		goto end;
	}
	if (res->ai_addrlen < sizeof(*sin6)) {
		fprintf(vh->vh_stream,
			"getaddrinfo: \"%s\" : sockaddr too short:%d\n",
			str, res->ai_addrlen);
		err = -ENOBUFS;
		goto end;
	}
	if (res->ai_family != AF_INET6) {
		fprintf(vh->vh_stream,
			"getaddrinfo: \"%s\" : family is not AF_INET6:%u\n",
			str, res->ai_family);
		err = -EINVAL;
		goto end;
	}
	sin6 = (struct sockaddr_in6 *)res->ai_addr;
	memcpy(addr, &sin6->sin6_addr, sizeof(*addr));

 end:
	if (res)
		freeaddrinfo(res);

	return err;
}

static const char *vt_str_space_skip(const char *str);
static char *vt_str_space_chop(const char *str, char *buf, int bufsize);

static int bcache_vt_cmd_bc_mod(const struct vt_handle *vh, const char *str,
				int add)
{
	struct in6_addr hoa;
	struct in6_addr coa;
	struct in6_addr local;
	uint16_t bce_flags;
	struct timespec lft;
	uint16_t sequence;
	int flags = (HA_BU_F_THREAD_JOIN | HA_BU_F_PASSIVE_SEQ |
		     HA_BU_F_SKIP_BA);
	const char *p = str;
	const char *hoap = NULL;
	const char *coap = NULL;
	const char *localp = NULL;
	int err;

	memset(&hoa, 0, sizeof(hoa));
	memset(&coa, 0, sizeof(coa));
	memset(&local, 0, sizeof(local));
	bce_flags = 0;
	tsclear(lft);
	sequence = 0;

	while (1) {
		char name[LINE_MAX];
		char val[LINE_MAX];

		memset(name, '\0', sizeof(name));
		memset(val, '\0', sizeof(val));

		p = vt_str_space_skip(p);
		if (strlen(p) == 0)
			break;
		if (!vt_str_space_chop(p, name, sizeof(name)))
			goto usage;
		p += strlen(name);
		p = vt_str_space_skip(p);
		if (!vt_str_space_chop(p, val, sizeof(val)))
			goto usage;

		if (strcmp("hoa", name) == 0) {
			hoap = p;
			if (vt_str_to_addr6(vh, val, &hoa))
				goto end;
		} else if (strcmp("coa", name) == 0) {
			coap = p;
			if (vt_str_to_addr6(vh, val, &coa))
				goto end;
		} else if (strcmp("local", name) == 0) {
			localp = p;
			if (vt_str_to_addr6(vh, val, &local))
				goto end;
		} else if (strcmp("flags", name) == 0) {
			int len = strlen(val);
			int i;

			for (i = 0; i < len; i++) {
				switch (val[i]) {
				case 'A':
					bce_flags |= IP6_MH_BU_ACK;
					break;
				case 'H':
					bce_flags |= IP6_MH_BU_HOME;
					break;
				case 'L':
					bce_flags |= IP6_MH_BU_LLOCAL;
					break;
				case 'K':
					bce_flags |= IP6_MH_BU_KEYM;
					break;
				case '-':
					break;
				default:
					fprintf(vh->vh_stream,
						"invalid flag:%c\n", val[i]);
					goto end;
				}
			}
		} else if (strcmp("lifetime", name) == 0) {
			uint32_t v;
			if (vt_str_to_uint32(vh, val, &v))
				goto end;
			tssetsec(lft, v);
		} else if (strcmp("seq", name) == 0) {
			if (strcmp("auto", val) != 0) {
				uint32_t v;
				if (vt_str_to_uint32(vh, val, &v))
					goto end;
				if (v >= USHRT_MAX) {
					fprintf(vh->vh_stream,
						"sequence overflow\n");
					goto end;
				}
				sequence = (uint16_t)v;
				flags &= ~HA_BU_F_PASSIVE_SEQ;
			}
		} else if (strcmp("dad", name) == 0) {
			if (strcmp("want", val) == 0){
				flags &= ~HA_BU_F_SKIP_DAD;
			} else if (strcmp("never", val) == 0){
				flags |= HA_BU_F_SKIP_DAD;
			} else {
				fprintf(vh->vh_stream,
					"either \"want\" or \"never\" is allowed for dad\n");
				goto end;
			}
		} else {
			fprintf(vh->vh_stream, "invalid command:%s\n", name);
			goto usage;
		}

		p += strlen(val);
	}

	if (!hoap) {
		fprintf(vh->vh_stream, "missing hoa\n");
		goto end;
	}
	if (!coap) {
		if (add) {
			fprintf(vh->vh_stream, "missing coa; required for adding\n");
			goto end;
		}
	}
	if (!localp) {
		fprintf(vh->vh_stream, "missing local\n");
		goto end;
	}

	if (tsisset(lft)) {
		if (!add) {
			fprintf(vh->vh_stream, "lifetime is not zero\n");
			goto end;
		}
	} else {
		if (add) {
			fprintf(vh->vh_stream, "lifetime is zero\n");
			goto end;
		}
	}

	{
		struct ip6_mh_binding_update bu;
		struct in6_addr_bundle ab;
		struct ip6_mh *mh = (struct ip6_mh *)&bu;
		int len = sizeof(bu);

		memset(&bu, 0, sizeof(bu));
		bu.ip6mhbu_flags = bce_flags;
		bu.ip6mhbu_lifetime = htons(lft.tv_sec >> 2);
		bu.ip6mhbu_seqno = htons(sequence);
		ab.src = &hoa;
		ab.dst = &local;
		ab.local_coa = NULL;
		if (coap)
			ab.remote_coa = &coa;
		else
			ab.remote_coa = NULL;
		ab.bind_coa = NULL;

		if (bu.ip6mhbu_flags & IP6_MH_BU_HOME)
			err = ha_recv_home_bu(mh, len, &ab, 0, flags);
		else {
			cn_recv_bu(mh, len, &ab, 0);
			err = 0;
		}
	}
	if (err < 0)
		fprintf(vh->vh_stream, "bc error=%d(%s)\n",
			-err, strerror(-err));
	else
		fprintf(vh->vh_stream, "bc status=%d\n", err);
 end:
	return 0;

 usage:
	fprintf(vh->vh_stream,
		"hoa ADDR [coa ADDR] local ADDR flags [A]H[LK] [lifetime SECOND]\n"
		"  [seq auto|NUMBER] [dad want|never]\n");
	return 0;
}

static int bcache_vt_cmd_bc_add(const struct vt_handle *vh, const char *str)
{
	return bcache_vt_cmd_bc_mod(vh, str, 1);
}

static int bcache_vt_cmd_bc_del(const struct vt_handle *vh, const char *str)
{
	return bcache_vt_cmd_bc_mod(vh, str, 0);
}

static int bcache_vt_cmd_bc(const struct vt_handle *vh, const char *str)
{
	struct bcache_vt_arg bva;
	bva.vh = vh;
	bva.is_nb = 0;

	if (strlen(str) > 0) {
		fprintf(vh->vh_stream, "unknown args\n");
		return 0;
	}

	bcache_iterate(bcache_vt_dump, &bva);
	return 0;
}

static int bcache_vt_cmd_nonce(const struct vt_handle *vh, const char *str)
{
	struct bcache_vt_arg bva;
	bva.vh = vh;
	bva.is_nb = 1;

	if (strlen(str) > 0) {
		fprintf(vh->vh_stream, "unknown args\n");
		return 0;
	}

	bcache_iterate(bcache_vt_dump, &bva);
	return 0;
}

static struct vt_cmd_entry vt_cmd_bc = {
	.cmd = "bc",
	.parser = bcache_vt_cmd_bc,
};

static struct vt_cmd_entry vt_cmd_bc_add = {
	.cmd = "add",
	.parser = bcache_vt_cmd_bc_add,
};

static struct vt_cmd_entry vt_cmd_bc_del = {
	.cmd = "del",
	.parser = bcache_vt_cmd_bc_del,
};

static struct vt_cmd_entry vt_cmd_nonce = {
	.cmd = "nonce",
	.parser = bcache_vt_cmd_nonce,
};

int vt_cmd_add(struct vt_cmd_entry *parent, struct vt_cmd_entry *e)
{
	int err = 0;
	struct list_head *lp;

	INIT_LIST_HEAD(&e->list);
	e->parent = NULL;
	INIT_LIST_HEAD(&e->child_list);

	pthread_rwlock_wrlock(&vt_lock);

	if (!parent || !e) {
		err = -EINVAL;
		goto fin;
	}
	if (e == &vt_cmd_root) {
		err = -EINVAL;
		goto fin;
	}
	if (parent != &vt_cmd_root && parent->parent == NULL) {
		err = -EINVAL;
		goto fin;
	}
	if (e->parent != NULL) {
		err = -EINVAL;
		goto fin;
	}

	/* insert the entry to the list */
	/* XXX: TODO: it should be checked infinite loop */
	list_for_each (lp, &parent->child_list) {
		struct vt_cmd_entry *ce;
		ce = list_entry(lp, struct vt_cmd_entry, list);
		if (strcmp(ce->cmd, e->cmd) > 0) {
			list_add_tail(&e->list, lp);
			goto inserted;
		}
	}

	list_add_tail(&e->list, &parent->child_list);

 inserted:
	e->parent = parent;

 fin:
	pthread_rwlock_unlock(&vt_lock);

	return err;
}

int vt_cmd_add_root(struct vt_cmd_entry *e)
{
	return vt_cmd_add(&vt_cmd_root, e);
}

static int vt_cmd_has_child(struct vt_cmd_entry *e)
{
	return !list_empty(&e->child_list);
}

static const char *vt_str_space_skip(const char *str)
{
	int len = strlen(str);
	int i = 0;

	for (i = 0; i < len; i++) {
		if (isspace(str[i]) == 0)
			break;
	}

	return &str[i];
}

static char *vt_str_space_chop(const char *str, char *buf, int bufsize)
{
	int len = strlen(str);
	int i = 0;

	if (len > bufsize - 1)
		len = bufsize - 1;

	for (i = 0; i < len; i++) {
		if (isspace(str[i]) == 0)
			continue;

		strncpy(buf, str, i);
		buf[i] = '\0';
		return buf;
	}

	return NULL;
}

static int vt_str_match(const char *def, const char *str)
{
	int def_len = strlen(def);
	int len = strlen(str);

	if (def_len == len) {
		if (strncmp(def, str, def_len) == 0)
			return 1;
	} else if (def_len < len) {
		if (strncmp(def, str, def_len) == 0 &&
		    isspace(str[def_len]) != 0)
			return 1;
	}
	return 0;
}

static int vt_cmd_match(struct vt_cmd_entry *e, const char *cmd)
{
	return (vt_str_match(e->cmd, cmd) ||
		(e->cmd_alias && vt_str_match(e->cmd_alias, cmd)));
}

/*
 * It is only used the parser which is the final level away from root.
 */
static int vt_cmd_input(const struct vt_handle *vh, char *line)
{
	struct vt_cmd_entry *ce = &vt_cmd_root;
	const char *p;
	int ret;

	pthread_rwlock_rdlock(&vt_lock);

	p = line;

	while (1) {
		const char *p_next = NULL;
		struct vt_cmd_entry *e = NULL;
		struct list_head *lp;

		p = vt_str_space_skip(p);
		/* command has no character */
		if (strlen(p) == 0)
			goto fin;

		list_for_each (lp, &ce->child_list) {
			e = list_entry(lp, struct vt_cmd_entry, list);

			if (vt_cmd_match(e, p) == 0)
				continue;

			p_next = p + strlen(e->cmd);
			p_next = vt_str_space_skip(p_next);

			if (strlen(p_next) > 0 && vt_cmd_has_child(e))
				break;

			if (!e->parser) {
				fprintf(vh->vh_stream, "do nothing\n");
				goto fin;
			}

			pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
			ret = e->parser(vh, p_next);
			pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
			if (ret != 0) {
				fprintf(vh->vh_stream, "command parse failed\n");
			}

			goto fin;
		}

		if (p_next) {
			p = p_next;
			ce = e;
		} else if (vt_str_match(VT_CMD_HELP_STR, p)) {
			/* try to show help message when no matching command */
			vt_cmd_dump_candidates(vh, ce, VT_CMD_HELP_LINE_MAX);
			goto fin;
		} else {
			char buf[VT_PKT_BUFLEN];
			fprintf(vh->vh_stream, "unknown command: \"%s\"\n",
				vt_str_space_chop(p, buf, sizeof(buf)));
			goto fin;
		}
	}

 fin:
	if (vh->vh_sock < 0) {
		/* socket is closed during paring (normal operation) */
		goto closed;
	}

	if (vh->vh_opt.prompt == VT_BOOL_TRUE) {
		/* send prompt */
		fprintf(vh->vh_stream, VT_CMD_PROMPT);
	}

	fprintf(vh->vh_stream, "%c", 0);
	fflush(vh->vh_stream);

	pthread_rwlock_unlock(&vt_lock);

	return 0;

 closed:
	pthread_rwlock_unlock(&vt_lock);

	return 1;

}

static int vt_connect_input(struct vt_handle *vh, void *data, ssize_t len)
{
	char *line = NULL;
	int ret;

	line = (char *)malloc(len);
	if (!line) {
		ret = -errno;
		goto fin;
	}
	memcpy(line, data, len);
	line[len - 1] = 0;

	ret = vt_cmd_input(vh, line);

	if (ret != 0)
		goto fin;

	free(line);

	return 0;

 fin:
	if (line)
		free(line);

	vt_connect_close(vh);
	return ret;
}

static int vt_connect_recv(struct vt_handle *vh)
{
	while (1) {
		char buf[VT_PKT_BUFLEN];
		struct msghdr msg;
		struct iovec iov = { buf, sizeof(buf) };
		ssize_t len;
		int ret;

		memset(buf, 0, sizeof(buf));
		memset(&msg, 0, sizeof(msg));

		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
		msg.msg_control = NULL;
		msg.msg_controllen = 0;
		msg.msg_flags = 0;

		if (vh->vh_sock < 0)
			return -EBADFD;

		ret = recvmsg(vh->vh_sock, &msg, 0);
		if (ret < 0) {
			if (errno == EINTR)
				continue;
			return -1;
		} else if (ret == 0) {
			goto disconnect;
		}
		len = ret;

		ret = vt_connect_input(vh, buf, len);

		if (ret != 0)
			goto disconnect;

		if (msg.msg_flags & MSG_TRUNC)
			continue;

		if (msg.msg_flags & MSG_CTRUNC)
			continue;

		break;
	}

	return 0;

 disconnect:
	vt_connect_close(vh);
	return 1;
}

static int vt_connect_close(struct vt_handle *vh)
{
	if (vh->vh_sock < 0)
		return 0;

	close(vh->vh_sock);

	vh->vh_sock = -1;

	return 0;
}

static int vt_connect_fini(struct vt_handle *vh)
{
	if (vh->vh_sock >= 0)
		vt_connect_close(vh);

	memset(vh, 0, sizeof(*vh)); /* for fail-safe */
	vh->vh_sock = -1;

	free(vh);

	vt_connect_handle = NULL; /* XXX: remove from global pointer */

	return 0;
}

static int vt_connect_init(const struct vt_server_entry *vse)
{
	int sock;
	FILE *stream;
	struct vt_handle *vh = NULL;
	int ret;

	sock = accept(vse->vse_sock, NULL, NULL);
	if (sock < 0)
		goto error;

	stream = fdopen(sock, "w");

	if (vt_handle_full()) {
		/* send error message */
		ret = fprintf(stream, "Too many connections\n");
		if (ret < 0) {
			/* ignore error here*/
		}
		close(sock);
		goto error;
	}

	vh = (struct vt_handle *)malloc(sizeof(*vh));
	if (vh == NULL) {
		ret = -errno;

		/* send error message */
		if (fprintf(stream, "Server cannot make connection\n") < 0) {
			/* ignore error here*/
		}
		close(sock);
		fclose(stream);
		goto error;
	}

	memset(vh, 0, sizeof(*vh));
	vh->vh_sock = sock;
	vh->vh_stream = stream;

	/* Apply default values to option per server */
	switch (vse->vse_sa.sa_family) {
	case AF_LOCAL:
		vh->vh_opt.prompt = VT_BOOL_FALSE;
		vh->vh_opt.verbose = VT_BOOL_FALSE;
		vh->vh_opt.fancy = VT_BOOL_FALSE;
		break;
	default:
		vh->vh_opt.prompt = VT_BOOL_TRUE;
		vh->vh_opt.verbose = VT_BOOL_FALSE;
		vh->vh_opt.fancy = VT_BOOL_TRUE;
		break;
	}

	ret = vt_handle_add(vh);
	if (ret != 0) {
		goto error;
	}

	if (vh->vh_opt.prompt == VT_BOOL_TRUE) {
		/* send prompt */
		ret = fprintf(vh->vh_stream, VT_CMD_PROMPT);
		if (ret < 0)
			goto error;
		fflush(vh->vh_stream);
	}

	return 0;

 error:
	if (vh)
		vt_connect_fini(vh);

	return 0; /* ignore error here */
}

static void *vt_server_recv(__attribute__ ((unused)) void *arg)
{
	pthread_dbg("thread started");

	while (1) {
		int ret;
		int sock_max = 0;
		fd_set fds;
		struct list_head *lp;

		FD_ZERO(&fds);
		list_for_each (lp, &vt_server_list) {
			struct vt_server_entry *e;
			e = list_entry(lp, struct vt_server_entry, list);
			FD_SET(e->vse_sock, &fds);

			if (sock_max < e->vse_sock)
				sock_max = e->vse_sock;
		}
		if (sock_max == 0)
			break;
		if (vt_connect_handle != NULL &&
		    vt_connect_handle->vh_sock >= 0) {
			FD_SET(vt_connect_handle->vh_sock, &fds);
			if (sock_max < vt_connect_handle->vh_sock)
				sock_max = vt_connect_handle->vh_sock;
		}

		ret = select(sock_max+1, &fds, NULL, NULL, NULL); 
		if (ret < 0) {
			if (errno == EINTR)
				continue;
			break;
		}

		ret = 0;
		list_for_each (lp, &vt_server_list) {
			struct vt_server_entry *e;
			e = list_entry(lp, struct vt_server_entry, list);

			if (FD_ISSET(e->vse_sock, &fds)) {
				ret = vt_connect_init(e);
				if (ret != 0)
					break;
			}
		}
		if (ret != 0)
			break;

		if (vt_connect_handle != NULL &&
		    vt_connect_handle->vh_sock >= 0) {
			if (FD_ISSET(vt_connect_handle->vh_sock, &fds)) {
				ret = vt_connect_recv(vt_connect_handle);
				if (ret != 0)
					vt_connect_fini(vt_connect_handle);
			}
		}
	}

	if (vt_connect_handle != NULL)
		vt_connect_fini(vt_connect_handle);
	vt_server_fini();

	pthread_exit(NULL);
}

static int vt_server_clean(const struct sockaddr *sa, int salen)
{
	if (sa->sa_family == AF_LOCAL) {
		const struct sockaddr_un *sun;
		if (salen >= 0 && (size_t)salen >= sizeof(*sun)) {
			sun = (const struct sockaddr_un *)sa;
			if (unlink(sun->sun_path))
				errno = 0; /* ignore error here */
		}
	}

	return 0;
}

static void vt_server_close(struct vt_server_entry *e)
{
	if (e->vse_sock) {
		close(e->vse_sock);
		e->vse_sock = -1;
	}
}


static int vt_server_fini(void)
{
	struct list_head *lp, *tmp;

	list_for_each_safe (lp, tmp,  &vt_server_list) {
		struct vt_server_entry *e;

		list_del(lp);
		e = list_entry(lp, struct vt_server_entry, list);

		if (e->vse_sock >= 0)
			vt_server_clean(&e->vse_sa, e->vse_salen);
		vt_server_close(e);
		memset(e, 0, sizeof(*e)); /* for fail-safe */
		free(e);
	}

	return 0;
}

static int vt_server_setsockopt(int sock, struct addrinfo *ai)
{
	int ret;

	ret = vt_server_clean(ai->ai_addr, ai->ai_addrlen);
	if (ret != 0)
		return ret;

	if (ai->ai_protocol == IPPROTO_TCP) {
		const int on = 1;

		ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
				 &on, sizeof(on));
		if (ret != 0)
			return ret;
	}
	return 0;
}

static int vt_server_init(const char *node, const char *service,
			  struct addrinfo *hints)
{
	struct addrinfo *res = NULL;
	struct addrinfo *ai;
	int ret;
	int n = 0;

	ret = getaddrinfo(node, service, hints, &res);
	if (ret != 0)
		goto error;

	errno = 0;

	for (ai = res; ai != NULL; ai = ai->ai_next) {
		int sock;
		struct vt_server_entry *e;

		sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
		if (sock < 0)
			continue;

		ret = vt_server_setsockopt(sock, ai);
		if (ret != 0) {
			close(sock);
			continue;
		}

		ret = bind(sock, ai->ai_addr, ai->ai_addrlen);
		if (ret != 0) {
			close(sock);
			continue;
		}

		ret = listen(sock, VT_SERVER_BACKLOG);
		if (ret != 0) {
			close(sock);
			continue;
		}

		e = (struct vt_server_entry *)malloc(sizeof(*e));
		if (e == NULL) {
			ret = -errno;
			close(sock);
			goto error;
		}
		memset(e, 0, sizeof(*e));
		e->vse_sock = sock;
		memcpy(&e->vse_sa, ai->ai_addr, ai->ai_addrlen);
		e->vse_salen = ai->ai_addrlen;

		list_add(&e->list, &vt_server_list);

		n ++;
	}
	if (n == 0) {
		ret = -1;
		goto error;
	}
	errno = 0;

	if (res != NULL)
		freeaddrinfo(res);

	return 0;
 error:
	if (res != NULL)
		freeaddrinfo(res);
	vt_server_fini();
	return ret;
}

int vt_start(const char *vthost, const char *vtservice)
{
	struct addrinfo hints;

	INIT_LIST_HEAD(&vt_server_list);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (vt_server_init(vthost, vtservice, &hints) != 0)
		return -1;

	if (pthread_create(&vt_listener, NULL, vt_server_recv, NULL))
		return -1;
	return 0;
}

int vt_bul_init(void)
{
	int ret;

	ret = vt_cmd_add_root(&vt_cmd_bul);
	if (ret < 0)
		return ret;

	ret = vt_cmd_add_root(&vt_cmd_rr);
	if (ret < 0)
		return ret;

	return 0;
}

int vt_bc_init(void)
{
	int ret;

	ret = vt_cmd_add_root(&vt_cmd_bc);
	if (ret < 0)
		return ret;
	if (is_ha()) {
		ret = vt_cmd_add(&vt_cmd_bc, &vt_cmd_bc_add);
		if (ret < 0)
			return ret;
		ret = vt_cmd_add(&vt_cmd_bc, &vt_cmd_bc_del);
		if (ret < 0)
			return ret;
	}

	ret = vt_cmd_add_root(&vt_cmd_nonce);
	if (ret < 0)
		return ret;

	return 0;
}

//Defined for PMIP////////////////
int vt_pbc_init(void)
{
    int ret;
    ret = vt_cmd_add_root(&vt_cmd_pbc);
    if (ret < 0)
    return ret;

    return 0;
}
//////////////////////////////////

int vt_init(void)
{
	if (pthread_rwlock_init(&vt_lock, NULL))
		return -1;

	return vt_cmd_sys_init();
}

void vt_fini(void)
{
	struct list_head *lp;

	list_for_each(lp,  &vt_server_list) {
		struct vt_server_entry *e;
		e = list_entry(lp, struct vt_server_entry, list);
		vt_server_close(e);
	}
	pthread_cancel(vt_listener);
	pthread_join(vt_listener, NULL);

	vt_server_fini();
}

/*
 * $Id: cn.c 1.84 06/05/07 21:52:42+03:00 anttit@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 * 
 * Authors: Ville Nuorvala <vnuorval@tcs.hut.fi>
 *          Antti Tuominen <anttit@tcs.hut.fi>
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
 * 02111-1307 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <time.h>
#include <pthread.h>

#include <netinet/icmp6.h>
#include <netinet/ip6mh.h>
#include <netinet/ip6.h>

#include "mipv6.h"
#include "debug.h"
#include "icmp6.h"
#include "mh.h"
#include "util.h"
#include "bcache.h"
#include "keygen.h"
#include "retrout.h"
#include "cn.h"
#include "conf.h"
#include "statistics.h"

#define MH_DEBUG_LEVEL 1

#if MH_DEBUG_LEVEL >= 1
#define MDBG dbg
#else
#define MDBG(x...)
#endif

#define ICMP_ERROR_PERSISTENT_THRESHOLD 3

const struct timespec cn_brr_before_expiry_ts =
{ CN_BRR_BEFORE_EXPIRY, 0 };

static void cn_recv_dst_unreach(const struct icmp6_hdr *ih, ssize_t len,
				__attribute__ ((unused)) const struct in6_addr *src,
				__attribute__ ((unused)) const struct in6_addr *dst,
				__attribute__ ((unused)) int iif,
				__attribute__ ((unused)) int hoplimit)
{
	struct ip6_hdr *ip6h = (struct ip6_hdr *)(ih + 1);
	int optlen = len - sizeof(struct icmp6_hdr);
	struct in6_addr *laddr = &ip6h->ip6_src;
	struct in6_addr *raddr = &ip6h->ip6_dst;
	struct bcentry *bce = NULL;

	/* check if data was truncated */
	if (icmp6_parse_data(ip6h, optlen, &laddr, &raddr) < 0)
		return;

	bce = bcache_get(laddr, raddr);

	if (bce == NULL) 
		return;

	bce->unreach++;
	if (bce->unreach > ICMP_ERROR_PERSISTENT_THRESHOLD &&
	    bce->type != BCE_HOMEREG) {
		bcache_release_entry(bce);
		bcache_delete(laddr, raddr);
		dbg("BCE for %x:%x:%x:%x:%x:%x:%x:%x deleted "
		    "due to receipt of ICMPv6 destination unreach\n", 
		    NIP6ADDR(raddr));
	} else {
		bcache_release_entry(bce);
	}
}

static struct icmp6_handler cn_dst_unreach_handler = {
	.recv = cn_recv_dst_unreach,
};

static void cn_recv_hoti(const struct ip6_mh *mh, ssize_t len,
			 const struct in6_addr_bundle *in, int iif)
{
	struct ip6_mh_home_test_init *hoti;
	struct ip6_mh_home_test *hot;
	struct in6_addr_bundle out;
	struct iovec iov;
	uint8_t keygen_token[8];

	statistics_inc(&mipl_stat, MIPL_STATISTICS_IN_HOTI);

	if (len < 0 || (size_t)len < sizeof(struct ip6_mh_home_test_init) ||
	    in->remote_coa)
		return;
	out.src = in->dst;
	out.dst = in->src;
	out.remote_coa = NULL;
	out.local_coa = NULL;

	hoti = (struct ip6_mh_home_test_init *)mh;
	hot = mh_create(&iov, IP6_MH_TYPE_HOT);
	hot->ip6mhht_nonce_index = 
		htons(rr_cn_keygen_token(in->src, 0, keygen_token));
	cookiecpy(hot->ip6mhht_cookie, hoti->ip6mhhti_cookie);
	memcpy(hot->ip6mhht_keygen, keygen_token, 8);
	mh_send(&out, &iov, 1, NULL, iif);
	free_iov_data(&iov, 1);
	statistics_inc(&mipl_stat, MIPL_STATISTICS_OUT_HOT);
}

static struct mh_handler cn_hoti_handler = {
	.recv = cn_recv_hoti,
};

static void cn_recv_coti(const struct ip6_mh *mh, ssize_t len,
			 const struct in6_addr_bundle *in, int iif)
{
	struct ip6_mh_careof_test_init *coti;
	struct ip6_mh_careof_test *cot;
	struct in6_addr_bundle out;
	struct iovec iov;
	uint8_t keygen_token[8];

	statistics_inc(&mipl_stat, MIPL_STATISTICS_IN_COTI);

	if (len < 0 || (size_t)len < sizeof(struct ip6_mh_careof_test_init) ||
	    in->remote_coa)
		return;
	out.src = in->dst;
	out.dst = in->src;
	out.remote_coa = NULL;
	out.local_coa = NULL;

	coti = (struct ip6_mh_careof_test_init *)mh;
	cot = mh_create(&iov, IP6_MH_TYPE_COT);
	cot->ip6mhct_nonce_index = 
		htons(rr_cn_keygen_token(in->src, 1, keygen_token));
	cookiecpy(cot->ip6mhct_cookie, coti->ip6mhcti_cookie);
	memcpy(cot->ip6mhct_keygen, keygen_token, 8);
	mh_send(&out, &iov, 1, NULL, iif);
	free_iov_data(&iov, 1);
	statistics_inc(&mipl_stat, MIPL_STATISTICS_OUT_COT);
}

static struct mh_handler cn_coti_handler = {
	.recv = cn_recv_coti,
};

/* After parsing, perform CN-specific checks (flags and auth + nonce) on BU.
 * -1 is returned if the BU needs to be silently dropped. Otherwise, a BA
 * status code is returned. If it is an error code, the BU should be dropped */
static int cn_bu_check(struct ip6_mh_binding_update *bu, ssize_t len,
		       struct mh_options *mh_opts,
		       struct in6_addr *peer_addr,
		       struct in6_addr *our_addr,
		       struct in6_addr *bind_coa,
		       struct timespec *lifetime, uint8_t *key)
{
	struct ip6_mh_opt_nonce_index *non_ind;
	struct ip6_mh_opt_auth_data *bauth;
	int ret;

	non_ind = mh_opt(&bu->ip6mhbu_hdr, mh_opts, IP6_MHOPT_NONCEID);

	if (bu->ip6mhbu_flags & IP6_MH_BU_HOME) {
		if (non_ind) {
			/* BU w/ Nonce and H bit set. It is simply invalid.
			 * Drop it. This matches behavior expectedin TAHI
			 * CN tests: 5-3-4, 5-3-5, 5-3-6 */
			return -1;
		}
		/* Now, as a CN, we are receiving a BU intended for a HA.
		 * We will send a BA to report the error. If the peer is
		 * already registered: registration type change is not
		 * allowed (TAHI CN tests 5-3-2, 5-3-3). If peer is unknown
		 * to us, just warn we are not a HA (TAHI CN Test 5-3-1) */
		if (bce_exists(our_addr, peer_addr))
			return IP6_MH_BAS_REG_NOT_ALLOWED;
		else
			return IP6_MH_BAS_HA_NOT_SUPPORTED;
	}

	if (!non_ind)
		return -1;

	/* We could also test if BU has R flag set (NEMO) and drop
	 * it in that case. We just act as expected by RFC 3775, i.e.
	 * just don't consider the value of the flag --arno */

	MDBG("src %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(peer_addr));
	MDBG("coa %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(bind_coa));

	if (tsisset(*lifetime))
		ret = rr_cn_calc_Kbm(ntohs(non_ind->ip6moni_home_nonce),
				     ntohs(non_ind->ip6moni_coa_nonce),
				     peer_addr, bind_coa, key);
	else /* Only use home nonce and address for dereg. */
		ret = rr_cn_calc_Kbm(ntohs(non_ind->ip6moni_home_nonce), 0,
				     peer_addr, NULL, key);
	if (ret)
		return ret;

	bauth = mh_opt(&bu->ip6mhbu_hdr, mh_opts, IP6_MHOPT_BAUTH);
	if (!bauth)
		return -1;

	/* Authenticator is calculated with MH checksum set to 0 */
	bu->ip6mhbu_hdr.ip6mh_cksum = 0;
	if (mh_verify_auth_data(bu, len, bauth, bind_coa, our_addr, key) < 0)
		return -1;

	return IP6_MH_BAS_ACCEPTED;
}

void cn_recv_bu(const struct ip6_mh *mh, ssize_t len,
		const struct in6_addr_bundle *in, int iif)
{
	struct mh_options mh_opts;
	struct in6_addr_bundle out;
	struct ip6_mh_binding_update *bu;
	struct ip6_mh_opt_nonce_index *non_ind;
	struct bcentry *bce = NULL;
	struct timespec lft;
	int status, new = 0;
	uint16_t bu_flags, seqno;
	uint8_t key[HMAC_SHA1_KEY_SIZE];
	uint8_t *pkey = NULL;

	statistics_inc(&mipl_stat, MIPL_STATISTICS_IN_BU);

	bu = (struct ip6_mh_binding_update *)mh;

	if (mh_bu_parse(bu, len, in, &out, &mh_opts, &lft) < 0)
		return;

	if ((status = cn_bu_check(bu, len, &mh_opts, out.dst, out.src,
				  out.bind_coa, &lft, key)) < 0)
		return;

	seqno = ntohs(bu->ip6mhbu_seqno);
	if (status >= IP6_MH_BAS_UNSPECIFIED)
		goto send_nack;

	bu_flags = bu->ip6mhbu_flags;
	non_ind = mh_opt(&bu->ip6mhbu_hdr, &mh_opts, IP6_MHOPT_NONCEID);
	bce = bcache_get(out.src, out.dst);
	if (bce) {
		if (!MIP6_SEQ_GT(seqno, bce->seqno)) {
			uint16_t nonce_hoa;
			if (bce->type != BCE_NONCE_BLOCK) {
				/* sequence number expired */
				seqno = bce->seqno;
				bcache_release_entry(bce);
				bce = NULL;
				status = IP6_MH_BAS_SEQNO_BAD;
				pkey = key;
				goto send_nack;
			}
			/* Don't accept the same home key generation token
			 * after deregistration with sequence numbers that
			 * have been used before with the same home address.
			 * This is more strict than the draft, but otherwise
			 * we would need to store all non-expired home kgen
			 * tokens mn had used before deregistration. */
			nonce_hoa = ntohs(non_ind->ip6moni_home_nonce);
			if (bce->nonce_hoa == nonce_hoa) {
				bcache_release_entry(bce);
				bce = NULL;
				status = IP6_MH_BAS_NI_EXPIRED;
				goto send_nack;
			}
		}
		if (bce->type == BCE_NONCE_BLOCK) {
			bcache_release_entry(bce);
			bce = NULL;
			/* don't let MN deregister BCE_NONCE_BLOCK entry */
			if (!tsisset(lft)) {
				status = IP6_MH_BAS_UNSPECIFIED;
				goto send_nack;
			}
			/* else get rid of it */
			bcache_delete(out.src, out.dst);
		}
	}

	status = conf.pmgr.discard_binding(out.dst, out.bind_coa,
					   out.src, bu, len);

	if (status >= IP6_MH_BAS_UNSPECIFIED) {
		pkey = key;
		goto send_nack;
	}
	if (tsisset(lft)) {
		pkey = key;
		if (!bce) {
			bce = bcache_alloc(BCE_CACHED);
			if (!bce) {
				/* new entry, bc full */
				status = IP6_MH_BAS_INSUFFICIENT;
				goto send_nack;
			}
			new = 1;
			bce->our_addr = *out.src;
			bce->peer_addr = *out.dst;
		}
		if (tsbefore(lft, MAX_RR_BINDING_LIFETIME_TS))
			lft = MAX_RR_BINDING_LIFETIME_TS;
		bce->coa = *out.bind_coa;
		bce->seqno = seqno;
		bce->flags = bu_flags;
		bce->unreach = 0;
		bce->type = BCE_CACHED;
		bce->nonce_coa = ntohs(non_ind->ip6moni_coa_nonce);
		bce->nonce_hoa = ntohs(non_ind->ip6moni_home_nonce);
		bce->lifetime = lft;
		if (new) {
			/* new entry, success */
			if (bcache_add(bce) < 0) {
				free(bce);
				bce = NULL;
				status = IP6_MH_BAS_INSUFFICIENT;
				goto send_nack;
			}
		} else {
			/* update entry, success */
			bcache_update_expire(bce);
			bcache_release_entry(bce);
		}
	} else {
		if (!bce) {
			status = IP6_MH_BAS_UNSPECIFIED;
			goto send_nack;
		}
		/* dereg, success */
		bcache_release_entry(bce);
		bcache_delete(out.src, out.dst);
		status = IP6_MH_BAS_ACCEPTED;
	}
	if (bu_flags & IP6_MH_BU_ACK)
		mh_send_ba(&out, status, 0, seqno, &lft, key, iif);
	return;
send_nack:
	if (bce) {
		bcache_release_entry(bce);
		bcache_delete(out.src, out.dst);
	}
	mh_send_ba_err(&out, status, 0, seqno, pkey, iif);
}

static struct mh_handler cn_bu_handler =
{
	.recv = cn_recv_bu,
};

void cn_init(void)
{
	icmp6_handler_reg(ICMP6_DST_UNREACH, &cn_dst_unreach_handler);
	mh_handler_reg(IP6_MH_TYPE_HOTI, &cn_hoti_handler);
	mh_handler_reg(IP6_MH_TYPE_COTI, &cn_coti_handler);
	mh_handler_reg(IP6_MH_TYPE_BU, &cn_bu_handler);
}

void cn_cleanup(void)
{
	mh_handler_dereg(IP6_MH_TYPE_BU, &cn_bu_handler);
	mh_handler_dereg(IP6_MH_TYPE_COTI, &cn_coti_handler);
	mh_handler_dereg(IP6_MH_TYPE_HOTI, &cn_hoti_handler);
	icmp6_handler_dereg(ICMP6_DST_UNREACH, &cn_dst_unreach_handler);
	bcache_flush();
}

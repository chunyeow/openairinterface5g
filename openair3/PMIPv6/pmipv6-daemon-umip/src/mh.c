/*
 * $Id: mh.c 1.103 06/05/07 21:52:43+03:00 anttit@tcs.hut.fi $
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
 * 02111-1307 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <pthread.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <sys/socket.h>
#ifdef HAVE_LIBCRYPTO
#include <openssl/opensslv.h>
#include <openssl/hmac.h>
#else
#include "crypto.h"
#endif

#include "mipv6.h"
#include "xfrm.h"
#include "mh.h"
#include "util.h"
#include "debug.h"
#include "conf.h"
#include "bcache.h"
#include "keygen.h"
#include "prefix.h"
#include "statistics.h"
#include "pmip_types.h"

#define MH_DEBUG_LEVEL 1

#if MH_DEBUG_LEVEL >= 1
#define MDBG dbg
#else
#define MDBG(x...)
#endif

struct sock mh_sock;

/* Are duplicate options allowed */
int mh_opts_dup_ok[] = {
	1, /* PAD1 */ 
	1, /* PADN */
	0, /* BRR */
	0, /* Alternate CoA */
	0, /* Nonce Index */
	0, /* Binding Auth Data */
	1, /* Mobile Network Prefix */
};

#define __MH_SENTINEL (IP6_MH_TYPE_MAX + 1)

static pthread_rwlock_t handler_lock;
static struct mh_handler *handlers[__MH_SENTINEL + 1];

static pthread_t mh_listener;

static inline int mh_type_map(uint8_t type)
{
	return type < __MH_SENTINEL ? type : __MH_SENTINEL;
}

static inline struct mh_handler *mh_handler_get(uint8_t type)
{
	return handlers[mh_type_map(type)]; 
}

void mh_handler_reg(uint8_t type, struct mh_handler *handler)
{
	int i = mh_type_map(type);

	assert(handler->next == NULL);

	pthread_rwlock_wrlock(&handler_lock);
	handler->next = handlers[i];
	handlers[i] = handler;
	pthread_rwlock_unlock(&handler_lock);
}

void mh_handler_dereg(uint8_t type, struct mh_handler *handler)
{
	struct mh_handler **h; 
	int i = mh_type_map(type);
	pthread_rwlock_wrlock(&handler_lock);
	h = &handlers[i];
	while (*h) {
		if (*h == handler) {
			*h = handler->next;
			handler->next = NULL;
			break;
		}
		h = &(*h)->next;
	}
	pthread_rwlock_unlock(&handler_lock);
}

static void *mh_listen(__attribute__ ((unused)) void *arg)
{
	uint8_t msg[MAX_PKT_LEN];
	struct in6_pktinfo pktinfo;
	struct in6_addr haoa, rta;
	struct sockaddr_in6 addr;
	struct ip6_mh *mh;
	struct in6_addr_bundle addrs;
	ssize_t len;
	struct mh_handler *h;

	pthread_dbg("thread started");

	while (1) {
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		len = mh_recv(msg, sizeof(msg), &addr, &pktinfo, &haoa, &rta);
		/* check if socket has closed */
		if (len == -EBADF)
			break;

		/* common validity check */
		if (len < 0 || (size_t)len < sizeof(struct ip6_mh))
			continue;

		addrs.src = &addr.sin6_addr;
		addrs.dst = &pktinfo.ipi6_addr;
		if (!IN6_IS_ADDR_UNSPECIFIED(&haoa)) {
			addrs.remote_coa = &haoa;
		} else {
			addrs.remote_coa = NULL;
		}
		if (!IN6_IS_ADDR_UNSPECIFIED(&rta)) {
			addrs.local_coa = &rta;
		} else {
			addrs.local_coa = NULL;
		}
		addrs.bind_coa = NULL;
		mh = (struct ip6_mh *) msg;
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
		pthread_rwlock_rdlock(&handler_lock);
		h = mh_handler_get(mh->ip6mh_type);
		if (h)
			h->recv(mh, len, &addrs, pktinfo.ipi6_ifindex);

		pthread_rwlock_unlock(&handler_lock);
	}
	pthread_exit(NULL);
}

int mh_init(void)
{
	pthread_mutexattr_t mattrs;
	int val;

	mh_sock.fd = socket(AF_INET6, SOCK_RAW, IPPROTO_MH);
	if (mh_sock.fd < 0) {
		syslog(LOG_ERR,
		       "Unable to open MH socket! "
		       "Do you have root permissions?");
		return mh_sock.fd;
	}
	val = 1;
	if (setsockopt(mh_sock.fd, IPPROTO_IPV6, IPV6_RECVPKTINFO,
		       &val, sizeof(val)) < 0)
		return -1;
	if (setsockopt(mh_sock.fd, IPPROTO_IPV6, IPV6_RECVDSTOPTS,
		       &val, sizeof(val)) < 0)
		return -1;
	if (setsockopt(mh_sock.fd, IPPROTO_IPV6, IPV6_RECVRTHDR,
		       &val, sizeof(val)) < 0)
		return -1;

	val = 4;
	if (setsockopt(mh_sock.fd, IPPROTO_RAW, IPV6_CHECKSUM,
		       &val, sizeof(val)) < 0)
		return -1;

	pthread_mutexattr_init(&mattrs);
	pthread_mutexattr_settype(&mattrs, PTHREAD_MUTEX_FAST_NP);
	if (pthread_mutex_init(&mh_sock.send_mutex, &mattrs) ||
	    pthread_rwlock_init(&handler_lock, NULL) ||
	    pthread_create(&mh_listener, NULL, mh_listen, NULL))
		return -1;
	return 0;
}

static const size_t _mh_len[] = {
	sizeof(struct ip6_mh_binding_request),
	sizeof(struct ip6_mh_home_test_init),
	sizeof(struct ip6_mh_careof_test_init),
	sizeof(struct ip6_mh_home_test),
	sizeof(struct ip6_mh_careof_test),
	sizeof(struct ip6_mh_binding_update),
	sizeof(struct ip6_mh_binding_ack),
	sizeof(struct ip6_mh_binding_error)
};

/**
 * mh_create - create a mobility header
 * @iov: vector
 * @type: mobility header type
 *
 * Initializes a new mobility header of type @type.  Stores pointer
 * and length in iovec vector @iov.  Returns pointer to newly created
 * header or NULL on error.
 **/
void *mh_create(struct iovec *iov, uint8_t type)
{
	struct ip6_mh *mh;

	if (type > IP6_MH_TYPE_MAX)
		return NULL;
	iov->iov_base = malloc(_mh_len[type]);
	if (iov->iov_base == NULL)
		return NULL;

	memset(iov->iov_base, 0, _mh_len[type]);

	mh = (struct ip6_mh *)iov->iov_base;
	mh->ip6mh_proto = IPPROTO_NONE;
	mh->ip6mh_hdrlen = 0; /* calculated after padding */
	mh->ip6mh_type = type;
	mh->ip6mh_reserved = 0;
	mh->ip6mh_cksum = 0; /* kernel does this for us */

	iov->iov_len = _mh_len[type];

	return mh;
}

/**
 * mh_create_opt_refresh_advice - create refresh advice option
 * @iov: vector
 * @interval: refresh interval
 *
 * Creates binding refresh advice option with @interval value.  Stores
 * pointer and length in iovec vector @iov.  Returns zero on success,
 * otherwise negative error value.
 **/
int mh_create_opt_refresh_advice(struct iovec *iov, uint16_t interval)
{
	struct ip6_mh_opt_refresh_advice *opt;
	size_t optlen = sizeof(struct ip6_mh_opt_refresh_advice);

	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;

	if (iov->iov_base == NULL)
		return -ENOMEM;

	opt = (struct ip6_mh_opt_refresh_advice *)iov->iov_base;

	opt->ip6mora_type = IP6_MHOPT_BREFRESH;
	opt->ip6mora_len = 2;
	opt->ip6mora_interval = interval;

	return 0;
}

/**
 * mh_create_opt_altcoa - create alternate care-of address option
 * @iov: vector
 * @addr: alternate care-of address
 *
 * Creates an alternate care-of address option.  Stores pointer and
 * length in iovec vector @iov.  Returns zero on success, otherwise
 * negative error value.
 **/
int mh_create_opt_altcoa(struct iovec *iov, struct in6_addr *addr)
{
	struct ip6_mh_opt_altcoa *opt;
	size_t optlen = sizeof(struct ip6_mh_opt_altcoa);

	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;

	if (iov->iov_base == NULL)
		return -ENOMEM;

	opt = (struct ip6_mh_opt_altcoa *)iov->iov_base;

	opt->ip6moa_type = IP6_MHOPT_ALTCOA;
	opt->ip6moa_len = 16;
	opt->ip6moa_addr = *addr;

	return 0;
}

/**
 * mh_create_opt_nonce_index - create nonce index option
 * @iov: vector
 * @home_nonce: home nonce value
 * @coa_nonce: coa nonce value
 *
 * Creates a nonce index option with @home_nonce and @coa_nonce
 * indices.  Stores pointer and length in iovec vector @iov.  Returns
 * zero on success, otherwise negative error value.
 **/
int mh_create_opt_nonce_index(struct iovec *iov, uint16_t home_nonce,
			      uint16_t coa_nonce)
{
	struct ip6_mh_opt_nonce_index *opt;
	size_t optlen = sizeof(struct ip6_mh_opt_nonce_index);

	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;

	if (iov->iov_base == NULL)
		return -ENOMEM;

	opt = (struct ip6_mh_opt_nonce_index *)iov->iov_base;

	opt->ip6moni_type = IP6_MHOPT_NONCEID;
	opt->ip6moni_len = 4;
	opt->ip6moni_home_nonce = htons(home_nonce);
	opt->ip6moni_coa_nonce = htons(coa_nonce);

	return 0;
}

/**
 * mh_create_opt_auth_data - create binding authorization data option
 * @iov: vector
 *
 * Creates a binding authorization data option with data set to zero.
 * Real data will be calculated after padding.  Stores pointer and
 * length in iovec vector @iov.  Returns zero on success, otherwise
 * negative error value.
 **/
int mh_create_opt_auth_data(struct iovec *iov)
{
	struct ip6_mh_opt_auth_data *opt;
	size_t optlen = sizeof(struct ip6_mh_opt_auth_data);

	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;

	if (iov->iov_base == NULL)
		return -ENOMEM;

	memset(iov->iov_base, 0, iov->iov_len);
	opt = (struct ip6_mh_opt_auth_data *)iov->iov_base;
	opt->ip6moad_type = IP6_MHOPT_BAUTH;
	opt->ip6moad_len = MIPV6_DIGEST_LEN;

	return 0;
}

/* We can use these safely, since they are only read and never change */
static const uint8_t _pad1[1] = { 0x00 };
static const uint8_t _pad2[2] = { 0x01, 0x00 };
static const uint8_t _pad3[3] = { 0x01, 0x01, 0x00 };
static const uint8_t _pad4[4] = { 0x01, 0x02, 0x00, 0x00 };
static const uint8_t _pad5[5] = { 0x01, 0x03, 0x00, 0x00, 0x00 };
static const uint8_t _pad6[6] = { 0x01, 0x04, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t _pad7[7] = { 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00 };

static int create_opt_pad(struct iovec *iov, int pad)
{
	if (pad == 2)
		iov->iov_base = (void *)_pad2;
	else if (pad == 4)
		iov->iov_base = (void *)_pad4;
	else if (pad == 6)
		iov->iov_base = (void *)_pad6;
	/* Odd pads do not occur with current spec, so test them last */
	else if (pad == 1)
		iov->iov_base = (void *)_pad1;
	else if (pad == 3)
		iov->iov_base = (void *)_pad3;
	else if (pad == 5)
		iov->iov_base = (void *)_pad5;
	else if (pad == 7)
		iov->iov_base = (void *)_pad7;

	iov->iov_len = pad;

	return 0;
}

int mh_create_opt_mob_net_prefix(struct iovec *iov, int mnp_count,
				 struct list_head *mnps)
{
	int optlen = (mnp_count * sizeof(struct ip6_mh_opt_mob_net_prefix) +
		      (mnp_count - 1) * sizeof(_pad4));
	struct list_head *l;
	int i = 0;
	uint8_t *data;
	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;

	if (iov->iov_base == NULL)
		return -ENOMEM;

	memset(iov->iov_base, 0, iov->iov_len);
	data = (uint8_t *)iov->iov_base;

	list_for_each(l, mnps) {
		struct prefix_list_entry *p;
		struct ip6_mh_opt_mob_net_prefix *mnp;

		p = list_entry(l, struct prefix_list_entry, list);
		mnp = (struct ip6_mh_opt_mob_net_prefix *)data;

		mnp->ip6mnp_type = IP6_MHOPT_MOB_NET_PRFX;
		mnp->ip6mnp_len = 18;
		mnp->ip6mnp_prefix_len = p->ple_plen;
		mnp->ip6mnp_prefix = p->ple_prefix;

		data += sizeof(struct ip6_mh_opt_mob_net_prefix);

		/* do internal padding here, so one iovec for MNPs is enough */
		if (++i < mnp_count) {
		  memcpy(data, _pad4, sizeof(_pad4));
		  data += sizeof(_pad4);
		}
	}
	return 0;
}

static size_t mh_length(struct iovec *vec, int count)
{
	size_t len = 0;
	int i;

	for (i = 0; i < count; i++) {
		len += vec[i].iov_len;
	}
	return len;
}

static inline int optpad(int xn, int y, int offset)
{
	return ((y - offset) & (xn - 1));
}

static int mh_try_pad(const struct iovec *in, struct iovec *out, int count)
{
	size_t len = 0;
	int m, n = 1, pad = 0;
	struct ip6_mh_opt *opt;

	out[0].iov_len = in[0].iov_len;
	out[0].iov_base = in[0].iov_base;
	len += in[0].iov_len;

	for (m = 1; m < count; m++) {
		opt = (struct ip6_mh_opt *)in[m].iov_base;
		switch (opt->ip6mhopt_type) {
		case IP6_MHOPT_BREFRESH:
			pad = optpad(2, 0, len); /* 2n */
			break;
		case IP6_MHOPT_ALTCOA:
			pad = optpad(8, 6, len); /* 8n+6 */
			break;
		case IP6_MHOPT_NONCEID:
			pad = optpad(2, 0, len); /* 2n */
			break;
		case IP6_MHOPT_BAUTH:
			pad = optpad(8, 2, len); /* 8n+2 */
			break;
		case IP6_MHOPT_MOB_NET_PRFX:
			pad = optpad(8, 4, len); /* 8n+4 */
			break;
		}
		if (pad > 0) {
			create_opt_pad(&out[n++], pad);
			len += pad;
		}
		len += in[m].iov_len;
		out[n].iov_len = in[m].iov_len;
		out[n].iov_base = in[m].iov_base;
		n++;
	}
	if (count == 1) {
		pad = optpad(8, 0, len);
		create_opt_pad(&out[n++], pad);
	}

	return n;
}

static int calculate_auth_data(const struct iovec *iov, int iovlen,
			       const struct in6_addr *coa,
			       const struct in6_addr *cn,
			       const uint8_t *key, uint8_t *digest)
{
	uint8_t buf[HMAC_SHA1_HASH_LEN];
	int i;

#ifdef HAVE_LIBCRYPTO
	unsigned int len = HMAC_SHA1_HASH_LEN;
	HMAC_CTX ctx;
	const EVP_MD *evp_md = EVP_sha1();

	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key, HMAC_SHA1_KEY_SIZE, evp_md, NULL);

	HMAC_Update(&ctx, (uint8_t *)coa, sizeof(*coa));
	HMAC_Update(&ctx, (uint8_t *)cn, sizeof(*coa));
	for (i = 0; i < iovlen; i++) {
		HMAC_Update(&ctx, (uint8_t *)iov[i].iov_base, iov[i].iov_len);
	}
	HMAC_Final(&ctx, buf, &len);
	HMAC_CTX_cleanup(&ctx);
#else
	HMAC_SHA1_CTX ctx;

	HMAC_SHA1_init(&ctx, key, HMAC_SHA1_KEY_SIZE);
	HMAC_SHA1_update(&ctx, (uint8_t *)coa, sizeof(*coa));
	HMAC_SHA1_update(&ctx, (uint8_t *)cn, sizeof(*coa));
	for (i = 0; i < iovlen; i++) {
		HMAC_SHA1_update(&ctx, (uint8_t *)iov[i].iov_base, 
				 iov[i].iov_len);
	}
	HMAC_SHA1_final(&ctx, buf);
#endif
	memcpy(digest, buf, MIPV6_DIGEST_LEN);
	return 0;
}

int mh_verify_auth_data(const void *msg, int len, const void *opt,
			const struct in6_addr *coa,
			const struct in6_addr *cn,
			const uint8_t *key)
{
	struct ip6_mh_opt_auth_data *bauth;
	struct iovec iov;
	uint8_t adata[MIPV6_DIGEST_LEN];

	bauth = (struct ip6_mh_opt_auth_data *)opt;
	iov.iov_base = (void *)msg;
	iov.iov_len = len - bauth->ip6moad_len;
	calculate_auth_data(&iov, 1, coa, cn, key, adata);
	if (memcmp(adata, bauth->ip6moad_data, bauth->ip6moad_len)) {
		/* binding auth data error */
		return -1;
	}
	return 0;
}

/**
 * mh_send - send mobility header message
 * @addrs: bundle of addresses
 * @mh_vec: scatter/gather array
 * @iovlen: array block count
 * @bind_key: key for calculating binding auth. data
 *
 * Sends a mobility header message to @dst with @src source address.
 * Mobility header is created from the @mh_vec vector array created by
 * the caller and initialized with mh_create() and mh_create_opt_*()
 * calls.  Padding is done automatically and mobility header length is
 * set.  Binding authorization data is calculated if present.  Returns
 * number of bytes sent on success, otherwise negative error value.
 **/
int mh_send(const struct in6_addr_bundle *addrs, const struct iovec *mh_vec,
	    int iovlen, const uint8_t *bind_key, int oif)
{
	struct ip6_mh_opt_auth_data lbad;
	struct sockaddr_in6 daddr;
	
	struct iovec iov[2*(IP6_MHOPT_MAX+1)];
	struct msghdr msg;
	struct cmsghdr *cmsg;
	int cmsglen;
	struct in6_pktinfo pinfo;
	int ret = 0, on = 1;
	struct ip6_mh *mh;
	int iov_count;
	socklen_t rthlen = 0;

	iov_count = mh_try_pad(mh_vec, iov, iovlen);
	mh = (struct ip6_mh *)iov[0].iov_base;
	mh->ip6mh_hdrlen = (mh_length(iov, iov_count) >> 3) - 1;

	/*
	 * We use MH out policy for all address. Then we should update it
	 * to refresh its bundle in kernel to be used with correct
	 * route, IPsec SA and neighbor cache entry for the destination.
	 * IKE daemon does the same thing for rekeying process.
	 */
        if (xfrm_cn_policy_mh_out_touch(1) < 0) {
                MDBG("MH out policy touch failed: BA for "
                     "%x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(addrs->dst));
        }

	MDBG("sending MH type %d\n"
	     "from %x:%x:%x:%x:%x:%x:%x:%x\n"
	     "to %x:%x:%x:%x:%x:%x:%x:%x\n",
	     mh->ip6mh_type, NIP6ADDR(addrs->src), NIP6ADDR(addrs->dst));
	    
	if (addrs->local_coa)
		MDBG("local CoA %x:%x:%x:%x:%x:%x:%x:%x\n",
		     NIP6ADDR(addrs->local_coa));

	if (addrs->remote_coa)
		MDBG("remote CoA %x:%x:%x:%x:%x:%x:%x:%x\n",
		     NIP6ADDR(addrs->remote_coa));

	if (bind_key) {
		assert(iov_count > 1); 
		struct ip6_mh_opt_auth_data *bauth;
		struct iovec *biov;
		struct in6_addr *cn = NULL;
		MDBG("Adding bind auth data\n");
		if (mh->ip6mh_type == IP6_MH_TYPE_BU)
			cn = addrs->dst;
		else 
			cn = addrs->src;
		assert(addrs->bind_coa != NULL && cn != NULL); 
		biov = &iov[iov_count - 1];
		bauth = (struct ip6_mh_opt_auth_data *)biov->iov_base;
		
		if (bauth->ip6moad_type == IP6_MHOPT_BAUTH) {
			size_t orig_len = biov->iov_len;
			
			MDBG("Adding auth_data\n");
			memcpy(&lbad, bauth, sizeof(lbad));
			
			/* temporarily set iov_len to option header
			 * length for auth data calculation */
			biov->iov_len -= MIPV6_DIGEST_LEN;
			biov->iov_base = &lbad;
			calculate_auth_data(iov, iov_count, addrs->bind_coa,
					    cn, bind_key, lbad.ip6moad_data);
			biov->iov_len = orig_len;
		}
	}

	memset(&daddr, 0, sizeof(struct sockaddr_in6));
	daddr.sin6_family = AF_INET6;
	daddr.sin6_addr = *addrs->dst;
	daddr.sin6_port = htons(IPPROTO_MH);

	memset(&pinfo, 0, sizeof(pinfo));
	pinfo.ipi6_addr = *addrs->src;
	pinfo.ipi6_ifindex = oif;

	cmsglen = CMSG_SPACE(sizeof(pinfo));
	if (addrs->remote_coa != NULL) {
		rthlen = inet6_rth_space(IPV6_RTHDR_TYPE_2, 1);
		if (!rthlen) {
			MDBG("inet6_rth_space error\n");
			return -1;
		}
		cmsglen += CMSG_SPACE(rthlen);
	}
	cmsg = malloc(cmsglen);
	if (cmsg == NULL) {
		MDBG("malloc failed\n");
		return -ENOMEM;
	}
	memset(cmsg, 0, cmsglen);
	memset(&msg, 0, sizeof(msg));

	msg.msg_control = cmsg;
	msg.msg_controllen = cmsglen;
	msg.msg_iov = iov;
	msg.msg_iovlen = iov_count;
	msg.msg_name = (void *)&daddr;
	msg.msg_namelen = sizeof(daddr);
	
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_len = CMSG_LEN(sizeof(pinfo));
	cmsg->cmsg_level = IPPROTO_IPV6;
	cmsg->cmsg_type = IPV6_PKTINFO;
	memcpy(CMSG_DATA(cmsg), &pinfo, sizeof(pinfo));

	if (addrs->remote_coa != NULL) {
		void *rthp;

		cmsg = CMSG_NXTHDR(&msg, cmsg);
		if (cmsg == NULL) {
			free(msg.msg_control);
			MDBG("internal error\n");
			return -2;
		}
		cmsg->cmsg_len = CMSG_LEN(rthlen);
		cmsg->cmsg_level = IPPROTO_IPV6;
		cmsg->cmsg_type = IPV6_RTHDR;
		rthp = CMSG_DATA(cmsg);
		rthp = inet6_rth_init(rthp, rthlen, IPV6_RTHDR_TYPE_2, 1);
		if (rthp == NULL) {
			free(msg.msg_control);
			MDBG("inet6_rth_init error\n");
			return -3;
		}
		inet6_rth_add(rthp, addrs->remote_coa);
		rthp = NULL;
	}

	pthread_mutex_lock(&mh_sock.send_mutex);
	setsockopt(mh_sock.fd, IPPROTO_IPV6, IPV6_PKTINFO,
		   &on, sizeof(int));
	ret = sendmsg(mh_sock.fd, &msg, 0);
	if (ret < 0)
		dbg("sendmsg: %s\n", strerror(errno));

	pthread_mutex_unlock(&mh_sock.send_mutex);

	free(msg.msg_control);

	statistics_inc(&mipl_stat, MIPL_STATISTICS_OUT_MH);
	return ret;
}

static int mh_opt_len_chk(uint8_t type, int len)
{
	switch (type) {
	case IP6_MHOPT_BREFRESH:
		return len != sizeof(struct ip6_mh_opt_refresh_advice);
	case IP6_MHOPT_ALTCOA:
		return len != sizeof(struct ip6_mh_opt_altcoa);
	case IP6_MHOPT_NONCEID:
		return len != sizeof(struct ip6_mh_opt_nonce_index);
	case IP6_MHOPT_BAUTH:
		return len != sizeof(struct ip6_mh_opt_auth_data);
	case IP6_MHOPT_MOB_NET_PRFX:
		return len != sizeof(struct ip6_mh_opt_mob_net_prefix);
	case IP6_MHOPT_PADN:
	default:
		return 0;
	}	
}

		
/**
 * mh_opt_parse - parse mobility options
 * @mh_opts: pointer to mh_options structure
 * @opts: pointer to beginning of options
 * @optlen: total length of options in bytes
 *
 * Parses mobility options from a buffer @opts.  Allocates an array of
 * ip6_mh_opt pointers pointing to beginnings of mobility options.  Array
 * is indexed with ip6mhopt_type values.  %IP6_MHOPT_PAD1 and
 * %IP6_MHOPT_PADN are ignored.  Returns number of options parsed
 * (excluding pad) on success, otherwise negative error value.
 **/
int mh_opt_parse(const struct ip6_mh *mh, ssize_t len, ssize_t offset,
		 struct mh_options *mh_opts)
{
	const uint8_t *opts = (uint8_t *) mh;
	ssize_t left = len - offset;
	ssize_t i = offset;
	int ret = 0;
	int bauth = 0;

	memset(mh_opts, 0, sizeof(*mh_opts));
	while (left > 0) {
		struct ip6_mh_opt *op = (struct ip6_mh_opt *)&opts[i];
		/* make sure the binding authorization data is last */
		if (bauth)
			return -EINVAL;

		if (op->ip6mhopt_type == IP6_MHOPT_PAD1) {
			left--;
			i++;
			continue;
		} 
		if ((size_t)left < sizeof(struct ip6_mh_opt) ||
		    mh_opt_len_chk(op->ip6mhopt_type, op->ip6mhopt_len + 2)) {
			syslog(LOG_ERR,
			       "Kernel failed to catch malformed Mobility"
			       "Option type %d. Update kernel!",
			       op->ip6mhopt_type);
			return -EINVAL;
		}
		if (op->ip6mhopt_type == IP6_MHOPT_PADN) {
			left -= op->ip6mhopt_len + 2;
			i += op->ip6mhopt_len + 2;
			continue;
		}
		if (op->ip6mhopt_type <= IP6_MHOPT_MAX) {
			if (op->ip6mhopt_type == IP6_MHOPT_BAUTH)
				bauth = 1;

			if (!mh_opts->opts[op->ip6mhopt_type])
				mh_opts->opts[op->ip6mhopt_type] = i;
			else if (mh_opts_dup_ok[op->ip6mhopt_type])
				mh_opts->opts_end[op->ip6mhopt_type] = i;
			else
				return -EINVAL;
			ret++;
		}
		left -= op->ip6mhopt_len + 2;
		i += op->ip6mhopt_len + 2;
	}
	return ret;
}

#define CMSG_BUF_LEN 128

/**
 * mh_recv - receive mobility header signaling message
 * @msg: buffer to store message in
 * @addr: packet source address
 * @pkt_info: packet destination and interface
 * @haoaddr: address in home address option (if any)
 * @rtaddr: address in routing header type 2 (if any)
 *
 * Waits for a packet from mobility header @mh_sock.fd socket.
 * Stores information about the packet to @addr, @pkt_info, @hoa, and
 * @rtaddr.  Packet data (i.e. mobility header) is stored in @msg.
 * Returns length of packet data received, or negative error value on
 * failure.
 **/
ssize_t mh_recv(unsigned char *msg, size_t msglen,
		struct sockaddr_in6 *addr, struct in6_pktinfo *pkt_info,
		struct in6_addr *haoaddr, struct in6_addr *rtaddr)
{
	struct ip6_mh *mh;
	struct msghdr mhdr;
	struct cmsghdr *cmsg;
	struct iovec iov;
	static unsigned char chdr[CMSG_BUF_LEN];
	void *databufp = NULL;
	int sockfd = mh_sock.fd;
	socklen_t hao_len;
	ssize_t len;

	iov.iov_len = msglen;
	iov.iov_base = (unsigned char *)msg;

	mhdr.msg_name = (void *)addr;
	mhdr.msg_namelen = sizeof(struct sockaddr_in6);
	mhdr.msg_iov = &iov;
	mhdr.msg_iovlen = 1;
	mhdr.msg_control = (void *)&chdr;
	mhdr.msg_controllen = CMSG_BUF_LEN;

	if ((len = recvmsg(sockfd, &mhdr, 0)) < 0)
		return -errno;

	memset(haoaddr, 0, sizeof(*haoaddr));
	memset(rtaddr, 0, sizeof(*rtaddr));

	statistics_inc(&mipl_stat, MIPL_STATISTICS_IN_MH);

	for (cmsg = CMSG_FIRSTHDR(&mhdr); cmsg;
	     cmsg = CMSG_NXTHDR(&mhdr, cmsg)) {
		int ret = 0;

		if (cmsg->cmsg_level != IPPROTO_IPV6)
			continue;
		switch (cmsg->cmsg_type) {
		case IPV6_PKTINFO:
			memcpy(pkt_info, CMSG_DATA(cmsg), sizeof(*pkt_info));
			break;
		case IPV6_DSTOPTS:
			ret = inet6_opt_find(CMSG_DATA(cmsg), cmsg->cmsg_len,
					     0, IP6OPT_HOME_ADDRESS,
					     &hao_len, &databufp);
			if (ret >= 0 && databufp != NULL &&
			    hao_len == sizeof(struct in6_addr)) {
				*haoaddr = *(struct in6_addr *) databufp;
			}
			break;
		case IPV6_RTHDR:
			if (inet6_rth_gettype(CMSG_DATA(cmsg)) ==
			    IPV6_RTHDR_TYPE_2) {
				struct in6_addr *seg = NULL;
				/* Kernel already processed routing
				 * header type 2 for us */
				seg = inet6_rth_getaddr(CMSG_DATA(cmsg), 0);
				if (!seg)
					MDBG("Invalid rth\n");
				else
					*rtaddr = *seg;
				statistics_inc(&mipl_stat, MIPL_STATISTICS_IN_RH2);
			}
			break;
		}
	}
	mh = (struct ip6_mh *)msg;

	if (mh->ip6mh_type > IP6_MH_TYPE_MAX) {
		struct in6_addr *src, *dst, *hoa;
		if (!IN6_IS_ADDR_UNSPECIFIED(rtaddr))
			src = rtaddr;
		else
			src = &pkt_info->ipi6_addr;

		if (!IN6_IS_ADDR_UNSPECIFIED(haoaddr)) {
			hoa = &addr->sin6_addr;
			dst = haoaddr;
		} else {
			hoa = NULL;
			dst = &addr->sin6_addr;
		}
		mh_send_be(dst, hoa, src, IP6_MH_BES_UNKNOWN_MH,
			   pkt_info->ipi6_ifindex);
		return -EINVAL;
	}

	/* No need to perform any other validity checks, since kernel
	 * does this for us. */

	statistics_inc(&mipl_stat, MIPL_STATISTICS_IN_MH);
	return len;
}

void mh_send_be(struct in6_addr *dst, struct in6_addr *hoa, 
		struct in6_addr *src, uint8_t status, int iif)
{
	struct ip6_mh_binding_error *be;
	struct iovec iov;
	struct in6_addr_bundle out;

	if (IN6_IS_ADDR_UNSPECIFIED(dst) ||
	    IN6_IS_ADDR_LOOPBACK(dst) ||
	    IN6_IS_ADDR_MULTICAST(dst)) {
		MDBG("Omit BE for non-unicast "
		     "%x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(dst));
		return;
	}

	out.remote_coa = NULL;
	out.local_coa = NULL;
	be = mh_create(&iov, IP6_MH_TYPE_BERROR);
	if (!be)
		return;
	be->ip6mhbe_status = status;
	out.src = src;
	out.dst = dst;
	if (hoa)
		be->ip6mhbe_homeaddr = *hoa;

	mh_send(&out, &iov, 1, NULL, iif);
	free_iov_data(&iov, 1);
	statistics_inc(&mipl_stat, MIPL_STATISTICS_OUT_BE);
}

/* Check if binding has been used recently and send a binding refresh
 * request to get a new BU
 */
void mh_send_brr(struct in6_addr *mn_addr, struct in6_addr *local)
{
	struct iovec mh_vec;
	struct in6_addr_bundle addrs;
	struct timespec now;
	long last_used;

	clock_gettime(CLOCK_REALTIME, &now);

	last_used = xfrm_last_used(mn_addr, local, IPPROTO_ROUTING, &now);
	if (last_used < 0)
		return;
	if (last_used > CN_BRR_TIME_THRESH) {
		dbg("Binding is about to expire, no need to send a BRR\n");
		return;
	}
	/* Should we check for a BUL entry here? */
	dbg("Binding is about to expire, sending a BRR\n");
	addrs.src = local;
	addrs.dst = mn_addr;
	addrs.remote_coa = NULL;
	addrs.local_coa = NULL;
	
	if (!mh_create(&mh_vec, IP6_MH_TYPE_BRR))
		return;
	
	mh_send(&addrs, &mh_vec, 1, NULL, 0);
	free_iov_data(&mh_vec, 1);
	statistics_inc(&mipl_stat, MIPL_STATISTICS_OUT_BRR);
}

void mh_send_ba(const struct in6_addr_bundle *addrs, uint8_t status,
		uint8_t flags, uint16_t sequence, 
		const struct timespec *lifetime, const uint8_t *key, int iif)
{
	int iovlen = 1;
	struct ip6_mh_binding_ack *ba;
	struct iovec mh_vec[2];

	MDBG("status %d\n", status);

	ba = mh_create(mh_vec, IP6_MH_TYPE_BACK);
	if (!ba)
		return;

	ba->ip6mhba_status = status;
	ba->ip6mhba_flags = flags;
	ba->ip6mhba_seqno = htons(sequence);
	ba->ip6mhba_lifetime = htons(lifetime->tv_sec >> 2);

	if (status < IP6_MH_BAS_UNSPECIFIED && !conf.NonVolatileBindingCache) {
		struct timespec refresh;
		tsclear(refresh);
		if (conf.pmgr.use_bradv(addrs->dst, addrs->bind_coa,
					addrs->src, lifetime, &refresh) &&
		    tsbefore(*lifetime, refresh))
			mh_create_opt_refresh_advice(&mh_vec[iovlen++], 
						     refresh.tv_sec);
	}
	if (key)
		mh_create_opt_auth_data(&mh_vec[iovlen++]);
	mh_send(addrs, mh_vec, iovlen, key, iif);
	free_iov_data(mh_vec, iovlen);
	statistics_inc(&mipl_stat, MIPL_STATISTICS_OUT_BA);
}

/* Main BU parser, used by both HA (H flag set) and CN (H flag not set).
 * Additional specific checks are performed for HA and CN via two
 * specific helpers: ha_bu_check() (ha.c) and cn_bu_check() (cn.c). */
int mh_bu_parse(struct ip6_mh_binding_update *bu, ssize_t len,
		const struct in6_addr_bundle *in_addrs,
		struct in6_addr_bundle *out_addrs,
		struct mh_options *mh_opts,
		struct timespec *lifetime)
{
	struct in6_addr *our_addr, *peer_addr, *remote_coa;
	struct ip6_mh_opt_altcoa *alt_coa;

	MDBG("Binding Update Received\n");
	if (len < 0 || (size_t)len < sizeof(struct ip6_mh_binding_update) ||
	    mh_opt_parse(&bu->ip6mhbu_hdr, len,
			 sizeof(struct ip6_mh_binding_update), mh_opts) < 0)
		return -1;
			 
	peer_addr = in_addrs->src;
	if (!in6_is_addr_routable_unicast(peer_addr))
		return -1;

	remote_coa = in_addrs->remote_coa;
	if (remote_coa && !IN6_ARE_ADDR_EQUAL(remote_coa, peer_addr))
		out_addrs->remote_coa = remote_coa;
	else
		out_addrs->remote_coa = NULL;

	alt_coa = mh_opt(&bu->ip6mhbu_hdr, mh_opts, IP6_MHOPT_ALTCOA);
	if (alt_coa)
		out_addrs->bind_coa = &alt_coa->ip6moa_addr;
	else
		out_addrs->bind_coa = in_addrs->remote_coa;

	our_addr = in_addrs->dst;
	tsclear(*lifetime);
	if (out_addrs->bind_coa) {
		if (!in6_is_addr_routable_unicast(out_addrs->bind_coa))
			return -1;
		if (!IN6_ARE_ADDR_EQUAL(out_addrs->bind_coa, peer_addr)) {
			/* check that there is no circular reference */
			if (bce_exists(our_addr, out_addrs->bind_coa))
				return -1;
			tssetsec(*lifetime, ntohs(bu->ip6mhbu_lifetime) << 2);
		}
	}
	/* Use Home address of MN for calculating BU and BA auth data
	 * for deregs. */
	if (!out_addrs->bind_coa)
		out_addrs->bind_coa = in_addrs->src;

	out_addrs->src = in_addrs->dst;
	out_addrs->dst = in_addrs->src;
	out_addrs->local_coa = NULL;

	return 0;
}

void mh_cleanup(void)
{
	close(mh_sock.fd);
	pthread_cancel(mh_listener);
	pthread_join(mh_listener, NULL);
}

/* based on http://www.iana.org/assignments/mobility-parameters (last
 * updated 2008-10-10 version), but only for protocols we support, i.e.
 * MIPv6 and NEMO. Think about updating MAX_BA_STATUS_STR_LEN in header
 * file if needed. */
void mh_ba_status_to_str(int status, char *err_str)
{
	char *s;

	switch (status) {
	case IP6_MH_BAS_ACCEPTED:            s = "Binding Update accepted"; break;
	case IP6_MH_BAS_PRFX_DISCOV:         s = "Accepted but prefix discovery necessary" ; break;
	case IP6_MH_BAS_UNSPECIFIED:         s = "Reason unspecified"; break;
	case IP6_MH_BAS_PROHIBIT:            s = "Administratively prohibited"; break;
	case IP6_MH_BAS_INSUFFICIENT:        s = "Insufficient resources"; break;
	case IP6_MH_BAS_HA_NOT_SUPPORTED:    s = "Home registration not supported"; break;
	case IP6_MH_BAS_NOT_HOME_SUBNET:     s = "Not home subnet"; break;
	case IP6_MH_BAS_NOT_HA:              s = "Not home agent for this mobile node"; break;
	case IP6_MH_BAS_DAD_FAILED:          s = "Duplicate Address Detection failed"; break;
	case IP6_MH_BAS_SEQNO_BAD:           s = "Sequence number out of window"; break;
	case IP6_MH_BAS_HOME_NI_EXPIRED:     s = "Expired home nonce index"; break;
	case IP6_MH_BAS_COA_NI_EXPIRED:      s = "Expired care-of nonce index";	break;
	case IP6_MH_BAS_NI_EXPIRED:          s = "Expired nonces"; break;
	case IP6_MH_BAS_REG_NOT_ALLOWED:     s = "Registration type change disallowed";	break;
	case IP6_MH_BAS_MR_OP_NOT_PERMITTED: s = "Mobile Router Operation not permitted"; break;
	case IP6_MH_BAS_INVAL_PRFX:          s = "Invalid Prefix"; break;
	case IP6_MH_BAS_NOT_AUTH_FOR_PRFX:   s = "Not Authorized for Prefix"; break;
	case IP6_MH_BAS_FWDING_FAILED:       s = "Forwarding Setup failed"; break;
	default:                             s = "unknown by UMIP"; break;
	}

	memset(err_str, 0, MAX_BA_STATUS_STR_LEN);
	sprintf(err_str, "%s (%d)", s, status);
}

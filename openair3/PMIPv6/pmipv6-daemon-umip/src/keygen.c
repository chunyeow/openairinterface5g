/*
 * $Id: keygen.c 1.15 06/05/05 19:40:57+03:00 anttit@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 * 
 * Author: Antti Tuominen <anttit@tcs.hut.fi>
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
#include <inttypes.h>
#ifdef HAVE_LIBCRYPTO
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#else
#include "crypto.h"
#endif

#include <netinet/ip6.h>
#include <netinet/ip6mh.h>

#include "mipv6.h"
#include "tqueue.h"
#include "keygen.h"

/* MAX_NONCES controls how many nonces we circulate.  It must be a
 * power of two */
#define MAX_NONCES 16

static uint8_t key_cn[HMAC_SHA1_KEY_SIZE];

struct nonce_holder {
	struct tq_elem tqe;
	uint16_t nonce_index;
	uint8_t nonce[NONCE_LENGTH];
	struct timespec valid_until;
};

static struct nonce_holder nonces[MAX_NONCES];
static pthread_rwlock_t nonce_lock;

static struct nonce_count {
	uint16_t max_nce;
	uint16_t min_nce;
} count;


static void nonce_regen(struct tq_elem *tqe)
{
	struct nonce_holder *n = tq_data(tqe, struct nonce_holder, tqe);
	pthread_rwlock_wrlock(&nonce_lock);
	count.max_nce++;
	count.min_nce++;
	n->nonce_index += MAX_NONCES;
	tsclear(n->valid_until);
#ifdef HAVE_LIBCRYPTO
	RAND_bytes(n->nonce, NONCE_LENGTH);
#else
	random_bytes(n->nonce, NONCE_LENGTH);
#endif
	pthread_rwlock_unlock(&nonce_lock);
}

static int nonce_init(void)
{
	int i;

	if (pthread_rwlock_init(&nonce_lock, NULL))
		return -1;

	for (i = 1; i <= MAX_NONCES; i++) {
		struct nonce_holder *n;
		n = &nonces[i & (MAX_NONCES - 1)];
#ifdef HAVE_LIBCRYPTO
		if (!RAND_bytes(n->nonce, NONCE_LENGTH))
			return -1;
#else
		random_bytes(n->nonce, NONCE_LENGTH);
#endif
		tsclear(n->valid_until);
		n->nonce_index = i;
	}
	count.min_nce = 1;
	count.max_nce = MAX_NONCES;
	return 0;
}

/* called with held nonce_lock */
static struct nonce_holder *_get_nonce(uint16_t idx)
{
	struct nonce_holder *n;

	n = &nonces[idx & (MAX_NONCES - 1)];
	if (n->nonce_index == idx)
		return n;

	return NULL;
}

/* called with held nonce_lock */
static struct nonce_holder *_get_valid_nonce(void)
{
	struct nonce_holder *n = NULL;
	struct timespec now, remain;
	uint16_t c, min, max;

	min = count.min_nce;
	max = count.max_nce;

	clock_gettime(CLOCK_REALTIME, &now);

	for (c = min; c <= max; c++) {
		n = _get_nonce(c);
		if (!tsisset(n->valid_until)) {
			tsadd(now, MAX_NONCE_LIFETIME_TS, n->valid_until);
			add_task_abs(&n->valid_until, &n->tqe, nonce_regen);
		}
		tssub(n->valid_until, now, remain);
		if (remain.tv_sec >= MAX_TOKEN_LIFETIME)
			break;
		else
			n = NULL;
	}
	return n;
}

/* called with held nonce_lock */
static struct nonce_holder *_validate_nonce(uint16_t nidx)
{
	struct nonce_holder *n;
	struct timespec now, remain;

	n = _get_nonce(nidx);
	if (n == NULL)
		return NULL;

	clock_gettime(CLOCK_REALTIME, &now);

	tssub(n->valid_until, now, remain);
	if (remain.tv_sec < 0)
		return NULL;

	return n;
}

static void build_kgen_token(struct in6_addr *addr, uint8_t *nonce, 
			     uint8_t id, uint8_t *buf)
{
	uint8_t tmp[20];
#ifdef HAVE_LIBCRYPTO
	unsigned int len = 20;
	HMAC_CTX ctx;

	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key_cn, sizeof(key_cn), EVP_sha1(), NULL);
	HMAC_Update(&ctx, (unsigned char *)addr, sizeof(*addr));
	HMAC_Update(&ctx, nonce, NONCE_LENGTH);
	HMAC_Update(&ctx, &id, sizeof(id));
	HMAC_Final(&ctx, tmp, &len);
	HMAC_CTX_cleanup(&ctx);
#else
	HMAC_SHA1_CTX ctx;

	HMAC_SHA1_init(&ctx, key_cn, sizeof(key_cn));
	HMAC_SHA1_update(&ctx, (unsigned char *)addr, sizeof(*addr));
	HMAC_SHA1_update(&ctx, nonce, NONCE_LENGTH);
	HMAC_SHA1_update(&ctx, &id, sizeof(id));
	HMAC_SHA1_final(&ctx, tmp);
#endif
	memcpy(buf, tmp, 8);
}

/**
 * rr_cn_keygen_token - create keygen tokens
 * @addr: node address
 * @bit: 0 for home keygen token, 1 for care-of keygen token
 * @kgt: buffer to store token in
 *
 * Creates home or care-of keygen token.  Token is stored in kgt.
 * Returns the index of the nonce used for calculation.
 **/
uint16_t rr_cn_keygen_token(struct in6_addr *addr, uint8_t bit, uint8_t *kgt)
{
	struct nonce_holder *n;
	uint16_t ret;
	pthread_rwlock_rdlock(&nonce_lock);
	n = _get_valid_nonce();
	build_kgen_token(addr, n->nonce, bit, kgt);
	ret = n->nonce_index;
	pthread_rwlock_unlock(&nonce_lock);
	return ret;
}

/**
 * rr_cn_nonce_lft - get lifetime of a nonce
 * @index: nonce index
 * @lft: timespec to store lifetime in
 *
 * Retrieves lifetime of nonce specified by @index.  Returns 0 and sets @lft
 * if nonce has a valid lifetime,  else returns -1.
 **/
int rr_cn_nonce_lft(uint16_t index, struct timespec *lft)
{
	struct nonce_holder *n;
	int ret = -1;
	pthread_rwlock_rdlock(&nonce_lock);
	n = _get_nonce(index);
	if (n != NULL) {
		*lft = n->valid_until;
		ret = 0;
	}
	pthread_rwlock_unlock(&nonce_lock);
	return ret;
}

/** 
 * rr_mn_calc_Kbm - calculates the binding authorization key 
 * @keygen_hoa: home address of MN
 * @keygen_coa: care-of address of MN
 * @kbm: buffer for storing the key, must be at least 20 bytes
 *
 * Calculates BU authorization key.  Use NULL value for keygen_coa,
 * when deregistering at home.
 **/
void rr_mn_calc_Kbm(uint8_t *keygen_hoa, uint8_t *keygen_coa, uint8_t *kbm)
{
#ifdef HAVE_LIBCRYPTO
	SHA_CTX ctx;

	SHA1_Init(&ctx);
	SHA1_Update(&ctx, keygen_hoa, NONCE_LENGTH);
	if (keygen_coa)
		SHA1_Update(&ctx, keygen_coa, NONCE_LENGTH);

	SHA1_Final(kbm, &ctx);
#else
	SHA1_CTX ctx;

	SHA1_init(&ctx);
	SHA1_update(&ctx, keygen_hoa, NONCE_LENGTH);
	if (keygen_coa)
		SHA1_update(&ctx, keygen_coa, NONCE_LENGTH);

	SHA1_final(&ctx, kbm);
#endif
}

/** 
 * rr_cn_calc_Kbm - calculates the binding authorization key 
 * @home_nonce_ind: home nonce index
 * @coa_nonce_ind: care-of nonce index
 * @hoa: home address of MN
 * @coa: care-of address of MN
 * @kbm: buffer for storing the bu_key, must be at least 20 bytes
 *
 * Returns 0 on success and BA error code on error
 **/
int rr_cn_calc_Kbm(uint16_t home_nonce_ind, uint16_t coa_nonce_ind, 
		   struct in6_addr *hoa, struct in6_addr *coa, uint8_t *kbm)
{
	struct nonce_holder *home_nce = NULL, *careof_nce = NULL;
	uint8_t home_token[20], careof_token[20];
	int ret = 0;

	pthread_rwlock_rdlock(&nonce_lock);
	if ((home_nce = _validate_nonce(home_nonce_ind)) == NULL)
		ret = IP6_MH_BAS_HOME_NI_EXPIRED;
	if (coa && ((careof_nce = _validate_nonce(coa_nonce_ind)) == NULL)) {
		if (ret)
			ret = IP6_MH_BAS_NI_EXPIRED;
		else
			ret = IP6_MH_BAS_COA_NI_EXPIRED;
	}
	if (ret) {
		pthread_rwlock_unlock(&nonce_lock);
		return ret;
	}
	build_kgen_token(hoa, home_nce->nonce, 0, home_token);

	if (coa) {
		build_kgen_token(coa, careof_nce->nonce, 1, careof_token);
		rr_mn_calc_Kbm(home_token, careof_token, kbm);
	} else {
		rr_mn_calc_Kbm(home_token, NULL, kbm);
	}
	pthread_rwlock_unlock(&nonce_lock);
	return 0;
}

/**
 * rr_cn_init - initialize key_cn
 *
 * Initializes key_cn with a random value, and creates a buffer of
 * nonces.
 **/
int rr_cn_init(void)
{
#ifdef HAVE_LIBCRYPTO
	if (!RAND_bytes(key_cn, HMAC_SHA1_KEY_SIZE))
		return -1;
#else
	random_bytes(key_cn, HMAC_SHA1_KEY_SIZE);
#endif

	return nonce_init();
}

/* $Id: mh.h 1.43 06/05/07 21:52:43+03:00 anttit@tcs.hut.fi $ */

#ifndef __MH_H__
#define __MH_H__ 1

#include <netinet/in.h>
#include <netinet/ip6mh.h>
#include "pmip_consts.h"


#define MIP6_SEQ_GT(x,y) ((short int)(((uint16_t)(x)) - ((uint16_t)(y))) > 0)

/* If new types or options appear, these should be updated. */
//For PMIP
#define IP6_MH_TYPE_MAX IP6_MH_TYPE_PBRES
#define IP6_MHOPT_MAX IP6_MHOPT_PMIP_MAX

struct sock {
    pthread_mutex_t send_mutex;
    int fd;
};

struct in6_addr_bundle {
	struct in6_addr *src;
	struct in6_addr *dst;
	struct in6_addr *local_coa;
	struct in6_addr *remote_coa;
	struct in6_addr *bind_coa;
};

struct mh_options {
	ssize_t opts[IP6_MHOPT_MAX + 1];
	ssize_t opts_end[IP6_MHOPT_MAX + 1];
};

struct mh_handler {
	struct mh_handler *next;
	void (* recv)(const struct ip6_mh *mh, ssize_t len,
		      const struct in6_addr_bundle *in_addrs, int iif);
};

int mh_init(void);
void mh_cleanup(void);

int mh_send(const struct in6_addr_bundle *addrs, 
	    const struct iovec *mh_vec, int iovlen, 
	    const uint8_t *bind_key, int oif);

void mh_send_brr(struct in6_addr *mn_addr, struct in6_addr *local);

void mh_send_ba(const struct in6_addr_bundle *addrs, uint8_t status, 
		uint8_t flags, uint16_t sequence, 
		const struct timespec *lifetime,
		const uint8_t *key, int iif);

static inline void mh_send_ba_err(const struct in6_addr_bundle *addrs,
				  uint8_t status, uint8_t flags,
				  uint16_t seqno, const uint8_t *key, int iif)
{
	struct timespec zero = { 0, 0 };
	mh_send_ba(addrs, status, flags, seqno, &zero, key, iif);
}

void mh_send_be(struct in6_addr *dst,
		struct in6_addr *hoa,
		struct in6_addr *src,
		uint8_t status, int iif);

ssize_t mh_recv(unsigned char *msg, size_t msglen,
		struct sockaddr_in6 *addr, struct in6_pktinfo *pkt_info,
		struct in6_addr *hoa, struct in6_addr *rtaddr);

/* Mobility header and option creation functions */
void *mh_create(struct iovec *iov, uint8_t type);

int mh_create_opt_refresh_advice(struct iovec *iov, uint16_t interval);

int mh_create_opt_altcoa(struct iovec *iov, struct in6_addr *addr);

int mh_create_opt_nonce_index(struct iovec *iov, uint16_t home_nonce,
			      uint16_t coa_nonce);

int mh_create_opt_auth_data(struct iovec *iov);

struct list_head;

int mh_create_opt_mob_net_prefix(struct iovec *iov, int mnp_count,
				 struct list_head *mnps);

static inline void *mh_opt(const struct ip6_mh *mh,
			   const struct mh_options *mh_opts, uint8_t type)
{
	if (mh_opts->opts[type]) {
		uint8_t *data = (uint8_t *)mh;
		return &data[mh_opts->opts[type]];
	}
	return NULL;
}

static inline void *mh_opt_next(const struct ip6_mh *mh,
				const struct mh_options *mh_opts,
				const void *vopt)
{
	uint8_t *data = (uint8_t *)mh;
	uint8_t *opt = (uint8_t *)vopt;
	if (opt) {
		ssize_t last = mh_opts->opts_end[opt[0]];
		ssize_t i = opt - data;
		while (i < last) {
			if (data[i] == IP6_MHOPT_PAD1)
				i++;
			else
				i += data[i + 1] + 2;
			if (i <= last && data[i] == opt[0])
				return &data[i];
		}
	}
	return NULL;
}

/* Mobility option parse functions */
int mh_opt_parse(const struct ip6_mh *mh, ssize_t len, ssize_t offset,
		 struct mh_options *mh_opts);

int mh_verify_auth_data(const void *msg, int len, const void *opt,
			const struct in6_addr *coa,
			const struct in6_addr *cn,
			const uint8_t *key);

int mh_bu_parse(struct ip6_mh_binding_update *bu, ssize_t len,
		const struct in6_addr_bundle *in_addrs,
		struct in6_addr_bundle *out_addrs,
		struct mh_options *mh_opts,
		struct timespec *lifetime);

void mh_handler_reg(uint8_t type, struct mh_handler *handler);
void mh_handler_dereg(uint8_t type, struct mh_handler *handler);

/* Some debug helper for BA status */
#define MAX_BA_STATUS_STR_LEN 64
void mh_ba_status_to_str(int status, char *err_str);

#endif

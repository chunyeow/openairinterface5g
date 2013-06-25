/* $Id: util.h 1.54 06/02/22 16:21:55+02:00 anttit@tcs.hut.fi $ */

#ifndef __UTIL_H__
#define __UTIL_H__ 1

#define MAX_PKT_LEN 1540

#define TIME_SEC_MSEC	1000
#define TIME_SEC_NSEC 	1000000000
#define TIME_MSEC_NSEC	1000000

#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/uio.h>

#define tstomsec(tv) \
	((tv).tv_sec * TIME_SEC_MSEC + (tv).tv_nsec / TIME_MSEC_NSEC)

#define tstodsec(tv) \
	((double)(tv).tv_sec + (double)(tv).tv_nsec / TIME_SEC_NSEC)

#define tsisset(tv)	((tv).tv_sec || (tv).tv_nsec)
#define tsclear(tv)	((tv).tv_sec = (tv).tv_nsec = 0)

#define tscmp(a, b, CMP) \
(((a).tv_sec == (b).tv_sec) ? \
  ((a).tv_nsec CMP (b).tv_nsec) : \
  ((a).tv_sec CMP (b).tv_sec))

#define tsadd(a, b, result) \
do { \
	(result).tv_sec = (a).tv_sec + (b).tv_sec; \
	(result).tv_nsec = (a).tv_nsec + (b).tv_nsec; \
	if ((result).tv_nsec >= TIME_SEC_NSEC) { \
		++(result).tv_sec; \
		(result).tv_nsec -= TIME_SEC_NSEC; \
	} \
} while (0)

#define tssub(a, b, result) \
do { \
	(result).tv_sec = (a).tv_sec - (b).tv_sec; \
	(result).tv_nsec = (a).tv_nsec - (b).tv_nsec; \
	if ((result).tv_nsec < 0) { \
		--(result).tv_sec; \
		(result).tv_nsec += TIME_SEC_NSEC; \
	} \
} while (0)

#define tsafter(a, b) tscmp(a,b,<)
#define tsbefore(a, b) tscmp(a,b,>)

#define tscpy(to, from) \
do { \
	(to).tv_sec = (from).tv_sec; \
	(to).tv_nsec = (from).tv_nsec; \
} while (0)

#define tsset(tv, sec, nsec) \
do { \
	(tv).tv_sec = (sec); \
	(tv).tv_nsec = (nsec); \
} while (0)

#define tssetsec(tv, sec) \
do { \
	(tv).tv_sec = (sec); \
	(tv).tv_nsec = 0; \
} while (0)

#define tssetmsec(tv, msec) \
do { \
	(tv).tv_sec = (msec) / TIME_SEC_MSEC; \
	(tv).tv_nsec = ((msec) % TIME_SEC_MSEC) * TIME_MSEC_NSEC; \
} while (0)

#define tssetdsec(tv, sec) \
do { \
	(tv).tv_sec = (long)(sec); \
	(tv).tv_nsec = (long)(((sec)-(tv).tv_sec) * TIME_SEC_NSEC); \
} while (0)


#define tsinc(tv, sec, nsec) \
do { \
	(tv).tv_sec += (sec); \
	(tv).tv_nsec += (nsec); \
	if ((tv).tv_nsec >= TIME_SEC_NSEC) { \
		++(tv).tv_sec; \
		(tv).tv_nsec -= TIME_SEC_NSEC; \
	} \
} while (0)

#define tsincmsec(tv, msec) \
do { \
	(tv).tv_sec += (msec) / TIME_SEC_MSEC; \
	(tv).tv_nsec += ((msec) % TIME_SEC_MSEC) * TIME_MSEC_NSEC; \
	if ((tv).tv_nsec >= TIME_SEC_NSEC) { \
		++(tv).tv_sec; \
		(tv).tv_nsec -= TIME_SEC_NSEC; \
	} \
} while (0)

#define tsdec(tv, sec, nsec) \
do { \
	(tv).tv_sec -= sec; \
	(tv).tv_nsec -= nsec; \
	if ((tv).tv_nsec < 0) { \
		--(tv).tv_sec; \
		(tv).tv_nsec += TIME_SEC_NSEC; \
	} \
} while (0)

#define tsdecmsec(tv, msec) \
do { \
	(tv).tv_sec -= (msec) / TIME_SEC_MSEC; \
	(tv).tv_nsec -= ((msec) % TIME_SEC_MSEC) * TIME_MSEC_NSEC; \
	if ((tv).tv_nsec < 0) { \
		--(tv).tv_sec; \
		(tv).tv_nsec += TIME_SEC_NSEC; \
	} \
} while (0)

#define tsmin(a, b) tsbefore((a), (b)) ? (b) : (a)

#define tsmax(a, b) tsafter((a), (b)) ? (b) : (a)

extern const char loopback_dev_name[];

extern const struct in6_addr in6addr_all_nodes_mc; /* ff02::1 */
extern const struct in6_addr in6addr_all_routers_mc; /* ff02::2 */

/* Following 4 routines are taken from include/net/ipv6.h */

static inline void ipv6_addr_set(struct in6_addr *addr, 
				 uint32_t w1, uint32_t w2,
				 uint32_t w3, uint32_t w4)
{
	addr->s6_addr32[0] = w1;
	addr->s6_addr32[1] = w2;
	addr->s6_addr32[2] = w3;
	addr->s6_addr32[3] = w4;
}

static inline void ipv6_addr_solict_mult(const struct in6_addr *addr,
					 struct in6_addr *solicited)
{
	ipv6_addr_set(solicited, htonl(0xFF020000), 0, htonl(0x1),
		      htonl(0xFF000000) | addr->s6_addr32[3]);
}

static inline void ipv6_addr_llocal(const struct in6_addr *addr,
				    struct in6_addr *llocal)
{
	ipv6_addr_set(llocal, htonl(0xFE800000), 0,
		      addr->s6_addr32[2], addr->s6_addr32[3]);
}

static inline int in6_is_addr_routable_unicast(const struct in6_addr *a)
{
	return ((!IN6_IS_ADDR_UNSPECIFIED(a) &&
		 !IN6_IS_ADDR_LOOPBACK(a) &&
		 !IN6_IS_ADDR_MULTICAST(a) &&
		 !IN6_IS_ADDR_LINKLOCAL(a)));
}

#define NIP6ADDR(addr) \
        ntohs((addr)->s6_addr16[0]), \
        ntohs((addr)->s6_addr16[1]), \
        ntohs((addr)->s6_addr16[2]), \
        ntohs((addr)->s6_addr16[3]), \
        ntohs((addr)->s6_addr16[4]), \
        ntohs((addr)->s6_addr16[5]), \
        ntohs((addr)->s6_addr16[6]), \
        ntohs((addr)->s6_addr16[7])

/**
 * free_iov_data - free vector data
 * @iov: vector array
 * @count: number of elements in array
 *
 * Frees an array of iovec data, specified by @iov with @count
 * elements.  Does not free actual array, only iov_base.
 **/
static inline void free_iov_data(struct iovec *iov, int count)
{
	if (iov == NULL) return;
	while (count--) {
		if (iov[count].iov_base)
			free(iov[count].iov_base);
	}
}

static inline unsigned long umin(unsigned long a, unsigned long b)
{
	return (a < b) ? a : b;
}

static inline long min(long a, long b)
{
	return (a < b) ? a : b;
}

static inline long max(long a, long b)
{
	return (a > b) ? a : b;
}

uint16_t in6_cksum(const struct in6_addr *src, const struct in6_addr *dst,
		   const void *data, socklen_t datalen, uint8_t nh);

unsigned int csum_partial(const void *data, socklen_t datalen, unsigned int sum);

uint16_t csum_ipv6_magic(const struct in6_addr *saddr,
                                          const struct in6_addr *daddr,
                                          uint32_t len, unsigned short proto,
                                          uint32_t csum);
/*
 * XXX: These may be missing on kernel header because either kernel is not
 * ready or should be removed since kernel will never support it.
 */
#ifndef RTPROT_MIP
#define RTPROT_MIP	16
#endif

#endif /* __UTIL_H__ */

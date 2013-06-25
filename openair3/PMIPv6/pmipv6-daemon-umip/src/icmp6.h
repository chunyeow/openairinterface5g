/* $Id: icmp6.h 1.17 06/05/07 21:52:43+03:00 anttit@tcs.hut.fi $ */

#ifndef __ICMP6_H__
#define __ICMP6_H__ 1

#include <netinet/icmp6.h>

struct icmp6_handler {
	struct icmp6_handler *next;
	void (* recv)(const struct icmp6_hdr *ih, 
		      ssize_t len, 
		      const struct in6_addr *src,
		      const struct in6_addr *dst,
		      int iif,
		      int hoplimit);
};

#define ICMP6_MAIN_SOCK -1

int if_mc_group(int sock, int ifindex, const struct in6_addr *mc_addr,
		int cmd);

void icmp6_handler_reg(uint8_t type, struct icmp6_handler *handler);
void icmp6_handler_dereg(uint8_t type, struct icmp6_handler *handler);

int icmp6_init(void);
void icmp6_cleanup(void);

int icmp6_send(int oif, uint8_t hoplimit, const struct in6_addr *src,
	       const struct in6_addr *dst, struct iovec *datav, size_t iovlen);

ssize_t icmp6_recv(int sock, unsigned char *msg, size_t msglen,
		   struct sockaddr_in6 *addr, struct in6_pktinfo *pkt_info,
		   int *hoplimit);

void *icmp6_create(struct iovec *iov, uint8_t type, uint8_t code);

struct ip6_hdr;

int icmp6_parse_data(struct ip6_hdr *ip6h, unsigned int len, 
		     struct in6_addr **lhoa, struct in6_addr **rhoa);

#endif

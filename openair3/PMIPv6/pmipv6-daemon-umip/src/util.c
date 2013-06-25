/*
 * $Id: icmp6.c 1.42 06/05/06 15:15:47+03:00 anttit@tcs.hut.fi $
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

#include "util.h"

/* Adapted from RFC 1071 "C" Implementation Example */
uint16_t in6_cksum(const struct in6_addr *src, const struct in6_addr *dst,
		   const void *data, socklen_t datalen, uint8_t nh)
{
	struct _phdr {
		struct in6_addr src;
		struct in6_addr dst;
		uint32_t plen;
		uint8_t reserved[3];
		uint8_t nxt;
	} phdr;

	register unsigned long sum = 0;
	socklen_t count;
	uint16_t *addr;
	int i;

	/* Prepare pseudo header for csum */
	memset(&phdr, 0, sizeof(phdr));
	phdr.src = *src;
	phdr.dst = *dst;
	phdr.plen = htonl(datalen);
	phdr.nxt = nh;

	/* caller must make sure datalen is even */
	addr = (uint16_t *)&phdr;
	for (i = 0; i < 20; i++)
		sum += *addr++;

	count = datalen;
	addr = (uint16_t *)data;

        while (count > 1) {
		sum += *(addr++);
		count -= 2;
	}

	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return (uint16_t)~sum;
}

unsigned int csum_partial(const void *data, socklen_t datalen, unsigned int sumP)
{
    register unsigned long sum = sumP;
    uint16_t *addr;
    socklen_t count;
    int i;

    count = datalen;
    addr = (uint16_t *)data;

    while (count > 1) {
        sum += *(addr++);
        count -= 2;
    }

    /*while (sum >> 16)
        sum = (sum & 0xffff) + (sum >> 16);

    return (uint16_t)~sum;*/
    return sum;
}

uint16_t csum_fold(uint32_t csum)
{
	uint32_t sum = (uint32_t)csum;
        sum = (sum & 0xffff) + (sum >> 16);
        sum = (sum & 0xffff) + (sum >> 16);
        return (uint16_t)~sum;
}


uint16_t csum_ipv6_magic(const struct in6_addr *saddr,
                                          const struct in6_addr *daddr,
                                          uint32_t len, unsigned short proto,
                                          uint32_t csum)
{

        int carry;
        uint32_t ulen;
        uint32_t uproto;
        uint32_t sum = (uint32_t)csum;

        sum += (uint32_t)saddr->s6_addr32[0];
        carry = (sum < (uint32_t)saddr->s6_addr32[0]);
        sum += carry;

        sum += (uint32_t)saddr->s6_addr32[1];
        carry = (sum < (uint32_t)saddr->s6_addr32[1]);
        sum += carry;

        sum += (uint32_t)saddr->s6_addr32[2];
        carry = (sum < (uint32_t)saddr->s6_addr32[2]);
        sum += carry;

        sum += (uint32_t)saddr->s6_addr32[3];
        carry = (sum < (uint32_t)saddr->s6_addr32[3]);
        sum += carry;

        sum += (uint32_t)daddr->s6_addr32[0];
        carry = (sum < (uint32_t)daddr->s6_addr32[0]);
        sum += carry;

        sum += (uint32_t)daddr->s6_addr32[1];
        carry = (sum < (uint32_t)daddr->s6_addr32[1]);
        sum += carry;

        sum += (uint32_t)daddr->s6_addr32[2];
        carry = (sum < (uint32_t)daddr->s6_addr32[2]);
        sum += carry;

        sum += (uint32_t)daddr->s6_addr32[3];
        carry = (sum < (uint32_t)daddr->s6_addr32[3]);
        sum += carry;

        ulen = (uint32_t)htonl((uint32_t) len);
        sum += ulen;
        carry = (sum < ulen);
        sum += carry;

        uproto = (uint32_t)htonl(proto);
        sum += uproto;
        carry = (sum < uproto);
        sum += carry;

        return csum_fold((uint32_t)sum);
}

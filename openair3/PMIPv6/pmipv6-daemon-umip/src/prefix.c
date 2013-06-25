/*
 * $Id: prefix.c 1.9 06/05/07 21:52:43+03:00 anttit@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 * 
 * Author: Ville Nuorvala <vnuorval@tcs.hut.fi>
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

#include "prefix.h"
#include "util.h"

struct prefix_list_entry *prefix_list_get(const struct list_head *pl, 
					  const struct in6_addr *addr, 
					  int plen)
{
	struct list_head *l;
	struct prefix_list_entry *p1 = NULL;
	list_for_each(l, pl) {
		struct prefix_list_entry *p2;
		p2 = list_entry(l, struct prefix_list_entry, list);
		if (!ipv6_pfx_cmp(&p2->ple_prefix, addr, p2->ple_plen)) {
			if (plen > 0) {
				if (p2->ple_plen == plen) {
					p1 = p2;
					break;
				}
			} else if (p1 == NULL || p1->ple_plen < p2->ple_plen)
				/* get the longest matching prefix */
				p1 = p2;
		}
	}
	return p1;
}

static inline int prefix_list_len(const struct list_head *pl)
{
	struct list_head *l;
	int res = 0;
	list_for_each(l, pl)
		res++;
	return res;
}

int prefix_list_cmp(const struct list_head *pl1, const struct list_head *pl2)
{
	struct list_head *l1, *l2;

	/* If lists lengths differ, no need to go further */
	if (prefix_list_len(pl1) != prefix_list_len(pl2))
		return 0;

	list_for_each(l1, pl1) {
		int match = 0;
		struct prefix_list_entry *p1;
		p1 = list_entry(l1, struct prefix_list_entry, list);

		list_for_each(l2, pl2) {
			struct prefix_list_entry *p2;
			p2 = list_entry(l2, struct prefix_list_entry, list);

			if (p1->ple_plen != p2->ple_plen ||
			    ipv6_pfx_cmp(&p1->ple_prefix, 
					 &p2->ple_prefix, p1->ple_plen))
				continue;
			match = 1;
			break;
		}

		if (!match)
			return 0;
	}
	return 1;
}

int prefix_list_copy(const struct list_head *pl1, struct list_head *pl2)
{
	struct list_head *l;
	int res = 0;
	list_for_each(l, pl1) {
		struct prefix_list_entry *p1, *p2;
		p1 = list_entry(l, struct prefix_list_entry, list);
		p2 = malloc(sizeof(struct prefix_list_entry));
		if (p2 == NULL) {
			prefix_list_free(pl2);
			return -1;
		}
		memcpy(p2, p1, sizeof(struct prefix_list_entry));
		list_add_tail(&p2->list, pl2);
		res++;
	}
	return res;
}

unsigned long mpd_curr_lft(const struct timespec *now,
			   const struct timespec *tstamp,
			   unsigned long lft)
{
		struct timespec tmp;
		unsigned long diff;

		if (prefix_lft_infinite(lft))
			return lft;

		tssub(*now, *tstamp, tmp);
		diff = tmp.tv_sec;

		if (tmp.tv_nsec > 0)
			diff++;

		if (lft > diff) 
			return lft - diff;
		return 0;
}

static const struct in6_addr dhaad_gen_suffix = { { { 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe } } };
static const struct in6_addr dhaad_eui64_suffix = { { { 0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0, 0xfd,0xff,0xff,0xff,0xff,0xff,0xff,0xfe } } };

void dhaad_gen_ha_anycast(struct in6_addr *anycast,
			  const struct in6_addr *pfx, int plen)
{
	ipv6_addr_create(anycast, 
			 pfx, 
			 plen == 64 ? &dhaad_eui64_suffix : &dhaad_gen_suffix,
			 plen);
}
